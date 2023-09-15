#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>

// 허프만 코드를 만들기 위한 구조체
typedef struct _node {
	char sym[2]; // 문자의 종류
	int freq; // 빈도수
	struct _node *left;
	struct _node *right;
	int leafnode; // 해당 문자가 1byte짜리인지 아니면 2byte짜리인지 알려줌
} NODE;

// 문자의 종류와 그 문자에 대응하는 0과 1로 이루어진 코드에 관한 정보를 담고있는 구조체
typedef struct _huff {
	char hcode[200]; // 0과 1로 이루어진 코드
	char key[2]; // 문자의 종류
} HUFF;

// u, v : heap에서 1,2번째로 작은 노드, w : u, v를 자식노드로 갖는 임시 노드,
NODE *u, *v, *w;

void swapfunc(NODE *a, NODE *b) {
	NODE temp;
	temp = *a;
	*a = *b;
	*b = temp;
}

// heapsort에서 minheap을 만들어주기 위한 함수. subtree를 minheap으로 만들어준다.
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

// minheap으로 만들어주기 위한 sorting algorithm
void heapsort(NODE *A, int n) {
	for (int i = n / 2; i > 0; i--) {
		adjust(A, i, n);
	}
	for (int j = n - 1; j > 0; j--) {
		swapfunc(A + 1, A + j + 1);
		adjust(A, 1, j);
	}
}

// w를 heap에 insert
void PQ_insert(NODE** heap, NODE* a, int *m) {
	int i;
	i = ++(*m); // 0
	while ((i != 1) && (a->freq < (*heap[i - 1]).freq)){
		heap[i] = heap[i - 1];
		i--;
	}
	heap[i] = a;
}

// heap에서 u, v를 뽑아냄
NODE* PQ_delete(NODE** heap, int *m) {
	NODE* item, *temp;
	int parent = 1;
	int child = 2;
	item = heap[1]; // 뽑아내는 노드
	for (int k = 2; k <= (*m); k++) {
		heap[k - 1] = heap[k];
	}
	(*m)--; // 맨 마지막 노드(다시 heap 정렬하기 위함)
	
	return item;
}

// u, v를 합치기위한 w를 생성
NODE* make_a_new_node() {
	w = (NODE*)malloc(sizeof(NODE));
	return w;
}

// 하나의 tree로 만들어 줌
void Huffman(NODE** heap, int m) {
	int l = m;
	for (int i = 1; i <= l - 1; i++) {
		u = PQ_delete(heap, &m); // 제일 작은 2개를 뽑고
		v = PQ_delete(heap, &m);

		w = make_a_new_node(); // w라는 새로운 노드를 만들어
		w->left = u; // 각각 왼/오른쪽 자식으로
		w->right = v;
		w->freq = u->freq + v->freq;

		PQ_insert(heap, w, &m); // w를 다시 PQ에
	}
	w = PQ_delete(heap, &m);
}

// leaf node에 도착하면 이 때까지의 경로, 즉 각 문자별 0과 1로 이루어진 코드를 구함
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

