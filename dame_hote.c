#include "dame_hote.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
/*Les fonction permettant de creer l'arbre quaternaire*/
void insert (Quadtree * A, int v) {
    Quadtree newNode = malloc(sizeof(struct _node));
    newNode->key = v;
    newNode->nord = NULL;
    newNode->sud = NULL; 
    newNode->est = NULL;
    newNode->ouest = NULL; 
    (*A) = newNode;
}

int height (Quadtree A) {
    int h_n, h_s, h_o, h_e;
    if(A == NULL)
        return -1;
    if(A->nord == NULL && A->sud == NULL && A->est == NULL && A->ouest == NULL)
        return 0;
    h_n = 1 + height(A->nord);
    h_s = 1 + height(A->sud);
    h_e = 1 + height(A->est);
    h_o = 1 + height(A->ouest);
    return maximum(h_n, h_s, h_o, h_e);
}

/* Libération complète de la mémoire occupée par un arbre */
void free_tree (Quadtree A) {
    if(A == NULL)
        return;
    free_tree(A->nord);
    free_tree(A->sud);
    free_tree(A->ouest);
    free_tree(A->est);
    free(A);
}

/*Remplir dans un tableau les étiquette de la longue branche de l'arbre quaternaire*/
void remplissage(Quadtree A, int *tab, int *indice){
    if(A != NULL){
        int h_n, h_s, h_o, h_e;
        tab[(*indice)] = A->key;
        *indice = (*indice)+1;
        
        h_n = height(A->nord);
        h_s = height(A->sud);
        h_e = height(A->est);
        h_o = height(A->ouest);
        
        if(h_n > h_s && h_n > h_e && h_n > h_o)
            remplissage(A->nord, tab, indice);
        else if(h_s > h_e && h_s > h_o)
            remplissage(A->sud, tab, indice);
        else if (h_e > h_o)
            remplissage(A->est, tab, indice);
        else
            remplissage(A->ouest, tab, indice);
    }
}

/**-------------------------------------------------------------------------------------------------------------*/
void melange(int *tableau_a_melanger, int taille){
    if(taille == 0)
        return;
    time_t t;
    srand((unsigned) time(&t));
    int rand1, rand2, tmp;
    for(int i = 0; i < 20; i++){
        rand1 = rand()%taille;
        rand2 = rand()%taille;
        tmp = tableau_a_melanger[rand1];
        tableau_a_melanger[rand1] = tableau_a_melanger[rand2];
        tableau_a_melanger[rand2] = tmp;
    }
}

int aleatoire(int **damier,int *tab_aleatoire, int joueur, int tab_indice[][11]){
   // contient tous les pions du joueur en notation manoury
    int nb_elem = 0;
    for(int i =  1; i <= 10 ; i++){
        for(int j = 1; j <= 10 ; j++)
            if(damier[i][j] == joueur  || damier[i][j] == joueur+2 ){
                tab_aleatoire[nb_elem] = tab_indice[i][j];
                nb_elem++;
            }
    }
    melange(tab_aleatoire, nb_elem);
    return nb_elem;
}

/**-----------------------------------------------------------------------------------------------------------*/

/*Fonction naive qui recherche une case jouable */
int cherche_case_jouable(int **damier, int joueur, char *dep, int tab_indice[][11], Coord *coord){
    int k;
    if (joueur == 1)
        k = -1;
    if (joueur == 2)
        k = 1; 
    int tab_aleatoire[20];

    int taille = aleatoire(damier, tab_aleatoire, joueur, tab_indice);
    Coord depart , arrive;
    int check = 0, i, j, val;

    for(int x =  0; x < taille && check == 0; x++){

	    val = tab_aleatoire[x];
	    i = coord[val].i; 
	    j = coord[val].j;
	    if(damier[i][j]==joueur){
		    if(damier[i+k][j-1] == 0) {
			    check =1;
			    sprintf(dep, "%d-%d",tab_indice[i][j],tab_indice[i+k][j-1]);
			    depart.i = i; depart.j = j;
			    arrive.i = i+k; arrive.j = j-1;
		    }
		    else if (damier[i+k][j+1] == 0){
			    check =1;
			    sprintf(dep, "%d-%d",tab_indice[i][j],tab_indice[i+k][j+1]);
			    depart.i = i; depart.j = j;
			    arrive.i = i+k; arrive.j = j+1;
		    }
	    }

	    else if(damier[i][j] == joueur+2){               
		    if(damier[i-1][j+1] == 0) {
			    check =1;
			    sprintf(dep, "%d-%d",tab_indice[i][j],tab_indice[i-1][j+1]);
			    depart.i = i; depart.j = j;
			    arrive.i = i-1; arrive.j = j+1;
		    }
		    else if (damier[i+1][j+1] == 0){
			    check =1;
			    sprintf(dep, "%d-%d",tab_indice[i][j],tab_indice[i+1][j+1]);
			    depart.i = i; depart.j = j;
			    arrive.i = i+1; arrive.j = j+1;
		    }
		    else if(damier[i+1][j-1] == 0) {
			    check =1;
			    sprintf(dep, "%d-%d",tab_indice[i][j],tab_indice[i+1][j-1]);
			    depart.i = i; depart.j = j;
			    arrive.i = i+1; arrive.j = j-1;
		    }
		    else if (damier[i-1][j-1] == 0){
			    check =1;
			    sprintf(dep, "%d-%d",tab_indice[i][j],tab_indice[i-1][j-1]);
			    depart.i = i; depart.j = j;
			    arrive.i = i-1; arrive.j = j-1;
		    }
	    }
    }
	dep[strlen(dep)] = '\0';
    if(check == 0)
	    return 0;
    deplacer(damier,depart,arrive,joueur);
    return 1;
}

