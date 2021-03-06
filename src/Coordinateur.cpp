#include <mpi.h>
#include <stdio.h>
#include <fstream>
#include <string>
#include <sstream>

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
float lastAverageTemp = 1000;

//Valeur a changer pour modifier la durée du refroidissement
float differenceBetweenMinMaxValueToStopCooldown = 0.1;

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
    int ambiantTempMargin = 50;
    //taille des rectangle des mini-plaque
    int widthSubRectangle = 100;
    int heightSubRectangle = 100;

    //taille des rectangles globaux
    int widthRectangle = 320; //20px pour la marge
    int heightRectangle = 320;

    //taille total SVG
    int widthTotal = cols * widthRectangle;
    int heightTotal = rows * heightRectangle;


    std::string name = "Metallica" + std::to_string(step) + ".html";
    cout << name << "/***********************************************************************/"<< endl;
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
    outfile << "width=\"" << widthTotal + (ambiantTempMargin *2) << "px\" height=\""<< heightTotal + (ambiantTempMargin *2) <<"px\">" << std::endl;

    //root rectangle for ambiant temp
    outfile << "<rect width=\""<< widthTotal + (ambiantTempMargin *2) <<"\" height=\""<< heightTotal + (ambiantTempMargin *2) <<"\" fill=\"" << getColorByTemperature(ambiantTemp) << "\" stroke=\"black\"/>" << std::endl;
    outfile << "<text x=\""<< 25 <<"\" y=\""<< 25 <<"\" width=\""<< widthRectangle <<"\" height=\""<< heightRectangle <<"\" fill=\"#CCCCCC\"> Ambiant Temperature : " << ambiantTemp <<"</text>"<< std::endl;

    //creation de la plaque maitre
    int count = -1;
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            //creation des plaques filles
            count++;
            float* subtable = table[count];
            int ypos = (i * widthRectangle) + ambiantTempMargin; //i creer les lignes donc la position y
            int xpos = (j * heightRectangle) + ambiantTempMargin; // j creer les colonnes donc le x

            //on creer le rectangle qui contiendra les petites plaques
            outfile << "<rect x=\""<< xpos <<"\" y=\""<< ypos <<"\" width=\""<< widthRectangle <<"\" height=\""<< heightRectangle <<"\" fill-opacity=\"0.1\" fill=\"white\"/>" << std::endl;

            //On creer les mini-plaques (toujours 3x3)
            int subcount = 0;
            for (int k = 0; k < 3; k++)
            {
                for (int l = 0; l < 3; l++)
                {
                    subcount++;
                    float value = subtable[subcount];
                    int ypos2 = ypos + (k * widthSubRectangle); //i creer les lignes donc la position y
                    int xpos2 = xpos + (l * heightSubRectangle); // j creer les colonnes donc le x
                    outfile << "<rect x=\""<< xpos2 <<"\" y=\""<< ypos2 <<"\" width=\""<< widthSubRectangle <<"\" height=\""<< heightSubRectangle <<"\" fill=\"" << getColorByTemperature(value) << "\" stroke=\"black\"/>" << std::endl;
                    outfile << "<text x=\""<< xpos2 + 25 <<"\" y=\""<< ypos2 + 25 <<"\" width=\""<< widthSubRectangle <<"\" height=\""<< heightSubRectangle <<"\" fill=\"#333333\">" << value <<"</text>"<< std::endl;

                }
            }
        }
        printf("\n");
    }

    outfile <<   "</svg> </body></html>" << std::endl;
    outfile.close();
    step++;
}

float getAvgTemperature(float **gridTemperatures)
{
    float* allGridsTemps = new float[12];
    float totalTemperature = 0.0f;

    for(int i=0; i<12; i++)
    {
        float gridTemp = 0.0f;
        for(int j=1 ; j<10 ; j++){
            gridTemp += gridTemperatures[i][j];
        }
        allGridsTemps[i] = gridTemp / 9;
    }

    for(int i=0 ; i<12 ; i++){
        totalTemperature += allGridsTemps[i];
    }

    return (totalTemperature / 12);
}

