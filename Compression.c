

#include "Compression.h"
#include <GL/gl.h>
#include <assert.h>
#include <stdlib.h>

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

void print_tab_int_Bits(size_t const size, void const * const ptr)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;
    int ind_elmnt_tab = 1;
    int ind;
    for(int ind_tab = 0; ind_tab < size ; ind_tab++ ){
        ind = ind_tab*4;
        for (i = 3+ind; i >= ind ; i--) {
            for (j = 7; j >=0; j--) {
                byte = (b[i] >> j) & 1;
                printf("%u",byte);
            }
            printf("/");
        }
        printf(" // ");
    }
    puts(" ");

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


int tab_shift[24] = {7,7,7,6,6,6,5,5,5,4,4,4,3,3,3,2,2,2,1,1,1,0,0,0};

int * tab_ind_coul_im = NULL; 

unsigned int * tab_moy_creation(unsigned char * im, int nbr_pixel){
    unsigned int * tab = (unsigned int*) calloc(nbr_pixel,sizeof(unsigned int));
    assert(tab);

    /* Initialisation du tableau des indcices des couleurs dans l'image */ 
    tab_ind_coul_im = (int*) calloc(nbr_pixel,sizeof(int));
    assert(tab_ind_coul_im);
    for(int i = 0 ;i< nbr_pixel; i++){
        tab_ind_coul_im[i] = i;
    }

    unsigned char  * ptr_char_tab;
    unsigned int * elem_tab ;
    unsigned char  r;
    unsigned char  g;
    unsigned char  b;
    unsigned char bit= 0,tmp_elm_tab;
    int ind_shift = 0;
    int ind_rgb = 0;
    unsigned char rgb[3] ;

    for(int i = 0 ;i < nbr_pixel ; i++){
        ind_shift= 0;
        ind_rgb = 0;
        rgb[0] = im[3*i];
        rgb[1] = im[3*i+2];
        rgb[2] = im[3*i+1];
        elem_tab = tab+i;
        for(int j = 0 ; j< 3 ; j++){
            ptr_char_tab = (unsigned char *)elem_tab+(3-j);
            tmp_elm_tab = 0;
            bit = 0;
            for(int k = 0 ;k< 8; k++){
                bit = rgb[ind_rgb%3] >> (tab_shift[ind_shift]) & 1 ;
                ind_rgb++;
                bit = bit << (7-k);
                ind_shift++;
                tmp_elm_tab |= bit; 
            }
            *ptr_char_tab= tmp_elm_tab;
        }
    } 
    return tab;
}


int tab_ind_rgb[24] =  {0,2,1,0,2,1,0,2,1,0,2,1,0,2,1,0,2,1,0,2,1,0,2,1};
// int tab_shift[24] = {7,7,7,6,6,6,5,5,5,4,4,4,3,3,3,2,2,2,1,1,1,0,0,0};

unsigned char * tab_couleur_creation(unsigned int * tab_moyenne ,int taille){

    unsigned char * tab_couleur = (unsigned char*) calloc(taille*3, sizeof(unsigned char));
    assert(tab_couleur);
    unsigned char * tab_moy_char ;
    unsigned char bit = 0;
    unsigned char rgb[3] = {0};
    int ind_rgb   = 0;
    int ind_shift = 0;
    unsigned int * tab_moy_ref = tab_moyenne;
    for(int i = 0 ;i< taille ; i++){

        tab_moy_char = ((unsigned char*)tab_moy_ref);
        tab_moy_char+=3;

        // printf(" %p  %u \n",tab_moy_char,*(tab_moy_char));
        ind_shift = 0;
        ind_rgb = 0;
        rgb[0]=rgb[1]=rgb[2]=0;
        for(int j = 0 ; j< 3 ;j++){
            for(int k = 0 ; k< 8 ;k++){
                bit = (*tab_moy_char) >> (7-k) & 1;
                bit = bit << tab_shift[ind_shift];
                ind_shift++;
                rgb[tab_ind_rgb[ind_rgb]] |= bit;
                ind_rgb++;
            }
            // printf(" %p  %u \n",tab_moy_char,*(tab_moy_char));
            tab_moy_char--;
        }

        tab_couleur[3*i]   = rgb[0];
        tab_couleur[3*i+1] = rgb[1];
        tab_couleur[3*i+2] = rgb[2];
        tab_moy_ref++;
        
    }

    return tab_couleur;
}



void quickSort(unsigned int t[], int g, int d) {
  int i, j, vpivot, tmp;
  if( d > g ) {
    vpivot = t[d]; i = g - 1; j = d;
    for(;;) {
      while(t[++i] < vpivot);
      while(t[--j] > vpivot) if(j == i) break;
      if(i >= j) break;
      tmp = t[i]; t[i] = t[j]; t[j] = tmp;
      tmp = tab_ind_coul_im[i]; tab_ind_coul_im[i] = tab_ind_coul_im[j]  ;tab_ind_coul_im[j] = tmp;  
    }
    if (i == d)
      return quickSort(t, g, i - 1);
    tmp = t[i]; t[i] = t[d]; t[d] = tmp;
    tmp = tab_ind_coul_im[i]; tab_ind_coul_im[i] = tab_ind_coul_im[d]  ;tab_ind_coul_im[d] = tmp;
    quickSort(t, g, i - 1);
    quickSort(t, i + 1, d);
  }
}


void print_tab_ind_couleur(int size){
    printf("tab_ind_coul : ");
    for(int i = 0 ; i< size; i++){
        printf(" %d |",tab_ind_coul_im[i]);
    }
    printf("\n");
}


unsigned int * reorganiser_tab_int(unsigned int * tab_int ,int size){
    unsigned int * tab_int_interm = (unsigned int *)calloc(size, sizeof(unsigned int));
    assert(tab_int_interm);

    
    for(int i = 0 ; i < size ; i++){
        tab_int_interm[tab_ind_coul_im[i]] = tab_int[i];
        // printf(" %d , %d \n ", tab_int_interm[tab_ind_coul_im[i]],tab_ind_coul_im[i]);
    }

    // for(int i = 0 ;i< size ;i++){
    //     printf(" %u |",tab_int_interm[i]);
    // }
    // printf("\n");

    free(tab_int);
    tab_int = NULL;
    return tab_int_interm;
}