#include "func.h"

/*构建stack 部分 */

/*DPLL 部分 1-26  共22个关键函数*/

void makecopy(CNF &newS,const CNF &S) 
//1.将S拷贝到newS (比操作栈要费时费空间，不过直观)
{
	newS.num_clau = S.num_clau;
	newS.num_var = S.num_var;
	newS.exist_emptyclause = S.exist_emptyclause;
	
	createClause(newS);
	for(int i = 0;i < newS.num_clau; i++){
		CLAUSE *node = newS.tail;
		node->isTrue = S.tail->isTrue;
		node->num = S.tail->num;
		node->lit = S.tail->lit;
		
		// lit不会删，所以副本地址可以不变，唯一变的就是clause的地址即可 
		if(i != newS.num_clau - 1){
			createClause(newS);
			node->next = newS.tail;	
		}
		
	}
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

void clearCNF(CNF &S, stack <INFO_C> &op_clau,LITSHEET* ans)
//3.清除CNF并释放之前的内存	;   
{
	while(!op_clau.empty()){
		restore_cl(1, op_clau, S);
	}

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

LITSHEET* CNFparser(CNF &S, char file[], LITSHEET* ans)
//7. 解析CNF文件
{
    FILE *fp;
    char c; int t;

    if((fp = fopen(file, "r")) == NULL) {
        printf("Can't open file.\n");
        return NULL;
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
                return NULL;
            }

            fscanf(fp, "%d %d", &S.num_var, &S.num_clau);
            break;
        }
    }
    
// 调整并初始化ans数组
//	if(S.num_var > SUDOKU_VAR) {
//		
////		LITSHEET *temp = (LITSHEET *)realloc(ans,(S.num_var + 1)* sizeof(LITSHEET) );
////		
////		if(temp == NULL){
////			printf("realloc ERROR!");
////			free(ans);
////			return NULL;
////		}
////		ans = temp;
//		for(int i = 0;i <= S.num_var; i++){
//			ans[i].ans = 0;
//			ans[i].pos = NULL;
//			ans[i].nag = NULL;
//		}
//		printf("ans for more is ready\n");
//			
//	} else{
//		for(int i = 0;i <= SUDOKU_VAR; i++){
//			ans[i].ans = 0;
//			ans[i].pos = NULL;
//			ans[i].nag = NULL;
//		}		
//		printf("ans is ready\n");
//	}
// 写入CNF

    if(!buildCNF(S, S.num_clau, fp, ans)) {
        printf("Error: CNF's building fail.\n");
        fclose(fp);
        free(ans);
        return NULL; 
    }

    printf("CNF created with %d clauses and %d variables.\n", S.num_clau, S.num_var);

    fclose(fp);
	return ans;
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

void showCNF(CNF &S,FILE *test)
//9
{
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
//10.展示文字出现在自己的次数 ，不包含0 
{
	int cnt;
	for(int i = 1;i <= range; i++){
		printf("var %d in status : %d\n",i,ans[i].ans);
	}
	printf("\n");
}

void showClause(CLAUSE *head)
//11显示某一子句 
{
	CLAUSE *node = head;
	if(!node)	return;
	
	LITERAL *lit = node->lit;
	printf("the clause is %d now has %d literals \n",node->isTrue,node->num);
	
	while(lit){
		printf("%d ",lit->value);
		lit = lit ->next;

	}
	printf("\n");
}

CLAUSE *existUnitClause(CLAUSE *head)
//12. 在子句链表中查找第一个单子句
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

bool existEmptyClause(CLAUSE *Head)
//13.	判断是否存在空子句 
{
	CLAUSE *head = Head;
	while(head){
		if(!head->num && !head->isTrue) return true;
		head = head->next;
	}	
	return false;
}

CLAUSE *locatePre(CLAUSE *node,CLAUSE *head)
//14.获取目标节点的前驱 
{
	if(node == NULL || head == NULL)	return NULL;
	
	CLAUSE *curr = head;
	if(curr == node)	return NULL;
	while(curr){
		
		if(curr->next == node)
			return curr;
		
		curr = curr->next;
	}
	
	return NULL;
}

int deleteOneClause(CLAUSE *node, stack <INFO_C> &op_clau, CNF &S, LITSHEET* ans) 
//15.将指针放进回收栈，返回单子句中变量的值 
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
		showClause(node);
		printf("\n");
	}
	/*try s*/
	if(node->isTrue == true){
		printf("existUnitClause has bug node is already true\n");
	}
		
	node->isTrue = true;
	/*try p*/	
	
	int value; 
	INFO_C opt;
	CLAUSE *pre = locatePre(node, S.head);
	LITERAL *lit = node->lit;

	if(pre == NULL){						//	first clau
		opt.self = node;
		opt.pre = NULL;
		S.head = S.head->next;				//	恢复时移动S.head指针即可 
		if(S.num_clau == 1) S.tail = S.head;				//	只剩这一个了,此时头尾均为NULL 
	}else{
		opt.self = node;
		opt.pre = pre;		
		
		if(node->next == NULL)	S.tail = pre;
		pre->next = pre->next->next;		//	移动指针以“删除”一个子句指针 
	} 

	S.num_clau --;
	op_clau.push(opt);
	
	while(lit) {
		value = lit->value;
		if(ans[abs(value)].ans == 0){
			return value;
		}	
		lit = lit->next;
	}
	printf("ERROR! Cannot find any lit in unitclause:\n");
	showClause(node); 
	printf("\n");

	return 0;
}

