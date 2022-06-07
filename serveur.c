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
#include <signal.h>
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
	signal(SIGPIPE,  SIG_IGN);

	/*  Création d'une socket tcp ipv6 */
	int sock = socket(AF_INET6, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("socket");
		exit(2);
	}

	/*  Création de la sockaddr locale */
	struct sockaddr_in6 sa = { .sin6_family = AF_INET6,
		.sin6_port = htons(PORT_DAME),
		.sin6_addr = in6addr_any };

	/* Faire en sorte de pouvoir réutiliser l'adresse sans
	 * attendre après la fin du serveur. */
	int opt = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));

	/*  Attacher la socket d'écoute à l'adresse */
	if (bind(sock, (struct sockaddr *) &sa, sizeof(sa)) < 0) {
		perror("bind");
		exit(3);
	}

	/*  Écouter sur la socket d'écoute */
	if (listen(sock, 1) < 0) {
		perror("listen");
	}
	while(1){
		/*  Creation de la sockaddr pour le client */
		struct sockaddr_in6 addr_clt;
		socklen_t taille_addr = sizeof(struct sockaddr_in6);
		int sock_echange = sock_echange = accept(sock, (struct sockaddr *) &addr_clt, &taille_addr);
		if (sock_echange < 0) {
			perror("accept");
		}

		/*	Remplissage des 17 octets du tableau pour l'enrgistrement */
		uint8_t donnees[256], nb_octets_ecris = 0;
		*(donnees) = (uint8_t)0xFF; /*le premier octet FF*/
		memcpy(donnees + 1, addr_clt.sin6_addr.s6_addr, 16); /*ipv6 de l'adversaire*/
		nb_octets_ecris = 17;

		//DECLARATION DES VARIABLES 
		int joueur = 1, adversaire = 2; /*1 represente le pion du serveur et 2 pour l'adversaire*/
		int capturePion; /*represente le nombre de pion maximun qu'il faut capturer au cours du jeu*/
		Coord coord[51]; /*Avoir les coordonnées i,j de chaque case en connaissant sa notation manoury*/

		/*Allouer de l'espace pour le damier*/
		int **damier = malloc(sizeof(int *)*(TAILLECOTEDAMIER+3)); 
		for(int i = 0; i < TAILLECOTEDAMIER + 3; i++){
			damier[i] = malloc(sizeof(int)*(TAILLECOTEDAMIER+3));
		}
		/*Initialiser la damier */
		initialiserdamier (damier, TAILLECOTEDAMIER, coord);

		/*Avoir la notation manoury d'une en connaissant ses coordonnées i, j*/
		int tab_indice[TAILLECOTEDAMIER+1][TAILLECOTEDAMIER+1] = {0};
		for(int x = 1; x <= 50; x++){
			tab_indice[coord[x].i][coord[x].j] = x;
		}

		afficher(damier,TAILLECOTEDAMIER);
		int nombre_de_coup = 1;

		while(nombre_de_coup <= NB_COUP_MAX ){
			promu_dame(damier,TAILLECOTEDAMIER);
			/*Quand c'est le tour de l'adversaire */
			if(nombre_de_coup%2 == 0){

				/*Scan tout le damier et renvois le nombre max qu'il faut capturé*/
				capturePion = ScanToutDammier(damier,TAILLECOTEDAMIER,adversaire,joueur);
				char deplacement[256]; /*variable pour stocker le deplacement de l'adversaire*/
				read(sock_echange,deplacement,256);
				printf("l'adversaire a joué %s\n", deplacement);
				/*Dans le cas ou c'est juste un deplacement et non une capture*/
				if(capturePion == 0){
					/*Recuperer le deplacement sur deux entiers correspondant a la case de depart et arrivée*/
					int n_depart, n_arrive;
					Coord depart, arrive;
					char_to_int(deplacement,&n_depart,&n_arrive); 
					/*Verifier que le deplacement n'excede pas la damier, et on verifie le coup envoyer*/
					/*Dans le cas où le coup est invalide on l'informe à l'adversaire et on attends qu'il rejoue,
					  Si apres trois reprises il n'envoie que des coups invalide on interumpe la jeu pour eviter
					  une boucle infinie */
					if(n_depart > 0 && n_depart <= 50 && n_arrive > 0 && n_arrive <= 50){
						/*Convertir les entiers en notation manoury en des coordonnées i,j et verifier le deplacement*/
						recupere_indice(n_depart, n_arrive,&depart,&arrive, coord);
						int check;
						if(damier[depart.i][depart.j] == adversaire+2)
							check = deplacer_dame(damier,depart, arrive, adversaire+2);
						else
							check = verif_deplacement_possible(damier, depart, arrive, adversaire);
						if(check)
							deplacer(damier,depart,arrive,adversaire);
						else{
							message_erreur(sock_echange, "INTERUPTION");
							break;
						}
					}
					else{
						message_erreur(sock_echange, "INTERUPTION");
						break; /*si je reçois quoi que ça soit qui n'est pas un deplacement*/
					}
					/*Ecrire apres chaque deplacement de l'adversaire dans le tableau d'enregistrement */
					nb_octets_ecris = remplissage_octets_dep(donnees, nb_octets_ecris, n_depart, n_arrive);

				}else{
					/*Verifier que le deplacement n'excede pas la damier, et qu'il est correcte*/
					/*Dans le cas où le coup est invalide on stop le jeu, y'a pas de seconde chance (^_^) */
					if(verif_coup_max(deplacement) == capturePion){
						Coord coord_pion_capture[capturePion];/*Pour les coordonnées des pions capturés*/
						int tab[capturePion+1]; /*le tableau qui va contenir tout ses deplacement*/
						char_to_int_tab(tab,deplacement);
						int check = verif_capture_possible(damier,adversaire,joueur,capturePion,coord,coord_pion_capture,tab);
						if(check)
							capturer(damier,coord,coord_pion_capture,tab,capturePion,adversaire);
						else{
							message_erreur(sock_echange, "INTERUPTION");
							break;
						}
						nb_octets_ecris = remplissage_octets_capture(donnees, nb_octets_ecris, tab, capturePion+1);
					}else{
						message_erreur(sock_echange,"MANGE");
						break;
					}
				}
				vider_tempon(deplacement,strlen(deplacement));
			}
			else{ /*Quand c'est le tour du serveur de jouer */

				int tab_dep[20]={0}; /*Pour recuper tout les deplacement du serveur s'il y a des captures de pions*/
				capturePion = ScanToutDammier_hote(damier,TAILLECOTEDAMIER,joueur,adversaire,tab_indice,tab_dep);

				/*S'il n'y a pas de capture on cherche un deplacement et on se deplace, s'il n'y a plus de deplacement
				  on informe l'adversaire qu'on a perdu et on rapporte la partie 
				  Sinon on ecris le deplacement dans la socket et on enregistre le coup dans le tableau*/
				if (capturePion == 0 ){
					char dep[50];
					int check = cherche_case_jouable(damier, joueur, dep, tab_indice, coord);
					if(!check){
						message_erreur(sock_echange, "PERDU");
						break;
					}

					printf("\nj'ai joué:  %s\n", dep);
					exact_write(sock_echange, dep, strlen(dep)+1);
					int n_depart, n_arrive;
					char_to_int(dep, &n_depart, &n_arrive);
					nb_octets_ecris = remplissage_octets_dep(donnees, nb_octets_ecris, n_depart, n_arrive);
					vider_tempon(dep,50);
				} 
				else{
					Coord coord_pion_capture[capturePion];
					int check = verif_capture_possible(damier,joueur,adversaire,capturePion,coord,coord_pion_capture,tab_dep);
					if(check)
						capturer(damier,coord,coord_pion_capture,tab_dep,capturePion,joueur);
					else{
						message_erreur(sock_echange, "INTERUPTION"); /*On y arrive jamais normalement*/
						break;
					}
					char deplacement[50];
					tabInt_to_chaine(deplacement, tab_dep, capturePion+1);
					printf("j'ai joué : %s \n",deplacement);
					exact_write(sock_echange, deplacement, strlen(deplacement)+1);
					vider_tempon(deplacement,50);
					nb_octets_ecris = remplissage_octets_capture(donnees, nb_octets_ecris, tab_dep, capturePion+1);
				}
			}

			afficher(damier,TAILLECOTEDAMIER);
			nombre_de_coup ++;
			printf("Coup N° %d \n",nombre_de_coup);
			if(nombre_de_coup > NB_COUP_MAX){
				printf("(------ EGALITE ------)\n" );
			}
		}

		/*Liberer la memoire allouer */
		for(int i = 0; i < TAILLECOTEDAMIER + 3; i++)
			free(damier[i]);
		free(damier);
		close(sock_echange);

		enregistrement_de_la_partie(donnees, nb_octets_ecris+1);
	}

	close(sock);
	return 0;
}

