#include <stdio.h>
#include <pthread.h>

volatile int turn = 0;

// Things to note:  alternating cricical sections of p0 and p1

void* p0(void* arg)
{
  int i;
  char* proc_name = (char*) arg;

  for (i = 0; i < 10; i++) {
    while (turn != 0)
      ;    // NOTE:  empty body of loop
    printf("%s in critical section\n", proc_name);
    turn = 1;
  }
  
  return NULL;   // technically need to return a pointer to void
}

void* p1(void* arg)
{
  int i;
  char* proc_name = (char*) arg;

  for (i = 0; i < 10; i++) {
    while (turn != 1)
      ;    // NOTE:  empty body of loop
    printf("%s in critical section\n", proc_name);
    turn = 0;
  }

  return NULL;   // technically need to return a pointer to void
}

int main()
{
  pthread_t thread0, thread1;
  const char* name0 = "process 0";
  const char* name1 = "process 1";
  
  pthread_create(&thread0, NULL, p0, (void*) name0);  // create 2 threads
  pthread_create(&thread1, NULL, p1, (void*) name1);

  pthread_join(thread0, NULL);  // wait for both threads to complete
  pthread_join(thread1, NULL); 

  printf("main thread done after joins\n");
  
  return 0;               // technically main() needs to return an int
}
