#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"
int main(int argc, char *argv[]) {

    int  *data , *masterData , localSize  ;
    double mean , variance , standardDeviation , masterCount =0 ,otherCount , localCount = 0  , globalSum = 0 , temp , temp2 ;
    int numOfProcesses, rank ,arraySize,remain , i , j,chunk,chunk_remaining , sumOfdata =0    ;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numOfProcesses);

    if(rank == 0){

    printf("Enter the size of the data  \n");
    scanf("%d", &arraySize );
    chunk = arraySize / numOfProcesses ;
    chunk_remaining = arraySize % numOfProcesses ;
    localSize = arraySize - chunk_remaining ;

    if (chunk_remaining>0){
         data = (int*) malloc(sizeof(int) * (localSize) );
         masterData = (int*) malloc(sizeof(int) * (chunk_remaining) );
          j=0 ;
         for ( i=0 ; i<arraySize ; i++){
            if (i<localSize){
                scanf("%d", &data[i] );
                sumOfdata+= data[i];
            }
            else {
                scanf("%d", &masterData[j] );
                sumOfdata+= masterData[j];
                j++;
              }
            }
         }
   else{
        data = (int*) malloc(sizeof(int) * arraySize);
        for ( i=0 ; i<arraySize ; i++  ){
            scanf("%d", &data[i] );
            sumOfdata+= data[i];
        }
   }
        mean = sumOfdata / arraySize ;
        if (chunk_remaining>0){
        for ( i=0; i<chunk_remaining ; i++){
        temp = pow((masterData[i] - mean) , 2 );
        masterCount+=temp;
           }
        }
        else{masterCount=0;}
 }
    int *localData = (int*) malloc(sizeof(int) * chunk);

    MPI_Bcast(&mean , 1 , MPI_DOUBLE , 0 , MPI_COMM_WORLD);
    MPI_Bcast(&chunk , 1 , MPI_INT , 0 , MPI_COMM_WORLD);
    MPI_Scatter(data , chunk , MPI_INT , localData , chunk , MPI_INT , 0 , MPI_COMM_WORLD);

    for ( i=0; i<chunk ; i++){
        temp = pow((localData[i] - mean) , 2 );
        localCount+=temp;
        }

     MPI_Reduce(&localCount, &otherCount, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
     if (rank == 0) {

       printf("The mean is %lf  \n" , mean) ;
       globalSum = otherCount + masterCount ;
       variance = globalSum / ( arraySize) ;

       printf("The variance is %lf  \n" , variance) ;
       standardDeviation = sqrt(variance);
       printf("The standardDeviation is %lf  \n" , standardDeviation) ;
       free(data);
  }
    free(localData);
    MPI_Finalize();
}
