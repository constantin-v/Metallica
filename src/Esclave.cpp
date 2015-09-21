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
        printf ("Esclave n°%d : Reception de la temperature case (%f°C) de la part du maitre !\n", myrank, temperature);

        for(int i=1; i< 10; i++){
        	MPI_Recv(&temperature, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, &etat);
            printf ("Esclave n°%d : Reception de la temperature ambiante (%f°C) de la part du coordinateur !\n", myrank, temperature);

            MPI_Send(&temperature, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
            printf ("Esclave n°%d : Envoi de la temperature ambiante (%f°C) au coordinateur !\n", myrank, temperature);
        }
	}

	printf ("Esclave n°%d : FIN !\n", myrank);

	MPI_Finalize();
	return 0;
}
