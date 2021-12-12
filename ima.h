


#ifndef IMA
#define IMA

#include <stdio.h> 
#include <stdlib.h>
#include <assert.h>


#include <GL/gl.h>      
#include <GL/glu.h>
#include <GL/glut.h>

    struct Image {
        unsigned long sizeX;
        unsigned long sizeY;
        GLubyte *data;
    };
    typedef struct Image Image;
    typedef unsigned short utab [3][3][3];

    int ImageLoad_PPM(char *filename, Image *image);
    void imagesave_PPM(char *filename, Image *image);
    void upsidedown(Image *);
    void gris_uniforme (Image *);
    void mon_moy_dif(Image *);
    void negatif(Image *);
    void inverse(Image *);
    void gris (Image *);
    void sobel (Image *);


 #endif
