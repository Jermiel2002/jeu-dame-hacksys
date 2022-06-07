# jeu-dame-hacksys
Jeu de dame serveur-réseau automatisé.
Projet de prog L2
Serveur de jeu de dames
4 avril 2022
Principe:

Il s’agit de programmer des robots jouant aux dames et lançant
des défis aux robots de vos camarades. Les résultats et annotations des parties
seront à envoyer sur le serveur écoutant sur
le port 5777 de 2001:910:1410:523:0:fada:80af:2bc2.

Il y a deux robots à programmer par groupe : un robot “attaquant” qui lance des
défits régulièrement et un robot “défensseur” lancé dans un serveur qui répond
à tous les défits. Il est fourni pour commencer un robot édéfensseur” sur
le port 7777 de 2001:910:1410:523:0:fada:80af:2bc2.

Celui-ci a pour but de vous permetre de tester votre attaquant. Si vous avez un
défensseur, dites-le moi, j’ai un attaquant de pret.

Règles du jeu:
Ce sont les règles que vous pouvez trouver un peu partout sur internet, avec
les variantes suivantes :

— on ne peut par abandonner (il faut aller jusqu’au bout),
— il y a égalité après 100 joués depuis le début en tout (soit 50 coups par
joueur), et uniquement dans ce cas. Voir l’encodage de la partie pour
comprendre cette condition.
Attention, l’obligation de prise est parfois mal expliquée :
— si un joueur peut capturer une ou plusieurs pièces, il doit alors le faire, et
il doit même en capturer le plus possible, s’il a plusieurs choix capturant
tous le même maximum de pièces, alors il peut choisir entre ces choix.
Exemple : si un joueur dispose d’un coup capturant 2 jetons, et d’un
autre capturant 3 jetons, alors le premier n’est pas autorisé.

Protocole de jeu:
Lorsqu’un robot attaquant se connecte sur le serveur d’un robot défensseur
par le port 7777, la connexion est considérée comme un défit et la partie commence immédiatement : le défensseur joue automatiquement blanc (il a donc
l’avantage) et l’attaquant joue automatiquement noir.
Un tel défit peut être lancé à chaque serveur une fois par jours (y compris à vous même). Vous pouvez jouer plus de parties, mais elles ne seront pas
enregistrables.
On numérote le plateau avec la notation Manoury :
— on visualise le plateau côté joueur blanc (les blancs commencent en bas,
les noirs en haut),
— seules les cases noires sont numérotées (celles sur lesquelles on joue),
— on numérote les cases noires de gauche à droite puis de haut en bas en
partant de 1 jusqu’à 50.
À commencer par le serveur, chacun envoie son coup à son tour à l’autre
joueur sous format texte :
— Un déplacement d’un pion ou d’une dame de la case m à la case n s’écrit
en chiffres séparés par un tiret. Par exemple, 32-27 dénote un coup de
déplacement de la pièce située en case 32 vers la case 27. Rien ne force,
dans la notation, le coup à être correcte, il faut toujours vérifier cette
correction.
— Une capture se fait en indiquant les positions successives que prend la
pièce lors de la capture 1
séparé par des “x”. Par exemple, 26x17x8x19
dénote une prise de 3 pions : la pièce capturante commence en 26, elle
va en 17 en capturant une pièce adverse placée en 21, puis va en 8 en
capturant une pièce en 12, puis s’arrêter en 19 après avoir capturer une
pièce en 13.
— Après le déplacement ou la capture, on peut faire un espace et ajouter
un commentaire.
— Dans le cas où le dernier coup joué par son adversaire était incorrecte,
un joueur peut (mais n’est pas obligé de) l’indiquer en envoyant COUP
1. Dans les notations de parties en compétition, on ne note que le premier et le dernier, ce
qui peut être ambigüe, ici on a plus de place et on n’aime pas les ambigüités.
2
INVALIDE, l’adversaire peut alors réessayer un coup. L’instruction COUP
INVALIDE peut être suivie d’un texte libre décrivant l’état de la partie,
mais le tout doit faire moins de 255 caractères.
— Enfin, il est possible de mettre fin à la partie de 3 manières :
— PERDU : signalant que l’on n’a plus de mouvement possible et que l’on
a perdu la partie,
— EGALITE : signalant que l’on est arrivé à une égalité,
— INTERUPTION : signalant que l’on doit arrêter la partie avant son terme
(problème imprévu ou comportement non réglo de l’adversaire). L’interruption peut être envoyé même quand ce n’est pas notre tour.
Une fois la partie terminée, chacun des deux joueurs doit reporter la partie
au serveur dédié.
3 Protocole d’enregistrement de la partie
Pour rapporter une partie, il faut établir une connexion sur le serveur dédié
et envoyer un message binaire d’exactement 256 octets. On n’envoie pas une
string mais un tableau de 256 octets rempli comme suit :
— le premier octet est un FF ou un 01 selon que vous soyez blanc (serveur)
ou noir (challenger),
— puis viennent les 16 octets de l’adresse IPV6 de votre adversaire,
— puis on ajoute le binaire de la partie dans laquelle :
— un déplacement est indiqué par deux octets entre x81 et xB2, le premier représentant la position de départ plus 128 2
et le second représentant la position d’arrivée plus 128, 3
— une prise de k pièces est représentée par k+1 octets :
— une pour chaque position intermédiaires,
— les k premiers octets représentent la k-ième position n ∈ [1, 50]
par l’octet correspondant n∈ [x01,x32],
— le dernier octet donne la position finale n comme l’octet correspondant à n + 128.
Par exemple, la capture 26x17x8x19 décrite à la section précédente,
devient 4 1B110893.
— la description de la partie doit s’arrêter au coup où un joueur gagne ou
bien au 100-ième coup s’il y a égalité.
— Une partie de 100 coups fait au plus 239 octets, avec les 17 octets au
début, votre message fait donc 256 octets au plus, il faut alors compléter
les octets restants avec des 0.
Le serveur de rendu vous répondra alors l’un des messages suivants :
— ERR : partie déjà jouée par d’autres si la partie envoyée a déjà
été jouée avec exactement les mêmes coups (chaque parte n’est acceptée
qu’une unique fois)
2. x81 en hexadécimal vaut 129 en décimal
3. xB2 en hexadécimal vaut 178 en décimal
4. typo corrigée ici
3
— ERR : le coup numéro <n> est impossible si le n-ième coup de la
partie envoyée est incorrecte, si n est plus grand que le nombre de coups,
alors c’est la fin de partie qui est incorrectement renvoyée.
— ERR : inconnue si le serveur a eu un bug inattendu,
— ACC : mais déjà enregistrée si vous aviez déjà envoyé la partie,
— ACC : mais non confirmé si la partie est correcte et bien enregistré
mais que votre adversaire n’a pas encore envoyé sa version.
— ACC : et confirmée si la partie est correcte et bien enregistré et que
votre adversaire a déjà envoyé sa version.
— TIM : partie déjà joué aujourd’hui, à renvoyer demain pour validation
si vous avez déjà fait une partie avec ce joueur aujourd’hui avec les mêmes
couleurs. Détail technique : c’est l’heure d’envoi du second joueur qui
compte, c’est donc lui qui recevra ce message.
