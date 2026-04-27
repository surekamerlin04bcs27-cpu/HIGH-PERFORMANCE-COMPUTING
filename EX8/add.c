#include<stdio.h>
#include<stdlib.h>
#include<omp.h>
#include<time.h>
int main(int argc,char* argv[])
{
   int i,n=100000;
   int *A,*B,*C;
   double start,end;
   A=(int*)malloc(n*sizeof(int));
   B=(int*)malloc(n*sizeof(int));
   C=(int*)malloc(n*sizeof(int));
   srand(time(0));
   int thread_count = atoi(argv[1]);
   for( i=0;i<n;i++)
   {
      A[i]=rand()%100;
      B[i]=rand()%100;
   }
   start=omp_get_wtime();
#pragma omp parallel for num_threads(thread_count)
   for( i=0;i<n;i++)
   {
      C[i]=A[i]+B[i];
   }
   end=omp_get_wtime();
   printf("The time taken is :%lf\n",end-start);
   printf("Printing few data...\n");
   for(i=0;i<10;i++){
      printf("%d+%d=%d\n",A[i],B[i],C[i]);
   }
   free(A);
   free(B);
   free(C);
   return 0;
}
