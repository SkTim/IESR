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

int num_threads = 8, lines_num = 0, negative1 = 15, negative2 = 18, group, iter_num;
long long vocab_size, layer1_size = 1000, pmi_size, esa_size;
real rho = 0.95, eta = 1e-6, *syn0, *syn1, *syn2, f, rate, *rate_table1, *rate_table2, *s_vocab, *s_article, *v_vocab, *v_article;
char lines[500000000][80], vocab_lines[1000000][40], article_lines[200000][40];
char train_file[40], vocab_file[40], article_file[40], output_file1[40], output_file2[40];
int *vocab, *sorted_vocab, *article, *sorted_article, *vocab_table, *article_table;
const int table_size = 1e8;

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

void InitUnigramTable() {
    int a, i;
    double train_words_pow = 0, train_articles_pow = 0;
    double d1, d2, power = 0.75;
    vocab_table = (int *)malloc(table_size * sizeof(int));
    article_table = (int *)malloc(table_size * sizeof(int));
    for (a = 0; a < vocab_size; a++) train_words_pow += pow(vocab[a], power);
    for (a = 0; a < vocab_size; a++) train_articles_pow += pow(article[a], power);
    i = 0;
    d1 = pow(vocab[sorted_vocab[i]], power) / train_words_pow;
    for (a = 0; a < table_size; a++) {
        vocab_table[a] = sorted_vocab[i];
        if (a / (double)table_size > d1) {
            i++;
            d1 += pow(vocab[sorted_vocab[i]], power) / train_words_pow;
        }
        if (i >= vocab_size) i = vocab_size - 1;
    }
    i = 0;
    d2 = pow(article[sorted_article[i]], power) / train_articles_pow;
    for (a = 0; a < table_size; a++) {
        article_table[a] = sorted_article[i];
        if (a / (double)table_size > d2) {
            i++;
            d2 += pow(article[sorted_article[i]], power) / train_articles_pow;
        }
        if (i >= esa_size) i = esa_size - 1;
    }
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
	rate_table1 = (real *)calloc(num_threads, sizeof(real));
	rate_table2 = (real *)calloc(num_threads, sizeof(real));
	for (a = 0; a < num_threads; a++) rate_table1[a] = rate;
	for (a = 0; a < num_threads; a++) rate_table2[a] = rate;
	vocab = (int *)calloc(vocab_size, sizeof(int));
    sorted_vocab = (int *)calloc(vocab_size, sizeof(int));
	article = (int *)calloc(esa_size, sizeof(int));
    sorted_article = (int *)calloc(esa_size, sizeof(int));
	for (a = 0; a < vocab_size; a++) vocab[a] = 0;
    for (a = 0; a < vocab_size; a++) sorted_vocab[a] = 0;
	for (a = 0; a < esa_size; a++) article[a] = 0;
    for (a = 0; a < esa_size; a++) sorted_article[a] = 0;
    s_vocab = (real *)calloc(vocab_size, sizeof(real));
    v_vocab = (real *)calloc(vocab_size, sizeof(real));
    s_article = (real *)calloc(esa_size, sizeof(real));
    v_article = (real *)calloc(esa_size, sizeof(real));
    for (a = 0; a < vocab_size; a++) s_vocab[a] = 0;
    for (a = 0; a < vocab_size; a++) v_vocab[a] = 0;
    for (a = 0; a < esa_size; a++) s_article[a] = 0;
    for (a = 0; a < esa_size; a++) v_article[a] = 0;
}

void GetVocab(FILE *fin) {
    int a = 0, b = 0, n = 0, i, l, line_id;
    char line[100];
    real value;
    while(!feof(fin)) {
        fgets(line, 100, fin);
        strcpy(vocab_lines[n], line);
        n++;
    }
    for (l = 1; l < n; l++) {
        char ch[20] = {}, ch1[20] = {};
        a = 0;
        b = 0;
        for (i = 0; i < strlen(vocab_lines[l]); i++) {
            if (vocab_lines[l][i] == '\n') break;
            if (vocab_lines[l][i] == ' ') {
                a += 1;
                b = i + 1;
                continue;
            }
            if (a == 0) ch[i] = vocab_lines[l][i];
            if (a == 1) ch1[i - b] = vocab_lines[l][i];
        }
        sorted_vocab[l - 1] = atoi(ch);
        vocab[sorted_vocab[l - 1]] = atoi(ch1);
    }
}

