#include <stdlib.h>
#include <unistd.h>     
#include <math.h>
#include "ima.h"
#include "Compression.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


Image *image;
char Mode = 0;
void compression();


#define ESCAPE 27

void Keyboard(unsigned char key, int x, int y)  {
  switch(key){
  case ESCAPE :
    exit(0);                   
    break;
  default:
    fprintf(stderr, "Unused key\n");
  }
}

void Mouse(int button, int state, int x, int y) {

  switch(button){
  case GLUT_LEFT_BUTTON:
    break;
  case GLUT_MIDDLE_BUTTON:
    break;
  case GLUT_RIGHT_BUTTON:
    break;    
  }
  glutPostRedisplay();
}






int Init(char *s) {
  image = (Image *) malloc(sizeof(Image));
  if (image == NULL) {
    fprintf(stderr, "Out of memory\n");
    return(-1);
  }
  if (ImageLoad_PPM(s, image)==-1) 
	return(-1);
  printf("tailles %d %d\n",(int) image->sizeX, (int) image->sizeY);

  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_FLAT);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glutReshapeWindow(image->sizeX, image->sizeY);
  
  return (0);
}


int ReInit() {
  /*
  if (ImageLoad_PPM(s, image)==-1) 
	return(-1);
  */
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_FLAT);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glutReshapeWindow(image->sizeX, image->sizeY);
  
  return (0);
}


void Display(void) {
  
  GLint w, h;

  glClear(GL_COLOR_BUFFER_BIT);

  w = glutGet(GLUT_WINDOW_WIDTH);
  h = glutGet(GLUT_WINDOW_HEIGHT);
  glDrawPixels(image->sizeX, image->sizeY, GL_RGB, GL_UNSIGNED_BYTE, 
	       image->data);

  glFlush();
}

void Reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, (GLdouble) w, 0.0, (GLdouble)h);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void menuFunc(int item) {
  char s[256];

  switch(item){
  case 0:
    free(image);
    exit(0);
    break;
  case 10:
    printf(" je suis dans clut \n");
    compression();
    Display();
  default:
    break;
  }
}


void Compression(char * fich_cmp){

  int facteur = 3;
  // int size = image->sizeX* image->sizeY *3;
  unsigned short width = image->sizeX;
  unsigned short height = image -> sizeY;
  int size = width* height *3;


  Image * image_copy = (Image*)calloc(1,sizeof(Image));
  assert(image_copy);


  image_copy->sizeX = image->sizeX;
  image_copy-> sizeY = image->sizeY;
  image_copy->data = (unsigned char *)calloc(size,sizeof(unsigned char));
  assert(image_copy->data);


  memcpy(image_copy->data, image->data, size);


  unsigned char * dither_tab ;
  dither_tab = tab_couleur_creation_dithering(image_copy, NULL, size/3,facteur);

  unsigned char  * palette_index_tab = indice_palette_creation(dither_tab,size/3);
  FILE * stream = fopen(fich_cmp, "wb" );
  if (stream == NULL)  /* If an error occurs during the file creation */
   {  
     fprintf(stderr, "fopen() failed for '%s'\n", "compresse.bin");
     exit(1);
   }

   printf(" %lu ",sizeof(palette_index_tab)*size/3);
   int element_size = size/3;
   int elements_to_write = 1;

    fwrite(&width,2,1,stream);
    fwrite(&height,2,1,stream);
    printf(" width_written = %d \n",width); 
    printf(" height_written =  %d \n",height);

   size_t elements_written = fwrite(palette_index_tab, element_size, elements_to_write, stream);
   
      fclose(stream);


}


void init_image_from_cmp_file(int width ,int height, unsigned char * color){
  image = (Image *) malloc(sizeof(Image));
  if (image == NULL) {
    fprintf(stderr, "Out of memory\n");
    exit(-1);
  }
  image->sizeX = width;
  image->sizeY = height;
  /* allocation memoire */
	int size = width* height* 3;
  image->data = (unsigned char *)calloc(size,sizeof(unsigned char));
  assert(image->data);
	image->data = color;

  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_FLAT);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glutReshapeWindow(width, height);

}




