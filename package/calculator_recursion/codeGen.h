#ifndef __CODEGEN__
#define __CODEGEN__

#include "parser.h"

int evaluateTree_And_printPrefix(BTNode *root, int reg);

// Evaluate the syntax tree
extern int evaluateTree(BTNode *root);

// Print the syntax tree in prefix
extern void printPrefix(BTNode *root);

#endif // __CODEGEN__
