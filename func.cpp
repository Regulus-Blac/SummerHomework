#include "func.h"
#include <math.h>
//#define CHECK

/*构建动态数组 部分 */
void InitList(SqList& L)
// 1.1线性表L不存在，构造一个空的线性表，返回OK，否则返回INFEASIBLE。
{
	L.elem = (PCLAUSE *)malloc(sizeof(PCLAUSE) * LIST_INIT_SIZE);
	if(!L.elem){
		L.length = 0;
		L.listsize = 0;
		printf("InitList: malloc failed\n");
		return;
	}
	L.length = 0;
	L.listsize = LIST_INIT_SIZE;
}
void InitBack(SqBack &L)
// 1.2
{
	L.elem = (int *)malloc(sizeof(int) * MAX_BACK);
	if(!L.elem){
		L.length = 0;
		L.listsize = 0;
		printf( "InitBack: malloc failed\n");
		return;
	}else{
		for(int i = 0;i < MAX_BACK;i++)	L.elem[i] = 0;
	}
	L.length = 0;
	L.listsize = MAX_BACK;
}
status DestroyList(SqList& L)
// 2.1如果线性表L存在，销毁线性表L，释放数据元素的空间，返回OK，否则返回INFEASIBLE。
{
	if(!L.elem) return INFEASIBLE;

	free(L.elem);
	L.elem = NULL;          // 避免悬挂指针
	L.length = 0;
	L.listsize = 0;

	return OK;
}
status DestroyBack(SqBack& L)
// 2.2
{
	if(!L.elem) return INFEASIBLE;

	free(L.elem);
	L.elem = NULL;          // 避免悬挂指针
	L.length = 0;
	L.listsize = 0;

	return OK;
}
status ClearList(SqList& L)
// 3.1如果线性表L存在，删除线性表L中的所有元素并重置，返回OK，否则返回INFEASIBLE。
{
    if(!L.elem) return INFEASIBLE;

    free(L.elem);

    L.listsize = LIST_INIT_SIZE;
    L.elem = (PCLAUSE *)malloc(sizeof(PCLAUSE) * L.listsize);
    L.length = 0;

    return OK;
}
status ClearBack(SqBack& L)
// 3.2
{
    if(!L.elem) return INFEASIBLE;

    free(L.elem);

    L.listsize = MAX_BACK;
    L.elem = (int *)malloc(sizeof(int) * L.listsize);
    L.length = 0;

    return OK;
}
status ListInsert(SqList &L, CLAUSE *node)
// 4.1如果线性表L存在，将元素node插入到线性表L的末尾，返回OK；当插入位置不正确时，返回ERROR；如果线性表L不存在，返回INFEASIBLE。
{
	if (!L.elem) {
		printf( "ListInsert: L.elem is NULL (maybe destroyed)\n");
		return INFEASIBLE;
	}
	if (L.listsize <= 0 || L.length < 0) {
		printf( "ListInsert: invalid SqList state: length=%d listsize=%d elem=%p\n", L.length, L.listsize, (void*)L.elem);
		return INFEASIBLE;
	}
    if (node == NULL) {
        printf("insert a NULL,ERROR !\n");
        return FALSE;
    }

	if (L.length >= L.listsize) {
        // 扩容
        int new_listsize = L.listsize + LISTINCREMENT;
        PCLAUSE *new_elem = (PCLAUSE *)realloc(L.elem, sizeof(PCLAUSE) * new_listsize);
        
		if (!new_elem) {
			printf("NO MEMORY !\n");
			return INFEASIBLE;  // 内存不足
		}
        // 扩容成功，更新指针和容量
        L.elem = new_elem;
        L.listsize = new_listsize;
//		printf("扩容成功，现在有%d个元素，容量为%d\n",L.length, L.listsize);       
    }
	//	对新分配的内存进行初始化
	for(int i = L.length ;i < L.listsize;i++){
		L.elem[i] = NULL;
	} 
    // 插入元素

	L.elem[L.length] = node;
    L.length ++;
    return TRUE;
}
status BackInsert(SqBack &L, int node)
// 4.2
{
	if (!L.elem) {
		printf("ListInsert: L.elem is NULL (maybe destroyed)\n");
		return INFEASIBLE;
	}
	if (L.listsize <= 0 || L.length < 0) {
		printf("ListInsert: invalid SqList state: length=%d listsize=%d elem=%p\n", L.length, L.listsize, (void*)L.elem);
		return INFEASIBLE;
	}
    if (node == NULL) {
        printf("insert a NULL,ERROR !\n");
        return FALSE;
    }

	if (L.length >= L.listsize) {
        // 扩容
        int new_listsize = L.listsize + BACKINCREMENT;
        int *new_elem = (int *)realloc(L.elem, sizeof(int) * new_listsize);
        
		if (!new_elem) {
			printf("NO MEMORY !\n");
			return INFEASIBLE;  // 内存不足
		}
        // 扩容成功，更新指针和容量
        L.elem = new_elem;
        L.listsize = new_listsize;
//		printf("扩容成功，现在有%d个元素，容量为%d\n",L.length, L.listsize);       
    }
	//	对新分配的内存进行初始化
	for(int i = L.length ;i < L.listsize;i++){
		L.elem[i] = 0;
	} 
    // 插入元素

	L.elem[L.length] = node;
    L.length ++;
    return TRUE;
}

/*DPLL 部分 1-26  */

void clearExtarr()
//1.清零外部数组 
{
	if(style == 2)
		for(int i = 0;i < 2 * MAX_VAR + 1;i++)
			score[i] = 0;
	else if(style == 3 || style == 4)
		for(int i = 0;i < 2 * MAX_VAR + 1;i++)
			jw[i] = 0.0;
	else	return ;
	return ;
}
void initCNF(CNF &S, int var)
//2.初始化 
{
	S.num_clau = 0;
	S.num_var = var;
	S.head = NULL;
	S.tail = NULL;
	S.exist_emptyclause = false;
}

void clearCNF(CNF &S,LITSHEET* ans)
//3.清除CNF并释放之前的内存;   
{
    if(S.head != NULL) {
     	CLAUSE *current = S.head;
         while (current != NULL) {
            CLAUSE *nextClause = current->next;
            LITERAL *currentLit = current->lit;
            while (currentLit != NULL) {
                LITERAL *nextLit = currentLit->next;
                free(currentLit);
                currentLit = nextLit;
            }
            free(current);
            current = nextClause;
        }
    }
     
    for(int i = 0;i <= S.num_var; i++){
    	INFO_L *p = ans[i].pos,*next = p;
    	while(p){
    		next = p->next;
    		free(p);
    		p = next;
		}
		p = ans[i].nag;	next = p;
    	while(p){
    		next = p->next;
    		free(p);
    		p = next;
		}
				
 		ans[i].ans = 0;
 		ans[i].pos = NULL;
 		ans[i].nag = NULL;
 	}
 	
    S.head = NULL;
    S.tail = NULL;
    S.num_clau = 0;
    S.num_var = 0;
    S.exist_emptyclause = false;
}

void createClause(CNF &S)
//4.	创建子句
{
    CLAUSE *newClause = (CLAUSE *)malloc(sizeof(CLAUSE));
    newClause->lit = NULL;
    newClause->num = 0;
    newClause->isTrue = false;
    newClause->next = NULL;

    if(S.head == NULL) {
        S.head = newClause;
        S.tail = newClause; 
    } else {
        S.tail->next = newClause;
        S.tail = newClause;
    }
}

bool buildCNF(CNF &S, int num_clau, FILE *fp, LITSHEET* ans)
//5. 构建CNF,并读入数据
{
	if(S.head != NULL) return false;

    for(int i = 0;i < num_clau;i++) {
        int a = 0;
        bool flag = false;
        createClause(S);

        while((fscanf(fp, "%d", &a)) != EOF && a != 0) {
        	if(a > S.num_var){
        		printf("Dirty Data! Beyond var_range!\n");
        		exit(INFEASIBLE);
			}
			flag = true;
            LITERAL *newLiteral = (LITERAL *)malloc(sizeof(LITERAL));
            newLiteral->value = a; // 设置文字的值
            newLiteral->next = NULL; // 初始化后指针

            if(S.tail->lit == NULL) {
                S.tail->lit = newLiteral; // 如果子句中没有文字，设置为新文字
            } else {
                LITERAL *lastLit = S.tail->lit;
                //  采用首插法
                newLiteral->next = lastLit;
                S.tail->lit = newLiteral; // 将新文字设置为子句的第一个文字
            }
            S.tail->num ++;

			buildLitsheet(ans, S.tail, a);
        }
        
        if(!flag){
        	if(a == 0)	S.exist_emptyclause = true;				//	出现空字句
        	return false;								 		//	或文件有问题 
		}	

    }
    
    return true;
}

void buildLitsheet(LITSHEET* ans, CLAUSE *node, int value)
//6.	将每个文字所出现的子句标联系上 
{
	int index = abs(value);
	INFO_L *lit = (INFO_L *)malloc (sizeof(INFO_L));
	lit->clause = node;
	lit->next = NULL;
	
	if(value > 0){
		if(ans[index].pos == NULL){
			ans[index].pos = lit;
		}
		else{
			lit->next = ans[index].pos;
			ans[index].pos = lit;
		}
//		ans[index].posfre ++;
	}else{
		if(ans[index].nag == NULL){
			ans[index].nag = lit;
		}
		else{
			lit->next = ans[index].nag;
			ans[index].nag = lit;
		}
//		ans[index].nagfre ++;		
	}
}

status CNFparser(CNF &S, char file[], LITSHEET* ans)
//7. 解析CNF文件
{
    FILE *fp;
    char c; int t;

    if((fp = fopen(file, "r")) == NULL) {
        printf("Can't open file.\n");
        return FALSE;
    }
// 处理数据之前的信息
    while((c = fgetc(fp)) != EOF) {
        // c开头为注释行
        if(c == 'c') {
            while((c = fgetc(fp)) != EOF && c != '\n'); 
            continue;
        }
        // p开头为信息行        
        if(c == 'p') {
            // 读取"cnf"
            char tmp[10];
            fscanf(fp, "%s", tmp); 
            
            if(strcmp(tmp, "cnf") != 0) {
                printf("Error: Invalid CNF format.\n");
                fclose(fp);
                return FALSE;
            }

            fscanf(fp, "%d %d", &S.num_var, &S.num_clau);
            break;
        }
    }

    if(!buildCNF(S, S.num_clau, fp, ans)) {
        printf("Error: CNF's building fail.\n");
        fclose(fp);
        free(ans);
        return FALSE; 
    }

    printf("CNF created with %d clauses and %d variables.\n", S.num_clau, S.num_var);

    fclose(fp);
	return TRUE;
}

