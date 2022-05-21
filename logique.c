#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "Type/types.h"
#include "Type/fnc.h"
#include "Type/clause.h"
#include "Type/litteral.h"
#include "analyse.h"
#include "logique.h"

//fonction qui determine le nombre de clause contenant le litteral x
int nb_clause_with_x(fnc f,litteral x){
  int res = 0;
  for(int i = 0; i < f->len; i++){
    for(int j = 0; j < f->c[i]->len; j++){
      if(f->c[i]->l[j] == x){
        res++;
        break;
      }
    }
  }
  return res;
}

//remplit le premier tableau avec le deuxieme
void concat(int*l1,int*ind,int*l2,int len2) {
  for(int i = *ind; i < *ind+len2; i++) {
    l1[i]=l2[i - *ind];
  }
  *ind = *ind + len2;
}

//fonction vérifiant si la clause vide apparait dans fnc f
bool mem_vide (fnc f){
  for(int i = 0; i < f->len; i++) {
    if(f->c[i]->len == 0) return true;
  }
  return false;
}

//fonction qui cherche une clause unitaire renvoie 0 si il n'y en a pas
litteral unit_clause(fnc f){
  for(int i = 0; i < f->len; i++){
    if(f->c[i]->len == 1) return f->c[i]->l[0];
  }
  return 0;
}

//fonction qui détermine si le littéral x est pr&sent dans une liste de littéraux
bool mem(litteral x,int*l,int len){
  for(int i = 0; i < len; i++){
    if(l[i]==x) return true;
  }
  return false;
}

//------------------ choix du litteral -----------------------------------------
//renvoie le littéral le plus présent dans une fnc
litteral max_apparition(fnc f){
  int ind=0,max=0;
  int*watch = (int*)malloc(2*(f->nb_lit)*sizeof(int));
  litteral res;
  for(int i = 0; i < f->len; i++){
    for(int j = 0; j < f->c[i]->len; j++){
      litteral x = f->c[i]->l[j];
      if(!mem(x,watch,ind)){//on regarde si on a déjà compter le nombre de litteraux x
        watch[ind]=x;
        ind++;
        int cmp = 1;
        for(int k = 0; k < f->len; k++){// on compte le nombre de fois qu'apparait x dans f
          for(int l = 0; l < f->c[k]->len; l++) {
            if(f->c[k]->l[l] == x) cmp++;
          }
        }
        if(cmp > max){//si c'est le maximum on le garde en mémoire
          res = x;
          max = cmp;
        }
      }
    }
  }
  free(watch);
  return res;
}

//renvoie un littéral apparaisant dans une clause de longeur minimale
litteral min_size_clause(fnc f){
  assert(f!=NULL && f->len != 0);
  int min = f->c[0]->len;
  litteral x = f->c[0]->l[0];
  for(int i = 1; i < f->len; i++){
    if(min > f->c[i]->len && f->c[i]->len != 0){
      min = f->c[i]->len;
      x=f->c[i]->l[0];
    }
  }
  return x;
}

//renvoie le premier littéral d'une fnc
litteral first(fnc f){
  assert(f!=NULL && f->len != 0);
  return f->c[0]->l[0];
}

//renvoie un littéral aléatoire d'une fnc
litteral random_lit_uni(fnc f){
  int ind=0;
  int*watch = (int*)malloc((f->nb_lit)*sizeof(int));
  //on creer une liste des litteraux pour ne les compter qu'une seule fois
  for(int i = 0; i < f->len; i++){
    for(int j = 0; j < f->c[i]->len; j++){
      litteral x = f->c[i]->l[j];
      if(!mem(abs_lit(x),watch,ind)){//on regarde si on a deja ajouter x a watch
        watch[ind]=abs_lit(x);
        ind++;
      }
    }
  }
  //on tire un littéral au hasard dans la liste
  srand(time(NULL));
  int i = rand() % ind;
  litteral res = watch[i];
  free(watch);
  return res;
}

//------------------------------------------------------------------------------

//supprime les clauses avec x et retire non x des clauses
fnc suppr(litteral x, fnc f){
  clause*c = (clause*)malloc((f->len - nb_clause_with_x(f,x))*sizeof(clause));
  int size_nf = 0;
  for(int i = 0; i < f->len; i++){
    litteral*l = (litteral*)malloc((f->c[i]->len)*sizeof(litteral));
    assert(l != NULL);
    int size_nc = 0;
    for (int j = 0; j < f->c[i]->len; j++){
      if(f->c[i]->l[j] == x){// le littéral x apparait dans la clause on ne la prend pas en compte
        size_nc = -1;
        free(l);
        break;
      }else if(abs_lit(f->c[i]->l[j]) != abs_lit(x)){//le litteral est ni x ni non x
        l[size_nc] = f->c[i]->l[j]; //on ajoute le litteral à la clause
        size_nc++;
      }
    }
    if(size_nc == 0){//on a une clause vide
      l[0] = 0;
      c[size_nf] = create_clause(l,0);
      size_nf++;
    }else if(size_nc != -1){//on créer et ajoute notre clause a notre fnc finale
      c[size_nf] = create_clause(l,size_nc);
      size_nf++;
    }
  }
  fnc nf = create_fnc(c,size_nf,f->nb_lit - 1);
  return nf;
}

