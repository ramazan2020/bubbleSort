#include <iostream>
#include "yardimci.hpp"
#include <thread>
#include <vector>
#include <pthread.h>



const int MAX = 8 * 100;


pthread_barrier_t barrier;

struct ThreadArgs {
	int* matrix;
	int start;
	int end;
};

struct ThreadData {
	int* matrix;
	int start;
	//int end;
};
//void bubbleSort(int matrix[],  int start, int end ) {
//	for (int i = start; i < end - 1; ++i) {
//		for (int j = start; j < end - i - 1; ++j) {
//			if (matrix[j] > matrix[j + 1]) {
//				// Swap i�lemi
//			/*	int temp = matrix[j];
//				matrix[j] = matrix[j + 1];
//				matrix[j + 1] = temp;*/
//
//				std::swap(matrix[j], matrix[j + 1]);
//
//			}
//		}
//	}
//}


//// Swap function
//void swap(int* a, int* b) {
//	int temp = *a;
//	*a = *b;
//	*b = temp;
//}
void swap(int  matrix[], int j)
{
	int temp = matrix[j];
	matrix[j] = matrix[j + 1];
	matrix[j + 1] = temp;
}

// Bubble sort algorithm for a range of elements
void* bubbleSort(int* matrix, int start, int end) {
	for (int i = start; i < end - 1; ++i) {
		for (int j = start; j < end - i - 1; ++j) {
			if (matrix[j] > matrix[j + 1]) {
				//std::swap(matrix[j], matrix[j + 1]);
				swap(matrix, j);

			}
		}
	}
	return  nullptr;

}
void* bubbleSort_pthread(void* arguments) {
	struct ThreadArgs* args = (struct ThreadArgs*)arguments;
	int* matrix = args->matrix;
	int start = args->start;
	int end = args->end;


	return bubbleSort(matrix, start, end);

}


void yazdir(int matrix[], int size) {
	for (int i = 0; i < size; ++i) {
		std::cout << matrix[i] << " ";
	}
	std::cout << std::endl;
}

void yazdirvektor(std::vector<int> arr) {
	//std::cout << "S�ralanm�� dizi: ";
	for (const auto& num : arr) {
		std::cout << num << " ";
	}
	std::cout << std::endl;
}
void init(int matrix[]) {
	// Generating random values in matA and matB
	for (int i = 0; i < MAX; i++)
		matrix[i] = rand() % 1000;



}
void paralel_calistir_pthread(int matrix[]) {
	TIMERSTART(paralel_calistir_pthread)
		const int num_threads = 4;
	pthread_t threads[num_threads];
	struct ThreadArgs thread_args[num_threads];

	int elements_per_thread = MAX / num_threads;

	for (int i = 0; i < num_threads; ++i) {
		thread_args[i].matrix = matrix;
		thread_args[i].start = i * elements_per_thread;
		thread_args[i].end = (i + 1) * elements_per_thread;

		pthread_create(&threads[i], NULL, bubbleSort_pthread, (void*)&thread_args[i]);
	}

	for (int i = 0; i < num_threads; ++i) {
		pthread_join(threads[i], NULL);
	}

	//pthread_exit(NULL);


	// Birleştirilmiş parçaları sırala
	for (int i = 1; i < MAX; ++i) {
		int key = matrix[i];
		int j = i - 1;

		while (j >= 0 && matrix[j] > key) {
			matrix[j + 1] = matrix[j];
			--j;
		}

		matrix[j + 1] = key;
	}

	TIMERSTOP(paralel_calistir_pthread)

}
void paralel_calistir_thread(int matrix[]) {
	TIMERSTART(paralel_calistir_thread)
		const int num_threads = 4;

	std::vector<std::thread> threads;
	int elements_per_thread = MAX / num_threads;

	for (int i = 0; i < num_threads; ++i) {
		int start = i * elements_per_thread;
		int end = (i + 1) * elements_per_thread;
		threads.emplace_back(bubbleSort, matrix, start, end);
	}

	for (auto& thread : threads) {
		thread.join();
	}


	// Birleştirilmiş parçaları sırala
	for (int i = 1; i < MAX; ++i) {
		int key = matrix[i];
		int j = i - 1;

		while (j >= 0 && matrix[j] > key) {
			matrix[j + 1] = matrix[j];
			--j;
		}

		matrix[j + 1] = key;
	}

	TIMERSTOP(paralel_calistir_thread)

}
void paralel_calistir_openmp(int matrix[]) {

	TIMERSTART(bubbleSort_paralel_omp)
#pragma omp parallel
	{
		for (int i = 0; i < MAX - 1; ++i) {
#pragma omp for
			for (int j = 0; j < MAX - i - 1; ++j) {
				if (matrix[j] > matrix[j + 1]) {
					// Swap işlemi
					swap(matrix, j);
					//std::swap(matrix[j], matrix[j + 1]);
				}
			}
		}
	}
	TIMERSTOP(bubbleSort_paralel_omp)
}
void seri_calistir(int matrix[]) {
	TIMERSTART(bubbleSort_seri)

		bubbleSort(matrix, 0, MAX);

	TIMERSTOP(bubbleSort_seri)
}

int main() {

	int matrix[MAX];
	init(matrix);


	std::cout << "Dizi (önce): ";
	yazdir(matrix, MAX);




	int copy1[MAX];
	std::copy(&matrix[0], &matrix[0] + MAX, &copy1[0]);
	seri_calistir(copy1);
	std::cout << "Dizi (Sonra): ";
	yazdir(copy1, MAX);



	int copy3[MAX];
	std::copy(&matrix[0], &matrix[0] + MAX, &copy3[0]);
	paralel_calistir_openmp(copy3);

	std::cout << "Dizi (Sonra): ";
	yazdir(copy3, MAX);


	int copy4[MAX];
	std::copy(&matrix[0], &matrix[0] + MAX, &copy4[0]);
	paralel_calistir_pthread(copy4);
	std::cout << "Dizi (Sonra): ";
	yazdir(copy4, MAX);


	int copy5[MAX];
	std::copy(&matrix[0], &matrix[0] + MAX, &copy5[0]);
	paralel_calistir_thread(copy5);
	std::cout << "Dizi (Sonra): ";
	yazdir(copy5, MAX);

	return NULL;
}