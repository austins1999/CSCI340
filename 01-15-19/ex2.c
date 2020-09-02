#include <stdio.h>
#include <stdlib.h>

#define DEBUG 1  // useful for turning on/off debugging statements

typedef struct {
  char* first;  // pointer to first character of first name ("string" of chars)
  char* last;   // pointer to first character of last name ("string of chars)
  int   age;    // age in years
  int   sex;    // 0 = male, 1 = female, 2 = both
} person_t;     // person type

void printPerson(person_t* p_person);  // forward declaration
                                       // p_person means "pointer to person"

int main(int argc, char** argv)
{
  // in C all variables should be declared at the beginning of the
  // function (including main)
    
  person_t person; // create a struct variable: person_t is the data
                   // type, person is the actual variable

  char* first = malloc(10*sizeof(char));  // create space for 10 characters
                                          // (on heap!)
  first[0] = 's';
  first[1] = 't';
  first[2] = 'o';
  first[3] = 'n';
  first[4] = 'y';
  first[5] = '\0';    // null character used to "terminate" the string

  char last[10] = {'l','e','c','l','e','r','c','\0'};  // initializer list

  person.first = first;
  person.last = last;
  person.age = 44;
  person.sex = 0;

  printPerson(&person);

  free(person.first);
  
  return 0;  
} // end main function


void printPerson(person_t* p_person)
{
  printf("-----------------------------------------------\n");

  printf("first %s, last %s, first initial %c, age %d, ",
	 p_person->first, p_person->last, p_person->first[0], p_person->age);

  if (p_person->sex == 0)
    printf("male\n");
  else
    printf("female\n");

  p_person->first[3] = 't';
  p_person->last[3] = 'z';

  printf("first %s, last %s\n", p_person->first, p_person->last);
}
