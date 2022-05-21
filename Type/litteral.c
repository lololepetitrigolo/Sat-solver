#include <stdlib.h>
#include <stdbool.h>
#include "types.h"
#include "litteral.h"

//renvoie la variavle associé à un litteral
int abs_lit (litteral l){
  if(l<0) return -l;
  return l;
}
