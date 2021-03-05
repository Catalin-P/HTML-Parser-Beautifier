#include <stdio.h>
#include <stdlib.h>
#include "lib.h"
#include "stackCustom.h"

void delRecClass(TArb root, TArb parent, char* class, int* erasedSth) {
  if (!root) {
    return;
  }
  delRecClass(root->firstChild, root, class, erasedSth);
  delRecClass(root->nextSibling, parent, class, erasedSth);
  TAttr crtAttr = root->info->otherAttributes;
  while (crtAttr) {
    if (strcmp(crtAttr->value, class) == 0) {
      if (root == parent->firstChild) {
        parent->firstChild = root->nextSibling;
      } else {
        TArb last = NULL, fc = parent->firstChild;
        while (fc) {
          last = fc;
          fc = fc->nextSibling;
          if (fc == root) {
            last->nextSibling = root->nextSibling;
            break;
          }
        }
      }
      TArb aux = root;
      aux->nextSibling = NULL;
      freeTree(&aux);
      *erasedSth = 1;
      break;
    }
    crtAttr = crtAttr->next;
  }
}
void delRecElemClass(TArb root, TArb parent, char* tag, char* class,
                     int* erasedSth) {
  if (!root) {
    return;
  }
  delRecElemClass(root->firstChild, root, tag, class, erasedSth);
  delRecElemClass(root->nextSibling, parent, tag, class, erasedSth);
  if (strcmp(root->info->type, tag) == 0) {
    TAttr crtAttr = root->info->otherAttributes;
    while (crtAttr) {
      if (strcmp(crtAttr->value, class) == 0) {
        if (root == parent->firstChild) {
          parent->firstChild = root->nextSibling;
        } else {
          TArb last = NULL, fc = parent->firstChild;
          while (fc) {
            last = fc;
            fc = fc->nextSibling;
            if (fc == root) {
              last->nextSibling = root->nextSibling;
              break;
            }
          }
        }
        TArb aux = root;
        aux->nextSibling = NULL;
        freeTree(&aux);
        *erasedSth = 1;
        break;
      }
      crtAttr = crtAttr->next;
    }
  }
}

void delRecParentChild(TArb root, TArb parent, char* parentName,
                       char* childName, int* erasedSth) {
  if (!root) {
    return;
  }
  delRecParentChild(root->firstChild, root, parentName, childName, erasedSth);
  delRecParentChild(root->nextSibling, parent, parentName, childName,
                    erasedSth);
  if (parent && root) {
    if (strcmp(parent->info->type, parentName) == 0) {
      TArb fc = parent->firstChild, last = NULL;
      while (fc) {
        if (strcmp(fc->info->type, childName) == 0) {
          if (parent->firstChild == fc) {
            parent->firstChild = fc->nextSibling;
          } else {
            last->nextSibling = fc->nextSibling;
          }
          TArb aux = fc;
          aux->nextSibling = NULL;
          freeTree(&aux);
          *erasedSth = 1;
          break;
        }
        last = fc;
        fc = fc->nextSibling;
      }
    }
  }
}

void delRecAllTags(TArb root, TArb parent, char* tag, int* erasedSth) {
  if (!root) {
    return;
  }
  delRecAllTags(root->firstChild, root, tag, erasedSth);
  delRecAllTags(root->nextSibling, parent, tag, erasedSth);
  if (strcmp(root->info->type, tag) == 0) {
    if (root == parent->firstChild) {
      parent->firstChild = root->nextSibling;
    } else {
      TArb last = NULL, fc = parent->firstChild;
      while (fc) {
        last = fc;
        fc = fc->nextSibling;
        if (fc == root) {
          last->nextSibling = root->nextSibling;
          break;
        }
      }
    }
    TArb aux = root;
    aux->nextSibling = NULL;
    freeTree(&aux);
    *erasedSth = 1;
  }
}

void delRecAncestorChild(TArb root, TArb parent, char* ancestorName,
                         char* childName, int* erasedSth) {
  if (!root) {
    return;
  }
  delRecAncestorChild(root->firstChild, root->firstChild, ancestorName,
                      childName, erasedSth);
  delRecAncestorChild(root->nextSibling, parent, ancestorName, childName,
                      erasedSth);
  if (strcmp(root->info->type, ancestorName) == 0) {
    delRecAllTags(root, parent, childName, erasedSth);
  }
}

