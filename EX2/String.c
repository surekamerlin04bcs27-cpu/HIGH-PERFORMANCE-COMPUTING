#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <mpi.h>
#include<time.h>
int isPalindrome(char str[])
{
    int i, len = strlen(str);
    for (i = 0; i < len / 2; i++)
    {
        if (str[i] != str[len - i - 1])
            return 0;
    }
    return 1;
}

int main(int argc, char *argv[])
{
    int rank, size;
    char msg[100];
    clock_t start_time, end_time;
    float exec_time;
    MPI_Init(&argc, &argv);                  // Start MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);    // Get rank
    MPI_Comm_size(MPI_COMM_WORLD, &size);    // Get number of processes

    if (rank == 0)   // MASTER PROCESS
    {
        MPI_Status status;

        for (int i = 1; i < size; i++)
        {
            MPI_Recv(msg, 100, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG,
                     MPI_COMM_WORLD, &status);

            if (status.MPI_TAG == 1)   // Type 1 message
            {
                for (int j = 0; msg[j]; j++)
                    msg[j] = toupper(msg[j]);

                printf("From rank %d | Type 1 | Uppercase: %s\n",status.MPI_SOURCE, msg);
            }
            else if (status.MPI_TAG == 2)   // Type 2 message
            {
                if (isPalindrome(msg))
                    printf("From rank %d | Type 2 | Palindrome: YES (%s)\n",status.MPI_SOURCE,msg);
                else
                    printf("From rank %d | Type 2 | Palindrome: NO (%s)\n",status.MPI_SOURCE,msg);
            }
        }
    }
    else   // WORKER PROCESSES
    {
            if(rank == 1)
            {strcpy(msg,"hello");
            }
            else if (rank == 2){
                    strcpy(msg,"madam");}
            else if(rank == 3){
                    strcpy(msg,"mepco");}
            else if(rank == 4){
                    strcpy(msg,"malayalam");
            }
            else if(rank == 5){
                    strcpy(msg,"sureka");}
            else if(rank == 6){
                    strcpy(msg,"computer");}
            start_time = clock();

        if (rank % 2 == 1)   // Odd rank → Type 1
        {
           // strcpy(msg, "hello");
            MPI_Send(msg, strlen(msg) + 1, MPI_CHAR,
                     0, 1, MPI_COMM_WORLD);
        }
        else   // Even rank → Type 2
        {
            //strcpy(msg, "madam");
            MPI_Send(msg, strlen(msg) + 1, MPI_CHAR,
                     0, 2, MPI_COMM_WORLD);
        }
    }
    end_time = clock();
        exec_time = ((double)(end_time - start_time) / CLOCKS_PER_SEC);
        printf("Execution time of process %d :  %lf\n",rank,exec_time);

    MPI_Finalize();   // End MPI
    return 0;
}
