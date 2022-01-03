#ifndef COMO_H
#define COMO_H

#include "ima.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int compter_taille_tab_compresse(unsigned int * tab_resultat_ordonne, int nbr_pixel);
void convert_to_palette_color(unsigned char * tab_coul_cmp ,int nbr_el_cmp,int facteur);
unsigned char * tab_couleur_creation_dithering(Image * image_origine ,unsigned char * tab_reff_color,int nbr_pixel, int facteur);
unsigned char * indice_palette_creation(unsigned char * tab_couleur_dither, int nbr_pixel);
unsigned char * create_image_from_index_ref(unsigned char * tab_index, int nbr_pixel);
unsigned char * tab_couleur_creation_dithering_avec_debordement(Image * image_origine ,unsigned char * tab_reff_color,int nbr_pixel, int facteur);
unsigned char * dcmp_6b_to_8b(unsigned char * tab_6b , int taille_tab6);
unsigned char * cmp_8b_to_6b(unsigned char * tab_8b , int taille_tab8);
unsigned char ieme_bit(unsigned char bloc , int bit, int placer);
void Compression(char * fich_cmp,int debordement, Image * image);
Image * Decompression(char * nom_fichier, Image *img);
Image * init_image_from_cmp_file(int width ,int height, unsigned char * color);
void free_ptr(unsigned char * ptr);

#endif