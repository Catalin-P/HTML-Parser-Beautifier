#include <stdio.h>
#include <stdlib.h>
#include "lib.h"
#include "stackCustom.h"

void scanAttrList(TArb root, char* name, char* value) {
  TAttr styles = root->info->style, aux;
  int newAttrname = 1;
  while (styles) {
    if (strcmp(styles->name, name) == 0) {
      newAttrname = 0;
      if (strcmp(styles->value, value) != 0) {
        free(styles->value);
        styles->value = strdup(value);
      }
    }
    styles = styles->next;
  }
  if (newAttrname) {
    styles = root->info->style;
    TAttr last = NULL;
    while (styles) {
      last = styles;
      styles = styles->next;
    }
    TAttr cell = (TAttr)malloc(sizeof(TNodAttr));
    cell->next = NULL;
    cell->name = strdup(name);
    cell->value = strdup(value);
    if (!last) {
      root->info->style = cell;
    } else {
      last->next = cell;
    }
  }
}
void appendToStyleList(TArb root, char* styles) {
  char* localCopy = strdup(styles);
  char* dummy = strdup(localCopy);
  removeWhitespaces(localCopy, dummy);
  char* name = strtok(localCopy, ":;");
  free(dummy);
  char* value = strtok(NULL, ":;");
  scanAttrList(root, name, value);
  while (name = strtok(NULL, ":;")) {
    value = strtok(NULL, ":;");
    scanAttrList(root, name, value);
  }
  free(localCopy);
}

void appendStyle(TArb root, char* selector, FILE* outFile) {
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
                "Append to style failed: no node found for selector %s!\n",
                copy);
        return;
      }
      for (j = 1; j < nrNodes; j++) {
        crtNode = crtNode->nextSibling;
        if (!crtNode) {
          fprintf(outFile,
                  "Append to style failed: no node found for selector %s!\n",
                  copy);
          return;
        }
      }
    }
    appendToStyleList(crtNode, newStyle);
    return;
  }
  if (selector[0] == '.') {
    char* compareToClass = selector + 1;
    int erasedSth = 0;
    appendClass(root, NULL, compareToClass, newStyle, &erasedSth);
    if (!erasedSth) {
      fprintf(outFile,
              "Append to style failed: no node found for selector %s!\n", copy);
    }
    return;
  }
  if (strstr(selector + 1, ".")) {
    char* tag = strtok(selector, ".");
    char* class = strtok(NULL, "\0");
    int erasedSth = 0;
    appendElemClass(root, NULL, tag, class, newStyle, &erasedSth);
    if (!erasedSth) {
      fprintf(outFile,
              "Append to style failed: no node found for selector %s!\n", copy);
    }
    return;
  }
  if (strstr(selector, ">")) {
    char* parentName = strtok(selector, ">");
    char* childName = strtok(NULL, "\0");
    int erasedSth = 0;
    appendParentChild(root, NULL, parentName, childName, newStyle, &erasedSth);
    if (!erasedSth) {
      fprintf(outFile,
              "Append to style failed: no node found for selector %s!\n", copy);
    }
    return;
  }
  if (strstr(selector, " ")) {
    char* elem1 = strtok(selector, " ");
    char* elem2 = strtok(NULL, "\0");
    int erasedSth = 0;
    appendAncestorChild(root, NULL, elem1, elem2, newStyle, &erasedSth);
    if (!erasedSth) {
      fprintf(outFile,
              "Append to style failed: no node found for selector %s!\n", copy);
    }
    return;
  }
  int erasedSth = 0;
  appendAllTags(root, NULL, selector, newStyle, &erasedSth);
  if (!erasedSth) {
    fprintf(outFile, "Append to style failed: no node found for selector %s!\n",
            copy);
  }
  return;
}

void appendClass(TArb root, TArb parent, char* class, char* styles,
                 int* erasedSth) {
  if (!root) {
    return;
  }
  appendClass(root->firstChild, root, class, styles, erasedSth);
  appendClass(root->nextSibling, parent, class, styles, erasedSth);
  TAttr crtAttr = root->info->otherAttributes;
  while (crtAttr) {
    if (strcmp(crtAttr->value, class) == 0) {
      appendToStyleList(root, styles);
      *erasedSth = 1;
      break;
    }
    crtAttr = crtAttr->next;
  }
}

void appendElemClass(TArb root, TArb parent, char* tag, char* class,
                     char* styles, int* erasedSth) {
  if (!root) {
    return;
  }
  appendElemClass(root->firstChild, root, tag, class, styles, erasedSth);
  appendElemClass(root->nextSibling, parent, tag, class, styles, erasedSth);
  if (strcmp(root->info->type, tag) == 0) {
    TAttr crtAttr = root->info->otherAttributes;
    while (crtAttr) {
      if (strcmp(crtAttr->value, class) == 0) {
        appendToStyleList(root, styles);
        *erasedSth = 1;
        break;
      }
      crtAttr = crtAttr->next;
    }
  }
}

void appendParentChild(TArb root, TArb parent, char* parentName,
                       char* childName, char* styles, int* erasedSth) {
  if (!root) {
    return;
  }
  appendParentChild(root->firstChild, root, parentName, childName, styles,
                    erasedSth);
  appendParentChild(root->nextSibling, parent, parentName, childName, styles,
                    erasedSth);
  if (parent) {
    if (strcmp(parent->info->type, parentName) == 0) {
      TArb fc = parent->firstChild, last = NULL;
      while (fc) {
        if (strcmp(fc->info->type, childName) == 0) {
          appendToStyleList(fc, styles);
          *erasedSth = 1;
          break;
        }
        last = fc;
        fc = fc->nextSibling;
      }
    }
  }
}

void appendAllTags(TArb root, TArb parent, char* tag, char* styles,
                   int* erasedSth) {
  if (!root) {
    return;
  }
  appendAllTags(root->firstChild, root, tag, styles, erasedSth);
  appendAllTags(root->nextSibling, parent, tag, styles, erasedSth);
  if (strcmp(root->info->type, tag) == 0) {
    appendToStyleList(root, styles);
    *erasedSth = 1;
  }
}

void appendAncestorChild(TArb root, TArb parent, char* ancestorName,
                         char* childName, char* styles, int* erasedSth) {
  if (!root) {
    return;
  }
  appendAncestorChild(root->firstChild, root->firstChild, ancestorName,
                      childName, styles, erasedSth);
  appendAncestorChild(root->nextSibling, parent, ancestorName, childName,
                      styles, erasedSth);
  if (strcmp(root->info->type, ancestorName) == 0) {
    appendAllTags(root, parent, childName, styles, erasedSth);
  }
}