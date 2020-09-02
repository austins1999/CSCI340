#include <stdio.h>
#include <stdlib.h>

#define DEBUG 1  // useful for turning on/off debugging statements

typedef struct {
  char* first;  // pointer to first character of first name ("string" of chars)
  char* last;   // pointer to first character of last name ("string of chars)
  int   age;    // age in years
  int   sex;    // 0 = male, 1 = female, 2 = both
} person_t;     // person type

// forward declarations (note no actual code --- code written later)
void initialize(person_t* p_person_array, int length);
void printPerson(person_t* p_person);

int main(int argc, char** argv)
{
  // in C all variables should be declared at the beginning of the
  // function (including main)

  int length;
  int i;

  person_t person_array[10];   // create a struct variable: p_person
			       // is data type, person is variable
			       // person
  
  length = sizeof(person_array)/sizeof(person_t);
  printf("%d\n", length);

  initialize(person_array, length);

  for(i = 0; i < length; i++) {
    printPerson(&person_array[i]);
  }

  return 0;
} // end main function

void initialize(person_t* p_person_array, int length)
{
  int i;

  // NOTE: in 'C' we don't declare i within the loop!
  for(i = 0; i < length; i++) {
    p_person_array[i].first=0;
    p_person_array[i].last=0;
    p_person_array[i].age=0;
    p_person_array[i].sex=0;
  }
}

void printPerson(person_t* p_person)
{
  printf("-----------------------------------------------\n");

  printf("first %s, last %s, age %d, ",
	 p_person->first, p_person->last, p_person->age);
    
  if (p_person->sex == 0)
    printf("male\n");
  else
    printf("female\n");
}
