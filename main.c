#include <stdio.h>
#include <stdlib.h>
#include "lib.h"
#include "stackCustom.h"

int main(int argc, char* argv[]) {
  TArb root = readTree(argv[1]);
  char* line = (char*)malloc(MAX_LINE_LEN);
  FILE* input = fopen(argv[2], "r");
  FILE* out = fopen(argv[3], "a");
  int commandsNo = 0, i;
  fscanf(input, "%d", &commandsNo);
  fgetc(input);
  for (i = 0; i < commandsNo; i++) {
    fgets(line, MAX_LINE_LEN, input);
    line[strlen(line) - 1] = '\0';
    char* cmd = strtok(line, " \0");
    if (strcmp(cmd, "format") == 0) {
      format(root, out, 0);
      continue;
    }
    if (strcmp(cmd, "add") == 0) {
      addID(root, cmd, out);
      continue;
    }
    if (strcmp(cmd, "deleteRecursively") == 0) {
      recursiveDelete(root, cmd, out);
      continue;
    }
    if (strcmp(cmd, "overrideStyle") == 0) {
      overrideStyle(root, cmd, out);
      continue;
    }
    if (strcmp(cmd, "appendStyle") == 0) {
      appendStyle(root, cmd, out);
      continue;
    }
  }
  free(line);
  freeTree(&root);
  fclose(out);
  fclose(input);
  return 0;
}