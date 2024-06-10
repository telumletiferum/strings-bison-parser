%{
#include "node.h"
#include "parser.tab.h"
#include <stdio.h>
#include <string.h>

int map_char_to_index(char c) {
    if (c >= 'a' && c <= 'z') {
        return c - 'a';  // Lowercase letters mapped to 0-25
    } else if (c >= 'A' && c <= 'Z') {
        return c - 'A' + 26;  // Uppercase letters mapped to 26-51
    }
    return -1;  // Invalid character
}

char* process_escape_sequences(char* str) {
    char* processed = malloc(strlen(str) + 1);
    char* src = str;
    char* dst = processed;
    while (*src) {
        if (*src == '\\') {
            src++;
            if (*src >= '0' && *src <= '7') {  // Octal escape sequence
                int octal_val = 0;
                for (int i = 0; i < 3 && *src >= '0' && *src <= '7'; i++) {
                    octal_val = (octal_val << 3) | (*src++ - '0');
                }
                *dst++ = (char)octal_val;
                src--;
            } else {
                switch (*src) {
                    case 'n': *dst++ = '\n'; break;
                    case 't': *dst++ = '\t'; break;
                    case '"': *dst++ = '\"'; break;
                    case '\\': *dst++ = '\\'; break;
                    default: *dst++ = '\\'; *dst++ = *src; break;
                }
            }
        } else {
            *dst++ = *src;
        }
        src++;
    }
    *dst = '\0';
    return processed;
}
%}


%%

[a-zA-Z]   { yylval.sIndex = map_char_to_index(yytext[0]); return VAR; } // single letter variables
\"([^\\\"]|\\.)*\" {
    yylval.iValue = strdup(yytext + 1);
    yylval.iValue[strlen(yylval.iValue) - 1] = '\0';
    char* processed_str = process_escape_sequences(yylval.iValue);
    free(yylval.iValue);  // Free the unprocessed string
    yylval.iValue = processed_str;
    return STRING;
} // double-quoted string with escape sequences processed
"print"    { return PRINT; }
"="        { return '='; }
"~"        { return '~'; }
";"        { return ';'; }
"^"        { return '^'; }
","        { return ','; }
"+"		   { return '+'; }
"-"        { return '-'; }
"++"       { return INC; }
"--"       { return DEC; }
"<"        { return LT; }
"<="       { return LE; }
">"        { return GT; }
">="       { return GE; }
"=="       { return EQ; }
"!="       { return NEQ; }
"#"        { return '#'; }
"/"        { return '/'; }
"("        { return '('; }
")"        { return ')'; }
[0-9]+     { yylval.iValue = strdup(yytext); return NUMBER; } // integer numbers
"*"        { return '*'; }
[ \t\n]+   { /* ignore whitespace */ }
.          { /* ignore unrecognized characters */ }

%%

int yywrap() {
    return 1;
}
