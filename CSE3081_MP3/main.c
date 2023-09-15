#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>

// ������ �ڵ带 ����� ���� ����ü
typedef struct _node {
	char sym[2]; // ������ ����
	int freq; // �󵵼�
	struct _node *left;
	struct _node *right;
	int leafnode; // �ش� ���ڰ� 1byte¥������ �ƴϸ� 2byte¥������ �˷���
} NODE;

// ������ ������ �� ���ڿ� �����ϴ� 0�� 1�� �̷���� �ڵ忡 ���� ������ ����ִ� ����ü
typedef struct _huff {
	char hcode[200]; // 0�� 1�� �̷���� �ڵ�
	char key[2]; // ������ ����
} HUFF;

// u, v : heap���� 1,2��°�� ���� ���, w : u, v�� �ڽĳ��� ���� �ӽ� ���,
NODE *u, *v, *w;

void swapfunc(NODE *a, NODE *b) {
	NODE temp;
	temp = *a;
	*a = *b;
	*b = temp;
}

// heapsort���� minheap�� ������ֱ� ���� �Լ�. subtree�� minheap���� ������ش�.
void adjust(NODE *A, int root, int n) {
	int value, child;
	NODE temp;
	temp = A[root];
	value = A[root].freq;
	child = root * 2;
	while (child <= n) {
		if ((child < n) && (A[child].freq < A[child + 1].freq)) {
			child++;
		}
		if (value > A[child].freq) {
			break;
		}
		else {
			A[child / 2] = A[child];
			child *= 2;
		}
	}
	A[child / 2] = temp;
}

// minheap���� ������ֱ� ���� sorting algorithm
void heapsort(NODE *A, int n) {
	for (int i = n / 2; i > 0; i--) {
		adjust(A, i, n);
	}
	for (int j = n - 1; j > 0; j--) {
		swapfunc(A + 1, A + j + 1);
		adjust(A, 1, j);
	}
}

// w�� heap�� insert
void PQ_insert(NODE** heap, NODE* a, int *m) {
	int i;
	i = ++(*m); // 0
	while ((i != 1) && (a->freq < (*heap[i - 1]).freq)){
		heap[i] = heap[i - 1];
		i--;
	}
	heap[i] = a;
}

// heap���� u, v�� �̾Ƴ�
NODE* PQ_delete(NODE** heap, int *m) {
	NODE* item, *temp;
	int parent = 1;
	int child = 2;
	item = heap[1]; // �̾Ƴ��� ���
	for (int k = 2; k <= (*m); k++) {
		heap[k - 1] = heap[k];
	}
	(*m)--; // �� ������ ���(�ٽ� heap �����ϱ� ����)
	
	return item;
}

// u, v�� ��ġ������ w�� ����
NODE* make_a_new_node() {
	w = (NODE*)malloc(sizeof(NODE));
	return w;
}

// �ϳ��� tree�� ����� ��
void Huffman(NODE** heap, int m) {
	int l = m;
	for (int i = 1; i <= l - 1; i++) {
		u = PQ_delete(heap, &m); // ���� ���� 2���� �̰�
		v = PQ_delete(heap, &m);

		w = make_a_new_node(); // w��� ���ο� ��带 �����
		w->left = u; // ���� ��/������ �ڽ�����
		w->right = v;
		w->freq = u->freq + v->freq;

		PQ_insert(heap, w, &m); // w�� �ٽ� PQ��
	}
	w = PQ_delete(heap, &m);
}

// leaf node�� �����ϸ� �� �������� ���, �� �� ���ں� 0�� 1�� �̷���� �ڵ带 ����
void encode(NODE* root, char str[200], HUFF* huffcode, int *i) {
	char str1[200];
	strcpy(str1, str);
	char str2[200];
	strcpy(str2, str);
	if ((root->leafnode == 1)|| (root->leafnode == 2)) {
		strcpy(huffcode[*i].hcode, str);
		huffcode[*i].key[0] = root->sym[0];
		huffcode[*i].key[1] = root->sym[1];
		(*i)++;
		return;
	}
	encode(root->left, strcat(str1, "0"), huffcode, i);
	encode(root->right, strcat(str2, "1"), huffcode, i);
}

// ������ Ʈ���� 0�� 1�� �̷���� �ڵ带 �̿��Ͽ� �����ڵ带 ������ ���ڷ� �ٲ۴�.
void decode(NODE* root, HUFF* huff, int mc2, FILE* out) {
	
	NODE *srch = root;
	
	for (int i = 0; i < mc2 + 1; i++) {
		if (srch->leafnode == 1) {
			if (srch->sym[0] == 13) {
			}
			else fprintf(out, "%c", srch->sym[0]);
			srch = root;
		}
		else if (srch->leafnode == 2) {
			fprintf(out, "%c%c", srch->sym[0], srch->sym[1]);
			srch = root;
		}
		if (huff[i].key[0] == '0') {
			srch = srch->left;
		}
		else if (huff[i].key[0] == '1') {
			srch = srch->right;
		}
	}
}