void showCNF(CNF &S)
//8. 显示CNF
{
    printf("\nCNF with %d clauses and %d variables:\n", S.num_clau, S.num_var);
    CLAUSE *curr = S.head;
    int cnt = 1;
    while(curr){
        printf("Clause %d: ", cnt++);
        LITERAL *lit = curr->lit;
        while(lit) {
            printf("%d ", lit->value);
            lit = lit->next;
        }
        printf("\n");
        printf("bool: %d\nlits: %d\n",curr->isTrue, curr->num);
        curr = curr->next;
    }
    printf("\n"); 
}

void fshowCNF(CNF &S)
//9
{
	FILE *test = fopen("check.txt","w");
    fprintf(test,"\nCNF with %d clauses and %d variables:\n", S.num_clau, S.num_var);
    CLAUSE *curr = S.head;
    int cnt = 1;
    while(curr){
       fprintf(test,"Clause %d: ", cnt++);
        LITERAL *lit = curr->lit;
        while(lit) {
            fprintf(test,"%d ", lit->value);
            lit = lit->next;
        }
        fprintf(test,"%s","\n");
        fprintf(test,"bool: %d\nlits: %d\n",curr->isTrue, curr->num);
        curr = curr->next;
    }
    fprintf(test,"%s","\n");
	fclose(test);
}

void showLitsheet(LITSHEET* ans, int range)
//10.展示文字出现在自己的次数 ，不包含0 
{
	int cnt;
	for(int i = 1;i <= range; i++){
		cnt = 0;
		printf("var %d in status: %d\n",i,ans[i].ans); 
		INFO_L *pos = ans[i].pos, *nag = ans[i].nag;
		while(pos){
			cnt ++;
			pos = pos->next;
		}
		printf("%d occur in %d clause\n",i,cnt);
		cnt = 0;
		while(nag){
			cnt ++;
			nag = nag->next;
		}		
		printf("%d occur in %d clause\n",-i,cnt);
	}
}

void showLitans(LITSHEET* ans, int range)
//10.展示文字当前所赋的值
{
	int cnt;
	for(int i = 1;i <= range; i++){
		printf("var %d in status : %d\n",i,ans[i].ans);
	}
	printf("\n");
}

void showClause(CLAUSE *head,LITSHEET *ans)
//11显示某一子句 
{
	CLAUSE *node = head;
	if(!node)	return;
	
	LITERAL *lit = node->lit;
	printf("the clause is %d now has %d literals \n",node->isTrue,node->num);
	
	while(lit){
		printf("%d ",lit->value);
		printf("ans = %d\n",ans[abs(lit->value)].ans);
		lit = lit ->next;
	}
	printf("\n");
}

CLAUSE *existUnitClause(CLAUSE *head)
//12. 在子句链表中查找第一个未判定真值的单子句
{
    CLAUSE *current = head;
    while (current != NULL) {
        if (current->isTrue == false && current->num == 1) {
            return current; 
        }
        current = current->next;
    }
    return NULL;
}

bool existEmptyClause(CLAUSE *node)
//13.	判断是否存在空子句 
{
	CLAUSE *head = node;
	while(head){
		if(!head->num && !head->isTrue) return true;
		head = head->next;
	}	
	return false;
}

CLAUSE *locatePre(CLAUSE *node,CLAUSE *head)
//14.获取目标节点的前驱 
{
	if(node == NULL || head == NULL){
		printf("目标节点或头结点为空\n");
		return NULL;
	}	
	
	CLAUSE *curr = head;
	if(curr == node)	return NULL;
	while(curr){
		
		if(curr->next == node)
			return curr;
		
		curr = curr->next;
	}
	
	return NULL;
}

int deleteOneClause(CLAUSE *node, SqList &L, CNF &S, LITSHEET* ans) 
//删除单子句，将指针标记改为true，存真子句地址于数组中，返回单子句中变量的值 
{
	if(!node || !node->lit){
		printf("ERROR! clause DON'T exist,can't delete\n");
		return 0;
	}	
			
	if( S.head == NULL){
		printf("ERROR! CNF already empty\n");
		return 0;
	}
	
	if(node->num != 1){
		printf("ERROR! Unitclause has %d clauses,showClause:\n",node->num);
		showClause(node,ans);
		printf("\n");
	}

	if(node->isTrue == true){
		printf("existUnitClause has bug, node is already true\n");
		return 0;
	}
		
	node->isTrue = true;		//代表删除
	S.num_clau --;
	ListInsert(L,node);

	LITERAL *lit = node->lit;
	int value;
	 
	while(lit) {
		value = lit->value;
		if(ans[abs(value)].ans == 0){
			return value;
		}					//对于连带的文字更新其score 
		lit = lit->next;
	}	
	
	printf("ERROR! Cannot find any lit in unitclause:\n");
	showClause(node,ans); 
	printf("\n");

	return 0;
}

bool deleteOneClause_slt_1(CLAUSE *node, SqList &L, CNF &S) 
//15.1删除真子句，存真子句地址于数组中，不用返回值 
{
	if(!node || !node->lit ||!node->num){
		printf("DON'T exist,can't delete\n");
		return false;
	}			
	if( S.head == NULL){
		printf("CNF already empty\n");
		return false;
	}

	if(node->isTrue == true){
		printf("existUnitClause has bug node is already true\n");
	}
		
	node->isTrue = true;			//删除
	S.num_clau --;
	ListInsert(L,node);

	return true;
}

bool deleteOneClause_slt_2(CLAUSE *node, SqList &L, CNF &S,LITSHEET*ans) 
//15.2删除真子句，存真子句地址于数组中，动态更新score 
{
	if(!node || !node->lit ||!node->num){
		printf("DON'T exist,can't delete\n");
		return false;
	}			
	if( S.head == NULL){
		printf("CNF already empty\n");
		return false;
	}

	if(node->isTrue == true){
		printf("existUnitClause has bug node is already true\n");
	}
//优化：动态更新score数组	
	LITERAL *lit = node->lit;
	int value;	 
	while(lit) {
		value = lit->value;
		if(ans[abs(value)].ans == 0){
			score[MAX_VAR + value] --;
		}					//对于连带的文字更新其score 
		lit = lit->next;
	}	
	
	node->isTrue = true;			//删除
	S.num_clau --;
	ListInsert(L,node);

	return true;
}

bool deleteOneClause_slt_3(CLAUSE *node, SqList &L, CNF &S,LITSHEET*ans) 
//15.3删除真子句，存真子句地址于数组中，动态更新jw
{
	if(!node || !node->lit ||!node->num){
		printf("DON'T exist,can't delete\n");
		return false;
	}			
	if( S.head == NULL){
		printf("CNF already empty\n");
		return false;
	}

	if(node->isTrue == true){
		printf("existUnitClause has bug node is already true\n");
	}
//优化：动态更新jw数组	
	LITERAL *lit = node->lit;
	int value;	 
	while(lit) {
		value = lit->value;
		if(ans[abs(value)].ans == 0){
			if(style == 3)
				jw[MAX_VAR + value] -= pow(2.0, - (double)node->num);
			else if(style == 4)
				jw[MAX_VAR + value] -= pow(2.0, - (double)node->num) * ALPHA + BETA;
		}					//对于连带的文字更新其jw 
		lit = lit->next;
	}	
	
	node->isTrue = true;			//删除
	S.num_clau --;
	ListInsert(L,node);

	return true;
}

int deleteClause(SqList &L, CNF &S, LITSHEET* ans, SqBack &backtrace)
//16. 删除所有单子句，并对CNF进行化简；记录操作次数,将单子句中变量值记录 
{
	if(S.num_clau == 0 || !S.head){
		//代表上一步deleteLit后CNF为空，DPLL成功，结束此函数即可
		return 0;
	}		 
	if(S.exist_emptyclause){
		//代表上一步deleteLit后CNF中出现空子句，DPLL失败，结束此函数即可
		return 0;
	}	
	
	CLAUSE *node = existUnitClause(S.head);
	int cnt = 0, value = 0;
	
	while(node != NULL){
		// 找到并删除单子句
//		if(cnt >= MEMORY_MAX){
//			//假如说单子句这里就很大了，后面deletelit更多，截断一下
//			printf("Too Much UClaus, we need to use another DPLL to load them!\n");
//			return cnt;
//		}
		if(node->isTrue == true){
			printf("ERROR in 'existUnitClause' unitclause is already true\n");
			node = existUnitClause(node->next);
			continue;
		}	 

		//先不更新ans，因为在delOneCl中要靠ans找没删除的文字
		value = deleteOneClause(node, L, S,ans);

		// 更新ans记录 
		if(value > 0){
			ans[value].ans = 1;
		}else if(value < 0){
			ans[-value].ans = 2;
		}else{
			printf("ERROR in 'deleteOneClause',clau is empty to be delete\n");
			node = existUnitClause(S.head);		
			continue;			
		}	
		
		cnt ++;
		BackInsert(backtrace, value);
		
		// 化简CNF,并记录删除真子句数
		cnt += deleteLit(L, S, value, ans);		

		//  PRUNE	
		if(S.exist_emptyclause == true){
			/*printf("发现空子句，删除单子句就此中断\n");*/
			return cnt;
		}	

		node = existUnitClause(S.head);	//	这样可以保证所有单子句被一网打尽（但是稍微慢了点		 
	}
	
	return cnt;
}

int deleteLit(SqList&L, CNF &S, int value, LITSHEET *ans)
//17.	删除子句中的文字，同时也会删除真子句并记录真子句次数 
{
	if(!value) {
		printf("ERROR! value = 0, cannot delete\n");
		return 0;
	}
	if(!S.num_clau || !S.head ){//这种情况下S为空,CNF已经有解，直接退出 
//		printf("S already empty, cannot delete\n");
		return 0;
	}
	
	int cnt = 0, index = abs(value);		//	直接删除包含value的子句，删除-value的文字 
	INFO_L * pos = ans[index].pos;	INFO_L *nag = ans[index].nag;
	bool flag = false;
	
	if(value > 0){	
		while(pos && pos->clause){

			if(pos->clause->isTrue == false) {
				if(style == 1){
					deleteOneClause_slt_1(pos->clause, L,S);
				}else if(style == 2){
					deleteOneClause_slt_2(pos->clause, L,S,ans);
				}else if(style == 3){
					deleteOneClause_slt_3(pos->clause, L,S,ans);
				}else if(style == 4){
					deleteOneClause_slt_3(pos->clause, L,S,ans);
				}	
				cnt ++;
			}
			
			pos->clause->num --;

			if(pos->clause->num < 0){
				printf("OVERFLOW!\n");
			}	
			pos = pos->next;
		}
		
		while(nag && nag->clause){	
			
			nag->clause->num --;	

			if(nag->clause->num < 0){
				printf("OVERFLOW!\n");
			}		
			// 删除负文字时，更新判定情况 
			if(nag->clause->num == 0 && nag->clause->isTrue == false)	flag = true;

			nag = nag->next;
		}
		
	}else if(value < 0){
		while(nag && nag->clause){

			if(nag->clause->isTrue == false) {
				if(style == 1){
					deleteOneClause_slt_1(nag->clause, L,S);
				}else if(style == 2){
					deleteOneClause_slt_2(nag->clause, L,S,ans);
				}else if(style == 3){
					deleteOneClause_slt_3(nag->clause, L,S,ans);
				}else if(style == 4){
					deleteOneClause_slt_3(nag->clause, L,S,ans);
				}				
				cnt ++;
			}
			
			nag->clause->num --;	

			if(nag->clause->num < 0){
				printf("OVERFLOW!\n");
			}	
			nag = nag->next;
		}
		
		while(pos && pos->clause){		
	
			pos->clause->num --;

			if(pos->clause->num < 0){
				printf("OVERFLOW!\n");
			}		

			if(pos->clause->num == 0 && pos->clause->isTrue == false)	flag = true;
			
			pos = pos->next;
		}				
	}else{
		printf("FUCK ! value = 0,ERROR,deletelit failed\n");
		return cnt;
	}

	if(flag == true){			//	存在空子句 
		S.exist_emptyclause = true;	
	}	
	return cnt;
}

