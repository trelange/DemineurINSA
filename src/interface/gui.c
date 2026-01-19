#include "gui.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Structure pour passer les données aux callbacks
typedef struct {
    InterfaceJeu* interface;
    int ligne;
    int colonne;
} DonneesBouton;

// Structure pour le tri des scores
typedef struct {
    char nom[50];
    int temps;
    char difficulte[20];
} Score;

// --- GESTION AUDIO (SDL2) ---

void initAudio(InterfaceJeu* interface) {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        g_warning("Erreur SDL Init: %s", SDL_GetError());
        return;
    }
    // Initialisation Mixer (44.1kHz, stereo)
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        g_warning("Erreur SDL Mixer: %s", Mix_GetError());
        return;
    }

    interface->sonClic = Mix_LoadWAV("click.wav");
    interface->sonBoom = Mix_LoadWAV("boom.wav");
    interface->sonVictoire = Mix_LoadWAV("win.wav");
}

void jouerSon(Mix_Chunk* son) {
    if (son != NULL) {
        Mix_PlayChannel(-1, son, 0);
    }
}

void nettoyerAudio(InterfaceJeu* interface) {
    if (interface->sonClic) Mix_FreeChunk(interface->sonClic);
    if (interface->sonBoom) Mix_FreeChunk(interface->sonBoom);
    if (interface->sonVictoire) Mix_FreeChunk(interface->sonVictoire);
    Mix_CloseAudio();
    SDL_Quit();
}

// --- GESTION CHRONO ---

static gboolean topChrono(gpointer data) {
    InterfaceJeu* interface = (InterfaceJeu*)data;
    if (!interface->timerActif) return FALSE;

    interface->tempsEcoule++;
    char texte[64];
    sprintf(texte, "⏱️ %03d s", interface->tempsEcoule);
    gtk_label_set_text(GTK_LABEL(interface->labelChrono), texte);

    return TRUE;
}

void demarrerChrono(InterfaceJeu* interface) {
    if (!interface->timerActif) {
        interface->timerActif = 1;
        interface->timerID = g_timeout_add(1000, topChrono, interface);
    }
}

void arreterChrono(InterfaceJeu* interface) {
    interface->timerActif = 0;
    if (interface->timerID > 0) {
        g_source_remove(interface->timerID);
        interface->timerID = 0;
    }
}

// --- GESTION SCORES (TOP 10) ---

int comparerScores(const void* a, const void* b) {
    Score* s1 = (Score*)a;
    Score* s2 = (Score*)b;
    return s1->temps - s2->temps;
}

void sauvegarderScore(const char* nom, int temps, const char* difficulte) {
    Score scores[100];
    int count = 0;

    FILE* f = fopen("scores.txt", "r");
    if (f) {
        while (count < 99 && fscanf(f, "%s %d %s", scores[count].nom, &scores[count].temps, scores[count].difficulte) == 3) {
            count++;
        }
        fclose(f);
    }

    strncpy(scores[count].nom, nom, 49);
    scores[count].temps = temps;
    strncpy(scores[count].difficulte, difficulte, 19);
    count++;


    qsort(scores, count, sizeof(Score), comparerScores);

    f = fopen("scores.txt", "w");
    if (f) {
        int limit = (count > 10) ? 10 : count;
        for (int i = 0; i < limit; i++) {
            fprintf(f, "%s %d %s\n", scores[i].nom, scores[i].temps, scores[i].difficulte);
        }
        fclose(f);
    }
}

