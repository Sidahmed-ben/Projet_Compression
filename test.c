#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


void printBits(size_t const size, void const * const ptr);

void affiche_tab(unsigned char * tab, int taille){
    printf(" tab = ");
    for(int i = 0 ; i< taille ; i++){
        printf(" %u|",tab[i]&63);
    }
    printf("\n");
}


unsigned char ieme_bit(unsigned char bloc , int bit, int placer){
    unsigned char and = (bloc>>bit)&1;
    return and << placer;
}


unsigned char * cmp_8b_to_6b(unsigned char * tab_8b , int taille_tab8){
    int taille_tab6 = (int)(((taille_tab8 *6) /8.0) +0.75) ;

    unsigned char * tab_6b = (unsigned char *)calloc(taille_tab6,sizeof(unsigned char)); 
    assert(tab_6b);

    int ind_shift_8 = 5;
    int ind_tab_8 = 0;

    unsigned char tmp = 0 , bit = 0;
    for(int i = 0 ;i< taille_tab6; i++ ){
        tmp = 0;
        for(int j = 0 ; j< 8 ; j++){
            bit = ieme_bit(tab_8b[ind_tab_8], ind_shift_8, 7-j);
            tmp = tmp | bit ;
            // printf("bit = %u \n",bit);

            ind_tab_8   = ind_shift_8 == 0 ? ind_tab_8+1: ind_tab_8;
            if(ind_tab_8==taille_tab8)
                break;
            ind_shift_8 = ind_shift_8 == 0 ? 5 : ind_shift_8-1 ;
        }
        tab_6b[i] = tmp;
    }
    return tab_6b;
}


unsigned char * dcmp_6b_to_8b(unsigned char * tab_6b , int taille_tab6){
    int taille_tab8 = (int)(((taille_tab6*8) /6.0) );

    unsigned char * tab_8 = (unsigned char *)calloc(taille_tab8, sizeof(unsigned char));
    assert(tab_8);

    int ind_tab_6 = 0;
    int shift_tab6 = 7;

    unsigned char tmp = 0 , bit = 0 ; 
    for(int i = 0 ; i < taille_tab8 ; i++){
        tmp = 0;
        for(int j = 0 ; j < 6 ; j++){
            bit = ieme_bit(tab_6b[ind_tab_6],shift_tab6, 5-j);
            tmp = tmp | bit ;

            ind_tab_6  = shift_tab6 == 0 ? ind_tab_6 +1 : ind_tab_6;
            if(ind_tab_6 == taille_tab6)
                break;
            shift_tab6 = shift_tab6 == 0 ? 7            : shift_tab6-1;
        }
        tab_8[i] = tmp;
    }

    return tab_8;
}






void printBits(size_t const size, void const * const ptr)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;
    
    for (i = 0; i < size ; i++) {
        for (j = 7; j >=0; j--) {
            byte = (b[i] >> j) & 1;
            printf("%u",byte);
        }
        printf("/");
    }
    puts("");
}



int main(){
    unsigned char tab[20] = {9,23,63,8,9,10,54,32,43,9,0,12,9,23,63,8,9,10,54,32} ;   
    int taille = 20;

    affiche_tab(tab, taille);

    printf(" tableau bit : ");
    printBits(taille, tab);
    printf("\n");

    unsigned char * tab6, *tab8 ; 
    tab6 =  cmp_8b_to_6b(tab, taille);
    int taille_6 =  (int)(((taille *6) /8.0) +0.75) ;
    affiche_tab(tab6, taille_6);

    tab8 = dcmp_6b_to_8b(tab6, taille_6);
    int taille_tab8 = (int)(((taille_6*8) /6.0) );
    affiche_tab(tab8, taille_tab8);






    // printf(" tableau 6 : ");
    // printBits(taille_6, tab6);
    // printf("\n");


    // unsigned char saisie;
    // int bit,placement,a,b, ent;
    // while(1){
    //     printf("saisissez un entier \n");
    //     scanf("%d",&ent);
        
    //     // a =  (int)(((ent *6) /8.0) +0.75);
    //     // printf (" < %d > ",a);

    //     b =  (int)(((ent*8) /6.0) );
    //     printf (" < %d > ",b);



    //     // printf("saisissez un entier \n");
    //     // scanf("%s",&saisie);

    //     // printf("saisissez le bit concerné \n");
    //     // scanf("%d",&bit);

    //     // printf("saisissez un placement \n");
    //     // scanf("%d",&placement);


    //     // printf(" < %u > \n",ieme_bit(saisie,bit, placement));

        
    // }

    

    return 0;
}