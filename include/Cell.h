#ifndef CELL_H
#define CELL_H
#pragma once

class Cell
{
    public:
        Cell();
        Cell(float temperature);
        void setTemperature(float temperature);
        float getTemperature();
    protected:
    private:
        float temperature;
};

#endif // CELL_H
