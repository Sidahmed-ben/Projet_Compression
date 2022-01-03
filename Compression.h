#ifndef COMO_H
#define COMO_H

#include "ima.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct  elm_tab_compresse{
    unsigned int val;
    int nbr_pixel_val;
};
typedef struct elm_tab_compresse etc;



void printBits(size_t const size, void const * const ptr);
void AffectBits(size_t const size, void const * const ptr , unsigned char *mon_bloc );
unsigned int * tab_moy_creation(unsigned char * im,int nbr_pixel);
void print_tab_int_Bits(size_t const size, void const * const ptr);
unsigned char * tab_couleur_creation(unsigned int * tab_moyenne ,int taille);
void quickSort(unsigned int t[], int g, int d);
void print_tab_ind_couleur(int size);
unsigned int * reorganiser_tab_int(unsigned int * tab_int ,int size);
unsigned int * compresse_clut(unsigned int * tab_resultat_ordonne,int nbr_pixel, int nbr_el_cmp);
int compter_taille_tab_compresse(unsigned int * tab_resultat_ordonne, int nbr_pixel);
unsigned int * tab_decompresse(unsigned int * tab_compressed, int nbr_el_cmp, int nbr_pixel);
void trier_tableau_ind(int size);
unsigned char * tab_couleur_creation_depuis_rgb_compresse(unsigned char * tab_coul_cmp ,int nbr_pixel,int nbr_el_cmp);
void convert_to_palette_color(unsigned char * tab_coul_cmp ,int nbr_el_cmp,int facteur);
void stock_couleur_reff(unsigned char * tab_coul_cmp ,int nbr_el_cmp,int facteur);
unsigned char * tab_couleur_creation_dithering(Image * image_origine ,unsigned char * tab_reff_color,int nbr_pixel, int facteur);
void verif(unsigned char * im, int nbr_col);
unsigned char * indice_palette_creation(unsigned char * tab_couleur_dither, int nbr_pixel);
unsigned char * create_image_from_index_ref(unsigned char * tab_index, int nbr_pixel);
unsigned char * tab_couleur_creation_dithering_avec_debordement(Image * image_origine ,unsigned char * tab_reff_color,int nbr_pixel, int facteur);
unsigned char ieme_bit(unsigned char bloc , int bit, int placer);
unsigned char * dcmp_6b_to_8b(unsigned char * tab_6b , int taille_tab6);
unsigned char * cmp_8b_to_6b(unsigned char * tab_8b , int taille_tab8);


#endif