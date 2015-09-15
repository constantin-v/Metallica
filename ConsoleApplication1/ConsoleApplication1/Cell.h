#pragma once
class Cell
{
private:
	float temperature;

public:
	Cell();
	Cell(float temperature);
	void setTemperature(float temperature);
	float getTemperature();

};