void Decompression(char * nom_fichier){

  int nbr_pixel = 0;
  int width_readen = 0,height_readen = 0;

  FILE * stream = fopen(nom_fichier, "rb" );
  if (stream == NULL)  /* If an error occurs during the file creation */
  {  
      fprintf(stderr, "fopen() failed for '%s'\n", "compresse.bin");
      exit(1);
  }

  fread(&width_readen,2,1,stream);
  fread(&height_readen,2,1,stream);

  nbr_pixel = width_readen*height_readen;
  printf(" width_readen = %d  , height_readen = %d  \n",width_readen,height_readen);

  unsigned char * palette_index_from_file = (unsigned char *)calloc(nbr_pixel,sizeof(unsigned char));
  assert(palette_index_from_file);

  int element_size = nbr_pixel;
  int elements_to_read = 1;

  size_t elements_readen = fread(palette_index_from_file,element_size , elements_to_read, stream); 
  unsigned char * image_coul = create_image_from_index_ref(palette_index_from_file, nbr_pixel);

  init_image_from_cmp_file(width_readen,height_readen,image_coul);
  fclose(stream);

}





void compression(){
  int size = image->sizeX* image->sizeY *3;
  unsigned char * tab_image = (unsigned char *)calloc(size, sizeof(unsigned char));
  unsigned int * tab_resultat;


  // creation du tableau des moyennes des couleurs
  tab_resultat = tab_moy_creation(image->data,size/3);
  // unsigned char *tab_couleur;

  // for(int i = 0 ;i< 10 ;i++){
  //   printf(" %u |",tab_resultat[i]);
  // }
  printf("\n");

  // Trier le tableau des moyennes avec QuickSort et mettre à jour les indices des couleurs dans l'image
  quickSort(tab_resultat, 0, (size/3)-1);

  // tab_resultat = reorganiser_tab_int(tab_resultat,size/3);
  // reorganiser_tab_int(tab_resultat,size/3);

  // printf(" Après oranisation ");
  // for(int i = 0 ;i< 10 ;i++){
  //   printf(" %u |",tab_resultat[i]);
  // }
  // printf("\n");

  // tab_couleur = tab_couleur_creation(tab_resultat, size/3);

  // Compression du tableau des couleurs triés et suppression des répétition 
  int nbr_el_cmp = compter_taille_tab_compresse(tab_resultat, size/3);
  printf(" nbr elm cmp = %d \n",nbr_el_cmp);
  unsigned int * compresse_tab_result = compresse_clut(tab_resultat,size/3,nbr_el_cmp);
  // for(int i = 0 ; i< size/3; i++){
  //   printf(" %u -> %u fois |\n",compresse_tab_result[i].val,compresse_tab_result[i].nbr_pixel_val);
  // }

  // // redécomresser lea tableau des moyennes des couleurs  
  // unsigned int * tab_dcmp;
  // tab_dcmp = tab_decompresse(compresse_tab_result,nbr_el_cmp,size/3);

  // // Transformer le tableau des moyenne en un tableau rgb pour le réafficher
  // unsigned char *tab_couleur;
  // tab_couleur = tab_couleur_creation(tab_dcmp, size/3);

  unsigned char * compressed_tab_couleur = tab_couleur_creation(compresse_tab_result, nbr_el_cmp*3);

  //   printf("compresse_tab_couleur : ");
  // for(int i = 0 ;i< nbr_el_cmp*3; i++){
  //   printf(" %u |",compressed_tab_couleur[i]);
  // }
  // printf("\n");
  int facteur = 3;
  // convert_to_palette_color(compressed_tab_couleur,nbr_el_cmp*3,facteur);
  // unsigned char * tab_coul_depuis_coul_cmp ;
  // tab_coul_depuis_coul_cmp = tab_couleur_creation_depuis_rgb_compresse(compressed_tab_couleur ,size/3,nbr_el_cmp);
  // image->data = tab_coul_depuis_coul_cmp;
  // verif(tab_coul_depuis_coul_cmp,size);

  // printf(" tab_coul_depuis_coul_cmp : ");
  // for(int i = 0 ;i< size; i++){
  //   printf(" %u |",tab_coul_depuis_coul_cmp[i]);
  // }
  // printf("\n");

  unsigned char * dither_tab ;
  dither_tab = tab_couleur_creation_dithering(image, NULL, size/3,facteur);
  image->data = dither_tab;

  unsigned char  * palette_index_tab = indice_palette_creation(dither_tab,size/3);



  FILE * stream = fopen("compresse.bin", "wb" );
  if (stream == NULL)  /* If an error occurs during the file creation */
   {  
     fprintf(stderr, "fopen() failed for '%s'\n", "compresse.bin");
     exit(1);
   }

   printf(" %lu ",sizeof(palette_index_tab)*size/3);
   int element_size = size/3;
   int elements_to_write = 1,elements_to_read = 1;
   size_t elements_written = fwrite(palette_index_tab, element_size, elements_to_write, stream);

  unsigned char * palette_index_from_file = (unsigned char *)calloc(size/3,sizeof(unsigned char));
  assert(palette_index_tab);
   
  fclose(stream);

  stream = fopen("compresse.bin", "rb" );
  if (stream == NULL)  /* If an error occurs during the file creation */
   {  
     fprintf(stderr, "fopen() failed for '%s'\n", "compresse.bin");
     exit(1);
   }

  size_t elements_readen = fread(palette_index_from_file,element_size , elements_to_read, stream); 
  

  unsigned char * image_coul = create_image_from_index_ref(palette_index_from_file, size/3);
  // printf(" image_coul \n");
  // for(int i = 0 ; i< size; i++){
  //   printf(" %u |",image_coul[i]);
  // }
  // printf("\n");
  verif(image->data,size);
  image->data = image_coul;


}



