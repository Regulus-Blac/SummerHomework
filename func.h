#ifndef __FUNC_H__
#define __FUNC_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <windows.h>
#include <stack> 
using namespace std;

#define INFEASIBLE -1
#define TRUE 1
#define OK 1
#define FALSE 0
#define ERROR 0
typedef int status;
typedef int ElemType;

// SqList
#define LIST_INIT_SIZE 1500
#define LISTINCREMENT 1000
#define MEMORY_MAX 8500
//ans
#define MAX_VAR 3200
//SqBack
#define MAX_BACK 400
#define BACKINCREMENT 100
//input
#define FILE_MAX 30
//sudoku (initnum太大有可能导致终盘生成失败)
#define SUDOKU_VAR 729
#define BOX_SIZE 3
#define N 9
#define INIT_NUM_1 15
#define INIT_NUM_2 18
#define MAX_GENERATE_1 10
#define MAX_GENERATE_2 20
//difficulty
#define EASY 35
#define MID 42
#define HARD 50
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

typedef struct{ 
	int * elem;
	int length;
	int listsize;
}SqBack;

/*Part SqList & SqBack*/
void InitList(SqList& L);
void InitBack(SqBack& L);
status DestroyList(SqList& L);
status DestroyBack(SqBack& L);
status ClearList(SqList& L);
status ClearBack(SqBack& L);
status ListInsert(SqList &L,CLAUSE *node);
status BackInsert(SqBack &L, int node);

/*Part DPLL*/
//void makecopy(CNF &newS, CNF &S);
int num_trueclau(CNF &S); 
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

void showClause(CLAUSE *head,LITSHEET *ans);
CLAUSE *existUnitClause(CLAUSE *head);
bool existEmptyClause(CLAUSE *head); 
CLAUSE *locatePre(CLAUSE *node,CLAUSE *head); 
int deleteOneClause(CLAUSE *node, SqList &L, CNF &S, LITSHEET* ans);
bool deleteOneClause_slt(CLAUSE *Node, SqList &L, CNF &S) ;

int deleteClause(SqList &L, CNF &S, LITSHEET* ans, SqBack &back);
int deleteLit( SqList &L, CNF &S, int value, LITSHEET* ans/*,FILE *test*/);
status restore_cl( SqList &L, CNF &S);
status restore_lit(SqBack &back, LITSHEET* ans);
void recover_lit(SqBack &back, LITSHEET* ans);
   
int choose_lit_1(CNF&S, LITSHEET* ans);
int choose_lit_2(CNF&S, LITSHEET* ans);
int choose_lit_jw(CNF&S, LITSHEET* ans);
int choose_lit_hybrid(CNF&S, LITSHEET* ans);

bool DPLL(CNF &S, LITSHEET* ans/*,FILE *test*/);
void check(LITSHEET* ans, int cnt);
void check(FILE *test,LITSHEET* ans,int cnt);
status saveOutput(LITSHEET* ans, int cnt, char file[], double used_time);

bool autocheck(CNF &S,LITSHEET* ans);

/*Part Sudoku*/

bool fundConsCNF(CNF &S, LITSHEET *ans);
bool percentConsCNF(CNF &S, LITSHEET *ans);
int ijk_cnf(int ijk);
int cnf_ijk(int cnf);
int randomNum(int code);

void addUnitClause(CNF &S, int value,LITSHEET * ans);
bool deleteS_head(CNF &S,LITSHEET * ans);
bool DFS_board_1(int row, int col, int cnt, CNF &S, int board[N][N],LITSHEET* ans);
bool generate_1(CNF &S, int board[N][N], LITSHEET* ans);
bool DFS_board_2(int row, int col, int cnt, CNF &S, int board[N][N],LITSHEET* ans);
bool generate_2(CNF &S, int board[N][N], LITSHEET* ans);

void recoverCNF(CNF &S, LITSHEET *ans);
void recoverBoard(int board[N][N]);
void copyBoard(int board[N][N], int record[N][N]);
bool dig_holes(int board[N][N], int diff, int mode);

bool fast_check_1(int board[N][N], int x, int y, int n);
bool fast_check_2(int board[N][N], int x, int y, int n);
void showBoard(int board[N][N]);
void Output_CNF(CNF &S);

/*Part Play*/
void play(int record[N][N],int board[N][N]);







#endif
