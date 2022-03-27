#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <math.h>

typedef struct array
{
	int* tab;
	int size;
} Array;

Array* scan(Array*);
Array* suffix(Array*);
Array* prefix(Array*);
Array* old_prefix(Array*);
Array* allocate_array(int);
void free_array(Array*);
Array* allocate_array_with_linspace(int, int);
Array* allocate_example_array();
void fill_array_with_random_values(Array*, int);
void fill_array_with_zeros(Array*);
void print_array(Array*);

Array* radix_sort(Array*);
Array* split(Array*, Array*);
Array* permute(Array*, Array*);
Array* minus(Array*, int);
Array* not(Array*);
int* base2(int);
Array* initialize_array();
void scan_args(int, char**);
int open_output_file(char*);
int open_input_file(char*);
void close_file_descriptors();
void print_usage(char*);
void print_args();
void my_exit(int);


int N = 0;
long ARRAY_SIZE = 0;
char* OUTPUT_FILE_NAME = "";
char* INPUT_FILE_NAME = "";
int N_THREADS = 0;

int INPUT_FILE_DESCRIPTOR = -1;
int OUTPUT_FILE_DESCRIPTOR = -1;


int main(int argc, char** argv)
{
	scan_args(argc, argv);
	print_args();

	Array* array = initialize_array();
	print_array(array);
	Array* sorted = radix_sort(array);
	print_array(sorted);

	close_file_descriptors();
	free_array(array);
	return 0;
}

Array* scan(Array* array)
{
	int m = ceil(log2(array->size));
	int padded_size = pow(2, m);

	Array* A_array = allocate_array(padded_size * 2 - 1);
	Array* B_array = allocate_array(padded_size * 2 - 1);

	fill_array_with_zeros(A_array);
	fill_array_with_zeros(B_array);

	int* A = A_array->tab;
	int* B = B_array->tab;

	for (int i = 0; i < array->size; i++)
	{
		*(A + padded_size - 1 + i) = *(array->tab + i);
	}

	for (int l = m-1; l >= 0; l--) // pour toute ligne de l'arbre
	{
		int upper_bound = pow(2, l+1) - 1;

#pragma omp parallel for
		for (int j = pow(2, l) - 1; j < upper_bound; j++) // pour tout noeud de cette ligne
		{
			*(A + j) = *(A + (j * 2) + 1) + *(A + (j * 2) + 2); // parent = fils gauche + fils droit
		}
	}

	*(B + 0) = 0;

	for (int l = 1; l <= m; l++)
	{
		int upper_bound = pow(2, l+1) - 1;

#pragma omp parallel for
		for (int j = pow(2, l) - 1; j < upper_bound; j++)
		{
			if (j % 2 != 0)
			{
				*(B + j) = *(B + (j - 1) / 2);
			}
			else
			{
				*(B + j) = *(B + (j - 2) / 2 ) + *(A + j - 1);
			}
		}
	}

	Array* scanned = allocate_array(array->size);

	int upper_bound = array->size - 1;

#pragma omp parallel for
	for(int j = 0; j < upper_bound; j++)
	{
		*(scanned->tab + j + 1) = *(A + padded_size - 1 + j) + *(B + padded_size - 1 + j);
	}

	free_array(A_array);
	free_array(B_array);

	return scanned;
}

