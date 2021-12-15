

#include "Compression.h"
#include <GL/gl.h>
#include <assert.h>
#include <stdlib.h>


// variables global 
int tab_shift[24] = {7,7,7,6,6,6,5,5,5,4,4,4,3,3,3,2,2,2,1,1,1,0,0,0};
int * tab_ind_coul_im = NULL; 
int * tab_repetition = NULL;
 int * tab_couleur_ord = NULL;

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

unsigned char * tab_couleur_reff_creation(unsigned int * tab_moyenne ,int taille){

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
      
    //   tab_ind_coul_im[i] = j ;  tab_ind_coul_im[j] = i;  

    }
    if (i == d)
      return quickSort(t, g, i - 1);
    tmp = t[i]; t[i] = t[d]; t[d] = tmp;
    tmp = tab_ind_coul_im[i]; tab_ind_coul_im[i] = tab_ind_coul_im[d]  ;tab_ind_coul_im[d] = tmp;
    // tab_ind_coul_im[i] = d ; tab_ind_coul_im[d] = i;  

    quickSort(t, g, i - 1);
    quickSort(t, i + 1, d);
  }
}


void print_tab_ind_couleur(int size){
    printf("tab_ind_coul : ");
    for(int i = 0 ; i< size; i++){
        printf(" %u |",tab_ind_coul_im[i]);
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

int compter_taille_tab_compresse(unsigned int * tab_resultat_ordonne, int nbr_pixel){
    int nbr_el_compresse = 1;
    unsigned int tomp = tab_resultat_ordonne[0];
    for(int i = 1 ; i< nbr_pixel; i++){
        if(tomp != tab_resultat_ordonne[i]){
            nbr_el_compresse++;
            tomp = tab_resultat_ordonne[i];
        }
    }

    return nbr_el_compresse;
}

unsigned int * compresse_clut(unsigned int * tab_resultat_ordonne,int nbr_pixel, int nbr_el_cmp){
    // printf(" nombre element compresed = %d\n ",nbr_el_compresse);

    int nbr_el_compresse = compter_taille_tab_compresse(tab_resultat_ordonne,nbr_pixel);

    unsigned int  * tab_el_compresse = (unsigned int *)calloc(nbr_el_compresse , sizeof(unsigned int));
    assert(tab_el_compresse);

    tab_repetition = (int *)calloc(nbr_el_compresse , sizeof(int));
    assert(tab_repetition);

    int ind_tab_el_cmp = 0;
    unsigned int tmp = tab_resultat_ordonne[0];

    // tab_el_compresse[0].val = tmp;
    // tab_el_compresse[0].nbr_pixel_val = 1;
    tab_el_compresse[0] = tmp;
    tab_repetition[0]= 1;

    for(int i = 1 ; i< nbr_pixel; i++){
        if( tmp == tab_resultat_ordonne[i] ){
            // tab_el_compresse[ind_tab_el_cmp].nbr_pixel_val++;
            tab_repetition[ind_tab_el_cmp]++;
        }else{
            tmp = tab_resultat_ordonne[i];
            // tab_el_compresse[++ind_tab_el_cmp].val = tmp;
            // tab_el_compresse[ind_tab_el_cmp].nbr_pixel_val = 1;
            tab_el_compresse[++ind_tab_el_cmp] = tmp;
            tab_repetition[ind_tab_el_cmp]= 1;
        }
    }

    for(int i = 0 ; i< nbr_el_compresse; i++){
    printf(" %u -> %u fois \n",tab_el_compresse[i],tab_repetition[i]);
  }


    return tab_el_compresse;
}

void quickSort_indice(unsigned int t[], int g, int d) {
  int i, j, vpivot, tmp;
  if( d > g ) {
    vpivot = t[d]; i = g - 1; j = d;
    for(;;) {
      while(t[++i] < vpivot);
      while(t[--j] > vpivot) if(j == i) break;
      if(i >= j) break;
      tmp = t[i]; t[i] = t[j]; t[j] = tmp;
      tmp = tab_couleur_ord[i]; tab_couleur_ord[i] = tab_couleur_ord[j] ; tab_couleur_ord[j] = tmp;
    }
    if (i == d)
      return quickSort(t, g, i - 1);
    tmp = t[i]; t[i] = t[d]; t[d] = tmp;
    tmp= tab_couleur_ord[i];  tab_couleur_ord[i]=tab_couleur_ord[d] ; tab_couleur_ord[d] = tmp;

    quickSort(t, g, i - 1);
    quickSort(t, i + 1, d);
  }
}



void trier_tableau_ind(int size){
    tab_couleur_ord = (int * ) calloc(size, sizeof(int));
    assert(tab_couleur_ord);
    for(int i = 0; i< size ; i++){
        tab_couleur_ord[i] = i;
    }

    quickSort_indice((unsigned int *)tab_ind_coul_im, 0, size-1 ); 

    printf("tab_ind_ord = ");
    for(int i = 0 ; i< size ; i++){
        printf(" %d |",tab_couleur_ord[i]);
    }
    printf("\n");

}



unsigned int * tab_decompresse(unsigned int  * tab_compressed, int nbr_el_cmp, int nbr_pixel){

    unsigned int * tab_dcmp = (unsigned int *) calloc(nbr_pixel,sizeof(unsigned int));
    assert(tab_dcmp);
    int indice_tab_coul = 0;
    for(int j = 0 ; j< nbr_el_cmp ; j++){
        for(int i = 0 ; i< tab_repetition[j] ; i++){
            tab_dcmp[tab_ind_coul_im[indice_tab_coul]] = tab_compressed[j];
            indice_tab_coul++;
        }
    }

    return tab_dcmp;
    
}


// unsigned int * tab_cmp_coul_to_reff(unsigned char * tab_cmp_coul , int nbr_coul_rgb ){
//     unsigned char * tab_reff = (unsigned char *)calloc(nbr_coul_rgb,sizeof(unsigned char));
//     assert(tab_reff);


// }