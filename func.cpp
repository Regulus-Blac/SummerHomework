#include "func.h"

/*构建stack 部分 */

/*DPLL 部分 1-24  共22个关键函数函数*/

void makecopy(const CNF &S,CNF &newS) 
//1.将S拷贝到newS (比操作栈要费时费空间，不过直观)
{
	newS.num_clau = S.num_clau;
	newS.num_var = S.num_var;
	
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
    
    S.head = NULL;
    S.tail = NULL;
    S.num_clau = 0;
    S.num_var = 0;
    
    for(int i = 0;i <= sizeof(ans)/sizeof(LITSHEET); i++){
		ans[i].ans = 0;
		ans[i].pos = NULL;
		ans[i].nag = NULL;
	}	
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
        int a;
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
        if(!flag)	return false;

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
		if(!ans[index].pos){
			ans[index].pos = lit;
		}
		else{
			lit->next = ans[index].pos;
			ans[index].pos = lit;
		}
	}else{
		if(!ans[index].nag){
			ans[index].nag = lit;
		}
		else{
			lit->next = ans[index].nag;
			ans[index].nag = lit;
		}		
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
	if(S.num_var > SUDOKU_VAR) {
		
		LITSHEET *temp = (LITSHEET *)realloc(ans,(S.num_var + 1)* sizeof(LITSHEET) );
		
		if(temp == NULL){
			printf("realloc ERROR!");
			free(ans);
			return NULL;
		}
		ans = temp;
		for(int i = 0;i <= S.num_var; i++){
			ans[i].ans = 0;
			ans[i].pos = NULL;
			ans[i].nag = NULL;
		}
			
	} else{
		for(int i = 0;i <= SUDOKU_VAR; i++){
			ans[i].ans = 0;
			ans[i].pos = NULL;
			ans[i].nag = NULL;
		}		
	}
// 写入CNF

    if(!buildCNF(S, S.num_clau, fp, ans)) {
        printf("Error: CNF already exists.\n");
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
//10
{
	int cnt;
	for(int i = 1;i <= range; i++){
		cnt = 0;
		INFO_L *pos = ans[i].pos, *nag = ans[i].nag;
		while(pos){
			cnt ++;
			pos = pos->next;
		}
		printf("var %d occur in %d clause\n",i,cnt);
		cnt = 0;
		while(nag){
			cnt ++;
			nag = nag->next;
		}		
		printf("var %d occur in %d clause\n",-i,cnt);
	}
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

//bool isUnitClause(CLAUSE *clause)
//// 判断是否为单子句
//{
//    if (clause == NULL|| clause->num != 1) {
//        return false; // 空子句不是单子句
//    }
//    return true;
//}

CLAUSE *existUnitClause(CLAUSE *head)
//12. 在子句链表中查找第一个单子句
{
    CLAUSE *current = head;
    while (current != NULL) {
        if (current->num == 1) {
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
	CLAUSE *curr = head;
	if(curr == node)	return NULL;
	while(curr){
		
		if(curr->next == node)
			return curr;
		
		curr = curr->next;
	}
	
	return NULL;
}

int deleteOneClause(CLAUSE *Node, stack <INFO_C> &op_clau, CNF &S, LITSHEET* ans) 
//15.将指针放进回收栈，返回单子句中变量的值 
{
	if(!Node || !Node->lit)			return 0;
	
	int value; 
	INFO_C opt;
	CLAUSE *node = Node, *pre = locatePre(node, S.head);
	LITERAL *lit = node ->lit;

	if(pre == NULL){						//	first clau
		opt.self = S.head;
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
		if(ans[abs(value)].ans == 0)	return value;
		lit = lit->next;
	}

	return 0;
}

int deleteClause(stack <INFO_C> &op_clau, CNF &S, LITSHEET* ans, int backtrace[]/*,FILE *test*/)
//16. 删除所有单子句，并对CNF进行化简；记录操作次数,将单子句中变量值记录 
{
	if(S.num_clau == 0)		return 0; 
	
	CLAUSE *node = existUnitClause(S.head);
	int cnt = 0, value = 0;
	
	while(node != NULL){
		// 找到并删除单子句
		
		if(node->isTrue == true){
			node = existUnitClause(node->next);
			continue;
		}	 
		
		node->isTrue = true; 
		value = deleteOneClause(node, op_clau, S,ans);
		
		/*printf("delete single clause with %d\n",value);
		showCNF(S); //每删一个单子句 */
		
		if(!value)		return 0; 
		
		cnt ++;
		// 保存记录 
		if(value > 0){
			ans[value].ans = 1;
		}else if(value < 0){
			ans[-value].ans = 2;
		}	
		backtrace[++backtrace[0]] = value;
		
		// 化简CNF,并记录删除真子句数		
		cnt += deleteLit(op_clau, S, value,ans/*, num_TrueClaus*//*,test*/); 
		
		/*showCNF(S); //每删一个文字*/
				
		node = existUnitClause(node->next);			// 记住是Next,不然会死循环 
		
	}
	
	return cnt;
}

int deleteLit( stack <INFO_C> &op_clau, CNF &S, int value, LITSHEET* ans/*,FILE *test*/)
//17.	删除子句中的文字，同时也会删除真子句并记录真子句次数 
{
	if(!S.num_clau || !S.head)	return 0;			//	S已经为空 
	
	int cnt = 0, index = abs(value);		//	直接删除包含value的子句，删除-value的文字 
	INFO_L * pos = ans[index].pos;	INFO_L *nag = ans[index].nag;
	
	if(value > 0){	
		while(pos){
			if(pos->clause->isTrue == false) {
				pos->clause->isTrue = true;
				deleteOneClause(pos->clause, op_clau,S,ans);
				cnt ++;
			}
			
			pos->clause->num --;
				
			pos = pos->next;
		}
		
		while(nag){		
			nag->clause->num --;
			nag = nag->next;
		}
		
	}else{
		while(nag){
			if(nag->clause->isTrue == false) {
				nag->clause->isTrue = true;
				deleteOneClause(nag->clause, op_clau,S,ans);
				cnt ++;
			}
			
			nag->clause->num --;
				
			nag = nag->next;
		}
		
		while(pos){		
			pos->clause->num --;
			pos = pos->next;
		}				
	}
	/*
printf("delete lit %d\n",value); 
showCNF(S); //每删一个文字*/
	return cnt;
}

status restore_cl(int num, stack <INFO_C> &op_clau, CNF &S)
//18.	恢复若干次语句指针 
{
	INFO_C temp;
	
	for(int i = 0;i < num; i++){
		
		if(op_clau.empty())	return INFEASIBLE;
		
		temp = op_clau.top();
		if(!temp.self)	return FALSE;
		
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
		
	}

	return TRUE;
}

status restore_lit(int back[], LITSHEET* ans)
//19.	恢复若干次文字指针(只恢复 变元bool值，子句的文字数，不管子句bool 
{
	for(int i = back[0];i > 0 ;i--){
		int index = abs(back[i]);
		ans[index].ans = 0;		//bool值
		
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
	return TRUE;
}

int choose_lit(CLAUSE *head, LITSHEET* ans)
//20.	选择第一个还是false的子句的第一个有效文字赋值 
{
	CLAUSE *node = head;
	while(node){
		if(node->isTrue == false) {
			LITERAL *lit = node->lit;
			while(lit){
				int i = lit->value;
				if(ans[abs(i)].ans == 0)		return i;
				lit = lit->next;
			}
		
		}
		node = node->next;
	}

	return 0;
}

bool DPLL( stack <INFO_C> &op_clau, CNF &S, LITSHEET* ans/*, FILE *test*/)
//21.	CORE
{
	int backtrace[MAX_BACK] = {0}; 			//back【0】为个数
 	int cntCLAU = 0 ;
 	int chosenlit = 0;
 	
 	
 	if(ans[0].ans != 0) {
 		chosenlit = ans[0].ans;
 		cntCLAU += deleteLit(op_clau, S, chosenlit, ans/*,test*/);
 		
 		backtrace[0] ++;
 		backtrace[1] = chosenlit;
	}
	// 删除所有单子句，删除包含该文字的子句，删去反文字,并记录操作次数

	cntCLAU += deleteClause(op_clau,S, ans, backtrace/*,test*/);

 	// 1.成功解出 
	if(S.num_clau == 0 || S.head == NULL )	return true;
	// 2.出现矛盾 

	else if(existEmptyClause(S.head)) {

		restore_cl(cntCLAU, op_clau, S);
		restore_lit(backtrace, ans);

		return false;		
	}	
	
	// 3.选取一个变量赋值 
	int chosen = choose_lit(S.head,ans), index = abs(chosen);
	if(!index)	return false;
	/*printf("choose %d as a break\n",chosen);*/

	ans[index].ans = (chosen > 0) ? 1:2;
	ans[0].ans = chosen;
	if(DPLL(op_clau, S, ans/*,test*/))	return true;
	

	ans[index].ans = (chosen > 0) ? 2:1;
	ans[0].ans = -chosen;
	if(DPLL(op_clau, S, ans/*,test*/))	return true;
	

	//	当前分支失败，开始回溯 
	ans[index].ans = 0;
    
	restore_cl(cntCLAU, op_clau, S);	
	restore_lit(backtrace, ans);

	return false;
}

void check(LITSHEET *ans, int cnt)
//22.	输出直观结果 
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
//23.
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
//24.	按要求保存结果文件 
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
//void recoverCNF(CNF &S, stack <INFO_C> &op_clau,int ans[])
////1.	将删去的节点全部复原
//{ 
//	while(!op_clau.empty()){
//		restore_cl(1, op_clau, S,ans);
//	}
//	for(int i = 0;i <= SUDOKU_VAR; i++)	ans[i] = 0; 
//}
//
//bool fundConsCNF(CNF &S)
////2.	将格、行列、宫限制转化为CNF (total 11988)
//{
//	if(S.num_clau) return false;
//	
//	S.num_var = 729;
//	int temp[9] = {1,2,3,4,5,6,7,8,9}, t = 1;
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
//// 行约束	
//	for(int i = 0;i < 9;i ++){
//		temp[i] = t;
//		t += 9;
//	}
//	
//	for(int row = 0;row < 9;row ++){		//9行
//	
//		for(int j = 0;j < 9;j++){
//		//row行不能重复j -1 -10 
//			for(int i = 0; i < 9; i++){
//				for(int k = i + 1;k < 9; k++){
//					createClause(S);
//					LITERAL *a = (LITERAL *)malloc(sizeof(LITERAL)), *b = (LITERAL *)malloc(sizeof(LITERAL));
//					a->next = NULL;	b->next = NULL;
//					a->value = -temp[i];b->value = -temp[k];
//					
//					S.tail->lit = a;
//					a->next = b;
//					S.tail->num = 2;
//				}
//			}			
//		//row行都有j 1 10 19... 73
//			createClause(S);		
//			for(int i = 0;i < 9;i ++){
//					
//		        LITERAL *newLiteral = (LITERAL *)malloc(sizeof(LITERAL));
//		        newLiteral->next = NULL;        
//		        newLiteral->value = temp[i]; 
//		        temp[i] += 1;				//	为 j 下一轮循环做准备
//					
//				if(S.tail->lit == NULL) {
//		            S.tail->lit = newLiteral;
//		        } else {
//		            LITERAL *lastLit = S.tail->lit;
//		            newLiteral->next = lastLit;
//		            S.tail->lit = newLiteral; 
//		        }
//					
//			}
//			S.tail->num = 9;	
//
//		}
//		
//		// 初始化辅助数组(下一行的 
//		temp[0] = temp[8] + 1;		
//		for(int i =1;i < 9;i ++)	temp[i] = temp[i-1] + 9;		
//	}
//	S.num_clau += 2997;
//// 列约束
//	t = 1;
//	for(int i = 0;i < 9;i ++){
//		temp[i] = t;
//		t += 81;
//	}
//	
//	for(int col = 0;col < 9;col ++){		//9列 
//	
//		for(int j = 0;j < 9;j++){
//		//col列不能重复j -1 -82 
//			for(int i = 0; i < 9; i++){
//				for(int k = i + 1;k < 9; k++){
//					createClause(S);
//					LITERAL *a = (LITERAL *)malloc(sizeof(LITERAL)), *b = (LITERAL *)malloc(sizeof(LITERAL));
//					a->next = NULL;	b->next = NULL;
//					a->value = -temp[i];b->value = -temp[k];
//					
//					S.tail->lit = a;
//					a->next = b;
//					S.tail->num = 2;
//				}
//			}			
//		//col列都有j 1 82 163... 649
//			createClause(S);		
//			for(int i = 0;i < 9;i ++){
//					
//		        LITERAL *newLiteral = (LITERAL *)malloc(sizeof(LITERAL));
//		        newLiteral->next = NULL;        
//		        newLiteral->value = temp[i]; 
//		        temp[i] += 1;				//	为 j 下一轮循环做准备
//					
//				if(S.tail->lit == NULL) {
//		            S.tail->lit = newLiteral;
//		        } else {
//		            LITERAL *lastLit = S.tail->lit;
//		            newLiteral->next = lastLit;
//		            S.tail->lit = newLiteral; 
//		        }
//					
//			}
//			S.tail->num = 9;	
//
//		}		
//	}
//	S.num_clau += 2997;
//// 宫约束
//	t = 1;	temp[0] = t;
//	for(int i = 1;i < 9;i ++){
//		temp[i] = temp[i - 1] + ((i % 3 == 0) ? 63 : 9);
//	}
//	
//	for(int row = 0; row < 3; row ++){		//宫按行分，有三行 
//	
//		for(int col = 0; col < 3; col++){	//每行有三个 
//		
//			for(int j = 0;j < 9;j++){		
//				//	每个宫内不重复j -1 -10 
//				for(int i = 0; i < 9; i++){
//					for(int k = i + 1;k < 9; k++){
//						createClause(S);
//						LITERAL *a = (LITERAL *)malloc(sizeof(LITERAL)), *b = (LITERAL *)malloc(sizeof(LITERAL));
//						a->next = NULL;	b->next = NULL;
//						a->value = -temp[i];b->value = -temp[k];
//						
//						S.tail->lit = a;
//						a->next = b;
//						S.tail->num = 2;
//					}
//				}
//				//	每个宫内都有j	
//				createClause(S);		
//				for(int i = 0;i < 9;i ++){
//						
//			        LITERAL *newLiteral = (LITERAL *)malloc(sizeof(LITERAL));
//			        newLiteral->next = NULL;        
//			        newLiteral->value = temp[i]; 
//			        temp[i] += (j == 8) ? 19 : 1;				//	为 j 下一轮以及下一个col循环做准备
//						
//					if(S.tail->lit == NULL) {
//			            S.tail->lit = newLiteral;
//			        } else {
//			            LITERAL *lastLit = S.tail->lit;
//			            newLiteral->next = lastLit;
//			            S.tail->lit = newLiteral; 
//			        }
//						
//				}
//				S.tail->num = 9;									
//			} 
//			
//		} 
//		
//		//	为下一行初始化辅助数组 
//		temp[0] = temp[8] - 18;//-19+1
//		for(int i = 1;i < 9;i ++){
//			temp[i] = temp[i - 1] + ((i % 3 == 0) ? 63 : 9);
//		}
//	}	
//	S.num_clau += 2997;
//
//	return true; 
//}
//
//bool percentConsCNF(CNF &S)
////3.	将反对角线、窗口限制转化为CNF(total 999)
//{
//	if(!S.num_clau || !S.head) return false;
//	
//	int temp[9];	temp[0] = 73;
//	for(int i = 1;i < 9;i++)	temp[i] = temp[i - 1] + 72; 
////反对角线约束
//	for(int j = 0;j < 9;j ++){
//		//不能重复j 
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
//			}
//		}
//		//反对角线有j	
//		createClause(S);		
//		for(int i = 0;i < 9;i ++){
//				
//	        LITERAL *newLiteral = (LITERAL *)malloc(sizeof(LITERAL));
//	        newLiteral->next = NULL;        
//	        newLiteral->value = temp[i]; 
//	        temp[i] += 1;				//	为 j 下一轮循环做准备
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
//	S.num_clau += 333;
////窗口限制				 
//
//	temp[0] = 91;
//	for(int i = 1;i < 9;i ++){
//		temp[i] = temp[i - 1] + ((i % 3 == 0) ? 63 : 9);
//	}	
//
//	for(int window = 0; window < 2; window ++){
//		for(int j = 0;j < 9;j ++){
//			//不能重复j 
//			for(int i = 0; i < 9; i++){
//				for(int k = i + 1;k < 9; k++){
//					createClause(S);
//					LITERAL *a = (LITERAL *)malloc(sizeof(LITERAL)), *b = (LITERAL *)malloc(sizeof(LITERAL));
//					a->next = NULL;	b->next = NULL;
//					a->value = -temp[i];b->value = -temp[k];
//					
//					S.tail->lit = a;
//					a->next = b;
//					S.tail->num = 2;
//				}
//			}
//			//反对角线有j	
//			createClause(S);		
//			for(int i = 0;i < 9;i ++){
//					
//		        LITERAL *newLiteral = (LITERAL *)malloc(sizeof(LITERAL));
//		        newLiteral->next = NULL;        
//		        newLiteral->value = temp[i]; 
//		        temp[i] += (j == 8) ? 352 : 1;				//	为 j 下一轮循环做准备
//					
//				if(S.tail->lit == NULL) {
//		            S.tail->lit = newLiteral;
//		        } else {
//		            LITERAL *lastLit = S.tail->lit;
//		            newLiteral->next = lastLit;
//		            S.tail->lit = newLiteral; 
//		        }
//					
//			}
//			S.tail->num = 9;	
//		} 
//		S.num_clau += 333;	
//	} 
//
//	return true;// 共12987个子句 
//}
//
//int ijk_cnf(int ijk)
////4.	棋盘信息转化为cnf变量 
//{
//	int cnf = 0;	bool minus = false;
//	if(ijk < 0)	minus = true;
//	ijk = abs(ijk); 
//	
//	cnf += (ijk/100 - 1)  *81 ;
//	ijk %= 100;
//	cnf += (ijk/10 - 1)	*9;
//	ijk %= 10;
//	cnf += ijk;
//	
//	if(minus)	cnf *= -1;
//	
//	return cnf;
//}
//
//int cnf_ijk(int cnf)
////5.	逆变换 
//{
//	int ijk = 0;	bool minus = false;
//	if(cnf < 0)	minus = true;
//	cnf = abs(cnf); 
//    int devide[2] = {81, 9};
//    
//    for(int i = 0; i < 2; ++i){
//        if(cnf % devide[i] != 0){
//            ijk += (cnf / devide[i] + 1) * (i == 0 ? 100 :  10 );
//            cnf %= devide[i];
//        } else {
//            ijk += (cnf / devide[i]) * (i == 0 ? 100 : 10);
//            cnf = devide[i];
//        }
//    }
//
//	ijk += cnf;
//	
//	if(minus)	ijk *= -1;
//	
//	return ijk;
//}
//
//int randomNum(int *value)
////6.	根据指令生成位置的随机值
//{
//	int range, ret;
//	//	位置 0-80
//	ret = rand() % 81;
//	//	值 1-9
//	*value = rand() % 9 + 1;
//	
//	return ret;	
//} 
//
//CLAUSE *addUnitClause(CNF &S, int value)
////7.	末尾增加单子句作为填数 
//{
//	CLAUSE *base = S.tail;
//	createClause(S);
//	S.num_clau ++;
//	LITERAL *lit = (LITERAL *)malloc(sizeof(LITERAL));
//	lit->next = NULL;
//	lit->value =value;
//	S.tail->lit = lit;
//	S.tail->num = 1;
//	
//	return base;
//}
//
//bool deleteS_tail(CNF &S, CLAUSE *base)
////8.	删掉末尾子句便于回溯 
//{
//	if(base->next != S.tail)	return false;
//	
//	free(S.tail->lit);
//	free(S.tail);
//	base->next = NULL;
//	S.tail = base;
//	S.num_clau --;
//	return true;
//}
//
//bool DFS_board(/*FILE *test,*/int row, int col, int cnt,  stack <INFO_C> &op_clau, CNF &S, int board[N][N],int ans[])
////9.
//{
//	if(cnt == 81)	return true;	//	已填满
//	 
//	int value, i, j;
//	bool flag = false;
//	CLAUSE *base; 
//	for(i = row; i < N; i ++){
//		for(j = col; j < N;j ++){
//			if(!board[i][j])	break;
//		}
//	}			// 找到第一个空位处 
//			
//
//		for(int k = 1; k < 10;k ++){
//			board[i][j] = k;
//			value = ijk_cnf((i+1)*100+(j+1)*10+k);	//棋盘位置加填的数转化为cnf变元的值 
//			base = addUnitClause(S, value);
//			
//			if(DPLL(op_clau, S, ans/*,test*/)){
//				flag =  DFS_board(/*test,*/i + (j + 1)/9, (j + 1)%9, cnt + 1,op_clau,S,board,ans);
//				if(flag)	return true;
//			}
//			//	DPLL或DFS失败
//			recoverCNF(S, op_clau,ans);
//			deleteS_tail(S, base);
//		}
//		
//		board[i][j] = 0; 
//
//	return false;	
//}
//
//
//bool generate(/*FILE *test,*/ stack <INFO_C> &op_clau, CNF &S, int board[N][N], int ans[])
////10.	通过拉斯维加斯算法生成终盘
//{
//	int i = 0, j = 0, t, cnt = 13, value = 0, *p_value = &value;
//	bool btemp = false;
//	CLAUSE *base = S.tail;		//	作为CNF新增条目的上一个 
//
//// Las Vagas随机选13个位置并随机选数 
//	while(cnt){
//		t = randomNum(p_value);
//		i = t / 9;
//		j = t % 9;
//		if(board[i][j])	continue;	//该位置已经填入过
//		
//		t = ijk_cnf((i + 1)* 100+ (j + 1)* 10+ value);
//		base = addUnitClause(S, t);
//		
//		btemp = DPLL( op_clau, S, ans/*,test*/);
//		printf("bool :%d\n",btemp);
//		recoverCNF(S,  op_clau, ans);
//		printf("bool :%d\n",btemp);
//		if(btemp){
//			board[i][j] = value;
//			cnt --;
//			base = S.tail;
//		}else{
//			if(!deleteS_tail(S, base)){
//				printf("ERROR\n");
//				break;
//			}	
//		}
//	
//		printf("check info :cnt = %d\nS.num_clau = %d\n",cnt,S.num_clau);
//		showClause(S.tail);printf("\n");
//	}
////	生成完整终盘 
//	if(DFS_board(/*test,*/ 0, 0, 13,op_lit,op_clau,S,board,ans)){
//		printf("生成终盘成功\n");
//		showBoard (board);
//	}else{
//		printf("生成终盘失败\n");
//	}
//	
//	
//	return true; 
// } 
//
//
//
//
//
//void showBoard(int board[N][N])
////20.
//{
//    printf("    1   2   3   4   5   6   7   8   9\n");
//    printf("  ╔═══╤═══╤═══╦═══╤═══╤═══╦═══╤═══╤═══╗\n");//开头2个空格
//    
//    for(int i = 0; i < N; i++) {
//
//        printf("%d ║",i+1);
//        
//        for(int j = 0; j < N; j++) {
//            // 0显示为空格
//            if(board[i][j] == 0) {
//                printf("   ");
//            } else {
//                printf(" %d ", board[i][j]);
//            }
//
//            if(j == 8) {
//                printf("║");
//            } else if(j % 3 == 2) {
//                printf("║");
//            } else {
//                printf("│");
//            }
//        }
//        printf("\n");
//
//        if(i == 8) {
//            printf("  ╚═══╧═══╧═══╩═══╧═══╧═══╩═══╧═══╧═══╝\n");
//        } else if(i % 3 == 2) {
//            printf("  ╠═══╪═══╪═══╬═══╪═══╪═══╬═══╪═══╪═══╣\n");
//        } else {
//            printf("  ╟───┼───┼───╫───┼───┼───╫───┼───┼───╢\n");
//        }
//    }
//}


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