Array* suffix(Array* array)
{
	int m = ceil(log2(array->size));
	int padded_size = pow(2, m);

	Array* A_array = allocate_array(padded_size * 2 - 1);
	Array* B_array = allocate_array(padded_size * 2 - 1);

	fill_array_with_zeros(A_array);
	fill_array_with_zeros(B_array);

	int* A = A_array->tab;
	int* B = B_array->tab;

	for (int i = 0; i < array->size; i++)
	{
		*(A + padded_size - 1 + i) = *(array->tab + i);
	}

	for (int l = m-1; l >= 0; l--) // pour toute ligne de l'arbre
	{
		int upper_bound = pow(2, l+1) - 1;

#pragma omp parallel for
		for (int j = pow(2, l) - 1; j < upper_bound; j++) // pour tout noeud de cette ligne
		{
			*(A + j) = *(A + (j * 2) + 1) + *(A + (j * 2) + 2); // parent = fils gauche + fils droit
		}
	}

	*(B + 0) = 0;

	for (int l = 1; l <= m; l++)
	{
		int upper_bound = pow(2, l+1) - 1;

#pragma omp parallel for
		for (int j = pow(2, l) - 1; j < upper_bound; j++)
		{
			if (j % 2 != 0)
			{
				*(B + j) = *(B + (j - 1) / 2) + *(A + j + 1);
			}
			else
			{
				*(B + j) = *(B + (j - 2) / 2);
			}
		}
	}

	Array* suffixed = allocate_array(array->size);

	int upper_bound = array->size;

#pragma omp parallel for
	for(int j = 0; j < upper_bound; j++)
	{
		*(suffixed->tab + j) = *(A + padded_size - 1 + j) + *(B + padded_size - 1 + j);
	}

	free_array(A_array);
	free_array(B_array);

	return suffixed;
}

Array* prefix(Array* array)
{
	int m = ceil(log2(array->size));
	int padded_size = pow(2, m);

	Array* A_array = allocate_array(padded_size * 2 - 1);
	Array* B_array = allocate_array(padded_size * 2 - 1);

	fill_array_with_zeros(A_array);
	fill_array_with_zeros(B_array);

	int* A = A_array->tab;
	int* B = B_array->tab;

	for (int i = 0; i < array->size; i++)
	{
		*(A + padded_size - 1 + i) = *(array->tab + i);
	}

	for (int l = m-1; l >= 0; l--) // pour toute ligne de l'arbre
	{
		int upper_bound = pow(2, l+1) - 1;

#pragma omp parallel for
		for (int j = pow(2, l) - 1; j < upper_bound; j++) // pour tout noeud de cette ligne
		{
			*(A + j) = *(A + (j * 2) + 1) + *(A + (j * 2) + 2); // parent = fils gauche + fils droit
		}
	}

	*(B + 0) = 0;

	for (int l = 1; l <= m; l++)
	{
		int upper_bound = pow(2, l+1) - 1;

#pragma omp parallel for
		for (int j = pow(2, l) - 1; j < upper_bound; j++)
		{
			if (j % 2 != 0)
			{
				*(B + j) = *(B + (j - 1) / 2);
			}
			else
			{
				*(B + j) = *(B + (j - 2) / 2 ) + *(A + j - 1);
			}
		}
	}

	Array* prefixed = allocate_array(array->size);

	int upper_bound = array->size;

#pragma omp parallel for
	for(int j = 0; j < upper_bound; j++)
	{
		*(prefixed->tab + j) = *(A + padded_size - 1 + j) + *(B + padded_size - 1 + j);
	}

	free_array(A_array);
	free_array(B_array);

	return prefixed;
}

Array* allocate_array(int size)
{
	Array* array = (Array*) malloc(sizeof(Array));
	array->size = size;
	array->tab = (int*) malloc(sizeof(int) * size);

	return array;
}

void free_array(Array* array)
{
	free(array->tab);
	free(array);
}

Array* allocate_array_with_linspace(int size, int begin)
{
	Array* array = allocate_array(size);

	for(int i = 0; i < array->size; i++)
	{
		*(array->tab + i) = i + begin;
	}

	return array;
}

Array* allocate_example_array()
{
	Array* array = allocate_array(10);
	int example_array[10] = {5, 7, 3, 1, 4, 2, 7, 2, 6, 0};

	for(int i = 0; i < 10; i++)
	{
		*(array->tab + i) = example_array[i];
	}

	return array;
}

void fill_array_with_random_values(Array* array, int upper_bound)
{
	srand(time(0));
	for(int i = 0; i < array->size; i++)
	{
		*(array->tab + i) = rand() % upper_bound;
	}
}

