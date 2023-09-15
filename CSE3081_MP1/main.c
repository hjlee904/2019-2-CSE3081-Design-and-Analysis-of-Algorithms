#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>

// N*M matrix (N : num of rows, M : num of cols)
// TS = ThisSum, MS = MaxSum
// rs = row start, re = row end, cs = col start, ce = col end
// anm : all negative matrix (모든 값이 전부 음수)인 경우의 MS를 구하기 위한 변수

// bigO(n^6) algorithm (index 1)
// 시작행(0~N), 마지막행(시작행~N), 시작열(0~M), 마지막열(시작열~M)인 경우의 sum을 모두 계산

int MSS1(const int A[], int N, int M) {

	int TS, MS, rs, re, cs, ce, k, l, p, anm;

	anm = A[0];
	MS = 0;

	for (rs = 0; rs < N; rs++) {
		for (re = rs; re < N; re++) {
			for (cs = 0; cs < M; cs++) {
				for (ce = cs; ce < M; ce++) {
					TS = 0;
					for (k = rs; k <= re; k++) {
						for (l = cs; l <= ce; l++) {
							p = k * M + l;

							// 각 셀이 모두 음수인 경우에 그 중 가장 큰 값을 구하기 위한 과정
							if (anm < A[p]) {
								anm = A[p];
							}

							TS += A[p];
						}
					}
					if (TS > MS) {
						MS = TS;
					}
				}
			}
		}
	}

	if (anm < 0) { // 각 셀이 모두 음수인 경우
		MS = anm;
	}

	return MS;
}

// bigO(N^4) algorithm (index 2)
// 시작행(0~N)과 마지막행(시작행~N)을 정해주었을 때 처음 열부터 끝 열까지(0~M) 마지막 열을 이동시키면서 이전 계산 값을 이용하여 계속 더해준다.
// TS값이 음수인 경우는 TS = 0으로 해주면서 MS을 구한다.
// Dynamaic programming

int MSS2(const int A[], int N, int M) {

	int TS, MS, ia, ib, jb, k, p, anm;

	anm = A[0];
	MS = 0;

	for (ia = 0; ia < N; ia++) {
		for (ib = ia; ib < N; ib++) {
			TS = 0;
			for (jb = 0; jb < M; jb++) {
				for (k = ia; k <= ib; k++) {
					p = k*M + jb;
					if (anm < A[p]) {
						anm = A[p];
					}
					TS += A[p];
				}
				if (TS > MS) {
					MS = TS;
				}
				else if (TS < 0) {
					TS = 0;
				}
			}
		}
	}

	if (anm < 0) {
		MS = anm;
	}

	return MS;
}

// bigO(N^3) algorithm (index 3)
/* 시작행과 끝행을 정해주고 이 범위에 속하는 값들을 열 단위로 더한다.
   더한 값은 배열 B에 저장하고 이를 1차원 배열로 생각할 수 있다.
   따라서 Maximum Subsequence Sum 문제로 해결할 수 있게 된다.*/

int MSS3(const int A[], int N, int M) {

	int TS, MS, cs, ce, k, p, anm;
	int B[1000] = { 0 }; // 열 단위로 시작행부터 끝행까지 더한 값

	anm = A[0];
	MS = 0;

	for (cs = 0; cs < M; cs++) {
		for (ce = cs; ce < M; ce++) {
			TS = 0;

			for (k = 0; k < N; k++) {
				p = M*k + ce;
				B[k] += A[p];
				if (anm < A[p]) {
					anm = A[p];
				}
			}

			for (k = 0; k < N; k++) {
				TS += B[k];
				if (TS > MS) {
					MS = TS;
				}
				else if (TS < 0) {
					TS = 0;
				}
			}

		}
		for (k = 0; k < N; k++) {
			B[k] = 0;
		}
	}

	if (anm < 0) {
		MS = anm;
	}

	return MS;
}


//  argv[0]     argv[1]          argv[2]
// filename  inputfilename  algorithmindex(1~3)
int main(int argc, char *argv[]) {

	// filename, input filename, algorithm index가 제대로 입력되지 않으면 0을 return 한다
	if (argc != 3) {
		printf("usage : %s inputfilename algorithmindex\n", argv[0]);
		return 0;
	}

	int A[10000];					 // input file으로 부터 행렬의 cell에 대한 정보(숫자값)를 저장한다.(0~N*M)
	int N, M, rr, MS;				 // N : num of rows, M : num of cols, MS = MaxSum
	int ai;							 // algorithm index 값
	char filename[200];				 // input filename
	char filename2[200] = "result_"; // output filename. 결과 저장

	// 변수 filename에 input filename 저장
	strcpy(filename, argv[1]);

	// 문자열로 받은 algorithm index값을 정수형으로 변환
	ai = atoi(argv[2]);

	FILE *infile;
	infile = fopen(filename, "r");
	if (infile == NULL) {
		printf("Error: input file cannot be opened.\n"); // input file이 존재하지 않음
		return 0;
	}

	// input file로부터 N, M에 행의 수와 열의 수를 저장
	fscanf(infile, "%d %d", &N, &M);

	// input file로부터 배열 A에 행열의 셀에 대한 정보 (정수값)을 저장
	for (rr = 0; rr < N*M; rr++) { 
		fscanf(infile, "%d", &A[rr]);
	}
	
	// record program start time
	clock_t start_time = clock();

	// index 1 algorithm 실행
	if (ai == 1) {
		MS = MSS1(A, N, M);
	}
	// index 2 algorithm 실행
	else if (ai == 2) {
		MS = MSS2(A, N, M);
	}
	// index 3 algorithm 실행
	else if (ai == 3) {
		MS = MSS3(A, N, M);
	}

	// record program end time
	clock_t end_time = clock();

	FILE *outfile;

	//"result_"에 input filename을 붙여서 output filename을 완성시켜줌
	strcat(filename2, filename);

	outfile = fopen(filename2, "w+");

	fprintf(outfile, "%s\n", filename);	 // line 1 : input 파일명
	fprintf(outfile, "%d\n", ai);		 // line 2 : algorithm index
	fprintf(outfile, "%d\n%d\n", N, M);  // line 3 & 4 : 행의 수 & 열의 수
	fprintf(outfile, "%d\n", MS);		 // line 5 : MaxSum 값
	fprintf(outfile, "%.3f milliseconds\n", (double)(end_time-start_time)*1000 / CLOCKS_PER_SEC); // line 6 : 걸린시간(ms)
	
	return 0;
}
