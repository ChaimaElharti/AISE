# AISE
Projet d'implémentation d'un débugger

Il est possible de compiler les exemples de code aux multipes erreurs que l'on a créés dans les tests à l'aide du Makefile, mais il est aussi possible de débugger vos propres fichiers binaires à l'aide de la commande debug. 
Enfin, nous avons pensé qu'il pouvait aussi ếtre judicieux de mettre une commande qui permettrait de décrire précisément les problèmes liés au binaire (faire make sig pour effectuer un catchsegv). 

Le fichier dbg.c est constitué de : 
- fonctions renseignant le PID, le PPID, le GID, la localisation en mémoire. 
- Il retourne les fonctions du programme, le pointeur correspondant. 
- Il rend compte de l’état de sa mémoire, des registres, des bibliothèques chargées et des variables globales disponibles. 
- Il traque également les erreurs liées au code, qui sont notamment liées à des erreurs de segmentation ou encore de débordement de pile etc. à l'aide de la librairie ptrace. 

Par manque d'expérience et de temps, nous avons implémenté une fonction permettant de localiser les signaux d'erreur dans le code indépendante, avec la méthode des signaux. On ajoute également un flag de compilation -fsanitize=address, qui permet de trouver les adresses qui causent les erreurs et les fonctions qui posent problème. 

