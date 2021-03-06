#include <stdio.h>
#include <stdlib.h>
/*#include <limits.h>*/
#include "moa.h"
#include "moamsa.h"
#include "utils.h"
#include "lq.h"

void createMOAStruct(MOA_rec * * MOA_val) {
  (*MOA_val) = (MOA_rec *) mmalloc(sizeof(MOA_rec));
  (*MOA_val)->dimn = 1;
  (*MOA_val)->shape = NULL;
  (*MOA_val)->indexes = NULL;
  (*MOA_val)->proc = NULL;
  (*MOA_val)->dproc_ub = NULL;
  (*MOA_val)->dproc = NULL;
  (*MOA_val)->elements = NULL;
}

void createMOA(long * shape, long dimn, MOA_rec * MOA_val, int callFlag, int cellValue)
{
  long i;
  
  MOA_val->elements_ub = Tau(shape, dimn);
  if (MOA_val->elements_ub == 0) {
    mprintf(outputfilename, "The smallest type of MOA array is a scalar that requires at least one element! NO MOA type supports zero element list!\n", 0);
    return;
  }
  MOA_val->dimn = dimn;
  if (dimn == 0) {
    MOA_val->shape = (long *) mcalloc (1, sizeof(long));
    MOA_val->shape[0] = 1;
  }
  else {
    MOA_val->shape = (long *) mcalloc(MOA_val->dimn, sizeof(long));
    for (i = 0; i<= dimn; i++) {
      MOA_val->shape[i] = shape[i];
    }
  }
  /* if callFlag is -1 means, don't allocate memory for the tensor, this is used with sparse arrays implementation. */

    MOA_val->proc = (int * ) mcalloc (MOA_val->elements_ub, sizeof(int));
    MOA_val->dproc_ub = (int * ) mcalloc (MOA_val->elements_ub, sizeof(int));
    MOA_val->dproc = (int * *) mcalloc (MOA_val->elements_ub, sizeof(int *));
  if (callFlag != -1) {
    
    /*MOA_val->elements_ub =  Tau(shape, dimn); */
    MOA_val->elements = (MOA_elm * ) mcalloc (MOA_val->elements_ub, sizeof(MOA_elm));
    MOA_val->indexes = (signed long * ) mcalloc (MOA_val->elements_ub, sizeof(signed long));    /* if callFlag is 0, means initialize cell values with 0, which is done with mcalloc */
    if (callFlag != 0) {
      for (i = 0; i< MOA_val->elements_ub; i++) {
	/* if callFlag is 1 meaning, initialize the array values with the cellValue argument value */
	MOA_val->elements[i].prev_ub = 0;	
	MOA_val->elements[i].prev = NULL;
	MOA_val->indexes[i] = i;
	if (callFlag == 1) {
	  MOA_val->elements[i].val = cellValue;
	}
	else { /* else initialize the cell values with their flat index value */
	  MOA_val->elements[i].val = i;
	}
      }
    }
  }
}

void printMOA_old (MOA_rec * MOA) { 
  long i;
  mprintf(outputfilename, "The Alignment Matrix:\n", 0);
  mprintf(outputfilename,"< ", 0);
  for (i=0; i<MOA->elements_ub;i++) {
    mprintf(outputfilename, "%4d  ", 1, MOA->elements[i].val);
    if (((i+1) %  MOA->shape[ MOA->dimn - 1]) == 0) {
      mprintf(outputfilename," >\n", 0);
      if ((i+1)<MOA->elements_ub)
	mprintf(outputfilename,"< ", 0);
    }
  }

}

void printMOA_old2 (MOA_rec * MOA) { 
  long i, j, lcnt;
  long * stride = NULL; 
  
  stride = (long *) mmalloc (MOA->dimn * sizeof(long));
  mprintf(outputfilename, "\n The Tensor of Dimn %d & elements %d & Shape: ",2,  MOA->dimn, MOA->elements_ub);
  for (i = 0; i < MOA->dimn; i++) {
    if (i == 0)
      stride[i] = MOA->shape[i];
    else
      stride[i] = MOA->shape[i] * stride[i-1];
    mprintf(outputfilename, " %d ",1, MOA->shape[i]);
  }
  mprintf(outputfilename,"\n", 0);
  for (j = 0; j < MOA->dimn; j++) 
    mprintf(outputfilename,"< ", 0);		       
 
  for (i = 0; i < MOA->elements_ub; i++) {
    /* mprintf  (outputfilename,"i %d start ", 1, i); */
    mprintf(outputfilename, "%5d  ", 1, MOA->elements [i]);
    for (j = MOA->dimn - 1; j >= 0; j--)      
      if (((i+1) %  stride[j]) == 0) 
	mprintf(outputfilename," >", 0);          		   
    lcnt=0;
    for (j = MOA->dimn - 1; j >= 0; j--) 
      if ((i+1) < MOA->elements_ub - 1) 
	if (((i+1) %  stride[j]) == 0) 
	  lcnt ++;      
    if (lcnt > 0) {
      mprintf(outputfilename,"\n", 0);
      for (j = 0; j <  (MOA->dimn - lcnt); j++)
	mprintf(outputfilename,"  ", 0); 		
      for (j = 0; j <  lcnt; j++)
	mprintf(outputfilename,"< ", 0);
    }    
  }
  
  mprintf(outputfilename, "\n", 0);
  if (stride != NULL)	
    free(stride);
}
void printMOA (MOA_rec * MOA) { 
  long i, j, lcnt, dim1cnt, dim2cnt, dim3cnt;
  long * stride = NULL;
  
  stride = (long *) mmalloc (MOA->dimn * sizeof(long));
  mprintf(outputfilename, "\n The Tensor of Dimn %d & elements %d & Shape: ", 2, MOA->dimn, MOA->elements_ub);
  for (i = 0; i < MOA->dimn; i++) {
    if (i == 0)
      stride[i] = MOA->shape[i];
    else
      stride[i] = MOA->shape[i] * stride[i-1];
    mprintf(outputfilename, " %d ",1, MOA->shape[i]);
  }

  dim1cnt = dim2cnt = dim3cnt = 0;
  /* print indices for dimn 1 */
  mprintf(outputfilename,": \n", 0);
  /* leave 7 spaces for the 2nd dimension indices */
  mprintf(outputfilename,"       ", 0);	
  /* leave 2 spaces for each < for ewach dimension in the tensor*/
  for (j = 0; j < MOA->dimn; j++) 
    mprintf(outputfilename,"  ", 0);
   
  for (j = 0; j < MOA->shape[0]; j++) {
    mprintf(outputfilename,"%5d ", 1, dim1cnt);
    dim1cnt ++;
  }
  /* print 3rd dimension indices */
  mprintf(outputfilename," %9d", 1, dim3cnt);
  dim3cnt ++;	
  if (dim3cnt > MOA->shape[2] - 1)
    dim3cnt = 0;
 
  /* print separator line */
  dim1cnt = 0;
  mprintf(outputfilename,"\n", 0);
  /* leave 7 spaces for the 2nd dimension indices & more for the end */
  mprintf(outputfilename,"___________________", 0);	
  /* leave 2 spaces for each < for ewach dimension in the tensor */
  for (j = 0; j < MOA->dimn; j++) 
    mprintf(outputfilename,"__", 0);
   
  for (j = 0; j < MOA->shape[0]; j++) {
    mprintf(outputfilename,"______",0);	
    dim1cnt ++;
  }
  mprintf(outputfilename,"\n", 0);
  /* print 2nd dimension indices */
  mprintf(outputfilename,"%5d| ", 1, dim2cnt);
  dim2cnt ++;	
  for (j = 0; j < MOA->dimn; j++) 
    mprintf(outputfilename,"< ", 0);			
  
  for (i = 0; i < MOA->elements_ub; i++) {
    mprintf(outputfilename, "%5d ", 1, MOA->elements[i].val);
    for (j = MOA->dimn - 1; j >= 0; j--) 
      if (((i+1) %  stride[j]) == 0) 
	mprintf(outputfilename," >", 0);
    lcnt = 0;
    for (j = MOA->dimn - 1; j >= 0; j--) 
      if ((i+1) < MOA->elements_ub - 1) 
	if (((i+1) %  stride[j]) == 0) 
	  lcnt ++;
	
    if (lcnt > 0) {
      if (lcnt == 2) {
	/* print 3rd dimension indices */
	mprintf(outputfilename," %5d", 1, dim3cnt);
	dim3cnt ++;	
	if (dim3cnt > MOA->shape[2] - 1)
	  dim3cnt = 0;
      }
      if (dim2cnt == 0) {
	/* print separator line */
	dim1cnt = 0;
	mprintf(outputfilename,"\n", 0);
	/* leave 7 spaces for the 2nd dimension indices & more for the end */
	mprintf(outputfilename,"___________________", 0);	
	/* leave 2 spaces for each < for ewach dimension in the tensor */
	for (j = 0; j < MOA->dimn; j++) 
	  mprintf(outputfilename,"__", 0);
   
	for (j = 0; j < MOA->shape[0]; j++) {
	  mprintf(outputfilename,"______", 0);	
	  dim1cnt ++;
	}
      }
      mprintf(outputfilename,"\n", 0);
      /* print 2nd dimension indices */
      mprintf(outputfilename,"%5d| ", 1, dim2cnt);
      dim2cnt ++;	
      if (dim2cnt > MOA->shape[1] - 1)
	dim2cnt = 0;
      for (j = 0; j <  (MOA->dimn - lcnt); j++)
	mprintf(outputfilename,"  ", 0); 		
      for (j = 0; j <  lcnt; j++)
	mprintf(outputfilename,"< ", 0);
    }    
  }
  
  mprintf(outputfilename, "\n", 0);
  if (stride != NULL)	
    free(stride);
}

