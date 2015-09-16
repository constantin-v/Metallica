#include "Grid.h"

Grid::Grid()
{

}

int Grid::getRows()
{
	return this->rows;
}

void Grid::setRows(int v)
{
	this->rows = v;
}

float Grid::getAmbientTemperature()
{
	return this->ambientTemperature;
}

void Grid::setAmbientTemperature(float v)
{
	this->ambientTemperature = v;
}



int Grid::getCols()
{
	return this->cols;
}

void Grid::setCols(int v)
{
	this->cols = v;
}


Cell& Grid::getCell(int x, int y)
{
	return grid[x][y];
}

void Grid::addCell(int x, int y, float v)
{
	Cell cell;
	cell.setTemperature(v);
	this->grid[x][y] = cell;
}

/* initialize the grid */
void Grid::allocateGridTab()
{
	/* Allocation dynamique */
	grid = new Cell*[rows + 1];
	for (int i = 0; i < rows + 1; i++)
		grid[i] = new Cell[cols + 1];

	/* Initialisation */
	for (int i = 0; i < rows + 1; i++)
	{
		for (int j = 0; j < cols + 1; j++)
		{
			this->addCell(i, j, 1);
		}
	}
}

/**
Récupère la température de la case dindex indexCase et celles de ses voisins (plus celle de lair ambiant le cas échéant).
**/
float* Grid::getLotTemperatures(int x, int y)
{
	float lotTemperatures[9]; //initialise with the good length
	int i = 0, j, k;
	for (j = x - 1; j <= (x + 1); j++)
	{
		for (k = y - 1; k <= (y + 1); k++)
		{
			lotTemperatures[i] = this->getTemperature(j, k);
			i++;
		}
	}
	return lotTemperatures;
}

/*
Calcule la moyenne de temperature dun ensemble de Cells
*/
float Grid::getAvgTemperature(int x, int y)
{
	float *temperatures = getLotTemperatures(x, y);

	int i = 0;
	float totalTemperature = 0;
	for (i = 0; i < 9; i++)
	{
		totalTemperature += temperatures[i];
	}
	return (totalTemperature / 9);

}

bool Grid::areCoordinatesCorrect(int x, int y)
{
	if (x >= 0 && x < 3 && y >= 0 && y < 3)
	{
		return true;
	}
	return false;
}

/**
retourne la température dune Cell à une case donnée de la grid.
retourne la température ambiante si Cell outofbound
**/
float Grid::getTemperature(int x, int y)
{
	if (this->areCoordinatesCorrect(x, y))
	{
		return this->grid[x][y].getTemperature();
	}
	else
	{
		return ambientTemperature;
	}
}

int* Grid::getCoordinatesFromIndex(int index){
	int coordinates[2];
	int count = -1;

	for (int j = 0; j<this->getRows(); j++){
		for (int k = 0; k<this->getCols(); k++){
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

int Grid::getIndexFromCoordinates(int x, int y){
	int index = -1;

	for (int j = 0; j<this->getRows(); j++){
		for (int k = 0; k<this->getCols(); k++){
			index++;
			if (x == j && y == k){
				break;
			}
		}
	}

	return index;
}
