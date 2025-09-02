#include "func.h"
 
int main()
{
	clock_t start, end;
	double used_time;
	srand(time(NULL));
	const char* inSuf = ".cnf";
	/*FILE *test = fopen("000.txt","w");*/
	stack <INFO_C> op_clau;
	CNF S;  initCNF(S, 0);
	CNF sudoku;	initCNF(sudoku, SUDOKU_VAR);
	LITSHEET *ans = (LITSHEET *)malloc(sizeof(LITSHEET) * (1 + MAX_VAR));
	
    char file[FILE_MAX];
    int board[N][N] = {0};
    int choice = 0, num_var = 0;

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
            	
            	ans = CNFparser(S,inFile,ans);
            	if(!ans)	break;

            	num_var = S.num_var;


                if ( num_var ) {
//					showCNF(S);

					start = clock();
					
					bool t = DPLL(op_clau, S, ans/*, test*/);
					
					end = clock();
					used_time = ((double)(end - start)) / CLOCKS_PER_SEC;
					
					if( t ){
						printf("\n\n");
						check(ans, num_var);
						/*check(test,ans, num_var);
						fclose(test);*/
						ans[0].ans = 1;
					}else{
						printf("\nERROR! no solution\n");
						ans[0].ans = 0;
					}

					printf("Duration:%.3f ms\n",used_time * 1000);
				}
                    
                else 
                    printf("Failed to parse CNF.\n");
                    
				Sleep(500);		//¼ä¸ô0.5s 
				printf("\nTry to Save as .res~~~\n");
				
				if(saveOutput(ans,num_var,file,used_time))	printf("RES file has been saved !\n");
				else										printf("ERROR!File NOT be saved\n\n");
				

				printf("\nstart delete and init ...\nsuccess!\n\n");
				
				clearCNF(S, op_clau, ans) ;
				for(int i = 0;i <= num_var; i++)	ans[i].ans = 0; 
	
                break;            	
			}
                
            case 2:{
            	int mode;char files[20] = "CNF_output.cnf";
            	printf("Choose your game:1->SUDOKU 2->%%-SUDOKU 0->QUIT\n");
            	scanf("%d",&mode);
            	
            	if(!mode)	break;
				fundConsCNF(sudoku,ans);
				if(mode == 2)	percentConsCNF(sudoku,ans);
				
				Output_CNF(sudoku);
				CNFparser(S,files,ans);
				
				printf("sudoku num_clau = %d \n",sudoku.num_clau);
				
				generate(/*test,*/ op_clau, sudoku, board, ans);
				clearCNF(sudoku, op_clau, ans);
				
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
