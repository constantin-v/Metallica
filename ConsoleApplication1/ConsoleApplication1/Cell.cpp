#include "stdafx.h"
#include "Cell.h"

Cell::Cell()
{
}

Cell::Cell(float temperature) {
	this->temperature = temperature;
}

void Cell::setTemperature(float temperature){
	this->temperature = temperature;
}

float Cell::getTemperature(){
	return this->temperature;
}
