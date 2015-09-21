#include <mpi.h>
#include <stdio.h>

int main( int argc, char *argv[] )
{
	int myrank;
	float temperature;
	float* temperatures;
	int rows = 3;
	int cols = 4;
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
		
		temperatures =  = new float[cols * rows];

		for (int i=1; i<10; i++)	{
			for (int j=1; j<cols * rows; j++)	{

		        float temperatureToSend = temperature;        

				MPI_Send (&temperatureToSend, 1, MPI_FLOAT,j, 0, MPI_COMM_WORLD);		
				printf ("Coordinateur : Envoi vers esclave n%d.\n", i);

				MPI_Recv(&temperatureToSend, 1, MPI_FLOAT,j, 0, MPI_COMM_WORLD, &etat);
				printf ("Coordinateur : Reception de esclave n%d: %f \n", i,temperatureToSend);

				
			}

		}

		char response = 'K';
		MPI_Send(&response, 1, MPI_CHAR, 0, 0, parent);
		printf ("Coordinateur %d : Envoi vers le pere !\n", myrank);
	}

	printf ("Coordinateur %d : FIN !\n", myrank);

	MPI_Finalize();
	return 0;
}
