#include <stdio.h>
#include <stdlib.h>
#include "lib.h"
#include "stackCustom.h"

TArb allocTreeCell() {
  TArb cell = (TArb)malloc(sizeof(TNodArb));
  if (!cell) {
    return NULL;
  }
  cell->firstChild = NULL;
  cell->nextSibling = NULL;
  cell->info = (TInfo)malloc(sizeof(TNodInfo));
  if (!cell->info) {
    free(cell);
    return NULL;
  }
  cell->info->isSelfClosing = 0;
  cell->info->style = NULL;
  cell->info->otherAttributes = NULL;
  cell->info->contents = (char*)malloc(HTML_MAX_LEN * sizeof(char));
  if (!cell->info->contents) {
    free(cell->info);
    free(cell);
    return NULL;
  }
  cell->info->id = NULL;
  cell->info->type = (char*)malloc(HTML_MAX_LEN * sizeof(char));
  if (!cell->info->type) {
    free(cell->info->contents);
    free(cell->info);
    free(cell);
    return NULL;
  }
  cell->info->contents[0] = '\0';
  cell->info->type[0] = '\0';
  return cell;
}

void appendToContent(TArb node, char c) {
  char charToString[2];
  charToString[0] = c;
  charToString[1] = '\0';
  if (c != '<' && c != '\n' && c != '\t' && c != '\r') {
    strcat(node->info->contents, charToString);
  }
}

void appendToTagType(TArb node, char c) {
  char charToString[2];
  charToString[0] = c;
  charToString[1] = '\0';
  strcat(node->info->type, charToString);
}

void appendToAttribute(char* string, char c) {
  char charToString[2];
  charToString[0] = c;
  charToString[1] = '\0';
  strcat(string, charToString);
}

void insertInChildrenList(void* ancestorStack) {
  void* aux;
  int verif1 = Pop(ancestorStack, &aux);
  if (!verif1) {
    return;
  }
  TArb child = (TArb)aux;
  if (VidaS(ancestorStack)) {
    Push(ancestorStack, aux);
    return;
  } else {
    int verif2 = Pop(ancestorStack, &aux);
    if (!verif2) {
      return;
    }
    TArb parent = (TArb)aux;
    if (parent->firstChild == NULL) {
      parent->firstChild = child;
    } else {
      TArb firstChild = parent->firstChild;
      if (firstChild->nextSibling == NULL) {
        firstChild->nextSibling = child;
      } else {
        TArb lastChild = NULL, p = parent->firstChild;
        while (p) {
          lastChild = p;
          p = p->nextSibling;
        }
        lastChild->nextSibling = child;
      }
    }
    Push(ancestorStack, parent);
  }
}

void removeWhitespaces(char* dest, char* source) {
  int len = strlen(source);
  int index = 0, i;
  for (i = 0; i < len; i++) {
    if (!isspace(source[i])) {
      dest[index] = source[i];
      index++;
    }
  }
  dest[index] = '\0';
}

void insertNodeLast(TArb node, char* name, char* value, TAttr* last) {
  TAttr cell = (TAttr)malloc(sizeof(TNodAttr));
  if (!cell) {
    return;
  }
  cell->name = (char*)malloc(sizeof(char) * (strlen(name) + 1));
  if (!cell->name) {
    free(cell);
    return;
  }
  cell->value = (char*)malloc(sizeof(char) * (strlen(value) + 1));
  if (!cell->value) {
    free(cell->name);
    free(cell);
    return;
  }
  cell->next = NULL;
  removeWhitespaces(cell->name, name);
  removeWhitespaces(cell->value, value);
  if (!(*last)) {
    node->info->style = cell;
  } else {
    (*last)->next = cell;
  }
  (*last) = cell;
}

void insertAttributeCell(TArb node, char* name, char* value) {
  if (strcmp(name, "style") == 0) {
    TAttr last = NULL;
    char* token = strtok(value, ":;");
    char* crtName = token;
    token = strtok(NULL, ":;");
    char* crtValue = token;
    insertNodeLast(node, crtName, crtValue, &last);
    while (token) {
      token = strtok(NULL, ":;");
      if (!token) {
        break;
      }
      crtName = token;
      token = strtok(NULL, ":;");
      crtValue = token;
      insertNodeLast(node, crtName, crtValue, &last);
    }
  } else {
    TAttr cell = (TAttr)malloc(sizeof(TNodAttr));
    if (!cell) {
      return;
    }
    cell->name = (char*)malloc(sizeof(char) * (strlen(name) + 1));
    if (!cell->name) {
      free(cell);
      return;
    }
    cell->value = (char*)malloc(sizeof(char) * (strlen(value) + 1));
    if (!cell->value) {
      free(cell->name);
      free(cell);
      return;
    }
    cell->value[0] = '\0';
    strcpy(cell->value, value);
    cell->next = NULL;
    removeWhitespaces(cell->name, name);
    if (!node->info->otherAttributes) {
      node->info->otherAttributes = cell;
    } else {
      TAttr p = node->info->otherAttributes, last = NULL;
      while (p) {
        last = p;
        p = p->next;
      }
      last->next = cell;
    }
  }
  name[0] = '\0';
  value[0] = '\0';
}

