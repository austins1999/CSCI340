#include <pthread.h>
#include <stdio.h>
#include <unistd.h>  // for sleep()
#include <math.h>    // for sqrt()

int global;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

#define NUMTIMES 100

#define MYLOCK pthread_mutex_lock(&mutex)
#define MYUNLOCK pthread_mutex_unlock(&mutex)
//#define MYLOCK
//#define MYUNLOCK

void* fooAPI(void* param)
{
  int i;
  
  MYLOCK;
  for (i = 0; i < NUMTIMES; i++)
    global++;
  MYUNLOCK;
  
  return 0;
}

int main(int argc, char** argv)
{
  pthread_t thread;
  int i, j;
  double sum;

  for (j = 0; j < 100000; j++) {
    global = 0;
    
    pthread_create(&thread, NULL, fooAPI, NULL);

    // do some computation that takes a little time...
    sum = 0;
    for (i = 0; i < 1000; i++) {
      sum += 3*i+sqrt(i);
    }

    // Now we need to lock to use the shared resource.
    for (i = 0; i < NUMTIMES; i++) {
      MYLOCK;
      global++;
      MYUNLOCK;
    }

    pthread_join(thread, NULL);

    if (global != 2*NUMTIMES) {
      printf("global = %d\n", global);
    }
  }

  return 0;
}
