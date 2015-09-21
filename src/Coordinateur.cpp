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
		printf ("Coordinateur %d : Pas de pere !\n", myrank);
	} else {
		MPI_Recv(&temperature, 1, MPI_FLOAT, 0, 0, parent, &etat);
		printf ("Coordinateur %d : Reception de la temperature ambiante %f !\n", myrank, temperature);
		
		for (int i=1; i<10; i++)	{

	        float temperatureToSend = temperature;        

			MPI_Send (&temperatureToSend, 1, MPI_FLOAT, i, 0, parent);		
			printf ("Coordinateur : Envoi vers esclave n%d.\n", i);

			MPI_Recv(&temperatureToSend, 1, MPI_FLOAT,0, 0, parent, &etat);
			printf ("Coordinateur : Reception de esclave : %f \n", temperatureToSend);

		}

		char response = 'K';
		MPI_Send(&response, 1, MPI_CHAR, 0, 0, parent);
		printf ("Coordinateur %d : Envoi vers le pere !\n", myrank);
	}

	printf ("Coordinateur %d : FIN !\n", myrank);

	MPI_Finalize();
	return 0;
}
