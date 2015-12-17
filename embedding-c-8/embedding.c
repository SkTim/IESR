//lhy
//2015.10

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>

#define MAX_NUMBER_SIZE 100
#define MAX_LINE_SIZE 10000000
#define MAX_VECTOR_LENGTH 200000

typedef float real;

int num_threads = 10, lines_num = 0, negative1 = 4, negative2 = 4;
long long vocab_size, layer1_size = 1000, pmi_size, esa_size;
real *syn0, *syn1, *syn2, f;
char lines[500000000][80], train_file[40], output_file[40];

void ReadWord(char *word, FILE *fin) {
	int a = 0, ch;
	while (!feof(fin)) {
		ch = fgetc(fin);
		if ((ch == ' ') || (ch == ':') || (ch == '\n')) {
			break;
		}
		word[a] = ch;
		a++;
		if (a >= MAX_NUMBER_SIZE - 1) a--;
	}
	word[a] = 0;
}

void InitVectors() {
	long long a, b;
	unsigned long long next_random = 1;
	a = posix_memalign((void **)&syn0, 128, (long long)vocab_size * layer1_size * sizeof(real));
	if (syn0 == NULL) {printf("Memory allocation failed\n"); exit(1);}
	a = posix_memalign((void **)&syn1, 128, (long long)pmi_size * layer1_size * sizeof(real));
    if (syn1 == NULL) {printf("Memory allocation failed\n"); exit(1);}
    for (a = 0; a < pmi_size; a++) for (b = 0; b < layer1_size; b++) {
		//next_random = next_random * (unsigned long long)25214903917 + 11;
    	syn1[a * layer1_size + b] = 0;
	}
    a = posix_memalign((void **)&syn2, 128, (long long)esa_size * layer1_size * sizeof(real));
    if (syn2 == NULL) {printf("Memory allocation failed\n"); exit(1);}
    for (a = 0; a < esa_size; a++) for (b = 0; b < layer1_size; b++) {
		//next_random = next_random * (unsigned long long)25214903917 + 11;
    	syn2[a * layer1_size + b] = 0;
	}
    for (a = 0; a < vocab_size; a++) for (b = 0; b < layer1_size; b++) {
    	next_random = next_random * (unsigned long long)25214903917 + 11;
    	syn0[a * layer1_size + b] = (((next_random & 0xFFFF) / (real)65536) - 0.5) / ((float)(layer1_size));
		//if (!(syn0[a * layer1_size + b] < 10)) printf("%f\n", syn0[a * layer1_size + b]);
	}
}

void GetMatrix(FILE *fin) {
	char ch[20] = {}, ch1[20] = {}, ch2[20] = {};
	int a = 0, b = 0, i;
	char line[100];
	while (!feof(fin)) {
		fgets(line, 100, fin);
		strcpy(lines[lines_num], line);
		lines_num++;
	}
	for (i = 0; i < 20; i++) {
		if (lines[0][i] == '\n') break;
		if (lines[0][i] == ' ') {
			a += 1;
			b = i + 1;
			continue;
		}
		if (a == 0) ch[i] = lines[0][i];
		if (a == 1) ch1[i - b] = lines[0][i];
		if (a == 2) ch2[i - b] = lines[0][i];
	}
	vocab_size = atoi(ch);
	pmi_size = atoi(ch1);
	esa_size = atoi(ch2);
	InitVectors();
	printf("%lld %lld %lld\n", vocab_size, pmi_size, esa_size);
}

