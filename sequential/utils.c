#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "utils.h"
#include "moa.h"
#include "moamsa.h"
#include <time.h>


struct tm * getTime () {
  time_t ltime;
  struct tm *m_now;
  
  time( &ltime );
  
  /*printf( "UNIX time and date:\t\t\t%s", _ctime64( &ltime ) );*/
  
  /* Convert to time structure and adjust for PM if necessary. */
  m_now = localtime( &ltime );
  
  if( m_now->tm_hour == 0 )  /* Adjust if midnight hour. */
    m_now->tm_hour = 12;
  return m_now;
}

int mprintf(const char * stream, const char *template, int count, ...){
  FILE * outfile;
  
  va_list margs;
  va_start (margs, count);         /* Initialize the argument list. */
  /*if (stream == NULL)
    stream = "moamsa.out";
  /*printf("%s \n", stream); */
  /*if (( outfile= fopen (stream, "a")) == NULL) {
    printf("Can not Open output file, exiting.\n");
    return -1;
  }*/
  /*vfprintf(outfile, template, margs); */
  /*fclose(outfile);*/
  vprintf(template, margs);
  va_end (margs);                  /* Clean up. */
  return 0;
}

void * mmalloc(size_t size) {
	void * alloc_mem = NULL;
	alloc_mem = (long *) malloc (size);
    if (alloc_mem == NULL ) {
		mprintf(outputfilename, "mmalloc: Can not allocate memory  with size %d \n", 1, size);
	  return NULL;
    }
	return alloc_mem;
}

void * mcalloc(size_t num, size_t size) {
	void * alloc_mem = NULL;
	alloc_mem = (long *) calloc (num, size);
    if (alloc_mem == NULL ) {
		mprintf(outputfilename, "mcalloc: Can not allocate memory with num %d and size %d \n", 2, num, size);
	  return NULL;
    }
	return alloc_mem;
}




/*
  reinventing the wheel? probably.. :)
  this code is in the public domain
  application of shell-swap algorithm avoids inefficiency of
  multiple strlen invocations. ;)
*/

char* strrev(char *pB)
{
  char *pE = pB;
  char *s = pB;

  /* avoid trouble */
  if(!pB || !*pB) return s;

  /* start looking for the null, then backtrack to last char */
  while (*++pE);
  pE--;

  /* repeat until begin/end pointers meet */
  while (pB < pE)
  {
    /* swap begin+n and end-n chars */
    char hold = *pB;
    /* gratuitous use of postfix increment with dereference */ 
    *pB++ = *pE;
    *pE-- = hold;
  }
  return s;
}



long mpow (long x, long y) {
  long powerV, i;
  powerV = 1;
  for (i=1;i<=y;i++) {
    powerV=powerV*x;
  }

  return powerV;
}
long  a_max(long * values, long ubound) {
  long maxVal, i;
  
  maxVal = values[0];
  for (i=1;i<ubound;i++) {
    if (maxVal < values[i])
      maxVal = values[i];
  }
  return maxVal;
}
long  a_min(long * values, long ubound) {
  long minVal, i;
  
  minVal = values[0];
  for (i=1;i<ubound;i++) {
    if (minVal > values[i])
      minVal = values[i];
  }
  return minVal;
}

long Factorial (long n) {
  if (n==0)
    return 1;
  else
    return (n * Factorial(n-1));
}

void l2Comb(long * n, long k, long * * * Combin)
{
  int i, j, row, column;

  row = 0;
  column = 0;
  for (i=0;i<k;i++){
    for (j=i+1;j<k;j++){
      (*Combin)[row][column] = n[i];
      column++;
      (*Combin)[row][column] = n[j];
      row++;
      column = 0;
    }
  }
}
/* max val in 3 values 
int max (int val1, int val2, int val3) {
  int maxVal = 0;
  if (val1  > maxVal)
    maxVal  = val1;
  if (val2  > maxVal)
    maxVal  = val2;
  if (val3  > maxVal)
    maxVal  = val3;
  
  return maxVal;
}
*/
/*max value from a linear array */
int maxVal (int * values, int valLen) {
  int i, maxVal = 0;
  for (i=0; i < valLen; i++)
    if (values[i] > maxVal)
      maxVal  = values[i];
  
  return maxVal;
}
/* Algorithm by Donald Knuth. */
/* C implementation by Glenn C. Rhoads */
void Combinations(long n, long k, long * * * Combin)
{
    long i, j=1, *c, x, row, column;
    c = mmalloc( (k+3) * sizeof(long));
    
    for (i=1; i <= k; i++) c[i] = i;
    c[k+1] = n+1;
    c[k+2] = 0;
    j = k;
    row = 0;
    column = 0;

visit:
    for (i=k; i >= 1; i--) {
      (*Combin)[row][column] = c[i];
      column++;
    }
    column = 0;
    row++;

    if (j > 0) {x = j+1; goto incr;}

    if (c[1] + 1 < c[2])
       {
       c[1] += 1;
       goto visit;
       }

    j = 2;

 do_more:
    c[j-1] = j-1;
    x = c[j] + 1;
    if (x == c[j+1]) {j++; goto do_more;}

    if (j > k) {
      return;
    }

 incr:
    c[j] = x;
    j--;
    goto visit;
}
