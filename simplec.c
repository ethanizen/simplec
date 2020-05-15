#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "simplec.h"
#include "template.h"


int curVar = 1;
int curLabel = 1;

SymbolTable* currentScope = NULL;
SymbolTable* globalScope = NULL;


int main(int argc, char const *argv[])
{

	inFP = fopen(argv[1], "r");

	currentScope = globalScope = symbolTableInit();

	printf(PROJ4_PROLOGUE);

	char c = fgetc(inFP);
	while (c != EOF)
	{
		LinkedList* tokens = listInit();

		ungetc(c, inFP);
		tokens->head = lex();

		parse(tokens);

		freeList(tokens);

		c = fgetc(inFP);
		while(isspace(c))
		{
			c = fgetc(inFP);
		}
	}
	
	return 0;
}


///////////////////////////
// Linked List Functions	//
///////////////////////////
Node* nodeInit()
{
	Node* newNode = (Node*)malloc(sizeof(Node));

	newNode->tokenID = ERROR;
	newNode->next = NULL;
	newNode->lexeme = malloc(sizeof(char) * BUFFER_SIZE);

	return newNode;
}

Node* nodeAdd(Node* head, char *lexeme, int tokenID)
{
	Node* newNode = nodeInit(lexeme, tokenID);

	if (head == NULL) return newNode;

	Node* tmp = head;

	while (tmp->next != NULL)
		tmp = tmp->next;

	tmp->next = newNode;
}

// Frees head, and returns the next node in the linked list
Node* freeNode(Node* head)
{
	Node* tmp = head->next;

	free(head->lexeme);
	free(head);

	return tmp;
}

Node* freeStatement(Node* head)
{
	while(head != NULL)
	{
		head = freeNode(head);
	}

	return head;
}

LinkedList* listInit(void)
{
	LinkedList* tmp = (LinkedList*)malloc(sizeof(LinkedList));

	tmp->head = NULL;

	return tmp;
}

void freeList(LinkedList* list)
{

	list->head = freeStatement(list->head);

	free(list);
}

int isOperatorChar(char c)
{
	if (c == '+' || c == '-' || c == '*' || c == '/' || c == '%')
		return 1;
	return 0;
}

int isOperatorID(int tokenID)
{
	if (tokenID >= 4 && tokenID <= 8) return 1;

	return 0;
}

///////////////////////////
//  Sym-Table Functions	//
///////////////////////////
SymbolTable* symbolTableInit(void)
{
	SymbolTable* newTable = (SymbolTable*)malloc(sizeof(SymbolTable));

	newTable->first = NULL;
	newTable->last = NULL;

	return newTable;
}

SymbolNode* symbolNodeInit(void)
{
	SymbolNode *newNode = (SymbolNode*)malloc(sizeof(SymbolNode));

	newNode->ident = NULL;
	newNode->addr = NULL;
	newNode->prev = NULL;

	return newNode;
}

void put(SymbolTable* table, char* ident, char* addr)
{
	SymbolNode* newNode = symbolNodeInit();
	newNode->ident = ident;
	newNode->addr = addr;

	if (table == NULL)
	{
		table = (SymbolTable*)(malloc(sizeof(SymbolTable)));

		table->first = newNode;
		table->last = newNode;
	}
	else
	{
		newNode->prev = table->first;
		table->first = newNode;
	}
}

char* lookup(SymbolTable* table, char* ident)
{
	SymbolNode* curr = table->first;

	while (curr != NULL)
	{
		if (strcmp(ident, curr->ident) == 0)
			return curr->addr;

		curr = curr->prev;
	}

	fprintf(stderr, "error: use of undeclared variable %s\n", ident);

	return NULL;
}

int contains(SymbolTable* table, char* ident)
{
	SymbolNode* curr = table->first;

	while(curr != NULL)
	{
		if (strcmp(ident, curr->ident) == 0)
			return 1;

		curr = curr->prev;
	}

	return 0;
}

///////////////////////////
// 	 Token Functions	//
///////////////////////////

// Buffer comes in with buffer[0] == 'p', continues to make sure
int printToken(char* buffer)
{
	getWord(buffer);
	return strncmp("print", buffer, PRINT_LENGTH) == 0 ? PRINT : ERROR;
}

void getWord(char* buffer)
{
	char c = fgetc(inFP);
	int i;
	for (i = 1; !isspace(c) && isalpha(c) || isdigit(c); i++)
	{
		buffer[i] = c;
		c = fgetc(inFP);
	}

	buffer[i] = '\0';

	ungetc(c, inFP);
}