void deleteMOA(MOA_rec * MOA)
{
  long i;
  if (MOA != NULL) {	
    if (MOA->elements != NULL) {
      for (i = 0; i< MOA->elements_ub; i++) {
	if (MOA->elements[i].prev != NULL) {
	  free(MOA->elements[i].prev);
        }
      }
      free(MOA->elements);
    }
    if (MOA->shape != NULL)
      free(MOA->shape);
    if (MOA->indexes != NULL) {
      free(MOA->indexes);
    }
    if (MOA->proc != NULL) {
      free(MOA->proc);
    }
    if (MOA->dproc != NULL) {
      for (i = 0; i< MOA->elements_ub; i++) {
	if (MOA->dproc[i] != NULL) {
	  free(MOA->dproc[i]);
        }
      }
      free(MOA->dproc);
    }
    
    free(MOA);
  }
}

/* ***************************************************************
// **************************  Tau  ******************************
// ***************************************************************/
/*Number of elements in an MOA structure based on dimn and shape - regular MOA*/
long Tau (long * array_in, long array_ub)
{
  long rslt, i;
  rslt = array_in[0];
  for ( i = 1; i< array_ub; i++) {
    rslt = rslt * array_in[i];	
  }
  return (rslt);
} /* end Tau */

/* ***************************************************************
// **********************  Gamma        **************************
// ***************************************************************/
long Gamma (long * ind, long ind_ub, long * arr_shape, long shape_ub, int Front) {
  long rslt, i;
  rslt = ind[shape_ub - 1];
  for (i = shape_ub - 2; i>=0;i--) 
    /*rslt = rslt + (ind[i] * arr_shape[i]);*/
    rslt = (rslt * arr_shape[i]) + ind[i];
  return rslt;
}

long Gamma_old (long * ind, long ind_ub, long * arr_shape, long shape_ub, int Front) {
  long * new_ind = NULL;
  long rslt, i;
  if (shape_ub == 0) {
    new_ind = (long *) mmalloc(sizeof(long));
  }
  else {
    new_ind =  (long *) mmalloc(sizeof(long) * shape_ub);
  }
  
  if (Front == 1) {
    if (ind_ub == 0) {
      new_ind[0] = ind[0];
    }
    else {
      for (i = shape_ub - ind_ub; i< shape_ub; i++) {
	new_ind[i] = ind[i];
      }
    }
    /* fill the empty dimensions with zeros */
    if (ind_ub < shape_ub) {
      if (ind_ub + 1 == shape_ub) {
	new_ind[ind_ub] = 0;
      }
      else {
	for (i = 0; i < shape_ub - ind_ub; i++) {
	  new_ind[i] = 0;
	}
      }
    }
  }
  else {
    if (ind_ub == 0) {
      new_ind[0] = ind[0];
    }
    else {
      for (i = 0; i< ind_ub; i++) {
	new_ind[i] = ind[i];
      }
    }
    /* fill the empty dimensions with zeros */
    if (ind_ub < shape_ub) {
      if (ind_ub + 1 == shape_ub) {
	new_ind[ind_ub] = 0;
      }
      else {
	for (i = ind_ub; i< shape_ub; i++) {
	  new_ind[i] = 0;
	}
      }
    }
  }
  
  if (IsValidIndex(arr_shape, shape_ub, new_ind) == -1) {
    if (new_ind != NULL)	
      free(new_ind);
    return (-1);
  }
  else {
    rslt = new_ind[0];
    /*	mprintf(outputfilename, "\n new_ind %d ", 1, new_ind[0]);*/
    for (i = 1; i< shape_ub; i++) {
      /*for (i = shape_ub - 1; i>= 1; i--) { */
      rslt = (rslt * arr_shape[i]) + new_ind[i];
      /*mprintf(outputfilename, "i %d shape %d ind %d rslt %d ", 4, i,   arr_shape[i],  new_ind[i], rslt);*/
    }
    /*mprintf(outputfilename, "\n", 0);*/
    if (new_ind != NULL)	
      free(new_ind);
    return (rslt);
  }
} /* end Gamma*/
/***************************************************************
// **********************  Gamma Inverse************************
// *************************************************************/

void Gamma_Inverse (long ind, long * arr_shape, long shape_ub, long * rslt)
{
  long i, ind_temp;
  div_t div_result;

  if (ind >= Tau(arr_shape, shape_ub)) {
    mprintf(outputfilename, "The Index is beyond the upper bound of the array of the defined shape!", 0);
    return;
  }
  /*for (i = 0; i< shape_ub; i++) {
  //  rslt[i] = 0;
  }*/
  ind_temp = ind;
  /*for (i = shape_ub - 1; i>= 0; i --) { */
  for (i = 0; i< shape_ub; i ++) {
    div_result = div(ind_temp, arr_shape[i]);
    rslt[i] = div_result.rem; /* ind_temp Mod arr_shape->GetAt(i) */
    ind_temp = div_result.quot; /* ind_temp \ arr_shape->GetAt(i); */
  }
} /* end Gamma_Inverse */

/* ***************************************************************
// ****************************** PSI ****************************
// ***************************************************************/

void Psi (long * ind, long ind_ub, MOA_rec * MOA_in, MOA_rec * rslt) {
  long offset, i;
  long j;
  
  if (ind_ub > MOA_in->dimn) {
    mprintf(outputfilename, "Invalid index dimension. Psi Can't Take index of dimension greater than the Array dimension!\n", 0);
    return;
  }
  else if (ind_ub == 0) {
    if (ind[0] > MOA_in->shape[0] - 1) {
      printf  ("Invalid index at dimension: 0\n");
      return;
    }
  }
  else {
    for (j=0;j<ind_ub;j++) {
      if (ind[j] > MOA_in->shape[j] - 1) {
	mprintf(outputfilename, "Invalid index at dimension: %d\n", 1, j);
	return;
      }
    }
  }
  
  if (ind_ub < MOA_in->dimn) {
    rslt->shape = (long *) mmalloc (sizeof(long) * (MOA_in->dimn - ind_ub));
    rslt->dimn = MOA_in->dimn - ind_ub;
    if (rslt->dimn == 0){
      rslt->shape[0] = MOA_in->shape[MOA_in->dimn - 1];
    }
    else {
      for (j = 0; j < rslt->dimn; j++) {
	rslt->shape[j] = MOA_in->shape[(MOA_in->dimn - 1) - j];
      }
    }
  }
  else {
    rslt->shape = (long *) mmalloc(sizeof(long));
    rslt->shape[0] = 1;
    rslt->dimn = 0;
  }
  
  rslt->elements_ub = Tau (rslt->shape, rslt->dimn);
  rslt->elements = (MOA_elm *) mmalloc(sizeof(MOA_elm) *rslt->elements_ub);
  offset = Gamma (ind, ind_ub, MOA_in->shape, MOA_in->dimn, 0);
  for (i = 0; i < rslt->elements_ub;i++) {
    rslt->elements[i].val = MOA_in->elements[offset + i].val;
  }
  
} /* end Psi */

/* ***************************************************************
// **********************   VecIsEqual   *************************
// ***************************************************************/
int VecIsEqual (long * Array_1, long array1_ub, long * Array_2, long array2_ub) 
{
  long rslt, i;
  if (array1_ub != array2_ub) 
    rslt = 0;
  else
    rslt = 1;
  
  for (i = 0 ; (i < array1_ub) && (rslt); i++ ) {
    if (Array_1[i] != Array_2[i]) {
      rslt = 0;
    }
  }
  
  return (rslt);
}

/* ***************************************************************
// **********************  GetLowerNeighbors  ********************
// ***************************************************************/

