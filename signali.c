#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <pthread.h>

int Id; /* identifikacijski broj segmenta */

int broj;
int *ZajednickaVarijabla = 0; // za procese
int RadnaIzlazna = 0;         // za dretve

FILE *file;
int b;

void brisi(int sig)
{
   printf("Brisem! \n");
   /* oslobađanje zajedničke memorije */
   (void)shmdt((char *)ZajednickaVarijabla);
   (void)shmctl(Id, IPC_RMID, NULL);
   exit(0);
}

void writeToFile(int x)
{
   file = fopen("file.txt", "a");
   fprintf(file, "%d\n", x);
   fclose(file);
}

void *ulazna()
{

   for (int k = 0; k < broj; k++)
   { // određeni broj random brojeva

      do
      { // RADNO ČEKANJE
         sleep(1);
      } while (*ZajednickaVarijabla != 0);

      for (int j = 0; j < rand() % 5 + 1; j++)
      { // "Svakih 1-5 sekundi
         sleep(1);
      }

      *ZajednickaVarijabla = rand() % 100 + 1;
      printf("\nULAZNA DRETVA: generiran je broj %d\n", *ZajednickaVarijabla);
   }
}

void *radna()
{
   for (int k = 0; k < broj; k++)
   {

      do
      { // RADNO ČEKANJE
         sleep(1);
         // printf("Zapeo sam! \n");

      } while (*ZajednickaVarijabla == 0);

      RadnaIzlazna = *ZajednickaVarijabla + 1;
      printf("RADNA DRETVA: pročitan broj %d i povećan na %d\n", *ZajednickaVarijabla, RadnaIzlazna);

      *ZajednickaVarijabla = 0;
   }
}

void *izlazna()
{

   for (int k = 0; k < broj; k++)
   {

      do
      {

         sleep(1);
      } while (RadnaIzlazna == 0);

      printf("IZLAZNI PROCES: broj upisan u datoteku %d\n\n", RadnaIzlazna);
      writeToFile(RadnaIzlazna);
      RadnaIzlazna = 0;
   }
}

int main(void)
{
   int x;
   srand(time(NULL));

   printf("Upisite broj: ");
   scanf("%d", &x);
   printf("\n");

   broj = x;

   // ZAUZIMANJE ZAJEDNIČKE MEMORIJE
   Id = shmget(IPC_PRIVATE, sizeof(int), 0600);

   if (Id == -1)
   {
      printf("Nema zajedničke memorije");
      exit(1); // NEMA ZAJEDNIČKE MEMORIJE - GREŠKA
   }

   ZajednickaVarijabla = (int *)shmat(Id, NULL, 0);
   *ZajednickaVarijabla = 0;

   struct sigaction act;
   sigaction(SIGINT, &act, NULL); // U SLUČAJU PREKIDA BRIŠI MEMORIJU

   pthread_t thr_id, thread_id[2];

   // POKRETANJE PARALELNIH PROCESA
   if (fork() == 0)
   {

      if (pthread_create(&thr_id, NULL, ulazna, NULL) != 0)
      {
         printf("Greska pri stvaranju dretve!\n");
         exit(1);
      }
      else
      {
         printf("Pokrenuta ULAZNA DRETVA \n");
      }

      pthread_join(thr_id, NULL);
      exit(0);
   }

   if (fork() == 0)
   {
      if (pthread_create(&thread_id[0], NULL, radna, NULL) != 0)
      {
         printf("Greska pri stvaranju dretve!\n");
         exit(1);
      }
      else
      {
         printf("Pokrenuta RADNA DRETVA\n");
      }

      if (pthread_create(&thread_id[1], NULL, izlazna, NULL) != 0)
      {
         printf("Greska pri stvaranju dretve!\n");
         exit(1);
      }
      else
      {
         printf("Pokrenuta IZLAZNA DRETVA\n");
      }

      pthread_join(thread_id[0], NULL);
      pthread_join(thread_id[1], NULL);

      exit(0);
   }

   (void)wait(NULL);
   (void)wait(NULL);
   printf("\nZavršila ULAZNA DRETVA\n");
   printf("Završila RADNA DRETVA\n");
   printf("Završio IZLAZNI PROCES\n");
   brisi(0);

   return 0;
}