int main(int argc, char **argv) {  

  if (argc != 3 && argc != 4 ) {
    fprintf(stderr, "Usage : Veuillez saisir le bon format pour exécuter le programme \n  \\
                    \t -Pour La compression : ./Palette <1> <Image_a_compresser> <Ficher_ou_compresser> \n \\
                    \t -Pour La Décompression : ./Palette <2> <Ficher_a_décompresser>\n");
    exit(0);           
  }

  Mode = argv[1][0];

  if(Mode == '1'){
    printf(" Mode Compression \n");
    glutInit(&argc, argv); 
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
    glutInitWindowSize(640,480);  
    glutInitWindowPosition(100, 100);  
    glutCreateWindow("VPUP8");  
    // initialisation de l'image
    Init(argv[2]);
    // Compression de l'image
    Compression(argv[3]);
    glutDisplayFunc(Display);  
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
    

  }else if(Mode == '2'){

    printf(" Mode Déompression \n");
    
    glutInit(&argc, argv); 
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
    glutInitWindowSize(640,480);  
    glutInitWindowPosition(100, 100);  
    glutCreateWindow("VPUP8");  
    // Decompression de l'image 
    Decompression(argv[2]);
    glutDisplayFunc(Display);  
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);


  }else if(Mode == 3){
    printf("Mode Demo \n");



  }else{
    printf(" Mode Inconnu \n ");
    exit(-1);
  }

  

  glutMainLoop();  

  return 1;
}



void Affiche_tab(unsigned int *tab, int size){
  for(int i = 0 ;i< size ; i++){
    printf(" %u |",tab[i]);
  }
  printf("\n");
}



// int main(int argc, char **argv){

//   int nbr_pixel = 8; 
//   // unsigned int a = 4294967050 ;
//   unsigned int * tab_resultat;
//   unsigned char tab_test[24] = {10,43,150,230,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255};
  
