#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <gtk/gtk.h>
#include "points.h"
#include "pile.h"

//-----------------------------------------------------------------------------
// Déclaration des types
//-----------------------------------------------------------------------------
/**
   Le contexte contient les informations utiles de l'interface pour
   les algorithmes de géométrie algorithmique.
*/
typedef struct SContexte {
    int width;
    int height;
    GtkWidget *drawing_area;
    GtkWidget *points_entry;
    GtkWidget *points_label;
    GtkWidget *sommet_label;
    GtkWidget *time_label;
    TabPoints P;
    PilePoints pile;
} Contexte;


//-----------------------------------------------------------------------------
// Déclaration des fonctions
//-----------------------------------------------------------------------------
/**
   Crée l'interface graphique en fonction du contexte \a pCtxt.
*/
GtkWidget *creerIHM(Contexte *pCtxt);

/**
   Génère un certain nombre de points distribués aléatoirement dans le
   disque unité et les ajoute au contexte.
*/
gboolean diskRandom(GtkWidget *widget, gpointer data);

/**
 * Génère un certain nombre de points distribués aléatoirement dans un losange
 */
gboolean losangeRandom(GtkWidget *widget, gpointer data);

/**
 * Effectue le balayage de Graham sur les points
 */
gboolean graham(GtkWidget *widget, gpointer data);

/**
 * Effectue l'algorithme de jarvis sur les points
 */
gboolean jarvis(GtkWidget *widget, gpointer data);

/**
    Cette réaction est appelée à la création de la zone de dessin.
*/
gboolean realize_evt_reaction(GtkWidget *widget, gpointer data);

/**
   C'est la réaction principale qui est appelée pour redessiner la zone de dessin.
*/
gboolean expose_evt_reaction(GtkWidget *widget, GdkEventExpose *event, gpointer data);

/**
   Fait la conversion coordonnées réelles de \a p vers coordonnées de la zone de dessin.
   @param p le point en entrée
   @param pCtxt le contexte de l'IHM
   @return ses coordonnées dans la zone de dessin.
*/
Point point2DrawingArea(Point p, Contexte *pCtxt);

/**
   Affiche un point \a p dans une zone de dessin cairo \a cr comme un disque.
   
   @param cr le contexte CAIRO pour dessiner dans une zone de dessin.
   @param p un point dans la zone de dessin.
 */
void drawPoint(cairo_t *cr, Point p);

/**
 * Dessine la ligne entre p et q
 * @param cr le contexte cairo pour dessiner
 * @param p un Point
 * @param q un autre Points
 */
void drawLine(cairo_t *cr, Point p, Point q);

//-----------------------------------------------------------------------------
// Programme principal
//-----------------------------------------------------------------------------
int main(int argc, char *argv[]) {
    srand(time(NULL));
    Contexte context;
    TabPoints_init(&context.P);

    /* Passe les arguments à GTK, pour qu'il extrait ceux qui le concernent. */
    gtk_init(&argc, &argv);

    /* Crée une fenêtre. */
    creerIHM(&context);

    /* Rentre dans la boucle d'événements. */
    gtk_main();
    return 0;
}

gboolean realize_evt_reaction(GtkWidget *widget, gpointer data) { // force un événement "expose" juste derrière.
    gtk_widget_queue_draw(widget);
    return TRUE;
}