long GetLowerNeighbors_test(long seqNum, long * seqLen,  long * m_index, long * * lNeighbors) {
  long score, i, j, k;
  long lnCount; /*lower neighbor processed Counter*/
  long combnum; /* number of combinations for decrementing i number of indices*/
  long * * combin = NULL; /* combinations of lower neighbor indices*/
  long flatIndex;
  long validIndex;
  
  


  score = 0;
  lnCount = 0;
  (*lNeighbors) = (long *) mcalloc (1, sizeof(long));
  /*process all decremented index neighbor */
  validIndex = 0;
  for (k=0; k<seqNum; k++) {
    m_index[k]--;
    if ( m_index[k] < 0) {
      validIndex = 1;
    }
  }
  if (validIndex == 0) { /* add it to lower neighbors list */
    if (pdebug == 2)
      mprintf(outputfilename, "The lower neighbor index: ", 0);
    if (pdebug == 2)
      for (k=0; k<seqNum; k++) {
	mprintf(outputfilename, "%d ",1,  m_index[k]);
      }
    flatIndex = Gamma(m_index, seqNum, seqLen, seqNum, 1);
    (*lNeighbors)[lnCount] = flatIndex;
    /*(*dd)[lnCount] = seqNum;
    (*ddpos)[lnCount] = (int *) mcalloc (seqNum, sizeof(int));
    */
    if (pdebug == 2)
      mprintf(outputfilename," ln %d ind %d ", 2, lnCount, flatIndex);
    lnCount ++;
  }
  for (k=0; k<seqNum; k++) {
    /*(*ddpos)[lnCount][k] = k; */
    m_index[k]++;
    }
  
  /* process remaining neighbors by starting by decrementing only one index entry at a time and try all different locations, then 2 locations at a time, and so forth till decrement all*/
  for (i = 1; i< seqNum; i++)  {
    /* number of the different combinations of which indeces in the multidimensional index to decrement so that all lower neighbors can be processed */
    combnum = (Factorial(seqNum)/(Factorial(seqNum-i) * Factorial(i)));
    /* create memory for combinations matrix */
    combin = (long * *) mcalloc (combnum, sizeof(long *));
    for (k=0; k<combnum; k++)
      combin[k] = (long *) mcalloc (i, sizeof(long));
    if (combin == NULL) {
      mprintf(outputfilename,"Can not allocate memory to combinations matrix\n", 0);
      return -1;
    }
    /* get matrix of all possible combinations */
    Combinations(seqNum, i, &combin);
    /* loop to decrement the selected indices */
    for (j=0; j<combnum; j++) {
      validIndex = 0;
      for (k=0; k<i; k++) {
	m_index[combin[j][k]-1]--;
	if ( m_index[combin[j][k]-1] < 0) {
	  validIndex = 1; /* false */
	}
      }
      if (validIndex == 0) { /*  add it to lower neighbors list*/
	if (pdebug == 2)
	  mprintf(outputfilename, "The lower neighbor index: ", 0);
	if (pdebug == 2)
	  for (k=0; k<seqNum; k++) {
	    mprintf(outputfilename, "%d ", 1, m_index[k]);
	  }
	flatIndex = Gamma(m_index, seqNum, seqLen, seqNum, 1);
	(*lNeighbors) = (long *) realloc ((*lNeighbors), (lnCount+1) * sizeof(long));
     if ((*lNeighbors) == NULL) {
		mprintf(outputfilename, "Could not reallocate memory for Lower Neighbors %d!\n", 1, (lnCount+1));
		return -1;
      }
	(*lNeighbors)[lnCount] = flatIndex;
	lnCount++; /*increment lower neighbor counter */
	if (pdebug == 2) {
	  mprintf(outputfilename," ln %d ind %d ", 2, lnCount, flatIndex);
	}
      }
      for (k=0; k<i; k++) {
	m_index[combin[j][k]-1]++;
	}
    }
    /*free combinations matrix*/
    if (combin != NULL) 
		for (k=0; k<combnum; k++) {
	        if (combin[k] != NULL)	
			  free(combin[k]);
		}
    free(combin);
  }
  return lnCount;
}

/* ***************************************************************
// **********************  IsValidIndex **************************
// ***************************************************************/

int IsValidIndex (long * shape, long shape_ub, long * index) {
  long i;
	if (shape_ub == 0) {
		if ((index[0] < 0) || (index[0] >= shape[0])) 
			return (-1);
	}
	else {
		for (i = 0; i < shape_ub; i++) {
			if ((index[i] < 0) || (index[i] >= shape[i])) 
				return (-1);
		}
	}
	return (1);
}
/* ***************************************************************
// **************************** NextIndex ************************
// ***************************************************************/

void NextIndex (long * shape, long shape_ub, long * Prev_Index) {
  long i, j;
  long * rslt = NULL;
  rslt = (long *) mmalloc( sizeof(long) * shape_ub);

  /* copy the previous index */
  for (i = 0; i < shape_ub; i++){
    rslt[i] = Prev_Index[i];
  }

  /*for (i = shape_ub - 1; i>= 0; i--){ */
  for (i = 0; i < shape_ub; i++){
    if (Prev_Index[i] < shape[i] - 1) {
      rslt[i] = Prev_Index[i] + 1;
      
      if (i > 0) {
	for (j = i - 1; j >= 0; j--) {
	  rslt[j] = 0;					
	}
      }
      
      break;
    }
  }

  for (i = 0; i< shape_ub; i++){
    Prev_Index[i] = rslt[i];
  }
  if (rslt != NULL)	
    free(rslt);
}/* end NextIndex */
/* ***************************************************************
// **********************      Take      *************************
// ***************************************************************/

