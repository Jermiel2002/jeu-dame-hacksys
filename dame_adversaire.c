#include "dame_adversaire.h"
#include <stdio.h>
#include <stdlib.h>

/*Verifier si le deplacement est possible*/
int verif_deplacement_possible( int **damier, Coord depart, Coord arrive, int joueur){
	int k;
	if (joueur == 1)
		k = -1;
	if (joueur == 2)
		k = 1; 
		//verifier que le pion a deplacer appartient au bien joueur qui deplace
	if (damier[depart.i][depart.j]!= joueur && damier[depart.i][depart.j]!= joueur+2 ){
		printf("COUP INVALIDE CE N'EST PAS TON PION %d %d\n",depart.i,depart.j);
		return 0;
		}//Verifier si le pion ne se deplace que d'une case et en diagonale
		if ( arrive.i != depart.i+k || damier[arrive.i][arrive.j] != 0){
			printf("COUP INVALIDE, TU NE PEUX TE DEPLACER QUE DE L'AVANT ET QUE D'UNE CASE \n");
			return 0;
		}
		return 1;    
	}

int ScanToutDammier(int **damier, int TAILLECOTEDAMIER, int monJoueur, int adversaire){
	int i, j, k = 0, max=0;
	for (j=1; j<=TAILLECOTEDAMIER ; j++){
		for (i= 1; i<=TAILLECOTEDAMIER;i++) {
			if(damier[j][i] == monJoueur){
				k = ScanCases(damier,j, i,monJoueur,adversaire); 
				if(k > max ) 
					max = k;
				for(int x = 1; x <= 10; x++ ){
					for(int y = 1; y <= 10; y++){
						if(damier[x][y] == 5)
							damier[x][y] = 0;
					}
				}
			}
			if(damier[j][i] == monJoueur+2){
				k = ScanCases_dame(damier,j, i,monJoueur+2,adversaire); 
				if(k > max ) 
					max = k;
				for(int x = 1; x <= 10; x++ ){
					for(int y = 1; y <= 10; y++) {
						if(damier[x][y] == 5)
							damier[x][y] = 0;
					}
				}
			}
		}
	} 
	printf("L'adversaire doit capturer %d pion(s)\n",max);
	return max;
}

int ScanCases ( int **damier,int j, int i, int joueur , int adversaire ){
	int nn = 0, ss = 0, so = 0, ne = 0;
	if (j<9 && i < 9){ 
		if((damier [j+1][i+1] == adversaire || damier [j+1][i+1] == adversaire+2) &&  damier [j+2][i+2]==0){
			damier[j+2][i+2] = 5;
			ss = 1 + ScanCases(damier, j+2, i+2, joueur, adversaire);
		}
	}
	if (j>2 && i>2 ){
	    if((damier [j-1][i-1] == adversaire || damier [j-1][i-1] == adversaire +2 ) && damier [j-2][i-2]==0) {
		    damier[j-2][i-2] = 5;
    		nn = 1 + ScanCases(damier, j-2, i-2, joueur, adversaire);
	    }	
    }
	if (j <9 && i >2 ){
		if((damier[j+1][i-1] == adversaire || damier[j+1][i-1] == adversaire+2) && damier [j+2][i-2]==0) {
    		damier[j+2][i-2] = 5;
        	so = 1 + ScanCases(damier, j+2, i-2, joueur, adversaire);
		}
	}
	if ( j > 2 && i < 9){
    	if((damier[j-1][i+1] == adversaire || damier[j-1][i+1] == adversaire+2) && damier [j-2][i+2]==0){
    		damier[j-2][i+2] = 5;
    		ne = 1 + ScanCases(damier, j-2, i+2, joueur, adversaire);
	    }
	}
	return maximum(ss,nn,so,ne);
}


int ScanCases_dame_nn ( int **damier,int j, int i, int joueur , int adversaire ){
	if (j>2 && i>2 ){
	    if((damier [j-1][i-1] == adversaire || damier [j-1][i-1] == adversaire+2) && damier [j-2][i-2]==0) {
		    damier[j-2][i-2] = 5;
    		return 1 + ScanCases(damier, j-2, i-2, joueur, adversaire);
	    }	
	    else if(damier [j-1][i-1] == 0) {
	    	damier[j-1][i-1] = 5;
	    	return ScanCases_dame_nn(damier, j-1, i-1, joueur, adversaire);
	    }
    }
    return 0;
}

int ScanCases_dame_ss ( int **damier,int j, int i, int joueur , int adversaire ){
	if (j<9 && i < 9){
		if((damier [j+1][i+1] == adversaire || damier [j+1][i+1] == adversaire+2) &&  damier [j+2][i+2]==0){
			damier[j+2][i+2] = 5;
			return 1 + ScanCases(damier, j+2, i+2, joueur, adversaire);
		}
		else if(damier [j+1][i+1] ==0) {
			damier[j+1][i+1] = 5;
			return ScanCases_dame_ss(damier, j+1, i+1, joueur, adversaire);
		}
	}
    return 0;
}

int ScanCases_dame_ne ( int **damier,int j, int i, int joueur , int adversaire ){
if ( j > 2 && i < 9){  
    	if((damier[j-1][i+1] == adversaire || damier[j-1][i+1] == adversaire+2) && damier [j-2][i+2]==0){
    		damier[j-2][i+2] = 5;
    		return 1 + ScanCases(damier, j-2, i+2, joueur, adversaire);
	    }
	    else if(damier[j-1][i+1]==0){
	    	damier[j-1][i+1] = 5;
	    	return ScanCases_dame_ne(damier, j-1, i+1, joueur, adversaire);
	    }
	}
    return 0;
}
int ScanCases_dame_so( int **damier,int j, int i, int joueur , int adversaire ){
	if (j <9 && i >2 ){
		if((damier[j+1][i-1] == adversaire || damier[j+1][i-1] == adversaire+2) && damier [j+2][i-2]==0) {
    		damier[j+2][i-2] = 5;
        	return 1 + ScanCases(damier, j+2, i-2, joueur, adversaire);
		}
		else if(damier[j+1][i-1] == 0) {
			damier[j+1][i-1] = 5;
			return ScanCases_dame_so(damier, j+1, i-1, joueur, adversaire);
		}
	}
    return 0;
}

int ScanCases_dame ( int **damier,int j, int i, int joueur , int adversaire ){
	int nn = ScanCases_dame_nn(damier, j, i, joueur, adversaire),
     ss = ScanCases_dame_ss(damier, j, i, joueur, adversaire), 
     so = ScanCases_dame_so(damier, j, i, joueur, adversaire), 
     ne = ScanCases_dame_ne(damier, j, i, joueur, adversaire);
   
    return maximum(ss,nn,so,ne);
}

int deplacer_dame(int **damier, Coord depart, Coord arrive, int valeur_dame){
	if(abs(depart.i - arrive.i) != abs(depart.j -  arrive.j))
		return 0;
	damier[arrive.i][arrive.j] = valeur_dame;
	damier[depart.i][depart.j]= 0;
	return 1;
}
