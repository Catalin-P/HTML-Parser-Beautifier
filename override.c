#include <stdio.h>
#include <stdlib.h>
#include "lib.h"
#include "stackCustom.h"

void addToStyle(TArb node, char* styles) {
  char* localCopy = strdup(styles);
  char* dummy = strdup(localCopy);
  removeWhitespaces(localCopy, dummy);
  free(dummy);
  char* name = strtok(localCopy, ":;");
  char* value = strtok(NULL, ":;");
  TAttr cell = (TAttr)malloc(sizeof(TNodAttr));
  cell->next = NULL;
  cell->name = strdup(name);
  cell->value = strdup(value);
  node->info->style = cell;
  TAttr last = cell;
  while (name = strtok(NULL, ":;")) {
    value = strtok(NULL, ":;");
    cell = (TAttr)malloc(sizeof(TNodAttr));
    cell->next = NULL;
    cell->name = strdup(name);
    cell->value = strdup(value);
    last->next = cell;
    last = cell;
  }
  free(localCopy);
}

void overrideStyle(TArb root, char* selector, FILE* outFile) {
  selector = strtok(NULL, "\"");
  selector = strtok(NULL, "\"");
  int selectorLen = strlen(selector);
  char copy[selectorLen + 1];
  copy[0] = '\0';
  strcpy(copy, selector);
  char* newStyle = strtok(NULL, "\"");
  newStyle = strtok(NULL, "\"");
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
                "Override style failed: no node found for selector %s!\n",
                copy);
        return;
      }
      for (j = 1; j < nrNodes; j++) {
        crtNode = crtNode->nextSibling;
        if (!crtNode) {
          fprintf(outFile,
                  "Override style failed: no node found for selector %s!\n",
                  copy);
          return;
        }
      }
    }
    freeStyleList(crtNode);
    addToStyle(crtNode, newStyle);
    return;
  }
  if (selector[0] == '.') {
    char* compareToClass = selector + 1;
    int erasedSth = 0;
    overrideClass(root, NULL, compareToClass, newStyle, &erasedSth);
    if (!erasedSth) {
      fprintf(outFile,
              "Override style failed: no node found for selector %s!\n", copy);
    }
    return;
  }
  if (strstr(selector + 1, ".")) {
    char* tag = strtok(selector, ".");
    char* class = strtok(NULL, "\0");
    int erasedSth = 0;
    overrideElemClass(root, NULL, tag, class, newStyle, &erasedSth);
    if (!erasedSth) {
      fprintf(outFile,
              "Override style failed: no node found for selector %s!\n", copy);
    }
    return;
  }
  if (strstr(selector, ">")) {
    char* parentName = strtok(selector, ">");
    char* childName = strtok(NULL, "\0");
    int erasedSth = 0;
    overrideParentChild(root, NULL, parentName, childName, newStyle,
                        &erasedSth);
    if (!erasedSth) {
      fprintf(outFile,
              "Override style failed: no node found for selector %s!\n", copy);
    }
    return;
  }
  if (strstr(selector, " ")) {
    char* elem1 = strtok(selector, " ");
    char* elem2 = strtok(NULL, "\0");
    int erasedSth = 0;
    overrideAncestorChild(root, NULL, elem1, elem2, newStyle, &erasedSth);
    if (!erasedSth) {
      fprintf(outFile,
              "Override style failed: no node found for selector %s!\n", copy);
    }
    return;
  }
  int erasedSth = 0;
  overrideAllTags(root, selector, newStyle, &erasedSth);
  if (!erasedSth) {
    fprintf(outFile, "Override style failed: no node found for selector %s!\n",
            copy);
  }
  return;
}

void overrideClass(TArb root, TArb parent, char* class, char* styles,
                   int* erasedSth) {
  if (!root) {
    return;
  }
  overrideClass(root->firstChild, root, class, styles, erasedSth);
  overrideClass(root->nextSibling, parent, class, styles, erasedSth);
  TAttr crtAttr = root->info->otherAttributes;
  while (crtAttr) {
    if (strcmp(crtAttr->value, class) == 0) {
      freeStyleList(root);
      addToStyle(root, styles);
      *erasedSth = 1;
      break;
    }
    crtAttr = crtAttr->next;
  }
}

void overrideElemClass(TArb root, TArb parent, char* tag, char* class,
                       char* styles, int* erasedSth) {
  if (!root) {
    return;
  }
  overrideElemClass(root->firstChild, root, tag, class, styles, erasedSth);
  overrideElemClass(root->nextSibling, parent, tag, class, styles, erasedSth);
  if (strcmp(root->info->type, tag) == 0) {
    TAttr crtAttr = root->info->otherAttributes;
    while (crtAttr) {
      if (strcmp(crtAttr->value, class) == 0) {
        freeStyleList(root);
        addToStyle(root, styles);
        *erasedSth = 1;
        break;
      }
      crtAttr = crtAttr->next;
    }
  }
}

void overrideParentChild(TArb root, TArb parent, char* parentName,
                         char* childName, char* styles, int* erasedSth) {
  if (!root) {
    return;
  }
  overrideParentChild(root->firstChild, root, parentName, childName, styles,
                      erasedSth);
  overrideParentChild(root->nextSibling, parent, parentName, childName, styles,
                      erasedSth);
  if (parent) {
    if (strcmp(parent->info->type, parentName) == 0) {
      TArb fc = parent->firstChild, last = NULL;
      while (fc) {
        if (strcmp(fc->info->type, childName) == 0) {
          freeStyleList(fc);
          addToStyle(fc, styles);
          *erasedSth = 1;
          break;
        }
        last = fc;
        fc = fc->nextSibling;
      }
    }
  }
}

void overrideAllTags(TArb root, char* tag, char* styles, int* erasedSth) {
  if (!root) {
    return;
  }
  overrideAllTags(root->firstChild, tag, styles, erasedSth);
  overrideAllTags(root->nextSibling, tag, styles, erasedSth);
  if (strcmp(root->info->type, tag) == 0) {
    freeStyleList(root);
    addToStyle(root, styles);
    *erasedSth = 1;
  }
}

void overrideAncestorChild(TArb root, TArb parent, char* ancestorName,
                           char* childName, char* styles, int* erasedSth) {
  if (!root) {
    return;
  }
  overrideAncestorChild(root->firstChild, root->firstChild, ancestorName,
                        childName, styles, erasedSth);
  overrideAncestorChild(root->nextSibling, parent, ancestorName, childName,
                        styles, erasedSth);
  if (strcmp(root->info->type, ancestorName) == 0) {
    overrideAllTags(root, childName, styles, erasedSth);
  }
}