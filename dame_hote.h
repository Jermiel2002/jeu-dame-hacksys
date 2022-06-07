#ifndef _DAME_H_
#define _DAME_H_

#include "dame_fonction_commune.h"

typedef struct _node {
    int key;
    struct _node * nord;
    struct _node * sud;
    struct _node * est;
    struct _node * ouest;
} *Quadtree;

void remplissage(Quadtree A, int *tab, int *indice);
void free_tree (Quadtree A);
void insert (Quadtree * A, int v) ;
int height (Quadtree A) ;
int ScanToutDammier_hote(int **damier, int, int, int, int tab_indice[][11], int *tab_dep);
int ScanCases_hote ( int **damier,int j, int i, int , int, Quadtree * , int tab_indice[][11]);
int ScanCases_nn ( int **damier,int j, int i, int , int, Quadtree * , int tab_indice[][11]);
int ScanCases_ss ( int **damier,int j, int i, int , int, Quadtree * , int tab_indice[][11]);
int ScanCases_so ( int **damier,int j, int i, int , int, Quadtree * , int tab_indice[][11]);
int ScanCases_ne ( int **damier,int j, int i, int , int, Quadtree * , int tab_indice[][11]);
int ScanCases_hote_dame ( int **damier,int j, int i, int , int, Quadtree * , int tab_indice[][11]);
int cherche_case_jouable(int **damier, int joueur, char *dep, int tab_indice[][11], Coord *coord);
int aleatoire(int **damier,int *tab_aleatoire, int joueur, int tab_indice[][11]);
void melange(int *tableau_a_melanger, int taille);
void tabInt_to_chaine(char *deplacement, int *tab, int taille);
#endif