#include <stdlib.h>
#include <unistd.h>     
#include <math.h>
#include "ima.h"
#include "Compression.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


Image *image;
unsigned char * copy_image;

char Mode = 0;
void compression();
void Demo_compression_sans_dithering();
void Compression(char * fich_cmp,int debordement);
void Save_im_origin();
void recuperer_riginal_image();
void init_image_from_cmp_file(int width ,int height, unsigned char * color);

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
    // Afficher l'image originale. 
    recuperer_riginal_image();
    Display();
    break;
  case 2:
    // Compression d'image sans dithering.
    Demo_compression_sans_dithering();
    Display();
    break;
  case 3:
    // Compression d'image avec dithering mais avec le débordement.
    Compression(NULL,1);
    Display();
    break;
  default:
    break;
  }
}


void recuperer_riginal_image(){
  int size = image->sizeX*image->sizeY*3;
  memcpy(image->data,copy_image,size);
}



void Save_im_origin(){
  int size = image->sizeX*image->sizeY*3;
  copy_image = (unsigned char*)calloc(size,sizeof(unsigned char));
  assert(copy_image);
  memcpy(copy_image, image->data, size);
}




void Compression(char * fich_cmp,int debordement){
  int facteur = 3;
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
  if(debordement){
    dither_tab = tab_couleur_creation_dithering_avec_debordement(image_copy, NULL, size/3,facteur);
  }else{
    dither_tab = tab_couleur_creation_dithering(image_copy, NULL, size/3,facteur);
  }

  unsigned char  * palette_index_tab = indice_palette_creation(dither_tab,size/3);

  // compresser le tableau des indices 
  unsigned char * tableau_indices_compresse = cmp_8b_to_6b(palette_index_tab, size/3);


  if(debordement){
    unsigned char * image_coul = create_image_from_index_ref(palette_index_tab, size/3);
    image->data = image_coul;

  }else{
      FILE * stream = fopen(fich_cmp, "wb" );
      if (stream == NULL)  /* If an error occurs during the file creation */
      {  
        fprintf(stderr, "Veuillez Saisir un nom de fichier \n");
        exit(1);
      }

      printf(" %lu ",sizeof(palette_index_tab)*size/3);

      int size_av_cmp  = size/3;
      int element_size = (int)(((size_av_cmp *6) /8.0) +0.75);
      int elements_to_write = 1;

      fwrite(&width,2,1,stream);
      fwrite(&height,2,1,stream);
      printf(" width_written = %d \n",width); 
      printf(" height_written =  %d \n",height);
      size_t elements_written = fwrite(tableau_indices_compresse, element_size, elements_to_write, stream);
      fclose(stream);
  }
 
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

  int nbr_pixel = 0 ,nbr_elem_lire = 0;
  int width_readen = 0,height_readen = 0;

  FILE * stream = fopen(nom_fichier, "rb" );
  if (stream == NULL)  /* If an error occurs during the file creation */
  {  
      fprintf(stderr, "Veuillez Saisir un nom de fichier valide \n");
      exit(1);
  }

  fread(&width_readen,2,1,stream);
  fread(&height_readen,2,1,stream);

  nbr_pixel     = width_readen*height_readen;
  nbr_elem_lire = (int)(((nbr_pixel *6) /8.0) +0.75);

  printf(" width_readen = %d  , height_readen = %d  \n",width_readen,height_readen);

  unsigned char * palette_index_from_file = (unsigned char *)calloc(nbr_elem_lire,sizeof(unsigned char));
  assert(palette_index_from_file);


  int element_size = nbr_elem_lire;
  int elements_to_read = 1;

  size_t elements_readen = fread(palette_index_from_file,element_size , elements_to_read, stream); 


  // decompresser le tableau des indices 
  unsigned char * tableau_index_dcmp  =  dcmp_6b_to_8b(palette_index_from_file,nbr_elem_lire);


  unsigned char * image_coul = create_image_from_index_ref(tableau_index_dcmp, nbr_pixel);
  init_image_from_cmp_file(width_readen,height_readen,image_coul);
  fclose(stream);

}


void Demo_compression_sans_dithering(){
  int facteur = 3;
  int size = image->sizeX* image->sizeY *3;
  convert_to_palette_color(image->data,image->sizeX* image->sizeY,facteur);
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
    Compression(argv[3],0);
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


  }else if(Mode == '3'){
    printf("Mode Demo \n");
    glutInit(&argc, argv); 
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
    glutInitWindowSize(640,480);  
    glutInitWindowPosition(100, 100);  
    glutCreateWindow("VPUP8");  
    // initialisation de l'image
    Init(argv[2]);
    Save_im_origin();

    glutCreateMenu(menuFunc);
    glutAddMenuEntry("Quit", 0);
    glutAddMenuEntry("Image originale", 1);
    glutAddMenuEntry("Compression sans dithering", 2);
    glutAddMenuEntry("Compression avec dithering (avec debordement)", 3);
    glutAttachMenu(GLUT_LEFT_BUTTON);


    glutDisplayFunc(Display);  
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);

  }else{
    printf(" Mode Inconnu \n ");
    exit(-1);
  }

  

  glutMainLoop();  

  return 1;
}

