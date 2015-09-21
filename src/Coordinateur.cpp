#include <mpi.h>
#include <stdio.h>

void printGrid(float* table, int rows, int cols)
{
	int count = -1;
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			count++;
			float value = table[count];
			printf("%f   ", value);
		}
		printf("\n");
	}
}

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
		printf ("Coordinateur : Pas de pere !\n");
	} else {
		MPI_Recv(&temperature, 1, MPI_FLOAT, 0, 0, parent, &etat);

		printf ("Coordinateur : Reception de la temperature ambiante %f !\n", temperature);
		
		temperatures =  new float[cols * rows];

		for (int i=1; i<10; i++)	{
			for (int j=1; j<cols * rows + 1; j++)	{

		        float temperatureToSend = temperature; 

				//printf ("Coordinateur : Envoi vers l'esclave n°%d de la temperature ambiante (%f°C).\n", j, temperature);
				MPI_Send (&temperature, 1, MPI_FLOAT,j, 0, MPI_COMM_WORLD);

				MPI_Recv(&temperature, 1, MPI_FLOAT,j, 0, MPI_COMM_WORLD, &etat);
				//printf ("Coordinateur : Reception de l'esclave n°%d: %f°C \n", j, temperature);
				
				temperatures[j-1] = temperature;
			}
			printGrid(temperatures,rows,cols);
		}

		char response = 'K';
		MPI_Send(&response, 1, MPI_CHAR, 0, 0, parent);
		printf ("Coordinateur : Envoi vers le pere !\n");
	}

	printf ("Coordinateur : FIN !\n");

	MPI_Finalize();
	return 0;
}
