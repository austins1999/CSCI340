#include <stdio.h>
#include <pthread.h>

typedef enum { false = 0, true = 1 } boolean;
volatile boolean flag[2] = { false, false};

void* p0(void* arg)
{
  int i;
  char* proc_name = (char*) arg;

  for (i = 0; i < 10; i++) {
    flag[0] = true;
    while (flag[1])
      ;    // NOTE:  empty body of loop
    printf("%s in critical section\n", proc_name);
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
    while (flag[0])
      ;    // NOTE:  empty body of loop
    printf("%s in critical section\n", proc_name);
    flag[1] = false;
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
