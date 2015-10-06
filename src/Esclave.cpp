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

//Retourne la position de l'esclave reçu par rapport à l'esclave en cours
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
        printf("RANG : %f", neighboursTemps[4][9]);
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
    temperaturesGrid[15] = neighboursTemps[5][4];
    temperaturesGrid[16] = neighboursTemps[3][9];
    temperaturesGrid[20] = neighboursTemps[5][7];
    temperaturesGrid[21] = neighboursTemps[6][3];
    temperaturesGrid[22] = neighboursTemps[7][1];
    temperaturesGrid[23] = neighboursTemps[7][2];
    temperaturesGrid[24] = neighboursTemps[7][3];
    temperaturesGrid[25] = neighboursTemps[8][1];

    printf("Temperature around esclave. Temperatures : \n|%f|%f|%f|%f|%f|\n|%f|%f|%f|%f|%f|\n|%f|%f|%f|%f|%f|\n|%f|%f|%f|%f|%f|\n|%f|%f|%f|%f|%f|\n",
                                        temperaturesGrid[1],temperaturesGrid[2],temperaturesGrid[3],temperaturesGrid[4],temperaturesGrid[5],
                                        temperaturesGrid[6],temperaturesGrid[7],temperaturesGrid[8],temperaturesGrid[9],temperaturesGrid[10],
                                        temperaturesGrid[11],temperaturesGrid[12],temperaturesGrid[13],temperaturesGrid[14],temperaturesGrid[15],
                                        temperaturesGrid[16],temperaturesGrid[17],temperaturesGrid[18],temperaturesGrid[19],temperaturesGrid[20],
                                        temperaturesGrid[21],temperaturesGrid[22],temperaturesGrid[23],temperaturesGrid[24],temperaturesGrid[25]);

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

        /*
            On met le rang de l'esclave actuel au début de la liste des températures à envoyer
            Cela permettra de les replacer dans le bon ordre lors de la reception
        */
        gridFloat[0] = myrank;

        /*
        On met dans notre tableau (au centre) de résultat les températures de notre esclave
        0 |  1  | 2
        3 | -4- | 5
        6 |  7  | 8
        */
        temperatures[4] = gridFloat;

        MPI_Recv(&rows, 1, MPI_INT, 0, 0, parent, &etat);
        printf ("Esclave n°%d : Reception du nombre de lignes %d !\n", myrank, rows);

		MPI_Recv(&cols, 1, MPI_INT, 0, 0, parent, &etat);
		printf ("Esclave n°%d : Reception du nombre de colonnes %d !\n", myrank, cols);

        //Calcul des coordonées de l'esclave pour distinguer ces voisins DIRECTS
		int *coords = getCoordinatesFromIndex(myrank);
		int *voisins = getVoisins(coords); // renvoi ==> [0,x,x,x,0,0....] ou x correspond aux index voisins direct.
		//printf("Coordonnees esclave N°%d : %d;%d \n", myrank,coords[0],coords[1]);

        //Directive de l'énoncé (envoyé 10 FOIS aux voisins)
        for(int i = 1; i < 10; i++){
        	//printf ("Esclave n°%d : Attente reception temperature ambiante!\n", myrank);
            //--Reception de la temperature ambiante du COORDINATEUR
        	MPI_Recv(&ambientTemperature, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, &etat);
            printf ("Esclave n°%d : Reception de la temperature ambiante (%f°C) de la part du coordinateur !\n", myrank, ambientTemperature);
            //grid.setAmbientTemperature(ambientTemperature);

            //Envoyer en asynchrone à tous ces voisins DIRECTS
            int nbVoisins = getNbVoisins(voisins);
            //printf("Nombre de voisins de l'esclave n°%d : %d \n", myrank, nbVoisins);
            int *tableauVoisinsTries = new int[8];
            int compteur = 0;

            //On envoi nos valeurs à tous nos voisins DIRECTS
            for(int rankVoisin = 0 ; rankVoisin < 9 ; rankVoisin++){
            	//Si l'esclave en cours d'analyse est un voisin direct, on lui envoi nos températures.
            	if(voisins[rankVoisin] != 0){
            		//MPI_Isend(&temperature, 1, MPI_FLOAT, voisins[rankVoisin], 0, MPI_COMM_WORLD, &requestNull);

                    printf("Temperature BLAAAA AVANT ENVOI ENVOYEUR %d===%f RECEPTIONNEUR %d. Temperatures : |%f|%f|%f|%f|%f|%f|%f|%f|%f| \n",
                                                                                                        myrank,gridFloat[0], voisins[rankVoisin],
                                                                                                        gridFloat[1],
                                                                                                        gridFloat[2], gridFloat[3],
                                                                                                        gridFloat[4], gridFloat[5],
                                                                                                        gridFloat[6], gridFloat[7],
                                                                                                        gridFloat[8], gridFloat[9]);

            		MPI_Isend(gridFloat, 10, MPI_FLOAT, voisins[rankVoisin], 0, MPI_COMM_WORLD, &requestNull);

            		//On stocke un par un les esclaves a qui on a envoyé les valeurs pour faciliter la phase de reception
            		tableauVoisinsTries[compteur] = voisins[rankVoisin];
            		compteur++;
            	}
            }


            //Réception des (grilles) températures voisins DIRECTS en synchrone
            float *temperaturesVoisins = new float[8];
            for(int k = 0 ; k < nbVoisins ; k++){

            	float* receivedTemp = new float[10];
            	//printf("Message a recevoir de esclave N°%d \n", tableauVoisinsTries[k]);
            	MPI_Recv(receivedTemp, 10, MPI_FLOAT, tableauVoisinsTries[k], 0, MPI_COMM_WORLD, &etat);
            	//printf("Message recu de esclave N°%d \n", tableauVoisinsTries[k]);
            	//temperaturesVoisins[k] = receivedTemp;




            	/*
                    On calcul la position de l'esclave emetteur
                    par rapport a l'esclave recepteur
                    Exemple :
                    1 |  2  | 3  | 4
                    5 |  6  | 7  | 8
                    9 |  10  | 11 | 12
                    On place le recepteur au centre et on recréer une plaque 3x3 autour de lui
                    Si esclave emetteur = 4 et recepteur 8 ==> emetteur - recepteur = -4
                    -5 |  -4 (emett) | -3
                    -1 |  recepteur  |  1
                    +3 |      +4     | +5
                    Par rapport à ce tableau on voit que la position est correct (
                */
            	int index = getIndexInTemperaturesTable(receivedTemp[0], myrank);
            	if(index != -1) {
                    temperatures[index] = receivedTemp;

                   // if(myrank == 1){
                        cout << "mise à jour tableau 3x3 de l'esclave " << myrank <<endl;
                    //}

            	}

            }

            //if(myrank == 1){

            //}

            // On calcul les moyennes de chaques cases de la grille de l'esclave en cours.
            temperatures[4] = getDecreasedTemperature(temperatures);
            gridFloat = temperatures[4];

          /*  printf("Temperature calcule esclave N°%d. Temperatures : |%f|%f|%f|%f|%f|%f|%f|%f|%f| \n", myrank, temperatures[4][0],temperatures[4][1],
                                                                                                        temperatures[4][2], temperatures[4][3],
                                                                                                        temperatures[4][4], temperatures[4][5],
                                                                                                        temperatures[4][6], temperatures[4][7],
                                                                                                        temperatures[4][8]);*/

            MPI_Send(temperatures[4], 9, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
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