int choose_lit_1(CNF&S, LITSHEET* ans)
//18.1	选false子句的第一个未定文字 
{
//	CLAUSE *big = existUnitClause(S.head);
//	if(big) {
//		lit = big->lit;
//		while(lit){
//			i = lit->value;
//			if(ans[abs(i)].ans == 0)		return i;
//			lit = lit->next;
//		}
//		printf("ERROR in choose_lit:UnitClause has no lit.\n");	
//		showClause(big,ans);			
//		return 0;
//	}
	CLAUSE *node = S.head;
	LITERAL *lit = NULL;
	int i ;
	while(node){
		if(node->isTrue == false && node->num > 0) {
			lit = node->lit;
			while(lit){
				i = lit->value;
				if(ans[abs(i)].ans == 0)		return i;
				lit = lit->next;
			}
			printf("ERROR in choose_lit:the chosen Clause has no lit.\n");
			showClause(node,ans);
			return 0;
		}
		node = node->next;
	}				
	
	return 0;
}

void clit_2_init(int num, LITSHEET* ans)
//18.2	选目前在更多false子句中出现的文字 
{
	INFO_L *lit = NULL;
	score[MAX_VAR] = num;
	for(int i = 1;i <= num; i++){
		if(ans[i].ans == 0){
			score[MAX_VAR + i] = 0;
			if(ans[i].pos){
				lit = ans[i].pos;
				while(lit){
					if(lit->clause && lit->clause->isTrue == false)	score[MAX_VAR + i] ++;
					
					lit = lit->next;
				}					
			}
			score[MAX_VAR - i] = 0;
			if(ans[i].nag){
				lit = ans[i].nag;
				while(lit){
					if(lit->clause && lit->clause->isTrue == false)	score[MAX_VAR - i] ++;
					
					lit = lit->next;
				}					
			}	
							
		}
	}

}

void clit_jw_init(int num, LITSHEET* ans)
// 18.3	Jeroslow-Wang:在还没结果的var中选择子句尽量短的 
{
	jw[MAX_VAR] = (double)num;
	for(int i = 1; i <= num; ++i){
		if(ans[i].ans != 0) continue;
		
		INFO_L* p = ans[i].pos;
		while(p){
			if(p->clause && p->clause->isTrue == false && p->clause->num > 0){
				jw[MAX_VAR + i] += pow(2.0, - (double)p->clause->num);
			}
			p = p->next;
		}

		p = ans[i].nag;
		while(p){
			if(p->clause && p->clause->isTrue == false && p->clause->num > 0){
				jw[MAX_VAR - i] += pow(2.0, - (double)p->clause->num);
			}
			p = p->next;
		}
	}
}

void clit_hybrid_init(int num, LITSHEET* ans)
//18.4 将jw和2混合起来，算综合加权评分 
{
	jw[MAX_VAR] = (double)num;
	
	for(int i = 1;i <= num; i++){
		if(ans[i].ans != 0) continue;

		// 方法2
		int cnt_pos = 0, cnt_neg = 0;
		INFO_L *lit = ans[i].pos;
		while(lit){
			if(lit->clause && lit->clause->isTrue == false) cnt_pos++;
			
			lit = lit->next; 
		}
		lit = ans[i].nag;
		while(lit){
			if(lit->clause && lit->clause->isTrue == false) cnt_neg++; 
			
			lit = lit->next; 
		}
		//	jw方法 
		double pos_jw = 0.0, neg_jw = 0.0;
		INFO_L *p = ans[i].pos; 
		while(p){
		 	if(p->clause && p->clause->isTrue == false && p->clause->num > 0) 	pos_jw += pow(2.0, -(double)p->clause->num); 
		 
		 	p = p->next; 
		 }
		p = ans[i].nag; 
		while(p){ 
			if(p->clause && p->clause->isTrue==false && p->clause->num>0) 		neg_jw += pow(2.0, -(double)p->clause->num); 
			
			p = p->next; 
		}

		jw[MAX_VAR + i] = ALPHA * pos_jw + BETA * (double)cnt_pos;
		jw[MAX_VAR - i] = ALPHA * neg_jw + BETA * (double)cnt_neg;
	}
}

int choose_lit_int(LITSHEET *ans)
//int 选得分最高的 
{
	int max = 0;
	int record = 0;
	for(int i = 1;i <= score[MAX_VAR];i++){
		if(i == MAX_VAR)	continue;
		if(ans[i].ans)		continue;
		if(score[MAX_VAR - i] > max){
			record = -i;
			max = score[MAX_VAR - i];
		}
		if(score[MAX_VAR + i] > max){
			record = i;
			max = score[MAX_VAR + i];
		}		
	}
	return record;
}
int choose_lit_dou(LITSHEET *ans)
//double 选得分最高的 
{
	double max = 0.0;
	int record = 0;
	for(int i = 1;i <= (int)jw[MAX_VAR];i++){
		if(i == MAX_VAR)	continue;
		if(ans[i].ans)		continue;
		if(jw[MAX_VAR - i] > max){
			record = -i;
			max = jw[MAX_VAR - i];
		}
		if(jw[MAX_VAR + i] > max){
			record = i;
			max = jw[MAX_VAR + i];
		}		
	}
	return record;
}

bool DPLL_1(CNF &S, LITSHEET* ans)
//19.1	CORE
{
#ifdef CHECK
	static int depth = 0;
	static int depth_max = 0;

	depth ++;

	if(depth > depth_max){
		depth_max = depth;
	}
	printf("本次DPLL前子句数目为%d\n",S.num_clau);
	printf("DPLL depth rightnow = %d\n",depth);
	showCNF(S);
#endif

	SqList L;	InitList(L);						//记录删除的真子句的地址，便于回溯时复原
	SqBack backtrace;	InitBack(backtrace);		//记录删除真子句时删除并确定值的文字，.length为个数
 	int cntCLAU = 0;
 	int chosenlit = 0;
//1.删掉并化简上一轮DPLL所选择的文字
 	if(ans[0].ans != 0) {				
 		chosenlit = ans[0].ans;
 		cntCLAU += deleteLit(L, S, chosenlit, ans);	 
 			 
 		backtrace.length ++;
 		backtrace.elem[0] = chosenlit;
	
 		if(S.exist_emptyclause) {			//Prune（不要放在更新backtrace数组之前！改了2天... 
			restore_lit(backtrace, ans);
			restore_cl_1(L, S);
			DestroyList(L);
			DestroyBack(backtrace);
//			depth --;	
			return false; 			
		 } 		
	}

//	if(L.length != cntCLAU){
//		printf("ERROR in DPLL, cntCLAU = %d, L.length = %d\n",cntCLAU,L.length);
//	}
	
// 2.删除所有单子句，删除包含该文字的子句，删去反文字,并记录操作次数
	cntCLAU += deleteClause(L,S, ans, backtrace);

//	if(L.length != cntCLAU){
//		printf("ERROR in DPLL, cntCLAU = %d, L.length = %d\n",cntCLAU,L.length);
//	}

//3.检查当前CNF状态
 		//3.1.成功解出 
	if(S.num_clau == 0 || S.head == NULL ){
		recover_lit(backtrace, ans);
		restore_cl_1(L, S);
		DestroyList(L);
		DestroyBack(backtrace);
		return true;		
	}	//3.2.出现矛盾
	else if(S.exist_emptyclause) {
		restore_lit(backtrace, ans);
		restore_cl_1(L, S);
		DestroyList(L);
		DestroyBack(backtrace);
//		depth --;
		return false;		
	}	
// 4.选取一个变量赋值 
	int chosen = choose_lit_1(S,ans);
	int index = abs(chosen);
	#ifdef CHECK	
	printf("choose %d as a break\n",chosen);
	#endif
	if(!index){		
		restore_lit(backtrace, ans);
		restore_cl_1(L, S);
		DestroyList(L);
		DestroyBack(backtrace);
//		depth --;
		return false;		
	}
	
	//4.1
	ans[index].ans = (chosen > 0) ? 1:2;
	ans[0].ans = chosen;
	
	if(DPLL_1(S, ans)){
		recover_lit(backtrace, ans);
		restore_cl_1(L, S);
		DestroyList(L);
		DestroyBack(backtrace);
		return true;
	}
	//4.2
	ans[index].ans = (chosen > 0) ? 2:1;
	ans[0].ans = -chosen;

	if(DPLL_1(S, ans)){
		recover_lit(backtrace, ans);
		restore_cl_1(L, S);
		DestroyList(L);
		DestroyBack(backtrace);
		return true;
	}
//	5.当前分支失败，开始回溯 
	ans[index].ans = 0;
	
	restore_lit(backtrace, ans);	   
	restore_cl_1(L, S);    
	DestroyList(L);
	DestroyBack(backtrace);
//	depth --;
	return false;
}