//fonction qui renvoie un littéral pur s'il existe sinon renvoie 0
litteral pure_lit(fnc f){
  int ind=0;
  int*watch = (int*)malloc((f->nb_lit)*sizeof(int));
  for(int i = 0; i < f->len; i++){
    for(int j = 0; j < f->c[i]->len; j++){
      litteral x = f->c[i]->l[j];
      if(!mem(abs_lit(x),watch,ind)){//on regarde si on a deja testé avec le litteral x
        watch[ind]=abs_lit(x);
        ind++;
        //on regarde si la negation du litteral apparait dans la fnc
        for(int k = i; k < f->len; k++){
          for(int l = 0; l < f->c[k]->len; l++) {
            if(f->c[k]->l[l] == -x){x=0;break;}
          }
          if(x==0) break;
        }
        if(x!=0){//on a trouvé notre littéral pur
          free(watch);
          return x;
        }
      }
    }
  }
  free(watch);
  return 0;
}

//implémentation de l'algorithme de quine
bool quine (fnc f,litteral (*heuristic) (fnc),int**l,int*ind){
 //la liste et l'indice en parramètre permet de stocker une éventuelle solution
 if(f->len == 0){//on regarde si on a affaire à une fnc vide
   free_fnc(f);
   return true;
 }
 if(mem_vide(f)){//on regarde si la fnc contient une clause vide
   free_fnc(f);
   return false;
 }
 litteral x = (*heuristic)(f); //on choisit le littéral
 if(quine(suppr(x,f),heuristic,l,ind)){//on teste avec x
   free_fnc(f);
   //on ajoute les choix fait à notre liste pour pouvoir renvoyer un modele de f
   (*l)[*ind] = x;
   (*ind) = (*ind) + 1;
   return true;
 }
 bool res = quine(suppr(-x,f),heuristic,l,ind); //si avec x la fnc n'est pas satisfiable on teste avec non x
 if(res){
   //on ajoute les choix fait à notre liste pour pouvoir renvoyer un modele de f
   (*l)[*ind] = -x;
   (*ind) = (*ind) + 1;
 }
 free_fnc(f);
 return res;
}

//implémentation de dpll
bool dpll(fnc f,litteral (*heuristic) (fnc),int**l,int*ind){
  if(f->len == 0){
    free_fnc(f);
    return true;
  }
  if(mem_vide(f)){
    free_fnc(f);
    return false;
  }
  int*mem = (int*)malloc((f->nb_lit)*sizeof(int));
  int i = 0;
  assert(mem != NULL);
  litteral x;
  while((x = unit_clause(f)) != 0){//on supprime les clauses unitaires
    fnc nf = suppr(x,f);
    free_fnc(f);
    mem[i] = x;//stocke les choix fais pour le modèle
    i++;
    f = nf;
    //on regarde que l'on ait pas une fnc vide ou de clause vide
    if(f->len == 0){concat(*l,ind,mem,i);free_fnc(f);free(mem);return true;}
    if(mem_vide(f)){free(mem);free_fnc(f);return false;}
  }
  while((x = pure_lit(f)) != 0){//on suppprime les littéraux purs
    fnc nf = suppr(x,f);
    mem[i] = x;//stocke les choix fais pour le modèle
    i++;
    free_fnc(f);
    f = nf;
    //on regarde que l'on ait pas une fnc vide ou de clause vide
    if(f->len == 0){concat(*l,ind,mem,i);free_fnc(f);free(mem);return true;}
    if(mem_vide(f)){free(mem);free_fnc(f);return false;}
  }

  x = (*heuristic)(f);//on choisit un littéral
  if(dpll(suppr(x,f),heuristic,l,ind)){//on teste avec x
    //on ajoute les choix fait à notre liste pour pouvoir renvoyer un modele de f
    (*l)[*ind] = x;
    (*ind) = (*ind) + 1;
    concat(*l,ind,mem,i);
    free(mem);
    free_fnc(f);
    return true;
  }
  bool res = dpll(suppr(-x,f),heuristic,l,ind); //si avec x la fnc n'est pas satisfiable on teste avec non x
  if(res){
    //on ajoute les choix fait à notre liste pour pouvoir renvoyer un modele de f
    (*l)[*ind] = -x;
    (*ind) = (*ind) + 1;
    concat(*l,ind,mem,i);
  }
  free(mem);
  free_fnc(f);
  return res;
}

//fonction qui combine l'algorithme à utiliser et l'heuristique
//qui affiche si la fnc provenant de fichier est satisfiable ou non
//et si c'est le cas affiche un modèle de la fnc
void exe(char*fichier,char*algo,char*heur) {
  //on convertit le fichier en fnc
  fnc f = file_to_fnc(fichier);
  int*l = (int*)malloc((f->nb_lit)*sizeof(int));
  assert(l!=NULL);
  int ind = 0;
  bool res;
  litteral (*heuristic) (fnc) = NULL;
  //sélection de l'heuristique
  if(strcmp(heur,"first")==0) heuristic = &first;
  else if(strcmp(heur,"rand")==0) heuristic = &random_lit_uni;
  else if(strcmp(heur,"min")==0) heuristic = &min_size_clause;
  else heuristic = &max_apparition;
  //sélection de l'algo
  if(strcmp(algo,"quine")==0) res = quine(f,heuristic,&l,&ind);
  else res = dpll(f,heuristic,&l,&ind);
  //on exploite le résultat
  if(res){
    printf("s SATISFIABLE\n");
    for(int i = 0; i < ind; i++) {
      printf("v %d\n",l[i]);
    }
  }else printf("s UNSATISFIABLE\n");
  free(l);
}