void recursiveDelete(TArb root, char* selector, FILE* outFile) {
  selector = strtok(NULL, "\"");
  selector = strtok(NULL, "\"");
  int selectorLen = strlen(selector);
  char copy[selectorLen + 1];
  copy[0] = '\0';
  strcpy(copy, selector);
  if (selector[0] == '#') {
    TArb crtNode = root, parent = NULL;
    int IDLen = strlen(selector);
    char id[IDLen];
    id[0] = '\0';
    strcpy(id, selector + 1);
    int charToInt[IDLen], index = 0, i, j, nrNodes;
    char* crtLevel = strtok(id, ".\0");
    charToInt[index] = atoi(crtLevel);
    index++;
    while (crtLevel = strtok(NULL, ".\0")) {
      charToInt[index] = atoi(crtLevel);
      index++;
    }
    for (i = 0; i < index; i++) {
      nrNodes = charToInt[i];
      parent = crtNode;
      crtNode = crtNode->firstChild;
      if (!crtNode) {
        fprintf(outFile,
                "Delete recursively failed: no node found for selector %s!\n",
                copy);
        return;
      }
      for (j = 1; j < nrNodes; j++) {
        crtNode = crtNode->nextSibling;
        if (!crtNode) {
          fprintf(outFile,
                  "Delete recursively failed: no node found for selector %s!\n",
                  copy);
          return;
        }
      }
    }
    if (parent->firstChild == crtNode) {
      parent->firstChild = crtNode->nextSibling;
    } else {
      TArb fc = parent->firstChild, last = NULL;
      while (fc) {
        last = fc;
        fc = fc->nextSibling;
        if (fc == crtNode) {
          last->nextSibling = crtNode->nextSibling;
          break;
        }
      }
    }
    crtNode->nextSibling = NULL;
    freeTree(&crtNode);
    eraseAllIDs(root);
    char dummy[MAX_LINE_LEN];
    dummy[0] = '\0';
    getIDs(root, dummy);
    return;
  }
  if (selector[0] == '.') {
    char* compareToClass = selector + 1;
    int erasedSth = 0;
    delRecClass(root, NULL, compareToClass, &erasedSth);
    if (!erasedSth) {
      fprintf(outFile,
              "Delete recursively failed: no node found for selector %s!\n",
              copy);
    }
    eraseAllIDs(root);
    char dummy[MAX_LINE_LEN];
    dummy[0] = '\0';
    getIDs(root, dummy);
    return;
  }
  if (strstr(selector + 1, ".")) {
    char* tag = strtok(selector, ".");
    char* class = strtok(NULL, "\0");
    int erasedSth = 0;
    delRecElemClass(root, NULL, tag, class, &erasedSth);
    if (!erasedSth) {
      fprintf(outFile,
              "Delete recursively failed: no node found for selector %s!\n",
              copy);
    }
    eraseAllIDs(root);
    char dummy[MAX_LINE_LEN];
    dummy[0] = '\0';
    getIDs(root, dummy);
    return;
  }
  if (strstr(selector, ">")) {
    char* parentName = strtok(selector, ">");
    char* childName = strtok(NULL, "\0");
    int erasedSth = 0;
    delRecParentChild(root, NULL, parentName, childName, &erasedSth);
    if (!erasedSth) {
      fprintf(outFile,
              "Delete recursively failed: no node found for selector %s!\n",
              copy);
    }
    eraseAllIDs(root);
    char dummy[MAX_LINE_LEN];
    dummy[0] = '\0';
    getIDs(root, dummy);
    return;
  }
  if (strstr(selector, " ")) {
    char* elem1 = strtok(selector, " ");
    char* elem2 = strtok(NULL, "\0");
    int erasedSth = 0;
    delRecAncestorChild(root, NULL, elem1, elem2, &erasedSth);
    if (!erasedSth) {
      fprintf(outFile,
              "Delete recursively failed: no node found for selector %s!\n",
              copy);
    }
    eraseAllIDs(root);
    char dummy[MAX_LINE_LEN];
    dummy[0] = '\0';
    getIDs(root, dummy);
    return;
  }
  int erasedSth = 0;
  delRecAllTags(root, NULL, selector, &erasedSth);
  if (!erasedSth) {
    fprintf(outFile,
            "Delete recursively failed: no node found for selector %s!\n",
            copy);
  }
  eraseAllIDs(root);
  char dummy[MAX_LINE_LEN];
  dummy[0] = '\0';
  getIDs(root, dummy);
  return;
}