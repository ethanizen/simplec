#ifndef SIMPLEC_H
#define SIMPLEC_H

#define PRINT_LENGTH 5
#define BUFFER_SIZE 100


// TOKEN IDs
#define ERROR -1
#define NUMBER 1
#define PRINT 2
#define SEMI 3
#define PLUS 4
#define MINUS 5
#define TIMES 6
#define DIVIDE 7
#define MOD 8
#define LPAREN 9
#define RPAREN 10
#define IDENT 11
#define READ 12
#define INT 13
#define RET 14
#define ASSIGN 15
#define IF 16
#define ELSE 17
#define WHILE 18
#define LCURLY 19
#define RCURLY 20
#define EQUALS 21
#define NEQUALS 22
#define LT 23
#define GT 24
#define AND 25
#define OR 26
#define NOT 27
#define COMMA 28

#define CHAR 99

// Error Values
#define VAR_EXISTS 1000
#define VAR_NOT_EXISTS 1001
#define EXPECT_ASSIGN 1002
#define EXPECT_SEMI 1003
#define EXPECT_CURLY 1004
#define EXPECT_IDENT 1005
#define EXPECT_INT 1006
#define FUNC_EXISTS 1007
#define FUNC_NOT_EXISTS 1008


static FILE* inFP;
static FILE* preambleFP;
static FILE* pstambleFP;

static char plusStr[] = "add nsw i32";
static char minusStr[] = "sub nsw i32";
static char timesStr[] = "mul nsw i32";
static char divideStr[] = "sdiv i32";
static char modStr[] = "srem i32";
static char equalStr[] = "icmp eq i32";
static char nequalStr[] = "icmp ne i32";
static char ltStr[] = "icmp slt i32";
static char gtStr[] = "icmp sgt i32";
static char andStr[] = "and i1";
static char orStr[] = "or i1";



typedef struct Node {
	struct Node *next;

	char *lexeme;
	int tokenID;
} Node;

typedef struct LinkedList
{
	Node* head;
} LinkedList;

typedef struct SymbolNode {
	struct SymbolNode *prev;

	char* ident;
	char* addr;
} SymbolNode;

typedef struct SymbolTable {
	SymbolNode* first;
	SymbolNode* last;
} SymbolTable;



char* getOperatorStr(int tokenID);
void printPrint(void);
void printNumPrint(char* num);
void printVarPrint(int varNum);
void printCurrentVariable();
void printStatement(Node* head);
void printFile(FILE* fp);
Node* nodeInit();
Node* nodeAdd(Node* head, char *lexeme, int tokenID);
Node* freeNode(Node* head);
Node* freeStatement(Node* head);
SymbolTable* symbolTableInit(void);
SymbolNode* symbolNodeInit(void);
void put(SymbolTable* table, char* ident, char* addr);
char* lookup(SymbolTable* table, char* ident);
int contains(SymbolTable* table, char* ident);
int isOperatorChar(char c);
int isOperatorID(int tokenID);
int printToken(char* buffer);
int numberToken(char* buffer);
int semicolonToken(char *buffer);
int getToken(char lexeme, char prev, int prevID);
int isParen(char c);
Node* getStatement(void);
char* expression(LinkedList* tokens);
char* expressionPrime(LinkedList* tokens, char* left);
char* term(LinkedList* tokens);
char* termPrime(LinkedList* tokens, char* left);
char* factor(LinkedList* tokens);
int addSymbol(LinkedList* tokens);
int prodSymbol(LinkedList* tokens);
void parenSymbol(LinkedList* tokens);
char* numberSymbol(LinkedList* tokens);
int lookahead(LinkedList* tokens);
int isAddSymbol(int id);
int isProdSymbol(int id);
char* emit(char* left, char* right, int tokenID);
void printTokens(Node* head);
LinkedList* listInit(void);
void parse(LinkedList* tokens);
void freeList(LinkedList* list);
void error(int errorCode, char* errorStr);
void getWord(char* buffer);
char* consumeIdent(LinkedList* tokens);
void consumeAssign(LinkedList* tokens);
void consumeSemi(LinkedList* tokens);
void print(LinkedList* tokens);
void declaration(LinkedList* tokens);
void read(LinkedList* tokens);
void assign(LinkedList* tokens);
Node* lex(void);
void ifStatement(LinkedList* tokens);
void elseStatment(LinkedList* tokens, char* cond);
void whileStatment(LinkedList* tokens);
void statement(LinkedList* tokens);
int consume(LinkedList* tokens);
char* orExpression(LinkedList* tokens);
char* andExpression(LinkedList* tokens);
char* equalsExpression(LinkedList* tokens);
char* relationExpression(LinkedList* tokens);
char* expression(LinkedList* tokens);
void function(LinkedList* tokens);









#endif
