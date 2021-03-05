#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "stackCustom.h"

#ifndef _LIB_H_
#define _LIB_H_

#define HTML_MAX_LEN 1024
#define INT_MAX_LEN 15
#define MAX_LINE_LEN 10241

typedef struct TNodAttr {
  char* name;
  char* value;
  struct TNodAttr* next;
} TNodAttr, *TAttr;

typedef struct TNodInfo {
  char* type;
  char* id;
  TAttr style;
  TAttr otherAttributes;
  int isSelfClosing;
  char* contents;
} TNodInfo, *TInfo;

typedef struct TNodArb {
  TInfo info;
  struct TNodArb* nextSibling;
  struct TNodArb* firstChild;
} TNodArb, *TArb;

typedef enum {
  PARSE_ERROR = 0,
  PARSE_CONTENTS = 1,
  PARSE_OPENING_BRACKET = 2,
  PARSE_TAG_TYPE = 3,
  PARSE_CLOSING_TAG = 4,
  PARSE_REST_OF_TAG = 5,
  PARSE_ATTRIBUTE_NAME = 6,
  PARSE_ATTRIBUTE_EQ = 7,
  PARSE_ATTRIBUTE_VALUE = 8,
  PARSE_SELF_CLOSING = 9,
} TParseState;

typedef void (*funDelete)(TArb);

TParseState Interpret(TParseState currentState, char c, void* ancestorStack,
                      TArb* crtNode, char* name, char* value);
TArb allocTreeCell();
void getIDs(TArb root, char* dummy);
void appendToContent(TArb node, char c);
void appendToTagType(TArb node, char c);
void insertInChildrenList(void* ancestorStack);
void appendToAttribute(char* string, char c);
void insertAttributeCell(TArb node, char* name, char* value);
void removeWhitespaces(char* dest, char* source);
void insertNodeLast(TArb node, char* name, char* value, TAttr* last);
TArb readTree(char* inputFile);
void allocID(TArb node, int index, char* parentId, char* dummy);
void getChildrenID(TArb node, TArb parent, TArb root, char* dummy);
void getIDs(TArb root, char* dummy);
void format(TArb root, FILE* outFile, int tabNo);
void WriteNode(TArb root, FILE* outFile);
void freeNodeInfo(TArb node);
void freeTree(TArb* root);
void addID(TArb root, char* cmd, FILE* outFile);
void recursiveDelete(TArb root, char* selector, FILE* outFile);
void eraseAllIDs(TArb root);
void delRecClass(TArb root, TArb parent, char* class, int* erasedSth);
void delRecElemClass(TArb root, TArb parent, char* tag, char* class,
                     int* erasedSth);
void delRecParentChild(TArb root, TArb parent, char* parentName,
                       char* childName, int* erasedSth);
void delRecAncestorChild(TArb root, TArb parent, char* ancestorName,
                         char* childName, int* erasedSth);
void delRecAllTags(TArb root, TArb parent, char* tag, int* erasedSth);
void overrideStyle(TArb root, char* selector, FILE* outFile);
void freeStyleList(TArb node);
void addToStyle(TArb node, char* styles);
void overrideClass(TArb root, TArb parent, char* class, char* styles,
                   int* erasedSth);
void overrideElemClass(TArb root, TArb parent, char* tag, char* class,
                       char* styles, int* erasedSth);
void overrideAllTags(TArb root, char* tag, char* styles, int* erasedSth);
void overrideParentChild(TArb root, TArb parent, char* parentName,
                         char* childName, char* styles, int* erasedSth);
void overrideAncestorChild(TArb root, TArb parent, char* ancestorName,
                           char* childName, char* styles, int* erasedSth);
void scanAttrList(TArb root, char* name, char* value);
void appendToStyleList(TArb root, char* styles);
void appendStyle(TArb root, char* selector, FILE* outFile);
void appendClass(TArb root, TArb parent, char* class, char* styles,
                 int* erasedSth);
void appendElemClass(TArb root, TArb parent, char* tag, char* class,
                     char* styles, int* erasedSth);
void appendParentChild(TArb root, TArb parent, char* parentName,
                       char* childName, char* styles, int* erasedSth);
void appendAllTags(TArb root, TArb parent, char* tag, char* styles,
                   int* erasedSth);
void appendAncestorChild(TArb root, TArb parent, char* ancestorName,
                         char* childName, char* styles, int* erasedSth);
#endif