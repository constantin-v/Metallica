#include <iostream>
#include <mpi.h>
#include <stdio.h>
#include <omp.h>

using namespace std;

//global
int rows = 3;
int cols = 4;
float ambientTemperature = 20;

void initialiseTemperatures();

int main(int argc, char *argv[])
{

	printf("Maitre : DEBUT DU PROGRAMME.\n");
	int i,  errCodes[cols * rows];
	char returnCodeFromChildren;
	MPI_Status etat;
	MPI_Comm intercomm;

	char *cmds[2] = {
		"./coordinateur",
		"./esclave",
	};

	int nbInstances[2] = {
		1,
		cols * rows
	};

	MPI_Info infos[2] = {
		MPI_INFO_NULL,
		MPI_INFO_NULL
	};

	MPI_Init(&argc, &argv);
	MPI_Comm_spawn_multiple(
		2,
		cmds,
		MPI_ARGVS_NULL,
		nbInstances,
		infos,
		0,
		MPI_COMM_WORLD,
		&intercomm,
		errCodes
	);

	printf("Pere : Toutes les instances sont lancees.\n");

    //Communication pere --> coordinateur
    float temperatureAmbiantToSend = 20;
    //printf ("Pere : Envoi vers le coordinateur de la temperature ambiante (%f°C)\n", temperatureAmbiantToSend);
    MPI_Send (&temperatureAmbiantToSend, 1, MPI_FLOAT, 0, 0, intercomm);

    //printf ("Pere : Envoi vers le coordinateur du nombre de lignes %d \n", rows);
    MPI_Send (&rows, 1, MPI_INT, 0, 0, intercomm);

    //printf ("Pere : Envoi vers le coordinateur du nombre de colonnes %d \n", cols);
    MPI_Send (&cols, 1, MPI_INT, 0, 0, intercomm);

	// Communication pere -> fils

    //Creation du tableau de temperature a envoyer aux esclaves
	float* temperatureToSend = new float[10];
	temperatureToSend[0] = 0.0f; //On réserve une case pour mettre l'index de l'esclave qui envoi ses données
    temperatureToSend[1] = 50.0f;
    temperatureToSend[2] = 50.0f;
    temperatureToSend[3] = 50.0f;
    temperatureToSend[4] = 50.0f;
    temperatureToSend[5] = 90.0f;
    temperatureToSend[6] = 50.0f;
    temperatureToSend[7] = 50.0f;
    temperatureToSend[8] = 50.0f;
    temperatureToSend[9] = 50.0f;


	for (i=1; i< rows * cols + 1; i++)	{
        //JALON 7
        //On envoi la grid des températures à chaque esclave sous forme de tableau de float
		MPI_Send (temperatureToSend, 10, MPI_FLOAT, i, 0, intercomm);

        //printf ("Pere : Envoi du nombre de ligne vers l'esclave n%d \n", i);
        MPI_Send (&rows, 1, MPI_INT, i, 0, intercomm);

        //printf ("Pere : Envoi du nombre de colonnes vers l'esclave n%d \n", i);
        MPI_Send (&cols, 1, MPI_INT, i, 0, intercomm);

	}

    MPI_Recv(&returnCodeFromChildren, 1, MPI_CHAR,0, 0, intercomm, &etat);
    printf ("Pere : Reception du coordinateur : %c \n", returnCodeFromChildren);


	printf ("Pere : Fin.\n");

	MPI_Finalize();
	return 0;

}