void GetArticle(FILE *fin) {
    int a = 0, b = 0, n = 0, i, l, line_id;
    char line[100];
    real value;
    while(!feof(fin)) {
        fgets(line, 100, fin);
        strcpy(article_lines[n], line);
        n++;
    }
    for (l = 1; l < n; l++) {
        char ch[20] = {}, ch1[20] = {};
        a = 0;
        b = 0;
        for (i = 0; i < strlen(article_lines[l]); i++) {
            if (article_lines[l][i] == '\n') break;
            if (article_lines[l][i] == ' ') {
                a += 1;
                b = i + 1;
                continue;
            }
            if (a == 0) ch[i] = article_lines[l][i];
            if (a == 1) ch1[i - b] = article_lines[l][i];
        }
        sorted_article[l - 1] = atoi(ch);
        article[sorted_article[l - 1]] = atoi(ch1);
    }
}

void GetMatrix(FILE *fin) {
	char ch[20] = {}, ch1[20] = {}, ch2[20] = {};
	int a = 0, b = 0, i, l, line_id, column_id;
	char line[100];
	real value;
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
	int a = 0, b = 0, matrix_id = 0, l, i, j, line_id, column_id, iter = 0, l1, l2, p_num, e_num;
	real value, g, g1, g2, test, ppmi_num = 0, esa_num = 0, min_rate = rate / 10;
	real *neu1e = (real *)calloc(layer1_size, sizeof(real));
	unsigned long long next_random = (long long)id;
	int t_id = (int)id, start = group * t_id + 1, end, target;
	if ((int)id == num_threads - 1) end = lines_num;
	else end = start + group;
	real line_num = (real)(end - start);
	real total_num = line_num * (float)iter_num;
	printf("COMF %d Begin, start = %d, end = %d\n", t_id, start, end);
	for (l = start; l < end; l++) {
		if (lines[l][0] == 'P') ppmi_num += 1;
	}
	esa_num = line_num - ppmi_num;
	ppmi_num *= (real)iter_num;
	esa_num *= (real)iter_num;
	p_num = 0;
	e_num = 0;
	while (iter < iter_num) {
		//p_num = 0;
		//e_num = 0;
		for (l = start; l < end; l++) {
			
			/*
			if ((l - start + 1) % 100 == 0 && rate_table1[t_id] > min_rate) {
				rate_table1[t_id] = rate * (line_num - (l - start + 1)) / line_num;
				if (rate_table1[t_id] < min_rate) rate_table1[t_id] = rate / 10;
			}
			*/

			if (lines[l][0] == 'P') {
				matrix_id = 0;
				p_num += 1;
				if (p_num % 100 == 0 && rate_table1[t_id] > min_rate) {
					rate_table1[t_id] = rate * (ppmi_num - (real)p_num) / ppmi_num;
					if (rate_table1[t_id] < min_rate) rate_table1[t_id] = min_rate;
				}
			}
			if (lines[l][0] == 'E') {
				matrix_id = 1;
				e_num += 1;
				if (e_num % 100 == 0 && rate_table2[t_id] > min_rate) {
					rate_table2[t_id] = rate * (esa_num - (real)e_num) / esa_num;
					if (rate_table2[t_id] < min_rate) rate_table2[t_id] = min_rate;
				}
			}
			char s1[10] = {}, s2[10] = {}, v[10] = {};
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
			if (value == 0) continue;
			l1 = line_id * layer1_size;
			l2 = column_id * layer1_size;
			for (i = 0; i < layer1_size; i++) neu1e[i] = 0;
			if (matrix_id == 0) for (i = 0; i < negative1 + 1; i++) {
				f = 0;
				if (i != 0) {
					next_random = next_random * (unsigned long long)25214903917 + 11;
					target = vocab_table[(next_random >> 16) % table_size];
                    if (column_id == target) continue;
					l2 = target * layer1_size;
					value = 0;
				}
                else target = column_id;
				for (j = 0; j < layer1_size; j++) {
					f += syn0[j + l1] * syn0[j + l2];
				}
				//g = rate_table1[t_id] * (value - f);
                g = value - f;
				for (j = 0; j < layer1_size; j++) {
                    g1 = g * syn0[j + l2];
                    g2 = g * syn0[j + l1];
                    v_vocab[line_id] = 0.95 * v_vocab[line_id] + 0.05 * g1 * g1;
                    v_vocab[target] = 0.95 * v_vocab[target] + 0.05 * g2 * g2;
                    g1 *= sqrt((s_vocab[line_id] + eta) / (v_vocab[line_id] + eta));
                    g2 *= sqrt((s_vocab[target] + eta) / (v_vocab[target] + eta));
                    s_vocab[line_id] = 0.95 * s_vocab[line_id] + 0.05 * g1 * g1;
                    s_vocab[target] = 0.95 * s_vocab[target] + 0.05 * g2 * g2;
					neu1e[j] += g1;
					syn0[j + l2] += g2;
				}
			}
			if (matrix_id == 1) for (i = 0 ; i < negative2 + 1; i++) {
				f = 0;
				if (i != 0) {
					next_random = next_random * (unsigned long long)25214903917 + 11;
					target = article_table[(next_random >> 16) % table_size];
                    if (column_id == target) continue;
					l2 = target * layer1_size;
					value = 0;
				}
                else target = column_id;
				for (j = 0; j < layer1_size; j++) {
					f += syn0[j + l1] * syn2[j + l2];
				}
				//g = rate_table2[t_id] * (value - f);
                g = value - f;
				for (j = 0; j < layer1_size; j++) {
                    g1 = g * syn2[j + l2];
                    g2 = g * syn0[j + l1];
                    v_vocab[line_id] = 0.95 * v_vocab[line_id] + 0.05 * g1 * g1;
                    v_article[target] = 0.95 * v_article[target] + 0.05 * g2 * g2;
                    g1 *= sqrt((s_vocab[line_id] + eta) / (v_vocab[line_id] + eta));
                    g2 *= sqrt((s_article[target] + eta) / (v_article[target] + eta));
                    s_vocab[line_id] = 0.95 * s_vocab[line_id] + 0.05 * g1 * g1;
                    s_article[target] = 0.95 * s_article[target] + 0.05 * g2 * g2;
					neu1e[j] += g1;
					syn2[j + l2] += g2;
				}
			}
			for (i = 0; i < layer1_size; i++) {
				syn0[i + l1] += neu1e[i];
			}
		}
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
}

void main(int argc, char **argv) {
	printf("Main Begin\n");
	FILE *fp, *f_vocab, *f_article;
	strcpy(train_file, "../data/infoMatrix_8_1");
    strcpy(vocab_file, "../data/vocabs.txt");
    strcpy(article_file, "../data/articles.txt");
	strcpy(output_file1, "../data/wordVectors_8");
	strcpy(output_file2, "../data/articleVectors_8");
	fp = fopen(train_file, "r");
    f_vocab = fopen(vocab_file, "r");
    f_article = fopen(article_file, "r");
	layer1_size = 1000;
	rate = 0.004;
	iter_num = 8;
	printf("GetMatrix Begin\n");
	GetMatrix(fp);
    printf("GetVocab Begin\n");
    GetVocab(f_vocab);
    printf("GetArticle Begin\n");
    GetArticle(f_article);
    printf("InitUnigramTable\n");
    InitUnigramTable();
	printf("TrainModel Begin\n");
	TrainModel();
}

