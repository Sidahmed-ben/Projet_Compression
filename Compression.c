

#include "Compression.h"
#include <GL/gl.h>

void printBits(size_t const size, void const * const ptr)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;
    
    for (i = size-1; i >= 0 ; i--) {
        for (j = 7; j >=0; j--) {
            byte = (b[i] >> j) & 1;
            printf("%u",byte);
        }
        printf("/");
    }
    puts("");
}



void AffectBits(size_t const size, void const * const ptr , unsigned char *mon_bloc )
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j,ind;

    for (i = 0; i < size; i++) {
        for (j = 7; j >=0; j--) {
            byte = (b[i] >> j) & 1;
            ind = (i*8 +(7-j));
            if(byte == 1)
              mon_bloc[ind] = '1';
            else  
              mon_bloc[ind] ='0';
        }
    }
}

int * tab_moy_creation(unsigned char * im){
    int * tab = (int*) calloc(5 ,sizeof(int));
    assert(tab);
    unsigned char  * ptr_char_tab;
    int * elem_tab ;
    unsigned char  r;
    unsigned char  g;
    unsigned char  b;
    unsigned char tmp = 0;
    for(int i = 0 ;i < 5 ; i++){
        r = im[3*i];
        g = im[3*i+1];
        b = im[3*i+2];
        elem_tab = tab+i;
        for(int j = 0 ; j< 3 ; j++){
            ptr_char_tab = (unsigned char *)elem_tab+j;
            for(int k = 0 ; k < 8 ; k++){
                //tmp = 
            }



        }

    } 

    return tab;
}