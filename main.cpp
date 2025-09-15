#define _CRT_SECURE_NO_WARNINGS
#include "func.h"
//外部变量初始化
int board[N][N] = {0};
int record[N][N] = {0};
int user[N][N] = {0};
int mode = 0;
int score[2 * MAX_VAR + 1] = {0};	//哈希表用来进行chooselit2/jw/hy 
double jw[2 * MAX_VAR + 1] = {0.0};
int game_flag = TRUE;
int style = 0;


int main()
{
	clock_t start, end;
	double used_time;
	srand(time(NULL));
	const char* inSuf = ".cnf"; char file[FILE_MAX];
    int choice = 0, num_var = 0;
    CNF S;	initCNF(S, 0);
    LITSHEET *ans = (LITSHEET *)malloc(sizeof(LITSHEET) * (1 + MAX_VAR));
	if(!ans){
		printf("NO MEMORY for ans!\n");
		return 0;
	}    
//	CNF newS;	initCNF(newS, 0);

	for(int i = 0;i <= MAX_VAR; i++){
			ans[i].ans = 0;
			ans[i].pos = NULL;
			ans[i].nag = NULL;
	}
	printf("_________HELLO_____AVA_______\n");
    printf("What's your aim?\n");
    printf("1. Build CNF from file and Solve\n");
    printf("2. Play SUDOKU\n");
    printf("3. Exit\n");
    printf("________SUDOKU_____LOL_______\n");
    scanf("%d", &choice);

    while(1){

        switch (choice) {
        	
        	case 1:{
            	printf("please input your filename(NOT need type .cnf):\n");
            	scanf("%s",file);
            	char inFile[FILE_MAX];
            	strcpy(inFile, file);
            	strcat(inFile, inSuf);

				int tempcp = CNFparser(S,inFile,ans);
            	if( tempcp== FALSE){
					printf("ERROR in CNFparser!\n");
					break;
				}else if(tempcp == INFEASIBLE)	{
					printf("realloc ERROR\n");
					break;
				}
            	num_var = S.num_var;
            	printf("采用哪一种DPLL方式？\n");
				scanf("%d",&style)	;
				
/*1.false子句第一个	2.按频率选	3.按长度选	4.23结合 */
				if(style == 1){
					;//不需要初始化 
				}else if(style == 2){
					clit_2_init(num_var,ans);
				}else if(style == 3){
					clit_jw_init(num_var,ans);
				}else if(style == 4){
					clit_hybrid_init(num_var,ans);
				}else{
					printf("What 's wrong with you?!\n");
				}
	            	
				
                if ( num_var ) {
					// showCNF(S);

					start = clock();
					
					bool t ;
					if(style == 1){
						t = DPLL_1(S, ans);
					}else if(style == 2){
						t = DPLL_2(S, ans);
					}else if(style == 3 || style == 4){		//3和4共用1个计分数组 
						t = DPLL_3(S, ans);
					}else	printf("What 's wrong with you?!\n");
					
					
					end = clock();
					used_time = ((double)(end - start)) / CLOCKS_PER_SEC;
					
					if( t ){
						printf("\n\n");
						check(ans, num_var);
						ans[0].ans = 1;
						
						if(autocheck(S,ans)){
							printf("\n\nAC\n\n");
						}else{
							printf("\n\nWA\n\n");
						}						
					}else{
						printf("\nNO SOLUTION !\n");
						ans[0].ans = 0;
					}					
					printf("Duration:%.3f ms\n",used_time * 1000);
				}else 
                    printf("Failed to parse CNF because no var!\n");
                    
				Sleep(500);
				printf("\nTry to Save as .res~~~\n");
				
				if(saveOutput(ans,num_var,file,used_time))	printf("RES file has been saved !\n\n");
				else										printf("ERROR!File NOT be saved\n\n");			
//				showCNF(S);
//				printf("AFTER RECOVER NUM_CLAU = %d\n\n",S.num_clau);
				printf("\nstart delete and init ...\nsuccess!\n\n");	
				clearCNF(S,ans);
				clearExtarr();
				style = 0;
                break;            	
			}
                
            case 2:{
            	style = 1;
            	initCNF(S,SUDOKU_VAR);
				char files[20] = "CNF_output.cnf";
				bool flag = false, t;
				
            	printf("Choose your game:1->SUDOKU 2->%%-SUDOKU 0->QUIT\n");
            	scanf("%d",&mode);	  
       	
            	if(!mode)	break;
				else if(mode == 1){
					
					fundConsCNF(S,ans);
					
					t = generate_1(S, /*board,*/ ans);
					while(t == false){
						static int howmany = 0;
						howmany ++;		
						if(howmany >= MAX_GENERATE_1)	{
							flag = true;
							printf("无法生成数独！\n");
							break;
						}
						printf("由于运气有点差QAQ,这一次生成终盘没有成功，正在重新尝试呢！AvA\n");
						t =  generate_1(S,/* board,*/ ans);
           	
					}					
				}		
				else if(mode == 2){
					
					fundConsCNF(S,ans);
					percentConsCNF(S,ans);
					
					t = generate_2(S, /*board,*/ ans);
					while(t == false){
						static int howmany = 0;
						howmany ++;		
						if(howmany >= MAX_GENERATE_2)	{
							flag = true;
							printf("无法生成数独！\n");
							break;
						}
						printf("由于运气有点差QAQ,这一次生成终盘没有成功，正在重新尝试呢！AvA\n");
						t =  generate_2(S,/* board, */ans);          	
					}					
				}	
				else{
					printf("无效输入，请重试\n");
					break;
				} 
					
				if(flag){
					clearCNF(S,ans);
					recoverBoard(board); 
					break;
				}
					
				//先存储正确答案 
				copyBoard(record, board);
				
				
				printf("请选择难度： 1 for easy,2 for mid, 3 for high\n");
				int diff, blank;	scanf("%d",&diff);
				if(diff == 1){
					blank = EASY;
				}else if(diff == 2){
					blank = MID;
				}else if(diff == 3){
					blank = HARD;
				}else{
					printf("输入错误，将自动生成easy难度\n");
					blank = EASY;
				}		
				
				int manytime = 0;
				while(dig_holes( /*board,*/blank/*, mode*/) == -1 && manytime <= INIT_NUM_1){
					manytime ++;
					showBoard(board);
					copyBoard(board, record);
				}
				if(manytime > INIT_NUM_1){
					printf("挖洞失败，构造数独失败！");					
					clearCNF(S,ans);
					recoverBoard(board);
					recoverBoard(record);
					initCNF(S, 0);
					style = 0;
					break;					
				}
				
				play();
										
				//结束后清零 
				clearCNF(S,ans);
				recoverBoard(board);
				recoverBoard(record);
				initCNF(S, 0);
				style = 0;
				break;
				
	}
            case 3:{
                printf("Exiting...\n");
                free(ans);
                return 0;           	
			}

            default:{
                printf("Invalid choice. Please try again.\n");
                break;
			}

        }
        
        printf("What's your next order?\n");
        scanf("%d", &choice);
    }

    return 0;
}
