

#include "Compression.h"
#include "ima.h"
#include <GL/gl.h>
#include <assert.h>
#include <stdlib.h>

void free_ptr(unsigned char *ptr) {
  free(ptr);
  ptr = NULL;
}

unsigned char palette_officiel[64][3] = {
    {0, 0, 0},     {0, 0, 85},     {0, 0, 170},     {0, 0, 255},
    {0, 85, 0},    {0, 85, 85},    {0, 85, 170},    {0, 85, 255},
    {0, 170, 0},   {0, 170, 85},   {0, 170, 170},   {0, 170, 255},
    {0, 255, 0},   {0, 255, 85},   {0, 255, 170},   {0, 255, 255},
    {85, 0, 0},    {85, 0, 85},    {85, 0, 170},    {85, 0, 255},
    {85, 85, 0},   {85, 85, 85},   {85, 85, 170},   {85, 85, 255},
    {85, 170, 0},  {85, 170, 85},  {85, 170, 170},  {85, 170, 255},
    {85, 255, 0},  {85, 255, 85},  {85, 255, 170},  {85, 255, 255},
    {170, 0, 0},   {170, 0, 85},   {170, 0, 170},   {170, 0, 255},
    {170, 85, 0},  {170, 85, 85},  {170, 85, 170},  {170, 85, 255},
    {170, 170, 0}, {170, 170, 85}, {170, 170, 170}, {170, 170, 255},
    {170, 255, 0}, {170, 255, 85}, {170, 255, 170}, {170, 255, 255},
    {255, 0, 0},   {255, 0, 85},   {255, 0, 170},   {255, 0, 255},
    {255, 85, 0},  {255, 85, 85},  {255, 85, 170},  {255, 85, 255},
    {255, 170, 0}, {255, 170, 85}, {255, 170, 170}, {255, 170, 255},
    {255, 255, 0}, {255, 255, 85}, {255, 255, 170}, {255, 255, 255}};

unsigned char *indice_palette_creation(unsigned char *tab_couleur_dither,
                                       int nbr_pixel) {
  unsigned char *pal_ind_tab =
      (unsigned char *)calloc(nbr_pixel, sizeof(unsigned char));
  assert(pal_ind_tab);

  unsigned char *pixel = (unsigned char *)calloc(3, sizeof(unsigned char));
  unsigned char *copy = pixel;
  assert(pixel);

  int ind_debut;
  pixel = tab_couleur_dither;

  for (int i = 0; i < nbr_pixel; i++) {
    if (pixel[0] == 0) {
      ind_debut = 0;
      for (int j = 0; j < 16; j++) {
        if (pixel[1] == palette_officiel[ind_debut][1] &&
            pixel[2] == palette_officiel[ind_debut][2]) {
          pal_ind_tab[i] = ind_debut;
          break;
        } else {
          ind_debut++;
          continue;
        }
        break;
      }
    } else if (pixel[0] == 85) {
      ind_debut = 16;
      for (int j = 0; j < 16; j++) {
        if (pixel[1] == palette_officiel[ind_debut][1] &&
            pixel[2] == palette_officiel[ind_debut][2]) {
          pal_ind_tab[i] = ind_debut;
          break;
        } else {
          ind_debut++;
          continue;
        }
        break;
      }
    } else if (pixel[0] == 170) {
      ind_debut = 32;
      for (int j = 0; j < 16; j++) {
        if (pixel[1] == palette_officiel[ind_debut][1] &&
            pixel[2] == palette_officiel[ind_debut][2]) {
          pal_ind_tab[i] = ind_debut;
          break;
        } else {
          ind_debut++;
          continue;
        }
        break;
      }
    } else {
      ind_debut = 48;
      for (int j = 0; j < 16; j++) {
        if (pixel[1] == palette_officiel[ind_debut][1] &&
            pixel[2] == palette_officiel[ind_debut][2]) {
          pal_ind_tab[i] = ind_debut;
          break;
        } else {
          ind_debut++;
          continue;
        }
        break;
      }
    }
    pixel += 3;
  }

  free_ptr(copy);
  pixel = NULL;

  return pal_ind_tab;
}