//   // unsigned int tab_test[15] = {0,56,21,9,0,1,0,9,0,56,21,9,0,1,1};


//   tab_resultat = tab_moy_creation(tab_test,nbr_pixel);
//   print_tab_int_Bits(nbr_pixel, tab_resultat);
//   // unsigned char *tab_couleur;
//   // tab_couleur = tab_couleur_creation(tab_resultat, nbr_pixel);

//   Affiche_tab(tab_resultat, nbr_pixel);
//   print_tab_ind_couleur(nbr_pixel);
//   quickSort(tab_resultat, 0, nbr_pixel-1);
//   print_tab_ind_couleur(nbr_pixel);
//   Affiche_tab(tab_resultat, nbr_pixel);

//   // tab_resultat = reorganiser_tab_int(tab_resultat,nbr_pixel);

  
//   printf(" Après organisation ");
//   for(int i = 0 ;i< nbr_pixel ;i++){
//     printf(" %u |",tab_resultat[i]);
//   }
//   printf("\n");


//   //  Partie compression du tableau de Clut ordonné
//   int nbr_el_cmp = compter_taille_tab_compresse(tab_resultat, nbr_pixel);
//   unsigned int * compresse_tab_result = compresse_clut(tab_resultat,nbr_pixel,nbr_el_cmp);
//     printf("compresse_tab_result : ");
//   for(int i = 0 ;i< nbr_el_cmp; i++){
//     printf(" %u |",compresse_tab_result[i]);
//   }
//   printf("\n");

//   unsigned char * compressed_tab_couleur = tab_couleur_creation(compresse_tab_result, nbr_el_cmp*3);

//     printf("compresse_tab_couleur : ");
//   for(int i = 0 ;i< nbr_el_cmp*3; i++){
//     printf(" %u |",compressed_tab_couleur[i]);
//   }
//   printf("\n");




//   // unsigned int * tab_dcmp;
//   // tab_dcmp = tab_decompresse(compresse_tab_result,nbr_el_cmp,nbr_pixel);

//   // printf("tab_dcmp : ");
//   // for(int i = 0 ;i< nbr_pixel; i++){
//   //   printf(" %u |",tab_dcmp[i]);
//   // }
//   // printf("\n");

//   // print_tab_ind_couleur(nbr_pixel);

//   // unsigned char *tab_couleur;
//   // tab_couleur = tab_couleur_creation(tab_dcmp, nbr_pixel);
//   //   printf("tab_couleur : ");
//   // for(int i = 0 ;i< nbr_pixel*3; i++){
//   //   printf(" %u |",tab_couleur[i]);
//   // }
//   // printf("\n");

//   // print_tab_ind_couleur(nbr_pixel);


//   // unsigned char *tab_couleur_cmp;
//   // tab_couleur_cmp = tab_couleur_creation(compresse_tab_result, nbr_el_cmp);

//   // printf("tab_couleur_cmp : ");
//   // for(int i = 0 ;i< nbr_el_cmp*3; i++){
//   //   printf(" %u |",tab_couleur_cmp[i]);
//   // }
//   // printf("\n");
//   // print_tab_ind_couleur(nbr_pixel);

  
//   // trier_tableau_ind(nbr_pixel);  
//   // printf("aprés trier \n");
//   // print_tab_ind_couleur(nbr_pixel);

//   int facteur = 3;
//   convert_to_palette_color(compressed_tab_couleur,nbr_el_cmp*3,facteur);
//     printf(" Apres conv palette col  : ");
//   for(int i = 0 ;i< nbr_el_cmp*3; i++){
//     printf(" %u |",compressed_tab_couleur[i]);
//   }
//   printf("\n");

