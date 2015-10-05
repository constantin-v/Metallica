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
	int index = 0;

	for (int j = 0; j<rows; j++){
		for (int k = 0; k<cols; k++){
			index++;
			if (x == j && y == k){
				return index;
			}
		}
	}
	return -1;
}

int* getCoordinatesFromIndex(int index){
	int* coordinates = new int[2];
	int count = 0;

	for (int j = 0; j< rows; j++){
		for (int k = 0; k< cols; k++){
			count++;
			if (count == index){
				coordinates[0] = j;
				coordinates[1] = k;
				return coordinates;
			}
		}
	}
	//printf("Coordonnees ZZZZZZZZZZZZZZZZZZZZ %d;%d\n", coordinates[0], coordinates[1]);
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
	int *lotVoisins = new int[9];
	int i = 0, j, k;
	for (j = coords[0] - 1; j <= (coords[0] + 1); j++)
	{
		for (k = coords[1] - 1; k <= (coords[1] + 1); k++)
		{
			if(areCoordinatesCorrect(j, k) && !(j == coords[0] && k == coords[1])) {
				lotVoisins[i] = getIndexFromCoordinates(j, k);
			} else {
				lotVoisins[i] = 0;
			}
			i++;
		}
	}
	return lotVoisins;
}

int getNbVoisins(int *voisins){
    int nbVoisins = 0;
    for(int i=0; i<9; i++){
        if(voisins[i] != 0){
            nbVoisins++;
        }
    }

    return nbVoisins;
}



int main( int argc, char *argv[] )
{
	int myrank;
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

        //JALON 7
        //Reception de la grid 3x3 du thread maitre (MPI)
        //... A FAIRE

		MPI_Recv(&temperature, 1, MPI_FLOAT, 0, 0, parent, &etat);
        //printf ("Esclave n°%d : Reception de la temperature case (%f°C) de la part du maitre !\n", myrank, temperature);

        MPI_Recv(&rows, 1, MPI_INT, 0, 0, parent, &etat);
		//printf ("Esclave n°%d : Reception du nombre de lignes %d !\n", myrank, rows);

		MPI_Recv(&cols, 1, MPI_INT, 0, 0, parent, &etat);
		//printf ("Esclave n°%d : Reception du nombre de colonnes %d !\n", myrank, cols);

		int *coords = getCoordinatesFromIndex(myrank);
		//printf("Coordonnees esclave N°%d : %d;%d \n", myrank,coords[0],coords[1]);
		int *voisins = getVoisins(coords);

        for(int i=1; i< 10; i++){
        	//printf ("Esclave n°%d : Attente reception temperature ambiante!\n", myrank);
        	MPI_Recv(&ambientTemperature, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, &etat);
            //printf ("Esclave n°%d : Reception de la temperature ambiante (%f°C) de la part du coordinateur !\n", myrank, ambientTemperature);

            //Envoyer en asynchrone à tous ces voisins
            int nbVoisins = getNbVoisins(voisins);
            int *tableauVoisinsTries = new int[8];
            int compteur = 0;

            //On envoi notre temprature à tous nos voisins
            //JALON 7 -- ENVOI GRID 3x3 A TOUS NOS VOISINS
            //printf("Nombre de voisins de l'esclave n°%d : %d \n", myrank, nbVoisins);
            for(int rankVoisin = 0 ; rankVoisin < 9 ; rankVoisin++){
            	if(voisins[rankVoisin] != 0){
            		MPI_Isend(&temperature, 1, MPI_FLOAT, voisins[rankVoisin], 0, MPI_COMM_WORLD, &requestNull);
            		tableauVoisinsTries[compteur] = voisins[rankVoisin];
            		compteur++;
            		//printf("Message de esclave N°%d vers esclave %d \n",myrank, voisins[rankVoisin] );
            	}
            }

            //Attendre réception des températures voisins en synchrone
            //JALON 7 -- RECEPTION GRID DE TOUS NOS VOISINS
            float *temperaturesVoisins = new float[8];
            for(int k = 0 ; k < nbVoisins ; k++){
            	float receivedTemp;
            	//printf("Message a recevoir de esclave N°%d \n", tableauVoisinsTries[k]);
            	MPI_Recv(&receivedTemp, 1, MPI_FLOAT, tableauVoisinsTries[k], 0, MPI_COMM_WORLD, &etat);
            	//printf("Message recu de esclave N°%d \n", tableauVoisinsTries[k]);
            	temperaturesVoisins[k] = receivedTemp;
            }

            //JALON 7 -- On complete non plus avec des temperatures simples, mais des GRID de temperature ambiante
            for(int k = nbVoisins ; k < 8 ; k++){
            	temperaturesVoisins[k] = ambientTemperature;
            }

            //JALON 7 --
            //On crée un tableau de 9x9 contenant les 9 grids crées précédemment
            //La case de l'esclave central se retrouve au millieu de ce tableau
            //On parcours la grid 3x3 centrale, en utilisant les cases à coté pour calculer la moyenne de chaque case

            //Calculer la nouvelle temp
            //JALON 7 -- On calcule la moyenne de la case globale, et on l'envoi au coordinateur
            temperature = getAvgTemperature(temperaturesVoisins, temperature);
            printf("L'esclave n°%d a mis a jour sa temperature : %f°C\n", myrank, temperature);

            MPI_Send(&temperature, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
            //printf ("Esclave n°%d : Envoi de la temperature ambiante (%f°C) au coordinateur !\n", myrank, ambientTemperature);
        }
	}

	printf ("Esclave n°%d : FIN !\n", myrank);

	MPI_Finalize();
	return 0;
}
