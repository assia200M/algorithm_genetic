# algorithm_genetic

.Ce projet implémente un algorithme génétique en C++ pour optimiser le placement des capteurs dans un espace donné. L'objectif est de maximiser la couverture des points d'intérêt en minimisant le nombre de capteurs utilisés.
.Les données de configuration sont stockées dans un fichier JSON contenant :
.locations : emplacements disponibles pour les capteurs.
.pois : points d'intérêt à couvrir.
.sensors : liste des capteurs et leur rayon de couverture.
.L'algorithme fonctionne en plusieurs étapes :
1.Lecture du fichier JSON et chargement des paramètres.
2.Génération de la population initiale.
3.Sélection des meilleurs individus selon un critère de fitness.
4.Croisement et mutation pour améliorer les solutions.