int Take (long * ind, long ind_ub, MOA_rec * MOA_in, MOA_rec  * rslt)
{
  long * valid_index = NULL;
  long * ref_dimn = NULL;
  long * Last_ref_dimn = NULL;
  long negative, positive, l1_finished;
  long counter;
  long offset, i;
  
  negative = 0;
  positive = 0;
  /* decide from which direction to take */
  if (ind[0] >= 0) {
    positive = 1;
  }
  else {
    negative = 1;
  }
  
  rslt->dimn = MOA_in->dimn;
  
  /* test the index parameter to form a valid index to the Psi function */
  if (MOA_in->dimn == 0) {
    valid_index = (long *) mmalloc (sizeof(long));
    ref_dimn = (long *) mmalloc (sizeof(long));
    Last_ref_dimn = (long * ) mmalloc (sizeof(long));
    rslt->shape = (long *) mmalloc (sizeof(long));
    if ((ind[0] > 0) && (ind[0] <= MOA_in->shape[0])) {
      if (negative == 1) {
	mprintf(outputfilename,"Take Error: Invalid index!\n", 0);
	return -1;
      }
      valid_index[0] = ind[0];
    }
    else if ((ind[0] < 0) && (abs(ind[0]) <= MOA_in->shape[0] + 1)) {
      if (positive == 1) {
	mprintf(outputfilename, "Take Error: Invalid index!\n", 0);
	return -1;
      }
      valid_index[0] = MOA_in->shape[0] - abs(ind[0]);
    }
    else {
      if (  !((ind[0] >= 0) && (ind[0] <= MOA_in->shape[0])) 
	    && !((ind[0] < 0) && (abs(ind[0]) <= MOA_in->shape[0] + 1 ) ) ) {
	mprintf(outputfilename, "Take Error: Invalid index!\n", 0);
	return -1;
      }
    }
  }
  else {
    valid_index = (long *) mmalloc (MOA_in->dimn * sizeof(long));
    ref_dimn = (long *) mmalloc (MOA_in->dimn * sizeof(long)); 
    Last_ref_dimn =(long *) mmalloc (MOA_in->dimn * sizeof(long));
    rslt->shape = (long *) mmalloc (MOA_in->dimn * sizeof(long));
    for (i = 0; i < MOA_in->dimn; i++) {
      if (ind_ub > i) {
	if ((ind[i] > 0) && (ind[i] <= MOA_in->shape[i])) {
	  if (negative == 1) {
	    mprintf(outputfilename, "Take Error: Invalid index!\n", 0);
	    return -1;
	  }
	  valid_index[i] = ind[i];
	}
	else if ((ind[i] < 0) && (abs(ind[i]) <= MOA_in->shape[i] + 1)) {
	  if (positive == 1) {
	    mprintf(outputfilename, "Take Error: Invalid index!\n", 0);
	    return -1;
	  }
	  
	  valid_index[i] = MOA_in->shape[i] - abs(ind[i]);
	}
	else {
	  if (  !((ind[i] >= 0) && (ind[i] <= MOA_in->shape[i])) 
		&& !((ind[i] < 0) && (abs(ind[i]) <= MOA_in->shape[i] + 1 ) ) ) {
	    /*mprintf(outputfilename, "Take Error: Invalid index!\n", 0);
	    //return -1; */
	    if (ind[i] >= 0)
	      valid_index[i] = MOA_in->shape[i];
	    else
	      valid_index[i] = 0;
	    
	  }
	}
      } 
      else {
	valid_index[i] = MOA_in->shape[i];
      }
    }
  }
  
  
  l1_finished = 0;
  counter = 0;
  if (pdebug == 7) {
    mprintf(outputfilename, "\npositive %d and valid_index:", 1, positive);
    for (i = 0; i < MOA_in->dimn; i++) {
      mprintf(outputfilename, " %d ", 1,  valid_index[i]);
    }
    mprintf(outputfilename, " and input ind:", 0);
    for (i = 0; i < MOA_in->dimn; i++) {
      mprintf(outputfilename, " %d ", 1,  ind[i]);
    }
    mprintf(outputfilename, " and shape:", 0);
    for (i = 0; i < MOA_in->dimn; i++) {
      mprintf(outputfilename, " %d ", 1,  MOA_in->shape[i]);
    }
  }
  
  
  /*call the Psi function with the valid index parameter to return the required parition of the array */
  if (positive == 1) {
    if (MOA_in->dimn == 0) {
      ref_dimn[0] = 0;
      Last_ref_dimn[0] = valid_index[0] - 1;
      rslt->shape[0] = valid_index[0];
    }
    else {
      for (i=0; i< MOA_in->dimn; i++) {
	ref_dimn[i] = 0;
	Last_ref_dimn[i] = valid_index[i] - 1;
	rslt->shape[i] = valid_index[i];
      }
    }
    rslt->elements_ub = Tau(rslt->shape, rslt->dimn);
    rslt->indexes = (signed long * ) mcalloc (rslt->elements_ub, sizeof(signed long));
    rslt->elements = (MOA_elm * ) mcalloc (rslt->elements_ub, sizeof(MOA_elm));
    if (rslt->elements_ub == 1) 
      rslt->dimn = 0;
    counter = 0;
    l1_finished = 0;
    
    if (pdebug == 7) {
      mprintf(outputfilename, " and  rslt->shape:", 0);
      for (i = 0; i < MOA_in->dimn; i++) {
	mprintf(outputfilename, " %d ", 1,   rslt->shape[i]);
      }
      offset = Gamma(valid_index, MOA_in->dimn, MOA_in->shape, MOA_in->dimn, 1);
      mprintf(outputfilename, "\n  offset %d  = & indices are : ", 1, offset);
    }
    while (l1_finished == 0) {
      rslt->indexes[counter] = Gamma(ref_dimn, MOA_in->dimn, MOA_in->shape, MOA_in->dimn, 1);
      rslt->elements[counter] = MOA_in->elements[rslt->indexes[counter]];
	  rslt->elements[counter].prev = NULL;
	  rslt->elements[counter].prev_ub = 0;
      if (pdebug == 7) {
	mprintf(outputfilename, "  %d ", 1, rslt->indexes[counter]);
      }
      counter ++;
      l1_finished = VecIsEqual(ref_dimn, MOA_in->dimn, Last_ref_dimn, MOA_in->dimn);
      NextIndex(valid_index, MOA_in->dimn, ref_dimn);
    }
    
  }
  
  else {
    if (MOA_in->dimn == 0) {
      if (abs(ind[0]) < MOA_in->shape[0]) {
	ind[0] = abs(ind[0]);
	rslt->shape[0] = ind[0];
	ref_dimn[0] = 0;
	Last_ref_dimn[0] = ind[0] - 1;
      }
      else{
	valid_index[0] = 0;
	ind[0] = MOA_in->shape[0];
	rslt->shape[0] = ind[0];
	ref_dimn[0] = 0;
	Last_ref_dimn[0] = ind[0] - 1;
      }
    }
    else {
      for (i = 0; i < MOA_in->dimn; i++) {
	if (abs(ind[i]) < MOA_in->shape[i]) {
	  ind[i] = abs(ind[i]);
	  rslt->shape[i] = ind[i];
	  ref_dimn[i] = 0;
	  Last_ref_dimn[i] = ind[i] - 1;
	}
	else {
	  valid_index[i] = 0;
	  ind[i] = MOA_in->shape[i];
	  rslt->shape[i] = ind[i];
	  ref_dimn[i] = 0;
	  Last_ref_dimn[i] = ind[i] - 1;
	}
      }
    }
    rslt->elements_ub = Tau(rslt->shape, rslt->dimn);
    rslt->indexes = (signed long * ) mcalloc (rslt->elements_ub, sizeof(signed long));
    rslt->elements = (MOA_elm * ) mcalloc (rslt->elements_ub, sizeof(MOA_elm));
    if (rslt->elements_ub == 1) 
      rslt->dimn = 0;
    if (pdebug == 7) {
      mprintf(outputfilename, " and  rslt->shape:", 0);
      for (i = 0; i < MOA_in->dimn; i++) {
	mprintf(outputfilename, " %d ", 1,   rslt->shape[i]);
      }
    }
    
    counter = 0;
    l1_finished = 0;
    offset = Gamma(valid_index, MOA_in->dimn, MOA_in->shape, MOA_in->dimn, 1);
    if (pdebug == 7) {
      mprintf(outputfilename, "\n  offset %d  = & indices are : ", 1, offset);
      
    }
    while (l1_finished == 0) {
      rslt->indexes[counter] = offset + Gamma(ref_dimn, MOA_in->dimn, MOA_in->shape, MOA_in->dimn, 1);
      rslt->elements[counter] = MOA_in->elements[rslt->indexes[counter]];	    	    
 	  rslt->elements[counter].prev = NULL;
	  rslt->elements[counter].prev_ub = 0;
     if (pdebug == 7) {
	mprintf(outputfilename, "  %d ", 1, rslt->indexes[counter]);
      }
      counter ++;
      l1_finished = VecIsEqual(ref_dimn, MOA_in->dimn, Last_ref_dimn, MOA_in->dimn);
      NextIndex(ind, MOA_in->dimn, ref_dimn);
    }
  }
  
  if (valid_index != NULL) 	
    free(valid_index);
  if (ref_dimn  != NULL) 
    free(ref_dimn);
  if (Last_ref_dimn != NULL)
    free(Last_ref_dimn);
  return 0;	
}
/* ***************************************************************
// *******  Take without creating the whole Tensor     ***********
// ***************************************************************/

