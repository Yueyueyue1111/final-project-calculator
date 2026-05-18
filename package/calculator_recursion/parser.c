#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "codeGen.h"

static BTNode* assign_expr(void);
static BTNode* or_expr(void);
static BTNode* xor_expr(void);
static BTNode* and_expr(void);
static BTNode* addsub_expr(void);
static BTNode* muldiv_expr(void);
static BTNode* unary_expr(void);
static BTNode* factor(void);

int sbcount = 0;
Symbol table[TBLSIZE];
int next_addr = 12;

void initTable(void) {
    strcpy(table[0].name, "x");
    table[0].addr = 0;
    strcpy(table[1].name, "y");
    table[1].addr = 4;
    strcpy(table[2].name, "z");
    table[2].addr = 8;
    sbcount = 3;
}

// int getval(char *str) {
//     int i = 0;

//     for (i = 0; i < sbcount; i++)
//         if (strcmp(str, table[i].name) == 0)
//             return table[i].addr;

//     if (sbcount >= TBLSIZE)
//         error(RUNOUT);
    
//     strcpy(table[sbcount].name, str);
//     table[sbcount].addr = 0;
//     sbcount++;
//     return 0;
// }

int find_var_addr(char *str){
    for(int i=0;i<sbcount;i++){
        if(strcmp(str, table[i].name)==0){
            return table[i].addr;
        }
    }
    return -1;
}

//declare new
int create_var(char *str){
    if(sbcount>=TBLSIZE) error(RUNOUT);
    strcpy(table[sbcount].name,str);
    table[sbcount].addr=next_addr;
    next_addr+=4;
    sbcount++;
    return table[sbcount-1].addr;
}

int setval(char *str, int val) {
    int i = 0;

    for (i = 0; i < sbcount; i++) {
        if (strcmp(str, table[i].name) == 0) {
            table[i].addr = val;
            return val;
        }
    }

    if (sbcount >= TBLSIZE)
        error(RUNOUT);
    
    strcpy(table[sbcount].name, str);
    table[sbcount].addr = val;
    sbcount++;
    return val;
}

BTNode *makeNode(TokenSet tok, const char *lexe) {
    BTNode *node = (BTNode*)malloc(sizeof(BTNode));
    strcpy(node->lexeme, lexe);
    node->data = tok;
    node->val = 0;
    node->left = NULL;
    node->right = NULL;
    return node;
}

void freeTree(BTNode *root) {
    if (root != NULL) {
        freeTree(root->left);
        freeTree(root->right);
        free(root);
    }
}

// factor := INT | ADDSUB INT |
//		   	 ID  | ADDSUB ID  | 
//		   	 ID ASSIGN expr |
//		   	 LPAREN expr RPAREN |
//		   	 ADDSUB LPAREN expr RPAREN
// BTNode *factor(void) {
//     BTNode *retp = NULL, *left = NULL;

//     if (match(INT)) {
//         retp = makeNode(INT, getLexeme());
//         advance();
//     } else if (match(ID)) {
//         left = makeNode(ID, getLexeme());
//         advance();
//         if (!match(ASSIGN)) {
//             retp = left;
//         } else {
//             retp = makeNode(ASSIGN, getLexeme());
//             advance();
//             retp->left = left;
//             retp->right = expr();
//         }
//     } else if (match(ADDSUB)) {
//         retp = makeNode(ADDSUB, getLexeme());
//         retp->left = makeNode(INT, "0");
//         advance();
//         if (match(INT)) {
//             retp->right = makeNode(INT, getLexeme());
//             advance();
//         } else if (match(ID)) {
//             retp->right = makeNode(ID, getLexeme());
//             advance();
//         } else if (match(LPAREN)) {
//             advance();
//             retp->right = expr();
//             if (match(RPAREN))
//                 advance();
//             else
//                 error(MISPAREN);
//         } else {
//             error(NOTNUMID);
//         }
//     } else if (match(LPAREN)) {
//         advance();
//         retp = expr();
//         if (match(RPAREN))
//             advance();
//         else
//             error(MISPAREN);
//     } else {
//         error(NOTNUMID);
//     }
//     return retp;
// }

