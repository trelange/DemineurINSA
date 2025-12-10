#ifndef GUI_H
#define GUI_H

#include <gtk/gtk.h>
#include "../logique/demineur.h"

// Structure contenant les données de l'interface
typedef struct {
    GtkWidget* fenetre;
    GtkWidget* grilleBoutons;
    GtkWidget* labelMines;
    GtkWidget* labelStatut;
    GtkWidget** boutons;  // Tableau de boutons
    Grille* grille;
    int tailleCase;
} InterfaceJeu;

/**
 * Crée et affiche la fenêtre principale du jeu
 */
InterfaceJeu* creerInterface(int lignes, int colonnes, int mines);

/**
 * Met à jour l'affichage d'un bouton
 */
void mettreAJourBouton(InterfaceJeu* interface, int lig, int col);

/**
 * Met à jour le label du nombre de mines
 */
void mettreAJourLabelMines(InterfaceJeu* interface);

/**
 * Affiche un message de fin de partie
 */
void afficherFinPartie(InterfaceJeu* interface);

/**
 * Démarre une nouvelle partie
 */
void nouvellePartie(InterfaceJeu* interface);

/**
 * Libère la mémoire de l'interface
 */
void detruireInterface(InterfaceJeu* interface);

#endif // GUI_H
