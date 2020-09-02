/*
Original work Copyright (c) 2017 Anthony Leclerc <leclerca@cofc.edu>

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.
*/

// to compile enter:
//    cc -Wall security_guard.c binary_semaphore.c -lpthread (-o security_guard)
// NOTE: you will get a warning about unused variable, sthreads, but
// this will go away, as you complete the code.

#include <stdio.h>
#include <stdlib.h>  // for exit(), rand(), strtol()
#include <pthread.h>
#include <time.h>    // for nanosleep()
#include <errno.h>   // for EINTR error check in millisleep()

#include "binary_semaphore.h"

// you can adjust next two values to speedup/slowdown the simulation
#define MIN_SLEEP      20   // minimum sleep time in milliseconds
#define MAX_SLEEP     100   // maximum sleep time in milliseconds

#define START_SEED     11   // arbitrary value to seed random number generator

// guard_state with a value of k:
//          k < 0 : means guard is waiting to enter the room
//          k = 0 : means guard is in the hall of department
//          k > 0 : means guard is IN the room
int guard_state;         // waiting, in the hall, or in the room
int num_students;        // number of students in the room

// TODO:  list here the "handful" of semaphores you will need to synchronize
//        I've listed one you will need for sure, to "get you going"
binary_semaphore mutex;  // to protect shared variables (including semaphores)
  //START ADDED SEMAPHORES
binary_semaphore studentExit;  //to control when a student can leave a room
  //NOTE: we can't use mutex again here since a student can always leave but
  // can't always enter, ie there are different criteria for the two actions
binary_semaphore guardEntry;  //To control when a guard can enter a room
binary_semaphore roomCleared;  //To control when a guard can leave a room
  //END

// will malloc space for seeds[] in the main
unsigned int *seeds;     // rand seeds for guard and students generating delays

// NOTE:  globals below are initialized by command line args and never changed !
int capacity;       // maximum number of students in a room
int num_checks;     // number of checks the guard makes

void millisleep(long millisecs)   // delay for "millisecs" milliseconds
{ // details of this function are unimportant for the assignment
  struct timespec req;
  req.tv_sec  = millisecs / 1000;
  millisecs -= req.tv_sec * 1000;
  req.tv_nsec = millisecs * 1000000;
  while(nanosleep(&req, &req) == -1 && errno == EINTR)
    ;
}

// generate random int in range [min, max]
int rand_range(unsigned int *seedptr, long min, long max)
{ // details of this function are unimportant for the assignment
  // using reentrante version of rand() function (because multithreaded)
  // NOTE: however, overall behavior of code will still be non-deterministic
  return min + rand_r(seedptr) % (max - min + 1);
}

void study(long id)  // student studies for some random time
{ // details of this function are unimportant for the assignment
  int ms = rand_range(&seeds[id], MIN_SLEEP, MAX_SLEEP);
  printf("student %2ld studying in room with %2d students for %3d millisecs\n",
   id, num_students, ms);
  millisleep(ms);
}

void do_something_else(long id)    // student does something else
{ // details of this function are unimportant for the assignment
  int ms = rand_range(&seeds[id], MIN_SLEEP, MAX_SLEEP);
  millisleep(ms);
}

void assess_security()  // guard assess room security
{ // details of this function are unimportant for the assignment
  // NOTE:  we have (own) the mutex when we first enter this routine
  guard_state = 1;     // positive means in the room
  int ms = rand_range(&seeds[0], MIN_SLEEP, MAX_SLEEP/2);
  printf("\tguard assessing room security for %3d millisecs...\n", ms);
  millisleep(ms);
  printf("\tguard done assessing room security\n");
}

void guard_walk_hallway()  // guard walks the hallway
{ // details of this function are unimportant for the assignment
  int ms = rand_range(&seeds[0], MIN_SLEEP, MAX_SLEEP/2);
  printf("\tguard walking the hallway for %3d millisecs...\n", ms);
  millisleep(ms);
}

// this function contains the main synchronization logic for the guard
void guard_check_room()
{
  // (COMPLETE) TODO: complete this routine to execute the behavior of the guard,
  // with proper synchronization.  You will need to determine how many
  // students are in the room to perform the appropriate action.
  // Depending on the number of students, you will either:
  //    * properly wait to enter, if some students are there
  //    * clear out the room, if capacity (or more) number of students
  //    * assess the security of the room, if no students are there
  //       (see function assess_security() above)

  // You will also need to properly maintain the global variable,
  // guard_state.  Once you have performed the appropriate action,
  // with proper synchronization, the guard will leave the room.

  // Remember, that whenever you access or change a global variable
  // (eg. num_students), you need to insure you are doing so in a
  // mutually exclusive fashion, for example, by calling

  semWaitB(&mutex); //Lock mutex to check num_students

  if(num_students == 0) {  //Room is empty, so enter
  assess_security();  //assess security for unspecified amount of time
  guard_state = 0;  //set guard_state to 0 (ie go back in hall)
  printf("\tguard left room\n");
  }

  else if(num_students >= capacity) {  //Capacity exceeded, so enter & clear
  guard_state = -2;
  printf("\tguard now entering room with %2d students\n", num_students);
  printf("\tguard clearing out room with %2d students\n", num_students);
  semWaitB(&roomCleared);  //wait in room until all students leave
  printf("\tguard done clearing out room\n");
  assess_security();  //assess security for unspecified amount of time
  guard_state = 0;  //set guard_state to 0 (ie go back in hall)
  printf("\tguard left room\n");
  }
  else {  //Wait until num_students >= capacity or = 0
  guard_state = -1;
  semSignalB(&mutex); //Unlock so students can enter while guard waits
  printf("\tguard waiting to enter room with %2d students\n", num_students);
  semWaitB(&guardEntry);
  semWaitB(&mutex); //Lock once again now that conditions are met
  printf("\tguard done waiting to enter room with %2d students\n", num_students);
  printf("\tguard clearing out room with %2d students\n", num_students);
  semWaitB(&roomCleared);  //wait in room until all students leave
  printf("\tguard done clearing out room\n");
  assess_security();  //assess security for unspecified amount of time
  guard_state = 0;  //set guard_state to 0 (ie go back in hall)
  printf("\tguard left room\n");
  }

  semSignalB(&mutex); //Unlock mutex

}

