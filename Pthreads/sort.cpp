#include <cstdio>
#include <pthread.h>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

/* Struct for mergesort arguements */
struct arg_struct{
 int* a;
 int low;
 int high;
};

/* Function to merge the two halves of the array */
void merge(int a[], int low, int mid, int high)
{
	int nl = mid - low + 1;
	int nr = high - mid;
	int l[nl] , r[nr];
	for(int i=0;i<nl;i++) /* copy the first half of the array in left array */
		l[i] = a[low + i];
	for(int j=0;j<nr;j++) /* copy the second half of the array in right array */
		r[j] = a[mid + 1 + j];
	int i=0, j=0, k=low;
	while(i < nl && j < nr) /* merging the left and right array in the original array */
	{
		if(l[i] < r[j])
		{
			a[k] = l[i];
			i++;
			k++;
		}
		else
		{
			a[k] = r[j];
			j++;
			k++;
		}
	}
	while(i < nl) /* saving the remaning elements of the left array in the original array */
	{
		a[k] = l[i];
		i++;
		k++;
		
	}
	while(j < nr) /* saving the remaning elements of the right array in the original array */
	{
		a[k] = r[j];
		j++;
		k++;
	}
}

/* Function to split the array and merge the two halves recursively */
void* mergesort(void* arg)
{
	struct arg_struct* args = (struct arg_struct*)arg; /* pointer to struct of arguements */
	int low = args->low;
	int high = args->high;
	int* a = args->a;
	if (low<high){
		int mid = (low+high)/2;
		/* Right Half */
		struct arg_struct args1 = {a, low, mid}; /* intialize an object of struct and assign the values for the first half */
		pthread_t thread1; /* thread1 identifiers */
		pthread_create(&thread1, NULL, mergesort, &args1); /* create the thread */
		
		/* Left Half */
		struct arg_struct args2 = {a, mid+1, high}; /* intialize an object of struct and assign the values for the second half */
		pthread_t thread2; /* thread2 identifiers */
		pthread_create(&thread2, NULL, mergesort, &args2); /* create the thread */
		
		pthread_join(thread1, NULL); /* wait for the thread1 to exit */
		pthread_join(thread2, NULL); /* wait for the thread2 to exit */
		
		/* Merge */
		merge(a, low, mid, high);
	}
	return NULL;
}


int main(int argc, char *argv[])
{
	ifstream file (argv[1]);
	if (file.is_open())
	{
		/* Reading from the file */
		int n;
		file >> n;
		int a[n];
		for(int i=0;i<n;i++)
			file >> a[i];
			
		file.close();
		
		/* Calling the Merge Sort */
		struct arg_struct args = {a, 0 , n-1}; /* intialize an object of struct and assign the initial values */
		mergesort(&args);
		
		/* Print the Sorted Array */
		cout << "Sorted array: ";
		for(int i=0;i<n;i++)
			cout << a[i] << " ";
		cout << endl;
	}
	else
		cerr << "Error Opening the File"<<endl; 
	
	return 0;
}
