#include <gtk/gtk.h>
#include <stdlib.h>
#include "interface/gui.h"

// Structure pour définir les niveaux de difficulté
typedef struct {
    int lignes;
    int colonnes;
    int mines;
    const char* nom;
} Niveau;

// Définition des niveaux
// ATTENTION : Vérifiez que 30 colonnes ne dépasse pas MAX_COLONNES dans demineur.h
const Niveau NIVEAUX[] = {
    {8, 8, 10, "Débutant (8x8 - 10 mines)"},
    {16, 16, 40, "Intermédiaire (16x16 - 40 mines)"},
    {16, 30, 99, "Expert (16x30 - 99 mines)"}
};

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    
    // Valeurs par défaut (Débutant)
    int lignes = NIVEAUX[0].lignes;
    int colonnes = NIVEAUX[0].colonnes;
    int mines = NIVEAUX[0].mines;
    
    // Si des arguments sont passés en ligne de commande (ex: ./demineur 20 20 50), on saute le menu
    if (argc == 4) {
        lignes = atoi(argv[1]);
        colonnes = atoi(argv[2]);
        mines = atoi(argv[3]);
        
        // Vérifications rapides de sécurité
        if (lignes < 5 || lignes > MAX_LIGNES) lignes = 10;
        if (colonnes < 5 || colonnes > MAX_COLONNES) colonnes = 10;
        if (mines < 1) mines = 10;
    } 
    else {
        // --- CRÉATION DE LA BOÎTE DE DIALOGUE DE SÉLECTION ---
        
        GtkWidget *dialog = gtk_dialog_new_with_buttons("Choix de la difficulté",
                                                        NULL,
                                                        GTK_DIALOG_MODAL,
                                                        "Jouer !",
                                                        GTK_RESPONSE_ACCEPT,
                                                        "Quitter",
                                                        GTK_RESPONSE_REJECT,
                                                        NULL);
        
        GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
        GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
        gtk_container_set_border_width(GTK_CONTAINER(vbox), 20);
        gtk_container_add(GTK_CONTAINER(content_area), vbox);
        
        // Label d'instruction
        GtkWidget *label = gtk_label_new("Veuillez choisir un niveau de difficulté :");
        gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);
        
        // Création des boutons radio
        GtkWidget *radio1 = gtk_radio_button_new_with_label(NULL, NIVEAUX[0].nom);
        GtkWidget *radio2 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio1), NIVEAUX[1].nom);
        GtkWidget *radio3 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio1), NIVEAUX[2].nom);
        
        // Ajouter les boutons à la boîte
        gtk_box_pack_start(GTK_BOX(vbox), radio1, FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(vbox), radio2, FALSE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(vbox), radio3, FALSE, FALSE, 0);
        
        // Sélectionner le niveau intermédiaire par défaut (optionnel)
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio1), TRUE);
        
        gtk_widget_show_all(dialog);
        
        // Attendre la réponse de l'utilisateur
        gint result = gtk_dialog_run(GTK_DIALOG(dialog));
        
        if (result == GTK_RESPONSE_ACCEPT) {
            if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio1))) {
                lignes = NIVEAUX[0].lignes;
                colonnes = NIVEAUX[0].colonnes;
                mines = NIVEAUX[0].mines;
            } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio2))) {
                lignes = NIVEAUX[1].lignes;
                colonnes = NIVEAUX[1].colonnes;
                mines = NIVEAUX[1].mines;
            } else {
                lignes = NIVEAUX[2].lignes;
                colonnes = NIVEAUX[2].colonnes;
                mines = NIVEAUX[2].mines;
            }
            gtk_widget_destroy(dialog);
        } else {
            // Si l'utilisateur clique sur Quitter ou ferme la fenêtre
            gtk_widget_destroy(dialog);
            return 0; 
        }
    }
    
    // Créer l'interface avec les paramètres choisis
    InterfaceJeu* interface = creerInterface(lignes, colonnes, mines);
    
    // Lancer la boucle GTK
    gtk_main();
    
    // Nettoyage
    detruireInterface(interface);
    
    return 0;
}