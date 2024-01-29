        #include <cstdio>
#include <pthread.h>
#include <iostream>
#include <fstream>
#include <time.h>
#include <vector>

using namespace std;

/* Struct for computeRow arguments */
struct arg_struct1{
 int col1; /* column of the first matrix */
 int col2; /* column of the second matrix = column of the third matrix */
 int* row; /* row of the first matrix */
 vector<int*> mat; /* vector for the second matrix */
 vector<int> out; /* output vector to store the computed row */
};

/* Function to cumpute row of the product matrix */
void* computeRow(void* arg) {
	struct arg_struct1* args = (struct arg_struct1*)arg; /* pointer to struct of arguements */
	int col1 = args->col1;
	int col2 = args->col2;
	int* row = args->row;
	vector<int*> mat= args->mat;
	vector<int> out; /* create an array for the computed row of mat3 (its size = col2 = col3) */
	int element;
	for (int j=0;j<col2;j++){ /* loop on columns of mat2 */
		element = 0;
	 	for (int k=0;k<col1;k++){ /* loop on mat3 elements */
			element += row[k] * mat[k][j]; /* compute an element in the row */
		}
		out.push_back(element); /* add the computed element to the output vector */
	}
	args->out = out; /* save the computed row to the struct of arguments */
   	return NULL;
}

/* Struct for computeElement arguments */
struct arg_struct2{
 int n; /* length of the row and column that are used to compute the element */
 int* row; /* row of the first matrix */
 int* col; /* column of the second matrix */
 int out; /* to store the value of the computed element */
};

/* Function to cumpute elements of the product matrix */
void* computeElement(void* arg) {
	struct arg_struct2* args = (struct arg_struct2*)arg;
	//int n = args->n;
	//int row[n] = args->row;
	//int col[n] = args->col;
	//cout<<n<<endl;
	int result = 0;
 	for (int k=0;k<args->n;k++){ /* loop on mat3 elements */
		result += (args->row[k] * args->col[k]); /* compute the element */
	}
	args->out = result;
   	return NULL;
}

int main(int argc, char *argv[])
{
	ifstream file ("input1-matrix.txt");
	if (file.is_open())
	{
		/* Some Declarations */
		int r1, c1;
		int r2, c2;
		float time1, time2;
		vector<int*> v2;

		/* Reading the two matrices from the file */
		file >> r1 >> c1;
		int mat1[r1][c1];
		for (int i=0;i<r1;i++)
			for(int j=0;j<c1;j++)
				file >> mat1[i][j];
		file >> r2 >> c2;
		int mat2[r2][c2];
		for (int i=0;i<r2;i++){
			for(int j=0;j<c2;j++)
				file >> mat2[i][j];
			v2.push_back(mat2[i]);
		}

		file.close();

		/* Multiplying the two matrices */
		int mat3_1[r1][c2];
		int mat3_2[r1][c2];
		vector<int> row; /* vector to hold the computed row returned */
		if (c1 != r2)
			cerr << "Multipication of Matrix is not Possible !!\n";
		else {
			/* BY ROW */
			clock_t start1 = clock();
			pthread_t thread1[r1]; /* the threads identifier */
			struct arg_struct1 args[r1];
			for (int i=0;i<r1;i++){ /* loop on the rows of mat1 */
				args[i] = {c1, c2, mat1[i], v2}; /* intialize an object of struct and assign its values */
				pthread_create(&thread1[i], NULL, computeRow, &args[i]); /* create the thread */
			}
			for (int i=0; i<r1;i++){
                pthread_join(thread1[i], NULL); /* wait for the thread to exit */
                row = args[i].out; /* get the computed row */
				for (int j=0;j<c2;j++){ /* loop for saving the computed row in the third matrix */
					mat3_1[i][j] = row[j];
				}
			}
			clock_t end1 = clock();
  			time1 = (float)(end1 - start1) / CLOCKS_PER_SEC; /* calculate the time */

			/* BY ELEMENT */
            clock_t start2 = clock();
            int s = r1 * c2;
            pthread_t thread2[s]; /* the threads identifier */
            struct arg_struct2 args2[s];

            for (int i = 0; i < r1; i++) {
                for (int j = 0; j < c2; j++) {
                    // Allocate memory for the col array for each thread
                    int* col = new int[r2];
                    for (int k = 0; k < r2; k++) {
                        col[k] = mat2[k][j];
                    }
                    args2[i * c2 + j] = {c1, mat1[i], col, 0};
                    pthread_create(&thread2[i * c2 + j], NULL, computeElement, &args2[i * c2 + j]);
                }
            }

            for (int i = 0; i < r1; i++) {
                for (int j = 0; j < c2; j++) {
                    pthread_join(thread2[i * c2 + j], NULL);
                    mat3_2[i][j] = args2[i * c2 + j].out;
                    // Free the memory allocated for the col array
                    delete[] args2[i * c2 + j].col;
                }
            }
            clock_t end2 = clock();
            time2 = (float)(end2 - start2) / CLOCKS_PER_SEC;

  			/* Printing the outputs */
  			/* By Row Output */
			cout << "By Row\n";
			for (int i=0;i<r1;i++){
				for(int j=0;j<c2;j++)
					cout << mat3_1[i][j] << " ";
				cout << "\n";
			}
			cout << "Time : " << time1 << " s\n\n";

			/* By Element Output */
			cout << "By Element\n";
			for (int i=0;i<r1;i++){
				for(int j=0;j<c2;j++)
					cout << mat3_2[i][j] << " ";
				cout << "\n";
			}
			cout << "Time : " << time2 << " s\n";
		}


	}
	else
		cerr << "Error Opening the File"<<endl;

	return 0;
}
