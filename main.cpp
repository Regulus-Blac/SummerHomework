#define _CRT_SECURE_NO_WARNINGS
#include "func.h"
 
int main()
{
	clock_t start, end;
	double used_time;
	srand(time(NULL));
	const char* inSuf = ".cnf"; char file[FILE_MAX];
    int board[N][N], record[N][N];
    recoverBoard( board);	recoverBoard( record);
    int choice = 0, num_var = 0;

	CNF S;  	initCNF(S, 0);
	CNF newS;	initCNF(newS, 0);
	LITSHEET *ans = (LITSHEET *)malloc(sizeof(LITSHEET) * (1 + MAX_VAR));

	if(!ans){
		printf("NO MEMORY for ans!\n");
		return 0;
	}
	for(int i = 0;i <= MAX_VAR; i++){
			ans[i].ans = 0;
			ans[i].pos = NULL;
			ans[i].nag = NULL;
	}

    printf("What's your aim?\n");
    printf("1. Build CNF from file and Solve\n");
    printf("2. Play SUDOKU\n");
    printf("3. Exit\n");
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
//				printf("BEFORE:NUM_CLAU= %d\n",S.num_clau); 
				//showLitsheet(ans,729);
                if ( num_var ) {
					// showCNF(S);

					start = clock();
					
					bool t = DPLL(S, ans/*, test*/);
					
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
					
                break;            	
			}
                
            case 2:{
            	int mode;
				char files[20] = "CNF_output.cnf";
				bool flag = false, t;
            	printf("Choose your game:1->SUDOKU 2->%%-SUDOKU 0->QUIT\n");
            	scanf("%d",&mode);	  
       	
            	if(!mode)	break;
				else if(mode == 1){
					
					fundConsCNF(S,ans);
					
					t = generate_1(S, board, ans);
					while(t == false){
						static int howmany = 0;
						howmany ++;		
						if(howmany >= MAX_GENERATE_1)	{
							flag = true;
							printf("无法生成数独！\n");
							break;
						}
						printf("由于运气有点差QAQ,这一次生成终盘没有成功，正在重新尝试呢！AvA\n");
						t =  generate_1(S, board, ans);
           	
					}					
				}		
				else if(mode == 2){
					
					fundConsCNF(S,ans);
					percentConsCNF(S,ans);
					
					t = generate_2(S, board, ans);
					while(t == false){
						static int howmany = 0;
						howmany ++;		
						if(howmany >= MAX_GENERATE_2)	{
							flag = true;
							printf("无法生成数独！\n");
							break;
						}
						printf("由于运气有点差QAQ,这一次生成终盘没有成功，正在重新尝试呢！AvA\n");
						t =  generate_2(S, board, ans);          	
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
				showBoard(record);
				
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
				
				if(dig_holes( board,blank, mode)){
//					printf("挖洞成功！挖了%d个空，棋盘现在为：\n",blank);
//					showBoard(board);
					play(record,board); 
				}else{
					printf("挖洞失败，构造数独失败！");
					clearCNF(S,ans);
					recoverBoard(board);
					recoverBoard(record);
					break;
				}	
			
				
				//结束后清零 
				clearCNF(S,ans);
				recoverBoard(board);
				recoverBoard(record);
				initCNF(S, 0);
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