TArb readTree(char* filename) {
  FILE* input = fopen(filename, "r");
  int c;
  void* ancestorStack = InitS();
  TArb crtNode = NULL;
  char *attrName, *attrValue;
  attrName = (char*)malloc(sizeof(char) * HTML_MAX_LEN), attrName[0] = '\0';
  attrValue = (char*)malloc(sizeof(char) * HTML_MAX_LEN), attrValue[0] = '\0';
  TParseState crtState = PARSE_CONTENTS;
  while ((c = fgetc(input)) != EOF) {
    crtState = Interpret(crtState, (char)c, ancestorStack, &crtNode, attrName,
                         attrValue);
  }
  fclose(input);
  void* aux;
  Pop(ancestorStack, &aux);
  DistrS(&ancestorStack);
  char* dummy = attrValue;
  dummy[0] = '\0';
  TArb root = (TArb)aux;
  getIDs(root, dummy);
  free(attrName);
  free(attrValue);
  return root;
}

void allocID(TArb node, int index, char* parentId, char* dummy) {
  sprintf(dummy, "%s.%d", parentId, index);
  node->info->id = (char*)malloc(strlen(dummy) + 1);
  node->info->id[0] = '\0';
  strcat(node->info->id, dummy);
  dummy[0] = '\0';
}

void getChildrenID(TArb node, TArb parent, TArb root, char* dummy) {
  if (node == NULL) {
    return;
  }
  if (parent != root) {
    TArb p = parent->firstChild;
    int index = 1;
    while (p) {
      if (p == node) {
        allocID(node, index, parent->info->id, dummy);
        break;
      }
      index++;
      p = p->nextSibling;
    }
  }
  getChildrenID(node->firstChild, node, root, dummy);
  getChildrenID(node->nextSibling, parent, root, dummy);
}
void getIDs(TArb root, char* dummy) {
  TArb head = root->firstChild;
  if (head) {
    head->info->id = (char*)malloc(2);
    head->info->id[0] = '\0';
    strcat(head->info->id, "1");
    TArb body = head->nextSibling;
    if (body) {
      body->info->id = (char*)malloc(2);
      body->info->id[0] = '\0';
      strcat(body->info->id, "2");
    }
  }
  getChildrenID(head, root, root, dummy);
}

void freeNodeInfo(TArb node) {
  TInfo info = node->info;
  if (info->id) {
    free(info->id);
    info->id = NULL;
  }
  if (info->contents) {
    free(info->contents);
    info->contents = NULL;
  }
  if (info->type) {
    free(info->type);
    info->type = NULL;
  }
  TAttr style = info->style, aux;
  while (style) {
    aux = style;
    style = style->next;
    free(aux->name);
    free(aux->value);
    free(aux);
  }
  info->style = NULL;
  TAttr others = info->otherAttributes;
  while (others) {
    aux = others;
    others = others->next;
    free(aux->value);
    free(aux->name);
    free(aux);
  }
  info->otherAttributes = NULL;
  free(node->info);
  node->info = NULL;
}

void freeTree(TArb* root) {
  TArb node = *root;
  if (!node) {
    return;
  }
  freeTree(&node->firstChild);
  freeTree(&node->nextSibling);
  node->firstChild = NULL;
  node->nextSibling = NULL;
  freeNodeInfo(node);
  free(*root);
  *root = NULL;
}
void addID(TArb root, char* cmd, FILE* outFile) {
  TArb crtNode = root;
  char* id = strtok(NULL, "=");
  id = strtok(NULL, " ");
  char* tag = strtok(NULL, "=");
  tag = strtok(NULL, "\0");
  char* tagType = strtok(tag, "<");
  tagType = strtok(NULL, ">");
  char* content = strtok(NULL, "<");
  int IDLen = strlen(id);
  char copy[IDLen + 1];
  copy[0] = '\0';
  strcpy(copy, id);
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
    crtNode = crtNode->firstChild;
    if (!crtNode) {
      fprintf(outFile, "Add tag failed: node with id %s not found!\n", copy);
      return;
    }
    for (j = 1; j < nrNodes; j++) {
      crtNode = crtNode->nextSibling;
      if (!crtNode) {
        fprintf(outFile, "Add tag failed: node with id %s not found!\n", copy);
        return;
      }
    }
  }
  char lastLevel[INT_MAX_LEN];
  lastLevel[0] = '\0';
  int lastNodeNo = 1;
  TArb last = NULL, firstChild = crtNode->firstChild, cell;
  while (firstChild) {
    last = firstChild;
    firstChild = firstChild->nextSibling;
    lastNodeNo++;
  }
  sprintf(lastLevel, ".%d", lastNodeNo);
  int newIDLen = strlen(lastLevel) + strlen(copy) + 1;
  cell = allocTreeCell();
  cell->info->id = (char*)malloc(newIDLen);
  cell->info->id[0] = '\0';
  strcat(cell->info->id, copy);
  strcat(cell->info->id, lastLevel);
  strcpy(cell->info->type, tagType);
  if (content[0] != '/') {
    for (i = strlen(content) - 1; i >= 0; i--) {
      if (isspace(content[i])) {
        content[i] = '\0';
      } else {
        break;
      }
    }
    for (i = 0; i < strlen(content); i++) {
      if (!isspace(content[i])) {
        break;
      }
    }
    strcpy(cell->info->contents, content + i);
  }
  if (last) {
    last->nextSibling = cell;
  } else {
    crtNode->firstChild = cell;
  }
}

void eraseAllIDs(TArb root) {
  if (root == NULL) {
    return;
  }
  eraseAllIDs(root->firstChild);
  eraseAllIDs(root->nextSibling);
  free(root->info->id);
  root->info->id = NULL;
}

void freeStyleList(TArb node) {
  TAttr style = node->info->style, aux;
  while (style) {
    aux = style;
    style = style->next;
    free(aux->value);
    free(aux->name);
    free(aux);
  }
  node->info->style = NULL;
}