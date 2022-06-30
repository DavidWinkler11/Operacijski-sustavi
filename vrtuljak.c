#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <semaphore.h>

const int N = 4;

int ID1, ID2, ID3;

sem_t *slobodna_mjesta;
sem_t *pun_vrtuljak;
sem_t *kraj_voznje;

void posjetitelj()
{
    sem_wait(slobodna_mjesta); // Cekanje da se oslobodi vrtuljak
    printf("Posjetitelj sjeda\n");
    sem_post(pun_vrtuljak); // Zauzeli smo jedno mjesto na vrtuljku
    sem_wait(kraj_voznje);  // Cekamo kraj voznje
    printf("Posjetitelj silazi\n");
    sem_post(slobodna_mjesta); // Oslobadam mjesto na vrtuljku
}

void vrtuljak()
{
    while (1)
    {
        for (int i = 0; i < N; i++)
        {
            sem_post(slobodna_mjesta); // Oslobodi jedno mjesto
        }

        for (int i = 0; i < N; i++)
        {
            sem_wait(pun_vrtuljak); // Cekaj da se popune sva mjesta
        }

        printf("\nPokrecem vrtuljak\n");
        sleep(2);
        printf("\nZaustavljam vrtuljak\n");

        for (int i = 0; i < N; i++)
        {
            sem_post(kraj_voznje);
        }
    }
}

int main()
{
    ID1 = shmget(IPC_PRIVATE, sizeof(sem_t), 0600);
    slobodna_mjesta = shmat(ID1, NULL, 0);
    shmctl(ID1, IPC_RMID, NULL);
    sem_init(slobodna_mjesta, 1, 0);

    ID2 = shmget(IPC_PRIVATE, sizeof(sem_t), 0600);
    pun_vrtuljak = shmat(ID2, NULL, 0);
    shmctl(ID2, IPC_RMID, NULL);
    sem_init(pun_vrtuljak, 1, 0);

    ID3 = shmget(IPC_PRIVATE, sizeof(sem_t), 0600);
    kraj_voznje = shmat(ID3, NULL, 0);
    shmctl(ID3, IPC_RMID, NULL);
    sem_init(kraj_voznje, 1, 0);

    if (fork() == 0)
    {
        vrtuljak();
    }

    fork();
    fork();
    // fork();

    while (1)
    {
        posjetitelj();
        sleep(5);
    }

    sem_destroy(slobodna_mjesta);
    shmdt(slobodna_mjesta);

    sem_destroy(pun_vrtuljak);
    shmdt(pun_vrtuljak);

    sem_destroy(kraj_voznje);
    shmdt(kraj_voznje);

    return 0;
}