// Buffer comes in with buffer[0] == the first number or of the token
int numberToken(char* buffer)
{
	char c = fgetc(inFP);

	int i;
	for (i = 1; isdigit(c); i++)
	{
		buffer[i] = c;
		c = fgetc(inFP);
	}

	buffer[i] = '\0';

	ungetc(c, inFP);
	return NUMBER;
}

void twoCharOpToken(char* buffer)
{
	buffer[1] = fgetc(inFP);
	buffer[2] = '\0';
}

int semicolonToken(char *buffer)
{
	return buffer[0] == ';' ? SEMI : ERROR;
}

int charToken(char* buffer)
{
	getWord(buffer);

	if (strcmp(buffer, "print") == 0)
		return PRINT;
	else if (strcmp(buffer, "read") == 0)
		return READ;
	else if (strcmp(buffer, "int") == 0)
		return INT;
	else if (strcmp(buffer, "return") == 0)
		return RET;
	else if (strcmp(buffer, "if") == 0)
		return IF;
	else if (strcmp(buffer, "else") == 0)
		return ELSE;
	else if (strcmp(buffer, "while") == 0)
		return WHILE;
	else if (strcmp(buffer, "return") == 0)
		return RET;
	else
		return IDENT;
}


int getToken(char lexeme, char prev, int prevID)
{
	if (isalpha(lexeme))
		return CHAR;
	else if (lexeme == '+')
		return PLUS;
	else if (lexeme == '-')
	{
		int retVal;
		char c = fgetc(inFP);
		if (isdigit(prev) || prev == ')' || prevID == IDENT)
			retVal = MINUS;
		else if (prev == '(')
			retVal =  NUMBER;
		else
			retVal = NUMBER;

		ungetc(c, inFP);
		return retVal;
	}
	else if (lexeme == '-')
		return MINUS;
	else if (lexeme == '/')
		return DIVIDE;
	else if (lexeme == '*')
		return TIMES;
	else if (lexeme == '%')
		return MOD;
	else if (isdigit(lexeme))
		return NUMBER;
	else if (lexeme == '(')
		return LPAREN;
	else if (lexeme == ')')
		return RPAREN;
	else if (lexeme == '=')
	{
		char next = fgetc(inFP);
		ungetc(next, inFP);
		if (next == '=')
			return EQUALS;
		else
			return ASSIGN;
	}
	else if (lexeme == '{')
		return LCURLY;
	else if (lexeme == '}')
		return RCURLY;
	else if (lexeme == '!')
	{
		char next = fgetc(inFP);
		ungetc(next, inFP);
		if (next == '=')
			return NEQUALS;
		else
			return NOT;
	}
	else if (lexeme == '<')
		return LT;
	else if (lexeme == '>')
		return GT;
	else if (lexeme == '&')
		return AND;
	else if (lexeme == '|')
		return OR;
	else if (lexeme == ';')
		return SEMI;
	else if (lexeme == ',')
		return COMMA;


	fprintf(stderr, "ERROR: Unknown character: '%c'\n", lexeme);
	exit(EXIT_FAILURE);
}

int isParen(char c)
{
	if (c == '(' || c == ')')
		return 1;
	return 0;
}

///////////////////////////
// 	 Lexer Functions	//
///////////////////////////

// Reads the next statement of the input to the semi-colon
// Returns 1 if no errors were encountered
Node* lex(void)
{
	char cur = fgetc(inFP);

	char* buffer = malloc(sizeof(char) * BUFFER_SIZE);
	buffer[0] = cur;
	buffer[1] = '\0';


	Node* head = nodeInit();
	Node* tmp = head;
	Node* prevNode = tmp;

	char prev = '\0';
	char prevID = ERROR;
	while (cur != EOF)
	{
		tmp->next = nodeInit();

		int tokenID = getToken(cur, prev, prevID);

		// Fill out buffer
		switch(tokenID){
			case CHAR:
				tokenID = charToken(buffer);
				break;
			case NUMBER:
				numberToken(buffer);
				break;
			default:
				break;
		}
		if (tokenID == EQUALS || tokenID == NEQUALS || tokenID == AND || tokenID == OR)
			twoCharOpToken(buffer);

		strcpy(tmp->lexeme, buffer);
		tmp->tokenID = tokenID;

		prevNode = tmp;
		tmp = tmp->next;

		prev = cur;
		cur = fgetc(inFP);
		while (isspace(cur) && cur != EOF)
			cur = fgetc(inFP);

		buffer[0] = cur;
		buffer[1] = '\0';

		prevID = tokenID;
	}

	prevNode->next = NULL;

	freeNode(tmp);

	free(buffer);

	return head;
}

