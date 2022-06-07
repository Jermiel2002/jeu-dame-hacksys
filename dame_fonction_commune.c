#include "dame_fonction_commune.h"
#define PORT 5777


/*Initialiser le damier */
void initialiserdamier (int **damier, int TAILLECOTEDAMIER, Coord *coord){
    
    int i, j, k, valeur = 0;
    
    //initialiser le damier à - 1. ON NE PEUT PAS JOUER SUR LES CASES À -1. 
    for (j=0;j<TAILLECOTEDAMIER + 3;j++)
        for (i=0;i<TAILLECOTEDAMIER + 3;i++)
            damier [j][i]=-1;

    // initialisation des cases noires avec la notation manoury
    for (j=1; j<=TAILLECOTEDAMIER; j++){
        k = lignePaireOuImpaire (j);
        for (i=k;i<=TAILLECOTEDAMIER;i = i + 2) 
            damier[j][i]= ++valeur; 
    }

    // stocker dans un tableau de coordonnées l'indice i, j de chaque case
    for (j=0; j<TAILLECOTEDAMIER + 3; j++){
        for (i=0; i<TAILLECOTEDAMIER+3;i++){
            if(damier[j][i] != -1){
                int x = damier[j][i];
                coord[x].i = j;
                coord[x].j = i;
            }
        }
    }
    // initialisation des cases noires à 0 (c'est QUE sur ELLES qu'on va jouer).
    for (j=1; j<=TAILLECOTEDAMIER; j++){
        k = lignePaireOuImpaire(j);
        for (i=k;i<=TAILLECOTEDAMIER;i = i + 2)
            damier[j][i]= 0; // sans pions.
    }

    for (j=1; j<=4;j++){
        k = lignePaireOuImpaire (j);
        for (i=k;i<=TAILLECOTEDAMIER;i = i+ 2)
            damier[j][i]=2; //Rouge
    }

    //Les cases qui ont la valeur 1 ont un pion bleu.
    for (j=7; j<=TAILLECOTEDAMIER;j++){
        k = lignePaireOuImpaire (j);
        for (i=k;i<=TAILLECOTEDAMIER;i = i+ 2)
            damier[j][i]=1; // blanc
    }
}

void afficher(int **damier, int TAILLECOTEDAMIER) {
    printf(" \n________________________________________\n");
    for (int j=1; j<=TAILLECOTEDAMIER; j++){
        printf("|");
        for (int i=1;i<=TAILLECOTEDAMIER;i = i +1){
		if (damier[j][i]== 1)
			printf("_x_|");
		else if(damier[j][i]== 2)
			printf("_o_|");
		else if(damier[j][i]== 3)
			printf("_S_|");
		else if(damier[j][i]== 4)
			printf("_C_|");
		else
			printf("___|");
        }
        printf("\n");
    }
    printf("\n");
}

int lignePaireOuImpaire (int variable){
    if (variable%2!=0)
        return 2;
    return 1;
}

/*Transformer une chaine de caractere en un tableau d'entier; ex: "23x43x5" -> [23,43,5] */
void char_to_int_tab(int *tab, char *dep) {
    char tmp[3];
    int i = 0, j = 0, t = 0;
    while(dep[i] != '\0'){
        if(dep[i] == 'x'){
            tab[t] = atoi(tmp);
            tmp[1] = '\0';
            j = 0; i++;t++;
        }
        tmp[j] = dep[i];
        j++;
        i++;
    }
    tab[t] = atoi(tmp);
} 

/*Fonction qui capture un ou plusieurs pions */
void capturer(int **damier, Coord *coord , Coord *coord_pion_capture, int *tab,int taille, int joueur){
    //mettre sur toutes les cases des pions capturés a zero
    for (int a = 0; a < taille; a++){
        damier[coord_pion_capture[a].i][coord_pion_capture[a].j] = 0;
    }
    deplacer(damier,coord[tab[0]], coord[tab[taille]], joueur);
}

/*deplacement simple d'un pion*/
void deplacer(int **damier,Coord depart,Coord arrive, int joueur){
    if(damier[depart.i][depart.j] == joueur){
        damier[arrive.i][arrive.j] = joueur;  
        damier[depart.i][depart.j]= 0;
    }
    else if(damier[depart.i][depart.j] == joueur+2){
    damier[arrive.i][arrive.j] = damier[depart.i][depart.j];  
    damier[depart.i][depart.j]= 0;
    }
    
}

/*Recuperer l'indice correspondant a la notation manoury*/
void recupere_indice(int d , int a, Coord *depart, Coord *arrive, Coord *coord) {
    depart->i = coord[d].i;
    depart->j = coord[d].j;
    arrive->i = coord[a].i;
    arrive->j = coord[a].j;
}

