#include <stdio.h>
#include <stdlib.h>
#include "lib.h"
#include "stackCustom.h"

void WriteNode(TArb node, FILE* outFile) {
  if (node->info->type) {
    fprintf(outFile, "<%s", node->info->type);
  }
  if (node->info->style) {
    fprintf(outFile, " style=\"");
    TAttr p = node->info->style;
    while (p) {
      if (p->next) {
        fprintf(outFile, "%s: %s; ", p->name, p->value);
      } else {
        fprintf(outFile, "%s: %s;\"", p->name, p->value);
      }
      p = p->next;
    }
  }
  if (node->info->otherAttributes) {
    TAttr q = node->info->otherAttributes;
    while (q) {
      fprintf(outFile, " %s=\"%s\"", q->name, q->value);
      q = q->next;
    }
  }
  if (node->info->isSelfClosing) {
    fprintf(outFile, "/>\n");
  } else {
    fprintf(outFile, ">\n");
  }
}

void format(TArb root, FILE* outFile, int tabNo) {
  if (root == NULL) {
    return;
  }
  int i;
  for (i = 0; i < tabNo; i++) {
    fprintf(outFile, "\t");
  }
  WriteNode(root, outFile);
  if (strlen(root->info->contents)) {
    for (i = 0; i <= tabNo; i++) {
      fprintf(outFile, "\t");
    }
    fprintf(outFile, "%s\n", root->info->contents);
  }
  if (root->firstChild) {
    format(root->firstChild, outFile, tabNo + 1);
  }
  if (!root->info->isSelfClosing) {
    for (i = 0; i < tabNo; i++) {
      fprintf(outFile, "\t");
    }
    fprintf(outFile, "</");
    fprintf(outFile, "%s", root->info->type);
    fprintf(outFile, ">\n");
  }
  if (root->nextSibling) {
    format(root->nextSibling, outFile, tabNo);
  }
}
