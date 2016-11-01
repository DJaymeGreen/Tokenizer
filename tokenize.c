/* Danielle Jayme Green */

#include <stdio.h>
#include <string.h>
#include "classes.h"
#include <stdbool.h>
#include <stdlib.h>

#define numOfCharacterClasses 12

/*
Global variables that are so to allow tMatrix to be declared in main
(and because of being able to do that, pass by refernece, will enable
it to be edited by being passed in as a parameter).
int states   number of total states
int start    where tmf starts
int accept   which states are the accepting states of tmf

Global variables which enable the saved/accepted/buffer characters
char savedChars   string which saves the saved/accepted characters
int endOfSavedChars  position of the last position of the string in
                     savedChars
*/
int states;
int start;
int accept;
char savedChars[100];
int endOfSavedChars = 0;

/*
Struct to represent the nodes within the tmf.
int next   the next character class to go to
bool save  represents whether to save the values a the string or not
*/
struct matrixPos{
	int next;
	bool save;
	bool exist;
};
/*
Sets all the matrixPos.exist = false so that the code can tell which structs were initialized or not
tMatrix   2-day array of structs as the tmf
*/
void setMatrixToNonExist(struct matrixPos tMatrix[][numOfCharacterClasses]){

	for (int r = 0; r < states; ++r){
		for (int c = 0; c < numOfCharacterClasses; ++c){
			tMatrix[r][c].exist = false;
		}
	}
}

/*
Converts the given file to a tmf
FILE *file   pointer to the file
tMatrix      the tmf as a 2d array of structs
*/
void fileToMatrix(FILE *file, struct matrixPos tMatrix[][numOfCharacterClasses]){

	setMatrixToNonExist(tMatrix);	

	char *ptr, buf[256];
	char *test;
	int row;
	//printf("In fileToMatrix, before loop\n");
	int col, go;
		int temp;
	bool save;
	char sOrD;

	while ((ptr = fgets(buf,256,file)) != NULL){
		test = strtok(ptr, " ");
		row = atoi(test);
		//printf("\nROW: %d ", row);
		while(test != NULL){
			test = strtok(NULL," ");
		//	printf("%s : ", test);
			if (test != NULL){
				sscanf(test, "%d/%d", &col, &go);

				sOrD = test[(strlen(test)-1)];
				if (sOrD == 10)
					sOrD = test[(strlen(test)-2)];
				if(sOrD == 100){
					save = false;
					temp = 1;
				}
				else if (sOrD == 115){
					save = true;
					temp = 0;
				}
				else{
					temp = 99;
					printf("SORD MESS UP: %d ", sOrD);
				}

				//printf("Col: %d Go: %d Save: %d ", col, go, temp); 
				if(test != NULL){
					struct matrixPos toBeAdded = { go, save , true};
					tMatrix[row][col] = toBeAdded;
					//printf("Col: %d Go: %d Save: %d ", col, go, temp);
				}
			}	
		}
	}

	for(int i = 0; i < (states); ++i){
		for(int j = 0; j < (numOfCharacterClasses); ++j){
			if (tMatrix[i][j].exist == false){
				struct matrixPos sTemp = { 99, false, true };
				tMatrix[i][j] = sTemp;
			}
		}
	}
}


/*
Reads the file enough to set the global variables (states, start, accept)
FILE *file  pointer to the file to read
*/
void setGlobalVariables(FILE *file){
	
	char *ptr, buf[256];
	char *token;
	int i = 0;
	while((ptr = fgets(buf,256,file)) != NULL){
		token = strtok(ptr, " ");
		while (token != NULL){
			++i;
			if( i==2){
				states = atoi(token);
				//printf("States: %d \n", states);
			}
			else if( i == 4){
				start = atoi(token);
				//printf("Start: %d \n", start);
			}
			else if( i == 6){
				accept = atoi(token);
				//printf("Accept: %d \n", accept);
				break;
			}
			token = strtok(NULL, " ");
		}
		if(i == 6){
			break;
		}
	}
}

