#include <iostream>
#include <mpi.h>
#include <omp.h>
#include <ctime>

using namespace std;

int main(int argc, char *argv[]) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    srand(time(NULL) + rank);

    int choice;
    int opponent_choice;

    if (rank == 0) {
        cout << "Escolha sua jogada: (0 para Pedra, 1 para Papel, 2 para Tesoura)" << endl;
        cin >> choice;
    }

    MPI_Bcast(&choice, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int opponent_rank = (rank + 1) % size;
    MPI_Recv(&opponent_choice, 1, MPI_INT, opponent_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    if (rank != 0) {
        choice = rand() % 3; // Jogada aleatória do oponente
        cout << "Processo " << rank << " escolheu: " << choice << endl;
    }

    MPI_Send(&choice, 1, MPI_INT, opponent_rank, 0, MPI_COMM_WORLD);

    int result;
    if (choice == opponent_choice)
        result = 0; // Empate
    else if ((choice == 0 && opponent_choice == 2) || 
             (choice == 1 && opponent_choice == 0) || 
             (choice == 2 && opponent_choice == 1))
        result = 1; // Vitória
    else
        result = -1; // Derrota

    int *results = new int[size];
    MPI_Gather(&result, 1, MPI_INT, results, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        int wins = 0, draws = 0, losses = 0;
        for (int i = 0; i < size; ++i) {
            if (results[i] == 1)
                ++wins;
            else if (results[i] == 0)
                ++draws;
            else
                ++losses;
        }

        cout << "Resultados:" << endl;
        cout << "Vitórias: " << wins << endl;
        cout << "Empates: " << draws << endl;
        cout << "Derrotas: " << losses << endl;
    }

    delete[] results;

    MPI_Finalize();
    return 0;
}
