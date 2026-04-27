#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#define MAX_NAME 50
typedef struct {
    char name[MAX_NAME];
    int rollno;
    float totalmarks;
    char grade;
} Student;
int main(int argc, char* argv[]) {
    int rank, size, totalStudents, studentsPerProcess, i;
    Student *allStudents = NULL;
    Student *localStudents = NULL;
    MPI_Datatype studentType;
    FILE *fp;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int blocklengths[4] = {MAX_NAME, 1, 1, 1};
    MPI_Aint displacements[4];
    MPI_Datatype types[4] = {MPI_CHAR, MPI_INT, MPI_FLOAT, MPI_CHAR};
    Student sample;
    MPI_Aint base;
    MPI_Get_address(&sample, &base);
    MPI_Get_address(&sample.name, &displacements[0]);
    MPI_Get_address(&sample.rollno, &displacements[1]);
    MPI_Get_address(&sample.totalmarks, &displacements[2]);
    MPI_Get_address(&sample.grade, &displacements[3]);
    displacements[0] -= base;
    displacements[1] -= base;
    displacements[2] -= base;
    displacements[3] -= base;
    MPI_Type_create_struct(4, blocklengths, displacements, types, &studentType);
    MPI_Type_commit(&studentType);
    // Root process reads input file
    if (rank == 0) {
        printf("\n=== MPI Student Grade Processing ===\n");
        printf("Number of processes: %d\n\n", size);

        fp = fopen("students_input.txt", "r");
        if (fp == NULL) {
            printf("Error: Cannot open input file!\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        fscanf(fp, "%d", &totalStudents);
        allStudents = (Student*)malloc(totalStudents * sizeof(Student));

        printf("Reading %d students from file...\n\n", totalStudents);

        for (i = 0; i < totalStudents; i++) {
            fscanf(fp, "%s %d %f", allStudents[i].name,
                   &allStudents[i].rollno, &allStudents[i].totalmarks);
            allStudents[i].grade = ' ';
        }
        fclose(fp);
        // Display input data
        printf("INPUT DATA:\n");
        printf("%-12s %-8s %-10s\n", "Name", "RollNo", "Marks");
        printf("--------------------------------\n");
        for (i = 0; i < totalStudents; i++) {
            printf("%-12s %-8d %-10.2f\n", allStudents[i].name,
                   allStudents[i].rollno, allStudents[i].totalmarks);
        }
        printf("\n");

        studentsPerProcess = totalStudents / size;
    }
    // Broadcast counts
    MPI_Bcast(&totalStudents, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&studentsPerProcess, 1, MPI_INT, 0, MPI_COMM_WORLD);
    localStudents = (Student*)malloc(studentsPerProcess * sizeof(Student));
    // Scatter data to all processes
    MPI_Scatter(allStudents, studentsPerProcess, studentType,
                localStudents, studentsPerProcess, studentType,
                0, MPI_COMM_WORLD);
    // Each process calculates grades
    printf("Process %d processing %d students:\n", rank, studentsPerProcess);
    for (i = 0; i < studentsPerProcess; i++) {
        // Calculate grade
        if (localStudents[i].totalmarks >= 90) localStudents[i].grade = 'A';
        else if (localStudents[i].totalmarks >= 80) localStudents[i].grade = 'B';
        else if (localStudents[i].totalmarks >= 70) localStudents[i].grade = 'C';
        else if (localStudents[i].totalmarks >= 60) localStudents[i].grade = 'D';
        else if (localStudents[i].totalmarks >= 50) localStudents[i].grade = 'E';
        else localStudents[i].grade = 'F';

        printf("  %s -> Grade: %c\n", localStudents[i].name, localStudents[i].grade);
    }
    // Gather results back to root
    MPI_Gather(localStudents, studentsPerProcess, studentType,
               allStudents, studentsPerProcess, studentType,
               0, MPI_COMM_WORLD);
    // Root writes output file and displays results
    if (rank == 0) {
        printf("\n=== FINAL RESULTS ===\n");
        printf("%-12s %-8s %-10s %-6s\n", "Name", "RollNo", "Marks", "Grade");
        printf("--------------------------------------\n");
        for (i = 0; i < totalStudents; i++) {
            printf("%-12s %-8d %-10.2f %-6c\n", allStudents[i].name,
                   allStudents[i].rollno, allStudents[i].totalmarks,
                   allStudents[i].grade);
        }
        // Write to output file
        fp = fopen("students_output.txt", "w");
        fprintf(fp, "STUDENT GRADE REPORT\n");
        fprintf(fp, "====================\n");
        fprintf(fp, "%-12s %-8s %-10s %-6s\n", "Name", "RollNo", "Marks", "Grade");
        fprintf(fp, "--------------------------------------\n");
        for (i = 0; i < totalStudents; i++) {
            fprintf(fp, "%-12s %-8d %-10.2f %-6c\n", allStudents[i].name,
                    allStudents[i].rollno, allStudents[i].totalmarks,
                    allStudents[i].grade);
        }
        fclose(fp);
        printf("\nResults written to 'students_output.txt'\n");
        free(allStudents);
    }
    free(localStudents);
    MPI_Type_free(&studentType);
    MPI_Finalize();
    return 0;
}