/*
Prints out the tmf. Mostly for testing purposes.
tMatrix   2d array of structs representing the tmf (already fully filled)
*/
void printMatrix(struct matrixPos tMatrix[][numOfCharacterClasses]){

	for(int i = 0; i <= (numOfCharacterClasses-1); ++i){
		if ( i == 0){
			char space = ' ';
			printf("%3c", space);
		}
		printf("%5d", i);
	}
	
	char temp;
	for(int r = 0; r < states; ++r){
		printf("\n%4d", r);
		for(int c = 0; c < (numOfCharacterClasses); ++c){
			if(tMatrix[r][c].save == true){
				temp = 's';
			}
			else{
				temp = 'd';
			}
			
			printf("%4d%c", tMatrix[r][c].next, temp);
		}
	}
	printf("\n");


}


/*
Translate the char parameter from a ascii character to an int determined by
character class header.
char translate   ascii character that will be translated to int based on
                 character classes
*/
int classify (char translate){

	if (translate == 32)
		return CC_WS;
	else if (translate == 10)
		return CC_NEWLINE;
	else if ((translate >= 65 && translate <= 90) || (translate >= 97 && translate <= 122))
		return CC_ALPHA;
	else if (translate == 48)
		return CC_DIG_0;
	else if (translate >= 48 && translate <= 55)
		return CC_DIG_1_7;
	else if (translate == 56 || translate == 57)
		return CC_DIG_8_9;
	else if (translate == 47)
		return CC_CHAR_SLASH;
	else if (translate == 42)
		return CC_CHAR_STAR;
	else if ((translate == 43) || (translate == 45) || (translate == 37))
		return CC_ARITH_OP;
	else if ((translate >= 0) && (translate <= 127))
		return CC_OTHER;
	else if (translate == EOF)
		return CC_EOF;
	else
		return CC_ERROR;
}

/*
"Saves" an accepted character in savedChars
char save   character to save
*/
void saveChar(char save){

	savedChars[endOfSavedChars] = save;
	++endOfSavedChars;
}

/*
"Resets"/erases the array when there becomes a fail/rejected state
*/
void resetCharArray(){

	strcpy(savedChars, "");
	endOfSavedChars = 0;
}

/*
Prints the array of saved characters
*/
void printCharArray(){

	printf("'");
	for(int i = 0; i < endOfSavedChars; ++i){
		printf("%c", savedChars[i]);
	}
	printf("' \n");
}

/*
Scans and traverses the tmf using next character.
Handles all inputs/expectations during traversal.
char input    input character that is the next transition
tMatrix       2d array of structs representing the tmf
int curr      the current state in the tmf
*/
int scanner (char input, struct matrixPos tMatrix[][numOfCharacterClasses], int curr ){
	
	int traverse = classify(input);

	struct matrixPos this = tMatrix[curr][traverse];

	if(curr == start){
		printf("%d ", start);
	}

	printf("%d ", this.next);

	if(this.save == true){
		saveChar(input);
	}
	if(this.next == 99){
		resetCharArray();
		printf("rejected \n");
		return 98;
	}
	else if (traverse == CC_EOF){
		printf("EOF\n");
		return start;
	}
	else if (this.next == accept){
		printf("recognized ");
		printCharArray();
		resetCharArray();
		return start;
	}
	else{
		return this.next;
	}

	
}

/*
Main function which opens the file, gives it to setGlobalVariables() then
creates the tmf (2d array of structs) and sends it to fileToMatrix to be edited
correctly. Will then deal with user input sending to scanner (character by character)
which traverses the tmf. 
*/
int main(int argc, char* argv[]){


	FILE *fp;
	
	fp = fopen( argv[1], "r");
	if (fp == NULL){
		perror (argv[1]);
		exit(1);
	}

	setGlobalVariables(fp);
	struct matrixPos tMatrix[states][numOfCharacterClasses];
	fileToMatrix(fp, tMatrix);
	printMatrix(tMatrix);

	char c;
	int curr = start;	
	
	while(( c = getchar()) != EOF){

		if (curr == 98){
			if (c == ' '){
				curr = start;
			}
		}
		else{
			curr = scanner(c, tMatrix, curr);
		}

		if(c == '\n'){
			curr = scanner(EOF, tMatrix, curr);
			break;
		}

	}

	if (curr != start){
		printf("EOF");
	}
	
	
}