void fill_array_with_zeros(Array* array)
{
	for (int i = 0; i < array->size; i++)
	{
		*(array->tab + i) = 0;
	}
}

void print_array(Array* array)
{
	for (int i = 0; i < array->size; i++)
	{
		printf("%d ", *(array->tab + i));
	}
	printf("\n");
}

Array* radix_sort(Array* array)
{
	Array* current = array;

	int** bits = malloc(sizeof(char*) * ARRAY_SIZE);

	for (int i = floor(log2(N)); i >= 0; i--)
	{
		Array* bit = allocate_array(ARRAY_SIZE);

		for(int j = 0; j < ARRAY_SIZE; j++)
		{
			int* i_base2 = base2(*(current->tab + j));
			*(bits + j) = i_base2;
		}

		for (int j = 0; j < ARRAY_SIZE; j++)
		{
			*(bit->tab + j) = *(*(bits + j) + i);
		}

		current = split(current, bit);
		free_array(bit);
//		for(int j = 0; j < ARRAY_SIZE; j++)
//		{
//			free(*(bits + j));
//		}
	}

	for(int i = 0; i < ARRAY_SIZE; i++)
	{
		free(*(bits + i));
	}

	return current;
}

Array* split(Array* array, Array* flags)
{
	Array* notFlags = not(flags);
	Array* iDown = scan(notFlags);
	Array* suffixed = suffix(flags);
	Array* iUp = minus(suffixed, ARRAY_SIZE);

	Array* indexes = allocate_array(ARRAY_SIZE);

	for (int i = 0; i < ARRAY_SIZE; i++)
	{
		if (*(flags->tab + i) == 1)
		{
			*(indexes->tab + i) = *(iUp->tab + i);
		}
		else
		{
			*(indexes->tab + i) = *(iDown->tab + i);
		}
	}

	printf("indexes: ");
	print_array(indexes);

	return permute(array, indexes);
}

Array* permute(Array* array, Array* indexes)
{
	Array* permuted = allocate_array(ARRAY_SIZE);

	for (int i = 0; i < ARRAY_SIZE; i++)
	{
		*(permuted->tab + *(indexes->tab + i)) = *(array->tab + i);
	}

	return permuted;
}

Array* minus(Array* array, int n)
{
	Array* minused = allocate_array(ARRAY_SIZE);

	for(int i = 0; i < ARRAY_SIZE; i++)
	{
		*(minused->tab + i) = ARRAY_SIZE - *(array->tab + i);
	}

	return minused;
}

Array* not(Array* flags)
{
	Array* array = allocate_array(ARRAY_SIZE);

	for(int i = 0; i < ARRAY_SIZE; i++)
	{
		*(array->tab + i) = (*(flags->tab + i) == 1) ? 0 : 1;
	}

	return array;
}

int* base2(int n)
{
	if (n == 0)
	{
		int* n_base2 = (int*) malloc(sizeof(int));
		*n_base2 = 0;
		return n_base2;
	}

	int str_len = floor(log2(N)) + 1;
	int* n_base2 = (int*) malloc(sizeof(int) * str_len);
	int shift = 0;

	for(int i = floor(log2(N)); i >= 0; i--)
	{
		if (n >= pow(2, i))
		{
			*(n_base2 + shift) = 1;
			n -= pow(2, i);
		}
		else
		{
			*(n_base2 + shift) = 0;
		}

		shift++;
	}

	return n_base2;
}

