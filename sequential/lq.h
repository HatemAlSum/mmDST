#ifndef LQH_
#define LQH_
#include "moa.h"

struct qnode
{
  int data;
  struct qnode *next;
};
struct qnode * processors;
int myProcid;/*My Process ID*/
int ClsuterSize;/*size of computing cluster*/

void enqueue(struct qnode **first,int n);
int dequeue(struct qnode **first);
void displayq(struct qnode *p);
void freeq (struct qnode  **first); 

void Diagonalize (MOA_rec * MOA_in);
int recivMOA (MOA_rec * MOA_in);
void sendMOA (int ProcDest, MOA_rec * MOA_in);
void sendMOA_Cont ();
void MasterProcess ();
void SlaveProcess ();
void communicate ();


#endif
