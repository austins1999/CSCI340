#include <stdio.h>
#include <pthread.h>

typedef enum { false = 0, true = 1 } boolean;
volatile boolean flag[2] = { false, false};
volatile int turn;   // NOTE:  does not need to be initialized

int count;
/*
  From stackoverflow: 

11626928/petersons-algorithms-behavior-for-various-optimization-flags

So if your program writes to volatile variable a before reading
volatile variable b, the write and read must occur in that order. Note
however that this doesn't stop the CPU from reordering the memory
accesses out from under you, if your architecture allows such a thing.

  Also see:

43444676/peterson-algorithm-about-data-modification-in-lock-visible-to-second-thread-with

  which discusses compiler reordering under optimization as well as
  memory write/read ordering.
 */

void* p0(void* arg)
{
  int i;
  char* proc_name = (char*) arg;

  for (i = 0; i < 10; i++) {
    flag[0] = true;
    turn = 1;
    asm volatile ("mfence" : : : "memory");
    while (flag[1] && turn == 1)
      ; // do nothing
    printf("%s in critical section\n", proc_name);
    count++;
    
    asm volatile ("" : : : "memory"); // here, forbidden compiler reorder
    flag[0] = false;
  }

  return NULL;   // technically need to return a pointer to void
}

void* p1(void* arg)
{
  int i;
  char* proc_name = (char*) arg;

  for (i = 0; i < 10; i++) {
    flag[1] = true;
    turn = 0;
    asm volatile ("mfence" : : : "memory");
    while (flag[0] && turn == 0)
      ; // do nothing
    printf("%s in critical section\n", proc_name);
    count++;
    asm volatile ("" : : : "memory"); // here, forbidden compiler reorder
    flag[1] = false;
  }

  return NULL;   // technically need to return a pointer to void
}

int main()
{
  int i;
  pthread_t thread0, thread1;
  const char* name0 = "process 0";
  const char* name1 = "process 1";

  for (i = 0; i < 1000000; i++) {
    count = 0;

    pthread_create(&thread0, NULL, p0, (void*) name0);  // create 2 threads
    pthread_create(&thread1, NULL, p1, (void*) name1);

    pthread_join(thread0, NULL);  // wait for both threads to complete
    pthread_join(thread1, NULL); 

    if (count != 20) {
      printf("count = %d at index %d\n", count, i);
      break;
    }
  }
  
  return 0;               // technically main() needs to return an int
}