char* getOperatorStr(int tokenID)
{
	switch(tokenID){
		case PLUS:
			return plusStr;
			break;
		case MINUS:
			return minusStr;
			break;
		case TIMES:
			return timesStr;
			break;
		case DIVIDE:
			return divideStr;
			break;
		case MOD:
			return modStr;
			break;
		case EQUALS:
			return equalStr;
			break;
		case NEQUALS:
			return nequalStr;
			break;
		case LT:
			return ltStr;
			break;
		case GT:
			return gtStr;
			break;
		case AND:
			return andStr;
			break;
		case OR:
			return orStr;
			break;
	}
}

void printPrint(void)
{
	printf("	call void @print_integer(i32 ");
}

void printNumPrint(char* num)
{
	printPrint();
	printf("%s)\n", num);
}

void printVarPrint(int varNum)
{
	printPrint();
	printCurrentVariable();
	printf(")\n");
}

void printCurrentVariable()
{
	printf("%%t%d", curVar);
}

char* getCurrentVariable()
{
	char* var = (char*)malloc(sizeof(char) * BUFFER_SIZE);
	sprintf(var, "%%t%d", curVar);

	return var;
}


void printStatement(Node* head)
{

	char* printVal = malloc(sizeof(char) * BUFFER_SIZE);

	char* firstVal = malloc(sizeof(char) * BUFFER_SIZE);
	char* secondVal = malloc(sizeof(char) * BUFFER_SIZE);

	Node* tmp = head;
	while(isOperatorID(tmp->tokenID) == 0 && tmp->next != NULL)
	{
		if (tmp->tokenID == NUMBER)
		{
			strcpy(printVal, tmp->lexeme);
		}
		tmp = tmp->next;
	}

	char* ouputBuffer = malloc(sizeof(char) * BUFFER_SIZE);

	if(tmp->next != NULL)
	{
		printf("	");
		printCurrentVariable();
		printf(" = ");
		printf("%s ", getOperatorStr(tmp->tokenID));
		printf("%s, %s\n", printVal, tmp->next->lexeme);
		printVarPrint(curVar);
		curVar++;

	}
	else
	{
		printNumPrint(printVal);
	}
}


void printFile(FILE* fp)
{
	char c = fgetc(fp);

	while (c != EOF)
	{
		printf ("%c", c);
	  	c = fgetc(fp);
   	}
}


///////////////////////////
// 	 Parser Functions	//
///////////////////////////

char* orExpression(LinkedList* tokens)
{
	char* left = andExpression(tokens);
	while (tokens->head->tokenID == OR)
	{
		int tokenID = consume(tokens);
		char* right = andExpression(tokens);
		char* result = emit(left, right, tokenID);
		left = result;
	}

	return left;

}

char* andExpression(LinkedList* tokens)
{
	char* left = equalsExpression(tokens);

	while (tokens->head->tokenID == AND)
	{
		int tokenID = consume(tokens);
		char* right = equalsExpression(tokens);
		char* result = emit(left, right, tokenID);
		left = result;
	}

	return left;
}

char* equalsExpression(LinkedList* tokens)
{
	char* left = relationExpression(tokens);

	while (tokens->head->tokenID == EQUALS || tokens->head->tokenID == NEQUALS)
	{
		int tokenID = consume(tokens);
		char* right = relationExpression(tokens);
		char* result = emit(left, right, tokenID);
		left = result;
	}

	return left;

}

char* relationExpression(LinkedList* tokens)
{
	char* left = expression(tokens);

	while (tokens->head->tokenID == LT || tokens->head->tokenID == GT)
	{
		int tokenID = consume(tokens);
		char* right = expression(tokens);
		char* result = emit(left, right, tokenID);
		left = result;
	}

	return left;

}

char* expression(LinkedList* tokens)
{
	char *left = term(tokens);

	while(isAddSymbol(tokens->head->tokenID))
	{
		int tokenID = addSymbol(tokens);
		char* right = term(tokens);
		char* result = emit(left, right, tokenID);
		left = result;
	}


	//char *result = expressionPrime(tokens, left);

	return left;
}


