#include "gui.h"
#include <stdlib.h>
#include <string.h>

// Structure pour passer les données aux callbacks
typedef struct {
    InterfaceJeu* interface;
    int ligne;
    int colonne;
} DonneesBouton;

// ============================================================================
// Callbacks GTK
// ============================================================================

static gboolean surClicBouton(GtkWidget* widget, GdkEventButton* event, gpointer data) {
    DonneesBouton* donnees = (DonneesBouton*)data;
    InterfaceJeu* interface = donnees->interface;
    int lig = donnees->ligne;
    int col = donnees->colonne;
    
    // Ne rien faire si la partie est terminée
    if (interface->grille->partieTerminee) {
        return FALSE;
    }
    
    if (event->button == 1) { // Clic gauche
        int mineTouchee = revelerCase(interface->grille, lig, col);
        
        // Mettre à jour l'affichage
        for (int i = 0; i < interface->grille->nbLignes; i++) {
            for (int j = 0; j < interface->grille->nbColonnes; j++) {
                mettreAJourBouton(interface, i, j);
            }
        }
        
        if (mineTouchee) {
            gtk_label_set_text(GTK_LABEL(interface->labelStatut), 
                             "💥 PERDU ! Vous avez touché une mine !");
            afficherFinPartie(interface);
        } else if (interface->grille->partieTerminee == 2) {
            gtk_label_set_text(GTK_LABEL(interface->labelStatut), 
                             "🎉 GAGNÉ ! Félicitations !");
            afficherFinPartie(interface);
        }
        
    } else if (event->button == 3) { // Clic droit
        basculerDrapeau(interface->grille, lig, col);
        mettreAJourBouton(interface, lig, col);
        mettreAJourLabelMines(interface);
    }
    
    return FALSE;
}

static void surNouvellePartie(GtkWidget* widget, gpointer data) {
    InterfaceJeu* interface = (InterfaceJeu*)data;
    nouvellePartie(interface);
}

static void surQuitter(GtkWidget* widget, gpointer data) {
    gtk_main_quit();
}

// ============================================================================
// Fonctions d'interface
// ============================================================================

InterfaceJeu* creerInterface(int lignes, int colonnes, int mines) {
    InterfaceJeu* interface = malloc(sizeof(InterfaceJeu));
    interface->grille = malloc(sizeof(Grille));
    interface->tailleCase = 40;
    
    // Initialiser la grille de jeu
    initialiserGrille(interface->grille, lignes, colonnes, mines);
    
    // Créer la fenêtre principale
    interface->fenetre = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(interface->fenetre), "Démineur - INSA");
    gtk_window_set_position(GTK_WINDOW(interface->fenetre), GTK_WIN_POS_CENTER);
    gtk_container_set_border_width(GTK_CONTAINER(interface->fenetre), 10);
    g_signal_connect(interface->fenetre, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    // Boîte verticale principale
    GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(interface->fenetre), vbox);
    
    // Barre d'information
    GtkWidget* hboxInfo = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(vbox), hboxInfo, FALSE, FALSE, 0);
    
    // Label du nombre de mines
    interface->labelMines = gtk_label_new("");
    gtk_box_pack_start(GTK_BOX(hboxInfo), interface->labelMines, FALSE, FALSE, 0);
    mettreAJourLabelMines(interface);
    
    // Label du statut
    interface->labelStatut = gtk_label_new("Bonne chance ! 🍀");
    gtk_box_pack_start(GTK_BOX(hboxInfo), interface->labelStatut, TRUE, TRUE, 0);
    
    // Bouton nouvelle partie
    GtkWidget* btnNouveau = gtk_button_new_with_label("Nouvelle Partie");
    gtk_box_pack_end(GTK_BOX(hboxInfo), btnNouveau, FALSE, FALSE, 0);
    g_signal_connect(btnNouveau, "clicked", G_CALLBACK(surNouvellePartie), interface);
    
    // Grille de boutons
    interface->grilleBoutons = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(interface->grilleBoutons), 2);
    gtk_grid_set_column_spacing(GTK_GRID(interface->grilleBoutons), 2);
    gtk_box_pack_start(GTK_BOX(vbox), interface->grilleBoutons, TRUE, TRUE, 0);
    
    // Créer les boutons
    int nbBoutons = lignes * colonnes;
    interface->boutons = malloc(nbBoutons * sizeof(GtkWidget*));
    
    for (int i = 0; i < lignes; i++) {
        for (int j = 0; j < colonnes; j++) {
            int index = i * colonnes + j;
            
            GtkWidget* btn = gtk_button_new();
            gtk_widget_set_size_request(btn, interface->tailleCase, interface->tailleCase);
            
            // Créer les données du bouton
            DonneesBouton* donnees = malloc(sizeof(DonneesBouton));
            donnees->interface = interface;
            donnees->ligne = i;
            donnees->colonne = j;
            
            // Connecter les événements
            gtk_widget_add_events(btn, GDK_BUTTON_PRESS_MASK);
            g_signal_connect(btn, "button-press-event", G_CALLBACK(surClicBouton), donnees);
            
            gtk_grid_attach(GTK_GRID(interface->grilleBoutons), btn, j, i, 1, 1);
            interface->boutons[index] = btn;
        }
    }
    
    // Afficher tous les widgets
    gtk_widget_show_all(interface->fenetre);
    
    return interface;
}