static void surVoirScores(GtkWidget* widget, gpointer data) {
    InterfaceJeu* interface = (InterfaceJeu*)data;
    char buffer[2048] = "🏆 TOP 10 MEILLEURS TEMPS 🏆\n\n";
    
    FILE* f = fopen("scores.txt", "r");
    if (f) {
        char nom[50], diff[50];
        int temps;
        int i = 1;
        while (fscanf(f, "%s %d %s", nom, &temps, diff) == 3) {
            char ligne[200];
            sprintf(ligne, "%d. %s - %d s (%s)\n", i++, nom, temps, diff);
            strcat(buffer, ligne);
        }
        fclose(f);
    } else {
        strcat(buffer, "Aucun score enregistré.");
    }

    GtkWidget* dialog = gtk_message_dialog_new(GTK_WINDOW(interface->fenetre),
                                   GTK_DIALOG_DESTROY_WITH_PARENT,
                                   GTK_MESSAGE_INFO,
                                   GTK_BUTTONS_OK,
                                   "%s", buffer);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}


void mettreAJourLabelMines(InterfaceJeu* interface) {
    char txt[50];
    int restants = interface->grille->nbMines - interface->grille->nbDrapeaux;
    sprintf(txt, "💣 Mines: %d", restants);
    gtk_label_set_text(GTK_LABEL(interface->labelMines), txt);
}

const char* obtenirCouleurChiffre(int n) {
    switch(n) {
        case 1: return "blue";
        case 2: return "green";
        case 3: return "red";
        case 4: return "darkblue";
        case 5: return "brown";
        default: return "black";
    }
}

void mettreAJourBouton(InterfaceJeu* interface, int lig, int col) {
    int index = lig * interface->grille->nbColonnes + col;
    GtkWidget* btn = interface->boutons[index];
    EtatCase etat = obtenirEtatCase(interface->grille, lig, col);

    // Nettoyer le contenu actuel du bouton (label ou image)
    GtkWidget *child = gtk_bin_get_child(GTK_BIN(btn));
    if (child) gtk_widget_destroy(child);

    if (etat == CACHE) {
        gtk_widget_set_sensitive(btn, TRUE);
        // Bouton vide standard (style par défaut)
    } 
    else if (etat == DRAPEAU) {
        GtkWidget *lbl = gtk_label_new("🚩");
        gtk_container_add(GTK_CONTAINER(btn), lbl);
        gtk_widget_show(lbl);
    }
    else if (etat == REVELE) {
        gtk_widget_set_sensitive(btn, FALSE); // Désactive le clic, aspect "enfoncé"

        if (estMine(interface->grille, lig, col)) {
            GtkWidget *lbl = gtk_label_new("💣");
            gtk_container_add(GTK_CONTAINER(btn), lbl);
            gtk_widget_show(lbl);
        } else {
            int n = obtenirNbMinesVoisines(interface->grille, lig, col);
            if (n > 0) {
                char nbStr[4];
                sprintf(nbStr, "%d", n);
                GtkWidget *lbl = gtk_label_new(NULL);
                
                // Utilisation de Markup pour la couleur
                char markup[100];
                sprintf(markup, "<span weight='bold' foreground='%s'>%s</span>", 
                        obtenirCouleurChiffre(n), nbStr);
                
                gtk_label_set_markup(GTK_LABEL(lbl), markup);
                gtk_container_add(GTK_CONTAINER(btn), lbl);
                gtk_widget_show(lbl);
            }
        }
    }
}


void regenererGrille(InterfaceJeu* interface, int lignes, int colonnes, int mines);

