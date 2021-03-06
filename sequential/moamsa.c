#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>      /* include the character library */
#include <pthread.h>
#include <errno.h>
#include "moa.h"
#include "scores.h"
#include "utils.h"
#include "moamsa.h"

const char GAPCHAR = '-';
struct thread_data{
  char * * sequences;
  MOA_rec * msaAlgn;
  int stype;
}; 


int processArguments (int argc, char ** argv, long * seqNum, char * * * sequences, long * * seqLen, int * stype) {
  int argRead = 1;
  long i;
  /* Initialize Arguments default Values*/
  (*seqNum) = 0;
  Epsilons = 0;
  (*stype) = 1;
  AlignmentType = Global;
  maxAlignmentsNumber = 20;
  outputfilename = "moamsa.out";
  pdebug = 0;
  partitionSize = 1;
 
  /* read & process MOAMSA Arguments */
  while (argRead < argc) {
    /*1. Read SeqNum and Sequences File Names */
    if (strcmp(argv[argRead],"-c") == 0) {
      /*printf ("\nread -c");       */
      (*seqNum) = atoi(argv[++argRead]);
      if ((*seqNum) <= 1) {
	printf("Sequence Num must be more than 1\n");
	return -1;
      }
      if (argc  <= ((*seqNum) + 3))  {
	printf("More Arguments expected\n");
	return -1;
      }
      (*sequences) = (char * *) mcalloc ((*seqNum), sizeof(char *));
      (*seqLen) = (long *)  mcalloc ((*seqNum), sizeof(long));
      for (i=0;i<(*seqNum);i++) {	
	(*seqLen)[i] =readInput(argv[++argRead], &(*sequences)[i]);
	if ((*seqLen)[i] == -1) {
	  printf("Sequence %ld File Could not open. Exiting.\n", i);
	  return -1;
	}
	(*seqLen)[i] ++;

	/* printSeq (sequences[i], seqLen[i]); */
      }

    /*2. Read Scoring Type */
    }
    else if (strcmp(argv[argRead], "-s") == 0) {      
      if (argc  <= argRead)  {
	printf("More Arguments expected\n");
	return -1;
      }
      if (argRead <argc) 
      (*stype) = atoi(argv[++argRead]);
	else {
      	printf("Expected Numeric Argument after -s for Scoring Type required.\n");
      	return -1;
      } 
    }
    /* 3. Decide if Local or Global Alignment */
    else if (strcmp(argv[argRead], "-l") == 0) {
      AlignmentType = Local;
    }
      else if (strcmp(argv[argRead],"-g") == 0) {
	AlignmentType = Global;
      }
    /* 4. Know Maximum Alignments Required to be returned */
    else if (strcmp(argv[argRead],"-m") == 0) {
      if (argc  < argRead)  {
	printf("More Arguments expected\n");
	return -1;
      }
      if (argRead <argc) 
      	maxAlignmentsNumber = atoi(argv[++argRead]);
      else {
      	printf("Expected Numeric Argument after -m for maximum Alignemnts required.\n");
      	return -1;
      } 
    }

    /* 5. Decide the Epsilons Value for search space reduction */
    else if (strcmp(argv[argRead],"-e") == 0) {
      if (argc  < argRead)  {
	printf("More Arguments expected\n");
	return -1;
      }
      if (argRead <argc) 
	Epsilons= atoi(argv[++argRead]);
 	else {
      	printf("Expected Numeric Argument after -e for Epsilons Value.\n");
      	return -1;
      } 
   }

    /* 6. Decide the output filename */
    else if (strcmp(argv[argRead], "-o") == 0) {
     
      if (argc  <= argRead)  {
	printf("More Arguments expected\n");
	return -1;
      }
      if (argRead <argc) 
      outputfilename = argv[++argRead];
 	else {
      	printf("Expected Numeric Argument after -o for outputfilename.\n");
      	return -1;
      } 
    }

    /* 7. Decide the debug Level */
    else if (strcmp(argv[argRead], "-d") == 0) {
      if (argc  < argRead)  {
	printf("More Arguments expected\n");
	return -1;
      }
      if (argRead <argc) 
	pdebug = atoi(argv[++argRead]);
      else {
      	printf("Expected Numeric Argument after -d for debug level required.\n");
      	return -1;
      } 
    }
    /* 8. Decide the partition size - for the distributed version */
    else if (strcmp(argv[argRead], "-p") == 0) {
      if (argc  < argRead)  {
	printf("More Arguments expected\n");
	return -1;
      }
      if (argRead <argc) 
	partitionSize = atoi(argv[++argRead]);
      else {
      	printf("Expected Numeric Argument after -p for partitions Size.\n");
      	return -1;
      } 
    }
    argRead ++;
  }

  if ((*seqNum) <= 1) {
    printf("Sequence Num must be more than 1\n");
    return -1;
  }
  /*open once to erase previous contents then close */
  if (( outfile= fopen (outputfilename, "w")) == NULL) {
    printf("Can not Open output file, exiting.\n");
    return -1;
  }
  fclose(outfile);
  mprintf(outputfilename, "Program Arguments: debuglevel = %d maxAlignmentsNumber = %d Epsilons= %d Alignment Type = %d stype %d outputfilename = %s partitionSize = %d\n", 7, pdebug, maxAlignmentsNumber, Epsilons, AlignmentType, (*stype), outputfilename, partitionSize);
}

