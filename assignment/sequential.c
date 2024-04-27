#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Open the file stream
 */
FILE* open_file(char*);

/* Determine the number of characters in the file
 */
int get_file_size(FILE*);

/* Read the whole file
 * Also removes punctuations
 */
void read_words_from_text(FILE*, int, char***, int*);

/* Handle special cases
 * x handle empty input
 * x remove punctuations
 * x remove numeric values
 * x convert all to lowercase
 * x remove stop words
 */
	/* Remove numeric values
	 */
	void remove_numeric_values(char**, int*);

	/* Convert words to lowercase
	 */
	void convert_to_lowercase(char**, int);

	/* Remove all stop words. Also remove a stop word if found
	 */
	void read_stop_words(char**, int*);

	//Remove word on given index and shift remaining words (for multiple use)
	void remove_word(char**, int, int);

/* Evaluate the text by counting words
 */
struct Words* evaluate_words(char**, int, int*);
struct Words{
	char* word;
	int frequency;
};
int get_non_duplicate_words(struct Words*, char**, int);
//Count_word function is going to be parallelized
int count_word(char*, char**, int);

/* Print the text
 */
void print_words(struct Words*, int);

/* Free dynamically allocated array
 */
void free_dynamic_array(char**, int);


int main(int argc, char** argv){
	//Open the given file
	char* file_name = argv[argc-1];
	FILE* fp = open_file(file_name);

	//Determine file size
	int file_char_count = get_file_size(fp);

	//Get all words from the text file
	char** words_in_text = NULL;
	int word_count = 0;
	read_words_from_text(fp,  file_char_count, &words_in_text, &word_count);

	//Handle special cases
	remove_numeric_values(words_in_text, &word_count);
	convert_to_lowercase(words_in_text, word_count);
	read_stop_words(words_in_text, &word_count);

	//Count words and determine frequency
	int struct_count=0;
	struct Words* words = evaluate_words(words_in_text, word_count, &struct_count);
	
	//Print the text
	print_words(words, struct_count);


	//Close file stream
	fclose(fp);
	
	//Free allocated memory segments
	free_dynamic_array(words_in_text, word_count);
	free(words_in_text);

	return 0;
}

FILE* open_file(char* file_name){
	FILE* fp = fopen(file_name, "r");
	if(fp==NULL){
		printf("The file could not be opened. The program now exits!\n");
		exit(0);
	}else{
		printf("File opened successfully!\n");
	}
	
	return fp;
}

int get_file_size(FILE* fp){
	//Set file position to the end
	fseek(fp, 0, SEEK_END);
	
	int char_count = ftell(fp);
	return char_count;
}

void read_words_from_text(FILE* fp, int size, char*** words, int* word_count){
	//Set file position to the beginning
	fseek(fp, 0, SEEK_SET);
	
	//Store the whole text file
	char full_text[size+1];
	fread(full_text, sizeof(char), size, fp);
	full_text[size] = '\0';

	//Define a copy of the array with the text
	char full_text_copy[size+1];
	strcpy(full_text_copy, full_text);

	//Count all delimeter separated content in the text
	char* delimeters = ".?!\n,;:#$%&()[]{} ";
	char* token = strtok(full_text, delimeters);
	while(token != NULL){
		(*word_count)++;
		token = strtok(NULL, delimeters);
	}
	
	//Allocate the words array
	*words = (char**)malloc((*word_count) * sizeof(char*));

	//Store all tokens in the words array AND remove punctuations by the delimeter
	token = strtok(full_text_copy, delimeters);
	int current_token = 0;
	while(token != NULL){
		(*words)[current_token] = (char*)malloc(sizeof(char) * (strlen(token)+1));
		strcpy((*words)[current_token], token);
		(*words)[current_token][strlen(token)] = '\0';
		token = strtok(NULL, delimeters);
		current_token++;
	}
}

void remove_numeric_values(char** words, int* size){
	for(int i=0; i< *size; i++){
		char* current_word = words[i];
		for(int j=0; current_word[j] != '\0'; j++){
			if(isdigit(current_word[j])){
				remove_word(words, --(*size), i--);
				break;
			}
		}
	}
}

void convert_to_lowercase(char** words, int size){
	for(int i=0; i < size; i++){
		char* current_word = words[i];
		for(int j=0; current_word[j] != '\0'; j++){
			current_word[j] = tolower(current_word[j]);
		}
	}
}

void read_stop_words(char** test_words, int* test_word_count){
	FILE* fp = open_file("stopwords.txt");
	
	int file_char_count = get_file_size(fp);
	
	char** stop_words = NULL;
	int stop_word_count = 0;
	read_words_from_text(fp,  file_char_count, &stop_words, &stop_word_count);
	
	for(int i=0; i< *test_word_count; i++){
		for(int j=0; j<stop_word_count; j++){
			if(strcmp(test_words[i], stop_words[j]) == 0){
				remove_word(test_words, --(*test_word_count), i--);
				break;
			}	
		}
	}	
}

void remove_word(char** words, int word_count, int index){
	for(int i=index; i< word_count; i++){
		strcpy(words[i], words[i+1]);
	}
	*words = realloc(*words, word_count * sizeof(char*));
}

struct Words* evaluate_words(char** duplicate_words, int duplicate_word_count, int* non_dupl_count){
	struct Words* words = (struct Words*) malloc(duplicate_word_count * sizeof(struct Words));
	
	*non_dupl_count = get_non_duplicate_words(words, duplicate_words, duplicate_word_count);
	
	for(int i=0; i< *non_dupl_count; i++){
		char* word = words[i].word;
		words[i].frequency = count_word(word, duplicate_words, duplicate_word_count);
	}

	return words;
}

int get_non_duplicate_words(struct Words *words, char** duplicate_words, int duplicate_size){
	int non_duplicate_index = 0;
	for(int i=0; i<duplicate_size; i++){
		char* current_word = duplicate_words[i];
		int is_duplicate=0;
		for(int j=0; j<non_duplicate_index; j++){
			if(strcmp(words[j].word, current_word) == 0){
				is_duplicate=1;
			}
		}
		if(is_duplicate==0){
			words[non_duplicate_index].word = malloc(strlen(current_word) * sizeof(char));
			strcpy(words[non_duplicate_index].word, current_word);
			non_duplicate_index++;
		}
	}
	return non_duplicate_index;
}

int count_word(char* word, char** duplicate_words, int duplicate_count){
	int frequency=0;
	for(int i=0; i<duplicate_count; i++){
		if(strcmp(word, duplicate_words[i]) == 0){
			frequency++;
		}	
	}
	return frequency;
}


void print_words(struct Words* words, int size){
	for(int i=0; i < size; i++){
		printf("%s - %d\n", words[i].word, words[i].frequency);
	}
}

void free_dynamic_array(char** array, int size){
	for(int i=0; i< size; i++){
		free(array[i]);
	}
}