/**----------------------------------------------------------------------------------------------------------------*/

/* Verifier combien de pion au maximum peut capturer un pion */
int ScanCases_hote ( int **damier,int j, int i, int joueur , int adversaire, Quadtree *A , int tab_indice[][11]){
    int nn = 0, ss = 0, so = 0, ne = 0;
    if (j<9 && i < 9){
        if((damier [j+1][i+1] == adversaire || damier [j+1][i+1] == adversaire+2)  &&  damier [j+2][i+2]==0 ){
            damier[j+2][i+2] = 5;
            insert(&((*A)->sud), tab_indice[j+2][i+2]);
            ss = 1 + ScanCases_hote(damier, j+2, i+2, joueur, adversaire, &(*A)->sud,tab_indice);
        }
    }
    if (j>2 && i>2 ){  
        if((damier [j-1][i-1] == adversaire || damier [j-1][i-1] == adversaire +2 ) && damier [j-2][i-2]==0) {
            damier[j-2][i-2] = 5;
            insert(&((*A)->nord), tab_indice[j-2][i-2]);
            nn = 1 + ScanCases_hote(damier, j-2, i-2, joueur, adversaire,&(*A)->nord,tab_indice);
        }
    }
    if (j <9 && i >2 ){
        if((damier[j+1][i-1] == adversaire || damier[j+1][i-1] == adversaire+2) && damier [j+2][i-2]==0) {
            damier[j+2][i-2] = 5;
            insert(&((*A)->est), tab_indice[j+2][i-2]);
            so = 1 + ScanCases_hote(damier, j+2, i-2, joueur, adversaire,&(*A)->est,tab_indice);
        }
    }
    if ( j > 2 && i < 9){
        if((damier[j-1][i+1] == adversaire || damier[j-1][i+1] == adversaire+2) && damier [j-2][i+2]==0){
            damier[j-2][i+2] = 5;
            insert(&((*A)->ouest), tab_indice[j-2][i+2]);
            ne = 1 + ScanCases_hote(damier, j-2, i+2, joueur, adversaire,&(*A)->ouest,tab_indice);
        }
    }

    return maximum(ss,nn,so,ne);
}


/**-----------------------------------------------------------------------------------------------------------------*/

int ScanCases_hote_nn(int **damier,int j, int i, int joueur , int adversaire, Quadtree *A , int tab_indice[][11]){
    if (j>2 && i>2 ){  
        if((damier [j-1][i-1] == adversaire || damier [j-1][i-1] == adversaire +2) && damier [j-2][i-2]==0) {
            damier[j-2][i-2] = 5;
            insert(&((*A)->nord), tab_indice[j-2][i-2]);
            return 1 + ScanCases_hote(damier, j-2, i-2, joueur, adversaire,&(*A)->nord,tab_indice);
        }
        else if(damier [j-1][i-1]== 0) {
            damier[j-1][i-1] = 5;
            return ScanCases_hote_nn(damier, j-1, i-1, joueur, adversaire, A,tab_indice);
        }
    }
    return 0;
}


int ScanCases_hote_ss(int **damier,int j, int i, int joueur , int adversaire, Quadtree *A , int tab_indice[][11]){
    if (j<9 && i < 9){
        if((damier [j+1][i+1] == adversaire || damier [j+1][i+1] == adversaire+2) &&  damier [j+2][i+2]==0) {
            damier[j+2][i+2] = 5;
            insert(&((*A)->sud), tab_indice[j+2][i+2]);
            return 1 + ScanCases_hote(damier, j+2, i+2, joueur, adversaire, &(*A)->sud,tab_indice);
        }
        else if(damier [j+1][i+1]== 0) {
             damier[j+1][i+1] = 5;
             return ScanCases_hote_ss(damier, j+1, i+1, joueur, adversaire, A,tab_indice);
        }
    }
    return 0;
}


