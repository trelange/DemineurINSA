#include <gtk/gtk.h>
#include <stdlib.h>
#include "interface/gui.h"

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    
    // Paramètres par défaut
    int lignes = 10;
    int colonnes = 10;
    int mines = 15;
    
    // Demander les paramètres (optionnel)
    if (argc == 4) {
        lignes = atoi(argv[1]);
        colonnes = atoi(argv[2]);
        mines = atoi(argv[3]);
        
        // Vérifications
        if (lignes < 5 || lignes > MAX_LIGNES) lignes = 10;
        if (colonnes < 5 || colonnes > MAX_COLONNES) colonnes = 10;
        if (mines < 1 || mines >= lignes * colonnes) mines = lignes * colonnes / 6;
    }
    
    // Créer l'interface
    InterfaceJeu* interface = creerInterface(lignes, colonnes, mines);
    
    // Lancer la boucle GTK
    gtk_main();
    
    // Nettoyage
    detruireInterface(interface);
    
    return 0;
}
