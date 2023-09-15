#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>

// ������ ��� ���� ����ü
typedef struct {
	int key;
} List;

// a�� b�� �ٲ��ش�.
void swapfunc(List *a, List *b) {
	List temp;
	temp = *a;
	*a = *b;
	*b = temp;
}

// heapsort���� maxheap�� ������ֱ� ���� �Լ�. subtree�� maxheap���� ������ش�.
void adjust(List *A, int root, int n) {
	int value, child;
	List temp;
	temp = A[root];
	value = A[root].key;
	child = root * 2;
	while (child <= n) {
		if ((child) < n && (A[child].key < A[child + 1].key)) {
			child++;
		}
		if (value > A[child].key) {
			break;
		}
		else {
			A[child / 2] = A[child];
			child *= 2;
		}
	}
	A[child / 2] = temp;
}

// quicksort���� pivot�� �������� subarray�� ������ ���� ����ϴ� �Լ�. pivot�� ��ġ�� �� �� �ִ�.
int partition(List *A, int left, int right) {
	int pivot = left;
	for (int i = left; i < right; i++) {
		if (A[i].key < A[right].key) {
			swapfunc(A + i, A + pivot);
			pivot++;
		}
	}
	swapfunc(A + right, A + pivot);
	return(pivot);
}

// Algorithm1 bubblesort
void bubblesort(List *A, int n) {
	int i, j;
	for (i = 1; i < n; i++) {
		for (j = n; j > i; j--) {
			if (A[j].key < A[j - 1].key) {
				swapfunc(A + j, A + j - 1);
			}
		}
	}
}

// Algorithm2 quicksort
void quicksort(List *A, int left, int right) {
	int pivot;
	if (right - left > 0) {
		pivot = partition(A, left, right);
		quicksort(A, left, pivot - 1);
		quicksort(A, pivot + 1, right);
	}

}

// Algorithm3 heapsort
void heapsort(List *A, int n) {
	for (int i = n / 2; i > 0; i--) {
		adjust(A, i, n);
	}
	for (int j = n - 1; j > 0; j--) {
		swapfunc(A + 1, A + j + 1);
		adjust(A, 1, j);
	}
}

// Algorithm4 mysort�� radix sort�� heap sort�� �̿��Ͽ� �����Ͽ���.
// ���� ū ���� n���� ���� ��쿡�� radix sort, �׷��� ���� ��쿡�� heap sort�� �̿��Ͽ� �����Ѵ�.
// radix sort�� ������ ��� ��ο��� ������ �� �ֵ��� ���� data�� ���data�� ������ ������ �� �ϳ��� ���ƴ�.
void mysort(List *A, int n) {
	int i, max = 0, max1 = 0, max2 = 0, sig_digit = 1, bin_count[10];
	int n1 = 0;
	int n2 = 0;
	List *temp1 = (List *)malloc(sizeof(List) * (n + 1)); // ������ �����ϱ� ����
	List *temp2 = (List *)malloc(sizeof(List) * (n + 1)); // ����� �����ϱ� ����
	List *temp = (List *)malloc(sizeof(List) * (n + 1)); // buffer�� ����

	// ���� data�� temp1��, ��� data�� temp2�� �����Ѵ�.
	for (i = 1; i <= n; i++) {
		if (A[i].key < 0) {
			temp1[++n1].key = (A[i].key) * (-1);
		}
		else {
			temp2[++n2].key = A[i].key;
		}
	}

	// temp1���� ���� ������ ū �� ���ϱ�
	for (i = 1; i <= n1; i++) {
		if (temp1[i].key > max1) {
			max1 = temp1[i].key;
		}
	}
	// temp2���� ���� ū �� ���ϱ�
	for (i = 1; i <= n2; i++) {
		if (temp2[i].key > max2) {
			max2 = temp2[i].key;
		}
	}

	// ������ ��ü �迭���� ���� ������ ū ��
	if ((-max1) < max2) max = max2;
	else max = max1;

	// ���� ū ���� n���� ���� ��쿡�� radix sort
	if (n > max) {
		// ���� data ����
		while (max1 / sig_digit > 0) {
			for (i = 0; i < 10; i++) {
				bin_count[i] = 0;
			}
			for (i = 1; i <= n1; i++) {
				bin_count[(temp1[i].key / sig_digit) % 10]++;
			}
			for (i = 1; i < 10; i++) {
				bin_count[i] += bin_count[i - 1];
			}
			for (i = n1; i > 0; i--) {
				temp[(--bin_count[(temp1[i].key / sig_digit) % 10]) + 1].key = temp1[i].key;
			}
			for (i = 1; i <= n1; i++) {
				temp1[i].key = temp[i].key;
			}
			sig_digit *= 10;
		}

		int k;
		for (i = 1; i <= (n1 + 1) / 2; i++) {
			k = temp[i].key;
			temp[i].key = -(temp[n1 - i + 1].key);
			temp[n1 - i + 1].key = -k;
		}

		sig_digit = 1;

		// ��� data ����
		while (max2 / sig_digit > 0) {
			for (i = 0; i < 10; i++) {
				bin_count[i] = 0;
			}
			for (i = 1; i <= n2; i++) {
				bin_count[(temp2[i].key / sig_digit) % 10]++;
			}
			for (i = 1; i < 10; i++) {
				bin_count[i] += bin_count[i - 1];
			}
			for (i = n2; i > 0; i--) {
				temp[(--bin_count[(temp2[i].key / sig_digit) % 10]) + 1 + n1].key = temp2[i].key;
			}
			for (i = 1; i <= n2; i++) {
				temp2[i].key = temp[i + n1].key;
			}
			sig_digit *= 10;
		}

		// �ϳ��� �����ش�.
		for (i = 1; i <= n; i++) {
			A[i].key = temp[i].key;
		}
	}

	// �׷��� ���� ��쿡�� heap sort
	else {
		for (int i = n / 2; i > 0; i--) {
			adjust(A, i, n);
		}
		for (int j = n - 1; j > 0; j--) {
			swapfunc(A + 1, A + j + 1);
			adjust(A, 1, j);
		}
	}

}

