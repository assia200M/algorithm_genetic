#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <random>
#include <iomanip>
#include <fstream>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

// Variables globales dynamiques
int NB_CAPTEURS;
int NB_EMPLACEMENTS;
int NB_POINTS_INTERET;
int TAILLE_CHROMOSOME;
int TAILLE_TOURNOI=3;
int TAILLE_COUVERTURE;

// Lecture du fichier JSON
json lire_json(const string& filename){
    ifstream fichier(filename);
    json data;
    fichier>>data;
    return data;
}

// Initialisation des paramètres depuis le JSON
void initialiser_parametres(const json& data){
    string key=data.begin().key(); // Prend la première clé du JSON
    NB_EMPLACEMENTS=data[key]["locations"].size();
    NB_POINTS_INTERET=data[key]["pois"].size();
    NB_CAPTEURS=data[key]["sensors"].size();
    TAILLE_CHROMOSOME=NB_CAPTEURS*NB_EMPLACEMENTS;
    TAILLE_COUVERTURE=NB_EMPLACEMENTS*NB_CAPTEURS;
}

// Génération d'une matrice de couverture aléatoire
vector<vector<int>> generer_matrice_couverture(){
    vector<vector<int>> couverture(TAILLE_COUVERTURE,vector<int>(NB_POINTS_INTERET));
    for(int i=0;i<TAILLE_COUVERTURE;i++){
        for(int j=0;j<NB_POINTS_INTERET;j++){
            couverture[i][j]=rand()%2;
        }
    }
    return couverture;
}

// Affichage de la matrice de couverture
void afficher_matrice_couverture(const vector<vector<int>>& matrice){
    cout<<"\nMatrice de couverture:\n";
    cout<<"         ";
    for(int j=0;j<NB_POINTS_INTERET;j++){
        cout<<setw(5)<<"P"+to_string(j+1);
    }
    cout<<"\n";
    for(int i=0;i<matrice.size();i++){
        cout<<"C"<<(i/NB_EMPLACEMENTS)+1<<"-E"<<(i%NB_EMPLACEMENTS)+1<<" ";
        for(int j=0;j<matrice[i].size();j++){
            cout<<matrice[i][j];
        }
        cout<<"\n";
    }
}

// Génération d'un individu
vector<int> generer_individu(){
    vector<int> individu(TAILLE_CHROMOSOME,0);
    
    for(int capteur=0;capteur<NB_CAPTEURS;capteur++){
        int position=rand()%NB_EMPLACEMENTS;
        individu[capteur*NB_EMPLACEMENTS+position]=1;
    }
    return individu;
}

// Calcul de la fitness d'un individu
int calculer_fitness(const vector<int>& individu){
    return count(individu.begin(),individu.end(),1);
}

// Sélection par tournoi
vector<vector<int>> selection_par_tournoi(const vector<vector<int>>& population,int taille_selection){
    vector<vector<int>> nouvelle_population;
    for(int i=0;i<taille_selection;i++){
        vector<int> meilleur_individu;
        int meilleure_fitness=-1;
        for(int j=0;j<TAILLE_TOURNOI;j++){
            int index=rand()%population.size();
            const vector<int>& candidat=population[index];
            int fitness=calculer_fitness(candidat);
            if(fitness>meilleure_fitness){
                meilleure_fitness=fitness;
                meilleur_individu=candidat;
            }
        }
        nouvelle_population.push_back(meilleur_individu);
    }
    return nouvelle_population;
}

// Initialisation de la population
vector<vector<int>> initialiser_population(int taille_population){
    vector<vector<int>> population;
    for(int i=0;i<taille_population;i++){
        population.push_back(generer_individu());
    }
    return population;
}
//
// Croisement à un point
vector<int> croisement(const vector<int>& parent1, const vector<int>& parent2, int index) {
    int point = rand() % TAILLE_CHROMOSOME;
    vector<int> enfant(TAILLE_CHROMOSOME);
    copy(parent1.begin(), parent1.begin() + point, enfant.begin());
    copy(parent2.begin() + point, parent2.end(), enfant.begin() + point);
    cout << "Croisement entre Individu " << index * 2 + 1 << " et Individu " << index * 2 + 2 << " => Enfant " << index + 1 << endl;
    return enfant;
}

