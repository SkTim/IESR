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

int num_threads = 24, lines_num = 0, negative1 = 1, negative2 = 0, group, iter_num;
long long vocab_size, layer1_size = 1000, pmi_size, esa_size;
real *syn0, *syn1, *syn2, f, rate, *rate_table;
char lines[500000000][80], train_file[40], output_file1[40], output_file2[40];

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
	srand((unsigned)time(NULL));
	a = posix_memalign((void **)&syn0, 128, (long long)vocab_size * layer1_size * sizeof(real));
	if (syn0 == NULL) {printf("Memory allocation failed\n"); exit(1);}
	a = posix_memalign((void **)&syn1, 128, (long long)pmi_size * layer1_size * sizeof(real));
    if (syn1 == NULL) {printf("Memory allocation failed\n"); exit(1);}
    for (a = 0; a < pmi_size; a++) for (b = 0; b < layer1_size; b++) {
		next_random = next_random * (unsigned long long)25214903917 + 11;
    	syn1[a * layer1_size + b] = (((next_random & 0xFFFF) / (real)65536) - 0.5) / ((float)(layer1_size));
	}
    a = posix_memalign((void **)&syn2, 128, (long long)esa_size * layer1_size * sizeof(real));
    if (syn2 == NULL) {printf("Memory allocation failed\n"); exit(1);}
    for (a = 0; a < esa_size; a++) for (b = 0; b < layer1_size; b++) {
		next_random = next_random * (unsigned long long)25214903917 + 11;
    	syn2[a * layer1_size + b] = (((next_random & 0xFFFF) / (real)65536) - 0.5) / ((float)(layer1_size));
	}
    for (a = 0; a < vocab_size; a++) for (b = 0; b < layer1_size; b++) {
    	next_random = next_random * (unsigned long long)25214903917 + 11;
    	syn0[a * layer1_size + b] = 10 * (((next_random & 0xFFFF) / (real)65536) - 0.5) / ((float)(layer1_size));
	}
	rate_table = (real *)calloc(num_threads, sizeof(real));
	for (a = 0; a < num_threads; a++) rate_table[a] = rate;
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

