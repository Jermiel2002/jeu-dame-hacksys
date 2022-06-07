#ifndef _DAME_C_
#define _DAME_C_
/* fichiers de la bibliothèque standard */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* bibliothèque standard unix */
#include <unistd.h> /* close, read, write */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stddef.h>
/* spécifique à internet */
#include <arpa/inet.h> /* inet_pton */

typedef struct{
    int i;
    int j;
}Coord;

void initialiserdamier (int **damier, int , Coord *coord);
void afficher(int **damier, int );
int lignePaireOuImpaire (int variable);
int maximum(int a , int b, int c , int d);
void recupere_indice(int n_depart,int n_arrive,Coord *depart,Coord *arrive,Coord *coord);
void deplacer(int **damier,Coord ,Coord ,int);
void char_to_int(char *deplacement, int *x , int *y) ;
int verif_coup_max(char *deplacement);
void char_to_int_tab(int *tab, char *dep);
int verif_capture_possible( int **damier, int joueur, int adversaire, int val, Coord *coord, Coord *coord_pion_capture, int *tab);
int capture_possible(int **damier ,Coord depart, Coord arrive,int adversaire, Coord *coord_pion_capture, int indice, int inf);
int capture_possible_dame(int **damier ,Coord depart, Coord arrive,int adversaire, Coord *coord_pion_capture, int indice, int inf);
void capturer(int **damier, Coord *coord , Coord *coord_pion_capture, int *tab,int, int joueur);
void message_erreur(int sock, char * err_mess);
void vider_tempon(char *dep , int taille);
ssize_t exact_write(int fd, void *buf, size_t nbytes);
ssize_t exact_read(int fd, char *buf);
uint8_t remplissage_octets_dep(uint8_t *donnees, uint8_t nb_octets_ecris, int depart , int arrive);
uint8_t remplissage_octets_capture(uint8_t *donnees, uint8_t nb_octets_ecris, int *tab_deplacement, int taille_tab);
int enregistrement_de_la_partie(uint8_t *donnees, int taille);
void promu_dame(int **damier,int TAILLECOTEDAMIER);

#endif