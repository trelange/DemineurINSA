#ifndef DEMINEUR_H
#define DEMINEUR_H

#define MAX_LIGNES 30
#define MAX_COLONNES 30
#define MAX_MINES 200

// États d'une case
typedef enum {
    CACHE = 0,      // Case non révélée
    REVELE = 1,     // Case révélée
    DRAPEAU = 2     // Case marquée avec un drapeau
} EtatCase;

// Structure représentant une case
typedef struct {
    int estMine;           // 1 si la case contient une mine, 0 sinon
    int nbMinesVoisines;   // Nombre de mines dans les cases voisines (0-8)
    EtatCase etat;         // État actuel de la case
} Case;

// Structure représentant la grille de jeu
typedef struct {
    Case grille[MAX_LIGNES][MAX_COLONNES];
    int nbLignes;
    int nbColonnes;
    int nbMines;
    int nbCasesRevelees;
    int nbDrapeaux;
    int partieTerminee;    // 1 si perdu, 2 si gagné, 0 sinon
    int premiereRevelation; // 1 si c'est le premier clic (pour éviter mine)
} Grille;

// ============================================================================
// Fonctions d'initialisation
// ============================================================================

/**
 * Initialise une nouvelle grille vide
 */
void initialiserGrille(Grille* grille, int lignes, int colonnes, int mines);

/**
 * Place aléatoirement les mines sur la grille
 * Évite de placer une mine à la position (ligIgnore, colIgnore)
 */
void placerMines(Grille* grille, int ligIgnore, int colIgnore);

/**
 * Calcule le nombre de mines voisines pour chaque case
 */
void calculerMinesVoisines(Grille* grille);

// ============================================================================
// Fonctions de jeu
// ============================================================================

/**
 * Révèle une case à la position (lig, col)
 * Retourne 1 si une mine est révélée, 0 sinon
 */
int revelerCase(Grille* grille, int lig, int col);

/**
 * Place ou retire un drapeau à la position (lig, col)
 */
void basculerDrapeau(Grille* grille, int lig, int col);

/**
 * Révèle récursivement les cases vides adjacentes
 */
void revelerCasesVides(Grille* grille, int lig, int col);

/**
 * Vérifie si la partie est gagnée
 */
int verifierVictoire(Grille* grille);

/**
 * Révèle toutes les mines (appelé quand le joueur perd)
 */
void revelerToutesMines(Grille* grille);

// ============================================================================
// Fonctions utilitaires
// ============================================================================

/**
 * Compte le nombre de voisins d'une case
 */
int compterVoisins(Grille* grille, int lig, int col, int typeCellule);

/**
 * Vérifie si une position est valide dans la grille
 */
int estPositionValide(Grille* grille, int lig, int col);

/**
 * Obtient l'état d'une case
 */
EtatCase obtenirEtatCase(Grille* grille, int lig, int col);

/**
 * Obtient le nombre de mines voisines d'une case
 */
int obtenirNbMinesVoisines(Grille* grille, int lig, int col);

/**
 * Vérifie si une case contient une mine
 */
int estMine(Grille* grille, int lig, int col);

#endif // DEMINEUR_H
