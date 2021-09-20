#include<stdio.h>
#include<mpi.h>

double find_sum(double* vector, int N) {
	double sum = 0;
	for (int i = 0; i < N; i++) {
		sum += vector[i];
	}
	return sum;
}

    double find_maximum (double*vector, int N){
        double max = 0;
        for(int i=0; i<N; i++){
            if(vector[i]>max){
                max = vector[i];
            }
        }
            return max;
    }


int main(int argc, char** argv) {

	int n_numbers = 6;
	int rank;
	double vector[n_numbers];
	double sum, max;
	double my_first_number;

    printf("\nTRABALHO 6: ALGORTIMO - MULTIPLICACAO DE MATRIZES\n");
    printf("\nNUNO PONTES E RAFAEL MAGALHAES\n");
    printf("\nUNIVERSIDADE LUSOFONA DO PORTO\n");

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	my_first_number = n_numbers * rank;

	for (int i = 0; i < n_numbers; i++) {
		vector[i] = my_first_number + i;
	}

	sum = find_sum(vector, n_numbers);
	printf("\nThe sum of the numbers is %f\n", sum);

    max = find_maximum(vector, n_numbers);
	printf("\nThe max of the numbers is %f\n", max);

	return MPI_Finalize();

}