void mettreAJourBouton(InterfaceJeu* interface, int lig, int col) {
    int index = lig * interface->grille->nbColonnes + col;
    GtkWidget* btn = interface->boutons[index];
    
    EtatCase etat = obtenirEtatCase(interface->grille, lig, col);
    
    if (etat == DRAPEAU) {
        gtk_button_set_label(GTK_BUTTON(btn), "🚩");
        return;
    }
    
    if (etat == REVELE) {
        gtk_widget_set_sensitive(btn, FALSE);
        
        if (estMine(interface->grille, lig, col)) {
            gtk_button_set_label(GTK_BUTTON(btn), "💣");
            
            // Modifier la couleur pour les mines
            GdkRGBA couleur;
            gdk_rgba_parse(&couleur, "#ff0000");
            gtk_widget_override_background_color(btn, GTK_STATE_FLAG_NORMAL, &couleur);
        } else {
            int nbMines = obtenirNbMinesVoisines(interface->grille, lig, col);
            
            if (nbMines == 0) {
                gtk_button_set_label(GTK_BUTTON(btn), "");
            } else {
                char label[2];
                sprintf(label, "%d", nbMines);
                gtk_button_set_label(GTK_BUTTON(btn), label);
                
                // Couleurs selon le nombre
                const char* couleurs[] = {
                    "#0000ff", "#008000", "#ff0000", "#000080",
                    "#800000", "#008080", "#000000", "#808080"
                };
                
                GdkRGBA couleur;
                gdk_rgba_parse(&couleur, couleurs[nbMines - 1]);
                gtk_widget_override_color(btn, GTK_STATE_FLAG_NORMAL, &couleur);
            }
            
            // Fond gris clair pour cases révélées
            GdkRGBA fondCouleur;
            gdk_rgba_parse(&fondCouleur, "#e0e0e0");
            gtk_widget_override_background_color(btn, GTK_STATE_FLAG_NORMAL, &fondCouleur);
        }
    } else {
        gtk_button_set_label(GTK_BUTTON(btn), "");
    }
}

void mettreAJourLabelMines(InterfaceJeu* interface) {
    char texte[50];
    sprintf(texte, "💣 Mines: %d / %d", 
            interface->grille->nbDrapeaux, 
            interface->grille->nbMines);
    gtk_label_set_text(GTK_LABEL(interface->labelMines), texte);
}

void afficherFinPartie(InterfaceJeu* interface) {
    GtkWidget* dialog;
    const char* message;
    
    if (interface->grille->partieTerminee == 1) {
        message = "💥 Partie perdue !\n\nVous avez touché une mine.";
    } else {
        message = "🎉 Félicitations !\n\nVous avez gagné !";
    }
    
    dialog = gtk_message_dialog_new(GTK_WINDOW(interface->fenetre),
                                   GTK_DIALOG_DESTROY_WITH_PARENT,
                                   GTK_MESSAGE_INFO,
                                   GTK_BUTTONS_OK,
                                   "%s", message);
    
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void nouvellePartie(InterfaceJeu* interface) {
    initialiserGrille(interface->grille, 
                     interface->grille->nbLignes,
                     interface->grille->nbColonnes,
                     interface->grille->nbMines);
    
    // Réinitialiser l'affichage
    for (int i = 0; i < interface->grille->nbLignes; i++) {
        for (int j = 0; j < interface->grille->nbColonnes; j++) {
            int index = i * interface->grille->nbColonnes + j;
            GtkWidget* btn = interface->boutons[index];
            
            gtk_widget_set_sensitive(btn, TRUE);
            gtk_button_set_label(GTK_BUTTON(btn), "");
            gtk_widget_override_background_color(btn, GTK_STATE_FLAG_NORMAL, NULL);
            gtk_widget_override_color(btn, GTK_STATE_FLAG_NORMAL, NULL);
        }
    }
    
    gtk_label_set_text(GTK_LABEL(interface->labelStatut), "Bonne chance ! 🍀");
    mettreAJourLabelMines(interface);
}

void detruireInterface(InterfaceJeu* interface) {
    free(interface->grille);
    free(interface->boutons);
    free(interface);
}
