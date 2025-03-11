#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <random>
#include<algorithm>
#include <iomanip>

using namespace std;

const int NB_CAPTEURS = 4;        // Nombre de capteurs
const int NB_EMPLACEMENTS = 10;   // Nombre d'emplacements
const int NB_POINTS_INTERET = 5;  // Nombre de points d'intérêt
const int TOTAL_PLACEMENTS = NB_CAPTEURS * NB_EMPLACEMENTS;
const int TAILLE_CHROMOSOME = NB_CAPTEURS * NB_EMPLACEMENTS;

struct Point {
    double x, y;
};

// Fonction pour générer des points avec des coordonnées aléatoires
vector<Point> generer_points(int nombre) {
    vector<Point> points(nombre);
    for (int i = 0; i < nombre; i++) {
        points[i] = {static_cast<double>(rand()) / RAND_MAX * 10, static_cast<double>(rand()) / RAND_MAX * 10};
    }
    return points;
}

// Générer la matrice de distance entre les capteurs placés et les points d'intérêt
vector<vector<double>> generer_matrice_distance(const vector<Point>& emplacements, const vector<Point>& points_interet) {
    vector<vector<double>> distances(TOTAL_PLACEMENTS, vector<double>(NB_POINTS_INTERET));
    
    for (int capteur = 0; capteur < NB_CAPTEURS; capteur++) {
        for (int emplacement = 0; emplacement < NB_EMPLACEMENTS; emplacement++) {
            int index = capteur * NB_EMPLACEMENTS + emplacement;

            for (int point = 0; point < NB_POINTS_INTERET; point++) {  
                distances[index][point] = sqrt(
                    pow(emplacements[emplacement].x - points_interet[point].x, 2) +
                    pow(emplacements[emplacement].y - points_interet[point].y, 2)
                );
            }
        }
    }
    return distances;
}

// Générer la matrice de couverture en fonction des distances et des rayons des capteurs
vector<vector<int>> generer_matrice_couverture(const vector<vector<double>>& distances, const vector<double>& rayons_capteurs) {
    vector<vector<int>> couverture(TOTAL_PLACEMENTS, vector<int>(NB_POINTS_INTERET, 0));
    
    for (int capteur = 0; capteur < NB_CAPTEURS; capteur++) {
        for (int emplacement = 0; emplacement < NB_EMPLACEMENTS; emplacement++) {
            int index = capteur * NB_EMPLACEMENTS + emplacement;

            for (int point = 0; point < NB_POINTS_INTERET; point++) {  
                if (distances[index][point] <= rayons_capteurs[capteur]) {  
                    couverture[index][point] = 1;
                }
            }
        }
    }
    return couverture;
}

// Générateur aléatoire
random_device rd;
mt19937 gen(rd());

// Fonction pour générer un entier aléatoire
int random_int(int min, int max) {
    uniform_int_distribution<> dis(min, max);
    return dis(gen);
}

// Fonction pour générer un individu valide
vector<int> generer_individu() {
    vector<int> individu(TAILLE_CHROMOSOME, 0);

    for (int capteur = 0; capteur < NB_CAPTEURS; capteur++) {
        int emplacement = random_int(0, NB_EMPLACEMENTS - 1);
        int index = capteur * NB_EMPLACEMENTS + emplacement;
        individu[index] = 1;
    }
    return individu;
}

// Fonction pour afficher la matrice de distance
void afficher_matrice_distance(const vector<vector<double>>& distances) {
    cout << "\nMatrice de Distance (Capteurs placés ↔ Points d'Intérêt) :\n";
    cout << "               ";
    for (int j = 0; j < NB_POINTS_INTERET; j++) {
        cout << setw(7) << "P" + to_string(j + 1);
    }
    cout << "\n";

    for (int capteur = 0; capteur < NB_CAPTEURS; capteur++) {
        for (int emplacement = 0; emplacement < NB_EMPLACEMENTS; emplacement++) {
            int index = capteur * NB_EMPLACEMENTS + emplacement;
            cout << "C" << capteur + 1 << "-E" << emplacement + 1 << " ";
            for (double d : distances[index]) {
                cout << setw(7) << fixed << setprecision(2) << d;
            }
            cout << "\n";
        }
    }
}

// Fonction pour afficher la matrice de couverture
void afficher_matrice_couverture(const vector<vector<int>>& matrice) {
    cout << "\nMatrice de Couverture :\n";
    cout << "               ";
    for (int j = 0; j < NB_POINTS_INTERET; j++) {
        cout << setw(5) << "P" + to_string(j + 1);
    }
    cout << "\n";

    for (int capteur = 0; capteur < NB_CAPTEURS; capteur++) {
        for (int emplacement = 0; emplacement < NB_EMPLACEMENTS; emplacement++) {
            int index = capteur * NB_EMPLACEMENTS + emplacement;
            cout << "C" << capteur + 1 << "-E" << emplacement + 1 << " ";
            for (int couverture : matrice[index]) {
                cout << setw(5) << couverture;
            }
            cout << "\n";
        }
    }
}
//

