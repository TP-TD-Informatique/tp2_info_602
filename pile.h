#ifndef TP2_PILE_H
#define TP2_PILE_H

#include <stdlib.h>
#include <assert.h>
#include "points.h"

typedef struct SPilePoint {
    int taille;
    int nb;
    Point *points;
} PilePoints;

/**
 * Initialise la structure PilePoints avec une taille de 100
 * @param pile un pointeur vers une structure PilePoints
 */
void PilePoints_init(PilePoints *pile);

/**
 * Renvoie si la pile est vide
 * @param pile un pointeur vers une structure PilePoints
 * @return 1|0
 */
int PilePoints_estVide(PilePoints *pile);

/**
 * Empile p au sommet de la pile
 * @param pile un pointeur vers une structure PilePoints
 * @param p un pointeur vers un point
 */
void PilePoints_empile(PilePoints *pile, Point p);

/**
 * Dépile le sommet de la pile
 * @param pile un pointeur vers une structure PilePoints
 */
void PilePoints_depile(PilePoints *pile);

/**
 * Renvoie le sommet de la pile
 * @param pile un pointeur vers une structure PilePoints
 * @return un Point
 */
Point PilePoints_sommet(PilePoints *pile);

/**
 * Renvoie le Point qui est sous le sommet de la pile
 * @param pile un pointeur vers une structure PilePoints
 * @return un Point
 */
Point PilePoints_deuxiemeSommet(PilePoints *pile);

/**
 * Agrandi la taille max de la pile de 100
 * @param pile un pointeur vers une structure PilePoints
 */
void PilePoints_agrandir(PilePoints *pile);

/**
 * Indique que la pile n'est plus utilisée et libère la mémoire
 * @param pile un pointeur vers une structure PilePoints
 */
void PilePoints_termine(PilePoints *pile);

/**
 * Renvoie le nombre de points dans la pile
 * @param pile un pointeur vers une structure PilePoints
 * @return un entier >= 0
 */
int PilePoints_nb(PilePoints *pile);

#endif //TP2_PILE_H
