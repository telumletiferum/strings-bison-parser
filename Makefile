CC = gcc
LEX = flex
YACC = bison

LEX_SRC = lexer.l
YACC_SRC = parser.y
YACC_HEADER = parser.tab.h
YACC_BISON = parser.tab.c

INTERPRETER_SRC = interpret.c
GRAPH_SRC = graph.c

LEX_OUT = lex.yy.c

INTERPRETER_BIN = interpreter
GRAPH_BIN = graph

all: $(INTERPRETER_BIN) $(GRAPH_BIN)

$(LEX_OUT): $(LEX_SRC)
	$(LEX) $(LEX_SRC)

$(YACC_BISON) $(YACC_HEADER): $(YACC_SRC)
	$(YACC) -d $(YACC_SRC)

$(INTERPRETER_BIN): $(LEX_OUT) $(YACC_BISON) $(YACC_HEADER) $(INTERPRETER_SRC) node.h
	$(CC) -o $@ $(LEX_OUT) $(YACC_BISON) $(INTERPRETER_SRC) -lfl

$(GRAPH_BIN): $(LEX_OUT) $(YACC_BISON) $(YACC_HEADER) $(GRAPH_SRC) node.h
	$(CC) -o $@ $(LEX_OUT) $(YACC_BISON) $(GRAPH_SRC) -lfl

clean:
	rm -f $(LEX_OUT) $(YACC_BISON) $(YACC_HEADER) $(INTERPRETER_BIN) $(GRAPH_BIN) ast.dot ast.png

.PHONY: all clean
