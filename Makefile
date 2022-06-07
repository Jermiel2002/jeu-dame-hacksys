all: serveur client

dame_fonction_commune.o: dame_fonction_commune.c dame_fonction_commune.h
	gcc -c dame_fonction_commune.c
dame_hote.o: dame_hote.c dame_hote.h
	gcc -c dame_hote.c
dame_adversaire.o: dame_adversaire.c dame_adversaire.h
	gcc -c dame_adversaire.c
client.o: client.c dame_adversaire.h dame_hote.h dame_fonction_commune.h 
	gcc -c client.c
serveur.o: serveur.c dame_adversaire.h dame_hote.h dame_fonction_commune.h 
	gcc -c serveur.c
serveur: serveur.o dame_adversaire.o dame_hote.o dame_fonction_commune.o 
	gcc -o serveur serveur.o dame_adversaire.o dame_hote.o dame_fonction_commune.o 
client: client.o dame_adversaire.o dame_hote.o dame_fonction_commune.o 
	gcc -o client client.o dame_adversaire.o dame_hote.o dame_fonction_commune.o 
clean:
	rm -f *.o serveur client