char* expressionPrime(LinkedList* tokens, char* left)
{
	char	*right;

	if (isAddSymbol(tokens->head->tokenID))
	{
		int tokenID = addSymbol(tokens);
		right = term(tokens);
		char* result = emit(left, right, tokenID);
		return expressionPrime(tokens, result);
	}
	else // empty string
		return left;
}

char* term(LinkedList* tokens)
{
	char* left = factor(tokens);

	while (isProdSymbol(tokens->head->tokenID))
	{
		int tokenID = prodSymbol(tokens);
		char* right = factor(tokens);
		char* result = emit(left, right, tokenID);
		left = result;
	}

	//char* result = termPrime(tokens, left);

	return left;
}

char* termPrime(LinkedList* tokens, char* left)
{
	if (isProdSymbol(tokens->head->tokenID))
	{
		int tokenID = prodSymbol(tokens);
		char* right = factor(tokens);
		char* result = emit(left, right, tokenID);
		return termPrime(tokens, result);
	}
	else // empty string
		return left;
}

char* factor(LinkedList* tokens)
{
	if (tokens->head->tokenID == LPAREN)
	{
		parenSymbol(tokens);
		char* result = orExpression(tokens);
		parenSymbol(tokens);
		return result;
	}
	else if (tokens->head->tokenID == IDENT && tokens->head->next->tokenID == LPAREN)
	{
		char* funName = consumeIdent(tokens);
		if (!contains(globalScope, funName))
			error(FUNC_NOT_EXISTS, funName);
		char* retVal = getCurrentVariable();
		curVar++;
		consume(tokens); // LPAREN
		char** actualParam = NULL;
		int numParams = 0;
		if (tokens->head->tokenID != RPAREN)
		{
			actualParam = (char**)(malloc(sizeof(char*) * BUFFER_SIZE));
			actualParam[numParams] = expression(tokens);
			numParams++;
			while (tokens->head->tokenID == COMMA)
			{
				consume(tokens); // COMMA
				actualParam[numParams] = expression(tokens);
				numParams++;
			}
		}
		consume(tokens); // RPAREN
		printf("%s = call i32 @%s(", retVal, funName);
		if (actualParam != NULL)
		{
			printf("i32 %s", actualParam[0]);
			for (int i = 1; i < numParams; i++)
				printf(", i32 %s", actualParam[i]);
		}
		printf(")\n");
		return retVal;
	}
	else if (tokens->head->tokenID == IDENT)
	{
		char* ident = consumeIdent(tokens);
		char* addr = lookup(currentScope, ident);
		char* result = getCurrentVariable();
		curVar++;
		printf("%s = load i32, i32* %s\n", result, addr);
		return result;
	}
	else if (tokens->head->tokenID == NOT)
	{
		consume(tokens); // NOT
		char* expr = orExpression(tokens);
		char* result = getCurrentVariable();
		curVar++;
		printf("%s = xor i1 %s, 1\n", result, expr);
		return result;
	}
	else // NUMBER
		return numberSymbol(tokens);
}

int addSymbol(LinkedList* tokens)
{
	int tokenID = tokens->head->tokenID;
	if (!isAddSymbol(tokenID))
	{
		fprintf(stderr, "	Unexpected symbol \"%s\"\n", tokens->head->lexeme);
		exit(EXIT_FAILURE);
	}

	tokens->head = freeNode(tokens->head);

	return tokenID;
}

int prodSymbol(LinkedList* tokens)
{
	int tokenID = tokens->head->tokenID;
	if (!isProdSymbol(tokenID))
	{
		fprintf(stderr, "	Unexpected symbol \"%s\"\n", tokens->head->lexeme);
		exit(EXIT_FAILURE);
	}

	tokens->head = freeNode(tokens->head);
	return tokenID;
}

void parenSymbol(LinkedList* tokens)
{
	int tokenID = tokens->head->tokenID;

	tokens->head = freeNode(tokens->head);
}

char* numberSymbol(LinkedList* tokens)
{
	char* buffer = malloc(sizeof(char) * BUFFER_SIZE);
	strcpy(buffer, tokens->head->lexeme);
	tokens->head = freeNode(tokens->head);

	return buffer;
}



int lookahead(LinkedList* tokens)
{
	return tokens->head->next->tokenID;
}

int isAddSymbol(int id)
{
	if (id == PLUS || id == MINUS)
		return 1;

	return 0;
}

