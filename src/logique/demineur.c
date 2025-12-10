#include "demineur.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>

// ============================================================================
// Fonctions d'initialisation
// ============================================================================

void initialiserGrille(Grille* grille, int lignes, int colonnes, int mines) {
    // TODO Question 2.1: Implementer l'initialisation de la grille
}

void placerMines(Grille* grille, int ligIgnore, int colIgnore) {
    // TODO Question 2.2: Placer les mines aleatoirement
    // Indice: utilisez rand() pour generer des positions aleatoires
    // N'oubliez pas d'initialiser srand() dans initialiserGrille
}

void calculerMinesVoisines(Grille* grille) {
    // TODO Question 2.3: Calculer le nombre de mines voisines pour chaque case
}

// ============================================================================
// Fonctions de jeu
// ============================================================================

int revelerCase(Grille* grille, int lig, int col) {
    // TODO Question 3.1: Implementer la revelation d'une case
    return 0;
}

void basculerDrapeau(Grille* grille, int lig, int col) {
    // TODO Question 3.2: Implementer la gestion des drapeaux
}

void revelerCasesVides(Grille* grille, int lig, int col) {
    // TODO Question 4.1: Implementer la revelation recursive des cases vides
    // Indice: utilisez les 8 directions pour parcourir les voisins
}

int verifierVictoire(Grille* grille) {
    // TODO Question 5.1: Verifier si toutes les cases non-minees sont revelees
    return 0;
}

void revelerToutesMines(Grille* grille) {
    // TODO Question 5.2: Reveler toutes les mines (lors d'une defaite)
}

// ============================================================================
// Fonctions utilitaires
// ============================================================================

int compterVoisins(Grille* grille, int lig, int col, int typeCellule) {
    // TODO Question 2.3: Compter le nombre de voisins d'un certain type
    // typeCellule == 1 pour compter les mines
    return 0;
}

int estPositionValide(Grille* grille, int lig, int col) {
    // TODO Question 6: Verifier si une position est dans les limites de la grille
    return 0;
}

EtatCase obtenirEtatCase(Grille* grille, int lig, int col) {
    // TODO Question 6: Retourner l'etat d'une case
    if (!estPositionValide(grille, lig, col)) {
        return CACHE;
    }
    return CACHE;
}

int obtenirNbMinesVoisines(Grille* grille, int lig, int col) {
    // TODO Question 6: Retourner le nombre de mines voisines
    if (!estPositionValide(grille, lig, col)) {
        return 0;
    }
    return 0;
}

int estMine(Grille* grille, int lig, int col) {
    // TODO Question 6: Verifier si une case contient une mine
    if (!estPositionValide(grille, lig, col)) {
        return 0;
    }
    return 0;
}