bool deleteOneClause_slt(CLAUSE *node, stack <INFO_C> &op_clau, CNF &S) 
//15*.将指针放进回收栈，只返回真假值 
{
	if(!node || !node->lit ||!node->num){
		printf("DON'T exist,can't delete\n");
		return false;
	}			
	if( S.head == NULL){
		printf("CNF already empty\n");
		return false;
	}
	/*try s*/
	if(node->isTrue == true){
		printf("existUnitClause has bug node is already true\n");
	}
		
	node->isTrue = true;
	/*try p*/	

	INFO_C opt;
	CLAUSE *pre = locatePre(node, S.head);

	if(pre == NULL){						//	first clau
		opt.self = node;
		opt.pre = NULL;
		S.head = S.head->next;				//	恢复时移动S.head指针即可 
		if(S.num_clau == 1) S.tail = S.head;				//	只剩这一个了,此时头尾均为NULL 
	}else{
		opt.self = node;
		opt.pre = pre;		
		
		if(node->next == NULL)	S.tail = pre;
		pre->next = pre->next->next;		//	移动指针以“删除”一个子句指针 
	} 

	S.num_clau --;
	op_clau.push(opt);

	return true;
}

int deleteClause(stack <INFO_C> &op_clau, CNF &S, LITSHEET* ans, int backtrace[]/*,FILE *test*/)
//16. 删除所有单子句，并对CNF进行化简；记录操作次数,将单子句中变量值记录 
{
	if(S.num_clau == 0 || !S.head){
		//代表上一步deleteLit后CNF为空，DPLL成功，结束此函数即可
		return 0;
	}		 
	if(/*S.exist_emptyclause*/existEmptyClause(S.head)){
		//代表上一步deleteLit后CNF中出现空子句，DPLL失败，结束此函数即可
		return 0;
	}	
	
	CLAUSE *node = existUnitClause(S.head);
	CLAUSE *next = NULL;
	int cnt = 0, value = 0;
	
	while(node != NULL){
		// 找到并删除单子句
//		 printf("正在处理单子句，首元素=%d\n", node->lit->value);
		if(node->isTrue == true){
			printf("ERROR in 'existUnitClause' unitclause is already true\n");
			node = existUnitClause(node->next);
			continue;
		}	 
		/*try s*/
//		node->isTrue = true; 
		/*try p*/


		//先不更新ans，因为在delOneCl中要靠ans找没删除的文字
		value = deleteOneClause(node, op_clau, S,ans);
		
		/*printf("delete single clause with %d\n",value);
		showCNF(S); //每删一个单子句 */
			
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
		backtrace[++backtrace[0]] = value;
		
		// 化简CNF,并记录删除真子句数		
		cnt += deleteLit(op_clau, S, value, ans);
		
		/*showCNF(S); //每删完一个文字*/
		//  PRUNE
		
		if(S.exist_emptyclause == true){
			/*printf("发现空子句，删除单子句就此中断\n");*/
			return cnt;
		}	

        
//		next = node;
//		while(next && next->isTrue)	next = next->next;	//	找下一个未被删除的子句
		node = existUnitClause(S.head);/*next*/	//	这样可以保证所有单子句被一网打尽（但是稍微慢了点		 
		// node = existUnitClause(node->next)// 记住是Next,不然会死循环;
	}
	
	return cnt;
}

