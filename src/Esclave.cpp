#include <mpi.h>
#include <stdio.h>

// Global
int rows, cols;

float getAvgTemperature(float *temperatures, float temperatureCase)
{
	int i = 0;
	float totalTemperature = 0;
	for (i = 0; i < 8; i++)
	{
		totalTemperature += temperatures[i];
	}
	totalTemperature += temperatureCase;

	return (totalTemperature / 9);

}

int getIndexFromCoordinates(int x, int y){
	int index = -1;

	for (int j = 0; j<rows; j++){
		for (int k = 0; k<cols; k++){
			index++;
			if (x == j && y == k){
				break;
			}
		}
	}

	return index;
}

int* getCoordinatesFromIndex(int index){
	int* coordinates = new int[2];
	int count = -1;

	for (int j = 0; j< rows; j++){
		for (int k = 0; k< cols; k++){
			count++;
			if (count == index){
				coordinates[0] = j;
				coordinates[1] = k;
				break;
			}
		}
	}

	return coordinates;
}

bool areCoordinatesCorrect(int x, int y)
{
	if (x >= 0 && x < rows && y >= 0 && y < cols)
	{
		return true;
	}
	return false;
}

int* getVoisins(int *coords)
{
	int *lotVoisins = new int[8];
	int i = 0, j, k;
	for (j = coords[0] - 1; j <= (coords[0] + 1); j++)
	{
		for (k = coords[1] - 1; k <= (coords[1] + 1); k++)
		{
			if(areCoordinatesCorrect(j, k) && (j != coords[0] && k != coords[1])){
				lotVoisins[i] = getIndexFromCoordinates(j, k);
				i++;
			}
		}
	}
	return lotVoisins;
}



int main( int argc, char *argv[] )
{
	int myrank,cols,rows;
	float temperature, ambientTemperature;
	MPI_Comm parent;
	MPI_Status etat;
	MPI_Init (&argc, &argv);
	MPI_Comm_get_parent (&parent);
	MPI_Comm_rank (MPI_COMM_WORLD,&myrank);
	MPI_Request requestNull;

	if (parent == MPI_COMM_NULL) {
		printf ("Esclave %d : Pas de pere !\n", myrank);
	} else {

		MPI_Recv(&temperature, 1, MPI_FLOAT, 0, 0, parent, &etat);
        printf ("Esclave n°%d : Reception de la temperature case (%f°C) de la part du maitre !\n", myrank, temperature);

        MPI_Recv(&rows, 1, MPI_INT, 0, 0, parent, &etat);
		printf ("Esclave n°%d : Reception du nombre de lignes %d !\n", myrank, rows);

		MPI_Recv(&cols, 1, MPI_INT, 0, 0, parent, &etat);
		printf ("Esclave n°%d : Reception du nombre de colonnes %d !\n", myrank, cols);

		int *coords = getCoordinatesFromIndex(myrank);
		int *voisins = getVoisins(coords);

        for(int i=1; i< 10; i++){
        	MPI_Recv(&ambientTemperature, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, &etat);
            printf ("Esclave n°%d : Reception de la temperature ambiante (%f°C) de la part du coordinateur !\n", myrank, ambientTemperature);

            //Envoyer en asynchrone à tous ces voisins
            int nbVoisins = sizeof(voisins)/sizeof(*voisins);
            printf("Nombre de voisins de l'esclabe n°%d : %d \n", myrank, nbVoisins);
            for(int rankVoisin = 0 ; rankVoisin < nbVoisins ; rankVoisin++){
            	MPI_Isend(&temperature, 1, MPI_FLOAT, voisins[rankVoisin], 0, MPI_COMM_WORLD, &requestNull);
            }

            //Attendre réception des températures voisins en synchrone
            float *temperaturesVoisins = new float[8];
            for(int k = 0 ; k < nbVoisins ; k++){
            	float receivedTemp;
            	MPI_Recv(&receivedTemp, 1, MPI_FLOAT, voisins[k], 0, MPI_COMM_WORLD, &etat);
            	temperaturesVoisins[k] = receivedTemp;
            }

            for(int k = nbVoisins ; k < 8 ; k++){
            	temperaturesVoisins[k] = ambientTemperature;
            }

            //Calculer la nouvelle temp
            temperature = getAvgTemperature(temperaturesVoisins, temperature);
            printf("L'esclave n°%d a mis a jour sa temperature : %f°C\n", myrank, temperature);

            MPI_Send(&temperature, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
            printf ("Esclave n°%d : Envoi de la temperature ambiante (%f°C) au coordinateur !\n", myrank, ambientTemperature);
        }
	}

	printf ("Esclave n°%d : FIN !\n", myrank);

	MPI_Finalize();
	return 0;
}
