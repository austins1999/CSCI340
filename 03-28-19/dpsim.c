#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_PHILOSOPHERS 5

static pthread_cond_t fork_ready[NUM_PHILOSOPHERS] = {PTHREAD_COND_INITIALIZER};
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static int num_eating = 0;

static int fork[NUM_PHILOSOPHERS];

void delay(long nanosec)
{
  struct timespec t_spec;

  t_spec.tv_sec = 0;
  t_spec.tv_nsec = nanosec;

  nanosleep(&t_spec, NULL);
}

void eat(long i)
{
  int j;

  printf("philosopher %ld is eating...\n", i);
  if (num_eating == 2) {
    printf("%d philosophers are eating...\n", num_eating);
    printf("\t");
    for (j = 0; j < NUM_PHILOSOPHERS; j++) {
      printf("%d ", fork[j]);
    }
    printf("\n");
  }
  else if (num_eating > 2) {
    printf("\toops! more than two philosophers eating\n");
    exit(0);
  }
  
  //delay(999999999);
}

void think(long i)
{
  printf("philosopher %ld is thinking...\n", i);
  //delay(999999999);
}

void get_forks(long i)
{  
  long left = i;
  long right = (i + 1) % NUM_PHILOSOPHERS;
  long j;

  pthread_mutex_lock(&mutex);
  
  while (fork[left] != -1)
    pthread_cond_wait(&fork_ready[left], &mutex);
  
  fork[left] = i;
  printf("\tphilosopher %ld has it's left fork(%ld)\n", i, left);

  printf("\t\t");
  for (j = 0; j < NUM_PHILOSOPHERS; j++) {
    printf("%d ", fork[j]);
  }
  printf("\n");
  
  while (fork[right] != -1)
    pthread_cond_wait(&fork_ready[right], &mutex);

  fork[right] = i;
  printf("\tphilosopher %ld has it's right fork(%ld)\n", i, right);
  
  num_eating++;

  pthread_mutex_unlock(&mutex);
}

void release_forks(long i)
{  
  long left = i;
  long right = (i + 1) % 5;

  pthread_mutex_lock(&mutex);
    
  fork[left] = -1;
  pthread_cond_signal(&fork_ready[left]);
  printf("\tphilosopher %ld released it's left fork(%ld)\n", i, left);
  
  fork[right] = -1;
  pthread_cond_signal(&fork_ready[right]);
  printf("\tphilosopher %ld released it's right fork(%ld)\n", i, right);

  num_eating--;  

  pthread_mutex_unlock(&mutex);
}

void* th_phil(void* th_phil_args)
{
  long i = (long) th_phil_args;
  
  printf("philosopher %ld starting up...\n", i);
  
  while (1) {
    think(i);
    get_forks(i);
    eat(i);
    release_forks(i);
  }
  
  return NULL;
}

int main(int argc, char** argv)
{
  long i;
  pthread_t philosophers[NUM_PHILOSOPHERS];

  for (i = 0; i < NUM_PHILOSOPHERS; i++) {
    fork[i] = -1;
  }

  for (i = 0; i < NUM_PHILOSOPHERS; i++) {
    if (pthread_create(&philosophers[i], NULL, th_phil, (void *) i) != 0) {
      fprintf(stderr, "can't create philosopher thread %ld\n", i);
      return 1;
    }
  }

  for (i = 0; i < NUM_PHILOSOPHERS; i++) {
    pthread_join(philosophers[i], NULL);
  }

  return 0;
}