// c'est la réaction principale qui va redessiner tout.
gboolean expose_evt_reaction(GtkWidget *widget, GdkEventExpose *event, gpointer data) {
    Contexte *pCtxt = (Contexte *) data;
    TabPoints *ptrT = &(pCtxt->P);
    PilePoints *ptrP = &(pCtxt->pile);
    // c'est la structure qui permet d'afficher dans une zone de dessin
    // via Cairo
    cairo_t *cr = gdk_cairo_create(widget->window);
    cairo_set_source_rgb(cr, 1, 1, 1); // choisit le blanc.
    cairo_paint(cr); // remplit tout dans la couleur choisie.

    // Affiche tous les points en rouge.
    cairo_set_source_rgb(cr, 1, 0, 0);
    for (int i = 0; i < TabPoints_nb(ptrT) - 1; ++i) {
        if (PilePoints_estVide(ptrP)) {
            cairo_set_source_rgb(cr, 0, 0, 1);
            drawLine(cr, point2DrawingArea(TabPoints_get(ptrT, i), pCtxt),
                     point2DrawingArea(TabPoints_get(ptrT, i + 1), pCtxt));
            cairo_set_source_rgb(cr, 1, 0, 0);
        }
        drawPoint(cr, point2DrawingArea(TabPoints_get(ptrT, i), pCtxt));
    }

    if (!PilePoints_estVide(ptrP)) {
        cairo_set_source_rgb(cr, 0, 1, 0);
        Point a = point2DrawingArea(PilePoints_sommet(ptrP), pCtxt);
        Point r = a;
        PilePoints_depile(ptrP);
        while (!PilePoints_estVide(ptrP)) {
            Point b = point2DrawingArea(PilePoints_sommet(ptrP), pCtxt);
            drawLine(cr, a, b);

            PilePoints_depile(ptrP);
            a = b;
        }
        drawLine(cr, a, r);
    }

    // On a fini, on peut détruire la structure.
    cairo_destroy(cr);
    return TRUE;
}

Point point2DrawingArea(Point p, Contexte *pCtxt) {
    Point q;
    q.x = (p.x + 1.0) / 2.0 * pCtxt->width;
    q.y = (1.0 - p.y) / 2.0 * pCtxt->height;
    return q;
}

void drawPoint(cairo_t *cr, Point p) {
    cairo_arc(cr, p.x, p.y, 1.5, 0., 2.0 * 3.14159626);
    cairo_fill(cr);
}

void drawLine(cairo_t *cr, Point p, Point q) {
    cairo_move_to(cr, p.x, p.y);
    cairo_line_to(cr, q.x, q.y);
    cairo_stroke(cr);
}