bool isCooldownTerminated(float **gridTemperatures){

    float newAverageTemp = getAvgTemperature(gridTemperatures);

    float differenceBetweenMinMaxValue = lastAverageTemp - newAverageTemp;

    printf("OLD average: %f\n", lastAverageTemp);
    printf("NEW average: %f\n", newAverageTemp);

    lastAverageTemp = newAverageTemp;

    cout << "ON TERMINE ?(" << differenceBetweenMinMaxValue << " < " << differenceBetweenMinMaxValueToStopCooldown << ") ? " << (differenceBetweenMinMaxValue < differenceBetweenMinMaxValueToStopCooldown) << endl;

    return differenceBetweenMinMaxValue < differenceBetweenMinMaxValueToStopCooldown;
}

int main( int argc, char *argv[] )
{
	int myrank;
	float temperature;
	float** temperatures = new float*[12];
	int rows = 2;
	int cols = 2;
	MPI_Comm parent;
	MPI_Status etat;
	MPI_Init (&argc, &argv);
	MPI_Comm_get_parent (&parent);
	MPI_Comm_rank (MPI_COMM_WORLD,&myrank);
	bool isNotEnd = true;
	MPI_Request requestNull;


	if (parent == MPI_COMM_NULL) {
		printf ("Coordinateur : Pas de pere !\n");
	} else {
		MPI_Recv(&temperature, 1, MPI_FLOAT, 0, 0, parent, &etat);
		printf ("Coordinateur : Reception de la temperature ambiante %f !\n", temperature);

		MPI_Recv(&rows, 1, MPI_INT, 0, 0, parent, &etat);
		printf ("Coordinateur : Reception du nombre de lignes %d !\n", rows);

		MPI_Recv(&cols, 1, MPI_INT, 0, 0, parent, &etat);
		printf ("Coordinateur : Reception du nombre de colonnes %d !\n", cols);

        for(int i =0;i< 12;i++) {
            float* temp = new float[10];
            temp[0] = 100;
            temp[1] = 100;
            temp[2] = 100;
            temp[3] = 100;
            temp[4] = 100;
            temp[5] = 100;
            temp[6] = 100;
            temp[7] = 100;
            temp[8] = 100;
            temp[9] = 100;
            temperatures[i] =  temp;
        }

		while(isNotEnd)	{

          //  if(isCooldownTerminated(temperatures)) {
           if(isCooldownTerminated(temperatures)) {
                isNotEnd = false;
                temperature = -500;
            }

            //On continue le processus
            for (int j=1; j<cols * rows + 1; j++)	{
                //Si il n'y a plus assez de refroidissement
                MPI_Send (&temperature, 1, MPI_FLOAT,j, 0, MPI_COMM_WORLD);
            }

            if(temperature != -500){

                for (int k=1; k<cols * rows + 1; k++)	{

                    float* storeTemperature = new float[9];
                    MPI_Recv(storeTemperature, 10, MPI_FLOAT,k, 0, MPI_COMM_WORLD, &etat);

                    //On met a jour les temperatures de la grille globale avec ce qu'on reçoit des esclaves
                    temperatures[k-1] = storeTemperature;
                }

                printSVG(temperatures,rows,cols, temperature);
            } else {
                //boucle qui permet d'attendre que tous les esclaves se terminent
                for (int k=1; k<cols * rows + 1; k++) {
                    float temp;
                    MPI_Recv(&temp, 1, MPI_FLOAT,k, 0, MPI_COMM_WORLD, &etat);
                }

                //Libération de la mémoire résevée par le tableau
                for(int i =0;i< 12;i++) {
                    delete[] temperatures[i];
                }
                delete[] temperatures;

                char response = 'K';
                MPI_Send(&response, 1, MPI_CHAR, 0, 0, parent);
                printf ("Coordinateur : Envoi vers le pere !\n");
            }
		}


	}

	printf ("Coordinateur : FIN !\n");

	MPI_Finalize();
	return 0;
}
