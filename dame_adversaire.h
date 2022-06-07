#ifndef _DAME_AD_
#define _DAME_AD_

#include "dame_fonction_commune.h"

int ScanToutDammier(int **damier, int TAILLECOTEDAMIER, int monJoueur, int adversaire);
int ScanCases ( int **damier,int j, int i, int joueur , int adversaire );
int verif_deplacement_possible( int **damier, Coord depart, Coord arrive, int joueur);
int ScanCases_dame ( int **damier,int j, int i, int joueur , int adversaire );
int ScanCases_dame_nn ( int **damier,int j, int i, int joueur , int adversaire );
int ScanCases_dame_ss ( int **damier,int j, int i, int joueur , int adversaire );
int ScanCases_dame_ne ( int **damier,int j, int i, int joueur , int adversaire );
int ScanCases_dame_so ( int **damier,int j, int i, int joueur , int adversaire );
int deplacer_dame(int **damier, Coord depart, Coord arrive, int valeur_dame);
#endif
