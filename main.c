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
void Demo_compression_sans_dithering();
void Save_im_origin();
void recuperer_riginal_image();
// void init_image_from_cmp_file(int width ,int height, unsigned char * color);

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
    Compression(NULL,1,image);
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
    Compression(argv[3],0,image);
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
    image = Decompression(argv[2],image);
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

