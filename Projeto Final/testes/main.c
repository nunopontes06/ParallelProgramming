#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

/* Aux fcts ------------------------------------------------------- */
int * allocVector(int n)
{
  return (int *)malloc(n*sizeof(int));
}

int ** allocMatrix(int m, int n)
{
  int ** newM = (int **)malloc(m*sizeof(int *));
  int i;
  for (i = 0; i < m; i++)
    newM[i] = allocVector(n);
  return newM;
}

void readMatrix(FILE * fin, int ** M, int m, int n)
{
  int i, j;
  for (i = 0; i < m; i++)
    for (j = 0; j < n; j++)
      fscanf(fin, "%d", &(M[i][j]));
}

void writeMatrix(FILE * fout, int ** M, int m, int n)
{
  int i, j;
  for (i = 0; i < m; i++) {
    for (j = 0; j < n; j++)
      fprintf(fout, "%d ", M[i][j]);
    putc('\n', fout);
  }
}


/* Aux fct: transpose m*n matrix into n*m matrix */
int ** transpose(int ** M, int m, int n)
{
  int ** MT;
  int i, j;
  MT = allocMatrix(n, m);
  for (i = 0; i < m; i++)
    for (j = 0; j < n; j++)
      MT[j][i] = M[i][j];
  return MT;
}


/* Aux fct: dot product of two vectors of length n */
int dotProd(int * V1, int * V2, int n)
{
  int dp = 0;
  int i;
  for (i = 0; i < n; i++)
    dp = dp + V1[i] * V2[i];
  return dp;
}

/* Master: Spread mult M1*M2 (a m*m matrix) over m workers; each computes 1 row */
int ** matrixProdMaster(int ** M1, int ** M2T, int m, int n)
{
  int i, j;
  MPI_Status status;

  int ** M3 = allocMatrix(m,m);

  for (j = 0; j < m; j++) {
    for (i = 0; i < m; i++) {
      MPI_Send(M1[i], n, MPI_INT, i+1, 0, MPI_COMM_WORLD);
      MPI_Send(M2T[j], n, MPI_INT, i+1, 0, MPI_COMM_WORLD);
    }

    for (i = 0; i < m; i++)
      MPI_Recv(&(M3[i][j]), 1, MPI_INT, i+1, 0, MPI_COMM_WORLD, &status);
  }
  return M3;
}


/* Worker: Compute 1 row of resulting m*m matrix */
void matrixProdWorker(int m, int n)
{
  MPI_Status status;
  int dp, i;
  int * R = allocVector(n);
  int * C = allocVector(n);

  for (i = 0; i < m; i++) {
    MPI_Recv(R, n, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    MPI_Recv(C, n, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    dp = dotProd(R, C, n);
    MPI_Send(&dp, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
  }
}
