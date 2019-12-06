/**
* Name: Roman Sanchez
* Lab: CI_LISP
* Date: 11/13/18
**/

#include "ciLisp.h"

void yyerror(char *s) {
    fprintf(stderr, "\nERROR: %s\n", s);
    // note stderr that normally defaults to stdout, but can be redirected: ./src 2> src.log
    // CLion will display stderr in a different color from stdin and stdout
}

// Array of string values for operations.
// Must be in sync with funcs in the OPER_TYPE enum in order for resolveFunc to work.
char *funcNames[] = {
        "neg",
        "abs",
        "exp",
        "sqrt",
        "add",
        "sub",
        "mult",
        "div",
        "remainder",
        "log",
        "pow",
        "max",
        "min",
        "exp2",
        "cbrt",
        "hypot",
        "read",
        "rand",
        "print",
        "equal",
        "less",
        "greater",
        ""
};

OPER_TYPE resolveFunc(char *funcName)
{
    int i = 0;
    while (funcNames[i][0] != '\0')
    {
        if (strcmp(funcNames[i], funcName) == 0)
            return i;
        i++;
    }
    return CUSTOM_OPER;
}

NUM_TYPE resolveType(char *type)
{
    if(type == NULL)
        return INT_TYPE;
    else if (strcmp(type, "int") == 0)
        return INT_TYPE;
    else if (strcmp(type, "double") == 0)
        return DOUBLE_TYPE;

    yyerror("Error - faulty type");
    return INT_TYPE;
}

AST_NODE *attachSTtoAST(SYMBOL_TABLE_NODE *symbolTable, AST_NODE *s_expr)
{
    s_expr->symbolTable = symbolTable;
    return s_expr;
}

SYMBOL_TABLE_NODE *findSymbol(SYMBOL_TABLE_NODE *symbolTableNode, char *ident)
{
    while (symbolTableNode != NULL)
    {
        if ( strcmp(ident, symbolTableNode->ident) == 0 )
            return symbolTableNode;

        symbolTableNode = symbolTableNode->next;
    }
    return NULL;
}

SYMBOL_TABLE_NODE *addToSymbolTable(SYMBOL_TABLE_NODE *parent, SYMBOL_TABLE_NODE *stNode)
{
    SYMBOL_TABLE_NODE *node = findSymbol(parent, stNode->ident);
    if (node != NULL)
    {
        yyerror("The symbol already exists");
        return parent;
    }

    stNode->next = parent;
    return stNode;
}

// Called when an INT or DOUBLE token is encountered (see ciLisp.l and ciLisp.y).
// Creates an AST_NODE for the number.
// Sets the AST_NODE's type to number.
// Populates the value of the contained NUMBER_AST_NODE with the argument value.
// SEE: AST_NODE, NUM_AST_NODE, AST_NODE_TYPE.
AST_NODE *createNumberNode(double value, NUM_TYPE type)
{
    // allocate space for the fixed size and the variable part (union)
    AST_NODE *numberNode = calloc(1, sizeof(AST_NODE));
    if (numberNode == NULL)
    {
        yyerror("Memory allocation failed!\n");
        return NULL;
    }

    // TODO set the AST_NODE's type, assign values to contained NUM_AST_NODE : DONE
    numberNode->data.number.type = type;
    numberNode->data.number.value = value;
    return numberNode;
}

// Called when an f_expr is created (see ciLisp.y).
// Creates an AST_NODE for a function call.
// Sets the created AST_NODE's type to function.
// Populates the contained FUNC_AST_NODE with:
//      - An OPER_TYPE (the enum identifying the specific function being called)
//      - 2 AST_NODEs, the operands
// SEE: AST_NODE, FUNC_AST_NODE, AST_NODE_TYPE.
AST_NODE *createFunctionNode(char *funcName, AST_NODE *op1, AST_NODE *op2)
{
    // allocate space (or error)
    AST_NODE *functionNode = calloc(1, sizeof(AST_NODE));
    if (functionNode == NULL) {
        yyerror("out of memory");
        return NULL;
    }

    // TODO set the AST_NODE's type, populate contained FUNC_AST_NODE : DONE
    // NOTE: you do not need to populate the "ident" field unless the function is type CUSTOM_OPER.
    // When you do have a CUSTOM_OPER, you do NOT need to allocate and strcpy here.
    // The funcName will be a string identifier for which space should be allocated in the tokenizer.
    // For CUSTOM_OPER functions, you should simply assign the "ident" pointer to the passed in funcName.
    // For functions other than CUSTOM_OPER, you should free the funcName after you've assigned the OPER_TYPE.
    functionNode->type = FUNC_NODE_TYPE;
    functionNode->data.function.oper = resolveFunc(funcName);

    // set funcNode ident to funcName so your custom variable can store any name you want
    if (functionNode->data.function.oper == CUSTOM_OPER)
    {
        functionNode->data.function.ident = (char*) malloc(sizeof(funcName) + 1);
        strcpy(functionNode->data.function.ident, funcName);
    }
    // free funcName because we're done using it now
    free(funcName);

    // set the op1 to the func ast node struct
    functionNode->data.function.op1 = op1;
    functionNode->data.function.op2 = op2;

    if (functionNode->data.function.op1 != NULL)
        functionNode->data.function.op1->parent = functionNode;
    if(functionNode->data.function.op2 != NULL)
        functionNode->data.function.op2->parent = functionNode;

    return functionNode;
}