unsigned char *create_image_from_index_ref(unsigned char *tab_index,
                                           int nbr_pixel) {
  unsigned char *image_color =
      (unsigned char *)calloc(nbr_pixel * 3, sizeof(unsigned char));
  assert(image_color);

  unsigned char *pixel = (unsigned char *)calloc(3, sizeof(unsigned char));
  unsigned char *copy = pixel;
  assert(pixel);
  copy = image_color;
  for (int i = 0; i < nbr_pixel; i++) {
    copy[0] = palette_officiel[tab_index[i]][0];
    copy[1] = palette_officiel[tab_index[i]][1];
    copy[2] = palette_officiel[tab_index[i]][2];
    copy += 3;
  }
  free_ptr(pixel);
  copy = NULL;
  return image_color;
}

void convert_to_palette_color(unsigned char *tab_coul_cmp, int nbr_el_cmp,
                              int facteur) {
  for (int i = 0; i < nbr_el_cmp * 3; i++) {
    tab_coul_cmp[i] = (int)(facteur * (tab_coul_cmp[i] / 255.0) + (0.5)) *
                      (255 / facteur /* 42 */);
  }
}

int indice(int x, int y, int width) { return 3 * (x + y * width); }

//  Cette version empêche de dépasser le 0 en bas et le 255 en haut
unsigned char *tab_couleur_creation_dithering(Image *image_origine,
                                              unsigned char *tab_reff_color,
                                              int nbr_pixel, int facteur) {
  unsigned char *dithred_image =
      (unsigned char *)calloc(nbr_pixel * 3, sizeof(unsigned char));
  assert(dithred_image);
  unsigned char *pixel = (unsigned char *)calloc(3, sizeof(unsigned char));
  unsigned char *cop1 = pixel;
  assert(pixel);
  unsigned char *voisin = (unsigned char *)calloc(3, sizeof(unsigned char));
  unsigned char *cop2 = voisin;
  assert(voisin);

  int w = image_origine->sizeX;
  unsigned char old_r, old_g, old_b, new_r, new_g, new_b;
  int error_r, error_g, error_b;
  int ind_x, ind_y;
  int ind_courant;

  for (int y = 0; y < image_origine->sizeY - 1; y++) {
    for (int x = 1; x < image_origine->sizeX - 1; x++) {
      ind_courant = indice(x, y, w);

      pixel = image_origine->data + (ind_courant);
      old_r = pixel[0];
      old_g = pixel[1];
      old_b = pixel[2];

      new_r = (unsigned char)(facteur * (old_r / 255.0) + (0.5)) *
              (255 / facteur /* 42 */);
      new_g = (unsigned char)(facteur * (old_g / 255.0) + (0.5)) *
              (255 / facteur /* 42 */);
      new_b = (unsigned char)(facteur * (old_b / 255.0) + (0.5)) *
              (255 / facteur /* 42 */);

      pixel = dithred_image + (ind_courant);
      pixel[0] = new_r;
      pixel[1] = new_g;
      pixel[2] = new_b;

      error_r = old_r - new_r;
      error_g = old_g - new_g;
      error_b = old_b - new_b;

      // diffusion erreur voisin a droite
      voisin = image_origine->data + indice(x + 1, y, w);
      voisin[0] = ((voisin[0] + (error_r * 7 / 16.0)) < 0 |
                   (voisin[0] + (error_r * 7 / 16.0)) > 255)
                      ? voisin[0]
                      : voisin[0] + (error_r * 7 / 16.0);
      voisin[1] = ((voisin[1] + (error_g * 7 / 16.0)) < 0 |
                   (voisin[1] + (error_g * 7 / 16.0)) > 255)
                      ? voisin[1]
                      : voisin[1] + (error_g * 7 / 16.0);
      voisin[2] = ((voisin[2] + (error_b * 7 / 16.0)) < 0 |
                   (voisin[2] + (error_b * 7 / 16.0)) > 255)
                      ? voisin[2]
                      : voisin[2] + (error_b * 7 / 16.0);


      voisin = image_origine->data + indice(x - 1, y + 1, w);
      voisin[0] = ((voisin[0] + (error_r * 3 / 16.0)) < 0 |
                   (voisin[0] + (error_r * 3 / 16.0)) > 255)
                      ? voisin[0]
                      : voisin[0] + (error_r * 3 / 16.0);
      voisin[1] = ((voisin[1] + (error_g * 3 / 16.0)) < 0 |
                   (voisin[1] + (error_g * 3 / 16.0)) > 255)
                      ? voisin[1]
                      : voisin[1] + (error_g * 3 / 16.0);
      voisin[2] = ((voisin[2] + (error_b * 3 / 16.0)) < 0 |
                   (voisin[2] + (error_b * 3 / 16.0)) > 255)
                      ? voisin[2]
                      : voisin[2] + (error_b * 3 / 16.0);


      voisin = image_origine->data + indice(x, y + 1, w);
      voisin[0] = ((voisin[0] + (error_r * 5 / 16.0)) < 0 |
                   (voisin[0] + (error_r * 5 / 16.0)) > 255)
                      ? voisin[0]
                      : voisin[0] + (error_r * 5 / 16.0);
      voisin[1] = ((voisin[1] + (error_g * 5 / 16.0)) < 0 |
                   (voisin[1] + (error_g * 5 / 16.0)) > 255)
                      ? voisin[1]
                      : voisin[1] + (error_g * 5 / 16.0);
      voisin[2] = ((voisin[2] + (error_b * 5 / 16.0)) < 0 |
                   (voisin[2] + (error_b * 5 / 16.0)) > 255)
                      ? voisin[2]
                      : voisin[2] + (error_b * 5 / 16.0);
      // printf(" voisine bas  with error :  %u %u %u
      // \n",voisin[0],voisin[1],voisin[2]);

      voisin = image_origine->data + indice(x + 1, y + 1, w);
      voisin[0] = ((voisin[0] + (error_r * 1 / 16.0)) < 0 |
                   (voisin[0] + (error_r * 1 / 16.0)) > 255)
                      ? voisin[0]
                      : voisin[0] + (error_r * 1 / 16.0);
      voisin[1] = ((voisin[1] + (error_g * 1 / 16.0)) < 0 |
                   (voisin[1] + (error_g * 1 / 16.0)) > 255)
                      ? voisin[1]
                      : voisin[1] + (error_g * 1 / 16.0);
      voisin[2] = ((voisin[2] + (error_b * 1 / 16.0)) < 0 |
                   (voisin[2] + (error_b * 1 / 16.0)) > 255)
                      ? voisin[2]
                      : voisin[2] + (error_b * 1 / 16.0);
    }
  }

  free_ptr(cop1);
  free_ptr(cop2);
  pixel = NULL;
  voisin = NULL;
  return dithred_image;
}

