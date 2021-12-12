#ifndef COMO_H
#define COMO_H

#include "ima.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void printBits(size_t const size, void const * const ptr);
void AffectBits(size_t const size, void const * const ptr , unsigned char *mon_bloc );
int * tab_moy_creation(unsigned char * im);
void print_tab_int_Bits(size_t const size, void const * const ptr);



#endif