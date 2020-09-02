#include <stdio.h>
#include <pthread.h>

typedef enum { false = 0, true = 1 } boolean;
volatile boolean flag[2] = { false, false};
volatile int turn = 1;

int count;

void* p0(void* arg)
{
  int i;
  //char* proc_name = (char*) arg;

  for (i = 0; i < 10; i++) {
    flag[0] = true;
    while (flag[1]) {
      if (turn == 1) {
	flag[0] = false;
	while (turn == 1)
	  ;  // do nothing
	flag[0] = true;
      }
    }
    //printf("%s in critical section\n", proc_name);
    count++;
    turn = 1;
    flag[0] = false;
  }

  return NULL;   // technically need to return a pointer to void
}

void* p1(void* arg)
{
  int i;
  //char* proc_name = (char*) arg;

  for (i = 0; i < 10; i++) {
    flag[1] = true;
    while (flag[0]) {
      if (turn == 0) {
	flag[1] = false;
	while (turn == 0)
	  ; // do nothing
	flag[1] = true;
      }
    }
    //printf("%s in critical section\n", proc_name);
    count++;
    turn = 0;
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
