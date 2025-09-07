#define _CRT_SECURE_NO_WARNINGS
#include "func.h"
 
int main()
{
	clock_t start, end;
	double used_time;
	srand(time(NULL));
	const char* inSuf = ".cnf"; char file[FILE_MAX];
    int board[N][N] = {0};
    int choice = 0, num_var = 0;

	CNF S;  initCNF(S, 0);
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

            	if(CNFparser(S,inFile,ans) == FALSE){
					printf("ERROR in CNFparser!\n");
					break;
				}	

            	num_var = S.num_var;
				printf("BEFORE:NUM_CLAU= %d\n",S.num_clau); 
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
					}else{
						printf("\nNO SOLUTION !\n");
						ans[0].ans = 0;
					}
					
					if(autocheck(S,ans)){
						printf("\n\nAC\n\n");
					}else{
						printf("\n\nWA\n\n");
					}
					
					printf("Duration:%.3f ms\n",used_time * 1000);
				}else 
                    printf("Failed to parse CNF because no var!\n");
                    
				Sleep(500);
				printf("\nTry to Save as .res~~~\n");
				
				if(saveOutput(ans,num_var,file,used_time))	printf("RES file has been saved !\n\n");
				else										printf("ERROR!File NOT be saved\n\n");
				
//				showCNF(S);
				printf("AFTER RECOVER NUM_CLAU = %d\n\n",S.num_clau);
				printf("\nstart delete and init ...\nsuccess!\n\n");	
				clearCNF(S,ans);
					
                break;            	
			}
                
            case 2:{
            	// int mode;char files[20] = "CNF_output.cnf";
            	// printf("Choose your game:1->SUDOKU 2->%%-SUDOKU 0->QUIT\n");
            	// scanf("%d",&mode);
            	
            	// if(!mode)	break;
				// fundConsCNF(S,ans);
				// if(mode == 2)	percentConsCNF(S,ans);
				
				// Output_CNF(S);
				
				// generate( op_clau, S, board, ans);
				// clearCNF(S, op_clau, ans);
				
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
