#include <stdlib.h>
#include <unistd.h>     
#include <math.h>
#include "ima.h"
#include "Compression.h"

Image *image;
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
  case 1:
    printf("Gris pondéré\n");
    gris (image);
    Display();
    break;
  case 2:
    printf("sobel \n");
    sobel(image);
    Display();
    break;
  case 3:
  case 4:
    printf("Revenir image départ\n");
    gris_uniforme(image);
    Display();
    break;
  case 5:
    printf("Entrer le nom pour l'image dans cette taille\n");
    scanf("%s", &s[0]);
    imagesave_PPM(s, image);
    break;
  case 6:
    printf("Taille de l image : %ld %ld\n", (int) image->sizeX, (int) image->sizeY);
    break;
  case 7:
    mon_moy_dif (image);
    break;
  case 8:
    negatif (image);
    break;
  case 9:
    inverse(image);
    Display();
    break;
  case 10:
    printf(" je suis dans clut \n");
    compression();

    Display();
  default:
    break;
  }
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


  unsigned char * tab_coul_depuis_coul_cmp ;
  tab_coul_depuis_coul_cmp = tab_couleur_creation_depuis_rgb_compresse(compressed_tab_couleur ,size/3,nbr_el_cmp);

  // printf(" tab_coul_depuis_coul_cmp : ");
  // for(int i = 0 ;i< size; i++){
  //   printf(" %u |",tab_coul_depuis_coul_cmp[i]);
  // }
  // printf("\n");




  image->data = tab_coul_depuis_coul_cmp;

}



// int main(int argc, char **argv) {  
//   if (argc<2) {
//     fprintf(stderr, "Usage : palette nom_de_fichier\n");
//     exit(0);
//   }


//   glutInit(&argc, argv); 
//   glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
//   glutInitWindowSize(640,480);  
//   glutInitWindowPosition(100, 100);  
//   glutCreateWindow("VPUP8");  

//   Init(argv[1]);

//   glutCreateMenu(menuFunc);
//   glutAddMenuEntry("Quit", 0);
//   glutAddMenuEntry("gris", 1);
//   glutAddMenuEntry("Sobel", 2);
//   glutAddMenuEntry("GRIS", 3);
//   glutAddMenuEntry("grey", 4);
//   glutAddMenuEntry("Sauver", 5);
//   glutAddMenuEntry("Informations", 6);
//   glutAddMenuEntry("moy_dif", 7);
//   glutAddMenuEntry("negatif", 8);
//   glutAddMenuEntry("inverse", 9);
//   glutAddMenuEntry("Clut", 10);
//   glutAttachMenu(GLUT_LEFT_BUTTON);

//   glutDisplayFunc(Display);  
//   glutReshapeFunc(Reshape);
//   glutKeyboardFunc(Keyboard);
  
//   glutMouseFunc(Mouse);

//   glutMainLoop();  

//   return 1;
// }



void Affiche_tab(unsigned int *tab, int size){
  for(int i = 0 ;i< size ; i++){
    printf(" %u |",tab[i]);
  }
  printf("\n");
}


int main(int argc, char **argv){

  int nbr_pixel = 8; 
  // unsigned int a = 4294967050 ;
  unsigned int * tab_resultat;
  unsigned char tab_test[24] = {3,0,0,45,0,4,1,34,0,1,0,0,1,200,0,1,0,0,1,6,0,1,0,0};
  
  // unsigned int tab_test[15] = {0,56,21,9,0,1,0,9,0,56,21,9,0,1,1};


  tab_resultat = tab_moy_creation(tab_test,nbr_pixel);
  print_tab_int_Bits(nbr_pixel, tab_resultat);
  // unsigned char *tab_couleur;
  // tab_couleur = tab_couleur_creation(tab_resultat, nbr_pixel);

  Affiche_tab(tab_resultat, nbr_pixel);
  print_tab_ind_couleur(nbr_pixel);
  quickSort(tab_resultat, 0, nbr_pixel-1);
  print_tab_ind_couleur(nbr_pixel);
  Affiche_tab(tab_resultat, nbr_pixel);

  // tab_resultat = reorganiser_tab_int(tab_resultat,nbr_pixel);

  
  printf(" Après organisation ");
  for(int i = 0 ;i< nbr_pixel ;i++){
    printf(" %u |",tab_resultat[i]);
  }
  printf("\n");


  //  Partie compression du tableau de Clut ordonné
  int nbr_el_cmp = compter_taille_tab_compresse(tab_resultat, nbr_pixel);
  unsigned int * compresse_tab_result = compresse_clut(tab_resultat,nbr_pixel,nbr_el_cmp);
    printf("compresse_tab_result : ");
  for(int i = 0 ;i< nbr_el_cmp; i++){
    printf(" %u |",compresse_tab_result[i]);
  }
  printf("\n");

  unsigned char * compressed_tab_couleur = tab_couleur_creation(compresse_tab_result, nbr_el_cmp*3);

    printf("compresse_tab_couleur : ");
  for(int i = 0 ;i< nbr_el_cmp*3; i++){
    printf(" %u |",compressed_tab_couleur[i]);
  }
  printf("\n");

  // unsigned int * tab_dcmp;
  // tab_dcmp = tab_decompresse(compresse_tab_result,nbr_el_cmp,nbr_pixel);

  // printf("tab_dcmp : ");
  // for(int i = 0 ;i< nbr_pixel; i++){
  //   printf(" %u |",tab_dcmp[i]);
  // }
  // printf("\n");

  // print_tab_ind_couleur(nbr_pixel);

  // unsigned char *tab_couleur;
  // tab_couleur = tab_couleur_creation(tab_dcmp, nbr_pixel);
  //   printf("tab_couleur : ");
  // for(int i = 0 ;i< nbr_pixel*3; i++){
  //   printf(" %u |",tab_couleur[i]);
  // }
  // printf("\n");

  // print_tab_ind_couleur(nbr_pixel);


  // unsigned char *tab_couleur_cmp;
  // tab_couleur_cmp = tab_couleur_creation(compresse_tab_result, nbr_el_cmp);

  // printf("tab_couleur_cmp : ");
  // for(int i = 0 ;i< nbr_el_cmp*3; i++){
  //   printf(" %u |",tab_couleur_cmp[i]);
  // }
  // printf("\n");
  // print_tab_ind_couleur(nbr_pixel);

  
  // trier_tableau_ind(nbr_pixel);  
  // printf("aprés trier \n");
  // print_tab_ind_couleur(nbr_pixel);


  unsigned char * tab_coul_depuis_coul_cmp ;
  tab_coul_depuis_coul_cmp = tab_couleur_creation_depuis_rgb_compresse(compressed_tab_couleur ,nbr_pixel,nbr_el_cmp);

  printf(" tab_coul_depuis_coul_cmp : ");
  for(int i = 0 ;i< nbr_pixel*3; i++){
    printf(" %u |",tab_coul_depuis_coul_cmp[i]);
  }
  printf("\n");

  // print_tab_ind_couleur(nbr_pixel);

  // free(tab_resultat);
  // tab_resultat = NULL;
  // free(image);
  // image = NULL;

}