// Mutation d'un individu
void mutation(vector<int>& individu, int index) {
    int mutation_point = rand() % TAILLE_CHROMOSOME;
    individu[mutation_point] = 1 - individu[mutation_point];
    cout << "Mutation appliquée sur Enfant " << index + 1 << " au point " << mutation_point << endl;
}
//

// Affichage de la population
void afficher_population(const vector<vector<int>>& population){
    for(int i=0;i<population.size();i++){
        cout<<"Individu "<<i+1<<" : ";
        for(int j=0;j<population[i].size();j++){
            cout<<population[i][j];
        }
        cout<<" | Fitness: "<<calculer_fitness(population[i]);
        cout<<endl;
    }
}
// Affichage d'un individu
void afficher_individu(const vector<int>& individu, int index) {
     cout << "Individu " << index + 1 << " : ";
     for (int gene : individu) {
        cout << gene;
    }
    cout << " | Fitness: " << calculer_fitness(individu) << endl;
}

int main(){
    srand(time(0));

    // Charger les données JSON
    json data = lire_json("data.json");

    // Vérifier si le fichier JSON est vide
    if (data.empty()) {
        cerr << "Erreur : Le fichier JSON est vide ou mal formé !" << endl;
        exit(EXIT_FAILURE);
    }

    // Afficher la liste des images disponibles
    cout << "Images disponibles dans le JSON :\n";
    for (auto& [image, _] : data.items()) {
        cout << "- " << image << endl;
    }

    // Demander à l'utilisateur quelle image traiter
    string image_choisie;
    cout << "\nEntrez le nom de l'image à traiter (ex: 1.bmp) : ";
    cin >> image_choisie;

    // Vérifier si l’image existe dans le JSON
    if (data.find(image_choisie) == data.end()) {
        cerr << "Erreur : L'image '" << image_choisie << "' n'existe pas dans data.json !" << endl;
        return 1;
    }

    cout << "\n==============================" << endl;
    cout << "Traitement de l'image : " << image_choisie << endl;
    cout << "==============================\n" << endl;

    // Récupérer les paramètres de l’image choisie
    json valeurs = data[image_choisie];
    NB_EMPLACEMENTS = valeurs["locations"].size();
    NB_POINTS_INTERET = valeurs["pois"].size();
    NB_CAPTEURS = valeurs["sensors"].size();
    TAILLE_CHROMOSOME = NB_CAPTEURS * NB_EMPLACEMENTS;
    TAILLE_COUVERTURE = NB_EMPLACEMENTS * NB_CAPTEURS;

    // Générer et afficher la matrice de couverture
    vector<vector<int>> couverture = generer_matrice_couverture();
    afficher_matrice_couverture(couverture);

    // Lire la taille de la population
    int taille_population;
    cout << "Entrez la taille de la population : ";
    cin >> taille_population;

    // Génération et affichage de la population initiale
    vector<vector<int>> population = initialiser_population(taille_population);
    cout << "\nPopulation initiale :\n";
    afficher_population(population);

    // Sélection par tournoi
    int taille_selection = taille_population / 2;
    vector<vector<int>> population_tournoi = selection_par_tournoi(population, taille_selection);
    cout << "\nPopulation après sélection par tournoi :\n";
    afficher_population(population_tournoi);

    // Croisement
    cout << "\nIndividus croisés :\n";
    vector<vector<int>> nouvelle_population;
    for (size_t i = 0; i < population.size() - 1; i += 2) {
        vector<int> enfant = croisement(population[i], population[i + 1], i / 2);
        afficher_individu(enfant, i / 2);
        nouvelle_population.push_back(enfant);
    }

    // Mutation
    cout << "\nIndividus mutés :\n";
    for (size_t i = 0; i < nouvelle_population.size(); i++) {
        mutation(nouvelle_population[i], i);
        afficher_individu(nouvelle_population[i], i);
    }

    cout << "\n Fin du traitement de " << image_choisie << "\n" << endl;

    return 0;
}

