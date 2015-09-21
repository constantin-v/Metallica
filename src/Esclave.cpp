#include <mpi.h>
#include <stdio.h>

int main( int argc, char *argv[] )
{
	int myrank;
	float temperature;
	MPI_Comm parent;
	MPI_Status etat;
	MPI_Init (&argc, &argv);
	MPI_Comm_get_parent (&parent);
	MPI_Comm_rank (MPI_COMM_WORLD,&myrank);

	if (parent == MPI_COMM_NULL) {
		printf ("Esclave %d : Pas de pere !\n", myrank);
	} else {
		MPI_Recv(&temperature, 1, MPI_FLOAT, 0, 0, parent, &etat);
		printf ("Esclave %d : Reception de la temperature %f !\n", myrank, temperature);
		char response = 'K';
		MPI_Send(&response, 1, MPI_CHAR, 0, 0, parent);
		printf ("Esclave %d : Envoi vers le pere !\n", myrank);
	}

printf ("Esclave %d : FIN !\n", myrank);
	MPI_Finalize();
	return 0;
}
