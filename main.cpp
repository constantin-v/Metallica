#include <iostream>
#include <mpi.h>
#include <stdio.h>
#include "Cell.h"
#include "Grid.h"
#include <omp.h>

using namespace std;

//global
Grid startingTemperatures;
Grid endingTemperatures;
int rows = 3;
int cols = 3;
float ambientTemperature = 20;

void initialiseTemperatures();
void printGrid(Grid grid);
Grid decreaseTemperature(Grid grid);

int main(int argc, char *argv[])
{
    initialiseTemperatures();

	printGrid(startingTemperatures);
	cout << endl;

	printf("Pere : Je suis VIVAAAAAAAANT.\n");
	int i, compteur, errCodes[10];
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
		9
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
    float temperatureAmbiantToSend = startingTemperatures.getAmbientTemperature();

    MPI_Send (&temperatureAmbiantToSend, 1, MPI_FLOAT, 0, 0, intercomm);
    printf ("Pere : Envoi vers coordinateur \n");


	// Communication pere -> fils
	for (i=1; i<10; i++)	{

        float temperatureToSend = startingTemperatures.getCell(i-1).getTemperature();

		MPI_Send (&temperatureToSend, 1, MPI_FLOAT, i, 0, intercomm);
		printf ("Pere : Envoi vers esclave %d.\n", i);
	}


        MPI_Recv(&returnCodeFromChildren, 1, MPI_CHAR,0, 0, intercomm, &etat);
		printf ("Pere : Reception du coordinateur : %c \n", returnCodeFromChildren);


	printf ("Pere   : Fin.\n");

	MPI_Finalize();
	return 0;

	/*
	for (int i = 0; i < 10; i++){
		startingTemperatures = decreaseTemperature(startingTemperatures);
		printGrid(startingTemperatures);
		cout << endl;
	}

	// Lit les températures de départ

	// Détermine un ensemble de températures à fournir aux threads esclaves

	// Thread esclave récupère les températures de ses voisins et de lui-meme

	// Thread calcule sa nouvelle température

	// Thread lenregistre dans le tableau final

	char* t = { "b" };
	scanf_s(t);
	return 0;
	*/
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
	startingTemperatures.setRows(rows);
	startingTemperatures.setCols(cols);
	startingTemperatures.setAmbientTemperature(ambientTemperature);
	startingTemperatures.allocateGridTab();

	int i;
	int j;
	for (i = 0; i<3; i++)
	{
		for (j = 0; j<3; j++)
		{
			if (i == 1 && j == 1)
			{
				// startingTemperatures.grid[i][j].setTemperature(50);
				startingTemperatures.getCell(i, j).setTemperature(50);
			}
			else
			{
				//startingTemperatures.grid[i][j].setTemperature(30);
				startingTemperatures.getCell(i, j).setTemperature(30.0f);
			}
		}
	}

	endingTemperatures.setRows(rows);
	endingTemperatures.setCols(cols);
	endingTemperatures.setAmbientTemperature(ambientTemperature);
	endingTemperatures.allocateGridTab();

	for (i = 0; i<3; i++)
	{
		for (j = 0; j<3; j++)
		{
			endingTemperatures.getCell(i, j).setTemperature(0.0f);
		}
	}
}

void printGrid(Grid grid)
{
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			float value = grid.getCell(i, j).getTemperature();
			cout << value << "  ";
		}
		cout << endl;
	}
}