int readInput (char * fileName, char * * sequence) {
  char elm;
  int i = 0;
  FILE * f = fopen (fileName, "r");
  if (f == NULL) {
    mprintf(outputfilename, "file could not be open!\n", 0);
    return -1;
  }
  while (!feof(f)) {
    elm = fgetc(f);
    if (elm != EOF)  {
      i++;
      if ((*sequence) == NULL)  {
	(*sequence) = (char *) mmalloc (sizeof(char));
      }
      else
	(*sequence) = (char *) realloc ((*sequence), sizeof(char) * i);
      if ((*sequence) == NULL) {
	mprintf(outputfilename, "Could not allocate memory for sequence!\n", 0);
	fclose(f);
	return -1;
      }
      else {
	(*sequence)[i-1] = elm;
      }
    }
  }
  fclose(f);
  return i;
}

void printSeq (char * sequence , int sq_sz){
  int i;
  mprintf (outputfilename, "\n> ", 0);
  for (i=0; i< sq_sz-1;i++)
    mprintf(outputfilename, "%c ", 1, sequence[i]);
}

int subScore (char char1, char char2, int stype) {
  int score = 0;
  if (stype == 1) { /* linear score */
    if (char1 == char2)
      score = 3;
    else if ((char1 == GAPCHAR) || (char2 == GAPCHAR))
      score = -6;
    else
      score = -2;
  }
  else if (stype == 2) /* PAM250 if protein */
    score  = PAM250 (char1, char2);
  else if (stype == 3) /* BLOSUM if protein */
    score  = BLOSUM (char1, char2);
  return score;
}

int gapScore(int stype) {
  int score = 0;
  score = subScore ('A', GAPCHAR, stype);
  return score;
}

void PrintASeq (long seqNum, char * * sequences, long * seqLen, char * * * * algnseq, long * aseqLen, int alignmentsNo) {
  long i, j, k = 0;
  mprintf(outputfilename, "\n Sequences Read: ", 0);
  for (i=0;i<seqNum;i++) {
    printSeq (sequences[i], seqLen[i]);
  }
  mprintf(outputfilename, "\n Found %d Optimal Alignments: \n", 1, alignmentsNo);
  for (k=0;k<alignmentsNo;k++) {
    mprintf(outputfilename, "\n Aligned Sequences %d: ", 1, k+1);
    for (i=0;i<seqNum;i++) {
      mprintf (outputfilename, "\n> ", 0);
      for (j=0;j<aseqLen[k];j++) 
	mprintf(outputfilename, "%c ", 1, (*algnseq)[k][i][j]);
    }
  }
}

/* Function to return the neighbors that got decremented , and those that didmn't*/

long getRelation (long * cell, long * neighbor, long dimn, long * decremented) {
  long i, cnt, ndecr = 0;
  cnt = 0;
  ndecr = 0;
  
  for (i = 0; i< dimn; i++) {
    if (neighbor[i] < cell[i]) {
      decremented[cnt] = i;
      cnt ++;
    }
    else
      ndecr ++;
  }
  return ndecr;
}

/* Function to examine all temporary neighbors to determine the current score of the new cell*/

