#include <stdio.h>
#include <stdlib.h>

#define DEBUG 1  // useful for turning on/off debugging statements

typedef struct {
  char* first;  // pointer to first character of first name ("string" of chars)
  char* last;   // pointer to first character of last name ("string of chars)
  int   age;    // age in years
  int   sex;    // 0 = male, 1 = female, 2 = both
} person_t;     // person type


int main(int argc, char** argv)
{
  // in C all variables should be declared at the beginning of the
  // function (including main)

  int i;

  person_t person;   // person is an instance of type person_t (on stack)

  char first[10];    // array of 10 characters (on stack!)

  first[0] = 's';
  first[1] = 't';
  first[2] = 'o';
  first[3] = 'n';
  first[4] = 'y';
  first[5] = '\0';    // null character used to "terminate" the string

  // Quick question ... how would be create an array that holds 10
  // person_t elements ??

  printf("-----------------------------------------------\n");

  // sizeof() returns the size of the data structure (not length of string)
  printf("Number of elements in first = %lu\n", sizeof(first)); 

  for (i = 0; i < sizeof(first); i++)
    printf("first[%d]=%c\n", i, first[i]); 

  person.first = first;     // person.first "points" to first character of first
  person.last = "leclerc";  // person.last "points" to string literal "leclerc"
  person.age = 44;
  person.sex = 0;           // male

  printf("-----------------------------------------------\n");

  // NOTE: would be "safer" in general to use braces below
  if (person.sex == 0)
    printf("first %s, last %s, first initial %c, age %d, male\n",
	   person.first, person.last, person.first[0], person.age);
  else
    printf("first %s, last %s, first initial %c, age %d, female\n",
	   person.first, person.last, person.first[0], person.age);

  person.first[3] = 't';

  printf("first %s\n", person.first);

  // -----------------------------------
  // Concept of a pointer
  // -----------------------------------
  // pointer "points" to a memory location (not the actual value)
  
  // you may de-reference a pointer (*p) to modify the value the
  // pointer is pointing at

  printf("-----------------------------------------------\n");

  int* p = malloc(sizeof(int));   // allocate space for an int and p points to
  int* pp;

  printf("(p) Memory Location [%p] has value = %d\n", p, (*p));

  (*p)=42;    // dereferencing

  printf("(p) Memory Location [%p] has value = %d\n", p, (*p));

  (*p)++;

  printf("(p) Memory Location [%p] has value = %d\n", p, (*p));

  pp = p;

  (*pp)++;

  printf("(pp) Memory Location [%p] has value = %d\n", pp, (*pp));
  printf("(p) Memory Location [%p] has value = %d\n", p, (*p));

  // -----------------------------------
  // Open a file and loop through each line
  // -----------------------------------
  
  printf("-----------------------------------------------\n");

  FILE* fhnd;

  fhnd = fopen("test.txt", "r");

  char line[50];

  if (fhnd != NULL) {

    while (fgets(line, sizeof(line), fhnd) != NULL) {
      if (DEBUG)
	printf("%s", line);
    }

  }

  fclose(fhnd);
  free(p);
  return 0;
}
