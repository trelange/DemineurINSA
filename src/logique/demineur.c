#include "demineur.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>

int estPositionValide(Grille* grille, int lig, int col) {
    return (lig >= 0 && lig < grille->nbLignes && 
            col >= 0 && col < grille->nbColonnes);
}

int estMine(Grille* grille, int lig, int col) {
    if (!estPositionValide(grille, lig, col)) return 0;
    return grille->grille[lig][col].estMine;
}

EtatCase obtenirEtatCase(Grille* grille, int lig, int col) {
    if (!estPositionValide(grille, lig, col)) return CACHE;
    return grille->grille[lig][col].etat;
}

int obtenirNbMinesVoisines(Grille* grille, int lig, int col) {
    if (!estPositionValide(grille, lig, col)) return 0;
    return grille->grille[lig][col].nbMinesVoisines;
}

int compterVoisins(Grille* grille, int lig, int col, int typeCellule) {
    int compte = 0;
    //On parcourt les cases voisines
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) continue; // Ignorer la case centrale

            int vLig = lig + i;
            int vCol = col + j;

            if (estPositionValide(grille, vLig, vCol)) {
                
                if (typeCellule == 1 && grille->grille[vLig][vCol].estMine) {
                    compte++;
                }
            }
        }
    }
    return compte;
}


void initialiserGrille(Grille* grille, int lignes, int colonnes, int mines) {
    // Initialisation du générateur aléatoire
    srand(time(NULL));

    // Protection des limites
    if (lignes > MAX_LIGNES) lignes = MAX_LIGNES;
    if (colonnes > MAX_COLONNES) colonnes = MAX_COLONNES;

    grille->nbLignes = lignes;
    grille->nbColonnes = colonnes;
    grille->nbMines = mines;
    grille->nbCasesRevelees = 0;
    grille->nbDrapeaux = 0;
    grille->partieTerminee = 0; // 0: en cours
    grille->premiereRevelation = 1;

    // Initialisation des cases
    for (int i = 0; i < lignes; i++) {
        for (int j = 0; j < colonnes; j++) {
            grille->grille[i][j].estMine = 0;
            grille->grille[i][j].nbMinesVoisines = 0;
            grille->grille[i][j].etat = CACHE;
        }
    }
}

void placerMines(Grille* grille, int ligIgnore, int colIgnore) {
    int minesPlacees = 0;
    
    while (minesPlacees < grille->nbMines) {
        int l = rand() % grille->nbLignes;
        int c = rand() % grille->nbColonnes;

        // On ne place pas de mine si :
        // 1. Il y en a déjà une
        // 2. C'est la case du premier clic (ligIgnore, colIgnore)
        if (!grille->grille[l][c].estMine && (l != ligIgnore || c != colIgnore)) {
            grille->grille[l][c].estMine = 1;
            minesPlacees++;
        }
    }
}

void calculerMinesVoisines(Grille* grille) {
    for (int i = 0; i < grille->nbLignes; i++) {
        for (int j = 0; j < grille->nbColonnes; j++) {
            if (!grille->grille[i][j].estMine) {
            
                grille->grille[i][j].nbMinesVoisines = compterVoisins(grille, i, j, 1);
            }
        }
    }
}


void revelerToutesMines(Grille* grille) {
    for (int i = 0; i < grille->nbLignes; i++) {
        for (int j = 0; j < grille->nbColonnes; j++) {
            if (grille->grille[i][j].estMine) {
                grille->grille[i][j].etat = REVELE;
            }
        }
    }
}

int verifierVictoire(Grille* grille) {
    int totalCases = grille->nbLignes * grille->nbColonnes;
    if (grille->nbCasesRevelees == totalCases - grille->nbMines) {
        grille->partieTerminee = 2; // 2 = Victoire
        return 1;
    }
    return 0;
}

void revelerCasesVides(Grille* grille, int lig, int col) {
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) continue;

            int vLig = lig + i;
            int vCol = col + j;

            if (estPositionValide(grille, vLig, vCol)) {
                Case* c = &grille->grille[vLig][vCol];
                if (c->etat == CACHE && !c->estMine) {
                    revelerCase(grille, vLig, vCol);
                }
            }
        }
    }
}

void basculerDrapeau(Grille* grille, int lig, int col) {
    if (!estPositionValide(grille, lig, col) || grille->partieTerminee) return;

    Case* c = &grille->grille[lig][col];

    if (c->etat == CACHE) {
        c->etat = DRAPEAU;
        grille->nbDrapeaux++;
    } else if (c->etat == DRAPEAU) {
        c->etat = CACHE;
        grille->nbDrapeaux--;
    }
}

int revelerCase(Grille* grille, int lig, int col) {
    if (!estPositionValide(grille, lig, col) || grille->partieTerminee) return 0;

    Case* c = &grille->grille[lig][col];

    // On ne peut pas révéler une case marquée d'un drapeau ou déjà révélée
    if (c->etat == DRAPEAU || c->etat == REVELE) return 0;

    // GESTION DU PREMIER CLIC :
    // On ne place les mines et on ne calcule les voisins qu'au premier clic pour s'assurer que le joueur ne tombe jamais sur une mine dès le début.
    if (grille->premiereRevelation) {
        grille->premiereRevelation = 0;
        placerMines(grille, lig, col);
        calculerMinesVoisines(grille);
    }

    c->etat = REVELE;
    grille->nbCasesRevelees++;

    if (c->estMine) {
        grille->partieTerminee = 1; // 1 = Défaite
        revelerToutesMines(grille);
        return 1; // Mine touchée
    }

    // Si la case est vide (0 mines voisines), on révèle les voisins
    if (c->nbMinesVoisines == 0) {
        revelerCasesVides(grille, lig, col);
    }

    verifierVictoire(grille);

    return 0; // Pas de mine
}