int TakeInd (long * ind, long ind_ub, MOA_rec * MOA_in, MOA_rec  * rslt)
{
  long * valid_index = NULL;
  long * ref_dimn = NULL;
  long * Last_ref_dimn = NULL;
  long negative, positive, l1_finished;
  long counter;
  long offset, i;
  
  negative = 0;
  positive = 0;
  /* decide from which direction to take */
  if (ind[0] >= 0) {
    positive = 1;
  }
  else {
    negative = 1;
  }
  
  rslt->dimn = MOA_in->dimn;
  
  /* test the index parameter to form a valid index to the Psi function */
  if (MOA_in->dimn == 0) {
    valid_index = (long *) mmalloc (sizeof(long));
    ref_dimn = (long *) mmalloc (sizeof(long));
    Last_ref_dimn = (long * ) mmalloc (sizeof(long));
    rslt->shape = (long *) mmalloc (sizeof(long));
    if ((ind[0] > 0) && (ind[0] <= MOA_in->shape[0])) {
      if (negative == 1) {
	mprintf(outputfilename,"Take Error: Invalid index!\n", 0);
	return -1;
      }
      valid_index[0] = ind[0];
    }
    else if ((ind[0] < 0) && (abs(ind[0]) <= MOA_in->shape[0] + 1)) {
      if (positive == 1) {
	mprintf(outputfilename, "Take Error: Invalid index!\n", 0);
	return -1;
      }
      valid_index[0] = MOA_in->shape[0] - abs(ind[0]);
    }
    else {
      if (  !((ind[0] >= 0) && (ind[0] <= MOA_in->shape[0])) 
	    && !((ind[0] < 0) && (abs(ind[0]) <= MOA_in->shape[0] + 1 ) ) ) {
	mprintf(outputfilename, "Take Error: Invalid index!\n", 0);
	return -1;
      }
    }
  }
  else {
    valid_index = (long *) mmalloc (MOA_in->dimn * sizeof(long));
    ref_dimn = (long *) mmalloc (MOA_in->dimn * sizeof(long)); 
    Last_ref_dimn =(long *) mmalloc (MOA_in->dimn * sizeof(long));
    rslt->shape = (long *) mmalloc (MOA_in->dimn * sizeof(long));
    for (i = 0; i < MOA_in->dimn; i++) {
      if (ind_ub > i) {
	if ((ind[i] > 0) && (ind[i] <= MOA_in->shape[i])) {
	  if (negative == 1) {
	    mprintf(outputfilename, "Take Error: Invalid index!\n", 0);
	    return -1;
	  }
	  valid_index[i] = ind[i];
	}
	else if ((ind[i] < 0) && (abs(ind[i]) <= MOA_in->shape[i] + 1)) {
	  if (positive == 1) {
	    mprintf(outputfilename, "Take Error: Invalid index!\n", 0);
	    return -1;
	  }
	  
	  valid_index[i] = MOA_in->shape[i] - abs(ind[i]);
	}
	else {
	  if (  !((ind[i] >= 0) && (ind[i] <= MOA_in->shape[i])) 
		&& !((ind[i] < 0) && (abs(ind[i]) <= MOA_in->shape[i] + 1 ) ) ) {
	    /*mprintf(outputfilename, "Take Error: Invalid index!\n", 0);
	    //return -1; */
	    if (ind[i] >= 0)
	      valid_index[i] = MOA_in->shape[i];
	    else
	      valid_index[i] = 0;
	    
	  }
	}
      } 
      else {
	valid_index[i] = MOA_in->shape[i];
      }
    }
  }
  
  
  l1_finished = 0;
  counter = 0;
  if (pdebug == 7) {
    mprintf(outputfilename, "\npositive %d and valid_index:", 1, positive);
    for (i = 0; i < MOA_in->dimn; i++) {
      mprintf(outputfilename, " %d ", 1,  valid_index[i]);
    }
    mprintf(outputfilename, " and input ind:", 0);
    for (i = 0; i < MOA_in->dimn; i++) {
      mprintf(outputfilename, " %d ", 1,  ind[i]);
    }
    mprintf(outputfilename, " and shape:", 0);
    for (i = 0; i < MOA_in->dimn; i++) {
      mprintf(outputfilename, " %d ", 1,  MOA_in->shape[i]);
    }
  }
  
  
  /*call the Psi function with the valid index parameter to return the required parition of the array */
  if (positive == 1) {
    if (MOA_in->dimn == 0) {
      ref_dimn[0] = 0;
      Last_ref_dimn[0] = valid_index[0] - 1;
      rslt->shape[0] = valid_index[0];
    }
    else {
      for (i=0; i< MOA_in->dimn; i++) {
	ref_dimn[i] = 0;
	Last_ref_dimn[i] = valid_index[i] - 1;
	rslt->shape[i] = valid_index[i];
      }
    }
    rslt->elements_ub = Tau(rslt->shape, rslt->dimn);
    rslt->indexes = (signed long * ) mcalloc (rslt->elements_ub, sizeof(signed long));
    rslt->elements = (MOA_elm * ) mcalloc (rslt->elements_ub, sizeof(MOA_elm));
    if (rslt->elements_ub == 1) 
      rslt->dimn = 0;
    counter = 0;
    l1_finished = 0;
    
    if (pdebug == 7) {
      mprintf(outputfilename, " and  rslt->shape:", 0);
      for (i = 0; i < MOA_in->dimn; i++) {
	mprintf(outputfilename, " %d ", 1,   rslt->shape[i]);
      }
      offset = Gamma(valid_index, MOA_in->dimn, MOA_in->shape, MOA_in->dimn, 1);
      mprintf(outputfilename, "\n  offset %d  = & indices are : ", 1, offset);
    }
    while (l1_finished == 0) {
      rslt->indexes[counter] = Gamma(ref_dimn, MOA_in->dimn, MOA_in->shape, MOA_in->dimn, 1);
      rslt->elements[counter] = MOA_in->elements[rslt->indexes[counter]];
	  rslt->elements[counter].prev = NULL;
	  rslt->elements[counter].prev_ub = 0;
      if (pdebug == 7) {
	mprintf(outputfilename, "  %d ", 1, rslt->indexes[counter]);
      }
      counter ++;
      l1_finished = VecIsEqual(ref_dimn, MOA_in->dimn, Last_ref_dimn, MOA_in->dimn);
      NextIndex(valid_index, MOA_in->dimn, ref_dimn);
    }
    
  }
  
  else {
    if (MOA_in->dimn == 0) {
      if (abs(ind[0]) < MOA_in->shape[0]) {
	ind[0] = abs(ind[0]);
	rslt->shape[0] = ind[0];
	ref_dimn[0] = 0;
	Last_ref_dimn[0] = ind[0] - 1;
      }
      else{
	valid_index[0] = 0;
	ind[0] = MOA_in->shape[0];
	rslt->shape[0] = ind[0];
	ref_dimn[0] = 0;
	Last_ref_dimn[0] = ind[0] - 1;
      }
    }
    else {
      for (i = 0; i < MOA_in->dimn; i++) {
	if (abs(ind[i]) < MOA_in->shape[i]) {
	  ind[i] = abs(ind[i]);
	  rslt->shape[i] = ind[i];
	  ref_dimn[i] = 0;
	  Last_ref_dimn[i] = ind[i] - 1;
	}
	else {
	  valid_index[i] = 0;
	  ind[i] = MOA_in->shape[i];
	  rslt->shape[i] = ind[i];
	  ref_dimn[i] = 0;
	  Last_ref_dimn[i] = ind[i] - 1;
	}
      }
    }
    rslt->elements_ub = Tau(rslt->shape, rslt->dimn);
    rslt->indexes = (signed long * ) mcalloc (rslt->elements_ub, sizeof(signed long));
    rslt->elements = (MOA_elm * ) mcalloc (rslt->elements_ub, sizeof(MOA_elm));
    if (rslt->elements_ub == 1) 
      rslt->dimn = 0;
    if (pdebug == 7) {
      mprintf(outputfilename, " and  rslt->shape:", 0);
      for (i = 0; i < MOA_in->dimn; i++) {
	mprintf(outputfilename, " %d ", 1,   rslt->shape[i]);
      }
    }
    
    counter = 0;
    l1_finished = 0;
    offset = Gamma(valid_index, MOA_in->dimn, MOA_in->shape, MOA_in->dimn, 1);
    if (pdebug == 7) {
      mprintf(outputfilename, "\n  offset %d  = & indices are : ", 1, offset);
      
    }
    while (l1_finished == 0) {
      rslt->indexes[counter] = offset + Gamma(ref_dimn, MOA_in->dimn, MOA_in->shape, MOA_in->dimn, 1);
      if (pdebug == 7) {
	mprintf(outputfilename, "  %d ", 1, rslt->indexes[counter]);
      }
      counter ++;
      l1_finished = VecIsEqual(ref_dimn, MOA_in->dimn, Last_ref_dimn, MOA_in->dimn);
      NextIndex(ind, MOA_in->dimn, ref_dimn);
    }
  }
  
  if (valid_index != NULL) 	
    free(valid_index);
  if (ref_dimn  != NULL) 
    free(ref_dimn);
  if (Last_ref_dimn != NULL)
    free(Last_ref_dimn);
  return 0;	
}

/* ***************************************************************
// **********************      Drop      *************************
// ***************************************************************/


int Drop (long * ind, long ind_ub, MOA_rec * MOA_in, MOA_rec *  rslt)
{

  long * valid_index = NULL;
  int positive, negative, ret;
  long i;
  
  negative = 0;
  positive = 0;
  
  if (ind[0] >= 0) {
    positive = 1;
  } 
  else {
    negative = 1;
  }
  
  if (MOA_in->dimn == 0) {
    valid_index = (long *) mmalloc (sizeof(long));
    if ((ind[0] >= 0) && ( ind[0] < MOA_in->shape[0])) {
      if (negative == 1) {
	mprintf(outputfilename, "Drop Error: Invalid index!\n", 0);
	return -1;
      }
      valid_index[0] = MOA_in->shape[0] - ind[0];
    }
    else if ((ind[0] < 0) && (abs(ind[0]) <= MOA_in->shape[0])) {
      if (positive == 1) {
	mprintf(outputfilename, "Drop Error: Invalid index!\n", 0);
	return -1;
      }
      valid_index[0] = MOA_in->shape[0] - abs(ind[0]);
    }
    else {
      mprintf(outputfilename, "Drop Error: Invalid index!\n", 0);
      return -1;
    }
  }
  else {
    valid_index = (long *) mmalloc(MOA_in->dimn * sizeof(long));
    for (i = 0; i < MOA_in->dimn; i++) {
      if (ind_ub > i) {
	if ((ind[i] >= 0) && ( ind[i] < MOA_in->shape[i])) {
	  if (negative == 1) {
	    mprintf(outputfilename, "Drop Error: Invalid index at dimension %d!\n", 1, i);
	    return -1;
	  }
	  valid_index[i] = MOA_in->shape[i] - ind[i];
	}
	else if ((ind[i] < 0) && (abs(ind[i]) <= MOA_in->shape[i])) {
	  if (positive == 1) {
	    mprintf(outputfilename, "Drop Error: Invalid index at dimension %d!\n", 1, i);
	    return -1;
	  }
	  valid_index[i] = MOA_in->shape[i] - abs(ind[i]);
	}
	else {
	  mprintf(outputfilename, "Drop Error: Invalid index at dimension %d!\n", 1, i);
	  return -1;
	}
      }
      else {
	valid_index[i] = MOA_in->shape[i];
      }
    }
  }
  
  if (ind_ub == 0) {
    valid_index[0] = MOA_in->shape[0] - ind[0];
  }
  
  if (positive) {
    if (MOA_in->dimn  == 0) {
      valid_index[0] = valid_index[0] * (-1);
    }
    else {
      for (i = 0; i < MOA_in->dimn; i ++) {
	valid_index[i] = valid_index[i] * (-1);
      }
    }
  }    
  if (pdebug == 7) {
    mprintf(outputfilename, "\nin drop before take with valid index =  ", 0);
    
    for (i = 0; i < MOA_in->dimn; i ++) {
      mprintf(outputfilename, " %d ", 1, valid_index[i]);
    }
    mprintf(outputfilename, " and  MOA_in->shape =  ", 0);
    for (i = 0; i < MOA_in->dimn; i ++) {
      mprintf(outputfilename, " %d ", 1,  MOA_in->shape[i]);
    }
    mprintf(outputfilename, " and  ind  =  ", 0);
    for (i = 0; i < MOA_in->dimn; i ++) {
      mprintf(outputfilename, " %d ", 1,  ind[i]);
    }
  }
  ret = Take(valid_index, MOA_in->dimn, MOA_in, rslt);
  
  if (valid_index != NULL) 	
    free (valid_index);
  
  return ret;
}