//   unsigned char * tab_coul_depuis_coul_cmp ;
//   tab_coul_depuis_coul_cmp = tab_couleur_creation_depuis_rgb_compresse(compressed_tab_couleur ,nbr_pixel,nbr_el_cmp);
//     printf(" tab_coul_depuis_coul_cmp : ");
//   for(int i = 0 ;i< nbr_pixel*3; i++){
//     printf(" %u |",tab_coul_depuis_coul_cmp[i]);
//   }
//   printf("\n");

//   // stock_couleur_reff(compressed_tab_couleur,nbr_el_cmp*3,facteur);
//   //   printf(" stock_color_reff : ");
//   // for(int i = 0 ;i< nbr_el_cmp*3; i++){
//   //   printf(" %u |",compressed_tab_couleur[i]);
//   // }
//   // printf("\n");


//   // unsigned char * tab_coul_depuis_coul_reff ;
//   // tab_coul_depuis_coul_reff = tab_couleur_creation_depuis_rgb_compresse(compressed_tab_couleur ,nbr_pixel,nbr_el_cmp);
//   //   printf(" tab_coul_depuis_coul_reff : ");
//   // for(int i = 0 ;i< nbr_pixel*3; i++){
//   //   printf(" %u |",tab_coul_depuis_coul_reff[i]);
//   // }
//   // printf("\n");




//   // print_tab_ind_couleur(nbr_pixel);

//   // free(tab_resultat);
//   // tab_resultat = NULL;
//   // free(image);
//   // image = NULL;

// }



// int main(int argc, char **argv){

//   int nbr_pixel = 8;
//   unsigned char tab_test[24] = {255,255,0,130,0,240,90,130,55/*core*/,76,255,255,255,255,80,255,255,255,255,255,255,255,255,255};
//   int width = 4;
//   int height = 2;
//   Image im = {4,2,tab_test};

//   printf(" tab_test \n");
//   for(int i =0; i< nbr_pixel*3; i++){
//     printf(" %u |",tab_test[i]);
//   }


//   int facteur = 3;
//   unsigned char * dither_tab ;
//   dither_tab = tab_couleur_creation_dithering(&im, NULL, nbr_pixel,facteur);
       
//   printf("\n dethering tab \n");
//   for(int i =0; i< nbr_pixel*3; i++){
//     printf(" %u |",dither_tab[i]);
//   }
//   printf("\n");

//   unsigned char  * palette_index_tab = indice_palette_creation(dither_tab,nbr_pixel);

//   printf("indice tab : ");
//   for(int i = 0 ; i< nbr_pixel ; i++){
//     printf(" %u |",palette_index_tab[i]);
//   }
//   printf("\n");



//   unsigned char * image = create_image_from_index_ref(palette_index_tab, nbr_pixel);
//   printf(" image \n");
//   for(int i = 0 ; i< nbr_pixel*3; i++){
//     printf(" %u |",image[i]);
//   }
//   printf("\n");

//   // unsigned char b = 14.7;
//   // printf(" %u ",b);


//   FILE * stream = fopen("compresse.bin", "wb" );
//   if (stream == NULL)  /* If an error occurs during the file creation */
//    {  
//      fprintf(stderr, "fopen() failed for '%s'\n", "compresse.bin");
//      exit(1);
//    }

//    printf(" %lu ",sizeof(palette_index_tab)*nbr_pixel);
//    int element_size = nbr_pixel;
//    int elements_to_write = 1;
//    size_t elements_written = fwrite(palette_index_tab, element_size, elements_to_write, stream); 

//   fclose(stream);

//   stream = fopen("compresse.bin", "rb" );
//   if (stream == NULL)  /* If an error occurs during the file creation */
//    {  
//      fprintf(stderr, " Erreur de lecture dans le fichier ");
//      exit(1);
//    }



//   unsigned char tab_read_from_file[8];
//   size_t elements_readen = fread(tab_read_from_file, element_size, elements_to_write, stream);



//   printf(" tab_read_from_file \n");
//   for(int i = 0 ; i< nbr_pixel;i++){
//     printf(" %u| ",tab_read_from_file[i]);
//   }

//   return 0;
// }