#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "logique.h"

//fonction qui renvoie la taille du plus grand paramètre donné en argument
int max_length_arg (const char**argv,int argc){
  int m = 0;
  for(int i = 0; i < argc; i++){
    if(strlen(argv[i])>m) m=strlen(argv[i]);
  }
  return m;
}

//fonction qui analyse les arguments donnés en paramètre
void extract_information(const char**argv,int argc,char**algo,char**heuristic,char**filename) {
  for(int i = 1; i < argc-1; i++) {
    if(strcmp(argv[i],"-algo")==0 && i < argc-2){
      strcpy(*algo,argv[i+1]);
    }else if(strcmp(argv[i],"-min")==0){
      strcpy(*heuristic,"min");
    }else if(strcmp(argv[i],"-max")==0){
      strcpy(*heuristic,"max");
    }else if(strcmp(argv[i],"-first")==0){
      strcpy(*heuristic,"first");
    }else if(strcmp(argv[i],"-rand")==0){
      strcpy(*heuristic,"rand");
    }
  }
  strcpy(*filename,argv[argc-1]);
}

int main(int argc, char const *argv[]) {
  int s = max_length_arg(argv,argc);
  char*algo = (char*)malloc((s+1)*sizeof(char));
  char*heuristic = (char*)malloc((s+1)*sizeof(char));
  char*filename = (char*)malloc((s+1)*sizeof(char));
  extract_information(argv,argc,&algo,&heuristic,&filename);//on determine quel algorithme et heuristique utiliser
  exe(filename,algo,heuristic);//on affiche si la fnc est satisifiable(et son modele) ou si elle ne l'est pas
  free(algo);
  free(heuristic);
  free(filename);
  return 0;
}
