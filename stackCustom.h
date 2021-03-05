// Picior Catalin 312CB
// Fisierul contine declaratiile structurilor
// si functiilor necesare implementarii unei stive
// care retine adresele de memorie a unor elemente

#include <stdlib.h>
#include <string.h>
#ifndef _STACK_H_
#define _STACK_H_

typedef struct celstc {
  struct celstc* urm;
  void* info;
} TCelStC, *ACelStC;

typedef struct stackC {
  ACelStC vf;
} TStackC, *AStC;

#define VF(a) (((AStC)(a))->vf)

typedef void (*funElibEl)(void*);

void* InitS();
int VidaS(void* a);
int Push(void* a, void* ae);
int Pop(void* a, void** ae);
void ResetS(void* a);
void DistrS(void** aa);

#endif /* _STACK_H_ */