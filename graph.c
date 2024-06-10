#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "node.h"
#include "parser.tab.h"

extern char *sym[52]; // Use the correct type from node.h
extern FILE *yyin;

int idCounter = 1;
int graphCounter = 1;

void ex(nodeType *p);
int graphNode(nodeType *p, FILE *out);
void processInput(const char *inputFile);
char* escape_quotes(const char* str);

void ex(nodeType *p) {
    char filename[256];
    snprintf(filename, sizeof(filename), "ast_%d.dot", graphCounter);

    FILE *out = fopen(filename, "w");
    if (!out) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    fprintf(out, "subgraph cluster_%d {\n", graphCounter);
    fprintf(out, "label=\"Graph %d\";\n", graphCounter);
    fprintf(out, "labelloc=t;\n");
    fprintf(out, "node [shape=record];\n");

    graphNode(p, out);

    fprintf(out, "}\n");
    fclose(out);

    // Increment graph counter for the next graph
    graphCounter++;
}

int graphNode(nodeType *p, FILE *out) {
    if (!p) return 0;

    int myId = idCounter++;
    switch (p->type) {
        case typeCon:
            fprintf(out, "node%d [label=\"{<f0> Const | <f1> %s}\"];\n", myId, escape_quotes(p->con.val));
            break;
        case typeId:
            fprintf(out, "node%d [label=\"{<f0> Id | <f1> %c}\"];\n", myId, p->id.i < 26 ? 'a' + p->id.i : 'A' + (p->id.i - 26));
            break;
        case typeOper:
            switch (p->opr.oper) {
                case PRINT: fprintf(out, "node%d [label=\"{<f0> Print}\"];\n", myId); break;
                case '=':   fprintf(out, "node%d [label=\"{<f0> =}\"];\n", myId); break;
                case '+':   fprintf(out, "node%d [label=\"{<f0> +}\"];\n", myId); break;
                case '-':   fprintf(out, "node%d [label=\"{<f0> -}\"];\n", myId); break;
                case '*':   fprintf(out, "node%d [label=\"{<f0> *}\"];\n", myId); break;
                case '/':   fprintf(out, "node%d [label=\"{<f0> /}\"];\n", myId); break;
                case '#':   fprintf(out, "node%d [label=\"{<f0> #}\"];\n", myId); break;
                case LT:   fprintf(out, "node%d [label=\"{<f0> LT}\"];\n", myId); break;
                case GT:   fprintf(out, "node%d [label=\"{<f0> GT}\"];\n", myId); break;
                case LE:    fprintf(out, "node%d [label=\"{<f0> LE}\"];\n", myId); break;
                case GE:    fprintf(out, "node%d [label=\"{<f0> GE}\"];\n", myId); break;
                case EQ:    fprintf(out, "node%d [label=\"{<f0> EQ}\"];\n", myId); break;
                case NEQ:   fprintf(out, "node%d [label=\"{<f0> NEQ}\"];\n", myId); break;
                case INC:   fprintf(out, "node%d [label=\"{<f0> ++}\"];\n", myId); break;
                case DEC:   fprintf(out, "node%d [label=\"{<f0> --}\"];\n", myId); break;
                case '~':   fprintf(out, "node%d [label=\"{<f0> ~}\"];\n", myId); break;
                case '^':   fprintf(out, "node%d [label=\"{<f0> ^}\"];\n", myId); break;
                case 'P':   fprintf(out, "node%d [label=\"{<f0> ++ (post)}\"];\n", myId); break;
                case 'Q':   fprintf(out, "node%d [label=\"{<f0> -- (post)}\"];\n", myId); break;
                default:    fprintf(out, "node%d [label=\"{<f0> op %c}\"];\n", myId, p->opr.oper); break;
            }

            for (int i = 0; i < p->opr.nops; i++) {
                int childId = graphNode(p->opr.op[i], out);
                fprintf(out, "node%d -> node%d;\n", myId, childId);
            }
            break;
    }
    return myId;
}

void processInput(const char *inputFile) {
    FILE *inFile = fopen(inputFile, "r");
    if (!inFile) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    char line[1024];
    while (fgets(line, sizeof(line), inFile)) {
        if (strlen(line) > 1) { // Skip empty lines
            yyin = fmemopen(line, strlen(line), "r");
            yyparse();
            fclose(yyin);
        }
    }

    fclose(inFile);

    // Combine individual DOT files into one
    FILE *combinedOut = fopen("combined_ast.dot", "w");
    if (!combinedOut) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    fprintf(combinedOut, "digraph Combined_AST {\n");
    fprintf(combinedOut, "node [shape=record];\n");

    for (int i = 1; i < graphCounter; i++) {
        char filename[256];
        snprintf(filename, sizeof(filename), "ast_%d.dot", i);

        FILE *in = fopen(filename, "r");
        if (!in) {
            perror("fopen");
            exit(EXIT_FAILURE);
        }

        char buffer[1024];
        while (fgets(buffer, sizeof(buffer), in)) {
            if (strstr(buffer, "subgraph") != NULL || strstr(buffer, "{") != NULL || strstr(buffer, "}") != NULL) {
                fputs(buffer, combinedOut);
            } else {
                fputs(buffer, combinedOut);
            }
        }
        fclose(in);
        remove(filename); // Remove individual DOT files after merging
    }

    fprintf(combinedOut, "}\n");
    fclose(combinedOut);

    // Generate the combined graph image
    system("dot -Tpng combined_ast.dot -o combined_ast.png");
}

char* escape_quotes(const char* str) {
    char* escaped_str = malloc(strlen(str) * 2 + 1); // Allocate enough space
    char* p = escaped_str;
    while (*str) {
        if (*str == '"') {
            *p++ = '\\';
        }
        *p++ = *str++;
    }
    *p = '\0';
    return escaped_str;
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        processInput(argv[1]);
    } else {
        fprintf(stderr, "Usage: %s <input file>\n", argv[0]);
        return 1;
    }

    return 0;
}
