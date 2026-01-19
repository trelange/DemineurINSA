#ifndef GUI_H
#define GUI_H

#include <gtk/gtk.h>
// Inclusion conditionnelle pour SDL2 selon le système
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "../logique/demineur.h"

// Structure contenant les données de l'interface
typedef struct {
    // Widgets GTK
    GtkWidget* fenetre;
    GtkWidget* vbox;            // Conteneur principal
    GtkWidget* grilleBoutons;   // Conteneur de la grille
    GtkWidget* labelMines;      // Affichage du compteur de mines
    GtkWidget* labelChrono;     // Affichage du temps
    GtkWidget* labelStatut;     // Message (Gagné/Perdu)
    GtkWidget** boutons;        // Tableau linéaire des boutons

    // Données du jeu
    Grille* grille;
    int tailleCase;

    // Gestion du Chronomètre
    int timerActif;     // Booléen (1 ou 0)
    int tempsEcoule;    // Secondes
    guint timerID;      // ID du timeout GTK pour pouvoir l'arrêter

    // Gestion du Son (SDL2)
    Mix_Chunk* sonClic;
    Mix_Chunk* sonBoom;
    Mix_Chunk* sonVictoire;

} InterfaceJeu;

/**
 * Crée et affiche la fenêtre principale du jeu
 */
InterfaceJeu* creerInterface(int lignes, int colonnes, int mines);

/**
 * Met à jour l'apparence d'un bouton spécifique selon l'état de la case
 */
void mettreAJourBouton(InterfaceJeu* interface, int lig, int col);

/**
 * Met à jour le texte du compteur de mines restantes
 */
void mettreAJourLabelMines(InterfaceJeu* interface);

/**
 * Gère la fin de partie (popup, score, etc.)
 */
void afficherFinPartie(InterfaceJeu* interface);

/**
 * Libère toute la mémoire (GTK et SDL)
 */
void detruireInterface(InterfaceJeu* interface);

#endif // GUI_H