//  Cette version n'empéche pas  de dépasser la valeur 0 vers les nombres
//  nègatifes
unsigned char *
tab_couleur_creation_dithering_avec_debordement(Image *image_origine,
                                                unsigned char *tab_reff_color,
                                                int nbr_pixel, int facteur) {
  unsigned char *dithred_image =
      (unsigned char *)calloc(nbr_pixel * 3, sizeof(unsigned char));
  assert(dithred_image);
  unsigned char *pixel = (unsigned char *)calloc(3, sizeof(unsigned char));
  unsigned char *cop1 = pixel;
  assert(pixel);
  unsigned char *voisin = (unsigned char *)calloc(3, sizeof(unsigned char));
  unsigned char *cop2 = voisin;
  assert(voisin);
  int w = image_origine->sizeX;
  unsigned char old_r, old_g, old_b, new_r, new_g, new_b;
  int error_r, error_g, error_b;
  int ind_x, ind_y;
  int ind_courant;

  for (int y = 0; y < image_origine->sizeY - 1; y++) {
    for (int x = 1; x < image_origine->sizeX - 1; x++) {
      ind_courant = indice(x, y, w);

      pixel = image_origine->data + (ind_courant);
      old_r = pixel[0];
      old_g = pixel[1];
      old_b = pixel[2];

      new_r = (unsigned char)(facteur * (old_r / 255.0) + (0.5)) *
              (255 / facteur /* 42 */);
      new_g = (unsigned char)(facteur * (old_g / 255.0) + (0.5)) *
              (255 / facteur /* 42 */);
      new_b = (unsigned char)(facteur * (old_b / 255.0) + (0.5)) *
              (255 / facteur /* 42 */);

      pixel = dithred_image + (ind_courant);
      pixel[0] = new_r;
      pixel[1] = new_g;
      pixel[2] = new_b;

      error_r = old_r - new_r;
      error_g = old_g - new_g;
      error_b = old_b - new_b;

      // diffusion erreur voisin a droite
      voisin = image_origine->data + indice(x + 1, y, w);
      // le valeurs de voisins ici peuvent être au dessous de 0 ou au dessus de
      // 255
      voisin[0] += (error_r * 7 / 16.0);
      voisin[1] += (error_g * 7 / 16.0);
      voisin[2] += (error_b * 7 / 16.0);

      voisin = image_origine->data + indice(x - 1, y + 1, w);
      // le valeurs de voisins ici peuvent être au dessous de 0 ou au dessus de
      // 255
      voisin[0] += (error_r * 3 / 16.0);
      voisin[1] += (error_g * 3 / 16.0);
      voisin[2] += (error_b * 3 / 16.0);

      voisin = image_origine->data + indice(x, y + 1, w);
      // le valeurs de voisins ici peuvent être au dessous de 0 ou au dessus de
      // 255
      voisin[0] += (error_r * 5 / 16.0);
      voisin[1] += (error_g * 5 / 16.0);
      voisin[2] += (error_b * 5 / 16.0);

      voisin = image_origine->data + indice(x + 1, y + 1, w);
      // le valeurs de voisins ici peuvent être au dessous de 0 ou au dessus de
      // 255
      voisin[0] += (error_r * 1 / 16.0);
      voisin[1] += (error_g * 1 / 16.0);
      voisin[2] += (error_b * 1 / 16.0);
    }
  }

  free_ptr(cop1);
  free_ptr(cop2);
  pixel = NULL;
  voisin = NULL;
  return dithred_image;
}