static void surClicBouton(GtkWidget* widget, GdkEventButton* event, gpointer data) {
    DonneesBouton* donnees = (DonneesBouton*)data;
    InterfaceJeu* interface = donnees->interface;
    
    if (interface->grille->partieTerminee) return;

    // Premier clic : on lance le chrono
    if (interface->grille->premiereRevelation && !interface->timerActif) {
        demarrerChrono(interface);
    }

    if (event->button == 1) { // Clic Gauche
        if (obtenirEtatCase(interface->grille, donnees->ligne, donnees->colonne) == DRAPEAU) return;

        int mineTouchee = revelerCase(interface->grille, donnees->ligne, donnees->colonne);
        jouerSon(interface->sonClic);
        
        // Rafraichir toute la grille (pour voir les effets de propagation)
        for(int i=0; i<interface->grille->nbLignes; i++) {
            for(int j=0; j<interface->grille->nbColonnes; j++) {
                mettreAJourBouton(interface, i, j);
            }
        }

        if (mineTouchee) {
            arreterChrono(interface);
            jouerSon(interface->sonBoom);
            gtk_label_set_text(GTK_LABEL(interface->labelStatut), "💥 BOOM !");
            afficherFinPartie(interface);
        } else if (interface->grille->partieTerminee == 2) {
            arreterChrono(interface);
            jouerSon(interface->sonVictoire);
            gtk_label_set_text(GTK_LABEL(interface->labelStatut), "🎉 VICTOIRE !");
            afficherFinPartie(interface);
        }

    } else if (event->button == 3) { // Clic Droit
        basculerDrapeau(interface->grille, donnees->ligne, donnees->colonne);
        mettreAJourBouton(interface, donnees->ligne, donnees->colonne);
        mettreAJourLabelMines(interface);
    }
}

void afficherFinPartie(InterfaceJeu* interface) {
    if (interface->grille->partieTerminee == 2) { // Victoire
        GtkWidget *dialog = gtk_dialog_new_with_buttons("Victoire !",
            GTK_WINDOW(interface->fenetre),
            GTK_DIALOG_MODAL,
            "Enregistrer", GTK_RESPONSE_ACCEPT,
            NULL);
        
        GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
        GtkWidget *lbl = gtk_label_new("Bravo ! Entrez votre pseudo :");
        GtkWidget *entry = gtk_entry_new();
        gtk_entry_set_text(GTK_ENTRY(entry), "Joueur");
        
        gtk_container_add(GTK_CONTAINER(content), lbl);
        gtk_container_add(GTK_CONTAINER(content), entry);
        gtk_widget_show_all(dialog);
        
        if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
            const char* nom = gtk_entry_get_text(GTK_ENTRY(entry));
            char diff[20] = "Perso";
            if (interface->grille->nbMines == 10) strcpy(diff, "Debutant");
            else if (interface->grille->nbMines == 40) strcpy(diff, "Intermed");
            else if (interface->grille->nbMines == 99) strcpy(diff, "Expert");
            
            sauvegarderScore(nom, interface->tempsEcoule, diff);
        }
        gtk_widget_destroy(dialog);
        // Afficher les scores après l'enregistrement
        surVoirScores(NULL, interface);
    } else {
        GtkWidget* dialog = gtk_message_dialog_new(GTK_WINDOW(interface->fenetre),
                                       GTK_DIALOG_DESTROY_WITH_PARENT,
                                       GTK_MESSAGE_ERROR,
                                       GTK_BUTTONS_OK,
                                       "Vous avez perdu !");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
}


void regenererGrille(InterfaceJeu* interface, int lignes, int colonnes, int mines) {
    arreterChrono(interface);
    interface->tempsEcoule = 0;
    interface->timerActif = 0;

    gtk_label_set_text(GTK_LABEL(interface->labelChrono), "⏱️ 000 s");
    gtk_label_set_text(GTK_LABEL(interface->labelStatut), "Bonne chance !");

    if (interface->grilleBoutons) {
        gtk_widget_destroy(interface->grilleBoutons);
        free(interface->boutons);
    }

    initialiserGrille(interface->grille, lignes, colonnes, mines);

    interface->grilleBoutons = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(interface->grilleBoutons), 2);
    gtk_grid_set_column_spacing(GTK_GRID(interface->grilleBoutons), 2);
    // Centrer la grille
    gtk_widget_set_halign(interface->grilleBoutons, GTK_ALIGN_CENTER);
    
    gtk_box_pack_start(GTK_BOX(interface->vbox), interface->grilleBoutons, TRUE, TRUE, 0);

    interface->boutons = malloc(lignes * colonnes * sizeof(GtkWidget*));

    for (int i = 0; i < lignes; i++) {
        for (int j = 0; j < colonnes; j++) {
            GtkWidget* btn = gtk_button_new();
            gtk_widget_set_size_request(btn, interface->tailleCase, interface->tailleCase);
            
            DonneesBouton* donnees = malloc(sizeof(DonneesBouton));
            donnees->interface = interface;
            donnees->ligne = i;
            donnees->colonne = j;
            
            // Attacher le callback de données pour nettoyage auto
            g_object_set_data_full(G_OBJECT(btn), "donnees", donnees, free);
            
            g_signal_connect(btn, "button-press-event", G_CALLBACK(surClicBouton), donnees);
            gtk_grid_attach(GTK_GRID(interface->grilleBoutons), btn, j, i, 1, 1);
            
            interface->boutons[i * colonnes + j] = btn;
        }
    }

    mettreAJourLabelMines(interface);
    gtk_widget_show_all(interface->fenetre);
    // Redimensionner la fenêtre pour qu'elle s'adapte à la nouvelle grille
    gtk_window_resize(GTK_WINDOW(interface->fenetre), 1, 1);
}

