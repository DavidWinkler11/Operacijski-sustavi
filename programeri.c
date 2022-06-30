#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <errno.h>
#include <math.h>

pthread_mutex_t monitor;
pthread_cond_t red[2];

int br_programera[2] = {0};
int pojeli[2] = {0};      // broj programera koji su pojeli
int red_cekanja[2] = {0}; // broj programera koji cekaju u redu
int n = 1;

void udji(int vrsta)
{
    pthread_mutex_lock(&monitor); // ulaz u monitor
    red_cekanja[vrsta]++;

    while ((br_programera[1 - vrsta] > 0) || (pojeli[vrsta] > n && red_cekanja[1 - vrsta] > 0))
    {
        printf("Cekam: %d\n", vrsta);
        pthread_cond_wait(&red[vrsta], &monitor);
    }

    printf("Usao: %d\n", vrsta);

    br_programera[vrsta]++;
    red_cekanja[vrsta]--;
    pojeli[1 - vrsta] = 0;

    pthread_mutex_unlock(&monitor); // izlaz iz monitora
}

void izadji(int vrsta)
{
    pthread_mutex_lock(&monitor); // ulaz u monitor

    br_programera[vrsta]--;
    pojeli[vrsta]++;

    printf("Izasao: %d\n", vrsta);

    if (br_programera[vrsta] == 0)
    {
        pthread_cond_broadcast(&red[1 - vrsta]);
    }

    pthread_mutex_unlock(&monitor); // izlaz iz monitora
}

void *programer()
{
    int mojaVrsta = rand() % 2;
    udji(mojaVrsta); // cekanje u redu
    printf("Jedem ja koji sam %d\n", mojaVrsta);
    sleep(4);
    izadji(mojaVrsta);
}

int main()
{
    pthread_t thr_id;
    srand(time(NULL));

    for (int i = 0; i < 20; i++)
    {
        pthread_create(&thr_id, NULL, programer, NULL);
        sleep(2);
    }

    for (int i = 0; i < 20; i++)
    {
        pthread_join(thr_id, NULL);
    }

    return 0;
}
