#ifndef MOAH_
#define MOAH_


typedef struct tagMOA_elm {
  int flag; /* to indicate whether it is calculated or not yet: 1 = calculated, 0 = not yet*/
  long val;
  unsigned long prev_ub;
  unsigned long * prev;
} MOA_elm ;

typedef struct tagMOA_rec {
  long dimn;
  long * shape;
  unsigned long elements_ub;
  unsigned long * indexes;	
  int * proc; /* the processor ID doing the calculation for elements*/
  int * dproc_ub; /*No. of dependant processors*/
  int * * dproc; /*  the dependant processor ID wiaitng for the score of this cell, the value should be sent to it after being calculated; it is the same as proc, in case it is an internal cell inside the chunk done by a processor, and not a border one - can be a list of processors for corner cells*/
  MOA_elm * elements;	
} MOA_rec ;

typedef struct {
  MOA_rec * msaAlgn;
  int empty;
  pthread_mutex_t *mut;
  pthread_cond_t *notEmpty;
} Master;


/* Global Identitifer */


void createMOAStruct(MOA_rec * * MOA_val);
void createMOA(long * shape, long dimn, MOA_rec * MOA_val, int callFlag, int cellValue);
void printMOA (MOA_rec * MOA);
void deleteMOA(MOA_rec * MOA);
void deleteMOA(MOA_rec * MOA);
long Tau (long * array_in, long array_ub);
long Gamma (long * ind, long ind_ub, long * arr_shape, long shape_ub, int Front);
void Gamma_Inverse (long ind, long * arr_shape, long shape_ub, long * rslt);
void Psi (long * ind, long ind_ub, MOA_rec * MOA_in, MOA_rec * rslt);
int VecIsEqual (long * Array_1, long array1_ub, long * Array_2, long array2_ub);int IsValidIndex (long * shape, long shape_ub, long * index);

void NextIndex (long * shape, long shape_ub, long * Prev_Index);
int Take (long * ind, long ind_ub, MOA_rec * MOA_in, MOA_rec  * rslt);
int Drop (long * ind, long ind_ub, MOA_rec * MOA_in, MOA_rec *  rslt);
void Reshape (long * new_shape, long  n_shape_ub, MOA_rec * MOA_in, MOA_rec * rslt);
void Catenate (MOA_rec * MOA_1, MOA_rec * MOA_2, long  Cat_DIM, MOA_rec * rslt);void scalar_op(char Op, MOA_rec * MOA_in, long scalar, MOA_rec * rslt);
long getMaxOnLastBorder(MOA_rec * MOA_in, long * flatindex);
long MOA_max(MOA_rec *  MOA_in, int callFlag, long ubMaxVal, long * flatindex);
int MOAGetLowerNeighbors (long * ind, MOA_rec * MOA_in, MOA_rec * rslt);
int MOAGetHigherNeighbors (long stride, long * ind, MOA_rec * MOA_in, MOA_rec * rslt);
void getDiagonals (long waveNo, long waveSize, long * ind, MOA_rec * MOA_in);
void DistributeDiagonals (Master * master, long waveNo, long waveSize, long * ind, MOA_rec * MOA_in, int currProcessor);
void GetNextBorderCell (long * * ind,  MOA_rec * MOA_in);
void Navigate (long * NavDir, long stride, long * stIndex, MOA_rec * MOA_in, long * * rslt, long * * rsltInd, long * rslt_ub);
void Navigate (long * NavDir, long stride, long * stIndex, MOA_rec * MOA_in, long * * rslt, long * * rsltInd, long * rslt_ub);
#endif