static void surNouvellePartie(GtkWidget* widget, gpointer data) {
    InterfaceJeu* interface = (InterfaceJeu*)data;
    // On relance avec les paramètres actuels (ou on pourrait rouvrir le menu)
    regenererGrille(interface, interface->grille->nbLignes, interface->grille->nbColonnes, interface->grille->nbMines);
}

// --- CREATION / DESTRUCTION ---

InterfaceJeu* creerInterface(int lignes, int colonnes, int mines) {
    InterfaceJeu* interface = malloc(sizeof(InterfaceJeu));
    interface->grille = malloc(sizeof(Grille));
    interface->tailleCase = 25; // Taille cases
    interface->grilleBoutons = NULL;
    interface->boutons = NULL;
    interface->timerActif = 0;
    interface->timerID = 0;
    interface->tempsEcoule = 0;
    
    // Initialisation Audio
    initAudio(interface);

    interface->fenetre = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(interface->fenetre), "Démineur Pro");
    gtk_window_set_position(GTK_WINDOW(interface->fenetre), GTK_WIN_POS_CENTER);
    gtk_container_set_border_width(GTK_CONTAINER(interface->fenetre), 10);
    g_signal_connect(interface->fenetre, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    interface->vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(interface->fenetre), interface->vbox);

    // Barre d'info supérieure
    GtkWidget* hboxInfo = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(interface->vbox), hboxInfo, FALSE, FALSE, 0);

    interface->labelMines = gtk_label_new("💣");
    gtk_box_pack_start(GTK_BOX(hboxInfo), interface->labelMines, FALSE, FALSE, 0);

    interface->labelStatut = gtk_label_new("Bonne chance !");
    gtk_box_pack_start(GTK_BOX(hboxInfo), interface->labelStatut, TRUE, TRUE, 0);

    interface->labelChrono = gtk_label_new("⏱️ 000 s");
    gtk_box_pack_start(GTK_BOX(hboxInfo), interface->labelChrono, FALSE, FALSE, 0);

    // Boutons de contrôle
    GtkWidget* btnScores = gtk_button_new_with_label("🏆 Scores");
    g_signal_connect(btnScores, "clicked", G_CALLBACK(surVoirScores), interface);
    gtk_box_pack_start(GTK_BOX(hboxInfo), btnScores, FALSE, FALSE, 0);
    
    GtkWidget* btnReset = gtk_button_new_with_label("🔄 Reset");
    g_signal_connect(btnReset, "clicked", G_CALLBACK(surNouvellePartie), interface);
    
    gtk_box_pack_start(GTK_BOX(hboxInfo), btnReset, FALSE, FALSE, 0);

    regenererGrille(interface, lignes, colonnes, mines);

    return interface;
}

void detruireInterface(InterfaceJeu* interface) {
    arreterChrono(interface);
    nettoyerAudio(interface);
    if (interface->grille) free(interface->grille);
    if (interface->boutons) free(interface->boutons);
    free(interface);
}