AST_NODE *createSymbolNode(char *ident)
{
    AST_NODE *symbolNode = calloc(1, sizeof(AST_NODE));
    if (symbolNode == NULL) {
        yyerror("Memory Allocation failed!");
        return NULL;
    }
    symbolNode->type = SYM_NODE_TYPE;
    symbolNode->data.symbol.ident = (char*) malloc(sizeof(ident));
    strcpy(symbolNode->data.symbol.ident, ident);
    free(ident);
    return symbolNode;
}

SYMBOL_TABLE_NODE *createSTNode(char *type, char *ident, AST_NODE *s_expr)
{
    AST_NODE *symbolTableNode = calloc(1, sizeof(SYMBOL_TABLE_NODE));
    symbolTableNode->type = SYM_NODE_TYPE;
    symbolTableNode->symbolTable = malloc(sizeof(SYMBOL_TABLE_NODE));
    symbolTableNode->symbolTable->ident = malloc(sizeof(char) * strlen(ident) + 1);
    strcpy(symbolTableNode->symbolTable->ident, ident);
    symbolTableNode->symbolTable->val_type = resolveType(type);
    symbolTableNode->symbolTable->val = s_expr;
    symbolTableNode->symbolTable->next = NULL;

    return symbolTableNode->symbolTable;
}

// Called after execution is done on the base of the tree.
// (see the program production in ciLisp.y)
// Recursively frees the whole abstract syntax tree.
// You'll need to update and expand freeNode as the project develops.
void freeNode(AST_NODE *node)
{
    if (!node)
        return;

    if (node->type == FUNC_NODE_TYPE)
    {
        // Recursive calls to free child nodes
        freeNode(node->data.function.op1);
        freeNode(node->data.function.op2);

        // Free up identifier string if necessary
        if (node->data.function.oper == CUSTOM_OPER)
        {
            free(node->data.function.ident);
        }
    }

    free(node);
}

NUM_TYPE findFunctionType(OPER_TYPE func, NUM_TYPE op1, NUM_TYPE op2)
{
    if (op1 == INT_TYPE)
    {
        if (func == NEG_OPER)
            return INT_TYPE;

        if (func == ADD_OPER || func == SUB_OPER || func == MULT_OPER || func == DIV_OPER)
            if (op2 == INT_TYPE)
                return INT_TYPE;
    }
    return DOUBLE_TYPE;
}

RET_VAL compareValTypeWithRetVal(SYMBOL_TABLE_NODE *symbolTableNode, RET_VAL value)
{
    // (stNode->valType  RET_VAL)
    // (int                 3.33)    = 3
    if (symbolTableNode->val_type == INT_TYPE && value.type == DOUBLE_TYPE)
    {
        printf("WARNING: incompatible type assignment for variables %s\n", symbolTableNode->ident);
        freeNode((symbolTableNode->val));
        symbolTableNode->val = createNumberNode(floor(value.value), INT_TYPE);
        return (RET_VAL){ INT_TYPE, floor(value.value) };
    }

    // (double 4) = 4.0
    if (symbolTableNode->val_type == DOUBLE_TYPE && value.type == INT_TYPE)
        return (RET_VAL){ DOUBLE_TYPE, value.value };


    return value;
}

// Evaluates an AST_NODE.
// returns a RET_VAL storing the the resulting value and type.
// You'll need to update and expand eval (and the more specific eval functions below)
// as the project develops.
RET_VAL eval(AST_NODE *node)
{
    if (!node)
        return (RET_VAL){INT_TYPE, NAN};

    RET_VAL result = {INT_TYPE, NAN}; // see NUM_AST_NODE, because RET_VAL is just an alternative name for it.

    // TODO complete the switch. : DONE
    // Make calls to other eval functions based on node type.
    // Use the results of those calls to populate result.
    switch (node->type)
    {
        case NUM_NODE_TYPE:
            result = evalNumNode(node);
            break;
        case FUNC_NODE_TYPE:
            result = evalFuncNode(node);
            break;
        case SYM_NODE_TYPE:
            result = evalSymbol(node, node->data.symbol.ident);
            break;
        default:
            yyerror("Invalid AST_NODE_TYPE, probably invalid writes somewhere!");
    }

    return result;
}  