int deleteLit( stack <INFO_C> &op_clau, CNF &S, int value, LITSHEET* ans/*,FILE *test*/)
//17.	删除子句中的文字，同时也会删除真子句并记录真子句次数 
{
	if(!value) {
		printf("ERROR! value = 0, cannot delete\n");
		return 0;
	}
	if(!S.num_clau || !S.head ){//这种情况下CNF已经有解，直接退出 
//		printf("S already empty, cannot delete\n");
		return 0;
	}				//	S已经为空 
	
	int cnt = 0, index = abs(value);		//	直接删除包含value的子句，删除-value的文字 
	INFO_L * pos = ans[index].pos;	INFO_L *nag = ans[index].nag;
	bool flag = false;
	
	if(value > 0){	
		while(pos && pos->clause){

			if(pos->clause->isTrue == false) {
		//		pos->clause->isTrue = true; 				
	
				deleteOneClause_slt(pos->clause, op_clau,S);
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
//				nag->clause->isTrue = true; 

				deleteOneClause_slt(nag->clause, op_clau,S);
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
		printf("FUCK ! value = 0,ERROR\n");
		exit(FALSE);
	}

	if(flag == true){			//	存在空子句 
		S.exist_emptyclause = true;
		/* printf("value = %d empty now! S.exist_emptyclause = %d\n",value,S.exist_emptyclause);*/	
	}	

/*printf("delete lit %d\n",value); 
showCNF(S); //每删一个文字
*/
	return cnt;
}

int choose_lit(CNF&S, LITSHEET* ans)
//18.	选择单子句或者 随机选一个 第一个还是false的子句的第一个有效文字赋值 
//18？ 随机大法 
{
//	CLAUSE *unit = existUnitClause(head);
//	if(unit != NULL){
//		LITERAL *lit = unit->lit;
//		while(lit){
//			int i = lit->value;
//			if(ans[abs(i)].ans == 0)		return i;
//			lit = lit->next;
//		}	
//		printf("ERROR in 'existUnitClause'or'choose_lit'\n");
//		return 0;	
//	}
	//	把单子句一网打尽按理说没有单子句了 
//	int lucky ;
//	lucky = rand() % 12;
//	if(lucky < 6 && lucky > 3){						//1/2 无脑第一个 
//		CLAUSE *node = S.head;	LITERAL *lit;	int i ;
//		while(node){
//			if(node->isTrue == false && node->num > 0) {
//				lit = node->lit;
//				while(lit){
//					i = lit->value;
//					if(ans[abs(i)].ans == 0)		return i;
//					lit = lit->next;
//				}
//				printf("ERROR in 'choose_lit'\n");
//				return 0;
//			}
//			node = node->next;
//		}		
//	}else if(lucky == 2){			//1/6 ans第一个 
//		for(int i = S.num_var;i > 0;i--){
//			if(ans[i].ans == 0)				return i;
//		} 
//	}else if(lucky == 1){			//1/6 ans最后一个 
//		for(int i = 1;i <= S.num_var;i++){
//			if(ans[i].ans == 0)				return i;
//		} 		
//	}else if(lucky == 3){			//ans中间往后 
//		for(int i = S.num_var/2;i <= S.num_var;i++){
//			if(ans[i].ans == 0)				return i;
//		} 		
//	}else if(lucky == 0){			//ans中间往前 
//		for(int i = S.num_var/2 ;i > 0 ;i--){
//			if(ans[i].ans == 0)				return i;
//		} 		
//	}else{							//选最多的未标记的 
	INFO_L *lit = NULL;
	int max = 0,record = 0, cnt = 0;
		for(int i = 1;i <= S.num_var; i++){
			cnt = 0;
			if(ans[i].ans == 0){
				
				if(ans[i].pos){
					lit = ans[i].pos;
					while(lit){
						if(lit->clause && lit->clause->isTrue == false)	cnt ++;
						
						lit = lit->next;
					}
					if(cnt > max){
						max = cnt;
						record = i;
					}					
				}
				cnt = 0;
				if(ans[i].nag){
					lit = ans[i].nag;
					while(lit){
						if(lit->clause && lit->clause->isTrue == false)	cnt ++;
						
						lit = lit->next;
					}
					if(cnt > max){
						max = cnt;
						record = -i;
					}					
				}	
								
			}
		}
		printf("record = %d, max = %d\n",record,max);
		return record;
//	}
//
//	printf("ERROR! cannot choose any lit.strategy is %d\n",lucky);
//	showLitsheet(ans,S.num_var);
//	return 0;
}

bool DPLL( stack <INFO_C> &op_clau, CNF &S, LITSHEET* ans/*, FILE *test*/)
//19.	CORE
{
	int dpll_start = S.num_clau, dpll_end = 0, stack_start = op_clau.size(),stack_end = 0, gap_dpll = 0, gap_stack = 0;
#ifdef CHECK
		printf("本次DPLL前子句数目为%d\n",S.num_clau);
#endif

FILE *fp = fopen("check.txt","w");
	showCNF(S,fp); 
fclose(fp);
	int backtrace[MAX_BACK] = {0};//back【0】为个数
//	for(int i = 0;i < MAX_BACK;i++)	backtrace[i] = 0;
 	int cntCLAU = 0;/*, start = 0*/
 	int chosenlit = 0;/*,trueclau = 0*/
 	
 	if(ans[0].ans != 0) {
 		chosenlit = ans[0].ans;
 		cntCLAU += deleteLit(op_clau, S, chosenlit, ans/*,test*/);	 
 			 
 		backtrace[0] ++;
 		backtrace[1] = chosenlit;
 		
 		if(S.exist_emptyclause/*existEmptyClause(S.head)*/) {			//剪枝（不要放在更新backtrace数组之前！改了2天... 
 			printf("code 1 删掉的%d个文字：",backtrace[0]); 
 			for(int i = backtrace[0];i > 0; i --)	printf("%d ",backtrace[i]); 
#ifdef CHECK 			 
	 		stack_end = op_clau.size();
	 		dpll_end = S.num_clau;
	 		gap_dpll = dpll_start - dpll_end;
			gap_stack = stack_end - stack_start;
			printf("1 gap_stack = %d cntCLAU = %d gap_dpll = %d\n",gap_stack,cntCLAU,gap_dpll);
			if(gap_stack != gap_dpll) 	printf("gap_stack != gap_dpll\n");
	 		if(gap_stack != cntCLAU)	printf("gap_stack != cntCLAU\n");
			if(cntCLAU != gap_dpll) 	printf("cntCLAU != gap_dpll\n");
#endif						
			restore_cl(cntCLAU, op_clau, S);
			restore_lit(backtrace, ans);	
			return false; 			
		 } 		
	}
	
	printf("code 1 删掉%d个单子句\n",cntCLAU);
	fp = fopen("check.txt","w");
	showCNF(S,fp); 
fclose(fp);
	/*start = backtrace[0];*/
	
	// 删除所有单子句，删除包含该文字的子句，删去反文字,并记录操作次数

	cntCLAU += deleteClause(op_clau,S, ans, backtrace/*,test*/);
		printf("code 12 删掉%d个单子句\n",cntCLAU);
fp = fopen("check.txt","w");
	showCNF(S,fp); 
fclose(fp);
	/*将deletelit从deliteclause中移出来*/ 
	// if(backtrace[0] > start){
	// 	//当上一步有单子句可删时
		
	// 	for(int i = 1 + start; i <= backtrace[0]; i++){
	// 		trueclau = backtrace[i];
	// 		cntCLAU += deleteLit(op_clau, S, trueclau, ans);

	// 		if(S.exist_emptyclause) {				
	// 	//需要单独更新被截断的ans片段
	// 			if(i < backtrace[0]){
	// 				 for(int j = i + 1;j <= backtrace[0];j++){
	// 				 	ans[abs(backtrace[j])].ans = 0;
	// 				 } 				
	// 			}		
		
	// 	//	只删到了这里，所以长度缩减到i	 	
	// 			backtrace[0] = i;						
	// 			restore_cl(cntCLAU, op_clau, S);
	// 			restore_lit(backtrace, ans);
	// 			return false; 			
	// 	 	}
		 	
	// 	}
		
	// }

 	// 1.成功解出 
	if(S.num_clau == 0 || S.head == NULL ){
		
		recover_cl(cntCLAU, op_clau, S);
		recover_lit(backtrace, ans);
		return true;		
	}	
	
	// 2.出现矛盾 

	else if(S.exist_emptyclause/*existEmptyClause(S.head)*/) {
 		 			printf("code 2 删掉的%d个文字：",backtrace[0]); 
 			for(int i = backtrace[0];i > 0; i --)	printf("%d ",backtrace[i]); 	 

#ifdef CHECK		
 		stack_end = op_clau.size();
 		dpll_end = S.num_clau;
	 	gap_dpll = dpll_start - dpll_end;
		gap_stack = stack_end - stack_start;
		printf("2 gap_stack = %d cntCLAU = %d gap_dpll = %d\n",gap_stack,cntCLAU,gap_dpll);
		if(gap_stack != gap_dpll) 	printf("gap_stack != gap_dpll\n");
 		if(gap_stack != cntCLAU)	printf("gap_stack != cntCLAU\n");
		if(cntCLAU != gap_dpll) 	printf("cntCLAU != gap_dpll\n");
#endif		
		
		restore_cl(cntCLAU, op_clau, S);
		restore_lit(backtrace, ans);
		return false;		
	}	
	
	// 3.选取一个变量赋值 
	int chosen = choose_lit(S,ans), index = abs(chosen);
	if(!index){
		
#ifdef CHECK 			 
 		stack_end = op_clau.size();
 		dpll_end = S.num_clau;	 		
		gap_dpll = dpll_start - dpll_end;
		gap_stack = stack_end - stack_start;
		
		printf("3 gap_stack = %d cntCLAU = %d gap_dpll = %d\n",gap_stack,cntCLAU,gap_dpll);
		if(gap_stack != gap_dpll) 	printf("gap_stack != gap_dpll\n");
 		if(gap_stack != cntCLAU)	printf("gap_stack != cntCLAU\n");
		if(cntCLAU != gap_dpll) 	printf("cntCLAU != gap_dpll\n");		
#endif		
 		printf("code 3 删掉的%d个文字：",backtrace[0]); 
 		for(int i = backtrace[0];i > 0; i --)	printf("%d ",backtrace[i]); 		
		restore_cl(cntCLAU, op_clau, S);
		restore_lit(backtrace, ans);
		return false;		
	}
	
//	printf("choose %d as a break\n",chosen);

	ans[index].ans = (chosen > 0) ? 1:2;
	ans[0].ans = chosen;
	if(DPLL(op_clau, S, ans/*,test*/)){
		recover_cl(cntCLAU, op_clau, S);
		recover_lit(backtrace, ans);
		return true;
	}
	printf("DPLL with %d FAIL\n",chosen);

	ans[index].ans = (chosen > 0) ? 2:1;
	ans[0].ans = -chosen;
	if(DPLL(op_clau, S, ans/*,test*/)){
		recover_cl(cntCLAU, op_clau, S);
		recover_lit(backtrace, ans);
		return true;
	}
	printf("DPLL with %d FAIL,ALL fail\n",-chosen);
	//	当前分支失败，开始回溯 
	ans[index].ans = 0;
 #ifdef CHECK			 
	stack_end = op_clau.size();
	dpll_end = S.num_clau;
	gap_dpll = dpll_start - dpll_end;
	gap_stack = stack_end - stack_start;
	
	printf("4 gap_stack = %d cntCLAU = %d gap_dpll = %d\n",gap_stack,cntCLAU,gap_dpll);
	if(gap_stack != gap_dpll) 	printf("gap_stack != gap_dpll\n");
	if(gap_stack != cntCLAU)	printf("gap_stack != cntCLAU\n");
	if(cntCLAU != gap_dpll) 	printf("cntCLAU != gap_dpll\n"); 
#endif	
 			printf("code 4 删掉的%d个文字：",backtrace[0]); 
 	for(int i = backtrace[0];i > 0; i --)	printf("%d ",backtrace[i]); 	   
	restore_cl(cntCLAU, op_clau, S);	
	restore_lit(backtrace, ans);

	return false;
}

status restore_cl(int num, stack <INFO_C> &op_clau, CNF &S)
//20.	恢复若干次语句指针 
{
	INFO_C temp;
#ifdef CHECK
	printf("恢复子句中，DPLL化简后CNF有%d个子句,num = %d\n",S.num_clau,num);
#endif
//	showCNF(S);
	for(int i = 0;i < num; i++){
		
		if(op_clau.empty()){
			printf("ERROR, op_cl is empty, cannot restore\n");
			return INFEASIBLE;
		}	
		
		temp = op_clau.top();
		if(!temp.self){
			printf("ERROR, cannot restore NULL\n");
			return FALSE;
		}	
		
		temp.self->isTrue = false;			//	恢复子句布尔值 
		
		if(temp.pre == NULL){
			
			temp.self->next = S.head;
			S.head = temp.self;
			if(S.tail == NULL)	S.tail = S.head;	//	第一个元素 

		}else{
			if(temp.pre == S.tail)	S.tail = temp.self; 
			
			temp.self->next = temp.pre->next;
			temp.pre->next = temp.self;
			
		}
		
		op_clau.pop();

		S.num_clau ++;
		
		if(S.exist_emptyclause == true)	S.exist_emptyclause = false;
	}	
#ifdef CHECK
	printf("恢复完成，现在CNF有%d个子句\n",S.num_clau);
#endif
//	showCNF(S);
	return TRUE;
}

status restore_lit(int back[], LITSHEET* ans)
//21.	恢复若干次文字指针(只恢复 变元bool值，子句的文字数，不管子句bool 
{
//	if(!back || !back[0])	printf("ERROR! back[] is empty!\n");
//showLitans(ans,729);
#ifdef CHECK
	printf("恢复文字中，展示backtrace数组：一共%d个文字被恢复\n",back[0]);
#endif
		for(int i = back[0];i > 0 ;i--){
		int index = abs(back[i]);
			
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
		printf("文字%d\n",back[i]);	
#endif
	}
#ifdef CHECK
	printf("文字恢复结束，showLitsheet\n");
#endif
//	showLitans(ans,729);		
	return TRUE;
}

void recover_cl(int num, stack <INFO_C> &op_clau, CNF &S)
//22.	恢复若干次语句指针 
{
	INFO_C temp;
	
	for(int i = 0;i < num; i++){
		
		if(op_clau.empty())	return;
		
		temp = op_clau.top();
		if(!temp.self)	return;
		
		temp.self->isTrue = false;
		
		if(temp.pre == NULL){
			
			temp.self->next = S.head;
			S.head = temp.self;
			if(S.tail == NULL)	S.tail = S.head;	//	第一个元素 

		}else{
			if(temp.pre == S.tail)	S.tail = temp.self; 
			
			temp.self->next = temp.pre->next;
			temp.pre->next = temp.self;
			
		}
		op_clau.pop();

		S.num_clau ++;
		

		if(S.exist_emptyclause == true)	S.exist_emptyclause = false;
 
	}
}

void recover_lit(int back[], LITSHEET* ans)
//23.	将删去的文字数全部加回来 
{ 
	for(int i = back[0];i > 0 ;i--){
		int index = abs(back[i]);
		
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

bool autocheck(CNF &S,LITSHEET* ans)
{
	CLAUSE *clause = S.head;
	int value;
	bool flag = false;
	
	while(clause && clause->lit){
		
		LITERAL *lit = clause->lit;
		while(lit){
			value = lit->value;
			if((value > 0 && ans[value].ans == 1) || (value < 0 && ans[value].ans == 2) || (value > 0 && ans[value].ans == 0)){
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


/*规约为数独 部分*/

//	1.生成合法终盘（拉斯维加斯算法） 
//	2.挖洞
//	3.用DPLL查看解的个数
//	4.反证法剪枝：若移除后存在多个解，则恢复该数字；否则保留空洞
//	5.达到目标空洞数时输出数独初盘 


bool fundConsCNF(CNF &S, LITSHEET *ans)
//1.	将格、行列、宫限制转化为CNF (total 11988)
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
	
// 行约束	
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
		temp[0] = temp[8] + 1;		
		for(int i =1;i < 9;i ++)	temp[i] = temp[i-1] + 9;		
	}
	S.num_clau += 2997;
	
// 列约束
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
	S.num_clau += 2997;
	
// 宫约束
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
	S.num_clau += 2997;

	return true; 
}

bool percentConsCNF(CNF &S, LITSHEET *ans)
//2.	将反对角线、窗口限制转化为CNF(total 999)
{
	if(!S.num_clau || !S.head) return false;
	
	int temp[9];	temp[0] = 73;
	for(int i = 1;i < 9;i++)	temp[i] = temp[i - 1] + 72; 
//反对角线约束
	for(int j = 0;j < 9;j ++){
		//不能重复j 
		for(int i = 0; i < 9; i++){
			for(int k = i + 1;k < 9; k++){
				createClause(S);
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
	S.num_clau += 333;
//窗口限制				 

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
		S.num_clau += 333;	
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
	int value = S.head->lit->value;  
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

bool DFS_board(/*FILE *test,*/int row, int col, int cnt,  stack <INFO_C> &op_clau, CNF &S, int board[N][N],LITSHEET * ans)
//8.通过给出的数进行DFS生成终盘 
{
	if(cnt == 81)	return true;	//	已填满
	 
	int value, i, j;
	bool flag = false;
	
	for(i = row; i < N; i ++){
		for(j = col; j < N;j ++){
			if(!board[i][j])	break;
		}
	}			// 找到第一个空位处 
			

	for(int k = 1; k < 10;k ++){
		board[i][j] = k;
		value = ijk_cnf((i+1)*100 + (j+1)*10 + k);	//棋盘位置加填的数转化为cnf变元的值 
		addUnitClause(S, value, ans);
		
		/*CNF newS;	initCNF(newS);
		makecopy(newS, S); */
		
		if(DPLL(op_clau, S, ans/*,test*/)){
			
			for(int ii = 0;ii <= SUDOKU_VAR; ii++)	ans[ii].ans = 0;		//	若为true，ans将不会清零
			 
			flag =  DFS_board(/*test,*/i + (j + 1)/9, (j + 1)%9, cnt + 1,op_clau,S,board,ans);
			
			if(flag)	return true;
		}
		//	DPLL或DFS失败

		deleteS_head(S, ans);
	}
		
		board[i][j] = 0; 

	return false;	
}

bool generate(/*FILE *test,*/ stack <INFO_C> &op_clau, CNF &S, int board[N][N], LITSHEET* ans)
//9.	通过拉斯维加斯算法生成终盘
{
	int i = 0, j = 0, t, cnt = 13, value = 0, *p_value = &value;
	bool btemp = false;
	
// Las Vagas随机选13个位置并随机选数 
	while(cnt){
//		t = randomNum(p_value);
//		i = t / 9;
//		j = t % 9;
//		if(board[i][j])	continue;	//该位置已经填入过，可省去格约束 
//		
//		t = ijk_cnf((i + 1)* 100 + (j + 1)* 10 + value);
		if(cnt == 13)	t = 10;
		if(cnt == 12)	t = 88;
		if(cnt == 11)	t = 96;
		addUnitClause(S, t, ans);
		/*// 不好复原，所以直接传入副本
		initCNF(newS, SUDOKU_VAR);
		makecopy(newS, S);*/
		
		btemp = DPLL( op_clau, S, ans/*,test*/);
		check(ans, 729);
//test		
		for(int ii = 1;ii <= 729; ii++){
			int cnt1 = 0;
			INFO_L *pos = ans[ii].pos, *nag = ans[ii].nag;
			while(pos){
				cnt1 ++;
				pos = pos->next;
			}
			if(cnt1)		printf("var %d occur in %d clause\n",ii,cnt);
			
			cnt1 = 0;
			while(nag){
				cnt1 ++;
				nag = nag->next;
			}		
			if(cnt1)		printf("var %d occur in %d clause\n\n",-ii,cnt);
	
		}		
//testend		
		
		
		if(btemp){
			board[i][j] = value;
			cnt --;
			Output_CNF(S);
			showLitsheet(ans,729);
		}else{
			if(!deleteS_head(S, ans)){
//				showCNF(S);
				printf("ERROR\n");
				break;
			}	
		}
	
	/*	printf("check info :cnt = %d\nS.num_clau = %d\n",cnt,S.num_clau);
		showClause(S.tail);printf("\n");*/
	}
	
	if(cnt != 0){
		printf("Las Vagas Alo FAILED\n");
		return false;
	}		
	
//	生成完整终盘 
	if(DFS_board(/*test,*/ 0, 0, 13,op_clau,S,board,ans)){
		printf("生成终盘成功\n");
		showBoard (board);
	}else{
		printf("生成终盘失败\n");
	}
	
	return true; 
 } 

/*
每次传入一个副本，并clear 
*/



void showBoard(int board[N][N])
//20.
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
//15.
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
        fprintf(fp,"0\r");

        curr = curr->next;
    }
    
	fclose(fp);
}

/*#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

#define N 9
#define BOX_SIZE 3



// 检查在grid[row][col]填入num是否有效
bool is_valid(int grid[N][N], int row, int col, int num) {
    // 检查行
    for (int x = 0; x < N; x++) {
        if (grid[row][x] == num) {
            return false;
        }
    }

    // 检查列
    for (int x = 0; x < N; x++) {
        if (grid[x][col] == num) {
            return false;
        }
    }

    // 检查3x3宫格
    int box_start_row = row - row % BOX_SIZE;
    int box_start_col = col - col % BOX_SIZE;
    for (int i = 0; i < BOX_SIZE; i++) {
        for (int j = 0; j < BOX_SIZE; j++) {
            if (grid[box_start_row + i][box_start_col + j] == num) {
                return false;
            }
        }
    }

    return true;
}

// 使用回溯法求解数独
bool solve_sudoku(int grid[N][N], int *solution_count) {
    int row = -1;
    int col = -1;
    bool isEmpty = false;

    // 寻找空白格
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (grid[i][j] == 0) {
                row = i;
                col = j;
                isEmpty = true;
                break;
            }
        }
        if (isEmpty) {
            break;
        }
    }

    // 无空白格，求解完成
    if (!isEmpty) {
        (*solution_count)++;
        return true; // 找到解
    }

    // 尝试1-9的数字
    for (int num = 1; num <= N; num++) {
        if (is_valid(grid, row, col, num)) {
            grid[row][col] = num;
            
            if (solve_sudoku(grid, solution_count)) {
                // 如果我们只需要知道是否有多个解，可以在这里返回
                if (*solution_count > 1) {
                    return true;
                }
            }
            
            grid[row][col] = 0; // 回溯
        }
    }
    
    return false;
}

// 复制网格
void copy_grid(int src[N][N], int dest[N][N]) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            dest[i][j] = src[i][j];
        }
    }
}

// 检查解的唯一性
bool has_unique_solution(int grid[N][N]) {
    int temp_grid[N][N];
    copy_grid(grid, temp_grid);
    
    int solution_count = 0;
    solve_sudoku(temp_grid, &solution_count);
    
    return solution_count == 1;
}

// 生成完整数独终盘
void generate_full_sudoku(int grid[N][N]) {
    // 先清空网格
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            grid[i][j] = 0;
        }
    }
    
    // 生成一个随机完整网格
    srand(time(0));
    
    // 填充第一个3x3宫格
    int nums[N] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    
    // 随机打乱数字
    for (int i = 0; i < N; i++) {
        int j = rand() % N;
        int temp = nums[i];
        nums[i] = nums[j];
        nums[j] = temp;
    }
    
    // 填充第一个宫格
    int index = 0;
    for (int i = 0; i < BOX_SIZE; i++) {
        for (int j = 0; j < BOX_SIZE; j++) {
            grid[i][j] = nums[index++];
        }
    }
    
    // 使用回溯法填充剩余部分
    int solution_count = 0;
    solve_sudoku(grid, &solution_count);
}

// 挖洞法生成谜题
void generate_puzzle(int grid[N][N], int holes) {
    // 生成完整终盘
    generate_full_sudoku(grid);
    
    // 复制一份完整网格作为备份
    int solution[N][N];
    copy_grid(grid, solution);
    
    // 计算需要挖去的洞数
    int holes_dug = 0;
    int attempts = 0;
    const int max_attempts = 200; // 防止无限循环
    
    srand(time(0));
    
    while (holes_dug < holes && attempts < max_attempts) {
        // 随机选择位置
        int row = rand() % N;
        int col = rand() % N;
        
        // 如果该位置已经为空，跳过
        if (grid[row][col] == 0) {
            attempts++;
            continue;
        }
        
        // 保存当前值
        int backup = grid[row][col];
        grid[row][col] = 0;
        
        // 检查是否唯一解
        int test_grid[N][N];
        copy_grid(grid, test_grid);
        
        if (has_unique_solution(test_grid)) {
            holes_dug++;
        } else {
            // 恢复原值
            grid[row][col] = backup;
        }
        
        attempts++;
    }
    
    printf("挖洞完成: %d/%d\n", holes_dug, holes);
}

// 保存数独到文件
void save_sudoku(int grid[N][N], const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("无法打开文件");
        return;
    }
    
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            fprintf(file, "%d", grid[i][j]);
            if (j < N - 1) fprintf(file, ",");
        }
        fprintf(file, "\n");
    }
    
    fclose(file);
    printf("数独已保存到 %s\n", filename);
}

// 从文件加载数独
void load_sudoku(int grid[N][N], const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("无法打开文件");
        return;
    }
    
    char line[256];
    int row = 0;
    
    while (fgets(line, sizeof(line), file) && row < N) {
        char *token = strtok(line, ",");
        int col = 0;
        
        while (token && col < N) {
            grid[row][col] = atoi(token);
            token = strtok(NULL, ",");
            col++;
        }
        row++;
    }
    
    fclose(file);
    printf("数独已从 %s 加载\n", filename);
}

int main() {
    int grid[N][N];
    int solution[N][N];
    
    // 用户选择
    int choice;
    printf("数独谜题生成器 - 挖洞法\n");
    printf("1. 生成新谜题\n");
    printf("2. 加载已有谜题\n");
    printf("3. 求解数独\n");
    printf("请选择: ");
    scanf("%d", &choice);
    
    if (choice == 1) {
        // 生成新谜题
        int holes;
        printf("输入要挖的洞数(20-60): ");
        scanf("%d", &holes);
        
        if (holes < 20 || holes > 60) {
            printf("无效洞数，使用默认值40\n");
            holes = 40;
        }
        
        generate_puzzle(grid, holes);
        
        printf("\n生成的数独谜题:\n");
        print_grid(grid);
        
        // 保存选项
        printf("\n保存谜题? (1=是, 0=否): ");
        scanf("%d", &choice);
        if (choice == 1) {
            char filename[100];
            printf("输入文件名: ");
            scanf("%s", filename);
            save_sudoku(grid, filename);
        }
        
        // 显示答案选项
        printf("\n显示答案? (1=是, 0=否): ");
        scanf("%d", &choice);
        if (choice == 1) {
            int solution_count = 0;
            copy_grid(grid, solution);
            solve_sudoku(solution, &solution_count);
            
            printf("\n数独答案:\n");
            print_grid(solution);
        }
    }
    else if (choice == 2) {
        // 加载谜题
        char filename[100];
        printf("输入文件名: ");
        scanf("%s", filename);
        load_sudoku(grid, filename);
        
        printf("\n加载的数独谜题:\n");
        print_grid(grid);
        
        // 求解选项
        printf("\n求解数独? (1=是, 0=否): ");
        scanf("%d", &choice);
        if (choice == 1) {
            int solution_count = 0;
            copy_grid(grid, solution);
            solve_sudoku(solution, &solution_count);
            
            printf("\n数独答案:\n");
            print_grid(solution);
        }
    }
    else if (choice == 3) {
        // 手动输入谜题
        printf("输入9x9数独网格(用空格分隔，0表示空):\n");
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                scanf("%d", &grid[i][j]);
            }
        }
        
        printf("\n输入的数独谜题:\n");
        print_grid(grid);
        
        // 求解
        int solution_count = 0;
        copy_grid(grid, solution);
        if (solve_sudoku(solution, &solution_count)) {
            if (solution_count == 1) {
                printf("\n唯一解:\n");
            } else {
                printf("\n多个解，显示其中一个:\n");
            }
            print_grid(solution);
        } else {
            printf("\n无解!\n");
        }
    }
    
    return 0;
}*/