// // term := factor term_tail
// BTNode *term(void) {
//     BTNode *node = factor();
//     return term_tail(node);
// }


// // term_tail := MULDIV factor term_tail | NiL
// BTNode *term_tail(BTNode *left) {
//     BTNode *node = NULL;

//     if (match(MULDIV)) {
//         node = makeNode(MULDIV, getLexeme());
//         advance();
//         node->left = left;
//         node->right = factor();
//         return term_tail(node);
//     } else {
//         return left;
//     }
// }

// // expr := term expr_tail
// BTNode *expr(void) {
//     BTNode *node = term();
//     return expr_tail(node);
// }

// // expr_tail := ADDSUB term expr_tail | NiL
BTNode *expr(void){
    return assign_expr();
}

static BTNode *assign_expr(void){
    BTNode *node=or_expr();
    if(match(ASSIGN) || match(ADDSUB_ASSIGN)){
        if(node->data != ID){
            error(NOTLVAL);
        }

        create_var(node->lexeme);
        TokenSet op_type=(match(ASSIGN))? ASSIGN:ADDSUB_ASSIGN;
        BTNode *temp=makeNode(op_type, getLexeme());
        advance();

        temp->left=node;
        temp->right=assign_expr();
        node=temp;
    }
    return node;
}

static BTNode *or_expr(void){
    BTNode *node=xor_expr();
    while(match(OR)){
        BTNode *temp=makeNode(OR,getLexeme());
        advance();
        temp->left=node;
        temp->right=xor_expr();
        node=temp;
    }
    return node;
}

static BTNode *xor_expr(void){
    BTNode *node=and_expr();
    while(match(XOR)){
        BTNode *temp=makeNode(XOR,getLexeme());
        advance();
        temp->left=node;
        temp->right=and_expr();
        node=temp;
    }
    return node;
}

static BTNode *and_expr(void){
    BTNode *node=addsub_expr();
    while(match(AND)){
        BTNode *temp=makeNode(AND,getLexeme());
        advance();
        temp->left=node;
        temp->right=addsub_expr();
        node=temp;
    }
    return node;
}

static BTNode *addsub_expr(void){
    BTNode *node=muldiv_expr();
    while(match(ADDSUB)){
        BTNode *temp=makeNode(ADDSUB,getLexeme());
        advance();
        temp->left=node;
        temp->right=muldiv_expr();
        node=temp;
    }
    return node;
}

int is_constant_tree(BTNode *root, int *result) {
    if (root == NULL) return 0;
    if (root->data == INT) {
        *result = atoi(root->lexeme);
        return 1;
    }
    if (root->data == ID || root->data == INCDEC) {
        return 0; // 只要含有變數或自增減，就不是純常數樹
    }

    int left_val = 0, right_val = 0;
    // 遞迴檢查左右子樹
    if (is_constant_tree(root->left, &left_val) && is_constant_tree(root->right, &right_val)) {
        if (root->data == ADDSUB) {
            if (strcmp(root->lexeme, "+") == 0) *result = left_val + right_val;
            else *result = left_val - right_val;
            return 1;
        } else if (root->data == MULDIV) {
            if (strcmp(root->lexeme, "*") == 0) {
                *result = left_val * right_val;
            } else {
                if (right_val == 0) return 0; // 巢狀的除以零讓它在後續處理
                *result = left_val / right_val;
            }
            return 1;
        } else if (root->data == AND) { *result = left_val & right_val; return 1; }
        else if (root->data == OR)  { *result = left_val | right_val; return 1; }
        else if (root->data == XOR) { *result = left_val ^ right_val; return 1; }
    }
    return 0;
}

