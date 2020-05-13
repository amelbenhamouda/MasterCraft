MasterCraft
=======

Projet du jeu MasterCraft programmé en C++ à l'aide de la librairie OpenGL.

Installation
============

## Compilation tester
    * g++ 6.3.0
    * OpenGL Version : 3.0 Mesa 13.0.6
    * GLEW Version : 2.0.0
	
	* Mingw32 GNU 8.1.0
	* OpenGL Version : 4.6.0 NVIDIA 442.19
	* GLEW Version : 1.5.7


Utilisation
===========

## Exécutable
    * cmake .. dans le dossier build sous Linux.
    * ./Mastercraft pour lancer l'exécutable.
	* Avec carte graphique renommé le dossier shader300 en shader pour utilisé la #version 330 core.
	* Sans carte graphique utilisé les shaders avec la #version 300 es avec precision mediump float.


## Commandes du jeu
	* z, q, s, d pour le mouvement de la caméra.
	* r pour détruire des blocks.
	* t pout ajouter des blocks.
	* e pour passer du mode nuit à jour.
	* F5 pour passer du mode Freefly aux déplacement sur la carte.
	* ctrl pour prendre de la vitesse.
	* f pour allumer/eteindre la lampe torche la nuit.
	* echap pour fermer l'appplication.
	
## Choix de la carte en ligne de commande
	* version 128x128x128 en rentrant 1 dans le terminal
	* version 512x512x128 en rentrant 2 dans le terminal -> plus longue à charger et demande plus de RAM.

## Travail effectué
	* Lecture de deux images heigthmap et colormap
	* Affichage du terrain avec au minimum 3 terrain
	* Déplacement de l'observateur virtuel avec changement Freefly/attaché à la map avec "F5"
	* Affichage de décors avec au moins 1 type de décors : 1 arbre / 1 soleil / 1 lune
	* Affichage d'un personnage non joueur + animation
	* Affichage de la skybox ( utilisation d'une cubemap avec desactivation du depth Buffer)
	* Ambiance lumineuse différente nuit/jour
	* Impossibilité de sortir du monde en mode attaché à la map

## Bonus effectué
	* types supplémentaires de terrain: ajout d'eau et animation de celle-ci
	* Animation de la lumière directionnelle avec rotation + changement état nuit/jour
	* Placement d'une lumière attaché à l'observateur virtuel de nuit (Lampe torche en appuyant sur "f" la nuit)
	* Test d'implémentation d'ajout("t") et de destruction("r") de terrain mais pas 100% fonctionnelle

## Bug / Amélioration possible
	* La partie ajout/destruction de block ne marche pas comme souhaité:
		- Sur Windows avec OpengGL, carte graphique NVIDIA 442.19 et compiler GNU 8.1.0:
			* Les données pointées à la souris sont correct
			* La destruction de certain block est mal effectuées
		
		- Sur Linux sans carte graphique avec OpenGL es:
			* Les données pointées  à la souris ne sont pas les bonnes
			* La destruction/Construction ne se fait pas comme souhaité ou comme sur la version windows
			
	* Déplacement du personnage:
		- le déplacement est fait par rapport à la heigthmap,
		- cela pose problème lorsque l'on souhaite ajouté la partie ajout/destruction de block,
		- il faudrait effectué les déplacements par rapport au chunk et à la map et non par rapport à la heigthmap.
     
	* Charger et décharger les chunks de maniére dynamique
		- actuellement toute la carte est chargée en brut dans la mémoire, ce qui contribut à une limitation du programme.

Notes
=====

## Authors
    * BEN HAMOUDA Amel (Université Paris-Est Marne-la-Vallee, France)
    * DURAND Aurélien (Université Paris-Est Marne-la-Vallee, France)