int compter_taille_tab_compresse(unsigned int *tab_resultat_ordonne,
                                 int nbr_pixel) {
  int nbr_el_compresse = 1;
  unsigned int tomp = tab_resultat_ordonne[0];
  for (int i = 1; i < nbr_pixel; i++) {
    if (tomp != tab_resultat_ordonne[i]) {
      nbr_el_compresse++;
      tomp = tab_resultat_ordonne[i];
    }
  }
  return nbr_el_compresse;
}

unsigned char ieme_bit(unsigned char bloc, int bit, int placer) {
  unsigned char and = (bloc >> bit) & 1;
  return and << placer;
}

unsigned char *cmp_8b_to_6b(unsigned char *tab_8b, int taille_tab8) {
  int taille_tab6 = (int)(((taille_tab8 * 6) / 8.0) + 0.75);

  unsigned char *tab_6b =
      (unsigned char *)calloc(taille_tab6, sizeof(unsigned char));
  assert(tab_6b);

  int ind_shift_8 = 5;
  int ind_tab_8 = 0;

  unsigned char tmp = 0, bit = 0;
  for (int i = 0; i < taille_tab6; i++) {
    tmp = 0;
    for (int j = 0; j < 8; j++) {
      bit = ieme_bit(tab_8b[ind_tab_8], ind_shift_8, 7 - j);
      tmp = tmp | bit;

      ind_tab_8 = ind_shift_8 == 0 ? ind_tab_8 + 1 : ind_tab_8;
      if (ind_tab_8 == taille_tab8)
        break;
      ind_shift_8 = ind_shift_8 == 0 ? 5 : ind_shift_8 - 1;
    }
    tab_6b[i] = tmp;
  }
  return tab_6b;
}

unsigned char *dcmp_6b_to_8b(unsigned char *tab_6b, int taille_tab6) {
  int taille_tab8 = (int)(((taille_tab6 * 8) / 6.0));

  unsigned char *tab_8 =
      (unsigned char *)calloc(taille_tab8, sizeof(unsigned char));
  assert(tab_8);

  int ind_tab_6 = 0;
  int shift_tab6 = 7;

  unsigned char tmp = 0, bit = 0;
  for (int i = 0; i < taille_tab8; i++) {
    tmp = 0;
    for (int j = 0; j < 6; j++) {
      bit = ieme_bit(tab_6b[ind_tab_6], shift_tab6, 5 - j);
      tmp = tmp | bit;

      ind_tab_6 = shift_tab6 == 0 ? ind_tab_6 + 1 : ind_tab_6;
      if (ind_tab_6 == taille_tab6)
        break;
      shift_tab6 = shift_tab6 == 0 ? 7 : shift_tab6 - 1;
    }
    tab_8[i] = tmp;
  }

  return tab_8;
}

