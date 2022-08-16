#include "func.h"
#include "../include/data_struct.h"
#include "os_sem.h"

int ReadBase() {
   int shmid;
   struct shmseg *shmp;
   shmid = shmget(SHM_KEY, sizeof(struct shmseg), 0644|IPC_CREAT);
   if (shmid == -1) {
      perror("Shared memory");
      return 1;
   }
 
   // Attach to the segment to get a pointer to it.
   shmp = (shmseg*)(shmat(shmid, NULL, 0));
   if (shmp == (void *) -1) {
      perror("Shared memory attach");
      return 1;
   }

   sem_t *s1 = nullptr;
	s1 = sem_open("sem_print1", O_CREAT, 0777, 0);  
 	if(SEM_FAILED == s1)
	{
		perror("sem_open");
      return -1;
	} else {
      printf("Create S1 Successfully! \n");
   }

   /* Transfer blocks of data from shared memory to stdout*/
   while (shmp->complete != 1) {
      sem_wait(s1);

      printf("segment contains : \n\"%s\"\n", shmp->buf);
      if (shmp->cnt == -1) {
         perror("read");
         return 1;
      }
      printf("Reading Process: Shared Memory: Read %d bytes\n", shmp->cnt);
      sleep(3);
   }
   printf("Reading Process: Reading Done, Detaching Shared Memory\n");
   if (shmdt(shmp) == -1) {
      perror("shmdt");
      return 1;
   }
   printf("Reading Process: Complete\n");

   return 0;
}

void print1(sem_t* s1, sem_t* s2) { 

   printf("Print1 Start!\n");

   int i = 0;
   while (true) {
      // WaitSem(s2);
      sem_wait(s1);

      printf("P1-print1: %d \n", i++);
      sleep(3);
      sem_post(s2);

      // PostSem(s1);
   }

}

void print2(sem_t* s1, sem_t* s2) { 

   printf("Print2 Start!\n");

   int i = 0;
   while (true) {
      // WaitSem(s2);
      
      sem_post(s1);

      printf("P1-print2: %d \n", i++);
      sleep(3);

      sem_wait(s2);
   
      // PostSem(s1);
   }

}



void SemphoreBaseTest() {
   printf("SemphoreBaseTest P1 \n");

	sem_t *s1, *s2;
	s1 = sem_open("sem_print1", O_CREAT, 0777, 0);  
	if(SEM_FAILED == s1)
	{
		perror("sem_open");
	} else {
      printf("Create S1 Successfully! \n");
   }


	s2 = sem_open("sem_print2", O_CREAT, 0777, 1);  
	if(SEM_FAILED == s2)
	{
		perror("sem_open");
	}else {
      printf("Create S2 Successfully! \n");
   }

   auto thread1 = std::thread(print1, s1, s2);
   // auto thread2 = std::thread(print2, s1, s2);

   if (thread1.joinable()) thread1.join();
   // if (thread2.joinable()) thread2.join();

	// print(s1, s2);  
}