bool DPLL_2(CNF &S, LITSHEET* ans)
//19.2	CORE
{
#ifdef CHECK
	static int depth = 0;
	static int depth_max = 0;
	depth ++;
	if(depth > depth_max){
		depth_max = depth;
	}
	printf("本次DPLL前子句数目为%d\n",S.num_clau);
	printf("DPLL depth rightnow = %d\n",depth);
	showCNF(S);
#endif

	SqList L;	InitList(L);						//记录删除的真子句的地址，便于回溯时复原
	SqBack backtrace;	InitBack(backtrace);		//记录删除真子句时删除并确定值的文字，.length为个数
 	int cntCLAU = 0;
 	int chosenlit = 0;
//1.删掉并化简上一轮DPLL所选择的文字
 	if(ans[0].ans != 0) {				
 		chosenlit = ans[0].ans;
 		cntCLAU += deleteLit(L, S, chosenlit, ans);	 
 			 
 		backtrace.length ++;
 		backtrace.elem[0] = chosenlit;
	
 		if(S.exist_emptyclause) {			//Prune（不要放在更新backtrace数组之前！改了2天... 
			restore_lit(backtrace, ans);
			restore_cl_2(L, S,ans);
			DestroyList(L);
			DestroyBack(backtrace);
//			depth --;	
			return false; 			
		 } 		
	}

//	if(L.length != cntCLAU){
//		printf("ERROR in DPLL, cntCLAU = %d, L.length = %d\n",cntCLAU,L.length);
//	}
	
// 2.删除所有单子句，删除包含该文字的子句，删去反文字,并记录操作次数
	cntCLAU += deleteClause(L,S, ans, backtrace);

//	if(L.length != cntCLAU){
//		printf("ERROR in DPLL, cntCLAU = %d, L.length = %d\n",cntCLAU,L.length);
//	}

//3.检查当前CNF状态
 		//3.1.成功解出 
	if(S.num_clau == 0 || S.head == NULL ){
		recover_lit(backtrace, ans);
		restore_cl_2(L, S,ans);
		DestroyList(L);
		DestroyBack(backtrace);
		return true;		
	}	//3.2.出现矛盾
	else if(S.exist_emptyclause) {
		restore_lit(backtrace, ans);
		restore_cl_2(L, S,ans);
		DestroyList(L);
		DestroyBack(backtrace);
//		depth --;
		return false;		
	}	
// 4.选取一个变量赋值 
	int chosen = choose_lit_int(ans);
	int index = abs(chosen);
	#ifdef CHECK	
	printf("choose %d as a break\n",chosen);
	#endif
	
	if(!index){		
		restore_lit(backtrace, ans);
		restore_cl_2(L, S,ans);
		DestroyList(L);
		DestroyBack(backtrace);
//		depth --;
		return false;		
	}
	//4.1
	ans[index].ans = (chosen > 0) ? 1:2;
	ans[0].ans = chosen;
	
	if(DPLL_2(S, ans)){
		recover_lit(backtrace, ans);
		restore_cl_2(L, S, ans);
		DestroyList(L);
		DestroyBack(backtrace);
		return true;
	}
	//4.2
	ans[index].ans = (chosen > 0) ? 2:1;
	ans[0].ans = -chosen;

	if(DPLL_2(S, ans)){
		recover_lit(backtrace, ans);
		restore_cl_2(L, S,ans);
		DestroyList(L);
		DestroyBack(backtrace);
		return true;
	}
//	5.当前分支失败，开始回溯 
	ans[index].ans = 0;	
	restore_lit(backtrace, ans);	   
	restore_cl_2(L, S, ans);    
	DestroyList(L);
	DestroyBack(backtrace);
//	depth --;
	return false;
}

bool DPLL_3(CNF &S, LITSHEET* ans)
//19.3	CORE
{
#ifdef CHECK
	static int depth = 0;
	static int depth_max = 0;
	depth ++;
	if(depth > depth_max){
		depth_max = depth;
	}
	printf("本次DPLL前子句数目为%d\n",S.num_clau);
	printf("DPLL depth rightnow = %d\n",depth);
	showCNF(S);
#endif
	SqList L;	InitList(L);						//记录删除的真子句的地址，便于回溯时复原
	SqBack backtrace;	InitBack(backtrace);		//记录删除真子句时删除并确定值的文字，.length为个数
 	int cntCLAU = 0;
 	int chosenlit = 0;
//1.删掉并化简上一轮DPLL所选择的文字
 	if(ans[0].ans != 0) {				
 		chosenlit = ans[0].ans;
 		cntCLAU += deleteLit(L, S, chosenlit, ans);	 
 			 
 		backtrace.length ++;
 		backtrace.elem[0] = chosenlit;
	
 		if(S.exist_emptyclause) {			//Prune（不要放在更新backtrace数组之前！改了2天... 
			restore_lit(backtrace, ans);
			restore_cl_3(L, S,ans);
			DestroyList(L);
			DestroyBack(backtrace);
//			depth --;	
			return false; 			
		 } 		
	}

//	if(L.length != cntCLAU){
//		printf("ERROR in DPLL, cntCLAU = %d, L.length = %d\n",cntCLAU,L.length);
//	}
	
// 2.删除所有单子句，删除包含该文字的子句，删去反文字,并记录操作次数
	cntCLAU += deleteClause(L,S, ans, backtrace);

//	if(L.length != cntCLAU){
//		printf("ERROR in DPLL, cntCLAU = %d, L.length = %d\n",cntCLAU,L.length);
//	}

//3.检查当前CNF状态
 		//3.1.成功解出 
	if(S.num_clau == 0 || S.head == NULL ){
		recover_lit(backtrace, ans);
		restore_cl_3(L, S,ans);
		DestroyList(L);
		DestroyBack(backtrace);
		return true;		
	}	//3.2.出现矛盾
	else if(S.exist_emptyclause) {
		restore_lit(backtrace, ans);
		restore_cl_3(L, S,ans);
		DestroyList(L);
		DestroyBack(backtrace);
//		depth --;
		return false;		
	}	
// 4.选取一个变量赋值 
	int chosen = choose_lit_dou(ans);
	int index = abs(chosen);
	#ifdef CHECK	
	printf("choose %d as a break\n",chosen);
	#endif
	
	if(!index){		
		restore_lit(backtrace, ans);
		restore_cl_3(L, S,ans);
		DestroyList(L);
		DestroyBack(backtrace);
//		depth --;
		return false;		
	}
	//4.1
	ans[index].ans = (chosen > 0) ? 1:2;
	ans[0].ans = chosen;
	
	if(DPLL_3(S, ans)){
		recover_lit(backtrace, ans);
		restore_cl_3(L, S, ans);
		DestroyList(L);
		DestroyBack(backtrace);
		return true;
	}
	//4.2
	ans[index].ans = (chosen > 0) ? 2:1;
	ans[0].ans = -chosen;

	if(DPLL_3(S, ans)){
		recover_lit(backtrace, ans);
		restore_cl_3(L, S,ans);
		DestroyList(L);
		DestroyBack(backtrace);
		return true;
	}
//	5.当前分支失败，开始回溯 
	ans[index].ans = 0;	
	restore_lit(backtrace, ans);	   
	restore_cl_3(L, S, ans);    
	DestroyList(L);
	DestroyBack(backtrace);
//	depth --;
	return false;
}

status restore_cl_1(SqList &L, CNF &S)
//20.1	恢复若干次子句bool
{
#ifdef CHECK
	printf("恢复子句中，DPLL化简后CNF有%d个子句\n",S.num_clau);
#endif
//	showCNF(S);
	for(int i = 0;i < L.length; i++){
		
		if(!L.elem)	{
			printf("L.elem is NULL\n");
			return FALSE;
		}
		
		if(L.elem[i]->isTrue == false){
			printf("ERROR in deleteoneclau,the flag is still false\n");
			return FALSE;
		}
		L.elem[i]->isTrue = false;

		S.num_clau ++;
	}	

	if(S.exist_emptyclause == true)	S.exist_emptyclause = false;

#ifdef CHECK
	printf("恢复完成，现在CNF有%d个子句\n",S.num_clau);
#endif
	return TRUE;
}

status restore_cl_2(SqList &L, CNF &S, LITSHEET*ans)
//20.2	恢复若干次子句bool和score 
{
#ifdef CHECK
	printf("恢复子句中，DPLL化简后CNF有%d个子句\n",S.num_clau);
#endif
	for(int i = 0;i < L.length; i++){
		
		if(!L.elem)	{
			printf("L.elem is NULL\n");
			return FALSE;
		}
		
		if(L.elem[i]->isTrue == false){
			printf("ERROR in deleteoneclau,the flag is still false\n");
			return FALSE;
		}
		L.elem[i]->isTrue = false;

		S.num_clau ++;
		//	优化开始 
		LITERAL *lit = L.elem[i]->lit;
		while(lit){
			int value = lit->value;
			if(ans[abs(value)].ans== 0)	score[MAX_VAR + value] ++;
			lit = lit->next;
		} 		
	}	

	if(S.exist_emptyclause == true)	S.exist_emptyclause = false;

#ifdef CHECK
	printf("恢复完成，现在CNF有%d个子句\n",S.num_clau);
#endif
	return TRUE;
}

status restore_cl_3(SqList &L, CNF &S, LITSHEET*ans)
//20.3	恢复若干次子句bool和jw 
{
#ifdef CHECK
	printf("恢复子句中，DPLL化简后CNF有%d个子句\n",S.num_clau);
#endif
	for(int i = 0;i < L.length; i++){
		
		if(!L.elem)	{
			printf("L.elem is NULL\n");
			return FALSE;
		}
		
		if(L.elem[i]->isTrue == false){
			printf("ERROR in deleteoneclau,the flag is still false\n");
			return FALSE;
		}
		L.elem[i]->isTrue = false;

		S.num_clau ++;
		//	优化开始 
		LITERAL *lit = L.elem[i]->lit;
		while(lit){
			int value = lit->value;
			if(ans[abs(value)].ans== 0){
				if(style == 3)	jw[MAX_VAR + value] += pow(2.0, - (double)L.elem[i]->num);
				else if(style == 4)	jw[MAX_VAR + value] += ALPHA * pow(2.0, - (double)L.elem[i]->num) + BETA;
			}	
			lit = lit->next;
		} 		
	}	

	if(S.exist_emptyclause == true)	S.exist_emptyclause = false;

#ifdef CHECK
	printf("恢复完成，现在CNF有%d个子句\n",S.num_clau);
#endif
	return TRUE;
}

status restore_lit(SqBack &back, LITSHEET* ans)
//21.	恢复 变元bool值，子句的文字数，不管子句bool 
{
	if(!back.elem){
		printf("ERROR! back[] is empty!\n");
		return FALSE;
	}	

#ifdef CHECK
	printf("恢复文字中，展示back数组：一共%d个文字被恢复\n",back.length);
#endif

	for(int i = 0;i < back.length ;i++){
		int index = abs(back.elem[i]);
			
		ans[index].ans = 0;		//bool值
		
		INFO_L* p = ans[index].pos;
		
		while(p && p->clause){
			p->clause->num ++;
			p = p->next;
		}
		
		p = ans[index].nag;
		
		while(p && p->clause){
			p->clause->num ++;
			p = p->next;
		}	
#ifdef CHECK
	printf("%d \n",back.elem[i]);
#endif

	}	
	return TRUE;
}

void recover_lit(SqBack &back, LITSHEET* ans)
//22.	将删去的文字数全部加回来,保留原来的ans值 
{ 
	if(!back.elem){
		printf("ERROR! back[] is empty!\n");
		return;
	}
	
	for(int i = 0;i < back.length ;i++){
		int index = abs(back.elem[i]);
		
		INFO_L* p = ans[index].pos;
		
		while(p){
			p->clause->num ++;
			p = p->next;
		}
		
		p = ans[index].nag;
		
		while(p){
			p->clause->num ++;
			p = p->next;
		}	
			
	}		
}

