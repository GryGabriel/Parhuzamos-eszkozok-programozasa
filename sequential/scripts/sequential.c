#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>


/* Open given file
 */
FILE* open_file(char*);

/* Struct type for storing words
 */
typedef struct word_t{
	char* word;
	int frequency;
} word_t;

/* Get words in text
 */
word_t* get_words(char*, size_t*);

/* Check the word for any unnecessary punctuation
 */
int validate_word(char*);

void remove_character(char*, int, int);

char** get_stopwords(char*, size_t*);

int is_stopword(char**, int, char*);

int is_duplicate(word_t*, int, char*);

int main(int argc, char** argv){
	char* file_name = argv[1];
	FILE* fp = open_file(file_name);
	
	fseek(fp, 0, SEEK_END);
	size_t file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char* input = malloc((file_size + 1) * sizeof(char));
	fread(input, sizeof(char), file_size, fp);
	input[file_size] = '\0';

	fclose(fp);
	
	/* Sequential Computation
	 */
	clock_t sequential_clock_start = clock();
	size_t word_count = 0;
	word_t* words = get_words(input, &word_count);

	free(input);
	for(int i=0; i<word_count; i++){
		printf("%s - %d\n", words[i].word, words[i].frequency);
		free(words[i].word);		
	}
	free(words);

	clock_t sequential_clock_end = clock();

	double cpu_time_used = ((double)(sequential_clock_end - sequential_clock_start)) / CLOCKS_PER_SEC;
	printf("Sequential word counting took %f seconds to compute!\n", cpu_time_used);

	return 0;
}

FILE* open_file(char* file_name){
	char file_path[100] = "./txts/";
	strcat(file_path, file_name);
	file_path[strlen(file_path)] = '\0';
	
	FILE* fp = fopen(file_path, "r");

	if(fp == NULL){
		printf("Can't open file %s!\n", file_path);
		perror("");
		exit(0);
	}
	return fp;
}

word_t* get_words(char* text, size_t* word_count){
	word_t* words = malloc((*word_count) * sizeof(word_t*));

	size_t stopword_count = 0;
	char** stopwords = get_stopwords("stopwords.txt", &stopword_count);
	
	char* word_start = text;

	while(*text != '\0'){
		//In this order: space, newline, carriage return
		if(*text == 32 || *text == 10 || *text == 13){
			//Initialize current word
			size_t current_length = text - word_start;
			char* current_word = malloc((current_length+1) * sizeof(char));
			
			strncpy(current_word, word_start, current_length);
			current_word[current_length] = '\0';
			
			word_start = text+1;

			//Validate current word
			if(validate_word(current_word)){
				if(!is_stopword(stopwords, stopword_count, current_word) && !is_duplicate(words, *word_count, current_word)){
					current_length = strlen(current_word);
					(*word_count)++;
					words = realloc(words, (*word_count) * sizeof(word_t));
					words[(*word_count)-1].word = malloc((current_length+1) * sizeof(char));
					words[(*word_count)-1].word[current_length] = '\0';
					strcpy(words[(*word_count)-1].word, current_word);
					words[(*word_count)-1].frequency = 1;
					free(current_word);
				}
			}
		}
		*text++;
	}

	return words;
}

int validate_word(char* word){
	//Remove unnecessary hyphens - should remain only if connecting two words
	//Remove any other punctuations
	//Remove numeric values
	//Remove stop word

	size_t l = strlen(word);
	for(int i=0; i<l; i++){
		//Convert word to lowercase
		word[i] = tolower(word[i]);
		
		//Remove characters in the following order: digits, spaces, newlines, punctuations(hyphen only if not connecting two words giving another meaning to them)
		if(isdigit(word[i]) != 0 || word[i] == ' ' || word[i] == '\n' || (ispunct(word[i]) != 0 && word[i] != '-') || (word[i] == '-' && (isalpha(word[i-1]) == 0  || isalpha(word[i+1]) == 0)) ){
			remove_character(word, i--, l);
		}
	}
	l = strlen(word);
	
	if(l == 0){
		return 0;
	}
	return 1;
}

void remove_character(char* w, int index, int l){
	for(int i=index; i<l; i++){
		w[i] = w[i+1];
	}
	w[l-1] = '\0';
}

char** get_stopwords(char* file_name, size_t* word_count){
	FILE* fp = open_file(file_name);
	
	fseek(fp, 0, SEEK_END);
	size_t file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char* input = malloc(file_size+1 * sizeof(char));
	fread(input, sizeof(char), file_size, fp);
	input[file_size] = '\0';

	char** words = malloc(sizeof(char*));
	
	char* token = strtok(input, "\n");
	while(token != NULL){
		int length = strlen(token)+1;

		words = realloc(words, ((*word_count)+1) * sizeof(char*));
		words[*word_count] = malloc(length * sizeof(char));
		strcpy(words[*word_count], token);
		words[*word_count][length] = '\0';
		(*word_count)++;
		
		token = strtok(NULL, "\n");

	}

	fclose(fp);
	free(input);

	return words;
}

int is_stopword(char** stopwords, int stopword_count, char* word){
	for(int i=0; i < stopword_count; i++){
		if(strcmp(stopwords[i], word) == 0){
			return 1;
		}
	}
	return 0;
}

int is_duplicate(word_t* words, int count, char* current_word){
	for(int i=0; i<count; i++){
		if(strcmp(words[i].word, current_word) == 0){
			words[i].frequency++;
			return 1;
		}
	}

	return 0;
}