/// Charge l'image donnée et crée l'interface.
GtkWidget *creerIHM(Contexte *pCtxt) {
    GtkWidget *window;
    GtkWidget *vbox1;
    GtkWidget *vbox2;
    GtkWidget *hbox1;
    GtkWidget *button_quit;
    GtkWidget *button_disk_random;
    GtkWidget *button_losange_random;
    GtkWidget *button_graham;
    GtkWidget *button_jarvis;

    /* Crée une fenêtre. */
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    // Crée un conteneur horitzontal box.
    hbox1 = gtk_hbox_new(FALSE, 10);
    // Crée deux conteneurs vertical box.
    vbox1 = gtk_vbox_new(FALSE, 10);
    vbox2 = gtk_vbox_new(FALSE, 10);
    // Crée une zone de dessin
    pCtxt->drawing_area = gtk_drawing_area_new();
    pCtxt->width = 500;
    pCtxt->height = 500;
    gtk_widget_set_size_request(pCtxt->drawing_area, pCtxt->width, pCtxt->height);
    // Crée le pixbuf source et le pixbuf destination
    gtk_container_add(GTK_CONTAINER(hbox1), pCtxt->drawing_area);
    // ... votre zone de dessin s'appelle ici "drawing_area"
    g_signal_connect(G_OBJECT(pCtxt->drawing_area), "realize",
                     G_CALLBACK(realize_evt_reaction), pCtxt);
    g_signal_connect(G_OBJECT(pCtxt->drawing_area), "expose_event",
                     G_CALLBACK(expose_evt_reaction), pCtxt);

    // Rajoute le 2eme vbox dans le conteneur hbox (pour mettre les boutons sélecteur de points
    gtk_container_add(GTK_CONTAINER(hbox1), vbox2);
    // Crée les boutons de sélection "source"/"destination".
    button_disk_random = gtk_button_new_with_label("Points aléatoires dans disque");
    // Connecte la réaction gtk_main_quit à l'événement "clic" sur ce bouton.
    g_signal_connect(button_disk_random, "clicked",
                     G_CALLBACK(diskRandom),
                     pCtxt);
    // Créé le label pour le nombre de points totaux
    pCtxt->points_label = gtk_label_new("0 sommets");
    // Créé l'entrée texte pour le nombre de points à rajouter
    pCtxt->points_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(pCtxt->points_entry), "100");
    // Créé le bouton losange
    button_losange_random = gtk_button_new_with_label("Points aléatoire dans losange");
    // Connecte la réaction losangeRandom à l'évenement 'click'
    g_signal_connect(button_losange_random, "clicked",
                     G_CALLBACK(losangeRandom),
                     pCtxt);
    // Crée le label pour le nombre de sommets
    pCtxt->sommet_label = gtk_label_new("");
    // Crée le label pour le temps d'algo
    pCtxt->time_label = gtk_label_new("");
    // Créé le bouton graham
    button_graham = gtk_button_new_with_label("Enveloppe convexe (Graham)");
    // Connecte la réaction graham à l'évenement 'click'
    g_signal_connect(button_graham, "clicked",
                     G_CALLBACK(graham),
                     pCtxt);
    // Créé le bouton jarvis
    button_jarvis = gtk_button_new_with_label("Enveloppe convexe (Jarvis)");
    // Connecte la réaction jarvis à l'évenement 'click'
    g_signal_connect(button_jarvis, "clicked",
                     G_CALLBACK(jarvis),
                     pCtxt);
    // Ajoute les éléments à la vue
    gtk_container_add(GTK_CONTAINER(vbox2), button_disk_random);
    gtk_container_add(GTK_CONTAINER(vbox2), button_losange_random);
    gtk_container_add(GTK_CONTAINER(vbox2), pCtxt->points_label);
    gtk_container_add(GTK_CONTAINER(vbox2), pCtxt->points_entry);
    gtk_container_add(GTK_CONTAINER(vbox2), pCtxt->sommet_label);
    gtk_container_add(GTK_CONTAINER(vbox2), pCtxt->time_label);
    gtk_container_add(GTK_CONTAINER(vbox2), button_graham);
    gtk_container_add(GTK_CONTAINER(vbox2), button_jarvis);
    // Crée le bouton quitter.
    button_quit = gtk_button_new_with_label("Quitter");
    // Connecte la réaction gtk_main_quit à l'événement "clic" sur ce bouton.
    g_signal_connect(button_quit, "clicked",
                     G_CALLBACK(gtk_main_quit),
                     NULL);
    // Rajoute tout dans le conteneur vbox.
    gtk_container_add(GTK_CONTAINER(vbox1), hbox1);
    gtk_container_add(GTK_CONTAINER(vbox1), button_quit);
    // Rajoute la vbox  dans le conteneur window.
    gtk_container_add(GTK_CONTAINER(window), vbox1);

    // Rend tout visible
    gtk_widget_show_all(window);

    return window;
}

gboolean diskRandom(GtkWidget *widget, gpointer data) {
    Contexte *pCtxt = (Contexte *) data;
    TabPoints *ptrP = &(pCtxt->P);
    int nbPoints;
    char *str = (char *) gtk_entry_get_text(GTK_ENTRY(pCtxt->points_entry));
    sscanf(str, "%d", &nbPoints);
    printf("diskRandom %d\n", nbPoints);
    for (int i = 0; i < nbPoints; ++i) {
        Point p;
        do {
            p.x = 2.0 * (rand() / (double) RAND_MAX) - 1.0;
            p.y = 2.0 * (rand() / (double) RAND_MAX) - 1.0;
        } while ((p.x * p.x + p.y * p.y) > 1.0);
        TabPoints_ajoute(ptrP, p);
    }
    gtk_widget_queue_draw(pCtxt->drawing_area);

    char nb[100];
    sprintf(nb, "%d points", ptrP->nb);
    gtk_label_set_text(GTK_LABEL(pCtxt->points_label), nb);

    return TRUE;
}