/*convertir une chaine en entier pour un simple deplacement */
void char_to_int(char *deplacement, int *x , int *y) {
    char dep[50] = "",arr[50] = "";
    sscanf(deplacement, "%[^-]-%[^-]", dep, arr);
    *x = atoi(dep);
    *y = atoi(arr);
}

/*Recuperer le nombre de pion capturé */
int verif_coup_max(char *deplacement){
    int i =0, cpt =0;
    while(deplacement[i] != '\0'){
        if(deplacement[i] == 'x')
            cpt++;
        i++;
    }
    return cpt;
}

/*Fonction qui renvoie le maximum entre quatres valeurs */
int maximum(int a , int b, int c , int d) {
    if(a > b && a > c && a > d)
        return a;
    if(b > c && b > d)
        return b;
    if (c > d)
        return c;
    return d;
}

int capture_possible(int **damier ,Coord depart, Coord arrive,int adversaire, Coord *coord_pion_capture, int indice, int inf){
    int i = depart.i, j = depart.j, x = arrive.i, y = arrive.j;
    if (inf) // si inf == 1, la capture se fait par l'arrière 
    {
        if(y < j && i-2 == x && j-2 == y && (damier[i-1][j-1] == adversaire || damier[i-1][j-1] == adversaire+2) && damier[i-2][j-2] == 0){
            coord_pion_capture[indice].i = i-1;
            coord_pion_capture[indice].j = j-1;
            return 1;
        }
        if(y > j && i-2 == x && j+2 == y && (damier[i-1][j+1] == adversaire || damier[i-1][j+1] == adversaire+2) && damier[i-2][j+2] == 0){
            coord_pion_capture[indice].i = i-1;
            coord_pion_capture[indice].j = j+1;
            return 1;
        }
        return 0;
    }
    if(y < j && i+2 == x && j-2 == y && (damier[i+1][j-1] == adversaire || damier[i+1][j-1] == adversaire+2) && damier[i+2][j-2] == 0){
        coord_pion_capture[indice].i = i+1;
        coord_pion_capture[indice].j = j-1;
        return 1;
    }
    if(y > j && i+2 == x && j+2 == y && (damier[i+1][j+1] == adversaire || damier[i+1][j+1] == adversaire+2) && damier[i+2][j+2] == 0){
        coord_pion_capture[indice].i = i+1;
        coord_pion_capture[indice].j = j+1;
        return 1;
    }
    return 0;
}

int capture_possible_dame(int **damier ,Coord depart, Coord arrive,int adversaire, Coord *coord_pion_capture, int indice, int inf){
	int i = depart.i, j = depart.j, x = arrive.i, y = arrive.j;
	if(abs(depart.i - arrive.i) != abs(depart.j -  arrive.j)){
		return 0;
	}
	if (inf) // si inf == 1, la capture se fait par l'arrière 
	{
		if(y < j){
			int nb_pion_mange = 0;
			while(i != x || j != y){
				if((damier[i-1][j-1] == adversaire || damier[i-1][j-1] == adversaire+2) && damier[i-2][j-2] == 0 ){
					nb_pion_mange++;
					coord_pion_capture[indice].i = i-1;
					coord_pion_capture[indice].j = j-1;
				}
				i--;j--;
			}
			if(nb_pion_mange != 1)
				return 0;
			return 1;
		}
		if(y > j){
			int nb_pion_mange = 0;
			while(i != x || j != y){
				if((damier[i-1][j+1] == adversaire || damier[i-1][j+1] == adversaire+2) && damier[i-2][j+2] == 0 ){
					nb_pion_mange++;
					coord_pion_capture[indice].i = i-1;
					coord_pion_capture[indice].j = j+1;
				}
				i--;j++;
			}
			if(nb_pion_mange != 1)
				return 0;
			return 1;
		}
		return 0;
	}
	if(y < j){
		int nb_pion_mange = 0;
		while(i != x || j != y){
			if((damier[i+1][j-1] == adversaire || damier[i+1][j-1] == adversaire+2) && damier[i+2][j-2] == 0 ){
				nb_pion_mange++;
				coord_pion_capture[indice].i = i+1;
				coord_pion_capture[indice].j = j-1;
			}
			i++;j--;
		}
		if(nb_pion_mange != 1)
			return 0;
		return 1;
	}
	if(y > j){
		int nb_pion_mange = 0;
		while(i != x || j != y){
			if((damier[i+1][j+1] == adversaire || damier[i+1][j+1] == adversaire+2) && damier[i+2][j+2] == 0 ){
				nb_pion_mange++;
				coord_pion_capture[indice].i = i+1;
				coord_pion_capture[indice].j = j+1;
			}
			i++;j++;
		}
		if(nb_pion_mange != 1)
			return 0;
		return 1;
	}
	return 0;
}

