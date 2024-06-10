#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "node.h"
#include "parser.tab.h"

extern FILE *yyout;
extern FILE *yyin;


char* remove_substring(char* str, char* substr) {
    char *p, *temp;
    size_t len = strlen(str);
    size_t sublen = strlen(substr);

    if (sublen == 0) return strdup(str);

    temp = malloc(len + 1);
    if (!temp) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    temp[0] = '\0';

    while ((p = strstr(str, substr)) != NULL) {
        strncat(temp, str, p - str);
        str = p + sublen;
    }
    strcat(temp, str);

    return temp;
}

char* increment_string(char* str) {
    char* result = strdup(str);
    if (result[0] != '\0') {
        result[0]++;
    }
    return result;
}

char* decrement_string(char* str) {
    char* result = strdup(str);
    if (result[0] != '\0') {
        result[0]--;
    }
    return result;
}

char* toggle_case(char* str) {
    char* result = strdup(str);
    for (char* p = result; *p; ++p) {
        if (islower(*p)) {
            *p = toupper(*p);
        } else if (isupper(*p)) {
            *p = tolower(*p);
        }
    }
    return result;
}

char* repeat_string(char* str, int num) {
    int len = strlen(str);
    char* result = malloc(len * num + 1);
    result[0] = '\0';
    for (int i = 0; i < num; i++) {
        strcat(result, str);
    }
    return result;
}

char* compare_length(char* str1, char* str2, int op) {
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    int result;

    switch (op) {
        case LT: result = len1 < len2; break;
        case GT: result = len1 > len2; break;
        case LE: result = len1 <= len2; break;
        case GE: result = len1 >= len2; break;
        default: result = 0; break;
    }

    return strdup(result ? "true" : "false");
}

char* compare_strings(char* str1, char* str2, int op) {
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    int result;

    if (len1 != len2) {
        result = (op == NEQ); // If lengths are different, NEQ is true, EQ is false
    } else {
        result = (op == EQ) ? (strcmp(str1, str2) == 0) : (strcmp(str1, str2) != 0);
    }

    return strdup(result ? "true" : "false");
}

char* first_n_chars(char* str, int n) {
    if (n > strlen(str)) n = strlen(str);
    char* result = malloc(n + 1);
    strncpy(result, str, n);
    result[n] = '\0';
    return result;
}

char* last_n_chars(char* str, int n) {
    int len = strlen(str);
    if (n > len) n = len;
    char* result = malloc(n + 1);
    strncpy(result, str + len - n, n);
    result[n] = '\0';
    return result;
}

char *ex(nodeType *p) {
    char *result, *left, *right;
    int len;

    if (!p) return 0;
    switch (p->type) {
        case typeCon: return p->con.val; // Constant value
        case typeId: return sym[p->id.i]; // Identifier value
        case typeOper:
            switch (p->opr.oper) {
                case PRINT: 
                    fprintf(yyout, "%s\n", ex(p->opr.op[0])); 
                    return 0;
                case '=':
                    sym[p->opr.op[0]->id.i] = strdup(ex(p->opr.op[1]));
                    return 0;
                case '+':
                    left = ex(p->opr.op[0]);
                    right = ex(p->opr.op[1]);
                    result = malloc(strlen(left) + strlen(right) + 1);
                    strcpy(result, left);
                    strcat(result, right);
                    return result;
                case '-':
                    left = ex(p->opr.op[0]);
                    right = ex(p->opr.op[1]);
                    result = remove_substring(left, right);
                    return result;
                case '*':
                    left = ex(p->opr.op[0]);
                    right = ex(p->opr.op[1]);
                    len = atoi(right); // Convert string to integer
                    result = repeat_string(left, len);
                    return result;
                case '#':
                    left = ex(p->opr.op[0]);
                    right = ex(p->opr.op[1]);
                    len = atoi(right); // Convert string to integer
                    result = first_n_chars(left, len);
                    return result;
                case '/':
                    left = ex(p->opr.op[0]);
                    right = ex(p->opr.op[1]);
                    len = atoi(right); // Convert string to integer
                    result = last_n_chars(left, len);
                    return result;
                case LT:
                case GT:
                case LE:
                case GE:
                    left = ex(p->opr.op[0]);
                    right = ex(p->opr.op[1]);
                    result = compare_length(left, right, p->opr.oper);
                    return result;
                case EQ:
                case NEQ:
                    left = ex(p->opr.op[0]);
                    right = ex(p->opr.op[1]);
                    result = compare_strings(left, right, p->opr.oper);
                    return result;
                case INC:
                    left = ex(p->opr.op[0]);
                    result = increment_string(left);
                    if (p->opr.op[0]->type == typeId) {
                        sym[p->opr.op[0]->id.i] = result; // Update symbol table
                    }
                    return result;
                case DEC:
                    left = ex(p->opr.op[0]);
                    result = decrement_string(left);
                    if (p->opr.op[0]->type == typeId) {
                        sym[p->opr.op[0]->id.i] = result; // Update symbol table
                    }
                    return result;
                case 'P': /* Post-increment */
                    left = ex(p->opr.op[0]);
                    result = strdup(left);
                    if (p->opr.op[0]->type == typeId) {
                        sym[p->opr.op[0]->id.i] = increment_string(left);
                    }
                    return result;
                case 'Q': /* Post-decrement */
                    left = ex(p->opr.op[0]);
                    result = strdup(left);
                    if (p->opr.op[0]->type == typeId) {
                        sym[p->opr.op[0]->id.i] = decrement_string(left);
                    }
                    return result;
                case '~': /* Unary length */
                    left = ex(p->opr.op[0]);
                    len = strlen(left);
                    result = malloc(12);  // Enough space for an integer and null terminator
                    sprintf(result, "%d", len);
                    return result;
                case '^': /* Unary case toggle */
                    left = ex(p->opr.op[0]);
                    result = toggle_case(left);
                    return result;
                case ',':
                    ex(p->opr.op[0]);
                    ex(p->opr.op[1]);
                    return 0;
            }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            perror(argv[1]);
            return 1;
        }
    }

    if (argc > 2) {
        yyout = fopen(argv[2], "w");
        if (!yyout) {
            perror(argv[2]);
            return 1;
        }
    } else {
        yyout = stdout;
    }

    yyparse();

    if (yyin && yyin != stdin) fclose(yyin);
    if (yyout && yyout != stdout) fclose(yyout);

    return 0;
}