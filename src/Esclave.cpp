#include <mpi.h>
#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include "Cell.h"
#include "Grid.h"
#include <omp.h>

using namespace std;

// Global
int rows, cols;
float* getDecreasedTemperature(float** temperaturesTab);

float getAvgTemperature(float *temperatures)
{
	int i = 0;
	float totalTemperature = 0;
	for (i = 0; i < 9; i++)
	{
		totalTemperature += temperatures[i];
	}

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



Grid parseFloatsToGrid(float* temperatures){
    int i = 0, j = 0, k = 0;
    Grid grid;

    grid.setRows(3);
	grid.setCols(3);
	grid.allocateGridTab();

	for (i = 0; i<3; i++)
	{
		for (j = 0; j<3; j++)
		{
            grid.getCell(i, j).setTemperature(temperatures[k]);
            k++;
		}
	}

	return grid;
}

int getIndexInTemperaturesTable(float index, int esclaveIndex){

    int indexRelatif = index - esclaveIndex;

    switch (indexRelatif){
        case -5:
            return 0;
            break;
        case -4:
            return 1;
            break;
        case -3:
            return 2;
            break;
        case -1:
            return 3;
            break;
        case 1:
            return 5;
            break;
        case 3:
            return 6;
            break;
        case 4:
            return 7;
            break;
        case 5:
            return 8;
            break;
        default:
            return -1;
            break;
    }
    return -1;
}

float* mergeTemperaturesGrid(float** neighboursTemps){
    float* temperaturesGrid = new float[25];
    int index = 7;

    for(int i = 1 ; i < 10 ; i++){
        temperaturesGrid[index] = neighboursTemps[4][i];
        index++;
        if(index%5 == 0){
            index=index+2;
        }
    }

    temperaturesGrid[1] = neighboursTemps[0][9];
    temperaturesGrid[2] = neighboursTemps[1][7];
    temperaturesGrid[3] = neighboursTemps[1][8];
    temperaturesGrid[4] = neighboursTemps[1][9];
    temperaturesGrid[5] = neighboursTemps[2][7];
    temperaturesGrid[6] = neighboursTemps[3][3];
    temperaturesGrid[10] = neighboursTemps[5][1];
    temperaturesGrid[11] = neighboursTemps[3][6];
    temperaturesGrid[13] = neighboursTemps[5][4];
    temperaturesGrid[16] = neighboursTemps[3][9];
    temperaturesGrid[20] = neighboursTemps[5][7];
    temperaturesGrid[21] = neighboursTemps[6][3];
    temperaturesGrid[22] = neighboursTemps[7][1];
    temperaturesGrid[23] = neighboursTemps[7][2];
    temperaturesGrid[24] = neighboursTemps[7][3];
    temperaturesGrid[25] = neighboursTemps[8][1];

    return temperaturesGrid;
}

/* Index sur un tableau relatif de 25*25 (3*3 de la case concernée plus une bordure d'une case) */
float* getRelativeToCellTempGrid(int index,float** neighboursTemps){
    float* mergedTemperatures = mergeTemperaturesGrid(neighboursTemps);
    float* relativeGrid = new float[9];

    int relativeIndex;
    if(index < 3){
        relativeIndex = index + 7;
    } else if (index < 6) {
        relativeIndex = index + 9;
    } else {
        relativeIndex = index + 11;
    }

    int i = 0;
    for(int j = (relativeIndex-6); j < (relativeIndex-3) ; j++){
        relativeGrid[i] = mergedTemperatures[j];
        i++;
    }

    for(int j = (relativeIndex-1); j < (relativeIndex+2) ; j++){
        relativeGrid[i] = mergedTemperatures[j];
        i++;
    }

    for(int j = (relativeIndex+4); j < (relativeIndex+7) ; j++){
        relativeGrid[i] = mergedTemperatures[j];
        i++;
    }

    return relativeGrid;
}


int main( int argc, char *argv[] )
{
	int myrank;
	float ambientTemperature;
	float** temperatures = new float*[9];
	Grid grid;
	float* gridFloat = new float[10];
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
        MPI_Recv(gridFloat, 10, MPI_FLOAT, 0, 0, parent, &etat);
        printf ("Esclave n°%d : Reception de la grid de la part du maitre !\n", myrank);
        //grid = parseFloatsToGrid(gridChar);

        //JALON 7 -- On remplis par défaut notre tableau de résultat avec des GRID de temperature ambiante
        for(int i =0;i< 9;i++) {
            float* temp = new float[10];
            temp[0] = 20;
            temp[1] = 20;
            temp[2] = 20;
            temp[3] = 20;
            temp[4] = 20;
            temp[5] = 20;
            temp[6] = 20;
            temp[7] = 20;
            temp[8] = 20;
            temp[9] = 20;
            temperatures[i] =  temp;
        }

        //On met dans le rang de l'esclave actuel au début de la liste des températures à envoyer
        gridFloat[0] = myrank;

        //On met dans notre tableau de résultat les températures de notre esclave
        temperatures[4] = gridFloat;

        MPI_Recv(&rows, 1, MPI_INT, 0, 0, parent, &etat);
        printf ("Esclave n°%d : Reception du nombre de lignes %d !\n", myrank, rows);

		MPI_Recv(&cols, 1, MPI_INT, 0, 0, parent, &etat);
		printf ("Esclave n°%d : Reception du nombre de colonnes %d !\n", myrank, cols);

		int *coords = getCoordinatesFromIndex(myrank);
		//printf("Coordonnees esclave N°%d : %d;%d \n", myrank,coords[0],coords[1]);

		int *voisins = getVoisins(coords);

        for(int i=1; i< 10; i++){
        	//printf ("Esclave n°%d : Attente reception temperature ambiante!\n", myrank);


        	MPI_Recv(&ambientTemperature, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, &etat);
            printf ("Esclave n°%d : Reception de la temperature ambiante (%f°C) de la part du coordinateur !\n", myrank, ambientTemperature);
            //grid.setAmbientTemperature(ambientTemperature);


            //Envoyer en asynchrone à tous ces voisins
            int nbVoisins = getNbVoisins(voisins);
            int *tableauVoisinsTries = new int[8];
            int compteur = 0;

            //On envoi notre temprature à tous nos voisins
            //JALON 7 -- ENVOI GRID 3x3 A TOUS NOS VOISINS
            //printf("Nombre de voisins de l'esclave n°%d : %d \n", myrank, nbVoisins);
            for(int rankVoisin = 0 ; rankVoisin < 9 ; rankVoisin++){
            	if(voisins[rankVoisin] != 0){
            		//MPI_Isend(&temperature, 1, MPI_FLOAT, voisins[rankVoisin], 0, MPI_COMM_WORLD, &requestNull);
            		MPI_Isend(gridFloat, 10, MPI_FLOAT, voisins[rankVoisin], 0, MPI_COMM_WORLD, &requestNull);
            		tableauVoisinsTries[compteur] = voisins[rankVoisin];
            		compteur++;
            		//printf("Message de esclave N°%d vers esclave %d \n",myrank, voisins[rankVoisin] );
            	}
            }


            //Attendre réception des températures voisins en synchrone
            //JALON 7 -- RECEPTION GRID DE TOUS NOS VOISINS
            float *temperaturesVoisins = new float[8];
            for(int k = 0 ; k < nbVoisins ; k++){
            	float* receivedTemp = new float[10];
            	//printf("Message a recevoir de esclave N°%d \n", tableauVoisinsTries[k]);
            	MPI_Recv(receivedTemp, 10, MPI_FLOAT, tableauVoisinsTries[k], 0, MPI_COMM_WORLD, &etat);
            	//printf("Message recu de esclave N°%d \n", tableauVoisinsTries[k]);
            	//temperaturesVoisins[k] = receivedTemp;
            	int index = getIndexInTemperaturesTable(receivedTemp[0],myrank);
            	if(index != -1) {
                    temperatures[index] = receivedTemp;
            	}

            }

            //JALON 7 --
            //On crée un tableau de 9x9 contenant les 9 grids crées précédemment
            //La case de l'esclave central se retrouve au millieu de ce tableau
            //On parcours la grid 3x3 centrale, en utilisant les cases à coté pour calculer la moyenne de chaque case
            temperatures[4] = getDecreasedTemperature(temperatures);

            cout << "L'esclave n°" << myrank << "a mis a jour ses temperatures a jour" << temperatures[4][0] << ":" << temperatures[4][4] <<endl;

            MPI_Send(temperatures[4], 10, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
            printf ("Esclave n°%d : Envoi de ses temperatures au coordinateur !\n", myrank);
        }
	}

	printf ("Esclave n°%d : FIN !\n", myrank);

	MPI_Finalize();
	return 0;
}


float* getDecreasedTemperature(float** temperaturesTab) { //On envoi le tableau de 3*3 de l'esclave courant
	float* returnTab = new float[9];
	int i, j;


	#pragma omp parallel private (i,j) shared (temperaturesTab,returnTab)
	{
		#pragma omp for
		for (i = 0; i < 9; i++){

            //Pour chaque températures, on récupère une grille de ses voisins
            float* tempAround = getRelativeToCellTempGrid(i,temperaturesTab);

            //on fait la moyenne des temperatures
            float avgTemp = getAvgTemperature(tempAround);

            //On met à jour le tableau de retour avec la valeur calculée
			returnTab[i] = avgTemp;

		}
	}
	return returnTab;
}