/* ***************************************************************
// *******  Drop without creating the whole Tensor  **************
// ***************************************************************/


int DropInd (long * ind, long ind_ub, MOA_rec * MOA_in, MOA_rec *  rslt)
{

  long * valid_index = NULL;
  int positive, negative, ret;
  long i;
  
  negative = 0;
  positive = 0;
  
  if (ind[0] >= 0) {
    positive = 1;
  } 
  else {
    negative = 1;
  }
  
  if (MOA_in->dimn == 0) {
    valid_index = (long *) mmalloc (sizeof(long));
    if ((ind[0] >= 0) && ( ind[0] < MOA_in->shape[0])) {
      if (negative == 1) {
	mprintf(outputfilename, "Drop Error: Invalid index!\n", 0);
	return -1;
      }
      valid_index[0] = MOA_in->shape[0] - ind[0];
    }
    else if ((ind[0] < 0) && (abs(ind[0]) <= MOA_in->shape[0])) {
      if (positive == 1) {
	mprintf(outputfilename, "Drop Error: Invalid index!\n", 0);
	return -1;
      }
      valid_index[0] = MOA_in->shape[0] - abs(ind[0]);
    }
    else {
      mprintf(outputfilename, "Drop Error: Invalid index!\n", 0);
      return -1;
    }
  }
  else {
    valid_index = (long *) mmalloc(MOA_in->dimn * sizeof(long));
    for (i = 0; i < MOA_in->dimn; i++) {
      if (ind_ub > i) {
	if ((ind[i] >= 0) && ( ind[i] < MOA_in->shape[i])) {
	  if (negative == 1) {
	    mprintf(outputfilename, "Drop Error: Invalid index at dimension %d!\n", 1, i);
	    return -1;
	  }
	  valid_index[i] = MOA_in->shape[i] - ind[i];
	}
	else if ((ind[i] < 0) && (abs(ind[i]) <= MOA_in->shape[i])) {
	  if (positive == 1) {
	    mprintf(outputfilename, "Drop Error: Invalid index at dimension %d!\n", 1, i);
	    return -1;
	  }
	  valid_index[i] = MOA_in->shape[i] - abs(ind[i]);
	}
	else {
	  mprintf(outputfilename, "Drop Error: Invalid index at dimension %d!\n", 1, i);
	  return -1;
	}
      }
      else {
	valid_index[i] = MOA_in->shape[i];
      }
    }
  }
  
  if (ind_ub == 0) {
    valid_index[0] = MOA_in->shape[0] - ind[0];
  }
  
  if (positive) {
    if (MOA_in->dimn  == 0) {
      valid_index[0] = valid_index[0] * (-1);
    }
    else {
      for (i = 0; i < MOA_in->dimn; i ++) {
	valid_index[i] = valid_index[i] * (-1);
      }
    }
  }    
  if (pdebug == 7) {
    mprintf(outputfilename, "\nin drop before take with valid index =  ", 0);
    
    for (i = 0; i < MOA_in->dimn; i ++) {
      mprintf(outputfilename, " %d ", 1, valid_index[i]);
    }
    mprintf(outputfilename, " and  MOA_in->shape =  ", 0);
    for (i = 0; i < MOA_in->dimn; i ++) {
      mprintf(outputfilename, " %d ", 1,  MOA_in->shape[i]);
    }
    mprintf(outputfilename, " and  ind  =  ", 0);
    for (i = 0; i < MOA_in->dimn; i ++) {
      mprintf(outputfilename, " %d ", 1,  ind[i]);
    }
  }
  ret = TakeInd(valid_index, MOA_in->dimn, MOA_in, rslt);
  
  if (valid_index != NULL) 	
    free (valid_index);
  
  return ret;
}


/* ***************************************************************
// **********************     Reshape    *************************
// ***************************************************************/

void Reshape (long * new_shape, long  n_shape_ub, MOA_rec * MOA_in, MOA_rec * rslt)
{

  long num_elements, i;
  div_t div_result;
  int k;
  
  num_elements = Tau(new_shape, n_shape_ub);
  rslt->dimn = n_shape_ub;
  
	
  if (rslt->dimn ==0) {
    rslt->shape = (long * ) mmalloc (sizeof(long));
    rslt->shape[0] = new_shape[0];
  }
  else {
    rslt->shape = (long *) mmalloc (sizeof(long) * rslt->dimn);
    
    for (k = 0; k < rslt->dimn; k++) {
      rslt->shape[k] = new_shape[k];
    }
  }
  /* rslt->lelm_size = MOA_in->lelm_size; */
  rslt->elements_ub = num_elements;
  rslt->elements = (MOA_elm *) mmalloc (sizeof(MOA_elm) * rslt->elements_ub);
  for (i = 0; i < rslt->elements_ub ; i++) {
    /* i Mod orig_num_elements */
    div_result = div(i, MOA_in->elements_ub);
    rslt->elements[i].val = MOA_in->elements[div_result.rem].val; /* i Mod orig_num_elements */
  }
      
}
/* ***************************************************************
// **********************    Catenate    *************************
// ***************************************************************/



void Catenate (MOA_rec * MOA_1, MOA_rec * MOA_2, long  Cat_DIM, MOA_rec * rslt)
{
  
  long * ref_dim = NULL;
  long * Last_ref_dimn = NULL;
  long current_idx, ref_idx, i;
  int l1_finished;
  
  if (MOA_1->dimn != MOA_2->dimn) {
    mprintf(outputfilename, "The Dimension of the Input Arrays must be equal!\n", 0);
    return;
  }
  if ((Cat_DIM < 0) || (Cat_DIM > MOA_1->dimn)) {
    mprintf(outputfilename, "Catenation Dimension Can't be less than nor greater than the Dimension of the Input Arrays which is: %d \n", 1, MOA_1->dimn);
    return;
  }
  
  
  rslt->dimn = MOA_1->dimn;
  if (MOA_1->dimn == 0) {
    ref_dim = (long *) mmalloc (sizeof(long));
    Last_ref_dimn = (long *) mmalloc (sizeof(long));
    rslt->shape = (long *) mmalloc (sizeof(long));
    rslt->shape[0] = MOA_1->shape[0];
  }
  else {
    ref_dim =(long *) mmalloc (sizeof(long) * MOA_1->dimn);
    Last_ref_dimn = (long *) mmalloc (sizeof(long) * MOA_1->dimn);
    rslt->shape = (long *) mmalloc (sizeof(long) * MOA_1->dimn);
    for (i = 0; i< MOA_1->dimn; i++) {
      rslt->shape[i] = MOA_1->shape[i];
    }
  }
  
  if (Cat_DIM == 0) 
    Cat_DIM = 1;
  
  rslt->shape[Cat_DIM - 1] = MOA_2->shape[Cat_DIM - 1] + MOA_1->shape[Cat_DIM - 1];
  
  if (MOA_1->dimn == 0) {
    ref_dim[0] = 0;
    Last_ref_dimn[0] = rslt->shape[0] - 1;
  }
  else {
    for (i = 0; i < MOA_1->dimn; i++ ) {
      ref_dim[i] = 0;
      Last_ref_dimn[i] = rslt->shape[i] - 1;
    }
  }
  
  rslt->elements_ub = Tau(rslt->shape, rslt->dimn);
  rslt->elements = (MOA_elm *) mmalloc (sizeof(MOA_elm) * rslt->elements_ub);
  l1_finished = 0;
  while (!l1_finished == 0) {
    current_idx = Gamma(ref_dim, MOA_1->dimn, rslt->shape, rslt->dimn, 1);
    if (ref_dim[Cat_DIM - 1] >= MOA_1->shape[Cat_DIM - 1]){
      ref_dim[Cat_DIM - 1] = ref_dim[Cat_DIM - 1] - MOA_1->shape[Cat_DIM - 1];
      ref_idx = Gamma(ref_dim, MOA_1->dimn, MOA_2->shape, MOA_2->dimn, 1);
      rslt->elements[current_idx] = MOA_2->elements[ref_idx];
      ref_dim[Cat_DIM - 1] = ref_dim[Cat_DIM - 1] + MOA_1->shape[Cat_DIM - 1];
    } 
    else {
      ref_idx = Gamma(ref_dim, MOA_1->dimn, MOA_1->shape, MOA_1->dimn, 1);
      rslt->elements[current_idx] = MOA_1->elements[ref_idx];
    }
    l1_finished = VecIsEqual(ref_dim, rslt->dimn, Last_ref_dimn, rslt->dimn);
    NextIndex(rslt->shape, rslt->dimn, ref_dim);
  }
  
  if (ref_dim != NULL)	
    free(ref_dim);
  if (Last_ref_dimn != NULL)	
    free(Last_ref_dimn);
  
}

