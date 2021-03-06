#ifndef MOAMSAH_
#define MOAMSAH_

#include <time.h>

struct tm * prevNow, *currNow;

int pdebug;
int maxAlignmentsNumber;
int Epsilons;
char * outputfilename;
FILE * outfile;
int partitionSize;
enum ModeEnum {Sequential, Distributed};
enum AlignmentTypeEnum {Global, Local};
enum AlignmentTypeEnum AlignmentType;
enum ModeEnum Mode;
int processArguments (int argc, char ** argv, long * seqNum, char * * * sequences, long * * seqLen, int * stype);
void * sfillTensor (void * threadarg);
void * pfillTensor (void * threadarg);
int readInput (char * fileName, char * * sequence);
void printSeq (char * sequence , int sq_sz);
int subScore (char char1, char char2, int stype);
int gapScore(int stype);
void PrintASeq (long seqNum, char * * sequences, long * seqLen, char * * * * algnseq, long * aseqLen, int alignmentsNo);
long getRelation (long * cell, long * neighbor, long dimn, long * decremented);
int getNeighborScores (char * * sequences, long * m_index, MOA_rec * msaAlgn, int stype, long * LNCount, long * * lnScores, long * * lnIndices, long * * * pwScores, long * * neighbor, long * * decremented, MOA_rec * NghbMOA);
int PrintPrevChains (MOA_rec *  msaAlgn);
int getPrevCells(long findex, long score, long LNCount, long * lnScores, long * lnIndices, MOA_rec *  msaAlgn);
long getScore (long findex, char * * sequences, long * m_index, MOA_rec * msaAlgn, int stype, long * LNCount, long * * lnScores, long * * lnIndices, long * * * pwScores, long * * neighbor, long * * decremented, MOA_rec * NghbMOA );
void initTensor ( MOA_rec * msaAlgn, int stype);
void * sfillTensor (void * threadarg);
void * pfillTensor (void * threadarg);
void fillTensor (char * * sequences, MOA_rec * msaAlgn, int stype);
int traceBack_one (long seqNum, char * * sequences, long * seqLen, MOA_rec * msaAlgn, int stype, char * * * algnseq);
void traceBack (long seqNum, char * * sequences, long * seqLen, MOA_rec * msaAlgn, int stype, char * * * * algnseq, long * * aSeqLen, int * alignmentsNo, long * currentCell, long * currentScore, int Prev_index);
void traceBack_loc (long seqNum, char * * sequences, long * seqLen, MOA_rec * msaAlgn, int stype, char * * * * algnseq, long * * aSeqLen, int * alignmentsNo, long * currentCell, long * currentScore, int Prev_index);
#endif