static BTNode *muldiv_expr(void) {
    BTNode *node = unary_expr();
    while (match(MULDIV)) {
        TokenSet op_type = (strcmp(getLexeme(), "*") == 0) ? MULDIV : MULDIV; // 判斷符號
        char op_lex[MAXLEN];
        strcpy(op_lex, getLexeme());

        BTNode *temp = makeNode(MULDIV, op_lex);
        advance();
        temp->left = node;
        temp->right = unary_expr(); // 解析出右子樹
        
        // 👈 在這裡進行 parser 階段的防守！
        if (strcmp(op_lex, "/") == 0) {
            int right_value = 0;
            // 如果右子樹整棵都是純常數，且算出來是 0，當場抓包報錯！
            if (is_constant_tree(temp->right, &right_value) && right_value == 0) {
                error(DIVZERO); // 噴出 EXIT 1 並結束程式
            }
        }
        node = temp;
    }
    return node;
}

static BTNode *unary_expr(void) {
    if (match(ADDSUB)) {
        char op_lex[MAXLEN];
        strcpy(op_lex, getLexeme());
        
        if (strcmp(op_lex, "-") == 0) {
            BTNode *temp = makeNode(ADDSUB, "-");
            advance(); // 跳過 "-"
            temp->left = makeNode(INT, "0");
            temp->right = unary_expr(); 
            return temp;
        } else {
            advance();
            return unary_expr();
        }
    }
    return factor();
}

// factor := INT | ADDSUB INT |
//		   	 ID  | ADDSUB ID  | 
//		   	 ID ASSIGN expr |
//		   	 LPAREN expr RPAREN |
//		   	 ADDSUB LPAREN expr RPAREN
static BTNode *factor(void){
    BTNode *rept=NULL;

    if(match(INT)){
        rept=makeNode(INT,getLexeme());
        advance();
    }else if(match(ID)){
        char id_lex[MAXLEN];
        strcpy(id_lex, getLexeme());
        rept = makeNode(ID, id_lex);
        advance();
        
        if (!match(ASSIGN) && !match(ADDSUB_ASSIGN)) {
            if (find_var_addr(id_lex) == -1) {
                error(NOTFOUND); // 未宣告先使用，報錯結束
            }
        }
    }else if(match(INCDEC)){
        rept=makeNode(INCDEC,getLexeme());
        advance();
        if(match(ID)){
            if(find_var_addr(getLexeme())==-1){
                error(NOTFOUND);
            }
            rept->left=makeNode(ID,getLexeme());
            advance();
        }else{
            error(NOTLVAL);
        }
    }else if(match(LPAREN)){
        advance();
        rept=expr();
        if(match(RPAREN)){
            advance();
        }else{
            error(MISPAREN);
        }
    }else{
        error(SYNTAXERR);
    }
    return rept;
}

// statement := ENDFILE | END | expr END
void statement(void) {
    BTNode *retp = NULL;

    if (match(ENDFILE)) {
        exit(0);
    } else if (match(END)) {
        //printf(">> ");
        advance();
    } else {
        retp = expr();
        if (match(END)) {
            evaluateTree_And_printPrefix(retp,0);
            printf("EXIT 0\n");
            freeTree(retp);
            advance();
        } else {
            error(SYNTAXERR);
        }
    }
}

void err(ErrorType errorNum) {
    if (PRINTERR) {
        fprintf(stderr, "error: ");
        switch (errorNum) {
            case MISPAREN:
                fprintf(stderr, "mismatched parenthesis\n");
                break;
            case NOTNUMID:
                fprintf(stderr, "number or identifier expected\n");
                break;
            case NOTFOUND:
                fprintf(stderr, "variable not defined\n");
                break;
            case RUNOUT:
                fprintf(stderr, "out of memory\n");
                break;
            case NOTLVAL:
                fprintf(stderr, "lvalue required as an operand\n");
                break;
            case DIVZERO:
                fprintf(stderr, "divide by constant zero\n");
                break;
            case SYNTAXERR:
                fprintf(stderr, "syntax error\n");
                break;
            default:
                fprintf(stderr, "undefined error\n");
                break;
        }
    }
    printf("EXIT 1\n");
    exit(0);
}