// this function contains the main synchronization logic for a student
void student_study_in_room(long id)
{
  // (COMPLETE) TODO: complete this routine to execute the behavior of a student,
  // with proper synchronization.  You will need to determine if the
  // guard is in the room.  You will also need to synchronize with the
  // guard, to clear out the room, and, to allow a possible waiting
  // guard to enter.  At the proper place, you will call the function
  // study(), above.  You will also need to properly maintain the
  // global variable, num_students.  When done, students leave the
  // room.

  semWaitB(&mutex);  //Lock the mutex for global variable alteration
  num_students++;  //Increment num_students
  if(num_students >= capacity && guard_state == -1) {  //Checks if capacity is exceeded
  semSignalB(&guardEntry);
  guard_state = -2;
  }
  semSignalB(&mutex);  //Unlock the mutex
  study(id);  //Student studies for unspecified period of time
  semWaitB(&studentExit); //Lock the exit semaphore
  if(num_students >= capacity && guard_state == -1) {  //Checks if capacity is exceeded
  semSignalB(&guardEntry);
  guard_state = -2;
  }
  num_students--; //Decrememnt num_students
  printf("student %2ld left the room (%2d students still in room)\n", id, num_students);
  if(num_students == 0 && guard_state == -1) {  //Checks if room is cleared (before capacity exceeded)
  semSignalB(&guardEntry);
  semSignalB(&roomCleared);
  }
  else if(num_students == 0 && guard_state == -2) {  //Checks if room is cleared (after capacity exceeded)
  semSignalB(&roomCleared);
  }
  semSignalB(&studentExit);  //Unlock the exit semaphore

}

// guard thread function  --- NO need to change this function !
void* guard(void* arg)
{
  int i;            // loop control variable
  srand(seeds[0]);  // seed the guard thread random number generator

  // the guard repeatedly checks the room (limited to num_checks) and
  // walks the hallway
  for (i = 0; i < num_checks; i++) {
    guard_check_room();
    guard_walk_hallway();
  }

  return NULL;   // thread needs to return a void*
}

// student thread function --- NO need to change this function !
void* student(void* arg)
{
  long id = (long) arg;  // determine thread id from arg
  srand(seeds[id]);      // seed this threads random number generator

  // repeatedly study and do something else
  while (1) {
    student_study_in_room(id);
    do_something_else(id);
  }

  return NULL;   // thread needs to return a void*
}

int main(int argc, char** argv)  // the main function
{
  int n;                   // number of student threads
  pthread_t  cthread;      // guard thread
  pthread_t* sthreads;     // student threads	while(num_students < capacity && num_students > 0);
  long i;                  // loop control variable

  if (argc < 4) {
    fprintf(stderr, "USAGE: %s num_threads capacity num_checks\n", argv[0]);
    return 0;
  }

  // (COMPLETE) TODO: get three input parameters, convert, and properly store
  //START
  n = atoi(argv[1]);
  capacity = atoi(argv[2]);
  num_checks = atoi(argv[3]);
  //NOTE: atoi() converts a str to int
  //END


  // (COMPLETE) TODO: allocate space for the seeds[] array
  // NOTE: seeds[0] is guard seed, seeds[k] is the seed for student k
  seeds = (unsigned int*) malloc((n + 1) * sizeof(unsigned int));
  //NOTE: (num_threads + 1) because its 1 for every student + 1 for guard

  // (COMPLETE) TODO: allocate space for the student threads array, sthreads
  //sthreads = malloc(8 * n);
  sthreads = (pthread_t*) malloc(n * sizeof(pthread_t));

  // Initialize global variables and semaphores
  guard_state = 0;   // not in room (walking the hall)
  num_students = 0;  // number of students in the room

  semInitB(&mutex, 1);  // initialize mutex
  // (COMPLETE) TODO: complete the semaphore initializations, for all your semaphores
  semInitB(&roomCleared, 0);
  semInitB(&guardEntry, 0);
  semInitB(&studentExit, 1);


  // initialize guard seed and create the guard thread
  seeds[0] = START_SEED;
  pthread_create(&cthread, NULL, guard, (void*) NULL);
  
  for (i = 1; i <= n; i++) {
    // (COMPLETE) TODO: create the student threads and initialize seeds[k], for
  // each student k
  seeds[i] = START_SEED + i;
    //seeds[k] is used to seed the random num generator on thread creation;
  pthread_create(&sthreads[i-1], NULL, student, (void*) i);
    //student = type of struct for the thread

  }

  pthread_join(cthread, NULL);   // wait for guard thread to complete

  for (i = 0; i < n; i++) {
    // (COMPLETE) TODO: cancel each of the student threads (do man on pthread_cancel)
  pthread_cancel(sthreads[i]);

  }

  // (COMPLETE) TODO: free up any dynamic memory you allocated
  free(sthreads);
  free(seeds);
  
  return 0;
}