Array* initialize_array()
{
	Array* array = allocate_array(ARRAY_SIZE);

	if (INPUT_FILE_DESCRIPTOR > 0)
	{
		char read_char;
		int number = 0;
		int spaces = 0;
		int* ptr = array->tab;

		while (read(INPUT_FILE_DESCRIPTOR, (void*) &read_char, 1) != 0)
		{
			if (!spaces)
			{
				if (read_char == ' ')
				{
					*(ptr++) = number;
					spaces = 1;
					number = 0;
				}
				else
				{
					char temp[] = {read_char, '\0'};
					number = number * 10 + atoi(temp);
				}
			}
			else
			{
				if (read_char != ' ')
				{
					char temp[] = {read_char, '\0'};
					number = number * 10 + atoi(temp);
					spaces = 0;
				}
			}
		}

		*(ptr++) = number;
	}
	else
	{
		srand(time(0));

		for (int i = 0; i < ARRAY_SIZE; i++)
		{
			*(array->tab + i) = rand() % N;
		}
	}

	return array;
}

void scan_args(int argc, char** argv)
{
	if (argc < 4)
	{
		print_usage(*argv);
		my_exit(0);
	}
	else
	{
		N = atoi(*(argv + 1));
		ARRAY_SIZE = atol(*(argv + 2));

		if (argc == 4)
		{
			OUTPUT_FILE_NAME = *(argv + 3);
		}
		else
		{
			INPUT_FILE_NAME = *(argv + 3);
			OUTPUT_FILE_NAME = *(argv + 4);

			INPUT_FILE_DESCRIPTOR = open_input_file(INPUT_FILE_NAME);
		}

		OUTPUT_FILE_DESCRIPTOR = open_output_file(OUTPUT_FILE_NAME);

		if (argc >= 6)
		{
			N_THREADS = atoi(*(argv + 5));
		}

		if (argc > 6)
		{
			printf("Ignoring all arguments after the 5th one (%s).\n", *(argv + 5));
		}

		if (N == 0)
		{
			fprintf(stderr, "Error : bad value for <value of N>. Exiting.\n");
			exit(1);
		}
		if (ARRAY_SIZE == 0)
		{
			fprintf(stderr, "Error : bad value for <size of the array>. Exiting.\n");
			my_exit(1);
		}
		if (argc >= 6 && N_THREADS == 0)
		{
			fprintf(stderr, "Error : bad value for [number of threads, otherwise the by default number]. Exiting.\n");
			my_exit(1);
		}
	}
}

int open_output_file(char* path)
{
	int fd = open(path,
				  O_CREAT | O_TRUNC | O_WRONLY,
				  S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

	if (fd < 0)
	{
		fprintf(stderr, "Error: an error occured trying to open or create the output file (errno=%d). Exiting.\n", errno);
		my_exit(-1);
	}

	return fd;
}

int open_input_file(char* path)
{
	int fd = open(path, O_RDONLY);

	if (fd < 0)
	{
		fprintf(stderr, "Error: an error occured trying to open the input file (errno=%d). Exiting.\n", errno);
		my_exit(-1);
	}

	return fd;
}

void close_file_descriptors()
{
	if (OUTPUT_FILE_DESCRIPTOR > 0)
	{
		close(OUTPUT_FILE_DESCRIPTOR);
	}

	if(INPUT_FILE_DESCRIPTOR > 0)
	{
		close(INPUT_FILE_DESCRIPTOR);
	}
}

void print_usage(char* cmd)
{
	printf("Usage: %s <value of N> <size of the array> [<input file name>] <output file name> [number of threads, otherwise the by default number]\n", cmd);
}

void print_args()
{
	printf("N = %d\nARRAY_SIZE = %ld\nOUTPUT_FILE_NAME = %s\nINPUT_FILE_NAME = %s\nN_THREADS = %d\nINPUT_FILE_DESCRIPTOR = %d\nOUTPUT_FILE_DESCRIPTOR = %d\n",
		   N,
		   ARRAY_SIZE,
		   OUTPUT_FILE_NAME,
		   INPUT_FILE_NAME,
		   N_THREADS,
		   INPUT_FILE_DESCRIPTOR,
		   OUTPUT_FILE_DESCRIPTOR);
}

void my_exit(int code)
{
	close_file_descriptors();
	exit(code);
}