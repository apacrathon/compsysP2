#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <math.h>

static int32_t NUM_CHILDREN = 10;
static FILE * output;
/*
 * Functions to read and write to a file.
 */
int32_t write_random_nums(int32_t n, FILE * f);
int32_t read_file(int32_t n, int32_t data[], FILE * f);

/*
 * Arithmetic functions for an array
 */

int32_t sum_of_array(int32_t n, int32_t data[]);
int32_t min_of_array(int32_t n, int32_t data[]);
int32_t max_of_array(int32_t n, int32_t data[]);

/*
 * Functions for each part
 */
void sig_action_function(int sig, siginfo_t *info, void *ptr)
{
  union sigval value = info->si_value;
  printf("Got a signal from %d. The message was: %d\n", info->si_pid, (int*) value.sival_ptr);
}

int32_t PART_D(int32_t data_size, int32_t data[]);

/*
 * Misc. functions.
 */
void print_array(int32_t data_size, int32_t data[]);

int main(void)
{
	srand(time(NULL));

	FILE * f_write = fopen("random.txt", "w");
	FILE * f_read = fopen("random.txt", "r");
	int32_t data_size = 100, data[data_size];

	read_file(data_size, data, f_read);

	PART_D(data_size, data);

	return EXIT_SUCCESS;
}

int32_t PART_D(int32_t data_size, int32_t data[])
{

	struct sigaction act;
   pid_t pid;
   union sigval value;
   int messageText = 69;

   memset (&act, '\0', sizeof(act));
   act.sa_sigaction = sig_action_function;
   act.sa_flags = SA_SIGINFO;

   sigaction(40, &act, 0);  
	printf("\n-------------------Running Part D-------------------\n");
	output = fopen("output_part_d.txt", "w");
	clock_t begin = clock();

	int32_t pipefd[2], data_buffer[data_size], * stats,
	 		NUM_GRANDCHILDREN = 4, NUM_CHILD = 2, total_processes = 6;

	key_t key = ftok("random.txt", 'ww');

	for (int i = 0; i < NUM_CHILD; i++)
	{
		pid_t pid = fork();

		if (pid > 0)
		{
			wait(NULL);
         	exit(0);
		}
		else if (pid == 0)
		{
			value.sival_ptr = messageText;
         	sigqueue(getppid(), 40, value);
			newFork2(0, 1);
			wait(NULL);
			exit(0);
		}
		else { printf("fork error\n"); return -1; }
	
}
	wait(NULL);
	clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	sleep(1);
	//printf("Max = %d\nMin = %d\nSum = %d\nTime: %lf\n", stats[1], stats[0], stats[2], time_spent);

	///output = fopen("output_part_d.txt", "a");
	//fprintf(output, "Max = %d\nMin = %d\nSum = %d\nTime: %lf\n", stats[1], stats[0], stats[2], time_spent);
	fclose(output);

	printf("-------------------Ending Part D-------------------\n");
	return 1;
}

void newFork2(int32_t i, int32_t n)
{

	if(i >= n) { return; }

	pid_t pid = fork();
	if(pid < 0) {}
	else if (pid == 0)
	{
		printf ("Hi I'm process %d and my parent is %d.\n", getpid (), getppid ());
		output = fopen("output_part_d.txt", "a");
		fprintf (output, "Hi I'm process %d and my parent is %d.\n", getpid (), getppid ());
		fclose(output);
	}
	else if (pid > 0)
	{
		printf ("Hi I'm process %d and my parent is %d.\n", getpid (), getppid ());
		output = fopen("output_part_d.txt", "a");
		fprintf (output, "Hi I'm process %d and my parent is %d.\n", getpid (), getppid ());
		fclose(output);

		pid_t pid2 = fork();
		if (pid2 == 0)
		{
			printf ("Hi I'm process %d and my parent is %d.\n", getpid (), getppid ());
			output = fopen("output_part_d.txt", "a");
			fprintf (output, "Hi I'm process %d and my parent is %d.\n", getpid (), getppid ());
			fclose(output);
		}
	}
	else { printf("fork error\n"); return; }
}

int32_t write_random_nums(int32_t n, FILE * f)
{

	int32_t result = 0;

	if (f == NULL)
	{
	    printf("Error opening file!\n");
	    exit(1);
	}

	for (int count = 0; count < n; count++)
	{
		fprintf(f, "%d\n", (rand() % 1000));
	}

	result = fclose(f);

	return result;
}

int32_t read_file(int32_t n, int32_t data[], FILE * f)
{
	int32_t result = 0;

	if (f == NULL)
	{
	    printf("Error opening file!\n");
	    return 1;
	}

	for (int i = 0; i < n; i++)
	{
		fscanf(f, "%d", &data[i]);
	}

	result = fclose(f);

	return result;
}

int32_t sum_of_array(int32_t n, int32_t data[])
{
	int sum = 0;

	for (int i = 0; i < n; i++)
	{
		sum += data[i];
	}

	return sum;
}

int32_t min_of_array(int32_t n, int32_t data[])
{
	int min = data[0];

	for (int i = 1; i < n; i++)
	{
		if (min > data[i]) { min = data[i]; }
	}

	return min;
}

int32_t max_of_array(int32_t n, int32_t data[])
{
	int min = data[0];

	for (int i = 1; i < n; i++)
	{
		if (min < data[i]) { min = data[i]; }
	}

	return min;
}

void print_array(int32_t n, int32_t data[])
{
	for (int32_t i = 0; i < n; i++)
	{
		printf("%d\n", data[i]);
	}
}
