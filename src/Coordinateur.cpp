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
		printf ("Coordinateur : Pas de pere !\n");
	} else {
		MPI_Recv(&temperature, 1, MPI_FLOAT, 0, 0, parent, &etat);
		printf ("Coordinateur : Reception de la temperature ambiante : %f°C !\n", temperature);

		for (int i=1; i<10; i++)	{
			for (int j=1; j<10; j++)	{
		        //float temperatureToSend = temperature;
				printf ("Coordinateur : Envoi vers l'esclave n°%d de la temperature ambiante (%f°C).\n", i, temperature);
				MPI_Send (&temperature, 1, MPI_FLOAT,j, 0, MPI_COMM_WORLD);

				MPI_Recv(&temperature, 1, MPI_FLOAT,j, 0, MPI_COMM_WORLD, &etat);
				printf ("Coordinateur : Reception de l'esclave n°%d: %f°C \n", i, temperature);
			}

		}

		char response = 'K';
		MPI_Send(&response, 1, MPI_CHAR, 0, 0, parent);
		printf ("Coordinateur : Envoi vers le pere !\n");
	}

	printf ("Coordinateur : FIN !\n");

	MPI_Finalize();
	return 0;
}