int isProdSymbol(int id)
{
	if (id == TIMES || id == DIVIDE || id == MOD)
		return 1;

	return 0;
}

char* emit(char* left, char* right, int tokenID)
{
	printf("	");
	char* var = getCurrentVariable();
	printf("%s", var);
	printf(" = ");
	printf("%s ", getOperatorStr(tokenID));
	printf("%s, %s\n", left, right);
	curVar++;

	return var;
}

char* consumeIdent(LinkedList* tokens)
{
	char* ident = malloc(sizeof(char) * strlen(tokens->head->lexeme) + 1);
	strcpy(ident, tokens->head->lexeme);

	tokens->head = freeNode(tokens->head);

	return ident;
}

void consumeAssign(LinkedList* tokens)
{
	if (tokens->head->tokenID != ASSIGN)
		error(EXPECT_ASSIGN, tokens->head->lexeme);

	tokens->head = freeNode(tokens->head);
}

void consumeSemi(LinkedList* tokens)
{
	if (tokens->head->tokenID != SEMI)
		error(EXPECT_SEMI, tokens->head->lexeme);

	tokens->head = freeNode(tokens->head);
}

char* consumeParam(LinkedList* tokens)
{
	if (tokens->head->tokenID != INT)
		error(EXPECT_INT, tokens->head->lexeme);
	tokens->head = freeNode(tokens->head);

	if (tokens->head->tokenID != IDENT)
		error(EXPECT_IDENT, tokens->head->lexeme);

	char* param = (char*)(malloc(sizeof(char) * strlen(tokens->head->lexeme) + 1));
	strcpy(param, tokens->head->lexeme);

	tokens->head = freeNode(tokens->head);

	return param;
}

int consume(LinkedList* tokens)
{
	int tokenID = tokens->head->tokenID;
	tokens->head = freeNode(tokens->head);

	return tokenID;
}

void print(LinkedList* tokens)
{
	tokens->head = freeNode(tokens->head);

	char* result = expression(tokens);

	consumeSemi(tokens);

	printf("call void @print_integer(i32 %s)\n", result);
}

void declaration(LinkedList* tokens)
{
	tokens->head = freeNode(tokens->head);

	char* ident = consumeIdent(tokens);

	consumeSemi(tokens);

	if (contains(currentScope, ident))
		error(VAR_EXISTS, ident);

	char* result = getCurrentVariable();
	curVar++;

	printf("%s = alloca i32\n", result);

	put(currentScope, ident, result);
}

void read(LinkedList* tokens)
{
	tokens->head = freeNode(tokens->head);

	char* ident = consumeIdent(tokens);

	consumeSemi(tokens);

	char* addr = lookup(currentScope, ident);

	char* result = getCurrentVariable();
	curVar++;
	printf("%s = call i32 @read_integer()\n", result);
	printf("store i32 %s, i32* %s\n", result, addr);
}

void assign(LinkedList* tokens)
{
	char* ident = consumeIdent(tokens);

	consumeAssign(tokens);

	char* result = expression(tokens);

	consumeSemi(tokens);

	char* addr = lookup(currentScope, ident);

	printf("store i32 %s, i32* %s\n", result, addr);
}

char* newLabel(void)
{
	char* buffer = malloc(sizeof(char) * BUFFER_SIZE);

	sprintf(buffer, "label%d", curLabel);
	curLabel++;
	return buffer;
}

void ifStatement(LinkedList* tokens)
{
	consume(tokens); // IF
	consume(tokens); // LPAREN
	char* cond = orExpression(tokens);
	consume(tokens); // RPAREN

	Node* temp = tokens->head;
	while (temp->next != NULL && temp->tokenID != ELSE && temp->tokenID != IF)
		temp = temp->next;

	if (temp->next != NULL && temp->tokenID == ELSE)
		elseStatment(tokens, cond);
	else
	{
		char* body = newLabel();
		char* end = newLabel();
		printf("br i1 %s, label %%%s, label %%%s\n", cond, body, end);
		printf("%s:\n", body);
		statement(tokens);
		printf("br label %%%s\n", end);
		printf("%s:\n", end);
	}
}

void elseStatment(LinkedList* tokens, char* cond)
{
	char* ifBody = newLabel();
	char* elseBody = newLabel();
	char* end = newLabel();
	printf("br i1 %s, label %%%s, label %%%s\n", cond, ifBody, elseBody);
	printf("%s:\n", ifBody);
	statement(tokens);
	printf("br label %%%s\n", end);
	consume(tokens); // ELSE
	printf("%s:\n", elseBody);
	statement(tokens);
	printf("br label %%%s\n", end);
	printf("%s:\n", end);
}