void COMF(int iter_num, real rate) {
	char ch[20], ch1[20], ch2[20];
	int a = 0, b = 0, matrix_id = 0, l, i, j, line_id, column_id, iter = 0, l1, l2;
	real value, g, test;
	real *neu1e = (real *)calloc(layer1_size, sizeof(real));
	unsigned long long next_random = 1;
	while (iter < iter_num) {
		for (l = 1; l < lines_num; l++) {
			char s1[10] = {}, s2[10] = {}, v[10] = {};
			if (strcmp(lines[l], "PPMI:\n") == 0) {
				matrix_id = 0;
				continue;
			}
			if (strcmp(lines[l], "ESA:\n") == 0) {
				matrix_id = 1;
				continue;
			}
			if (strcmp(lines[l], "\n") == 0) break;
			a = 0;
			b = 0;
			for (i = 0; i < strlen(lines[l]); i++) {
				if (lines[l][i] == '\n') break;
				if (lines[l][i] == ' ') {
					a += 1;
					b = i + 1;
					continue;
				}
				if (a == 0) s1[i] = lines[l][i];
				if (a == 1) s2[i - b] = lines[l][i];
				if (a == 2) v[i - b] = lines[l][i];
			}
			line_id = atoi(s1);
			column_id = atoi(s2);
			value = atof(v);
			if (value > 100) printf("%f\n",value);
			//real f1 = 0;
			l1 = line_id * layer1_size;
			l2 = column_id * layer1_size;
			for (i = 0; i < layer1_size; i++) neu1e[i] = 0;
			if (matrix_id == 0) for (i = 0; i < negative1 + 1; i++) {
				f = 0;
				if (i != 0) {
					next_random = (int)((next_random * (unsigned long long)25214903917 + 11) % pmi_size);
					column_id = next_random;
					value = 0;
				}
				for (j = 0; j < layer1_size; j++) {
					//f1 = f;
					f += syn0[j + l1] * syn1[j + l2];
					//if (f != f) {
					//	printf("PPMI: l = %d, syn0 = %f, syn1 = %f, f1 = %f\n", l, syn0[line_id * layer1_size + j], syn1[column_id * layer1_size + j], f1);
					//	return;
					//}
				}
				g = value - f;
				//if (g > 10) printf("PPMI: value = %f f = %f\n", value, f);
				//if (l == 2) printf("g = %f\n",g);
				for (j = 0; j < layer1_size; j++) {
					//test = syn0[line_id * layer1_size + j];
					//syn0[line_id * layer1_size + j] += rate * g * syn1[column_id * layer1_size + j];
					neu1e[j] += rate * g * syn1[j + l2];
					syn1[j + l2] += rate * g * syn0[j + l1];
					//if (syn0[line_id * layer1_size + j] - test > 10) {
					//	printf("PPMI: test = %f g = %f syn1 = %f\n", test, g, syn1[column_id * layer1_size + j]);
					//	return;
					//}
					//if (syn1[column_id * layer1_size + j] != syn1[column_id * layer1_size + j]) {
					//	printf("PMI: test = %f, g = %f, value = %f, f = %f\n",test, g, value, f);
					//	return;
					//}
				}
			}
			if (matrix_id == 1) for (i = 0 ; i < negative2 + 1; i++) {
				f = 0;
				if (i != 0) {
					next_random = (int)((next_random * (unsigned long long)25214903917 + 11) % esa_size);
					column_id = next_random;
					value = 0;
				}
				for (j = 0; j < layer1_size; j++) {
					//f1 = f;
					f += syn0[j + l1] * syn2[j + l2];
					//if (f != f) {
					//	printf("ESA: l = %d, syn0 = %f, syn2 = %f, f1 = %f\n", l, syn0[line_id * layer1_size + j], syn2[column_id * layer1_size + j], f1);
					//	return;
					//}
				}
				g = value - f;
				//if (g > 10) printf("ESA: value = %f f = %f\n", value, f);
				for (j = 0; j < layer1_size; j++) {
					//test = syn0[line_id * layer1_size + j];
					//syn0[line_id * layer1_size + j] += rate * g * syn2[column_id * layer1_size + j];
					neu1e[j] += rate * g * syn2[j + l2];
					syn2[j + l2] += rate * g * syn0[j + l1];
					//if (syn0[line_id * layer1_size + j] - test > 10) {
					//	printf("ESA: test = %f g = %f syn2 = %f\n", test, g, syn2[column_id * layer1_size + j]);
					//	return;
					//}
					//if (syn2[column_id * layer1_size + j] != syn2[column_id * layer1_size + j]) {
					//	printf("ESA: test = %f, g = %f, value = %f, f = %f\n",test, g, value, f);
					//	return;
					//}
				}
			}
			for (i = 0; i < layer1_size; i++) syn0[i + l1] += rate * neu1e[i];
		}
		rate *= 0.8;
		/*
		for (i = 0; i < layer1_size; i++) {
			printf("%f ",syn1[i]);
			printf("\n");
		}
		*/
		printf("Iter %d\n", iter);
		iter++;
	}
	free(neu1e);
}

void TrainModel(int iter_num, real rate) {
	int a, b;
	FILE *fo;
	printf("COMF Begin\n");
	COMF(iter_num, rate);
	printf("COMF OK\n");
	fo = fopen(output_file, "wb");
	fprintf(fo, "%lld %lld\n", vocab_size, layer1_size);
    for (a = 0; a < vocab_size; a++) {
    	for (b = 0; b < layer1_size; b++) fprintf(fo, "%f ", syn0[a * layer1_size + b]);
    	fprintf(fo, "\n");
    }
}

void main(int argc, char **argv) {
	printf("Main Begin\n");
	FILE *fp;
	strcpy(train_file, "../data/infoMatrix");
	strcpy(output_file, "../data/wordVectors");
	fp = fopen(train_file,"r");
	layer1_size = 2000;
	printf("GetMatrix Begin\n");
	GetMatrix(fp);
	printf("TrainModel Begin\n");
	TrainModel(20, 0.2);
}