void Compression(char *fich_cmp, int debordement, Image *image,
                 int mode_vitesse) {
  int facteur = 3;
  unsigned short width = image->sizeX;
  unsigned short height = image->sizeY;
  int size = width * height * 3;
  Image *image_copy = (Image *)calloc(1, sizeof(Image));
  assert(image_copy);

  image_copy->sizeX = image->sizeX;
  image_copy->sizeY = image->sizeY;
  image_copy->data = (unsigned char *)calloc(size, sizeof(unsigned char));
  assert(image_copy->data);

  memcpy(image_copy->data, image->data, size);

  unsigned char *dither_tab;
  if (debordement) {
    dither_tab = tab_couleur_creation_dithering_avec_debordement(
        image_copy, NULL, size / 3, facteur);
  } else {
    dither_tab =
        tab_couleur_creation_dithering(image_copy, NULL, size / 3, facteur);
  }

  unsigned char *palette_index_tab =
      indice_palette_creation(dither_tab, size / 3);

  unsigned char *tableau_indices_compresse =
      cmp_8b_to_6b(palette_index_tab, size / 3);
  if (!mode_vitesse) {
    if (debordement) {
      /* a liberrer */ unsigned char *image_coul =
          create_image_from_index_ref(palette_index_tab, size / 3);
      image->data = image_coul;

    } else {
      FILE *stream = fopen(fich_cmp, "wb");
      if (stream == NULL) /* If an error occurs during the file creation */
      {
        fprintf(stderr, "Veuillez Saisir un nom de fichier \n");
        exit(1);
      }

      int size_av_cmp = size / 3;
      int element_size = (int)(((size_av_cmp * 6) / 8.0) + 0.75);
      int elements_to_write = 1;
      fwrite(&width, 2, 1, stream);
      fwrite(&height, 2, 1, stream);
      size_t elements_written = fwrite(tableau_indices_compresse, element_size,
                                       elements_to_write, stream);
      fclose(stream);
    }
  }

  free(image_copy->data);
  image_copy->data = NULL;
  free(image_copy);
  image_copy = NULL;
  free_ptr(tableau_indices_compresse);
  free_ptr(palette_index_tab);
}

Image *init_image_from_cmp_file(int width, int height, unsigned char *color) {
  Image *image = (Image *)malloc(sizeof(Image));
  if (image == NULL) {
    fprintf(stderr, "Out of memory\n");
    exit(-1);
  }
  image->sizeX = width;
  image->sizeY = height;
  /* allocation memoire */
  int size = width * height * 3;
  image->data = color;
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_FLAT);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glutReshapeWindow(width, height);

  return image;
}

Image *Decompression(char *nom_fichier, Image *image, int mode_vitesse) {

  int nbr_pixel = 0, nbr_elem_lire = 0;
  int width_readen = 0, height_readen = 0;

  FILE *stream = fopen(nom_fichier, "rb");
  if (stream == NULL) /* If an error occurs during the file creation */
  {
    fprintf(stderr, "Veuillez Saisir un nom de fichier valide \n");
    exit(1);
  }

  fread(&width_readen, 2, 1, stream);
  fread(&height_readen, 2, 1, stream);

  nbr_pixel = width_readen * height_readen;
  nbr_elem_lire = (int)(((nbr_pixel * 6) / 8.0) + 0.75);


  unsigned char *palette_index_from_file =
      (unsigned char *)calloc(nbr_elem_lire, sizeof(unsigned char));
  assert(palette_index_from_file);

  int element_size = nbr_elem_lire;
  int elements_to_read = 1;

  size_t elements_readen =
      fread(palette_index_from_file, element_size, elements_to_read, stream);
  unsigned char *tableau_index_dcmp =
      dcmp_6b_to_8b(palette_index_from_file, nbr_elem_lire);
  unsigned char *image_coul =
      create_image_from_index_ref(tableau_index_dcmp, nbr_pixel);

  if (!mode_vitesse) {
    image = init_image_from_cmp_file(width_readen, height_readen, image_coul);
    fclose(stream);
  }

  free_ptr(tableau_index_dcmp);
  return image;
}