// Fonction d'évaluation (fitness) : maximiser la couverture tout en minimisant le nombre de capteurs
double fitness(const vector<int>& individu, const vector<vector<int>>& couverture) {
    vector<int> points_couverts(NB_POINTS_INTERET, 0);
    int capteurs_utilises = 0;

    for (int i = 0; i < TAILLE_CHROMOSOME; i++) {
        if (individu[i] == 1) {
            capteurs_utilises++;
            for (int j = 0; j < NB_POINTS_INTERET; j++) {
                points_couverts[j] |= couverture[i][j];
            }
        }
    }

    int total_couvert = 0;
    for (int c : points_couverts) {
        total_couvert += c;
    }

    return total_couvert - 0.1 * capteurs_utilises; // Pénalisation légère pour les capteurs en excès
}
//


// Fonction pour afficher la fitness de chaque individu
void afficher_fitness_population(const vector<vector<int>>& population, const vector<vector<int>>& couverture) {
    cout << "\nÉvaluation de la fitness :\n";
    for (size_t i = 0; i < population.size(); i++) {
        cout << "Individu " << i + 1 << " - Fitness : " << fitness(population[i], couverture) << endl;
    }
}
//
// Fonction pour afficher un individu
void afficher_individu(const vector<int>& individu) {
    for (int gene : individu) {
        cout << gene << " ";
    }
    cout << endl;
}

// Fonction pour afficher une population
void afficher_population(const vector<vector<int>>& population) {
    for (size_t i = 0; i < population.size(); i++) {
        cout << "Individu " << i + 1 << ": ";
        afficher_individu(population[i]);
    }
}

// Fonction pour initialiser une population aléatoire
vector<vector<int>> initialiser_population(int taille_population) {
    vector<vector<int>> population;
    for (int i = 0; i < taille_population; i++) {
        population.push_back(generer_individu());
    }
    return population;
}
//
// Sélection par tournoi biaisé avec taille et probabilité définies à l'exécution
vector<int> selection_par_tournoi_biaise(const vector<vector<int>>& population, const vector<double>& fitness_scores, int taille_tournoi, double p) {
    vector<int> indices_selectionnes;
    int taille_population = population.size();

    for (size_t i = 0; i < taille_population; i++) {
        vector<int> candidats;

        // Sélection aléatoire des candidats pour le tournoi
        for (int j = 0; j < taille_tournoi; j++) {
            int rand_index = rand() % taille_population;
            candidats.push_back(rand_index);
        }

        // Trier les candidats par fitness décroissante
        sort(candidats.begin(), candidats.end(), [&fitness_scores](int a, int b) {
            return fitness_scores[a] > fitness_scores[b];
        });

        // Sélection biaisée : probabilité p de choisir le meilleur
        if ((static_cast<double>(rand()) / RAND_MAX) < p) {
            indices_selectionnes.push_back(candidats[0]); // Meilleur individu
        } else {
            int rand_other = 1 + (rand() % (taille_tournoi - 1)); // Un autre candidat
            indices_selectionnes.push_back(candidats[rand_other]);
        }
    }

    return indices_selectionnes;
}

//
	
// Fonction pour effectuer un croisement à un point entre deux parents
pair<vector<int>, vector<int>> croisement(const vector<int>& parent1, const vector<int>& parent2, double probCroisement, int index1, int index2) {
    if ((double)rand() / RAND_MAX > probCroisement) {
        // Pas de croisement, on retourne les parents inchangés
        cout << " Pas de croisement entre individu " << index1 << " et individu " << index2 << endl;
        return {parent1, parent2};
    }

    int pointCroisement = rand() % TAILLE_CHROMOSOME;
    vector<int> enfant1 = parent1;
    vector<int> enfant2 = parent2;

    for (int i = pointCroisement; i < TAILLE_CHROMOSOME; i++) {
        swap(enfant1[i], enfant2[i]);
    }

    cout << "✅ Croisement entre individu " << index1 << " et individu " << index2 << " au point " << pointCroisement << endl;
    return {enfant1, enfant2};
}

