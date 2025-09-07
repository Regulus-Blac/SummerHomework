#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <windows.h>
#include <stack> 
using namespace std;
//#define CHECK

#define INFEASIBLE -1
#define TRUE 1
#define OK 1
#define FALSE 0
#define ERROR 0
typedef int status;
typedef int ElemType;

#define LIST_INIT_SIZE 1000
#define LISTINCREMENT 500
#define MAX_VAR 2000
#define FILE_MAX 30
#define SUDOKU_VAR 729
#define N 9
#define MAX_BACK 400

/*数据结构*/
typedef struct literal {
    int value; // 文字的值
    struct literal *next; // 指向下一个文字的指针
} LITERAL;

typedef struct clause {
    LITERAL *lit; // 子句中的文字
    int num;     // 子句中文字的数量
    bool isTrue; // 子句的真假值
    struct clause *next; // 指向下一个子句的指针
} CLAUSE;
typedef CLAUSE* PCLAUSE;
typedef struct cnf {
    int num_clau; // 子句数量
    int num_var;  // 变量数量
    bool exist_emptyclause;     
    CLAUSE *head; // 指向子句链表的头
    CLAUSE *tail; // 指向子句链表的尾
} CNF;

// typedef struct info_clau{
// 	CLAUSE *pre;
// 	CLAUSE *self;
// } INFO_C;

typedef struct info_lit{
	CLAUSE *clause;
	struct info_lit *next;
} INFO_L;

typedef struct {
	int ans;
	INFO_L *pos;		//正 
	INFO_L *nag;		//负 
} LITSHEET; 

/*动态数组（线性表） */
typedef struct{ 
	PCLAUSE * elem;
	int length;
	int listsize;
}SqList;

int num_trueclau(CNF &S);
/*Part SqList*/
void InitList(SqList& L);
status DestroyList(SqList& L);
status ClearList(SqList& L);
status ListInsert(SqList &L,CLAUSE *node);
/*Part DPLL*/

void makecopy(CNF &newS,const CNF &S); 
void initCNF(CNF &S, int var); 
void clearCNF(CNF &S,LITSHEET* ans);
void createClause(CNF &S);
bool buildCNF(CNF &S, int num_clau, FILE *fp, LITSHEET* ans);

void buildLitsheet(LITSHEET* ans, CLAUSE *node, int value);
status CNFparser(CNF &S, char file[],LITSHEET* ans);
void showCNF(CNF &S);
void fshowCNF(CNF &S);
void showLitsheet(LITSHEET* ans, int range);
void showLitans(LITSHEET* ans, int range);

void showClause(CLAUSE *head);
CLAUSE *existUnitClause(CLAUSE *head);
bool existEmptyClause(CLAUSE *head); 
CLAUSE *locatePre(CLAUSE *node,CLAUSE *head); 
int deleteOneClause(CLAUSE *node, SqList &L, CNF &S, LITSHEET* ans);
bool deleteOneClause_slt(CLAUSE *Node, SqList &L, CNF &S) ;

int deleteClause(SqList &L, CNF &S, LITSHEET* ans, int backtrace[]/*,FILE *test*/);
int deleteLit( SqList &L, CNF &S, int value, LITSHEET* ans/*,FILE *test*/);
status restore_cl( SqList &L, CNF &S);
status restore_lit(int back[], LITSHEET* ans);
void recover_lit(int back[], LITSHEET* ans);
   
int choose_lit_1(CNF&S, LITSHEET* ans);
int choose_lit_2(CNF&S, LITSHEET* ans);

bool DPLL(CNF &S, LITSHEET* ans/*,FILE *test*/);
void check(LITSHEET* ans, int cnt);
void check(FILE *test,LITSHEET* ans,int cnt);
status saveOutput(LITSHEET* ans, int cnt, char file[], double used_time);

bool autocheck(CNF &S,LITSHEET* ans);
/*Part Sudoku*/
    
// bool fundConsCNF(CNF &S, LITSHEET *ans);
// bool percentConsCNF(CNF &S, LITSHEET *ans);
// int ijk_cnf(int ijk);
// int cnf_ijk(int cnf);
// int randomNum(int code);

// void addUnitClause(CNF &S, int value,LITSHEET * ans);
// bool deleteS_head(CNF &S,LITSHEET * ans);
// bool DFS_board(/*FILE *test,*/int row, int col, int cnt,  stack <INFO_C> &op_clau, CNF &S, int board[N][N],LITSHEET* ans);
// bool generate(/*FILE *test,*/ stack <INFO_C> &op_clau, CNF &S, int board[N][N], LITSHEET* ans);


// void showBoard(int board[N][N]);
void Output_CNF(CNF &S);