void *COMF(void *id) {
	int a = 0, b = 0, matrix_id = 0, l, i, j, line_id, column_id, iter = 0, l1, l2;
	real value, g, test;
	real *neu1e = (real *)calloc(layer1_size, sizeof(real));
	unsigned long long next_random = (long long)id;
	int t_id = (int)id, start = group * (int)id + 1, end;
	if ((int)id == num_threads - 1) end = lines_num;
	else end = start + group;
	real line_num = (real)(end - start);
	printf("COMF %d Begin, start = %d, end = %d\n", t_id, start, end);
	while (iter < iter_num) {
		for (l = start; l < end; l++) {
			if ((l - start + 1) % 100 == 0 && rate_table[t_id] > rate / 1000) {
				rate_table[t_id] *= (line_num - (real)(l - start + 1)) / line_num;
				if (rate_table[t_id] < rate / 1000) rate_table[t_id] = rate / 1000;
			}
			char s1[10] = {}, s2[10] = {}, v[10] = {};
			if (lines[l][0] == 'P') matrix_id = 0;
			if (lines[l][0] == 'E') matrix_id = 1;
			a = 0;
			b = 0;
			for (i = 1; i < strlen(lines[l]); i++) {
				if (lines[l][i] == '\n') break;
				if (lines[l][i] == ' ') {
					a += 1;
					b = i + 1;
					continue;
				}
				if (a == 0) s1[i - 1] = lines[l][i];
				if (a == 1) s2[i - b] = lines[l][i];
				if (a == 2) v[i - b] = lines[l][i];
			}
			line_id = atoi(s1);
			column_id = atoi(s2);
			value = atof(v);
			//if (value == 0) continue;
			//real f1 = 0;
			l1 = line_id * layer1_size;
			l2 = column_id * layer1_size;
			for (i = 0; i < layer1_size; i++) neu1e[i] = 0;
			if (matrix_id == 0) for (i = 0; i < negative1 + 1; i++) {
				f = 0;
				if (i != 0) {
					if (value == 0) break;
					//srand((unsigned)time(NULL));
					next_random = next_random * (unsigned long long)25214903917 + 11;
					column_id = (next_random >> 16) % pmi_size;
					//column_id = (int)(rand() % pmi_size);
					l2 = column_id * layer1_size;
					value = 0;
				}
				for (j = 0; j < layer1_size; j++) {
					f += syn0[j + l1] * syn0[j + l2];
				}
				g = value - f;
				for (j = 0; j < layer1_size; j++) {
					neu1e[j] += rate_table[t_id] * g * syn0[j + l2];
					syn0[j + l2] += rate_table[t_id] * g * syn0[j + l1];
					if (syn0[j + l2] < 0) syn0[j + l2] = 0;
				}
			}
			if (matrix_id == 1) for (i = 0 ; i < negative2 + 1; i++) {
				f = 0;
				if (i != 0) {
					if (value == 0) break;
					//srand((unsigned)time(NULL));
					next_random = next_random * (unsigned long long)25214903917 + 11;
					column_id = (next_random >> 16) % esa_size;
					//column_id = (int)(rand() % esa_size);
					l2 = column_id * layer1_size;
					value = 0;
				}
				for (j = 0; j < layer1_size; j++) {
					f += syn0[j + l1] * syn2[j + l2];
				}
				g = value - f;
				for (j = 0; j < layer1_size; j++) {
					neu1e[j] += rate_table[t_id] * g * syn2[j + l2];
					syn2[j + l2] += rate_table[t_id] * g * syn0[j + l1];
					if (syn2[j + l2] < 0) syn2[j + l2] = 0;
				}
			}
			for (i = 0; i < layer1_size; i++) {
				syn0[i + l1] += rate_table[t_id] * neu1e[i];
				if (syn0[i + l1] < 0) syn0[i + l1] = 0;
			}
		}
		if (iter > 4) rate *= 0.8;
		printf("COMF %d Iter %d\n",(int)id, iter);
		iter++;
	}
	free(neu1e);
	pthread_exit(NULL);
}

void TrainModel() {
	int a, b;
	FILE *fo;
	group = (int)((lines_num - 1) / num_threads);
	pthread_t *pt = (pthread_t *)malloc(num_threads * sizeof(pthread_t));
	for (a = 0; a < num_threads; a++) pthread_create(&pt[a], NULL, COMF, (void *)a);
	for (a = 0; a < num_threads; a++) pthread_join(pt[a], NULL);
	fo = fopen(output_file1, "wb");
	fprintf(fo, "%lld %lld\n", vocab_size, layer1_size);
    for (a = 0; a < vocab_size; a++) {
    	for (b = 0; b < layer1_size; b++) fprintf(fo, "%f ", syn0[a * layer1_size + b]);
    	fprintf(fo, "\n");
    }
	fclose(fo);
	fo = fopen(output_file2,"wb");
	fprintf(fo, "%lld %lld\n", esa_size, layer1_size);
	for (a = 0; a < esa_size; a++) {
		for (b = 0; b < layer1_size; b++) fprintf(fo, "%f ", syn2[a * layer1_size + b]);
		fprintf(fo, "\n");
	}
	fclose(fo);
}

void main(int argc, char **argv) {
	printf("Main Begin\n");
	FILE *fp;
	strcpy(train_file, "../data/infoMatrix_1");
	strcpy(output_file1, "../data/wordVectors_2");
	strcpy(output_file2, "../data/articleVectors_2");
	fp = fopen(train_file,"r");
	layer1_size = 1000;
	rate = 0.005;
	iter_num = 4;
	printf("GetMatrix Begin\n");
	GetMatrix(fp);
	printf("TrainModel Begin\n");
	TrainModel();
}