// returns a RET_VAL with the data stored in the number node
RET_VAL evalNumNode(AST_NODE *node)
{
    if (!node)
        return (RET_VAL){INT_TYPE, NAN};

    RET_VAL result = {INT_TYPE, NAN};

    // TODO populate result with the values stored in the node. : DONE
    // SEE: AST_NODE, AST_NODE_TYPE, NUM_AST_NODE
    result.type = node->data.number.type;
    result.value = node->data.number.value;
    return result;
}


RET_VAL evalFuncNode(AST_NODE *node)
{
    if (!node)
        return (RET_VAL){INT_TYPE, NAN};

    RET_VAL result = {INT_TYPE, NAN};

    // TODO populate result with the result of running the function on its operands. : DONE
    // SEE: AST_NODE, AST_NODE_TYPE, FUNC_AST_NODE
    OPER_TYPE func = node->data.function.oper;

    RET_VAL op1 = eval(node->data.function.op1);
    RET_VAL op2 = eval(node->data.function.op2);

    switch( func )
    {
        case NEG_OPER:
            result.type = findFunctionType(func, op1.type, op2.type);
            result.value = -op1.value;
            break;
        case ABS_OPER:
            result.type = findFunctionType(func, op1.type, op2.type);
            result.value = fabs(op1.value);
            break;
        case EXP_OPER:
            result.type = findFunctionType(func, op1.type, op2.type);
            result.value = exp(op1.value);
            break;
        case SQRT_OPER:
            result.type = findFunctionType(func, op1.type, op2.type);
            result.value = sqrt(op1.value);
            break;
        case ADD_OPER:
            result.type = findFunctionType(func, op1.type, op2.type);
            result.value = op1.value + op2.value;
            break;
        case SUB_OPER:
            result.type = findFunctionType(func, op1.type, op2.type);
            result.value = op1.value - op2.value;
            break;
        case MULT_OPER:
            result.type = findFunctionType(func, op1.type, op2.type);
            result.value = op1.value * op2.value;
            break;
        case DIV_OPER:
            result.type = findFunctionType(func, op1.type, op2.type);
            if (op2.value == 0)
                result.value = NAN;
            result.value = op1.value / op2.value;
            break;
        case REMAINDER_OPER:
            result.type = findFunctionType(func, op1.type, op2.type);
            result.value = fmod(op1.value, op2.value);
            break;
        case LOG_OPER:
            result.type = findFunctionType(func, op1.type, op2.type);
            if (op1.value == 0)
                result.value = NAN;
            result.value = log10(op1.value);
            break;
        case POW_OPER:
            result.type = findFunctionType(func, op1.type, op2.type);
            result.value = pow(op1.value, op2.value);
            break;
        case MAX_OPER:
            result.type = findFunctionType(func, op1.type, op2.type);
            result.value = fmax(op1.value, op2.value);
            break;
        case MIN_OPER:
            result.type = findFunctionType(func, op1.type, op2.type);
            result.value = fmin(op1.value, op2.value);
            break;
        case EXP2_OPER:
            result.type = findFunctionType(func, op1.type, op2.type);
            result.value = exp2(op1.value);
            break;
        case CBRT_OPER:
            result.type = findFunctionType(func, op1.type, op2.type);
            result.value = cbrt(op1.value);
            break;
        case HYPOT_OPER:
            result.type = findFunctionType(func, op1.type, op2.type);
            result.value = hypot(op1.value, op2.value);
            break;

        default:
            yyerror("illegal function evaluated");
    }

    return result;
}

RET_VAL evalSymbol(AST_NODE *astNode, char *ident)
{
    if (astNode == NULL)
    {
        yyerror("Undeclared symbol");
        printf("Error: An undeclared symbol was entered\n");
        return (RET_VAL) {INT_TYPE, NAN};
    }

    SYMBOL_TABLE_NODE *node = findSymbol(astNode->symbolTable, ident);
    if (node != NULL)
        return compareValTypeWithRetVal(node, eval(node->val));

    return evalSymbol(astNode->parent, ident);
}

// prints the type and value of a RET_VAL
void printRetVal(RET_VAL val)
{
    // TODO print the type and value of the value passed in. : DONE
    if (val.type == INT_TYPE)
        printf("Type: Int\nValue: %d\n",  (int)val.value);
    else
        printf("Type: Double\nValue: %lf\n", val.value);

}
