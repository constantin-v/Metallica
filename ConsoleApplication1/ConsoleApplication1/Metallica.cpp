// ConsoleApplication1.cpp : définit le point d'entrée pour l'application console.
//

#include "stdafx.h"
#include <iostream>

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

int main()
{

	initialiseTemperatures();

	printGrid(startingTemperatures);
	cout << endl;

	for (int i = 0; i < 10; i++){
		startingTemperatures = decreaseTemperature(startingTemperatures);
		printGrid(startingTemperatures);
		cout << endl;
	}

	// Lit les températures de départ

	// Détermine un ensemble de températures à fournir aux threads esclaves

	// Thread esclave récupère les températures de ses voisins et de lui-meme

	// Thread calcule sa nouvelle température

	// Thread l’enregistre dans le tableau final

	char* t = { "b" };
	scanf_s(t);
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



