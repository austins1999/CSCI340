// -----------------------------------
// CSCI 340 - Operating Systems
// Spring 2019
// cache.c file
// 
// Homework 1
//
// -----------------------------------

#include "cache.h"
#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int initializeCache(unsigned int number_of_lines)
{
  int line, retVal;

  retVal = OK;

  cache = malloc(sizeof(cache_line*) * number_of_lines);

  if (cache != NULL) {

    for (line=0; line<number_of_lines; line++) {

      cache[line] = (cache_line*) malloc(sizeof(cache_line));
      cache[line]->tag = UNK;
      cache[line]->hit_count = ZERO;
      
    }
    
  } else
    retVal = FAIL;

  return retVal;

} // end initializeCache function


int cread(unsigned int cmf, unsigned int* hex_addr, unsigned int* found,
	  unsigned int* replace) {

	int retVal = FAIL;
	unsigned int line;
	unsigned int offsetHolder;
	unsigned int offset;
	unsigned int minHits;
	unsigned int minHitsLine;
	unsigned int temp_addr;

	minHits = cache[0]->hit_count;
	minHitsLine = 0;

	*found = MISS;
	*replace = YES;

	offsetHolder = *hex_addr & 0x2;

	if(cmf == DM) { //Direct mapping
		for (line=0; line<NUM_OF_LINES; line++) {
			temp_addr = *hex_addr >> 5;
			//hex_addr = hex_addr >> 2;
			//hex_addr = hex_addr & 0x7;
			if(cache[line]->tag == temp_addr) {
				for(offset=0; offset<pow(2,NUM_BLOCK_OFFSET_BITS); offset++) {
					if(phy_memory[(cache[line]->tag + line) + offset] == phy_memory[(cache[line]->tag + line) + offsetHolder]) {
						retVal = phy_memory[(cache[line]->tag + line) + offset];
						*found = HIT;
						*replace = NO;
					}
				}
			}
		}
	}

	//Set associative; its a lost cause at this point
	/*else {
		*hex_addr = *hex_addr >> 4;
		if(cache[line]->tag == *hex_addr) {
			for(offset=0; offset<pow(2,NUM_BLOCK_OFFSET_BITS); offset++) {
				if(phy_memory[(cache[line]->tag + line) + offset] == phy_memory[(cache[line]->tag + line) + offsetHolder]) {
					retVal = phy_memory[(cache[line]->tag + line) + offset];
					*found = HIT;
					*replace = NO;
				}
			}
		}
	}*/

	//find line to replace using LFU
	if(*replace == YES) {
		for (line=1; line<NUM_OF_LINES; line++) {
			if(cache[line]->hit_count < minHits) {
				minHits = cache[line]->hit_count;
				minHitsLine = line;
			}
		}
		cache[minHitsLine]->tag = (*hex_addr >> 5);
		cache[minHitsLine]->hit_count = 0;
	}

	//increment hit count 
	(cache[line]->hit_count) += *found;
	
  return retVal;

} // end cread function



void cprint()
{
  unsigned int line;

  printf("\n---------------------------------------------\n");
  printf("line\ttag\tnum of hits\n");
  printf("---------------------------------------------\n");

  for (line=0; line<NUM_OF_LINES; line++) { 

    if (cache[line]->tag == UNK) {
			
      printf("%d\t%d\t%d\n", line, cache[line]->tag, cache[line]->hit_count);

    } else {

      printf("%d\t%02X\t%d\n", line, cache[line]->tag, cache[line]->hit_count);

    }

  }

} // end cprint function
