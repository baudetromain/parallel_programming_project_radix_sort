#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>


void scan_args(int, char**);
int open_output_file(char*);
int open_input_file(char*);
void close_file_descriptors();
void print_usage(char*);
void print_args();


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

	close_file_descriptors();
	return 0;
}

void scan_args(int argc, char** argv)
{
	if (argc < 4)
	{
		print_usage(*argv);
		exit(0);
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
			exit(1);
		}
		if (argc >= 6 && N_THREADS == 0)
		{
			fprintf(stderr, "Error : bad value for [number of threads, otherwise the by default number]. Exiting.\n");
			exit(1);
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
		exit(-1);
	}

	return fd;
}

int open_input_file(char* path)
{
	int fd = open(path, O_RDONLY);

	if (fd < 0)
	{
		fprintf(stderr, "Error: an error occured trying to open the input file (errno=%d). Exiting.\n", errno);
		exit(-1);
	}

	return fd;
}

void close_file_descriptors()
{
	close(OUTPUT_FILE_DESCRIPTOR);

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
		   OUTPUT_FILE_DESCRIPTOR,
		   INPUT_FILE_DESCRIPTOR);
}