/* ***************************************************************
// **********************    scalar_op    ************************
// ***************************************************************/

void scalar_op(char Op, MOA_rec * MOA_in, long scalar, MOA_rec * rslt)
{
  long i;

  rslt->dimn = MOA_in->dimn;
  if (rslt->dimn == 0) {
    rslt->shape = (long *) mmalloc (sizeof(long));
    rslt->shape[0] = MOA_in->shape[0];
  }
  else {
    rslt->shape = (long *) mmalloc (sizeof(long) * rslt->dimn);
    for (i = 0; i< rslt->dimn; i++) {
      rslt->shape[i] = MOA_in->shape[i];
    }
  }

  rslt->elements_ub = Tau(rslt->shape, rslt->dimn);
  rslt->elements = (MOA_elm *) mmalloc (sizeof(MOA_elm) * rslt->elements_ub);
  
  for (i = 0; i < rslt->elements_ub; i++) {
    switch (Op) {
    case '+':
      rslt->elements[i].val = MOA_in->elements[i].val + scalar;
      break;
    case '*':
      rslt->elements[i].val = MOA_in->elements[i].val * scalar;
      break;
    case '-':
      rslt->elements[i].val = MOA_in->elements[i].val - scalar;
      break;
    case '/':
      if (scalar == 0) 
	rslt->elements[i].val = MOA_in->elements[i].val;
      else
	rslt->elements[i].val = MOA_in->elements[i].val / scalar;
      
      break;
    case '<':
      if (MOA_in->elements[i].val < scalar) {
	rslt->elements[i].val = MOA_in->elements[i].val;
      }
      else {
	rslt->elements[i].val = scalar;
      }
      break;
    case '>':
      if (MOA_in->elements[i].val > scalar) {
	rslt->elements[i].val = MOA_in->elements[i].val;
      }
      else {
	rslt->elements[i].val = scalar;
      }
      break;
    default:
      rslt->elements[i].val = MOA_in->elements[i].val;
    }
  }
        
}

long getMaxOnLastBorder(MOA_rec * MOA_in, long * flatindex) {
  long i, j, maxValue, maxIndex;
  long * m_index = NULL; /* multidimensional index */
  
 
  maxIndex = -1;
  m_index = (long *)  mcalloc ((MOA_in->dimn), sizeof(long));
  
  maxValue = MOA_in->elements[0].val;
  for (i = 1; i< MOA_in->elements_ub; i++)  {
    maxValue = MOA_in->elements[i].val;
    Gamma_Inverse(i, MOA_in->shape, MOA_in->dimn, m_index);
    for (j = 0; j< MOA_in->dimn; j++) {
      if (m_index[j] == MOA_in->shape[j] - 1) {
	if (MOA_in->elements[i].val >= maxValue) {
	  maxValue = MOA_in->elements[i].val;
	  maxIndex = i;
	}
      }
    }
  }

  if (m_index != NULL)	
    free(m_index);
  (*flatindex) = maxIndex;
  return maxValue;
}

long MOA_max(MOA_rec *  MOA_in, int callFlag, long ubMaxVal, long * flatindex) {
  long i, maxValue, maxIndex;

  
  maxValue = MOA_in->elements[0].val;
  maxIndex = 0;
    
  for (i = 0; i< MOA_in->elements_ub; i++)  {
    if ((callFlag == 1) && (MOA_in->elements[i].val >= maxValue) && (MOA_in->elements[i].val < ubMaxVal)) {
      maxValue = MOA_in->elements[i].val;
      maxIndex = i;
    }
    else if ((callFlag == 0) && (MOA_in->elements[i].val >= maxValue)) {
      maxValue = MOA_in->elements[i].val;
      maxIndex = i;
    }
  }

  (*flatindex) = maxIndex;
  return maxValue;
}


/* ***************************************************************
// **********************    MOAGetLowerNeighbors ****************
// ***************************************************************/
int MOAGetLowerNeighbors (long * ind, MOA_rec * MOA_in, MOA_rec * rslt) {
  long * tmpInd1 = NULL;
  long * tmpInd2 = NULL;
  long * tmpInd3 = NULL;
  long i, idx;
  int l1_finished, ret;

  MOA_rec * rslt2;
  tmpInd1 =  (long *) mmalloc (sizeof(long) * MOA_in->dimn);
  tmpInd2 =  (long *) mmalloc (sizeof(long) * MOA_in->dimn);
  tmpInd3 =  (long *) mmalloc (sizeof(long) * MOA_in->dimn);
  /*rslt2 = (MOA_rec *) mmalloc(sizeof(MOA_rec));*/
  createMOAStruct (&rslt2);
  for (i = 0; i < MOA_in->dimn; i++) {
    tmpInd1[i] = 2;
    if (ind[i] > 0)
      tmpInd2[i] = ind[i] - 1;
    else
      return -1;
  }
 
  ret = Drop(tmpInd2, MOA_in->dimn, MOA_in, rslt2);  
  if (ret >= 0)
    ret = Take(tmpInd1, rslt2->dimn, rslt2, rslt);

  if (ret < 0) 
      return -1;
  
  for (i = 0; i < MOA_in->dimn; i++) {
    tmpInd1[i] = 0;
    tmpInd2[i] = 1;
    tmpInd3[i] = 2;
  }
  
    l1_finished = 0;
    i = 0;
    if (rslt->elements_ub > 0) {
      while (l1_finished == 0) {
	idx = Gamma(tmpInd1, rslt2->dimn, rslt2->shape, rslt2->dimn, 1);
	rslt->indexes[i] =  rslt2->indexes[idx];
	i++;
	l1_finished = VecIsEqual(tmpInd1, MOA_in->dimn, tmpInd2, MOA_in->dimn);
	NextIndex(tmpInd3, MOA_in->dimn, tmpInd1);
      }
    }
    

  if (tmpInd1 != NULL)
    free(tmpInd1);
  if (tmpInd2 != NULL)
    free(tmpInd2);
  if (tmpInd3 != NULL)
    free(tmpInd3);
  deleteMOA (rslt2);  
  return i-1;
}