bool autocheck(CNF &S,LITSHEET* ans)
//23.自动校验求解是否正确
{
	CLAUSE *clause = S.head;
	int value;
	bool flag = false;
	
	while(clause){				//check 每一个子句
		
		LITERAL *lit = clause->lit;
		while(lit){
			value = lit->value;
			//填什么都可以的时候默认填TRUE
			if((value > 0 && ans[value].ans == 1) || (value < 0 && ans[-value].ans == 2) || (value > 0 && ans[value].ans == 0)){
				flag = true;
				break;
			}
			lit = lit->next;
		}
		if(!flag)	return false;
		
		flag = false;
		clause = clause->next;
	}

	return true;
}

void check(LITSHEET *ans, int cnt)
//24.	输出直观结果 
{
	for(int i =1; i <= cnt; i++){
		printf("var %d :",i);
		if(ans[i].ans == 1)	printf(" TRUE\n");
		else if(ans[i].ans  == 2)	printf(" FALSE\n");
		else printf("Whatever\n"); 
	}
	printf("\n");
}

void check(FILE *fp,LITSHEET *ans,int cnt)
//25.
{
	for(int i =1; i <= cnt; i++){
		fprintf(fp,"var %d :",i);
		if(ans[i].ans  == 1)	fprintf(fp,"%s"," TRUE\n");
		else if(ans[i].ans  == 2)	fprintf(fp,"%s"," FALSE\n");
		else fprintf(fp,"%s","Whatever\n"); 
	}
	fprintf(fp,"%s","\n");
}

status saveOutput(LITSHEET* ans, int cnt, char file[], double used_time)
//26.	按要求保存结果文件 
{
	char output[FILE_MAX];
	int t;
	strcpy(output, file);
	strcat(output, ".res");
	
	FILE *fp = fopen(output,"w");
	if(!fp)	return FALSE;
	
	fprintf(fp, "%s%d\n","s ",ans[0].ans );
	
	if(ans[0].ans){
		
		fprintf(fp,"%s","v ");
		for(int i = 1;i <= cnt;i ++){
			/**/
			if(ans[i].ans == 1)	t = i;
			else if(ans[i].ans == 2)	t = -1 * i;
			else if(!ans[i].ans)	t = i;	//	其实都可，这里取真 
			fprintf(fp,"%d ",t);
		}
		fprintf(fp,"\n");			
	}
	
	fprintf(fp,"%c %.3lf\n", 't', used_time * 1000);
	
	fclose(fp);
	return TRUE;
}


/*规约为数独 部分*/

//	1.生成合法终盘（拉斯维加斯算法） 
//	2.挖洞
//	3.用DPLL查看解的个数
//	4.反证法剪枝：若移除后存在多个解，则恢复该数字；否则保留空洞
//	5.达到目标空洞数时输出数独初盘 
bool fundConsCNF(CNF &S, LITSHEET *ans)
//1.	将格、行列、宫限制转化为CNF (total 11988)
//除去格约束（可省）一共8991个子句 
{
 	if(S.num_clau) return false;
 	S.num_var = 729;
 	// 初始化ans 
 	for(int i = 0;i <= SUDOKU_VAR; i++){
 		ans[i].ans = 0;
 		ans[i].pos = NULL;
 		ans[i].nag = NULL;
 	}		
 	int temp[9] = {1,2,3,4,5,6,7,8,9}, t = 1;
	
 //// 格约束 (2997个子句)
 //	for(int j = 0;j < 81; j ++){	//81个格子每个格子37个子句 
 //	// -1 -2 0同一格不可以填两个数	
 //		for(int i = 0; i < 9; i++){
 //			for(int k = i + 1;k < 9; k++){
 //				createClause(S);
 //				LITERAL *a = (LITERAL *)malloc(sizeof(LITERAL)), *b = (LITERAL *)malloc(sizeof(LITERAL));
 //				a->next = NULL;	b->next = NULL;
 //				a->value = -temp[i];b->value = -temp[k];
 //				
 //				S.tail->lit = a;
 //				a->next = b;
 //				S.tail->num = 2;
 //				
 //				buildLitsheet(ans, S.tail, -temp[i]);
 //				buildLitsheet(ans, S.tail, -temp[k]);
 //			}
 //		}
 //		
 //	// 1 2 3 4 5 6 7 8 9 每格填一个1-9的数 
 //		createClause(S);		
 //		for(int i = 0;i < 9;i ++){
 //				
 //	        LITERAL *newLiteral = (LITERAL *)malloc(sizeof(LITERAL));
 //	        newLiteral->next = NULL;        
 //	        newLiteral->value = temp[i]; 
 //	        
 //	        buildLitsheet(ans, S.tail, temp[i]);
 //	        
 //	        temp[i] += 9;				//	为下一轮循环做准备 
 //				
 //			if(S.tail->lit == NULL) {
 //	            S.tail->lit = newLiteral;
 //	        } else {
 //	            LITERAL *lastLit = S.tail->lit;
 //	            newLiteral->next = lastLit;
 //	            S.tail->lit = newLiteral;
 //	        }       
 //	        			
 //		}
 //		S.tail->num = 9;
 //	}	
 //	S.num_clau += 2997;	
 // 行约束	2997
 	for(int i = 0;i < 9;i ++){
 		temp[i] = t;
 		t += 9;
 	}	
 	for(int row = 0;row < 9;row ++){		//9行
 		for(int j = 0;j < 9;j++){
 		//row行不能重复j -1 -10 
 			for(int i = 0; i < 9; i++){
 				for(int k = i + 1;k < 9; k++){
 					createClause(S);
 					S.num_clau ++;
 					LITERAL *a = (LITERAL *)malloc(sizeof(LITERAL)), *b = (LITERAL *)malloc(sizeof(LITERAL));
 					a->next = NULL;	b->next = NULL;
 					a->value = -temp[i];b->value = -temp[k];					
 					S.tail->lit = a;
 					a->next = b;
 					S.tail->num = 2;					
 					buildLitsheet(ans, S.tail, -temp[i]);
 					buildLitsheet(ans, S.tail, -temp[k]);					
 				}
 			}			
 		//row行都有j 1 10 19... 73
 			createClause(S);
			S.num_clau ++;		
 			for(int i = 0;i < 9;i ++){					
 		        LITERAL *newLiteral = (LITERAL *)malloc(sizeof(LITERAL));
 		        newLiteral->next = NULL;        
 		        newLiteral->value = temp[i]; 		        
 		        buildLitsheet(ans, S.tail, temp[i]);
 		        temp[i] += 1;				//	为 j 下一轮循环做准备
					
 				if(S.tail->lit == NULL) {
 		            S.tail->lit = newLiteral;
 		        } else {
 		            LITERAL *lastLit = S.tail->lit;
 		            newLiteral->next = lastLit;
 		            S.tail->lit = newLiteral; 
 		        }
					
 			}
 			S.tail->num = 9;	

 		}
		
 		// 初始化辅助数组(下一行的 
 		temp[0] = temp[8];		//这里不用+1因为上面已经加过了 
 		for(int i =1;i < 9;i ++)	temp[i] = temp[i-1] + 9;		
 	}

	
 // 列约束 2997
 	t = 1;
 	for(int i = 0;i < 9;i ++){
 		temp[i] = t;
 		t += 81;
 	}
	
 	for(int col = 0;col < 9;col ++){		//9列 
	
 		for(int j = 0;j < 9;j++){
 		//col列不能重复j -1 -82 
 			for(int i = 0; i < 9; i++){
 				for(int k = i + 1;k < 9; k++){
 					createClause(S);
 					S.num_clau ++;
 					LITERAL *a = (LITERAL *)malloc(sizeof(LITERAL)), *b = (LITERAL *)malloc(sizeof(LITERAL));
 					a->next = NULL;	b->next = NULL;
 					a->value = -temp[i];b->value = -temp[k];
					
 					S.tail->lit = a;
 					a->next = b;
 					S.tail->num = 2;
					
 					buildLitsheet(ans, S.tail, -temp[i]);
 					buildLitsheet(ans, S.tail, -temp[k]);					
 				}
 			}			
 		//col列都有j 1 82 163... 649
 			createClause(S);
			S.num_clau ++;		
 			for(int i = 0;i < 9;i ++){
					
 		        LITERAL *newLiteral = (LITERAL *)malloc(sizeof(LITERAL));
 		        newLiteral->next = NULL;        
 		        newLiteral->value = temp[i];
		        
 				buildLitsheet(ans, S.tail, temp[i]); 
				
 		        temp[i] += 1;				//	为 j 下一轮循环做准备
					
 				if(S.tail->lit == NULL) {
 		            S.tail->lit = newLiteral;
 		        } else {
 		            LITERAL *lastLit = S.tail->lit;
 		            newLiteral->next = lastLit;
 		            S.tail->lit = newLiteral; 
 		        }
					
 			}
 			S.tail->num = 9;	

 		}		
 	}

	
 // 宫约束	2997
 	t = 1;	temp[0] = t;
 	for(int i = 1;i < 9;i ++){
 		temp[i] = temp[i - 1] + ((i % 3 == 0) ? 63 : 9);
 	}
	
 	for(int row = 0; row < 3; row ++){		//宫按行分，有三行 
	
 		for(int col = 0; col < 3; col++){	//每行有三个 
		
 			for(int j = 0;j < 9;j++){		
 				//	每个宫内不重复j -1 -10 
 				for(int i = 0; i < 9; i++){
 					for(int k = i + 1;k < 9; k++){
 						createClause(S);
 						S.num_clau ++;
 						LITERAL *a = (LITERAL *)malloc(sizeof(LITERAL)), *b = (LITERAL *)malloc(sizeof(LITERAL));
 						a->next = NULL;	b->next = NULL;
 						a->value = -temp[i];b->value = -temp[k];
						
 						S.tail->lit = a;
 						a->next = b;
 						S.tail->num = 2;
						
 						buildLitsheet(ans, S.tail, -temp[i]);
 						buildLitsheet(ans, S.tail, -temp[k]);						
 					}
 				}
 				//	每个宫内都有j	
 				createClause(S);
				S.num_clau ++;		
 				for(int i = 0;i < 9;i ++){
						
 			        LITERAL *newLiteral = (LITERAL *)malloc(sizeof(LITERAL));
 			        newLiteral->next = NULL;        
 			        newLiteral->value = temp[i]; 
			        
 			        buildLitsheet(ans, S.tail, temp[i]);
			        
 			        temp[i] += (j == 8) ? 19 : 1;				//	为 j 下一轮以及下一个col循环做准备
						
 					if(S.tail->lit == NULL) {
 			            S.tail->lit = newLiteral;
 			        } else {
 			            LITERAL *lastLit = S.tail->lit;
 			            newLiteral->next = lastLit;
 			            S.tail->lit = newLiteral; 
 			        }
						
 				}
 				S.tail->num = 9;									
 			} 		
 		} 
		
 		//	为下一行初始化辅助数组 
 		temp[0] = temp[8] - 18;//-19+1
 		for(int i = 1;i < 9;i ++){
 			temp[i] = temp[i - 1] + ((i % 3 == 0) ? 63 : 9);
 		}
 	}	


 	return true; 
 }	

