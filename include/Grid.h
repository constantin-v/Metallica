#ifndef GRID_H
#define GRID_H
#include "Cell.h"
#pragma once

class Grid
{
    public:
        Grid();
        void addCell(int x, int y, float v);
        Cell& getCell(int x, int y);
        Cell& getCell(int index);
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
        char* toStringPipe();
    protected:
    private:
        Cell **grid;
        int rows;
        int cols;
        float ambientTemperature;
};

#endif // GRID_H