void whileStatement(LinkedList* tokens)
{
	consume(tokens); // WHILE
	consume(tokens); // LPAREN
	char* head = newLabel();
	printf("br label %%%s\n", head);
	printf("%s:\n", head);
	char* cond = orExpression(tokens);
	consume(tokens); // RPAREN
	char* body = newLabel();
	char* end = newLabel();
	printf("br i1 %s, label %%%s, label %%%s\n", cond, body, end);
	printf("%s:\n", body);
	statement(tokens);
	printf("br label %%%s\n", head);
	printf("%s:\n", end);
}

void ret(LinkedList* tokens)
{
	consume(tokens); // RET
	char* retVal = expression(tokens);
	consume(tokens); // SEMI
	printf("ret i32 %s\n", retVal);
}

void statement(LinkedList* tokens)
{
	int continues = 1;
	while (continues && tokens->head != NULL)
	{
		continues = 0;
		switch(tokens->head->tokenID){
			case IF:
				ifStatement(tokens);
				continues = 1;
				break;
			case WHILE:
				whileStatement(tokens);
				continues = 1;
				break;
			case PRINT:
				print(tokens);
				continues = 1;
				break;
			case INT:
				declaration(tokens);
				continues = 1;
				break;
			case READ:
				read(tokens);
				continues = 1;
				break;
			case IDENT:
				assign(tokens);
				continues = 1;
				break;
			case LCURLY:
				consume(tokens);
				continues = 1;
				break;
			case RCURLY:
				consume(tokens);
				break;
		}

	}
}

void function(LinkedList* tokens)
{
	consume(tokens); // INT
	printf("define i32 ");
	char* funName = (char*)malloc(sizeof(char) * BUFFER_SIZE);
	strcpy(funName, tokens->head->lexeme);
	consume(tokens);
	if (contains(globalScope, funName))
		error(FUNC_EXISTS, funName);
	printf("@%s", funName);
	consume(tokens); // LPAREN

	char** paramaters = (char**)(malloc(sizeof(char*) * BUFFER_SIZE));

	printf("(");

	int numParams = 0;
	if (tokens->head->tokenID == INT)
	{
		char* param = consumeParam(tokens);
		paramaters[0] = param;
		printf("i32");
		numParams++;
		while (tokens->head->tokenID != RPAREN)
		{
			consume(tokens); // COMMA
			param = consumeParam(tokens);
			printf(", i32");
			paramaters[numParams] = param;
			numParams++;
		}
	}
	consume(tokens); // RPAREN
	printf(")");
	put(currentScope, funName, funName);
	consume(tokens); // LCURLY
	printf("{\n");
	SymbolTable* localScope = symbolTableInit();
	SymbolTable* parentScope = currentScope;
	currentScope = localScope;

	for (int i = 0; i < numParams; i++)
	{
		char* param = paramaters[i];
		char* paramVar = getCurrentVariable();
		curVar++;
		put(localScope, param, paramVar);
		printf("%s = alloca i32\n", paramVar);
		printf("store i32 %%%d, i32* %s\n", i, paramVar);
	}

	while(tokens->head->tokenID != RET)
	{
		statement(tokens);
	}

	ret(tokens);

	consume(tokens); // RCURLY
	printf("}\n");
	currentScope = parentScope;

}

void parse(LinkedList* tokens)
{
	while (tokens->head != NULL)
	{
		function(tokens);
	}
}

///////////////////////////
// 	 Debug Functions	//
///////////////////////////
void printTokens(Node* head)
{
	while (head != NULL)
	{
		printf("tokenID = %d, lexeme = %s\n", head->tokenID, head->lexeme);
		head = head->next;
	}
}

void error(int errorCode, char* errorStr)
{
	switch(errorCode){
		case VAR_NOT_EXISTS:
			fprintf(stderr, "error: use of undeclared variable %s\n", errorStr);
			break;
		case VAR_EXISTS:
			fprintf(stderr, "error: multiple definitions of %s\n", errorStr);
			break;
		case FUNC_NOT_EXISTS:
			fprintf(stderr, "error: use of undeclared function %s\n", errorStr);
			break;
		case FUNC_EXISTS:
			fprintf(stderr, "error: multiple definitions of %s\n", errorStr);
	}

	exit(-1);
}
