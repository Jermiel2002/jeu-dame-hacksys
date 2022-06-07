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
/* spécifique à internet */
#include <arpa/inet.h> /* inet_pton */
#include "dame_fonction_commune.h"
#include "dame_adversaire.h"
#include "dame_hote.h"
#define TAILLECOTEDAMIER 10
#define NB_COUP_MAX 100 
#define PORT_DAME 5777

int main(int argc, char *argv[])
{

	/*  Création d'une socket tcp ipv6 */
	int sock = socket(AF_INET6, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("socket");
		exit(2);
	}

	/*  Création de la sockaddr locale */
	struct sockaddr_in6 sa = { .sin6_family = AF_INET6,
		.sin6_port = htons(PORT_DAME),
	};
	if (inet_pton(AF_INET6, argv[1], &sa.sin6_addr) != 1) {
		fprintf(stderr, "adresse ipv6 non valable\n");
		exit(1);
	}
	if (connect(sock, (struct sockaddr *)&sa, sizeof(struct sockaddr_in6)) < 0) {
		perror("connect");
		exit(3);
	}

	uint8_t donnees[256], nb_octets_ecris = 0;
	*(donnees) = (uint8_t)0x01;
	memcpy(donnees + 1, sa.sin6_addr.s6_addr, 16);
	nb_octets_ecris = 17;


	//DECLARATION DES VARIABLES 
	int joueur = 2, adversaire = 1;
	int capturePion;
	Coord coord[51];

	/*Allouer de l'espace pour le damier*/
	int **damier = malloc(sizeof(int *)*(TAILLECOTEDAMIER+3)); 
	for(int i = 0; i < TAILLECOTEDAMIER + 3; i++){
		damier[i] = malloc(sizeof(int)*(TAILLECOTEDAMIER+3));
	}
	/*Initialiser la dame */
	initialiserdamier (damier, TAILLECOTEDAMIER, coord);
	/*Avoir la notation manoury d'une en connaissant ses coordonnées i, j*/
	int tab_indice[TAILLECOTEDAMIER+1][TAILLECOTEDAMIER+1] = {0};
	for(int x = 1; x <= 50; x++){
		tab_indice[coord[x].i][coord[x].j] = x;
	}

	afficher(damier,TAILLECOTEDAMIER);
	int nombre_de_coup = 1;

	while(nombre_de_coup <= NB_COUP_MAX){
		promu_dame(damier,TAILLECOTEDAMIER);
		/*Quand c'est le tour de l'adversaire */
		if(nombre_de_coup%2 != 0){
			/*Scan tout le damier et renvois le nombre max qu'il faut capturé*/
			char deplacement[256]; /*variable pour stocker le deplacement de l'adversaire*/
			read(sock,deplacement,256);
			printf("l'adversaire a jouer %s\n", deplacement);
			capturePion = ScanToutDammier(damier,TAILLECOTEDAMIER,adversaire,joueur);
			/*Dans le cas ou c'est juste un deplacement et non une capture*/
			if(capturePion == 0){
				/*Recuperer le deplacement sur deux entiers correspondant a la case de depart et arrivée*/
				int n_depart, n_arrive;
				Coord depart, arrive;
				char_to_int(deplacement,&n_depart,&n_arrive); 
				/*Verifier que le deplacement n'excede pas la damier, et qu'il est correcte*/
				/*Dans le cas où le coup est invalide on stop le jeu, y'a pas de seconde chance (^_^) */
				if(n_depart > 0 && n_depart <= 50 && n_arrive > 0 && n_arrive <= 50){
					/*Convertir les entiers en notation manoury en des coordonnées i,j et verifier le deplacement*/
					recupere_indice(n_depart, n_arrive,&depart,&arrive, coord);
					int check;
					if(damier[depart.i][depart.j] == adversaire +2)
						check = deplacer_dame(damier,depart, arrive, adversaire+2);
					else
						check = verif_deplacement_possible(damier, depart, arrive, adversaire);
					if(check)
						deplacer(damier,depart,arrive,adversaire);
					else{
						printf("coup impossible !");
						message_erreur(sock, "INTERUPTION");
						break;
					}
				}
				else{
					printf("interuption du jeu\n");
					message_erreur(sock, "INTERUPTION");
					break;
				}
				/*Ecrire apres chaque deplacement de l'adversaire dans le tableau d'enregistrement */
				nb_octets_ecris = remplissage_octets_dep(donnees, nb_octets_ecris, n_depart, n_arrive);

			}else{
				/*Dans le cas où une capture de pions est possible, on verifie que l'adversaire capture le maximum possible
				 *On recupère tout ses deplacements dans un tableau d'entier et on verifie que son deplacement est correcte
				 *si tout est correcte alors on enlève les pions capturés du damier et on deplace son pion */
				/*Dans le cas contraire on informe l'adversaire que son coup est invalide*/
				if(verif_coup_max(deplacement) == capturePion){
					Coord coord_pion_capture[capturePion];/*Pour les coordonnées des pions capturés*/
					int tab[capturePion+1]; /*le tableau qui va contenir tout ses deplacement*/
					char_to_int_tab(tab,deplacement);
					int check = verif_capture_possible(damier,adversaire,joueur,capturePion,coord,coord_pion_capture,tab);
					if(check)
						capturer(damier,coord,coord_pion_capture,tab,capturePion,adversaire);
					else{
						message_erreur(sock, "INTERUPTION");
						break;
					}
					nb_octets_ecris = remplissage_octets_capture(donnees, nb_octets_ecris, tab, capturePion+1);
				}else{
					message_erreur(sock, "INTERUPTION");
					break;
				}
			}
			vider_tempon(deplacement,strlen(deplacement));
		}
		else{ /*Quand c'est le tour de notre bot de jouer */

			int tab_dep[20]={0}; /*Pour recuper tout les deplacement de notre bot s'il y a des captures de pions*/
			capturePion = ScanToutDammier_hote(damier,TAILLECOTEDAMIER,joueur,adversaire,tab_indice,tab_dep);

			/*S'il n'y a pas de capture on cherche un deplacement et on se deplace, s'il n'y a plus de deplacement
			  on informe l'adversaire qu'on a perdu et on rapporte la partie 
			  Sinon on ecris le deplacement dans la socket et on enregistre le coup dans le tableau*/
			if (capturePion == 0 ){
				char dep[20];
				int check = cherche_case_jouable(damier, joueur, dep, tab_indice, coord);
				if(!check){
					message_erreur(sock, "PERDU");
					break;
				}
				printf("\nj'ai joué  %s\n", dep);
				exact_write(sock, dep, strlen(dep)+1);
				int n_depart, n_arrive;
				char_to_int(dep, &n_depart, &n_arrive);
				nb_octets_ecris = remplissage_octets_dep(donnees, nb_octets_ecris, n_depart, n_arrive);
				vider_tempon(dep,20);
			} 
			else{
				Coord coord_pion_capture[capturePion];
				int check = verif_capture_possible(damier,joueur,adversaire,capturePion,coord,coord_pion_capture,tab_dep);
				if(check)
					capturer(damier,coord,coord_pion_capture,tab_dep,capturePion,joueur);
				else{
					message_erreur(sock, "INTERUPTION"); /*On y arrive jamais normalement*/
					break;
				}
				char deplacement[50];
				tabInt_to_chaine(deplacement, tab_dep, capturePion+1);
				printf("j'ai joué : %s \n",deplacement);
				exact_write(sock, deplacement, strlen(deplacement)+1);
				vider_tempon(deplacement,50);
				nb_octets_ecris = remplissage_octets_capture(donnees, nb_octets_ecris, tab_dep, capturePion+1);
			}
		}

		afficher(damier,TAILLECOTEDAMIER);
		nombre_de_coup ++;
		printf("Coup N° %d \n",nombre_de_coup);
		if(nombre_de_coup > NB_COUP_MAX){
			printf("(------ EGALITE ------) \n" );
			message_erreur(sock, "EGALITE");
		}
	}

	/*Liberer la memoire allouer */
	for(int i = 0; i < TAILLECOTEDAMIER + 3; i++)
		free(damier[i]);
	free(damier);
	close(sock);	

	enregistrement_de_la_partie(donnees, nb_octets_ecris+1);

	return 0;
}