int getNeighborScores (char * * sequences, long * m_index, MOA_rec * msaAlgn, int stype, long * LNCount, long * * lnScores, long * * lnIndices, long * * * pwScores, long * * neighbor, long * * decremented, MOA_rec * NghbMOA) {

  long i, j, k, l, ndecr = 0;
  long totpwiseScore = 0;

  if (pdebug == 3)
    mprintf(outputfilename, "Pairwise scores matrix: ", 0);
  totpwiseScore = 0;
      for (l=0;l<msaAlgn->dimn  - 1; l++) {
	for (k=l+1;k<msaAlgn->dimn; k++) {
	  /*for (l=1; l<msaAlgn->dimn; l++) {
	  //for (k=0; k<l; k++) {*/
      (*pwScores)[l][k] = subScore(sequences[l][m_index[l]-1], sequences[k][m_index[k]-1], stype);
      totpwiseScore = totpwiseScore + (*pwScores)[l][k];
       if (pdebug == 3)
	mprintf(outputfilename, "%d, %d, %c %c %d ",5, l, k,  sequences[l][m_index[l]-1], sequences[k][m_index[k]-1], pwScores[l][k]);
    }
     if (pdebug == 3)
      mprintf(outputfilename, "\n tpw %d ", 1, totpwiseScore);
  }

      /*get neighbors of the current cell */
  if (MOAGetLowerNeighbors (m_index, msaAlgn, NghbMOA) == -1) {
    return -1;
  }
  /*printMOA(NghbMOA); */
  totpwiseScore = 0;

  /* loop throught neighbors */
  for (i=0;i<NghbMOA->elements_ub - 1;i++) {
    /*mprintf(outputfilename, "nghb %d containts %d and index %d\n", 3, i, NghbMOA->elements[i].val, NghbMOA->indexes[i]);
  
    // get relation between this neighbor and the current cell
*/
    Gamma_Inverse(NghbMOA->indexes[i], msaAlgn->shape, msaAlgn->dimn, (*neighbor));
    ndecr = getRelation (m_index, (*neighbor), msaAlgn->dimn, (*decremented));
    /* if we have 2 or more decremented neighbors*/
    /* sum pairwise scores for each pair of dimensions that got decremented*/
    totpwiseScore = 0;
    if ((msaAlgn->dimn  - ndecr) > 1) {
      if (pdebug == 3) {
	mprintf(outputfilename, "\ndecr %d ndecr %d  decremented Neighb ", 2, (msaAlgn->dimn  - ndecr), ndecr);
	for (j = 0; j < (msaAlgn->dimn  - ndecr);j++) {
	  mprintf(outputfilename, " %d ", 1, decremented[j]);
	}
      }

      for (j=0;j<(msaAlgn->dimn  - ndecr) - 1; j++) {
	for (k=j+1;k<(msaAlgn->dimn  - ndecr); k++) {
	  totpwiseScore += (*pwScores)[(*decremented)[j]][(*decremented)[k]];
	  if (pdebug == 3) 
	    mprintf(outputfilename, "\n pw[%d][%d] = %d seq1 %c seq2 %c ", 5, (*decremented)[j], (*decremented)[k], (*pwScores)[(*decremented)[j]][(*decremented)[k]], sequences[j][m_index[j]-1], sequences[k][m_index[k]-1]);
	}
      }
    }    
    /* multiple the number of dimensions that did not get decremented by the gap score
    // ndecr = ndecr * gapScore(stype);      
    // add both to the previoues score in the neighbor's cell in the alignment matrix to get the temporary score for this neighbor*/
    if (pdebug == 3) 
      mprintf(outputfilename, "\n", 0);
      (*lnScores)[i] = msaAlgn->elements[NghbMOA->indexes[i]].val + totpwiseScore + (ndecr * gapScore(stype));
      (*lnIndices)[i] = NghbMOA->indexes[i];
    if (pdebug == 3) 
      mprintf(outputfilename, " lnScores[%d] %d totpwiseScore %d gapscore %d ",4, i, (*lnScores)[i], totpwiseScore, ndecr * gapScore(stype));
  }
  /* take the maximum temporary score as the score for the current cell*/
  (*LNCount) =  NghbMOA->elements_ub - 1;
  return 0;
}

int PrintPrevChains (MOA_rec *  msaAlgn) {
  long i, j = 0;
 
  for (i = 0; i< msaAlgn->elements_ub; i++)  {
    mprintf (outputfilename, "\nelm %d with score %d has %d previous: ", 3, i, msaAlgn->elements[i].val, msaAlgn->elements[i].prev_ub);
   for (j = 0; j< msaAlgn->elements[i].prev_ub; j++)  {
     mprintf (outputfilename, " %d ", 1,  msaAlgn->elements[i].prev[j]);
   }
  }
  return 0;
}

int getPrevCells(long findex, long score, long LNCount, long * lnScores, long * lnIndices, MOA_rec *  msaAlgn) {
  int i = 0;
  msaAlgn->elements[findex].prev_ub = 0;
  for (i = 0; i < LNCount; i++) {
    if (lnScores[i] == score) {
      if ( msaAlgn->elements[findex].prev_ub == 0) {
	msaAlgn->elements[findex].prev = (signed long *) mmalloc (sizeof(signed long));
      }
      else {
	msaAlgn->elements[findex].prev  = (signed long *) realloc (msaAlgn->elements[findex].prev, sizeof(signed long) * (msaAlgn->elements[findex].prev_ub + 1));
     if (msaAlgn->elements[findex].prev == NULL) {
		mprintf(outputfilename, "Could not reallocate memory for MSA Align Previous Cells pointer!\n", 0);
		return -1;
      }
      }
      msaAlgn->elements[findex].prev[msaAlgn->elements[findex].prev_ub] = lnIndices[i];
      msaAlgn->elements[findex].prev_ub ++;      
      if (pdebug == 4) 
	mprintf (outputfilename, "\nelm = %d prev_ub = %d lnIndices[%d] = %d\n", 3, findex, msaAlgn->elements[findex].prev_ub, i, lnIndices[i] );
    }
  }
  return 0;
}

long getScore (long findex, char * * sequences, long * m_index, MOA_rec * msaAlgn, int stype, long * LNCount, long * * lnScores, long * * lnIndices, long * * * pwScores, long * * neighbor, long * * decremented, MOA_rec * NghbMOA ) {

  long score, i = 0;

  getNeighborScores (sequences, m_index, msaAlgn, stype, LNCount, lnScores, lnIndices, pwScores, neighbor, decremented, NghbMOA);

  score = a_max((*lnScores), (*LNCount));
  if (AlignmentType == Local) {
    if (score <0)
      score = 0;
  }
  if ((*LNCount) > 0)
    msaAlgn->elements[findex].val = score;
  if (pdebug == 4) {
    for (i = 0; i < (*LNCount); i++)
      mprintf(outputfilename, "lnScores[%d], %d ", 2, i, (*lnScores)[i]);
    for (i = 0; i < (*LNCount); i++)
      mprintf(outputfilename, "lnIndices[%d], %d ", 2, i, (*lnIndices)[i]);
    mprintf(outputfilename, "score %d  \n", 1, score);
  }
  i = getPrevCells(findex, score, (*LNCount), (*lnScores), (*lnIndices), msaAlgn);
  return score;
}

