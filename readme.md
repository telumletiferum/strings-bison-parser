# A string based bison parser

This project has been made for the formal languages and translators course ("LFT" in romanian). It is basically a C based parser that support various operators for strings.

# Compiling and running the program

Let's get the important things out of the way.
For this program to work you obviously need to
have `flex`, `bison` and `gcc` installed.
This usually is not a problem since most linux
distributions come with `flex` and `bison`
preinstalled since they are part of the **GNU Project**.
`gcc` is also usually installed, but if it's not you need to install it.

Another requirement just for the **Abstract Syntax Tree (AST)** representation is that you need to have the utility called `dot`. You can install it by installing the `graphviz` package on your distro.

That is all in terms of requirements, now on how to run the program (I will add a Makefile later, so this becomes easier):

1. `bison -d parser.y` (This will generate the header and c files for the parser)
2. `flex lexer.l` (will genereate the lex.yy.c file which will be needed for lexical analysis)
3. `gcc -o parser_interpret parser.tab.c lex.yy.c interpret.c -lfl` (this will generate the executable `parser_interpret` which will be the interpretative version of parsing code)

The executable accepts two arguments, `<input_file>` and `<output_file`, if no arguments are given `stdin` is taken as input

For the graph version you would need to run the following

1. Generate bison and flex files like in the interpretative version
2. `gcc -o parser_graph parser.tab.c lex.yy.c graph.c -lfl`

The graph version take in one positional argument and that is `<input file>` and it will output both the .dot file for the graphs and a png with all the graphs generated from each line

Alternatively you can run the `make` command to build both versions, `make interpreter` just for the interpreter version, `make graph` just for the graph version. `make clean` removes all the generated files.

# Specs/Syntax

Like I said the syntax is relatively simple being quite similar to C/C++ syntax

## Variable declaration:

A variable is declared much like in C:

`a="abc";` This assigns the string literal `abc` to variable `a`. Like I said pretty self-explanatory

Variables can also take on the values of other variables. Like this:

```
a="abc";
b=a;
```

this piece of code will assign the value of variable a to variable b;

To note is that variables can be only single uppercase or lowercase character since that was the requirement of the project

## Print statement

As anyone could've guessed print just prints out the value of an expression, for example

```
a="abc";
b=a;
print b;
```

will print out to our `stdout` the value `abc`.

Print can be used in conjuction with any kind of expression (like the ones discussed after this)

## String concatenation

String concatenation is also done very simylarly to C using the `+` operator

```
a="abc";
b=a+"de";
print b;
```

will print out `abcde`

## String substraction

String substraction works with the `-` operator and is of form `<str1> - <str2>` and what it will do is, it will remove **ALL** the instances of `<str2>` from `<str1>` (if there are any)

```
a="abc";
b=a-"b";
print b;
```

will print `ac`

## Case operator

The case operator is `^` and is of form `^<str>` and it will return `<str>` with all it's characters switched in case (i.e lowercase become uppercase and vice-versa)

```
a="aBc";
b=^a;
print b;
```

will print `AbC`

## Length operator

The length operator is `~` and is of form `~<str>` and it will return the length that the string has with spaces

```
a="hello world";
print ~a;
```

will print `11`

## Increment and decrement

The increment and increment operator work much the same like in C and have the same form, i.e can be postfixed and prefixed with all the same quirks. They are of form `++<str>` and will increment the first character of the string

```
a="abc";
a++;
print a;
```

will print `bbc`;

## Beginning and End char operators

The language also has two other operators `#` and `/`, of form `<str>#<num>`. In the case of `#` it will return the first `<num>` characters of the string, while `/` will return the last `<num>` characters

```
a="abc";
print a#2;
print a/1;
```

will print `ab` and `c` respectively

## Comparators

The comparison operators are the same like in any other language, where it will `<,>,<=,>=` will compare the string lexico-graphically, i.e length, and `==,!=` will compare lexico-graphically and content wise the strings. It will return `true` if the condition is met, else `false`

```
a="abc";
b="abcd";
print a<b;
```

will print `true`
