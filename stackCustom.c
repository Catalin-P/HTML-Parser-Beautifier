// Picior Catalin 312CB
#include <stdlib.h>
#include <string.h>
#include "stackCustom.h"

// functie initializare stiva
// aproape identica cu functia aferenta cozii
void* InitS() {
  AStC s;
  s = (AStC)malloc(sizeof(TStackC));
  if (!s) return NULL;
  s->vf = NULL;
  return (void*)s;
}

// functie de inserare in sitva
// intoarce 1 daca s-a inserat cu succes si 0 daca nu
// aproape identica cu IntrQ
int Push(void* a, void* ae) {
  ACelStC new_top = (ACelStC)malloc(sizeof(TCelStC));
  if (!new_top) {
    return 0;
  }
  new_top->info = ae;
  // elementul este pus in "varful" stivei
  new_top->urm = VF(a);
  VF(a) = new_top;
  return 1;
}

// functie de verificare coada vida
// identica cu VidaQ
int VidaS(void* a) {
  if (VF(a)) {
    return 0;
  } else {
    return 1;
  }
}

// functie de extragere din stiva
// intoarce 1 daca s-a inserat cu succes si 0 daca nu
int Pop(void* a, void** ae) {
  // verificare stiva vida
  if (VidaS(a) == 1) {
    return 0;
  }
  *ae=VF(a)->info;
  ACelStC aux = VF(a);
  VF(a) = VF(a)->urm;
  free(aux);
  return 1;
}

// functie resetare stiva = stiva vida
void ResetS(void* a) {
  // iteram prin lista de celule
  ACelStC cells = VF(a), aux;
  while (cells) {
    // retinem cel curenta si continuam parcurgerea
    aux = cells;
    cells = cells->urm;
    // eliberam celula
    free(aux);
  }
  // stiva a devenit vida
  VF(a) = NULL;
}
// distrugere = reset + eliberarea elem TStack
void DistrS(void** aa) {
  ResetS(*aa);
  free(*aa);
  *aa = NULL;
}
