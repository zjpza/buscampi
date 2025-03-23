//MARCOS ARAMBASIC REBELO DA SILVA - 10443260

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#define VECTOR_SIZE 65536 // 2^16

void load_vector(const char *filename, int *vector) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Erro ao abrir o arquivo %s\n", filename);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    for (int i = 0; i < VECTOR_SIZE; i++) {
        fscanf(file, "%d", &vector[i]);
    }
    fclose(file);
}

int main(int argc, char *argv[]) {
    int rank, size;
    int vector[VECTOR_SIZE];
    int local_vector[VECTOR_SIZE];
    int target = -1;
    int local_size;
    int found_index = -1, global_index = -1;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    local_size = VECTOR_SIZE / size;

    if (rank == 0) {
        load_vector("vetor1.csv", vector);
        target = vector[VECTOR_SIZE / 2];
    }
    
    MPI_Bcast(&target, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(vector, local_size, MPI_INT, local_vector, local_size, MPI_INT, 0, MPI_COMM_WORLD);
    
    for (int i = 0; i < local_size; i++) {
        if (local_vector[i] == target) {
            found_index = rank * local_size + i;
            break;
        }
    }
    
    MPI_Reduce(&found_index, &global_index, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        if (global_index != -1)
            printf("Elemento %d encontrado na posição %d.\n", target, global_index);
        else
            printf("Elemento não encontrado.\n");
    }

    MPI_Finalize();
    return 0;
}