gboolean losangeRandom(GtkWidget *widget, gpointer data) {
    Contexte *pCtxt = (Contexte *) data;
    TabPoints *ptrP = &(pCtxt->P);
    int nbPoints;
    char *str = (char *) gtk_entry_get_text(GTK_ENTRY(pCtxt->points_entry));
    sscanf(str, "%d", &nbPoints);
    printf("losangeRandom %d\n", nbPoints);
    for (int i = 0; i < nbPoints; ++i) {
        Point p;
        p.x = 2.0 * (rand() / (double) RAND_MAX) - 1.0;
        p.y = 2.0 * (rand() / (double) RAND_MAX) - 1.0;

        double x = (p.x + p.y) * (sqrt(2.0) / 4.0);
        double y = (-p.x + p.y) * (sqrt(2.0) / 4.0);
        p.x = x;
        p.y = y;

        TabPoints_ajoute(ptrP, p);
    }
    gtk_widget_queue_draw(pCtxt->drawing_area);

    char nb[100];
    sprintf(nb, "%d", ptrP->nb);
    gtk_label_set_text(GTK_LABEL(pCtxt->points_label), nb);

    return TRUE;
}

gboolean graham(GtkWidget *widget, gpointer data) {
    Contexte *pCtxt = (Contexte *) data;
    TabPoints *ptrT = &(pCtxt->P);
    PilePoints *ptrP = &(pCtxt->pile);

    printf("Graham\n");

    struct timespec start;
    clock_gettime(CLOCK_REALTIME, &start);
    int index = TabPoints_indexBasGauche(ptrT);
    TabPoints_echange(ptrT, index, 0);
    TabPoints_triSelonT0(ptrT);

    PilePoints_init(ptrP);
    PilePoints_empile(ptrP, TabPoints_get(ptrT, 0));
    PilePoints_empile(ptrP, TabPoints_get(ptrT, 1));

    for (int i = 2; i < TabPoints_nb(ptrT); ++i) {
        while (!estAGauche(PilePoints_deuxiemeSommet(ptrP), PilePoints_sommet(ptrP), TabPoints_get(ptrT, i))) {
            PilePoints_depile(ptrP);
        }
        PilePoints_empile(ptrP, TabPoints_get(ptrT, i));
    }

    char nb[100];
    sprintf(nb, "%d sommets", ptrP->nb);
    gtk_label_set_text(GTK_LABEL(pCtxt->sommet_label), nb);

    struct timespec current;
    clock_gettime(CLOCK_REALTIME, &current);
    double t = ((double) (current.tv_sec - start.tv_sec) * 1000
                + (double) (current.tv_nsec - start.tv_nsec) / 1000000.0);
    char nb2[100];
    sprintf(nb2, "%lf ms", t);
    gtk_label_set_text(GTK_LABEL(pCtxt->time_label), nb2);

    gtk_widget_queue_draw(pCtxt->drawing_area);

    return TRUE;
}

gboolean jarvis(GtkWidget *widget, gpointer data) {
    Contexte *pCtxt = (Contexte *) data;
    TabPoints *ptrT = &(pCtxt->P);
    PilePoints *ptrP = &(pCtxt->pile);

    printf("Jarvis\n");

    int index = TabPoints_indexBasGauche(ptrT);
    Point point0 = TabPoints_get(ptrT, index);
    Point pointA = point0;
    PilePoints_init(ptrP);
    do {
        PilePoints_empile(ptrP, pointA);

        int minI = 0;
        Point p = TabPoints_get(ptrT, minI);
        double minA = angle(&point0, &p);
        for (int i = 1; i < TabPoints_nb(ptrT); ++i) {
            p = TabPoints_get(ptrT, i);
            if (p.x != pointA.x && p.y != pointA.y) {
                double a = angle(&point0, &p);
                if (a < minA) {
                    minA = a;
                    minI = i;
                }
            }
        }
        pointA = TabPoints_get(ptrT, minI);
        printf("%d\n", PilePoints_nb(ptrP));
    } while (pointA.x != point0.x && pointA.y != point0.y);

    gtk_widget_queue_draw(pCtxt->drawing_area);

    return TRUE;
}
