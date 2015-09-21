#include <mpi.h>
#include <stdio.h>
#include <iostream>
#include <fstream>

using namespace std;

int step = 0;

void printGrid(float* table, int rows, int cols)
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

void printSVG(float* table, int rows, int cols)
{
	int widthRectangle = 20;
	int heightRectangle = 20;
	int widthTotal = cols * widthRectangle;
	int heightTotal = rows * heightRectangle;
	
	std::string name = "Metallica" + std::to_string(step) + ".html";	
	std::ofstream outfile (name);
	outfile << "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"no\"?>";
	outfile << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" ";
	outfile << "\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">";
	outfile << "<html xmlns=\"http://www.w3.org/1999/xhtml\"> ";
	outfile << "<head>";
	outfile << "<title>Refroidissement d'une plaque de métal</title>";
	outfile << "</head>";
	outfile << "<body>";
	outfile << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\"";
	outfile << "width=\"" << widthTotal << "px\" height=\""<< heightTotal <<"px\">" << std::endl;

	int count = -1;
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			count++;
			float value = table[count];
			int xpos = i * widthRectangle;
			int ypos = j * heightRectangle;
			outfile << "<rect x=\""<< xpos <<"\" y=\""<< ypos <<"\" width=\""<< widthRectangle <<"\" height=\""<< heightRectangle <<"\" fill=\"none\" stroke=\"black\"/>" << std::endl;
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
	float* temperatures;
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

		
		temperatures =  new float[cols * rows];

		for (int i=1; i<10; i++)	{
			for (int j=1; j<cols * rows + 1; j++)	{

		        float temperatureToSend = temperature; 

				//printf ("Coordinateur : Envoi vers l'esclave n°%d de la temperature ambiante (%f°C).\n", j, temperature);
				MPI_Send (&temperature, 1, MPI_FLOAT,j, 0, MPI_COMM_WORLD);					
			}

			for (int k=1; k<cols * rows + 1; k++)	{
				MPI_Recv(&temperature, 1, MPI_FLOAT,k, 0, MPI_COMM_WORLD, &etat);
				//printf ("Coordinateur : Reception de l'esclave n°%d: %f°C \n", k, temperature);
				temperatures[k-1] = temperature;
			}
						
			printGrid(temperatures,rows,cols);
			printSVG(temperatures,rows,cols);
		}

		char response = 'K';
		MPI_Send(&response, 1, MPI_CHAR, 0, 0, parent);
		printf ("Coordinateur : Envoi vers le pere !\n");
	}

	printf ("Coordinateur : FIN !\n");

	MPI_Finalize();
	return 0;
}