//  argv[0]     argv[1]          argv[2]
// filename  inputfilename  algorithmindex(1~4)
int main(int argc, char *argv[]) {

	// filename, input filename, algorithm index�� ����� �Էµ��� ������ 0�� return �Ѵ�
	if (argc != 3) {
		printf("usage : %s inputfilename algorithmindex\n", argv[0]);
		return 0;
	}

	int ai;							 // algorithm index ��
	char filename[200];				 // input filename
	char filename2[200] = "result_2_"; // output filename. ��� ����

									   // ���� filename�� input filename ����
	strcpy(filename, argv[1]);

	// ���ڿ��� ���� algorithm index���� ���������� ��ȯ
	ai = atoi(argv[2]);

	int n;
	FILE *infile = fopen(filename, "r");
	if (infile == NULL) {
		printf("Error: input file cannot be opened.\n"); // input file�� �������� ����
		return 0;
	}

	fscanf(infile, "%d", &n);
	List *list = (List *)malloc(sizeof(List) * (n + 1));
	for (int i = 1; i <= n; i++) {
		fscanf(infile, "%d", &(list[i].key));
	}
	// record program start time
	clock_t start_time = clock();

	// index 1 algorithm ����
	if (ai == 1) {
		bubblesort(list, n);
	}
	// index 2 algorithm ����
	else if (ai == 2) {
		quicksort(list, 1, n);
	}
	// index 3 algorithm ����
	else if (ai == 3) {
		heapsort(list, n);
	}
	// index 4 algorithm ����
	else if (ai == 4) {
		mysort(list, n);
	}

	// record program end time
	clock_t end_time = clock();

	FILE *outfile;

	//"result_"�� input filename�� �ٿ��� output filename�� �ϼ�������
	strcat(filename2, filename);

	outfile = fopen(filename2, "w+");

	fprintf(outfile, "%s\n", filename);	 // line 1 : input ���ϸ�
	fprintf(outfile, "%d\n", ai);		 // line 2 : algorithm index
	fprintf(outfile, "%d\n", n);		 // line 3 : input size
	fprintf(outfile, "%.6f\n", (double)(end_time - start_time) / CLOCKS_PER_SEC); // line 4 : �ɸ��ð�(ms)
	for (int j = 1; j <= n; j++) {
		fprintf(outfile, "%d ", list[j].key);
	} // line 5 : the sorted list

	return 0;
}

