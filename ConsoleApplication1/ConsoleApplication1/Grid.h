
#include "Cell.h"

#pragma once
class Grid
{
private:
	Cell **grid;
	int rows;
	int cols;
	float ambientTemperature;
public:
	Grid();
	void addCell(int x, int y, float v);
	Cell& getCell(int x, int y);
	int getRows();
	void setRows(int v);
	int getCols();
	void setCols(int v);
	void allocateGridTab();
	float* getLotTemperatures(int x, int y);
	float getAvgTemperature(int x, int y);
	bool areCoordinatesCorrect(int x, int y);
	float getTemperature(int x, int y);
	float getAmbientTemperature();
	void setAmbientTemperature(float v);
	int* getCoordinatesFromIndex(int index);
	int getIndexFromCoordinates(int x, int y);
};