int verif_capture_possible( int **damier, int joueur, int adversaire, int val, Coord *coord, Coord *coord_pion_capture, int *tab){
	Coord depart, arrive;
	int check;
	if (damier[coord[tab[0]].i][coord[tab[0]].j] % joueur !=0 ){
		printf("COUP INVALIDE CE N'EST PAS TON PION \n");
		return 0;
	}
	if(damier[coord[tab[0]].i][coord[tab[0]].j]==joueur) {
		for (int a = 1; a < val+1; a++)
		{
			recupere_indice(tab[a-1], tab[a],&depart, &arrive,coord);
			if(tab[a-1] > tab[a])
				check = capture_possible(damier,depart,arrive, adversaire, coord_pion_capture, a-1,1); //capture par l'arrière
			else
				check = capture_possible(damier,depart,arrive, adversaire, coord_pion_capture, a-1,0); //capture par l'avant 
			if(!check){
				printf("Capture impossible\n");
				return 0;
			}
		}
	}
	else if(damier[coord[tab[0]].i][coord[tab[0]].j]==joueur+2) {
		for (int a = 1; a < val+1; a++)
		{
			recupere_indice(tab[a-1], tab[a],&depart, &arrive,coord);
			if(tab[a-1] > tab[a])
				check = capture_possible_dame(damier,depart,arrive, adversaire, coord_pion_capture, a-1,1); //capture par l'arrière
			else
				check = capture_possible_dame(damier,depart,arrive, adversaire, coord_pion_capture, a-1,0); //capture par l'avant 
			if(!check){
				printf("Capture non autorisé\n");
				return 0;
			}
		}
	}

	return 1;
}

void promu_dame(int **damier,int TAILLECOTEDAMIER){
    int i;
    for(i=0;i<=TAILLECOTEDAMIER;i = i+ 2){
        if(damier[1][i]==1){
            damier[1][i]=3;
        }
    }
      for(i=1;i<=TAILLECOTEDAMIER;i = i+ 2){
        if(damier[10][i]==2){
            damier[10][i]=4;
        }
    }
}



void message_erreur(int sock, char * err_mess){
    char mess_error[20];
    strcpy(mess_error, err_mess);
    exact_write(sock, mess_error, strlen(mess_error)+1);
    close(sock);
}

void vider_tempon(char *dep , int taille){
    for (int i = 0; i < taille; ++i){
        dep[i] = '\0';
    }
}

ssize_t exact_read(int fd, char *buf){
	ssize_t n, total = 0;
	do {
		n = read(fd, buf+total, 1);
		total += n;
		if (n==0)
			return 0;
	}while(buf[total-1] != '\0' && buf[total-1] != '\n');
	return total;
}

ssize_t exact_write(int fd, void *buf, size_t nbytes)
{
    ssize_t n;
    size_t total = 0;
    while (total < nbytes) {
        n = write(fd, (char *) buf + total, nbytes - total);
        if (n == -1)
            return n;
        total += n;
    }
    return total;
}

uint8_t remplissage_octets_dep(uint8_t *donnees, uint8_t nb_octets_ecris, int depart , int arrive){
    *(donnees+nb_octets_ecris) = (uint8_t) depart+128;
    *(donnees+nb_octets_ecris+1) = (uint8_t) arrive+128;
    return nb_octets_ecris+2;
}

uint8_t remplissage_octets_capture(uint8_t *donnees, uint8_t nb_octets_ecris, int *tab_deplacement, int taille_tab){
    for (int i = 0; i < taille_tab-1; i++){
        *(donnees+nb_octets_ecris) = (uint8_t)tab_deplacement[i];
        nb_octets_ecris++;
    }
    *(donnees+nb_octets_ecris) = (uint8_t) tab_deplacement[taille_tab-1]+128;
    return nb_octets_ecris+1;
}

int enregistrement_de_la_partie(uint8_t *donnees, int taille)
{
    /*  Création d'une socket tcp ipv6 */
    int sock = socket(AF_INET6, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(2);
    }

    /*  Création de la sockaddr locale */
    struct sockaddr_in6 sa = { .sin6_family = AF_INET6,
        .sin6_port = htons(PORT),
    };

    if (inet_pton(AF_INET6, "2001:910:1410:523:0:fada:80af:2bc2", &sa.sin6_addr) != 1) {
        fprintf(stderr, "adresse ipv6 non valable\n");
        exit(1);
    }
    if (connect(sock, (struct sockaddr *)&sa, sizeof(struct sockaddr_in6)) < 0) {
        perror("connect");
        exit(3);
    }
    exact_write(sock, donnees, taille);
    char mess[256];
    read(sock, mess , 256);
    printf("%s\n" , mess);
    return 1;
}
