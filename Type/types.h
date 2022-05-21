#ifndef _TYPES_H
#define _TYPES_H
// Déclaration des types litteral clause et fnc
typedef int litteral;
typedef struct{litteral*l;int len;}*clause;
typedef struct{clause*c;int len;int nb_lit;}*fnc;
#endif