int ScanCases_hote_ne(int **damier,int j, int i, int joueur , int adversaire, Quadtree *A , int tab_indice[][11]){
    if ( j > 2 && i < 9){
        if((damier[j-1][i+1] == adversaire || damier[j-1][i+1] == adversaire+2) && damier [j-2][i+2]==0){
            damier[j-2][i+2] = 5;
            insert(&((*A)->ouest), tab_indice[j-2][i+2]);
            return 1 + ScanCases_hote(damier, j-2, i+2, joueur, adversaire,&(*A)->ouest,tab_indice);
        }
        else if(damier[j-1][i+1]== 0){
            damier[j-1][i+1] = 5;
            
            return ScanCases_hote_ne(damier, j-1, i+1, joueur, adversaire, A,tab_indice);
        }
    }
    return 0;
 }
 
 
int ScanCases_hote_so(int **damier,int j, int i, int joueur , int adversaire, Quadtree *A , int tab_indice[][11]){
    if (j <9 && i >2 ){
        if((damier[j+1][i-1] == adversaire || damier[j+1][i-1] == adversaire+2) && damier [j+2][i-2]==0) {
            damier[j+2][i-2] = 5;
            insert(&((*A)->est), tab_indice[j+2][i-2]);
            return 1 + ScanCases_hote(damier, j+2, i-2, joueur, adversaire,&(*A)->est,tab_indice);
        }
        else if(damier[j+1][i-1]== 0){
            damier[j+1][i-1] = 5;
            
            return ScanCases_hote_so(damier, j+1, i-1, joueur, adversaire, A,tab_indice);
        }
    }
    return 0;
}
    

int ScanCases_hote_dame(int **damier,int j, int i, int joueur , int adversaire, Quadtree *A , int tab_indice[][11]){
    int nn = ScanCases_hote_nn(damier, j, i, joueur, adversaire, A,tab_indice),
        ss = ScanCases_hote_ss(damier, j, i, joueur, adversaire, A,tab_indice), 
        so = ScanCases_hote_so(damier, j, i, joueur, adversaire, A,tab_indice), 
        ne = ScanCases_hote_ne(damier, j, i, joueur, adversaire, A,tab_indice);   
    return maximum(ss,nn,so,ne);
}

/**-----------------------------------------------------------------------------------------------------------------*/

/*Scanner tous le damier et renvoiyer le nombre de pions max qu'il faut capturé et stock le deplacement dans un abre quaternaire*/
int ScanToutDammier_hote(int **damier, int TAILLECOTEDAMIER, int joueur, int adversaire, int tab_indice[][11], int *tab_dep){
   int i, j, k = 0, max=0; 
    int indice = 0;
    for (j=1; j<=TAILLECOTEDAMIER ; j++){
        for (i= 1; i<=TAILLECOTEDAMIER;i++) {
            Quadtree test = NULL;
            if(damier[j][i] == joueur){
                insert(&test, tab_indice[j][i]);
                k = ScanCases_hote(damier,j, i,joueur,adversaire,&test, tab_indice);
                if(k > max ) {
                    max = k;
                    remplissage(test, tab_dep, &indice);
                    indice = 0;
                }
                for(int x = 1; x <= 10; x++ ){
                    for(int y = 1; y <= 10; y++)
                        if(damier[x][y] == 5)
                            damier[x][y] = 0;
                }
            }

            if(damier[j][i] == joueur+2){
                insert(&test, tab_indice[j][i]);
                k = ScanCases_hote_dame(damier,j, i,joueur,adversaire,&test, tab_indice);
                if(k > max ) {
                    max = k;
                    remplissage(test, tab_dep, &indice);
                    indice = 0;
                }
                for(int x = 1; x <= 10; x++ ){
                    for(int y = 1; y <= 10; y++)
                        if(damier[x][y] == 5)
                            damier[x][y] = 0;
                }                
            }
            free_tree(test);
        }
     }
    return max;
}

/**-----------------------------------------------------------------------------------------------------------------*/

/*Transformer tous les elements du tableau en une chaine: exemple [12,34,65] -> "12x34x65" */
void tabInt_to_chaine(char *deplacement, int *tab, int taille){
    int length = 0;
    for(int i = 0; i < taille-1; i++){
        sprintf(deplacement+length, "%dx",tab[i]);
        length = strlen(deplacement);
    }
    sprintf(deplacement+length, "%d", tab[taille-1]);
    length = strlen(deplacement);
    deplacement[length] = '\0';
}
