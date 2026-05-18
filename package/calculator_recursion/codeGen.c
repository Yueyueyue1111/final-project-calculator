#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codeGen.h"

extern int find_var_addr(char *str);

int evaluateTree_And_printPrefix(BTNode *root, int reg){
    if(root==NULL)return -1;
    
    if(root->data==INT){
        printf("MOV r%d, %s\n",reg, root->lexeme);
        return reg;
    }

    if(root->data==ID){
        int addr=find_var_addr(root->lexeme);
        printf("MOV r%d [%d]\n",reg, addr);
        return reg;
    }

    if(root->data==INCDEC){
        int addr=find_var_addr(root->left->lexeme);
        printf("MOV r%d [%d]\n",reg,addr);
        int constant_reg=reg+1;
        printf("MOC r%d 1\n",constant_reg);
        if(strcmp(root->lexeme, "++")==0){
            printf("ADD r%d r%d\n",reg, constant_reg);
        }else{
            printf("SUB r%d r%d\n",reg, constant_reg);
        }
        printf("MOV [%d] r%d\n",addr, reg);
        return reg;
    }

    if(root->data==ASSIGN||root->data==ADDSUB_ASSIGN){
        int addr=find_var_addr(root->left->lexeme);

        int r_reg=evaluateTree_And_printPrefix(root->right,reg);
        if(root->data==ASSIGN){
            printf("MOV [%d] r%d\n",addr,r_reg);
        }else{
            int l_reg=r_reg+1;
            printf("MOV r%d, [%d]\n",l_reg,addr);
            if(strcmp(root->lexeme, "+=")==0){
                printf("ADD r%d r%d\n",l_reg, r_reg);
            }else{
                printf("SUB r%d r%d\n",l_reg, r_reg);
            }
            printf("MOV [%d] r%d\n",addr, l_reg);
            printf("MOV r%d r%d",r_reg, l_reg);
        }
        return r_reg;
    }

    int left_reg=evaluateTree_And_printPrefix(root->left,reg);
    int right_reg=evaluateTree_And_printPrefix(root->right,reg+1);
    if(root->data==ADDSUB){
        if(strcmp(root->lexeme,"+")==0){
            printf("ADD r%d r%d\n", left_reg, right_reg);
        }else{
            printf("SUB r%d r%d\n",left_reg, right_reg);
        }
    }else if(root->data==MULDIV){
        if(strcmp(root->lexeme,"*")==0){
            printf("MUL r%d r%d\n",left_reg, right_reg);
        }else{
            printf("DIV r%d r%d\n", left_reg, right_reg);
        }
    }else if(root->data==AND){
        printf("AND r%d r%d\n",left_reg, right_reg);
    }else if(root->data==OR){
        printf("OR r%d r%d\n",left_reg, right_reg);
    }else if(root->data==XOR){
        printf("XOR r%d r%d\n",left_reg, right_reg);
    }
    return left_reg;
}

// int evaluateTree(BTNode *root) {
//     int retval = 0, lv = 0, rv = 0;

//     if (root != NULL) {
//         switch (root->data) {
//             case ID:
//                 retval = getval(root->lexeme);
//                 break;
//             case INT:
//                 retval = atoi(root->lexeme);
//                 break;
//             case ASSIGN:
//                 rv = evaluateTree(root->right);
//                 retval = setval(root->left->lexeme, rv);
//                 break;
//             case ADDSUB:
//             case MULDIV:
//                 lv = evaluateTree(root->left);
//                 rv = evaluateTree(root->right);
//                 if (strcmp(root->lexeme, "+") == 0) {
//                     retval = lv + rv;
//                 } else if (strcmp(root->lexeme, "-") == 0) {
//                     retval = lv - rv;
//                 } else if (strcmp(root->lexeme, "*") == 0) {
//                     retval = lv * rv;
//                 } else if (strcmp(root->lexeme, "/") == 0) {
//                     if (rv == 0)
//                         error(DIVZERO);
//                     retval = lv / rv;
//                 }
//                 break;
//             default:
//                 retval = 0;
//         }
//     }
//     return retval;
// }

// void printPrefix(BTNode *root) {
//     if (root != NULL) {
//         printf("%s ", root->lexeme);
//         printPrefix(root->left);
//         printPrefix(root->right);
//     }
// }
