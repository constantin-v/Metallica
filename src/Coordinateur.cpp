#include <mpi.h>
#include <stdio.h>
#include <fstream>
#include <string>
#include <sstream>
#include "Cell.h"
#include "Grid.h"

namespace std
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}
#include <iostream>

using namespace std;

int step = 0;

void printGrid(float** table, int rows, int cols)
{
	int count = -1;
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			count++;
			float value = table[count];
			printf("%f   ", value);
		}
		printf("\n");
	}
}

string getColorByTemperature(float temp){
    string color = "FFFFFF";
    int temperature = (int) temp;
    if(temperature <= 22) { color = "003FFA";}
    else if(temperature <= 24){ color = "3064FF";}
    else if(temperature <= 26){ color = "30ACFF";}
    else if(temperature <= 28){ color = "30CFFF";}
    else if(temperature <= 30){ color = "DCF598";}
    else if(temperature <= 32){ color = "E7FF4A";}
    else if(temperature <= 34){ color = "FFF34A";}
    else if(temperature <= 36){ color = "FFE44A";}
    else if(temperature <= 38){ color = "FFC94A";}
    else if(temperature <= 40){ color = "FF9B4A";}
    else if(temperature > 40){ color = "FF4A4A";}
    else {color = "FFFFFF";}

    return "#"+color;
}

void printSVG(float** table, int rows, int cols, float ambiantTemp)
{
	int widthRectangle = 100;
	int heightRectangle = 100;
	int widthTotal = cols * widthRectangle;
	int heightTotal = rows * heightRectangle;

	std::string name = "Metallica" + std::to_string(step) + ".html";
	cout << name << endl;
	std::ofstream outfile;
	outfile.open (name.c_str());
	outfile << "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?>";
	outfile << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" ";
	outfile << "\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">";
	outfile << "<html xmlns=\"http://www.w3.org/1999/xhtml\"> ";
	outfile << "<head>";
	outfile << "<title>Refroidissement d'une plaque de métal</title>";
	outfile << "</head>";
	outfile << "<body>";
	outfile << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\"";
	outfile << "width=\"" << widthTotal + 100 << "px\" height=\""<< heightTotal +100 <<"px\">" << std::endl;

    //root rectangle for ambiant temp
    outfile << "<rect width=\""<< widthTotal + 100 <<"\" height=\""<< heightTotal + 100 <<"\" fill=\"" << getColorByTemperature(ambiantTemp) << "\" stroke=\"black\"/>" << std::endl;
    outfile << "<text x=\""<< 25 <<"\" y=\""<< heightTotal + 25 <<"\" width=\""<< widthRectangle <<"\" height=\""<< heightRectangle <<"\" fill=\"#333333\"> Ambiant Temperature : " << ambiantTemp <<"</text>"<< std::endl;

	int count = -1;
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			count++;
			float value = table[count];
			int ypos = i * widthRectangle; //i creer les lignes donc la position y
			int xpos = j * heightRectangle; // j creer les colonnes donc le x
			outfile << "<rect x=\""<< xpos <<"\" y=\""<< ypos <<"\" width=\""<< widthRectangle <<"\" height=\""<< heightRectangle <<"\" fill=\"" << getColorByTemperature(value) << "\" stroke=\"black\"/>" << std::endl;
            outfile << "<text x=\""<< xpos + 25 <<"\" y=\""<< ypos + 25 <<"\" width=\""<< widthRectangle <<"\" height=\""<< heightRectangle <<"\" fill=\"#333333\">" << value <<"</text>"<< std::endl;

		}
		printf("\n");
	}

	outfile <<   "</svg> </body></html>" << std::endl;
	outfile.close();
	step++;
}

int main( int argc, char *argv[] )
{
	int myrank;
	float temperature;
	float* storeTemperature;
	float** temperatures;
	int rows = 2;
	int cols = 2;
	MPI_Comm parent;
	MPI_Status etat;
	MPI_Init (&argc, &argv);
	MPI_Comm_get_parent (&parent);
	MPI_Comm_rank (MPI_COMM_WORLD,&myrank);

	if (parent == MPI_COMM_NULL) {
		printf ("Coordinateur : Pas de pere !\n");
	} else {
		MPI_Recv(&temperature, 1, MPI_FLOAT, 0, 0, parent, &etat);
		printf ("Coordinateur : Reception de la temperature ambiante %f !\n", temperature);

		MPI_Recv(&rows, 1, MPI_INT, 0, 0, parent, &etat);
		printf ("Coordinateur : Reception du nombre de lignes %d !\n", rows);

		MPI_Recv(&cols, 1, MPI_INT, 0, 0, parent, &etat);
		printf ("Coordinateur : Reception du nombre de colonnes %d !\n", cols);

        //OLD
		//temperatures =  new float[cols * rows];

        for(int i =0;i< 9;i++) {
            float temp[10] = {0,0,0,0,0,0,0,0,0,0};
            temperatures[i] =  temp;
        }

		for (int i=1; i<10; i++)	{
			for (int j=1; j<cols * rows + 1; j++)	{

		        float temperatureToSend = temperature;

				//printf ("Coordinateur : Envoi vers l'esclave n°%d de la temperature ambiante (%f°C).\n", j, temperature);
				MPI_Send (&temperature, 1, MPI_FLOAT,j, 0, MPI_COMM_WORLD);
			}

			for (int k=1; k<cols * rows + 1; k++)	{
				MPI_Recv(storeTemperature, 10, MPI_FLOAT,k, 0, MPI_COMM_WORLD, &etat);
				//printf ("Coordinateur : Reception de l'esclave n°%d: %f°C \n", k, temperature);
				temperatures[k-1] = storeTemperature;
			}

			printGrid(temperatures,rows,cols);
			printSVG(temperatures,rows,cols, temperature);
		}

		char response = 'K';
		MPI_Send(&response, 1, MPI_CHAR, 0, 0, parent);
		printf ("Coordinateur : Envoi vers le pere !\n");
	}

	printf ("Coordinateur : FIN !\n");

	MPI_Finalize();
	return 0;
}