void initTensor ( MOA_rec * msaAlgn, int stype) {
  long i, j = 0;
  long * m_index = NULL;
  int borderCell = 0;

  m_index = (long *) mcalloc  (msaAlgn->dimn,  sizeof(long));
  for (i=0;i<msaAlgn->elements_ub;i++) {
    Gamma_Inverse(i, msaAlgn->shape, msaAlgn->dimn, m_index);
    if (pdebug == 2)
      mprintf(outputfilename, "m_index: ", 0);
    for (j=0;j<msaAlgn->dimn;j++){
      if (pdebug == 2)
	mprintf(outputfilename, "%d ", 1, m_index[j]);
      if (m_index[j] == 0) {
	borderCell = 1;
      }
      m_index[j] = m_index[j] + 1;
    }
    if (borderCell ==1){
      msaAlgn->elements[i].val = (gapScore(stype) * Tau(m_index, msaAlgn->dimn));
    }
    borderCell = 0;
    if (pdebug == 2)
      mprintf(outputfilename, " index %d contains %d tau = %d score = %d\n", 4, i, msaAlgn->elements[i].val, Tau(m_index, msaAlgn->dimn), (gapScore(stype) * Tau(m_index, msaAlgn->dimn)));
  }
  //msaAlgn->elements[0].val = 0;
  if (m_index != NULL)
    free(m_index);
}
void * sfillTensor (void * threadarg) {
  char * * sequences;
  MOA_rec * msaAlgn; 
  int stype;
  struct thread_data * tdata;

  tdata = (struct thread_data *) threadarg;

  sequences = tdata->sequences;
  msaAlgn = tdata->msaAlgn;
  stype = tdata->stype;

  return NULL;
}


void * pfillTensor (void * threadarg) {
  long i, j, score, borderCell = 0;
  long * m_index = NULL; /* multidimensional index*/
  long LNCount, CalLnCount;
  long * lnScores = NULL; /* Lower neighbor temporary scores*/
  long * lnIndices = NULL; /* Lower neighbor Indices */
  long * * pwScores = NULL; /* holds the scores of pairwise match / mistmatch scores*/
  MOA_rec * NghbMOA = NULL;
  long * neighbor = NULL; /* multidimensional index of the current neighbor*/
  long * decremented = NULL; /* list of decremented indices in the multidimensional index of the current neighbor*/

  char * * sequences;
  MOA_rec * msaAlgn; 
  int stype;
  struct thread_data * tdata;

  tdata = (struct thread_data *) threadarg;

  sequences = tdata->sequences;
  msaAlgn = tdata->msaAlgn;
  stype = tdata->stype;

  CalLnCount = (long) mpow(2, msaAlgn->dimn) - 1;
  neighbor = (long *)  mcalloc ((msaAlgn->dimn), sizeof(long));
  decremented = (long *)  mcalloc ((msaAlgn->dimn), sizeof(long));
  /*NghbMOA = (MOA_rec *) mmalloc(sizeof(MOA_rec));*/
  createMOAStruct (&NghbMOA);

  lnScores = (long *) mcalloc (CalLnCount, sizeof(long));
  lnIndices = (long *) mcalloc (CalLnCount, sizeof(long));

  /*get pairwise scores of the corresponding residues of the current cell in the alignment tensor*/
  /* Compute pairwise scores for the current index*/
  pwScores = (long * *) mcalloc (msaAlgn->dimn, sizeof(long *));
  for (i=0; i<msaAlgn->dimn; i++)
    pwScores[i] = (long *) mcalloc (msaAlgn->dimn, sizeof(long));

  m_index = (long *)  mcalloc ((msaAlgn->dimn), sizeof(long));
  /* loop the MOAAlign tensor for scores*/
  for (i = 0; i< msaAlgn->elements_ub; i++)  {
    currNow = getTime();
    if (((currNow->tm_yday * 1440) + (currNow->tm_hour * 60) + currNow->tm_min) > ((prevNow->tm_yday * 1440) + (prevNow->tm_hour * 60) + prevNow->tm_min + 10)) {
      printf("10 minutes elapsed and doing cell = %ld from total %ld \n", i, msaAlgn->elements_ub);
      prevNow->tm_hour = currNow->tm_hour;
      prevNow->tm_isdst = currNow->tm_isdst;
      prevNow->tm_mday = currNow->tm_mday;
      prevNow->tm_min = currNow->tm_min;
      prevNow->tm_mon = currNow->tm_mon;
      prevNow->tm_sec = currNow->tm_sec;
      prevNow->tm_wday = currNow->tm_wday;
      prevNow->tm_yday = currNow->tm_yday;
      prevNow->tm_year = currNow->tm_year;
    }
    Gamma_Inverse(i, msaAlgn->shape, msaAlgn->dimn, m_index);
    borderCell = 0;
    LNCount = 0;
    for (j = 0; j< msaAlgn->dimn; j++) {
      if (m_index[j] == 0)
	borderCell = 1;
    }
    if (pdebug == 4) {
      mprintf(outputfilename, "cell %d in_score %d md index:", 2, i, msaAlgn->elements[i].val);
      for (j = 0; j< msaAlgn->dimn; j++) {
	mprintf(outputfilename, " %d ", 1,  m_index[j]);
      }
      mprintf(outputfilename, "\n", 0);
    }
    if (borderCell == 0) {
      score = getScore(i, sequences, m_index, msaAlgn, stype, &LNCount,  &lnScores, &lnIndices, &pwScores, &neighbor, &decremented, NghbMOA);
      if (LNCount > 0)
	msaAlgn->elements[i].val = score;
      
    }
    if (pdebug == 4)
      mprintf(outputfilename, " LNCount %d score %d \n", 2, LNCount, msaAlgn->elements[i].val);
  }
  if (m_index != NULL)
    free(m_index);
  if (lnScores != NULL)
    free(lnScores);
  if (lnIndices != NULL)
    free(lnIndices);
 
  deleteMOA (NghbMOA);
  if (neighbor != NULL)
    free(neighbor);
  if (decremented != NULL)
    free(decremented);
  if (pwScores != NULL) {
    for (i=0; i<msaAlgn->dimn; i++) {
      if (pwScores[i] != NULL) 
        free( pwScores[i]);
    }
    free(pwScores);
  }
  return NULL;
}