// Fonction pour appliquer la mutation sur un individu
void mutation(vector<int>& individu, double probMutation, int index) {
    cout << " Mutation de l'individu " << index << " aux indices : ";
    bool mutationEffectuee = false;	
    
    for (int i = 0; i < TAILLE_CHROMOSOME; i++) {
        if ((double)rand() / RAND_MAX < probMutation) {
            individu[i] = 1 - individu[i]; // Inverse 0 ↔ 1
            cout << i << " "; // Afficher l'index muté
            mutationEffectuee = true;
        }
    }
    
    if (!mutationEffectuee) {
        cout << "aucune mutation";
    }
    
    cout << endl;
}
//
// Fonction pour afficher uniquement les indices activés d'un individu après l'affichage normal
void afficher_indices_actives(const vector<int>& individu) {
    cout << "Indices activés : ";
    for (size_t i = 0; i < individu.size(); i++) {
        if (individu[i] == 1) {
            cout << i << " ";
        }
    }
    cout << endl;
}

//

int main() {
    srand(time(0));

    // Générer les emplacements et les points d'intérêt
    vector<Point> emplacements = generer_points(NB_EMPLACEMENTS);
    vector<Point> points_interet = generer_points(NB_POINTS_INTERET);

    // Saisir les rayons des 4 capteurs
    vector<double> rayons_capteurs(NB_CAPTEURS);
    cout << "Entrez les rayons des " << NB_CAPTEURS << " capteurs :\n";
    for (int i = 0; i < NB_CAPTEURS; i++) {
        cout << "Rayon du capteur C" << i + 1 << " : ";
        cin >> rayons_capteurs[i];
    }

    // Générer et afficher la matrice de distance
    vector<vector<double>> distances = generer_matrice_distance(emplacements, points_interet);
    afficher_matrice_distance(distances);

    // Générer et afficher la matrice de couverture
    vector<vector<int>> couverture = generer_matrice_couverture(distances, rayons_capteurs);
    afficher_matrice_couverture(couverture);
    
    
    // Initialiser et afficher la population initiale
int taille_population;
cout << "\nEntrez la taille de la population : ";
cin >> taille_population;
vector<vector<int>> population = initialiser_population(taille_population);
cout << "\nPopulation Initiale :\n";
afficher_population(population);
     
    // Évaluer et afficher la fitness de chaque individu
    afficher_fitness_population(population, couverture);
//
int taille_tournoi;
double p_selection;

cout << "\nEntrez la taille du tournoi : ";
cin >> taille_tournoi;

cout << "Entrez la probabilité de sélectionner le meilleur (entre 0 et 1) : ";
cin >> p_selection;

// Calcul de la fitness de chaque individu
vector<double> fitness_scores;
for (const auto& individu : population) {
    fitness_scores.push_back(fitness(individu, couverture));
}

// Sélection des individus avec le tournoi biaisé
vector<int> selectionnes = selection_par_tournoi_biaise(population, fitness_scores, taille_tournoi, p_selection);


// Affichage des individus sélectionnés
cout << "\nIndividus sélectionnés :\n";
for (size_t i = 0; i < selectionnes.size(); i++) {
    cout << "Individu sélectionné " << i + 1 << " (Fitness: " << fixed << setprecision(2) << fitness_scores[selectionnes[i]] << "): ";
    afficher_individu(population[selectionnes[i]]);
}

//
// Entrée des probabilités
    double probCroisement, probMutation;
    cout << "Entrez la probabilité de croisement (entre 0 et 1) : ";
    cin >> probCroisement;
    cout << "Entrez la probabilité de mutation (entre 0 et 1) : ";
    cin >> probMutation;

    // Génération d'une population initiale
   
    cout << "\nPopulation initiale :" << endl;
    for (int i = 0; i < taille_population; i++) {
        population.push_back(generer_individu());
        cout << "Individu " << i + 1 << ": ";
        afficher_individu(population[i]);
    }

    // Croisement et mutation
    vector<vector<int>> nouvellePopulation;
    for (int i = 0; i < taille_population; i += 2) {
        if (i + 1 < taille_population) {
            auto [enfant1, enfant2] = croisement(population[i], population[i + 1], probCroisement, i + 1, i + 2);
            mutation(enfant1, probMutation, i + 1);
            mutation(enfant2, probMutation, i + 2);
            nouvellePopulation.push_back(enfant1);
            nouvellePopulation.push_back(enfant2);
        } else {
            nouvellePopulation.push_back(population[i]); // Si impair, le dernier reste inchangé
        }
    }

    // Affichage de la nouvelle population
    cout << "\nNouvelle population après croisement et mutation :" << endl;
    for (int i = 0; i < nouvellePopulation.size(); i++) {
        cout << "Individu " << i + 1 << ": ";
        afficher_individu(nouvellePopulation[i]);
    }
    //
    

    return 0;
}
