#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <math.h>

int counter = 0;

int sig_action_function(int sig, siginfo_t *info, void *ptr)
{
  union sigval value = info->si_value;
  printf("Got a signal from %d. Max: %d\n", info->si_pid, (int*) value.sival_ptr);
}

int main()
{
	srand(time(NULL));

	//FILE * f_write = fopen("random.txt", "w");
	FILE * f_read = fopen("random.txt", "r");
	int32_t data_size = 100, data[data_size], total_processes = 6;

	// printf("\nWriting to file...\n");
	// write_random_nums(data_size, f_write);
	// printf("Successfully wrote to file...\n");

	printf("Reading from file...\n");
	read_file(data_size, data, f_read);
	printf("Successfully read from file...\n");

	struct sigaction max;
	memset (&max, '\0', sizeof(max));
	max.sa_sigaction = sig_action_function;
	max.sa_flags = SA_SIGINFO;
	union sigval maxValue;
	sigaction(40, &max, 0);

	struct sigaction min;
	memset(&min, '\0', sizeof(min));
	min.sa_sigaction = sig_action_function;
	min.sa_flags = SA_SIGINFO;
	union sigval minValue;
	sigaction(41, &min, 0);

	struct sigaction sum;
	memset(&sum, '\0', sizeof(sum));
	sum.sa_sigaction = sig_action_function;
	sum.sa_flags = SA_SIGINFO;
	union sigval sumValue;
	sigaction(42, &sum, 0);

	int data_per_process;
	if (data_size <= 10)
	{
		data_per_process = 1;
	}
	else
	{
		data_per_process = ceil((double)data_size / total_processes);
	}
	
	for(int i = 0; i < 2; i++)
	{ 
		pid_t pid = fork();

		if(pid > 0){
			wait(NULL);
   		
   		}
   		else if(pid == 0){

   			//printf ("Hi I'm process %d and my parent is %d.\n", getpid (), getppid ());
   			newFork2(0, 1, data, data_size, data_per_process, i);
   			wait(NULL);
   			//int maxVal = max_of_array(data_size,data);
   			//value.sival_ptr = maxVal;
   			//sigqueue(getppid(), 40, value);
    		exit(0);
    	}
   		else{
   			printf("Fork Failed\n");
   			return 0;
   		}
   	}	

}

void newFork2(int32_t i, int32_t n, int32_t data[], int32_t data_size, int data_per_process, int counter)
{
	struct sigaction max;
	memset (&max, '\0', sizeof(max));
	max.sa_sigaction = sig_action_function;
	max.sa_flags = SA_SIGINFO;
	union sigval maxValue;
	sigaction(40, &max, 0);

	if(i >= n) { return 0; }
	pid_t pid = fork();

	if(pid > 0){
		wait(NULL);
		pid_t pid2= fork();
		if(pid2 > 0){
			wait(NULL);
			printf ("Hi1 I'm process %d and my parent is %d.\n", getpid (), getppid ());
			if(counter == 0)
   			{	
   				int max1 = max_of_array((counter+2)*data_per_process+1,(counter+3)*data_per_process, data);
   				printf("Max: %d\n", max1);
   			}
   			else if(counter == 1)
   			{	
   			int max1 = max_of_array((counter+4)*data_per_process+1,((counter+5)*data_per_process)-2 ,data);
   			printf("Max: %d\n", max1);
   			}
		}
		else if(pid2 == 0)
		{
			printf ("Hi2 I'm process %d and my parent is %d.\n", getpid (), getppid ());
			if(counter == 0)
   			{	
   				int max1 = max_of_array((counter+1)*data_per_process+1,(counter+2)*data_per_process, data);
   				printf("Max: %d\n", max1);
   			}
   			else if(counter == 1)
   			{	
   			int max1 = max_of_array((counter+3)*data_per_process+1,(counter+4)*data_per_process, data);
   			printf("Max: %d\n", max1);
   			}
		}
		
   	}
   	else if(pid == 0){
   		printf ("Hi3 I'm process %d and my parent is %d.\n", getpid (), getppid ());
   		if(counter == 0)
   		{	
   			int max1 = max_of_array(counter*data_per_process,(counter+1)*data_per_process, data);
   			printf("Max: %d\n", max1);
   			maxValue.sival_ptr = max1;
   			sigqueue(getppid(), 40, maxValue);
   		}
      	else if(counter == 1)
   		{	
   			int max1 = max_of_array((counter+2)*data_per_process+1,(counter+3)*data_per_process, data);
   			printf("Max: %d\n", max1);
   		}
   		

    }
   	else{
   		printf("Fork Failed\n");
   		return 0;
   	}
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
		fprintf(f, "%d\n", (rand() % 100));
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
int32_t max_of_array(int32_t first, int32_t last, int32_t data[])
{
	int max = data[first];

	for (int i = first; i < last; i++)
	{
		if (max < data[i]) { max = data[i]; }
	}

	return max;
}