/* ***************************************************************
// **********************    MOAGetHigherNeighbors ****************
// ***************************************************************/
int MOAGetHigherNeighbors (long stride, long * ind, MOA_rec * MOA_in, MOA_rec * rslt) {
  long * tmpInd1 = NULL;
  long * tmpInd2 = NULL;
  long * tmpInd3 = NULL;
  long i, idx, ret;
  int l1_finished;

  MOA_rec * rslt2;
  tmpInd1 =  (long *) mmalloc (sizeof(long) * MOA_in->dimn);
  tmpInd2 =  (long *) mmalloc (sizeof(long) * MOA_in->dimn);
  tmpInd3 =  (long *) mmalloc (sizeof(long) * MOA_in->dimn);
  /*rslt2 = (MOA_rec *) mmalloc (sizeof(MOA_rec));*/
  createMOAStruct (&rslt2);
  for (i = 0; i < MOA_in->dimn; i++) {

    if (ind[i] > MOA_in->shape[i] - 1)
      return -1;
    else {
      tmpInd1[i] = stride;
      tmpInd2[i] = ind[i];
    }
  }
 

  ret = DropInd(tmpInd2, MOA_in->dimn, MOA_in, rslt2);  
  if (ret >= 0)
    ret = TakeInd(tmpInd1, rslt2->dimn, rslt2, rslt);

  if (ret < 0) 
      return -1;
  
  for (i = 0; i < MOA_in->dimn; i++) {
    tmpInd1[i] = 0;
    tmpInd3[i] = rslt->shape[i];
    tmpInd2[i] = tmpInd3[i] - 1;
  }
  
  l1_finished = 0;
  i = 0;
  if (rslt->elements_ub > 0) {
    while (l1_finished == 0) {
      idx = Gamma(tmpInd1, rslt2->dimn, rslt2->shape, rslt2->dimn, 1);
      rslt->indexes[i] =  rslt2->indexes[idx];
      i++;
      l1_finished = VecIsEqual(tmpInd1, MOA_in->dimn, tmpInd2, MOA_in->dimn);
      NextIndex(tmpInd3, MOA_in->dimn, tmpInd1);
    }
  }
  
  if (tmpInd1 != NULL)	
    free(tmpInd1);
  if (tmpInd2 != NULL)	
    free(tmpInd2);
  if (tmpInd3 != NULL)	
    free(tmpInd3);
  deleteMOA (rslt2);  
  return i-1;
}


/* ***************************************************************
// **********************    getDiagonals ************************
// ***************************************************************/
void getDiagonals (long waveNo, long waveSize, long * ind, MOA_rec * MOA_in) {
  MOA_rec * MOA_nghb;
  long i, j, k, l, border, flatIndex; 
  int moreneighb;
  long * nghb_ind;
  long combnum; /* number of combinations for decrementing i number of indices*/
  long * * combin; /* combinations of lower neighbor indices*/

  createMOAStruct (&MOA_nghb);

  moreneighb = MOAGetHigherNeighbors (waveSize, ind, MOA_in, MOA_nghb);
  if (moreneighb <= 0)
    return;
  else {
    mprintf(outputfilename, "\nWave %d moreneighb %d MOA_nghb->elements_ub %d from point : ", 3, waveNo , moreneighb, MOA_nghb->elements_ub);
    for (j = 0; j <  MOA_in->dimn; j++) {
      mprintf(outputfilename, "%ld ", 1, ind[j]);
     
    }
    mprintf(outputfilename, " has %d points with Indices: ", 1, moreneighb);
    for (i=0;i<MOA_nghb->elements_ub; i++) {
      mprintf(outputfilename, "%ld ", 1, MOA_nghb->indexes[i]);    
      /*assign point to the next available processor*/
    }
    
    
    nghb_ind =  (long *) mmalloc (sizeof(long) * MOA_nghb->dimn);
      
    for (j = 1; j< MOA_nghb->dimn; j++)  {
      
      /* number of the different combinations of which indeces in the multidimensional index to give the shape bound so that all edge points can be processed */
      combnum = (Factorial(MOA_nghb->dimn)/(Factorial(MOA_nghb->dimn-j) * Factorial(j)));
      /* create memory for combinations matrix */
      combin = (long * *) mcalloc (combnum, sizeof(long *));
      for (k=0; k<combnum; k++)
	combin[k] = (long *) mcalloc (j, sizeof(long));
      if (combin == NULL) {
	printf ("Can not allocate memory to combinations matrix\n");
	return;
      }
      /* get matrix of all possible combinations */
      Combinations(MOA_nghb->dimn, j, &combin);
      /* loop to decrement the selected indices */
      for (k=0; k<combnum; k++) {
	for (l = 0; l <MOA_nghb->dimn; l++) {
	  nghb_ind[l] = 0;
	}
	for (l=0; l<j; l++) {
	  nghb_ind[combin[k][l]-1] = MOA_nghb->shape[combin[k][l]-1] - 1;
	}
	flatIndex = Gamma( nghb_ind, MOA_nghb->dimn,  MOA_nghb->shape, MOA_nghb->dimn, 1);
	
	Gamma_Inverse(MOA_nghb->indexes[flatIndex], MOA_in->shape, MOA_in->dimn, ind);
	
	border = 1;
	for (l=0; l<MOA_in->dimn; l++) {
	  if (ind[l] >= MOA_in->shape[l] - 1) {
	    border = 0;
	  }
	}
	if (border == 1) {
	  getDiagonals (waveNo + 1, waveSize, ind, MOA_in);
	  //MPI_Isend (&myProcid, 1, MPI_INT, i, tag, MPI_COMM_WORLD, &request);
	}
      }
    }
    /* do the final edge where all at the shape limit*/
    for (l = 0; l <MOA_nghb->dimn; l++) {
      nghb_ind[l] = MOA_nghb->shape[l] - 1;
    }
    flatIndex = Gamma( nghb_ind, MOA_nghb->dimn,  MOA_nghb->shape, MOA_nghb->dimn, 1);
    
    Gamma_Inverse(MOA_nghb->indexes[flatIndex], MOA_in->shape, MOA_in->dimn, ind);
    
    border = 1;
    for (l=0; l<MOA_in->dimn; l++) {
      if (ind[l] >= MOA_in->shape[l] - 1) {
	border = 0;
      }
    }
    if (border == 1) {
      getDiagonals (waveNo + 1, waveSize, ind, MOA_in);
    }
    free(nghb_ind);
  }
  deleteMOA (MOA_nghb);  
}

/* ***************************************************************
// **********************    Navigate ****************************
// ***************************************************************/
void GetNextBorderCell (long * * ind,  MOA_rec * MOA_in) {
  
}

void Navigate (long * NavDir, long stride, long * stIndex, MOA_rec * MOA_in, long * * rslt, long * * rsltInd, long * rslt_ub) {
  long * ind = NULL;
  long i, count, rounds, error;
  int border, done;
  border = done = rounds = 0;

  ind =  (long *) mmalloc (sizeof(long) * MOA_in->dimn);
  count = 0;

  (*rslt) =  (long *) mmalloc (sizeof(long));
  (*rsltInd) =  (long *) mmalloc (sizeof(long));
  (*rsltInd)[count] = Gamma(stIndex, MOA_in->dimn, MOA_in->shape, MOA_in->dimn, 1);
  (*rslt)[count] = MOA_in->elements[(*rsltInd)[count]].val;
  for (i=0; i<MOA_in->dimn; i++) {
    ind[i] = stIndex[i] + NavDir[i];
    if ((ind[i] < 0) || (ind[i] >= MOA_in->shape[i]))
      border = 1;
  }
  while (done == 0) {
    while (border == 0) {
      count ++;    
      (*rslt) =  (long *) realloc ((*rslt), sizeof(long) * (count+1));
     if ((*rslt) == NULL) {
		mprintf(outputfilename, "Could not reallocate memory!\n", 0);
		return;
      }
      (*rsltInd) =  (long *) realloc ((*rsltInd), sizeof(long) * (count+1));
     if ((*rsltInd) == NULL) {
		mprintf(outputfilename, "Could not reallocate memory!\n", 0);
		return;
      }
      
      (*rsltInd)[count] = Gamma(ind, MOA_in->dimn, MOA_in->shape, MOA_in->dimn, 1);
      (*rslt)[count] = MOA_in->elements[(*rsltInd)[count]].val;
      
      for (i=0; i<MOA_in->dimn; i++) {
	ind[i] = ind[i] + NavDir[i];
	if ((ind[i] < 0) || (ind[i] >= MOA_in->shape[i]))
	  border = 1;
      }
    }
    border = 0;
    rounds ++;
    GetNextBorderCell(&ind, MOA_in);
    error = 0;
    for (i=0; i<MOA_in->dimn; i++) {
      if (ind[i] >= MOA_in->shape[i]) {
	done = 1;
	error = 1;
      }
      if (ind[i] >= MOA_in->shape[i] - 1)
	done = 1;
      NavDir[i] = NavDir[i] * (-1);
    }
    if (rounds == stride)  
	done = 1;
    if (error == 0) {
      count ++;    
      (*rslt) =  (long *) realloc ((*rslt), sizeof(long) * (count+1));
     if ((*rslt) == NULL) {
		mprintf(outputfilename, "Could not reallocate memory!\n", 0);
		return;
      }
      (*rsltInd) =  (long *) realloc ((*rsltInd), sizeof(long) * (count+1));
     if ((*rsltInd) == NULL) {
		mprintf(outputfilename, "Could not reallocate memory!\n", 0);
		return;
      }
      (*rsltInd)[count] = Gamma(ind, MOA_in->dimn, MOA_in->shape, MOA_in->dimn, 1);
      (*rslt)[count] = MOA_in->elements[(*rsltInd)[count]].val;
    }
  }
  (*rslt_ub) = count + 1;
  if (ind != NULL)	
    free(ind);
}