bool percentConsCNF(CNF &S, LITSHEET *ans)
 //2.	将反对角线、窗口限制转化为CNF(total 999)
 //和前面加起来9990（没有格约束） 
{
 	if(!S.num_clau || !S.head) return false;
	
 	int temp[9];	temp[0] = 73;
 	for(int i = 1;i < 9;i++)	temp[i] = temp[i - 1] + 72; 
 //反对角线约束	333
 	for(int j = 0;j < 9;j ++){
 		//不能重复j 
 		for(int i = 0; i < 9; i++){
 			for(int k = i + 1;k < 9; k++){
 				createClause(S);
 				S.num_clau ++;
 				LITERAL *a = (LITERAL *)malloc(sizeof(LITERAL)), *b = (LITERAL *)malloc(sizeof(LITERAL));
 				a->next = NULL;	b->next = NULL;
 				a->value = -temp[i];b->value = -temp[k];
				
 				S.tail->lit = a;
 				a->next = b;
 				S.tail->num = 2;
				
 				buildLitsheet(ans, S.tail, -temp[i]);
 				buildLitsheet(ans, S.tail, -temp[k]);				
 			}
 		}
 		//反对角线有j	
 		createClause(S);
		S.num_clau ++;		
 		for(int i = 0;i < 9;i ++){
				
 	        LITERAL *newLiteral = (LITERAL *)malloc(sizeof(LITERAL));
 	        newLiteral->next = NULL;        
 	        newLiteral->value = temp[i]; 
	        
 	        buildLitsheet(ans, S.tail, temp[i]);
	        
 	        temp[i] += 1;				//	为 j 下一轮循环做准备
				
 			if(S.tail->lit == NULL) {
 	            S.tail->lit = newLiteral;
 	        } else {
 	            LITERAL *lastLit = S.tail->lit;
 	            newLiteral->next = lastLit;
 	            S.tail->lit = newLiteral; 
 	        }
				
 		}
 		S.tail->num = 9;	
 	} 

 //窗口限制		333			 

 	temp[0] = 91;
 	for(int i = 1;i < 9;i ++){
 		temp[i] = temp[i - 1] + ((i % 3 == 0) ? 63 : 9);
 	}	

 	for(int window = 0; window < 2; window ++){
 		for(int j = 0;j < 9;j ++){
 			//不能重复j 
 			for(int i = 0; i < 9; i++){
 				for(int k = i + 1;k < 9; k++){
 					createClause(S);
 					S.num_clau ++;
 					LITERAL *a = (LITERAL *)malloc(sizeof(LITERAL)), *b = (LITERAL *)malloc(sizeof(LITERAL));
 					a->next = NULL;	b->next = NULL;
 					a->value = -temp[i];b->value = -temp[k];
					
 					S.tail->lit = a;
 					a->next = b;
 					S.tail->num = 2;
					
 					buildLitsheet(ans, S.tail, -temp[i]);
 					buildLitsheet(ans, S.tail, -temp[k]);					
 				}
 			}
 			//反对角线有j	
 			createClause(S);
			S.num_clau ++;		
 			for(int i = 0;i < 9;i ++){
					
 		        LITERAL *newLiteral = (LITERAL *)malloc(sizeof(LITERAL));
 		        newLiteral->next = NULL;        
 		        newLiteral->value = temp[i];
		        
 				buildLitsheet(ans, S.tail, temp[i]); 
				
 		        temp[i] += (j == 8) ? 352 : 1;				//	为 j 下一轮循环做准备
					
 				if(S.tail->lit == NULL) {
 		            S.tail->lit = newLiteral;
 		        } else {
 		            LITERAL *lastLit = S.tail->lit;
 		            newLiteral->next = lastLit;
 		            S.tail->lit = newLiteral; 
 		        }
					
 			}
 			S.tail->num = 9;	
 		} 
 	} 

 	return true;// 共12987个子句 
 }

int ijk_cnf(int ijk)
 //3.	棋盘信息转化为cnf变量 
{
 	int cnf = 0;	bool minus = false;
 	if(ijk < 0)	minus = true;
 	ijk = abs(ijk); 
	
 	cnf += (ijk/100 - 1)  *81 ;
 	ijk %= 100;
 	cnf += (ijk/10 - 1)	*9;
 	ijk %= 10;
 	cnf += ijk;
	
 	if(minus)	cnf *= -1;
 	return cnf;
}

int cnf_ijk(int cnf)
 //4.	逆变换 
{
 	int ijk = 0;	bool minus = false;
 	if(cnf < 0)	minus = true;
 	cnf = abs(cnf); 
     int devide[2] = {81, 9};
    
     for(int i = 0; i < 2; ++i){
         if(cnf % devide[i] != 0){
             ijk += (cnf / devide[i] + 1) * (i == 0 ? 100 :  10 );
             cnf %= devide[i];
         } else {
             ijk += (cnf / devide[i]) * (i == 0 ? 100 : 10);
             cnf = devide[i];
         }
     }

 	ijk += cnf;
	
 	if(minus)	ijk *= -1;
	
 	return ijk;
 }

int randomNum(int *value)
 //5.	根据指令生成位置的随机值
{
 	int range, ret;
 	//	位置 0-80
 	ret = rand() % 81;
 	//	值 1-9
 	*value = rand() % 9 + 1;
	
 	return ret;	
 } 

void addUnitClause(CNF &S, int value,LITSHEET * ans)
 //6.	头部增加单子句作为填数 
{
 	LITERAL *lit = (LITERAL *)malloc(sizeof(LITERAL));
 	lit->next = NULL;
 	lit->value =value;
	
     CLAUSE *newClause = (CLAUSE *)malloc(sizeof(CLAUSE));
     newClause->lit = lit;
     newClause->num = 1;
     newClause->isTrue = false;
     newClause->next = S.head;
    
     S.head = newClause;
 	 S.num_clau ++;
	
 	int index = abs(value);
 	INFO_L *info_lit = (INFO_L *)malloc (sizeof(INFO_L));
 	info_lit->clause = S.head;
 	info_lit->next = NULL;
	
 	if(value > 0){
 		if(!ans[index].pos){
 			ans[index].pos = info_lit;
 		}
 		else{
 			info_lit->next = ans[index].pos;
 			ans[index].pos = info_lit;
 		}
 	}else{
 		if(!ans[index].nag){
 			ans[index].nag = info_lit;
 		}
 		else{
 			info_lit->next = ans[index].nag;
 			ans[index].nag = info_lit;
 		}		
 	}
	
 }

bool deleteS_head(CNF &S,LITSHEET * ans)
 //7.	删掉开头子句便于回溯 
{
 	if(! S.head){
 		printf("CNF empty !Can't delete head!\n");
 		return false;
 	}	
 	if(S.head->num != 1){
 		printf("head has more than one lit! ERROR\n");
 		return false; 
 	}

	LITERAL *llit = S.head->lit;
	int value = llit->value;
	free(llit);
 	CLAUSE *base = S.head->next;
 	free(S.head);
	
 	S.head = base;
 	S.num_clau --;
 	//	由于是首插法，所以删第一个就可以了 
	
 	if(value > 0){
 		INFO_L *lit = ans[value].pos;
		
 		if(!lit){
 			printf("ERROR ! find no lit! can't update ans\n");
 			return false;
 		}
 		else{	
 			ans[value].pos = lit->next;
 			free(lit);
 		}
		
 	}else{
 		INFO_L *lit = ans[-value].nag;
		
 		if(!lit){
 			printf("ERROR ! find no lit! can't update ans\n");
 			return false;
 		}
 		else{
 			ans[-value].nag = lit->next;
 			free(lit);
 		}		
 	}	

 	return true;
 }
 
bool DFS_board_1(int row, int col, int cnt, CNF &S, /*int board[N][N],*/LITSHEET * ans)
 //8.1通过给出的数进行DFS生成终盘 
{
 	if(cnt == 81)	return true;	//	已填满
	 
 	int value, i, j, start;
 	bool flag = false;
 	bool prun = false;
	// 找到第一个空位处	
 	for(i = row; i < N; i ++){
 		if(i == row)	start = col;
 		else			start = 0;
 		for(j = start; j < N;j ++){
 			if(board[i][j] == 0){
 				flag = true;
 				break;
			 }	
 		}
 		if(flag)	break;
 	}			 
 	flag = false;
 	
 	//构建 1-9随机数组 
	int ran_arr[9]={0}, ran;
	for(int t = 1;t < 10;t ++){
		ran = rand() % 9;
		while(ran_arr[ran] ) ran = (ran + 1)% 9;
			ran_arr[ran] = t;
	}	
//	for(int t = 0;t < 9;t++)	printf("%d ",ran_arr[t]);		

 	for(int t = 0;t < 9;t ++){
 		int k = ran_arr[t];
 		
 		for(int r = 0;r < N;r++){ 
 			if(board[i][r] == k){
 				prun = true;
				break; 
			 }
		 }	
		if(prun){
			prun = false;
			continue;
		}
		for(int c = 0;c < N;c++){ 
 			if(board[c][j] == k){
 				prun = true;
				break; 
			 }
		 }
		if(prun){
			prun = false;
			continue;
		}
	    for (int r = 0; r < BOX_SIZE; r++) {
	        for (int c = 0; c < BOX_SIZE; c++) {
	        	int box_row = i - i % BOX_SIZE;
	    		int box_col = j - j % BOX_SIZE;
	            if (board[box_row + r][box_col + c] == k) {
	                prun = true;
	                break;
	            }
	        }
	        if(prun)	break;
	    }
		if(prun){
			prun = false;
			continue;
		}		 	
			
 		board[i][j] = k;
 		value = ijk_cnf((i+1)*100 + (j+1)*10 + k);	//棋盘位置加填的数转化为cnf变元的值 
 		
		addUnitClause(S, value, ans);

		flag =  DFS_board_1(i + (j + 1)/9, (j + 1)%9, cnt + 1, S/*,board*/,ans);
		
		if(flag)	return true;

//	DFS失败
 		deleteS_head(S, ans);
 	}

 	board[i][j] = 0; 
// 		showBoard(board);

 	return false;	
 }

