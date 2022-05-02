#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>

/* funkcije za obradu signala, navedene ispod main-a */
void obradi_SIGUSR1(int sig);
void obradi_SIGTERM(int sig);

int broj;
FILE *status;
FILE *obrada;

int nije_kraj = 1;
int i;

int main()
{
    struct sigaction act;

    /* 1. maskiranje signala SIGUSR1 */
    act.sa_handler = obradi_SIGUSR1; /* kojom se funkcijom signal obrađuje */
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;               /* naprednije mogućnosti preskočene */
    sigaction(SIGUSR1, &act, NULL); /* maskiranje signala preko sučelja OS-a */

    /* 2. maskiranje signala SIGTERM */
    act.sa_handler = obradi_SIGTERM;
        sigemptyset(&act.sa_mask);
    sigaction(SIGTERM, &act, NULL);

    printf("Program s PID=%ld krenuo s radom\n", (long)getpid());

    status = fopen("status.txt", "r");
    fscanf(status, "%d", &broj);
    fclose(status);

    printf("Broj zapisan u status.txt je: %d\n", broj); // PROVJERA

    if (broj == 0)
    {
        obrada = fopen("obrada.txt", "r");
        while ((getc(obrada)) != EOF)
        {
            fscanf(obrada, "%d", &broj);
        }
        broj = sqrt(broj);
        fclose(obrada);
    }

    printf("Prvi broj koji slijedi u obradi %d\n", broj + 1);
    int nula = 0; // POSTAVLJANJE STATUSA NA NULU
    status = fopen("status.txt", "w");
    fprintf(status, "%d", nula);
    fclose(status);

    /* neki posao koji program radi; ovdje samo simulacija */

    while (nije_kraj)
    {
        obrada = fopen("obrada.txt", "a");
        fprintf(obrada, "%d\n", (broj + 1) * (broj + 1));
        fclose(obrada);

        printf("%d\n", (broj + 1) * (broj + 1)); // PROVJERA
        broj++;

        for (int j = 0; j < 5; j++)
        {
            sleep(1);
        }
    }
    printf("Program s PID=%ld zavrsio s radom\n", (long)getpid());
    return 0;
}

void obradi_SIGUSR1(int sig)
{
    printf("BROJ U OBRADI JE: %d\n", broj); // PROVJERA
}

void obradi_SIGTERM(int sig)
{
    status = fopen("status.txt", "w");
    fprintf(status, "%d", broj);
    fclose(status);

    status = fopen("status.txt", "r");
    fscanf(obrada, "%d", &broj);
    fclose(status);

    // printf("BROJ NA KRAJU JE: %d\n", broj); PROVJERA
    printf("Primio signal SIGTERM, spremam broj u obradi (%d) u status.txt!\n", broj);
    nije_kraj = 0;
}