// 허프만 트리와 0과 1로 이루어진 코드를 이용하여 이진코드를 원래의 문자로 바꾼다.
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
	// int형 변수. m : 문자의 종류 수, mc : 입력파일의 총 문자 수, index : heap2를 만들 때 카운트를 위한 변수
	//			ind : encode 함수에서 카운트를 위한 변수
	//			ri, ri2 : 카운트를 위한 변수
	//			a : input파일을 스캔할 때 문자형으로 받은 것을 정수형으로.
	// char형 변수. b : input파일 스캔할 때 문자를 하나씩 읽을 때 사용
	// 문자열. str : encode 함수에서 사용
	int m = 1, mc = 0, index = 1, ind = 1, ri = 0, ri2, a;
	char b;
	char str[200] = "";
	
	int mk = 0; // 0과 1의 개수를 6으로 나눈 나머지
	int mk2 = 0; // 6으로 나눈 몫

	// 버퍼 역할들
	unsigned char wrr;
	unsigned char temp = 0;
	char sen[6] = "";
	int ee = 0;
	char res[200] = "";
	char asdf[6];
	unsigned int aa;

	// filename, input filename, algorithm index가 제대로 입력되지 않으면 0을 return 한다
	if (argc != 3) {
		printf("usage : %s inputfilename algorithmindex\n", argv[0]);
		return 0;
	}

	char cd[3];						 // compressing인지 decompressing인지
	char filename[200];				 // input filename
	char filename2[200];			 // output filename. compressing 결과 저장
	char filename3[200];			 // output filename. decompressing 결과 저장

	strcpy(filename, argv[2]);		 // 변수 filename에 input filename 저장

	strcpy(cd, argv[1]);			 // -c 인지 -d 인지를 저장

	// output filename을 완성시켜줌
	strcpy(filename2, filename);
	strcat(filename2, ".zz");
	strcpy(filename3, filename);
	strcat(filename3, ".yy");

	// compressing 실행
	if ((cd[0] == '-') && (cd[1] == 'c')) {

		FILE *fpr = fopen(filename, "r");
		FILE *outfile = fopen(filename2, "w+");

		if (fpr == NULL) {
			printf("Error: input file cannot be opened.\n"); // input file이 존재하지 않음
			return 0;
		}
		// 입력파일의 문자가 1byte짜리인지 2byte짜리인지를 구분해서 문자의 개수를 센다.
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

		// 입력파일에 들어있는 모든 문자에 대한 정보를 저장
		NODE *star = (NODE*)malloc(sizeof(NODE)*(mc + 1));

		mc = 1;
		FILE *fpr2 = fopen(filename, "r");

		if (fpr2 == NULL) {
			printf("Error: input file cannot be opened.\n"); // input file이 존재하지 않음
			return 0;
		}
		// 입력파일의 문자 1byte짜리와 2byte짜리를 구분
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
			printf("Error: input file cannot be opened.\n"); // input file이 존재하지 않음
			return 0;
		}
		// 1byte짜리와 2byte짜리를 구분해서 각각의 문자의 종류를 저장
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

		// 각 문자의 종류와 빈도수를 저장하기 위한 구조체
		NODE *star2 = (NODE*)malloc(sizeof(NODE)*(mc + 1));

		// 문자의 종류 수 파악
		// 배열에 저장되어 있던 모든 문자의 종류와 빈도수를 구조체에 저장한다.
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
		
		// 하나의 문자로만 이루어져 있을 때
		if (m == 1) {

			fprintf(outfile, "%d %d ", m, mc);
			fprintf(outfile, "%d ", star2[1].leafnode);
			if (star2[1].leafnode == 1) {
				fprintf(outfile, "%d ", star2[1].sym[0]);
			}
			else fprintf(outfile, "%c%c ", star2[1].sym[0], star2[1].sym[1]);
		
		}

		// 여러 개의 문자로 이루어져 있을 때
		else {

			// min heap으로 정렬
			heapsort(star2, m);

			// star2에 있던 원소들(즉 하나의 구조체들) 각각을 하나의 싱글 노드 트리로 만들어준다.
			NODE **heap = (NODE**)malloc(sizeof(NODE*) * (m + 1));

			// 각각을 하나의 싱글 노드 트리로 만든다.
			for (int i = 1; i <= m; i++) {
				heap[i] = &(star2[i]);
			}

			// freq가 가장 작은 트리를 두 개씩 뽑아 합쳐 하나의 트리로 만들어준다.
			Huffman(heap, m);

			// 문자의 종류와 그에 대한 0과 1로 이루어진 코드 정보를 저장하기 위한 구조체 
			HUFF* huffcode = (HUFF*)malloc(sizeof(HUFF)*(m + 1));

			// 하나로 만들어준 트리의 root node부터 leaf node까지의 경로를 따라가며 0과 1로 표현된 코드를 찾는다.
			// 각 leaf node에는 문자의 종류와 빈도수에 대한 정보가 들어있으므로 이 경로는 leaf node의 문자에 대한 코드이다.
			encode(heap[1], str, huffcode, &ind);

			// 0과 1로 구성된 코드를 6bit씩 끊어 1byte의 문자로 표현해주기 위한 구조체
			HUFF* huff = (HUFF*)malloc(sizeof(HUFF)*mc);

			// 0과 1로 이루어진 코드로 변환
			for (int i = 1; i <= mc; i++) {
				for (int j = 1; j <= m; j++) {
					if ((star[i].sym[0] == huffcode[j].key[0]) && (star[i].sym[1] == huffcode[j].key[1])) {
						strcpy(res, huffcode[j].hcode);
						strcpy(huff[ee].hcode, res);
						ee++;
					}
				}
			}

			// 6bit짜리가 몇 묶음인지, 6개씩 묶고 남은건 몇 bit인지를 알기 위함.
			for (int i = 0; i < ee; i++) {
				for (int j = 0; j < strlen(huff[i].hcode); j++) {
					mk++;
					if (mk == 6) {
						mk = 0;
						mk2++;
					}
				}
			}

			int mm = mk2 * 6 + mk; // 총 0과 1의 개수

			// decompressing을 위해 문자의 종류 수와 총 문자 수, 6개씩 묶음의 수 등에 대한 정보를 압축파일에 담는다.
			// 문자의 종류와 빈도수 등에 대한 정보를 담은 트리에 대한 정보도 저장한다.
			// 이를 이용하여 decompressing를 위한 허프만 트리를 다시 만들 수 있다.
			fprintf(outfile, "%d %d %d ", m, mm, mk2);
			for (int i = 1; i <= m; i++) {
				fprintf(outfile, "%d %d ", star2[i].leafnode, star2[i].freq);
				if (star2[i].leafnode == 1) {
					fprintf(outfile, "%d ", star2[i].sym[0]);
				}
				else fprintf(outfile, "%c%c ", star2[i].sym[0], star2[i].sym[1]);
			}

			mk = 0; mk2 = 0;

			// 6개씩 끊어서 변환
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

	// decompressing 실행
	else if ((cd[0] == '-') && (cd[1] == 'd')) {

		int ind = 1, m, mm, mk2, mk;
		char str[200] = "";
		char b;

		FILE *fp3 = fopen(filename, "r");
		if (fp3 == NULL) {
			printf("Error: input file cannot be opened.\n"); // output file이 존재하지 않음
			return 0;
		}
	
		FILE *outfile1 = fopen(filename3, "w+");

		NODE *star2 = (NODE*)malloc(sizeof(NODE) * 2);

		// 문자의 종류 수와 총 문자 수, 6개씩 묶음의 수 등에 대한 정보를 읽는다.
		fscanf(fp3, "%d %d ", &m, &mm);

		// 하나의 문자로만 이루어진 파일일 때
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

		// 여러 개의 문자로 이루어진 파일일 때
		else {

			fscanf(fp3, "%d ", &mk2);
			mk = (mk2) % 6;

			// 각 문자의 종류와 빈도수를 저장하기 위한 구조체
			star2 = (NODE*)realloc(star2, sizeof(NODE)*(m + 1));

			// 문자의 종류와 빈도수 등에 대한 정보를 담은 트리에 대한 정보도 읽는다.
			for (int i = 1; i <= m; i++) {
				fscanf(fp3, "%d %d ", &star2[i].leafnode, &star2[i].freq);
				if (star2[i].leafnode == 1) {
					fscanf(fp3, "%d ", &a);
					star2[i].sym[0] = a;
					star2[i].sym[1] = '\0';
				}
				else fscanf(fp3, "%c%c ", &star2[i].sym[0], &star2[i].sym[1]);
			}

			// 허프만 트리를 다시 만든다.
			NODE **heap = (NODE**)malloc(sizeof(NODE*) * (m + 1));
			for (int i = 1; i <= m; i++) {
				heap[i] = &(star2[i]);
			}
			Huffman(heap, m);

			// 문자의 종류와 그에 대한 0과 1로 이루어진 코드 정보를 저장하기 위한 구조체 
			HUFF* huffcode = (HUFF*)malloc(sizeof(HUFF)*(m + 1));

			// 하나로 만들어준 트리의 root node부터 leaf node까지의 경로를 따라가며 0과 1로 표현된 코드를 찾는다.
			// 각 leaf node에는 문자의 종류와 빈도수에 대한 정보가 들어있으므로 이 경로는 leaf node의 문자에 대한 코드이다.

			encode(heap[1], str, huffcode, &ind);

			// 0과 1로 이루어진 코드를 저장하기 위한 구조체
			HUFF* huff2 = (HUFF*)malloc(sizeof(HUFF)*mm);

			ind = 0;
			// 0과 1로 이루어진 코드를 변환
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

			// huffcode를 이용하여 0과 1로 이루어진 코드를 대응하는 문자로 바꾸고 출력 파일에 저장한다.
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