bool DFS_board_2(int row, int col, int cnt, CNF &S,/* int board[N][N],*/LITSHEET * ans)
 //8.1通过给出的数进行DFS生成终盘 
{
 	if(cnt == 81)	return true;	//	已填满
	 
 	int value, i, j, start;
 	bool flag = false;
 	bool prun = false;
	// 找到第一个空位处	
	for(i = row; i < N; i ++){
 		if(i == row)	start = col;
 		else			start = 0;
 		for(j = start; j < N;j ++){
 			if(board[i][j] == 0){
 				flag = true;
 				break;
			 }	
 		}
 		if(flag)	break;
 	}			 
 	flag = false;
 	
 	//构建 1-9随机数组 
	int ran_arr[9]={0}, ran;
	for(int t = 1;t < 10;t ++){
		ran = rand() % 9;
		while(ran_arr[ran] ) ran = (ran + 1)% 9;
			ran_arr[ran] = t;
	}			

 	for(int t = 0;t < 9;t ++){
 		int k = ran_arr[t];
 		
 		for(int r = 0;r < N;r++){ 
 			if(board[i][r] == k){
 				prun = true;
				break; 
			 }
		 }	
		if(prun){
			prun = false;
			continue;
		}
		
		for(int c = 0;c < N;c++){ 
 			if(board[c][j] == k){
 				prun = true;
				break; 
			 }
		 }
		if(prun){
			prun = false;
			continue;
		}
		
	    for (int r = 0; r < BOX_SIZE; r++) {
	        for (int c = 0; c < BOX_SIZE; c++) {
	        	int box_row = i - i % BOX_SIZE;
	    		int box_col = j - j % BOX_SIZE;
	            if (board[box_row + r][box_col + c] == k) {
	                prun = true;
	                break;
	            }
	        }
	        if(prun)	break;
	    }
		if(prun){
			prun = false;
			continue;
		}		
		 	
		if(i + j == N - 1){
			for(int dia = 0; dia < N; dia ++){
				if(board[dia][N-1-dia] == k){
					prun = true;
					break;
				}
			}
			if(prun){
				prun = false;
				continue;
			}			
		}
									
 		board[i][j] = k;
 		value = ijk_cnf((i+1)*100 + (j+1)*10 + k);	//棋盘位置加填的数转化为cnf变元的值 
 		
		addUnitClause(S, value, ans);
		
		flag =  DFS_board_2(i + (j + 1)/9, (j + 1)%9, cnt + 1, S/*,board*/,ans);
		
		if(flag)	return true;

//	DFS失败
 		deleteS_head(S, ans);
 	}
 	board[i][j] = 0; 

 	return false;	
 }

bool generate_1(CNF &S, /*int board[N][N],*/ LITSHEET* ans)
//9.1	通过拉斯维加斯算法生成终盘
{
	int ran_arr1[9]={0},ran_arr2[9]={0},ran_arr3[9]={0}, ran, value, t;
	
	for(int t = 1;t < 10;t ++){
		ran = rand() % 9;
		while(ran_arr1[ran] ) ran = (ran + 1)% 9;
		
		ran_arr1[ran] = t;
	}
	for(int t = 1;t < 10;t ++){
		ran = rand() % 9;
		while(ran_arr2[ran] ) ran = (ran + 1)% 9;
		
		ran_arr2[ran] = t;
	}
	for(int t = 1;t < 10;t ++){
		ran = rand() % 9;
		while(ran_arr3[ran] ) ran = (ran + 1)% 9;
		
		ran_arr3[ran] = t;
	}	
	ran = 0;
	for(int r = 0;r < 3;r++)
		for(int c = 0;c < 3;c++){
			value = ran_arr1[ran];
	 		t = ijk_cnf((r + 1)* 100 + (c + 1)* 10 + value);
	 		addUnitClause(S, t, ans);
	 		
			board[r][c] = value;
			ran ++;				
		}
		
	ran = 0;	
	for(int r = 3;r < 6;r++)
		for(int c = 3;c < 6;c++){
			value = ran_arr2[ran];
	 		t = ijk_cnf((r + 1)* 100 + (c + 1)* 10 + value);
	 		addUnitClause(S, t, ans);
	 		
			board[r][c] = value;
			ran ++;				
		}	
	ran = 0;	
	for(int r = 6;r < 9;r++)
		for(int c = 6;c < 9;c++){
			value = ran_arr3[ran];
	 		t = ijk_cnf((r + 1)* 100 + (c + 1)* 10 + value);
	 		addUnitClause(S, t, ans);
	 		
			board[r][c] = value;
			ran ++;				
		}	
 // Las Vagas随机选13个位置并随机选数 
// 	int i = 0, j = 0, t, cnt = INIT_NUM_1, value = 0, *p_value = &value;
// 	bool btemp = false;
// 	while(cnt){
// 		t = randomNum(p_value);
// 		i = t / 9;
// 		j = t % 9;
// 		if(board[i][j] != 0)	continue;	//该位置已经填入过，可省去格约束 
//	
// 		t = ijk_cnf((i + 1)* 100 + (j + 1)* 10 + value);
//
// 		addUnitClause(S, t, ans);
// 		if(cnt == INIT_NUM_1){
// 			board[i][j] = value;
// 			cnt --;
// 			continue;
//		 }
//
// 		btemp = DPLL(S, ans);	
//		
// 		if(btemp){
// 			board[i][j] = value;
// 			cnt --;
// 			for(int ii = 0;ii <= SUDOKU_VAR; ii++)	ans[ii].ans = 0;		//清零ans 
// 		}else{
// 			//	如果回溯不了就报错 
// 			if(!deleteS_head(S, ans)){
// 				printf("ERROR in delete_head, can't backtrace\n");
// 				break;
// 			}	
// 		}	
// 	}

// 	if(cnt != 0){
// 		printf("Las Vagas Alo FAILED\n");
// 		return false;
// 	}		
//	printf("Las Vagas Alo SUCCESS with %d items!\n",INIT_NUM_1);
//	showBoard(board);
 //	生成完整终盘 

 	if(DFS_board_1( 0, 0, INIT_NUM_1, S,/* board,*/ ans)){
 		Output_CNF(S);
	 	showBoard (board); 		
 		if(DPLL_1(S, ans)){
//	  		printf("生成终盘成功，Congratulations!\n");
	  		for(int ii = 0;ii <= SUDOKU_VAR; ii++)	ans[ii].ans = 0;
			
 		}else{
 			printf("生成终盘但终盘错误！\n");
 			recoverCNF(S,ans);
 			recoverBoard(board);
 			return false;
		 }
 	}else{
 		printf("生成终盘失败\n");
 		recoverCNF(S,ans);
 		recoverBoard(board);
 		return false;
 	}
	
 	return true; 
} 

bool generate_2(CNF &S, /*int board[N][N],*/ LITSHEET* ans)
//9.2	通过拉斯维加斯算法生成终盘(先完成窗口，其他的正常进行)
{
 	int ran_arr1[9]={0},ran_arr2[9]={0}, ran, value, t;
	for(int t = 1;t < 10;t ++){
		ran = rand() % 9;
		while(ran_arr1[ran] ) ran = (ran + 1)% 9;
		
		ran_arr1[ran] = t;
	}
	for(int t = 1;t < 10;t ++){
		ran = rand() % 9;
		while(ran_arr2[ran] ) ran = (ran + 1)% 9;
		
		ran_arr2[ran] = t;
	}
	if(ran_arr1[8] == ran_arr2[0]){
		int t = ran_arr2[0];
		ran_arr2[0] = ran_arr2[1];
		ran_arr2[1] = t;
	}
 	//第一个窗口 11到33 
 	ran = 0;	
	for(int r = 1;r <= BOX_SIZE; r++){
		for(int c = 1;c <= BOX_SIZE; c++){
			
			value = ran_arr1[ran];
	 		t = ijk_cnf((r + 1)* 100 + (c + 1)* 10 + value);
	 		addUnitClause(S, t, ans);
	 		
			board[r][c] = value;
			ran ++;
		}
	}
	//第二个窗口55-77 
	ran = 0; 
	for(int r = 5;r < 8; r++){
		for(int c = 5;c < 8; c++){
			value = ran_arr2[ran];
	 		t = ijk_cnf((r + 1)* 100 + (c + 1)* 10 + value);
	 		addUnitClause(S, t, ans);
			 			
			board[r][c] = value;
			ran ++;
		}				
	}
//	printf("Las Vagas Alo SUCCESS with %d items!\n",INIT_NUM_2);
//	showBoard(board); 
//	Output_CNF(S); 
 
 	if(DFS_board_2( 0, 0, INIT_NUM_2, S,/* board,*/ ans)){
 		if(DPLL_1(S, ans)){
	  		for(int ii = 0;ii <= SUDOKU_VAR; ii++)	ans[ii].ans = 0;
			  return true;		
 		}else{
 			printf("生成终盘但终盘错误！\n");
 			recoverCNF(S,ans);
 			recoverBoard(board);
 			return false;
		 }
 	}else{
 		printf("生成终盘失败\n");
 		recoverCNF(S,ans);
 		recoverBoard(board);
 		return false;
 	}
} 

void recoverCNF(CNF &S, LITSHEET *ans)
//10
{
	while(S.head && S.head->num == 1){
		deleteS_head(S,ans);
	}
//	Output_CNF(S);
}
void recoverBoard(int board[N][N])
//11
{
	for(int i = 0;i < N; i++)
		for(int j = 0;j < N; j++)
			board[i][j] = 0;
}
void copyBoard(int record[N][N], int board[N][N])
//12.
{
	for(int i = 0;i < N; i++)
		for(int j = 0;j < N; j++)
			record[i][j] = board[i][j];
}

int dig_holes(/*int board[N][N],*/ int blank/*, int mode*/)
//13. 挖洞法生成数独游戏 
{
	size_t s,e;	double d = 0.0;
 	int row = 0, col = 0, t, cnt = blank, value;
 	int visit[N][N] = {0}; 
 	
	bool (*pfunc)(/*int board[N][N], */int, int, int) = NULL;
	if(mode == 1)			pfunc = fast_check_1;
	else if(mode == 2)		pfunc = fast_check_2;
	else return false;
	
 	bool btemp = false,flag = false;
	s = clock();	
 	while(cnt && !isStuck(visit) && d < 5.0){
 		btemp = false;
		flag = false;
		e = clock();
		d = ((double)(e - s))/CLOCKS_PER_SEC;			
		t = rand() % 81;
		row = t / 9;
		col = t % 9;
		//已经挖了/已经访问过了（剪枝）
		if(visit[row][col] == 1 || board[row][col] == 0)	continue; 
		
		value = board[row][col];
		board[row][col] = 0;
		visit[row][col] = 1; 
		
		if(cnt == blank){
			cnt --;
			continue;
		}
		
		for(int i = 1;i < 10; i ++){
			if(i != value){						//替换成别的数 
				btemp = pfunc(/*board,*/row,col,i);
				if(btemp){						//挖掉这个不存在唯一解,复原 
					board[row][col] = value;
					flag = true;
					break;
				}
			}
		}
		
		if(!flag)		cnt --;	 		
	}
	
	if(cnt >= blank/5)	return -1;			
	return blank - cnt;
}

bool fast_check_1(/*int board[N][N],*/ int x, int y, int n)
//14.1
{
	for (int j = 0; j < 9; j++)	//检测行
		if (board[x][j] == n)
			return 0;
	for (int i = 0; i < 9; i++)	//检测列
		if (board[i][y] == n)
			return 0;
	for (int i = x / 3 * 3; i < x / 3 * 3 + 3; i++)	//单元格矩阵
		for (int j = y / 3 * 3; j < y / 3 * 3 + 3; j++)
			if (board[i][j] == n)
				return 0;
	return 1;
}

