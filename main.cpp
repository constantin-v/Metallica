#include <iostream>
#include <mpi.h>
#include <stdio.h>
#include "Cell.h"
#include "Grid.h"
#include <omp.h>

using namespace std;

//global
Grid** startingTemperatures;
int rows = 3;
int cols = 4;
float ambientTemperature = 20;

void initialiseTemperatures();
void printGrid(Grid grid);
Grid decreaseTemperature(Grid grid);
int* getCoordinatesFromIndex(int index);
int getIndexFromCoordinates(int x, int y);
void printTableOfGrid(Grid** tableOfGrid);

int main(int argc, char *argv[])
{
    initialiseTemperatures();

	printTableOfGrid(startingTemperatures);
	cout << endl;

	printf("Pere : Je suis VIVAAAAAAAANT.\n");
	int i, compteur, errCodes[cols * rows];
	char returnCodeFromChildren;
	MPI_Status etat;
	MPI_Comm intercomm;
	MPI_Request requestNull;

	char *cmds[2] = {
		"./coordinateur",
		"./esclave",
	};

	int nbInstances[2] = {
		1,
		cols * rows
	};

	MPI_Info infos[2] = {
		MPI_INFO_NULL,
		MPI_INFO_NULL
	};

	MPI_Init(&argc, &argv);
	MPI_Comm_spawn_multiple(
		2,
		cmds,
		MPI_ARGVS_NULL,
		nbInstances,
		infos,
		0,
		MPI_COMM_WORLD,
		&intercomm,
		errCodes
	);

	printf("Pere : Toutes les instances sont lancees.\n");

    //Communication pere --> coordinateur
    float temperatureAmbiantToSend = startingTemperatures[0][0].getAmbientTemperature();
    //printf ("Pere : Envoi vers le coordinateur de la temperature ambiante (%f°C)\n", temperatureAmbiantToSend);
    MPI_Send (&temperatureAmbiantToSend, 1, MPI_FLOAT, 0, 0, intercomm);

    //printf ("Pere : Envoi vers le coordinateur du nombre de lignes %d \n", rows);
    MPI_Send (&rows, 1, MPI_INT, 0, 0, intercomm);

    //printf ("Pere : Envoi vers le coordinateur du nombre de colonnes %d \n", cols);
    MPI_Send (&cols, 1, MPI_INT, 0, 0, intercomm);

	// Communication pere -> fils

	char *temperaturesToSend;
	for (i=1; i< rows * cols + 1; i++)	{
        //JALON 7
        //On crée un Grid 3x3 contenant les 9 témperatures de la case maître
        //On envoi cette grid à chaque esclave
        int *coords = getCoordinatesFromIndex(i-1);
        temperaturesToSend = startingTemperatures[coords[0]][coords[1]].toStringPipe();

        //JALON 5
        //temperaturesToSend = startingTemperatures.getCell(i-1).getTemperature();
        //printf ("Pere : Envoi vers l'esclave n°%d de sa temperature case (%f°C).\n", i, temperatureToSend);
		MPI_Send (&temperaturesToSend, 1, MPI_CHAR, i, 0, intercomm);

        //printf ("Pere : Envoi du nombre de ligne vers l'esclave n%d \n", i);
        MPI_Send (&rows, 1, MPI_INT, i, 0, intercomm);

        //printf ("Pere : Envoi du nombre de colonnes vers l'esclave n%d \n", i);
        MPI_Send (&cols, 1, MPI_INT, i, 0, intercomm);

	}

    MPI_Recv(&returnCodeFromChildren, 1, MPI_CHAR,0, 0, intercomm, &etat);
    printf ("Pere : Reception du coordinateur : %c \n", returnCodeFromChildren);


	printf ("Pere : Fin.\n");

	MPI_Finalize();
	return 0;

}

Grid decreaseTemperature(Grid grid) {
	Grid returnGrid;
	returnGrid.setRows(rows);
	returnGrid.setCols(cols);
	returnGrid.setAmbientTemperature(ambientTemperature);
	returnGrid.allocateGridTab();
	int i, j;

	#pragma omp parallel private (i,j) shared (grid,returnGrid)
	{
		int cellNumber = rows * cols;

		#pragma omp for
		for (i = 0; i < cellNumber; i++){
			int *coords = grid.getCoordinatesFromIndex(i);
			float avgTemp = grid.getAvgTemperature(coords[0], coords[1]);
			coords = grid.getCoordinatesFromIndex(i);
			returnGrid.getCell(coords[0], coords[1]).setTemperature(avgTemp);
		}
	}
	return returnGrid;
}

/* Initialise :
les temperature dans la matrice de métal 3x3 de départ
les temperature de la matrice de refroidissement
*/
void initialiseTemperatures()
{
	Grid** startingTemperatures;

	int i;
	int j;
	for (i = 0; i<rows; i++)
	{
		for (j = 0; j<cols; j++)
		{
            //On créer une grille 3*3 par case au lieu de 1 température par case.
            Grid tempGrid;
            tempGrid.setRows(3);
            tempGrid.setCols(3);
            tempGrid.setAmbientTemperature(ambientTemperature);
            tempGrid.allocateGridTab();

            for (int k = 0;k <3;k++) {
                for(int l = 0;l <3;l++) {
                    if (k == 1 && l == 1) {
                        tempGrid.getCell(i, j).setTemperature(90);
                    }	else	{
                        tempGrid.getCell(i, j).setTemperature(44.0f);
                    }
                }
            }

            startingTemperatures[i][j] = tempGrid;

		}
	}

	/*endingTemperatures.setRows(rows);
	endingTemperatures.setCols(cols);
	endingTemperatures.setAmbientTemperature(ambientTemperature);
	endingTemperatures.allocateGridTab();

	for (i = 0; i<rows; i++)
	{
		for (j = 0; j<cols; j++)
		{
			endingTemperatures.getCell(i, j).setTemperature(0.0f);
		}
	}*/
}

void printGrid(Grid grid)
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			float value = grid.getCell(i, j).getTemperature();
			cout << value << "  ";
		}
		cout << endl;
	}
}

void printTableOfGrid(Grid** tableOfGrid)
{
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			printGrid(tableOfGrid[i][j]);
		}
		cout << endl;
	}
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

int getIndexFromCoordinates(int x, int y){
	int index = -1;

	for (int j = 0; j < rows; j++){
		for (int k = 0; k < cols; k++){
			index++;
			if (x == j && y == k){
				break;
			}
		}
	}

	return index;
}