//  argv[0]     argv[1]       argv[2]
// filename     -c / -d    inputfilename
int main(int argc, char *argv[]) {
	// int�� ����. m : ������ ���� ��, mc : �Է������� �� ���� ��, index : heap2�� ���� �� ī��Ʈ�� ���� ����
	//			ind : encode �Լ����� ī��Ʈ�� ���� ����
	//			ri, ri2 : ī��Ʈ�� ���� ����
	//			a : input������ ��ĵ�� �� ���������� ���� ���� ����������.
	// char�� ����. b : input���� ��ĵ�� �� ���ڸ� �ϳ��� ���� �� ���
	// ���ڿ�. str : encode �Լ����� ���
	int m = 1, mc = 0, index = 1, ind = 1, ri = 0, ri2, a;
	char b;
	char str[200] = "";
	
	int mk = 0; // 0�� 1�� ������ 6���� ���� ������
	int mk2 = 0; // 6���� ���� ��

	// ���� ���ҵ�
	unsigned char wrr;
	unsigned char temp = 0;
	char sen[6] = "";
	int ee = 0;
	char res[200] = "";
	char asdf[6];
	unsigned int aa;

	// filename, input filename, algorithm index�� ����� �Էµ��� ������ 0�� return �Ѵ�
	if (argc != 3) {
		printf("usage : %s inputfilename algorithmindex\n", argv[0]);
		return 0;
	}

	char cd[3];						 // compressing���� decompressing����
	char filename[200];				 // input filename
	char filename2[200];			 // output filename. compressing ��� ����
	char filename3[200];			 // output filename. decompressing ��� ����

	strcpy(filename, argv[2]);		 // ���� filename�� input filename ����

	strcpy(cd, argv[1]);			 // -c ���� -d ������ ����

	// output filename�� �ϼ�������
	strcpy(filename2, filename);
	strcat(filename2, ".zz");
	strcpy(filename3, filename);
	strcat(filename3, ".yy");

	// compressing ����
	if ((cd[0] == '-') && (cd[1] == 'c')) {

		FILE *fpr = fopen(filename, "r");
		FILE *outfile = fopen(filename2, "w+");

		if (fpr == NULL) {
			printf("Error: input file cannot be opened.\n"); // input file�� �������� ����
			return 0;
		}
		// �Է������� ���ڰ� 1byte¥������ 2byte¥�������� �����ؼ� ������ ������ ����.
		while (!feof(fpr)) {
			fscanf(fpr, "%c", &b);
			a = (int)b;
			if ((a >= 0) && (a <= 127)) { }
			else {
				fscanf(fpr, "%c", &b);
			}
			mc++;
		}
		fclose(fpr);

		// �Է����Ͽ� ����ִ� ��� ���ڿ� ���� ������ ����
		NODE *star = (NODE*)malloc(sizeof(NODE)*(mc + 1));

		mc = 1;
		FILE *fpr2 = fopen(filename, "r");

		if (fpr2 == NULL) {
			printf("Error: input file cannot be opened.\n"); // input file�� �������� ����
			return 0;
		}
		// �Է������� ���� 1byte¥���� 2byte¥���� ����
		while (!feof(fpr2)) {
			fscanf(fpr2, "%c", &b);
			a = (int)b;
			if ((a >= 0) && (a <= 127)) {
				star[mc].leafnode = 1;
			}
			else {
				fscanf(fpr2, "%c", &b);
				star[mc].leafnode = 2;
			}
			mc++;
		}
		fclose(fpr2);

		mc = 1;
		FILE *fpr3 = fopen(filename, "r");

		if (fpr3 == NULL) {
			printf("Error: input file cannot be opened.\n"); // input file�� �������� ����
			return 0;
		}
		// 1byte¥���� 2byte¥���� �����ؼ� ������ ������ ������ ����
		while (!feof(fpr3)) {
			if (star[mc].leafnode == 1) {
				fscanf(fpr2, "%c", &b);
				a = (int)b;
				star[mc].sym[0] = a;
				star[mc].sym[1] = '\0';
			}
			else if (star[mc].leafnode == 2) {
				fscanf(fpr3, "%c%c", &star[mc].sym[0], &star[mc].sym[1]);
			}
			mc++;
		}
		fclose(fpr3);

		mc -= 2;

		// �� ������ ������ �󵵼��� �����ϱ� ���� ����ü
		NODE *star2 = (NODE*)malloc(sizeof(NODE)*(mc + 1));

		// ������ ���� �� �ľ�
		// �迭�� ����Ǿ� �ִ� ��� ������ ������ �󵵼��� ����ü�� �����Ѵ�.
		for (int i = 1; i <= mc; i++) {
			for (int j = 1; j < m; j++) {
				if ((star[i].sym[0] == star2[j].sym[0]) && (star[i].sym[1] == star2[j].sym[1])) {
					ri = 1; ri2 = j;
				}
			}
			if (ri == 1) {
				star2[ri2].freq += 1;
			}
			else {
				star2[m].sym[0] = star[i].sym[0];
				star2[m].sym[1] = star[i].sym[1];
				star2[m].freq = 1;
				star2[m].leafnode = star[i].leafnode;
				m++;
			}
			ri = 0;
		}
		m -= 1;
		
		// �ϳ��� ���ڷθ� �̷���� ���� ��
		if (m == 1) {

			fprintf(outfile, "%d %d ", m, mc);
			fprintf(outfile, "%d ", star2[1].leafnode);
			if (star2[1].leafnode == 1) {
				fprintf(outfile, "%d ", star2[1].sym[0]);
			}
			else fprintf(outfile, "%c%c ", star2[1].sym[0], star2[1].sym[1]);
		
		}

		// ���� ���� ���ڷ� �̷���� ���� ��
		else {

			// min heap���� ����
			heapsort(star2, m);

			// star2�� �ִ� ���ҵ�(�� �ϳ��� ����ü��) ������ �ϳ��� �̱� ��� Ʈ���� ������ش�.
			NODE **heap = (NODE**)malloc(sizeof(NODE*) * (m + 1));

			// ������ �ϳ��� �̱� ��� Ʈ���� �����.
			for (int i = 1; i <= m; i++) {
				heap[i] = &(star2[i]);
			}

			// freq�� ���� ���� Ʈ���� �� ���� �̾� ���� �ϳ��� Ʈ���� ������ش�.
			Huffman(heap, m);

			// ������ ������ �׿� ���� 0�� 1�� �̷���� �ڵ� ������ �����ϱ� ���� ����ü 
			HUFF* huffcode = (HUFF*)malloc(sizeof(HUFF)*(m + 1));

			// �ϳ��� ������� Ʈ���� root node���� leaf node������ ��θ� ���󰡸� 0�� 1�� ǥ���� �ڵ带 ã�´�.
			// �� leaf node���� ������ ������ �󵵼��� ���� ������ ��������Ƿ� �� ��δ� leaf node�� ���ڿ� ���� �ڵ��̴�.
			encode(heap[1], str, huffcode, &ind);

			// 0�� 1�� ������ �ڵ带 6bit�� ���� 1byte�� ���ڷ� ǥ�����ֱ� ���� ����ü
			HUFF* huff = (HUFF*)malloc(sizeof(HUFF)*mc);

			// 0�� 1�� �̷���� �ڵ�� ��ȯ
			for (int i = 1; i <= mc; i++) {
				for (int j = 1; j <= m; j++) {
					if ((star[i].sym[0] == huffcode[j].key[0]) && (star[i].sym[1] == huffcode[j].key[1])) {
						strcpy(res, huffcode[j].hcode);
						strcpy(huff[ee].hcode, res);
						ee++;
					}
				}
			}

			// 6bit¥���� �� ��������, 6���� ���� ������ �� bit������ �˱� ����.
			for (int i = 0; i < ee; i++) {
				for (int j = 0; j < strlen(huff[i].hcode); j++) {
					mk++;
					if (mk == 6) {
						mk = 0;
						mk2++;
					}
				}
			}

			int mm = mk2 * 6 + mk; // �� 0�� 1�� ����

			// decompressing�� ���� ������ ���� ���� �� ���� ��, 6���� ������ �� � ���� ������ �������Ͽ� ��´�.
			// ������ ������ �󵵼� � ���� ������ ���� Ʈ���� ���� ������ �����Ѵ�.
			// �̸� �̿��Ͽ� decompressing�� ���� ������ Ʈ���� �ٽ� ���� �� �ִ�.
			fprintf(outfile, "%d %d %d ", m, mm, mk2);
			for (int i = 1; i <= m; i++) {
				fprintf(outfile, "%d %d ", star2[i].leafnode, star2[i].freq);
				if (star2[i].leafnode == 1) {
					fprintf(outfile, "%d ", star2[i].sym[0]);
				}
				else fprintf(outfile, "%c%c ", star2[i].sym[0], star2[i].sym[1]);
			}

			mk = 0; mk2 = 0;

			// 6���� ��� ��ȯ
			for (int i = 0; i < ee; i++) {
				for (int j = 0; j < strlen(huff[i].hcode); j++) {
					wrr = huff[i].hcode[j];
					sen[mk] = wrr;
					mk++;
					if (mk == 6) {
						for (int i = 0; i < 6; i++) {
							temp += (sen[i] - 48) << (5 - i);
						}
						if (temp == 26) {
							fprintf(outfile, "%c", 64);
						}
						else {
							fprintf(outfile, "%c", temp);
						}
						for (int k = 0; k < 6; k++) {
							sen[k] = 0;
						}
						temp = 0;
						mk = 0;
						mk2++;
					}
				}
			}
			if (mk != 0) {
				fprintf(outfile, "%s", sen);
			}
		}
		fclose(outfile);	
	}

	// decompressing ����
	else if ((cd[0] == '-') && (cd[1] == 'd')) {

		int ind = 1, m, mm, mk2, mk;
		char str[200] = "";
		char b;

		FILE *fp3 = fopen(filename, "r");
		if (fp3 == NULL) {
			printf("Error: input file cannot be opened.\n"); // output file�� �������� ����
			return 0;
		}
	
		FILE *outfile1 = fopen(filename3, "w+");

		NODE *star2 = (NODE*)malloc(sizeof(NODE) * 2);

		// ������ ���� ���� �� ���� ��, 6���� ������ �� � ���� ������ �д´�.
		fscanf(fp3, "%d %d ", &m, &mm);

		// �ϳ��� ���ڷθ� �̷���� ������ ��
		if (m == 1) {

			fscanf(fp3, "%d ", &star2[1].leafnode);

			if (star2[1].leafnode == 1) {
				fscanf(fp3, "%d ", &a);
				star2[1].sym[0] = a;
				star2[1].sym[1] = '\0';
			}
			else fscanf(fp3, "%c%c ", &star2[1].sym[0], &star2[1].sym[1]);

			for (int i = 1; i <= mm; i++) {
				fprintf(outfile1, "%c%c", star2[1].sym[0], star2[1].sym[1]);
			}

		}

		// ���� ���� ���ڷ� �̷���� ������ ��
		else {

			fscanf(fp3, "%d ", &mk2);
			mk = (mk2) % 6;

			// �� ������ ������ �󵵼��� �����ϱ� ���� ����ü
			star2 = (NODE*)realloc(star2, sizeof(NODE)*(m + 1));

			// ������ ������ �󵵼� � ���� ������ ���� Ʈ���� ���� ������ �д´�.
			for (int i = 1; i <= m; i++) {
				fscanf(fp3, "%d %d ", &star2[i].leafnode, &star2[i].freq);
				if (star2[i].leafnode == 1) {
					fscanf(fp3, "%d ", &a);
					star2[i].sym[0] = a;
					star2[i].sym[1] = '\0';
				}
				else fscanf(fp3, "%c%c ", &star2[i].sym[0], &star2[i].sym[1]);
			}

			// ������ Ʈ���� �ٽ� �����.
			NODE **heap = (NODE**)malloc(sizeof(NODE*) * (m + 1));
			for (int i = 1; i <= m; i++) {
				heap[i] = &(star2[i]);
			}
			Huffman(heap, m);

			// ������ ������ �׿� ���� 0�� 1�� �̷���� �ڵ� ������ �����ϱ� ���� ����ü 
			HUFF* huffcode = (HUFF*)malloc(sizeof(HUFF)*(m + 1));

			// �ϳ��� ������� Ʈ���� root node���� leaf node������ ��θ� ���󰡸� 0�� 1�� ǥ���� �ڵ带 ã�´�.
			// �� leaf node���� ������ ������ �󵵼��� ���� ������ ��������Ƿ� �� ��δ� leaf node�� ���ڿ� ���� �ڵ��̴�.

			encode(heap[1], str, huffcode, &ind);

			// 0�� 1�� �̷���� �ڵ带 �����ϱ� ���� ����ü
			HUFF* huff2 = (HUFF*)malloc(sizeof(HUFF)*mm);

			ind = 0;
			// 0�� 1�� �̷���� �ڵ带 ��ȯ
			for (int j = 0; j < mk2; j++) {
				fscanf(fp3, "%c", &b);
				aa = (int)b;
				if (aa == 64) {
					aa = 26;
				}
				for (int i = 0; i < 6; i++) {
					asdf[5 - i] = (aa % 2) + 48;
					aa /= 2;
				}
				for (int k = 0; k < 6; k++) {
					huff2[ind + k].key[0] = asdf[k];
				}
				ind += 6;
			}

			for (int j = 0; j < mk; j++) {
				fscanf(fp3, "%c", &b);
				huff2[ind++].key[0] = b;
			}

			// huffcode�� �̿��Ͽ� 0�� 1�� �̷���� �ڵ带 �����ϴ� ���ڷ� �ٲٰ� ��� ���Ͽ� �����Ѵ�.
			decode(heap[1], huff2, mm, outfile1);
		}
		fclose(fp3);
		fclose(outfile1);
	}

	else {
		printf("Error : please write '-c' / or '-d'");
	}

	return 0;

}
