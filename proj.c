#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

#define SIDEONE "fox"
#define SIDETWO "cnn"



typedef struct wordfreq {
	char * word;
	int freqyes;
	int freqnot;
	struct wordfreq* next;
} wordfreq;

long double P_yes = 0.5;
int total_words_yes = 0;
int total_words_not = 0;
wordfreq* freqs = NULL;

void print_freq(wordfreq* freq);
void print_freqs(wordfreq* freqs);


void test(int x)
{
	printf("testing: %d\n", x);
}

wordfreq* getwordfreq(char* word, wordfreq* freq_list)
{
	wordfreq* ret = NULL;
	while(freq_list){
		if(!strcmp(word, freq_list->word)){
			ret = freq_list;
			break;
		}
		freq_list = freq_list->next;
	}
	return ret;
}

long double P_word_ifside(char* word, wordfreq* freq_list, int ifyes)
{
	long double prob=0;

	wordfreq* i_freq = getwordfreq(word, freq_list);
	long double i_prob = 0;
	long double total = 0;
	if(ifyes){
			if(i_freq) i_prob = i_freq->freqyes;
			total = total_words_yes;
		}
			
	else{
			if(i_freq) i_prob = i_freq->freqnot;
			total = total_words_not;
		}
		
	//prob = (long double)(i_prob)/(long double)(total);
	prob = (i_prob+1)/((total + total_words_not + total_words_yes));
	
	return prob;
}


void word_occurence(char* word, int sideyes, wordfreq* freq_list)
{
	wordfreq* wfreq;
	wfreq = getwordfreq(word, freq_list);
	
	if(!wfreq){
		wfreq = calloc(1, sizeof(wordfreq));
		if(freq_list){
			while(freq_list->next){
				freq_list = freq_list->next;
			}
		
			freq_list->next = wfreq;
		}
		else{
			freqs = wfreq;
		}
	}
	wfreq->word = strdup(word);


	if(sideyes){
		wfreq->freqyes++;
		total_words_yes++;
	}
	else{
		wfreq->freqnot++;
		total_words_not++;
	}
	//print_freq(wfreq);
}

void train_from_string(char *article, int sideyes)
{
	char wrd[100];
	char *c = article;
	int i;
	int word;
	while(*c != '\0'){
		
		i = 0;
		word = 0;
		
		while(isalnum(*c)){
			
			word = 1;
			wrd[i] = tolower(*c);
			c++;
			i++;
		}
		if(word){

			wrd[i] = '\0';
			if(strcmp(wrd, SIDEONE) && strcmp(wrd, SIDETWO)){
				word_occurence(wrd, sideyes, freqs);
			}
		}
		if(c!= '\0')
			c++;
	}
}


// train from a file with a specific setup: 
		// *!* side
		//	article text
void train_from_file(FILE* f)
{
	int side;
	char sidebuf[100];
	char buffer[5000];


	while(fgets(buffer, 5000, f)){

		if(sscanf(buffer, "*!* %s", sidebuf)){
			
			if(!strcmp(sidebuf, SIDEONE)){
				side = 1;
			}
			else if(!strcmp(sidebuf, SIDETWO)){
				side = 0;
			}
			else{
			 printf("error reading side\n");
			 exit(1);
			}	
		}
		else{		
			train_from_string(buffer, side);
		}
	}
	
}

void print_freq(wordfreq* freq)
{
	printf("Word: %s, freqs1: %d, freqs2: %d\n", freq->word, freq->freqyes, freq->freqnot);
}

void print_freqs(wordfreq* freqss)
{
	while(freqss){
		print_freq(freqss);
		freqss = freqss->next;
	}
}

long double side_ratio(FILE* f)
{
	char buffer[10000];
	long double probone = 0.5;
	long double probtwo = 0.5;
	long double ratio = 1;

	while(fgets(buffer, 10000, f)){
		char wrd[100];
		char *c = buffer;
		int i;
		int word;
		while(*c != '\0'){
		
			i = 0;
				word = 0;
			
			while(isalnum(*c)){
				
				word = 1;
				wrd[i] = tolower(*c);
				c++;
				i++;
			}
			if(word){

				wrd[i] = '\0';
				
				long double p1 = P_word_ifside(wrd, freqs, 1);
				long double p2 = P_word_ifside(wrd, freqs, 0);
				
				//if(fabsl(1 - p1/p2) >= .2){
					 ratio *= (p1/p2);
					// printf("word: %s, p1: %Lf, p2: %Lf ra: %Lf\n", wrd, p1, p2, p1/p2);
				//	printf("ratio: %Lf\n", ratio);
			//}	
				//probone *= p1;
				//probtwo *= p2;

				
				//if(p1) probone *= p1;
				//if(p2) probtwo *= p2;
				//printf("ratio %Lf\n", ratio);
				
			}
			if(c!= '\0')
				c++;
			}
		}

	//printf("prob one: %Lf prob two: %Lf ratio: %Lf\n", probone, probtwo, probone/probtwo);
	return ratio;
}

int main(int argc, char** argv)
{
	FILE* trainfile;
	FILE* guessfile;

	for(int i = 0; i < argc; i++){
		if(!strcmp(argv[i], "-t")){
			trainfile = fopen(argv[i+1], "r");
			
		}
		if(!strcmp(argv[i], "-g")){
			guessfile = fopen(argv[i+1], "r");
			
		}
	}
	if(trainfile){
		train_from_file(trainfile);
		if(guessfile){
			long double ratio = side_ratio(guessfile);
			if(ratio > 1){
				printf(SIDEONE);
			}
			else if(ratio < 1){
				printf(SIDETWO);
			}
			else printf("Cannot be determined");
			printf("\n");
			
		}
	}
	//print_freqs(freqs);




	return 0;
}
