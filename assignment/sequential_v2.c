#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Open given file
 */
FILE* open_file(char*);

/* Count words in text
 */
typedef struct Words{
	char* word;
	int frequency;
} Words;

//Count words
int count_words(Words*, FILE*, FILE*);
void check_unnecessary_hyphens(char*, int*);
void remove_character(char*, int*, int);

//Evaluate words
void already_occuring_word(Words*, int*, char*);


int main(int argc, char** argv){
	//Define two pointers on the file given as terminal parameter
	//FUTURE IDEA: Opening multiple files
	FILE* first_fp = open_file(argv[1]);
	FILE* second_fp = open_file(argv[1]);
	
	//Itearate through the file to count all words
	Words* words = NULL;
	int word_count = count_words(words, first_fp, second_fp);

	fclose(first_fp);
	fclose(second_fp);
}

FILE* open_file(char* file_name){
	FILE* fp = fopen(file_name, "r");

	if(fp == NULL){
		printf("Can't open the \"%s\" file, try again!\n", file_name);
		exit(0);
	}
	return fp;
}

int count_words(Words* words, FILE* fp_1, FILE* fp_2){
	int word_counter = 0;
	char* current_word = NULL;
	int current_word_length;

	char fp_1_ch, fp_2_ch;
	do{
		current_word_length=0;
		current_word = malloc(current_word_length * sizeof(char));
		
		fp_1_ch = fgetc(fp_1);
		if(fp_1_ch == '\n' || fp_1_ch == 32 || fp_1_ch == EOF){
			do{
				fp_2_ch = fgetc(fp_2);
				//Save the character if it is an alphabet or a hyphen
				if(isalpha(fp_2_ch) != 0 || fp_2_ch == 45){
					current_word = realloc(current_word, (current_word_length+1) * sizeof(char));
					current_word[current_word_length++] = tolower(fp_2_ch);
				}
			}while(fp_2_ch != '\n' && fp_2_ch != 32 && fp_2_ch != EOF);

			current_word = realloc(current_word, (++current_word_length) * sizeof(char));
			current_word[current_word_length-1] = '\0';
			check_unnecessary_hyphens(current_word, &current_word_length);
			for(int i=0; i<word_counter; i++){
				printf("%s\n", words[i].word);
			}

			already_occuring_word(words, &word_counter, current_word);
			
			printf("%d\n", word_counter);
			free(current_word);
		}
	}while(fp_1_ch != EOF);

	return word_counter;
}

void check_unnecessary_hyphens(char* word, int* size){
	for(int i=0; word[i] != '\0'; i++){
		if(word[i] == 45 && (isalpha(word[i-1]) == 0 || isalpha(word[i+1]) == 0)){
			remove_character(word, size, i);
		}
	}
}

void remove_character(char* word, int* size, int index){
	for(int i=index; i< *size; i++){
		word[i] = word[i+1];
	}
	word = realloc(word, --(*size) * sizeof(char));
}

void already_occuring_word(Words* words, int* word_counter, char* current_word){
	int occuring = 0;
	for(int i=0; i< *word_counter; i++){
		if(strcmp(words[i].word, current_word) == 0){
			occuring = 1;
			words[i].frequency++;
			break;
		}
	}
	if(occuring == 0){
		printf("word counter: %d\n", (*word_counter)+1);
		words = realloc(words, ++(*word_counter) * sizeof(Words));
		strcpy(words[(*word_counter)-1].word, current_word);
		printf("%s\n", words[(*word_counter)-1].word);
	}
}