void fillTensor (char * * sequences, MOA_rec * msaAlgn, int stype) {
  pthread_attr_t attr;
  pthread_t pt_id, st_id;    /* main and secondary threads ids and attributes */
  struct thread_data * tdata = NULL;
  int ret;
  void * status;

  tdata = (struct thread_data *) mmalloc (sizeof(struct thread_data));
  pthread_attr_init(&attr);
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
 
  tdata->sequences = sequences;
  tdata->msaAlgn=msaAlgn;
  tdata->stype = stype;
  /* Start Primary Thread*/
  if (pthread_create(&pt_id, &attr, pfillTensor, (void *) tdata)){
    printf("Cannot Create Primary thread\n");
    exit(1);
  }
  /* Start Secondary Thread*/
  if (pthread_create(&st_id, &attr, sfillTensor, (void *) tdata)) {
    printf(" Cannot Create Secondary thread\n");
    exit(1);
  }
  if (ret = pthread_join(pt_id, &status)){
    printf("Cannot Join Primary Thread\n");
    exit(1);
  }
  if (ret = pthread_join(st_id,  &status)) {
    printf("Cannot Join Secondary Thread\n");
    exit(1);
  }
  

  if (tdata != NULL) 
    free(tdata);
  
}


int traceBack_one (long seqNum, char * * sequences, long * seqLen, MOA_rec * msaAlgn, int stype, char * * * algnseq) {
  
 
  long currentScore, currentCell, i, j, aSeqLen = 0;
  long prevScore, prevCell = 0;
  long * m_index = NULL; /* multidimensional index*/
  long * n_index = NULL; /* multidimensional index of new cell*/
  char temp;

  aSeqLen = 0;
  m_index = (long *)  mcalloc ((msaAlgn->dimn), sizeof(long));
  n_index = (long *)  mcalloc ((msaAlgn->dimn), sizeof(long));
  /* Get Max Cell on Last Border as Current Cell*/
  currentScore = getMaxOnLastBorder (msaAlgn, &currentCell);
  Gamma_Inverse(currentCell, msaAlgn->shape, msaAlgn->dimn, n_index);
  for (i=0;i<seqNum;i++) 
    m_index[i] = n_index[i];
  /* Iterate making the Neighbor Cell that Computed the Current Cell, the new current cell till the origin is reached*/
  while (currentCell > 0) {
    prevScore = currentScore; 
    prevCell = currentCell; 
    if ((msaAlgn->elements[prevCell].prev_ub > 0) && (msaAlgn->elements[prevCell].prev != NULL)) {
      currentCell = msaAlgn->elements[prevCell].prev[0];
      currentScore = msaAlgn->elements[currentCell].val;
    }
    else {
      currentCell = 0;
      currentScore = msaAlgn->elements[currentCell].val;
    }
     if (pdebug == 6) 
	mprintf(outputfilename, "\n prevCell = %d prevScore = %d currentCell = %d currentScore = %d is %c  ", 4, prevCell, prevScore, currentCell, currentScore );
     fflush(stdout);
    Gamma_Inverse(currentCell, msaAlgn->shape, msaAlgn->dimn, n_index);
    for (i=0;i<seqNum;i++) {
      if (aSeqLen == 0)
	(*algnseq)[i] = (char *) mmalloc (sizeof(char) * (aSeqLen + 1));
      else
	(*algnseq)[i] = (char *) realloc ((*algnseq)[i], sizeof(char) * (aSeqLen+1));
     
      if ((*algnseq)[i] == NULL ) {
	mprintf(outputfilename, "Can not allocate memory to Alligned Sequence %d\n", 1, i);
	return -1;
      }
      if ((m_index[i] > n_index[i])) /* && (isalpha( sequences[i][m_index[i] - 1]))) */
	(*algnseq)[i][aSeqLen] = sequences[i][m_index[i] - 1];
      else
	(*algnseq)[i][aSeqLen] = GAPCHAR;
      if (pdebug == 6) 
	mprintf(outputfilename, "\n m_index[%d] = %d n_index = %d is %c  ", 4, i,  m_index[i], n_index[i],(*algnseq)[i][aSeqLen] );
      m_index[i] = n_index[i];
    }
    aSeqLen ++;
    }

  if (pdebug == 6) 
    mprintf(outputfilename, "\n aSeqLen %d ", 1, aSeqLen);  
     fflush(stdout);
     
     /* Reverse the contents of the Aligned Sequences*/
  for (i=0;i<seqNum;i++) {
    for (j=0;j<aSeqLen/2;j++) {
      temp = (*algnseq)[i][j];
      (*algnseq)[i][j] = (*algnseq)[i][aSeqLen - j - 1];
      (*algnseq)[i][aSeqLen - j - 1] = temp;
    }
    }
  if (n_index != NULL) 
    free(n_index);
  if (m_index != NULL) 
    free(m_index);
  return aSeqLen;
}
void traceBack (long seqNum, char * * sequences, long * seqLen, MOA_rec * msaAlgn, int stype, char * * * * algnseq, long * * aSeqLen, int * alignmentsNo, long * currentCell, long * currentScore, int Prev_index) {
  
 
  long i, j, k=0;
  long prevScore, prevCell=0;
  long * m_index = NULL; /* multidimensional index*/
  long * n_index = NULL; /* multidimensional index of new cell*/
  char temp;
  int myAlign = (*alignmentsNo);
  int myPrevInd = Prev_index;

  m_index = (long *)  mcalloc ((msaAlgn->dimn), sizeof(long));
  n_index = (long *)  mcalloc ((msaAlgn->dimn), sizeof(long));

  Gamma_Inverse((*currentCell), msaAlgn->shape, msaAlgn->dimn, n_index);
  for (i=0;i<seqNum;i++) 
    m_index[i] = n_index[i];
  
  /* Iterate making the Neighbor Cell that Computed the Current Cell, the new current cell till the origin is reached*/
  while ((*currentCell) > 0) {
    prevScore = (*currentScore); 
    prevCell = (*currentCell); 
    if (myPrevInd != 0) {
      (*currentCell) = msaAlgn->elements[prevCell].prev[myPrevInd];
      (*currentScore) = msaAlgn->elements[(*currentCell)].val;
      myPrevInd = 0;
    }
    else if ((((*alignmentsNo) + 1) < maxAlignmentsNumber) && (msaAlgn->elements[prevCell].prev_ub > 1) && (msaAlgn->elements[prevCell].prev != NULL)) {
      for (i=1;i<msaAlgn->elements[prevCell].prev_ub;i++) {
	(*alignmentsNo) ++; 
	(*algnseq) = (char * * *) realloc ((*algnseq), ((*alignmentsNo) + 1) * sizeof(char * *));
     if ((*algnseq) == NULL) {
		mprintf(outputfilename, "Could not reallocate memory for New Aligned Sequences Set %d!\n", 1, alignmentsNo+1);
		return;
      }
     (*algnseq)[(*alignmentsNo)] = (char * *) mmalloc (seqNum * sizeof(char *));
	(*aSeqLen) = (long *)  realloc ((*aSeqLen), ((*alignmentsNo) + 1) * sizeof(long));
     if ((*aSeqLen) == NULL) {
		mprintf(outputfilename, "Could not reallocate memory for New Aligned Sequences Length %d!\n", 1, alignmentsNo+1);
		return;
      }
	(*aSeqLen)[(*alignmentsNo)] = (*aSeqLen)[myAlign];
	if (pdebug == 6) 
	  mprintf(outputfilename, "\n copying %d: ", 1, (*aSeqLen)[myAlign]);
	for (j=0;j<seqNum;j++) {
	  (*algnseq)[(*alignmentsNo)][j] = (char *) mmalloc (sizeof(char) * ((*aSeqLen)[myAlign] + 1));
	  for (k=0;k<(*aSeqLen)[myAlign];k++) {
	    (*algnseq)[(*alignmentsNo)][j][k] = (*algnseq)[myAlign][j][k];
	    if (pdebug == 6) 
	      mprintf(outputfilename, "%c ", 1, (*algnseq)[myAlign][j][k]);
	  }
	  mprintf (outputfilename, "\n", 0);
	}
	if (pdebug == 6) 
	  mprintf(outputfilename, "\n myAlign = %d : (*alignmentsNo) = %d, prev_ub: %d prevCell = %d prevScore = %d currentCell = %d currentScore = %d ", 6, myAlign, (*alignmentsNo), msaAlgn->elements[prevCell].prev_ub, prevCell, prevScore, (*currentCell), (*currentScore)  );

	traceBack (seqNum, sequences, seqLen, msaAlgn, stype, algnseq, aSeqLen, alignmentsNo, currentCell, currentScore, i);
      } 
      (*currentCell) = msaAlgn->elements[prevCell].prev[0];
      (*currentScore) = msaAlgn->elements[(*currentCell)].val;
      if (pdebug == 6) 
	mprintf(outputfilename, "\n myAlign = %d : (*alignmentsNo) = %d, prev_ub: %d prevCell = %d prevScore = %d currentCell = %d currentScore = %d ", 6, myAlign, (*alignmentsNo), msaAlgn->elements[prevCell].prev_ub, prevCell, prevScore, (*currentCell), (*currentScore)  );
    }
    else if ((msaAlgn->elements[prevCell].prev_ub > 0) && (msaAlgn->elements[prevCell].prev != NULL)) {
      (*currentCell) = msaAlgn->elements[prevCell].prev[0];
      (*currentScore) = msaAlgn->elements[(*currentCell)].val;
    }
    else {
      (*currentCell) = 0;
      (*currentScore) = msaAlgn->elements[(*currentCell)].val;
    }
    if (pdebug == 6) 
      mprintf(outputfilename, "\n myAlign = %d : prev_ub: %d prevCell = %d prevScore = %d currentCell = %d currentScore = %d ", 6, myAlign, msaAlgn->elements[prevCell].prev_ub, prevCell, prevScore, (*currentCell), (*currentScore)  );
    fflush(stdout);
    Gamma_Inverse((*currentCell), msaAlgn->shape, msaAlgn->dimn, n_index);
    for (i=0;i<seqNum;i++) {
      if ((*aSeqLen)[myAlign] == 0) {
	(*algnseq)[myAlign][i] = (char *) mmalloc (sizeof(char) * ((*aSeqLen)[myAlign] + 1));
      }
      else {
	(*algnseq)[myAlign][i] = (char *) realloc ((*algnseq)[myAlign][i], sizeof(char) * (((*aSeqLen)[myAlign])+1));
     if ((*algnseq)[myAlign][i] == NULL) {
		mprintf(outputfilename, "Could not reallocate memory for %d Aligned %d Sequence at Character %d!\n", 3, myAlign, i+1, (((*aSeqLen)[myAlign])+1));
		return;
      }
	
      }
      if ((*algnseq)[myAlign][i] == NULL ) {
	mprintf(outputfilename, "Can not allocate memory to Alligned Sequence %d\n", 1, i);
	return;
      }
      if ((m_index[i] > n_index[i])) /* && (isalpha( sequences[i][m_index[i] - 1]))) */
	(*algnseq)[myAlign][i][(*aSeqLen)[myAlign]] = sequences[i][m_index[i] - 1];
      else
	(*algnseq)[myAlign][i][(*aSeqLen)[myAlign]] = GAPCHAR;
      if (pdebug == 6) 
	mprintf(outputfilename, "\n aSeqLen: %d:  m_index[%d] = %d n_index = %d is %c  ", 5 , (*aSeqLen)[myAlign], i,  m_index[i], n_index[i],(*algnseq)[myAlign][i][(*aSeqLen)[myAlign]] );
      m_index[i] = n_index[i];
    }
    (*aSeqLen)[myAlign] ++;
  }   
  /* Reverse the contents of the Aligned Sequences*/
  for (i=0;i<seqNum;i++) {
    for (j=0;j<(*aSeqLen)[myAlign]/2;j++) {
      temp = (*algnseq)[myAlign][i][j];
      (*algnseq)[myAlign][i][j] = (*algnseq)[myAlign][i][(*aSeqLen)[myAlign] - j - 1];
      (*algnseq)[myAlign][i][(*aSeqLen)[myAlign] - j - 1] = temp;
    }
  }
  if (n_index != NULL) 
    free(n_index);
  if (m_index != NULL) 
    free(m_index);
}
void traceBack_loc (long seqNum, char * * sequences, long * seqLen, MOA_rec * msaAlgn, int stype, char * * * * algnseq, long * * aSeqLen, int * alignmentsNo, long * currentCell, long * currentScore, int Prev_index) {
  
 
  long i, j, k=0;
  long prevScore, prevCell=0;
  long * m_index = NULL; /* multidimensional index*/
  long * n_index = NULL; /* multidimensional index of new cell*/
  char temp;
  int myAlign = (*alignmentsNo);
  int myPrevInd = Prev_index;

  m_index = (long *)  mcalloc ((msaAlgn->dimn), sizeof(long));
  n_index = (long *)  mcalloc ((msaAlgn->dimn), sizeof(long));

  Gamma_Inverse((*currentCell), msaAlgn->shape, msaAlgn->dimn, n_index);
  for (i=0;i<seqNum;i++) 
    m_index[i] = n_index[i];
  
  /* Iterate making the Neighbor Cell that Computed the Current Cell, the new current cell till the origin is reached*/
  while ((*currentScore) > 0) {
    prevScore = (*currentScore); 
    prevCell = (*currentCell); 
    if (myPrevInd != 0) {
      (*currentCell) = msaAlgn->elements[prevCell].prev[myPrevInd];
      (*currentScore) = msaAlgn->elements[(*currentCell)].val;
      myPrevInd = 0;
    }
    else if ((((*alignmentsNo) + 1) < maxAlignmentsNumber) && (msaAlgn->elements[prevCell].prev_ub > 1) && (msaAlgn->elements[prevCell].prev != NULL)) {
      for (i=1;i<msaAlgn->elements[prevCell].prev_ub;i++) {
	(*alignmentsNo) ++; 
	(*algnseq) = (char * * *) realloc ((*algnseq), ((*alignmentsNo) + 1) * sizeof(char * *));
     if ((*algnseq) == NULL) {
		mprintf(outputfilename, "Could not reallocate memory for Aligned Sequence Set Pointer %d!\n", 1, (*alignmentsNo) + 1);
		return;
      }
	(*algnseq)[(*alignmentsNo)] = (char * *) mmalloc (seqNum * sizeof(char *));
	(*aSeqLen) = (long *)  realloc ((*aSeqLen), ((*alignmentsNo) + 1) * sizeof(long));
     if ((*aSeqLen) == NULL) {
		mprintf(outputfilename, "Could not reallocate memory for Aligned Sequence Length %d!\n", 1, (*alignmentsNo) + 1);
		return;
      }
	(*aSeqLen)[(*alignmentsNo)] = (*aSeqLen)[myAlign];
	if (pdebug == 6) 
	  mprintf(outputfilename, "\n copying %d: ", 1, (*aSeqLen)[myAlign]);
	for (j=0;j<seqNum;j++) {
	  (*algnseq)[(*alignmentsNo)][j] = (char *) mmalloc (sizeof(char) * ((*aSeqLen)[myAlign] + 1));
	  for (k=0;k<(*aSeqLen)[myAlign];k++) {
	    (*algnseq)[(*alignmentsNo)][j][k] = (*algnseq)[myAlign][j][k];
	    if (pdebug == 6) 
	      mprintf(outputfilename, "%c ", 1, (*algnseq)[myAlign][j][k]);
	  }
	  mprintf (outputfilename, "\n", 0);
	}
	if (pdebug == 6) 
	  mprintf(outputfilename, "\n myAlign = %d : (*alignmentsNo) = %d, prev_ub: %d prevCell = %d prevScore = %d currentCell = %d currentScore = %d ", 6, myAlign, (*alignmentsNo), msaAlgn->elements[prevCell].prev_ub, prevCell, prevScore, (*currentCell), (*currentScore)  );

	traceBack (seqNum, sequences, seqLen, msaAlgn, stype, algnseq, aSeqLen, alignmentsNo, currentCell, currentScore, i);
      } 
      (*currentCell) = msaAlgn->elements[prevCell].prev[0];
      (*currentScore) = msaAlgn->elements[(*currentCell)].val;
      if (pdebug == 6) 
	mprintf(outputfilename, "\n myAlign = %d : (*alignmentsNo) = %d, prev_ub: %d prevCell = %d prevScore = %d currentCell = %d currentScore = %d ", 6, myAlign, (*alignmentsNo), msaAlgn->elements[prevCell].prev_ub, prevCell, prevScore, (*currentCell), (*currentScore)  );
    }
    else if ((msaAlgn->elements[prevCell].prev_ub > 0) && (msaAlgn->elements[prevCell].prev != NULL)) {
      (*currentCell) = msaAlgn->elements[prevCell].prev[0];
      (*currentScore) = msaAlgn->elements[(*currentCell)].val;
    }
    else {
      (*currentCell) = 0;
      (*currentScore) = msaAlgn->elements[(*currentCell)].val;
    }
    if (pdebug == 6) 
      mprintf(outputfilename, "\n myAlign = %d : prev_ub: %d prevCell = %d prevScore = %d currentCell = %d currentScore = %d ", 6, myAlign, msaAlgn->elements[prevCell].prev_ub, prevCell, prevScore, (*currentCell), (*currentScore)  );
    Gamma_Inverse((*currentCell), msaAlgn->shape, msaAlgn->dimn, n_index);
    for (i=0;i<seqNum;i++) {
      if ((*aSeqLen)[myAlign] == 0) {
	(*algnseq)[myAlign][i] = (char *) mmalloc (sizeof(char) * ((*aSeqLen)[myAlign] + 1));
      }
      else {
	(*algnseq)[myAlign][i] = (char *) realloc ((*algnseq)[myAlign][i], sizeof(char) * (((*aSeqLen)[myAlign])+1));
     if ((*algnseq)[myAlign][i] == NULL) {
		mprintf(outputfilename, "Could not reallocate memory for %d Aligned %d Sequence at Character %d!\n", 3, myAlign, i+1, (((*aSeqLen)[myAlign])+1));
		return;
      }
	
      }
      if ((*algnseq)[myAlign][i] == NULL ) {
	mprintf(outputfilename, "Can not allocate memory to Alligned Sequence %d\n", 1, i);
	return;
      }
      if ((m_index[i] > n_index[i])) /* && (isalpha( sequences[i][m_index[i] - 1]))) */ 
	(*algnseq)[myAlign][i][(*aSeqLen)[myAlign]] = sequences[i][m_index[i] - 1];
      else
	(*algnseq)[myAlign][i][(*aSeqLen)[myAlign]] = GAPCHAR;
      if (pdebug == 6) 
	mprintf(outputfilename, "\n aSeqLen: %d:  m_index[%d] = %d n_index = %d is %c  ", 5 , (*aSeqLen)[myAlign], i,  m_index[i], n_index[i],(*algnseq)[myAlign][i][(*aSeqLen)[myAlign]] );
      m_index[i] = n_index[i];
    }
    (*aSeqLen)[myAlign] ++;
  }   
  /* Reverse the contents of the Aligned Sequences */
  for (i=0;i<seqNum;i++) {
    for (j=0;j<(*aSeqLen)[myAlign]/2;j++) {
      temp = (*algnseq)[myAlign][i][j];
      (*algnseq)[myAlign][i][j] = (*algnseq)[myAlign][i][(*aSeqLen)[myAlign] - j - 1];
      (*algnseq)[myAlign][i][(*aSeqLen)[myAlign] - j - 1] = temp;
    }
  }
  if (n_index != NULL) 
    free(n_index);
  if (m_index != NULL) 
    free(m_index);
}

