typedef enum
{
    typeCon,
    typeId,
    typeOper
} nodeEnum;

/* constants */
typedef struct
{
    char *val; /* for string values */
} conNodeType;

/* identifiers */
typedef struct
{
    int i; /* index into symbol table */
} idNodeType;

/* operators */
typedef struct
{
    int oper;           /* operator */
    int nops;           /* number of operands */
    struct nodeTypeTag *op[1]; /* operands (expandable) */
} oprNodeType;

typedef struct nodeTypeTag
{
    nodeEnum type; /* type of node */
    union
    {
        conNodeType con; /* constants */
        idNodeType id;   /* identifiers */
        oprNodeType opr; /* operators */
    };
} nodeType;

extern char *sym[52]; /* symbol table */
