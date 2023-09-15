#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>

// 수열을 담기 위한 구조체
typedef struct {
	int key;
} List;

// a와 b를 바꿔준다.
void swapfunc(List *a, List *b) {
	List temp;
	temp = *a;
	*a = *b;
	*b = temp;
}

// heapsort에서 maxheap을 만들어주기 위한 함수. subtree를 maxheap으로 만들어준다.
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

// quicksort에서 pivot을 기준으로 subarray를 나누기 위해 사용하는 함수. pivot의 위치를 알 수 있다.
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

// Algorithm4 mysort는 radix sort와 heap sort를 이용하여 구현하였다.
// 가장 큰 수가 n보다 작을 경우에는 radix sort, 그렇지 않은 경우에는 heap sort를 이용하여 정렬한다.
// radix sort를 음수와 양수 모두에서 구현할 수 있도록 음수 data와 양수data를 나누어 정렬한 후 하나로 합쳤다.
void mysort(List *A, int n) {
	int i, max = 0, max1 = 0, max2 = 0, sig_digit = 1, bin_count[10];
	int n1 = 0;
	int n2 = 0;
	List *temp1 = (List *)malloc(sizeof(List) * (n + 1)); // 음수를 정렬하기 위함
	List *temp2 = (List *)malloc(sizeof(List) * (n + 1)); // 양수를 정렬하기 위함
	List *temp = (List *)malloc(sizeof(List) * (n + 1)); // buffer의 역할

	// 음수 data를 temp1에, 양수 data를 temp2에 저장한다.
	for (i = 1; i <= n; i++) {
		if (A[i].key < 0) {
			temp1[++n1].key = (A[i].key) * (-1);
		}
		else {
			temp2[++n2].key = A[i].key;
		}
	}

	// temp1에서 가장 절댓값이 큰 수 구하기
	for (i = 1; i <= n1; i++) {
		if (temp1[i].key > max1) {
			max1 = temp1[i].key;
		}
	}
	// temp2에서 가장 큰 수 구하기
	for (i = 1; i <= n2; i++) {
		if (temp2[i].key > max2) {
			max2 = temp2[i].key;
		}
	}

	// 원래의 전체 배열에서 가장 절댓값이 큰 수
	if ((-max1) < max2) max = max2;
	else max = max1;

	// 가장 큰 수가 n보다 작을 경우에는 radix sort
	if (n > max) {
		// 음수 data 정렬
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

		// 양수 data 정렬
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

		// 하나로 합쳐준다.
		for (i = 1; i <= n; i++) {
			A[i].key = temp[i].key;
		}
	}

	// 그렇지 않은 경우에는 heap sort
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

	// filename, input filename, algorithm index가 제대로 입력되지 않으면 0을 return 한다
	if (argc != 3) {
		printf("usage : %s inputfilename algorithmindex\n", argv[0]);
		return 0;
	}

	int ai;							 // algorithm index 값
	char filename[200];				 // input filename
	char filename2[200] = "result_2_"; // output filename. 결과 저장

									   // 변수 filename에 input filename 저장
	strcpy(filename, argv[1]);

	// 문자열로 받은 algorithm index값을 정수형으로 변환
	ai = atoi(argv[2]);

	int n;
	FILE *infile = fopen(filename, "r");
	if (infile == NULL) {
		printf("Error: input file cannot be opened.\n"); // input file이 존재하지 않음
		return 0;
	}

	fscanf(infile, "%d", &n);
	List *list = (List *)malloc(sizeof(List) * (n + 1));
	for (int i = 1; i <= n; i++) {
		fscanf(infile, "%d", &(list[i].key));
	}
	// record program start time
	clock_t start_time = clock();

	// index 1 algorithm 실행
	if (ai == 1) {
		bubblesort(list, n);
	}
	// index 2 algorithm 실행
	else if (ai == 2) {
		quicksort(list, 1, n);
	}
	// index 3 algorithm 실행
	else if (ai == 3) {
		heapsort(list, n);
	}
	// index 4 algorithm 실행
	else if (ai == 4) {
		mysort(list, n);
	}

	// record program end time
	clock_t end_time = clock();

	FILE *outfile;

	//"result_"에 input filename을 붙여서 output filename을 완성시켜줌
	strcat(filename2, filename);

	outfile = fopen(filename2, "w+");

	fprintf(outfile, "%s\n", filename);	 // line 1 : input 파일명
	fprintf(outfile, "%d\n", ai);		 // line 2 : algorithm index
	fprintf(outfile, "%d\n", n);		 // line 3 : input size
	fprintf(outfile, "%.6f\n", (double)(end_time - start_time) / CLOCKS_PER_SEC); // line 4 : 걸린시간(ms)
	for (int j = 1; j <= n; j++) {
		fprintf(outfile, "%d ", list[j].key);
	} // line 5 : the sorted list

	return 0;
}