bool fast_check_2(/*int board[N][N],*/ int x, int y, int n)
//14.2
{
	for (int j = 0; j < 9; j++)	//检测行
		if (board[x][j] == n)
			return 0;
	for (int i = 0; i < 9; i++)	//检测列
		if (board[i][y] == n)
			return 0;
	for (int i = x / 3 * 3; i < x / 3 * 3 + 3; i++)	//单元格矩阵
		for (int j = y / 3 * 3; j < y / 3 * 3 + 3; j++)
			if (board[i][j] == n)
				return 0;
	for(int dia = 0; dia < N; dia++)//反对角线 
		if(board[dia][N-1-dia] == n)
			return 0;
	for(int i = 1;i < 4; i++)		//两个窗口 
		for(int j = 1;j < 4;j ++)
			if (board[i][j] == n)
				return 0;
	for(int i = 5;i < 8; i++)
		for(int j = 5;j < 8;j ++)
			if (board[i][j] == n)
				return 0;	
	return 1;
}

void showBoard(int board[N][N])
 //15.
{
     printf("    1   2   3   4   5   6   7   8   9\n");
     printf("  ╔═══╤═══╤═══╦═══╤═══╤═══╦═══╤═══╤═══╗\n");//开头2个空格
    
     for(int i = 0; i < N; i++) {

         printf("%d ║",i+1);
        
         for(int j = 0; j < N; j++) {
             // 0显示为空格
             if(board[i][j] == 0) {
                 printf("   ");
             } else {
                 printf(" %d ", board[i][j]);
             }

             if(j == 8) {
                 printf("║");
             } else if(j % 3 == 2) {
                 printf("║");
             } else {
                 printf("│");
             }
         }
         printf("\n");

         if(i == 8) {
             printf("  ╚═══╧═══╧═══╩═══╧═══╧═══╩═══╧═══╧═══╝\n");
         } else if(i % 3 == 2) {
             printf("  ╠═══╪═══╪═══╬═══╪═══╪═══╬═══╪═══╪═══╣\n");
         } else {
             printf("  ╟───┼───┼───╫───┼───┼───╫───┼───┼───╢\n");
         }
     }
}

void Output_CNF(CNF &S)
 //16.将cnf输出到CNF_output文件中 
{
 	FILE *fp = fopen("CNF_output.cnf","w");
 	if(!fp){
 		printf("ERROR cannot output to 'CNF_output'");
 	}
 //	showCNF(S,fp);
 	time_t now = time(NULL);
     fprintf(fp,"c Created Time: %sp cnf %d %d\n", ctime(&now), S.num_var, S.num_clau);
     CLAUSE *curr = S.head;
    
     while(curr){
         LITERAL *lit = curr->lit;
         while(lit) {
             fprintf(fp,"%d ", lit->value);
             lit = lit->next;
         }
         fprintf(fp,"0\n");

         curr = curr->next;
     }
    
 	fclose(fp);
 }
bool isStuck(int visit[N][N])
//17.
{
	for(int i = 0;i < N;i++)
		for(int j = 0;j < N;j++)
			if(user[i][j] == 0)	return false;
			
	return true;	
}

/*与用户交互 部分*/
void play()
//1.处理输入 
{
	game_flag = TRUE;
	int input, i, j, k;
	copyBoard(user, board);
	clock_t time_start,time_end;
	time_start = clock();
	printf("您的数独已生成，请输入形如ijk的数,代表第i行第j列填入k,ij0代表撤回\n");
	printf("如果实在想不出来想要提示请先输入0后输入ij，如果发现做不出来了那就输入-1退出游戏,重选适合自己的难度与类型即可\n"); 
	printf("注意：给出的初始值不能修改!\n");
	while(!gameEnd()){
		
		showBoardstar();
		scanf("%d",&input);
		 
		if(!input){						//提示 
			int helper;	scanf("%d",&helper);
			if(helper <= 0 || helper >99){
				printf("无效输入\n");
			}else{
				i = helper / 10;
				j = helper % 10; 
				k = hint(i - 1,j - 1);
				if(k){
					if(game_flag)	printf("此处该填%d\n",k);
					else{
						Sleep(500);
						printf("即将离开游戏，答案已保存在sudoku_cnf和sudoku_res，请重新输入2指令进行游戏选择...\n");
						break; 
					}
				}
						
			}
		}else if(input == -1){
			game_flag = FALSE;
			break;
		}else if( input < 110 || input > 999){
			printf("无效输入\n");
		}else{
			i = input / 100 ;
			j = input % 100 / 10 ;
			k = input % 100 % 10;
			if(!i || !j)	printf("无效输入！\n");
			
			if(erroInfo(i-1,j-1,k))
				printf("请重新输入\n"); 
			else
				user[i-1][j-1] = k;			
		}

	}
	if(game_flag == TRUE){
		time_end = clock();
		double delta = time_end - time_start;
		printf("您已成功解出数独，耗时%f s",delta);		
	}else{
		printf("byebye 下次再战:)\n");
	}

	
}
bool erroInfo(int i, int j, int k)
//2.检查是否出错，并输出报错信息 
{
	if(board[i][j]){
		printf("不可以修改给定值！\n");
		return true;
	}
	if(k == record[i][j] || !k)	return false;	//	正解或回退 
	int row,col;
	for (col = 0; col < 9; col++)	//检测行
		if (board[i][col] == k){
			printf("同一行只能有一个相同的数，与第%d行第%d列冲突\n",i+1,col+1);
			return true;			
		}	
	for (row = 0; row < 9; row++)
		if(board[row][j] == k){
			printf("同一列只能有一个相同的数，与第%d行第%d列冲突\n",row+1,j+1);
			return true;			
		}
	for ( row = i / 3 * 3; row < i / 3 * 3 + 3; row++)	//单元格矩阵
		for ( col = j / 3 * 3; col < j / 3 * 3 + 3; col++)
			if (board[row][col] == k){
				printf("同一3*3宫只能有一个相同的数，与第%d行第%d列冲突\n",row+1,col+1);
				return true;		
			}
	if(mode == 2){
		if(i + j == 8){
			for(int dia = 0; dia < N; dia++)//反对角线 
				if(board[dia][N-1-dia] == k){
					printf("斜对角线只能有一个相同的数，与第%d行第%d列冲突\n",dia+1,N-dia);
					return true;			
				}		
		}
		if(i > 0 && i < 4 && j > 0 && j < 4 ){
			for( row = 1;row < 4; row++)		//两个窗口 
				for( col = 1;col < 4;col ++)
					if (board[row][col] == k){
						printf("左上窗口只能有一个相同的数，与第%d行第%d列冲突\n",row+1,col+1);
						return true;				
					}	
		}
		if(i > 4 && i < 8 && j > 4 && j < 8 ){
			for(int row = 5;row < 8; row++)
				for(int col = 5;col < 8;col ++)
					if (board[row][col] == k){
						printf("右下窗口只能有一个相同的数，与第%d行第%d列冲突\n",row+1,col+1);
						return true;	
					}	
		}		
	}		

//	printf("乍一看没什么问题，可是走到后面会发现不可以哦\n");
	game_flag = FALSE;
	return false;
}
bool gameEnd(/*int user[N][N]*/)
//3.是否填完 
{
	for(int i = 0;i < N;i++)
		for(int j = 0;j < N;j++)
			if(user[i][j] == 0)	return false;
			
	return true;
}
void showBoardstar()
//4.按特定格式输出棋盘 
{
     printf("    1   2   3   4   5   6   7   8   9\n");
     printf("  ╔═══╤═══╤═══╦═══╤═══╤═══╦═══╤═══╤═══╗\n");//开头2个空格
    
     for(int i = 0; i < N; i++) {

         printf("%d ║",i+1);
        
         for(int j = 0; j < N; j++) {
             // 0显示为空格
             if(user[i][j] == 0) {
                 printf("   ");
             } else {
             		if(mode == 2){
	             		if((i > 0 && i < 4 && j > 0 && j < 4 )||(i > 4 && i < 8 && j > 4 && j < 8 ))	printf("*");
	             		else																			printf(" ");             			
					 }else{
					 	printf(" ");
					 }

             		if(board[i][j])		printf("%d.", user[i][j]);
                 	else				printf("%d ", user[i][j]);
             }

             if(j == 8) {
                 printf("║");
             } else if(j % 3 == 2) {
                 printf("║");
             } else {
                 printf("│");
             }
         }
         printf("\n");

         if(i == 8) {
             printf("  ╚═══╧═══╧═══╩═══╧═══╧═══╩═══╧═══╧═══╝\n");
         } else if(i % 3 == 2) {
             printf("  ╠═══╪═══╪═══╬═══╪═══╪═══╬═══╪═══╪═══╣\n");
         } else {
             printf("  ╟───┼───┼───╫───┼───┼───╫───┼───┼───╢\n");
         }
	}
}
int hint(int i,int j)
//5.当用户选择时，发送正确答案 
{
	if(game_flag == FALSE){
		printf("悄悄告诉你o...\n重开吧孩子AvA\n");
		return 0;
	}
	if(!board[i][j] && !user[i][j]){
		user[i][j] = record[i][j];
		return record[i][j];
	}	
	else{
		printf("错误！已经填有数字\n");
		return 0;
	}	
} 
status saveSudoku(LITSHEET* ans, CNF &S)
//6.保存数独结果文件 
{
	int t;
	FILE *fp = fopen("sudoku_record.res","w");
	if(!fp)	return FALSE;
	
	fprintf(fp, "%s%d\n","s ",ans[0].ans );
	
	if(ans[0].ans){
		
		fprintf(fp,"%s","v ");
		for(int i = 1;i <= S.num_var;i ++){
			/**/
			if(ans[i].ans == 1)	t = i;
			else if(ans[i].ans == 2)	t = -1 * i;
			else if(!ans[i].ans)	t = i;	//	其实都可，这里取真 
			fprintf(fp,"%d ",t);
		}
		fprintf(fp,"\n");			
	}
	fclose(fp);

	FILE *test = fopen("sudoku_cnf.cnf","w");
    fprintf(test,"\nCNF with %d clauses and %d variables:\n", S.num_clau, S.num_var);
    CLAUSE *curr = S.head;
    int cnt = 1;
    while(curr){
       fprintf(test,"Clause %d: ", cnt++);
        LITERAL *lit = curr->lit;
        while(lit) {
            fprintf(test,"%d ", lit->value);
            lit = lit->next;
        }
        fprintf(test,"%s","\n");
        fprintf(test,"bool: %d\nlits: %d\n",curr->isTrue, curr->num);
        curr = curr->next;
    }
    fprintf(test,"%s","\n");
	fclose(test);

	return TRUE;
}


