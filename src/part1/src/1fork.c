#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <math.h>

int counter = 0;
int h[3]; 
int h2[3];
int final[3]; 
int i =0;
int sig_action_function(int sig, siginfo_t *info, void *ptr)
{
  
  union sigval value = info->si_value;
  //printf("Got a signal from %d. Max: %d\n", info->si_pid, (int*) value.sival_ptr);
  if(sig == 41){
  	printf("Nill is a lil bitch\n");
  }
  if(h2[2] > -1)
  {
  	for(int i = 0; i < 3; i++)
	{
		h[i] = -1;
		h2[i] = -1;
	}
  }
  if(h[2] > -1)
  {
  	h2[i] = (int*) value.sival_ptr;
  }
  else {
  	h[i] = (int*) value.sival_ptr;
  }
  
  i++;
  if(i>2)
  {
  	i = 0;
  }

}

int main()
{
	for(int i = 0; i < 3; i++)
	{
		final[i] = -1;
		h[i] = -1;
		h2[i] = -1;
	}
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
	wait(NULL);
	for(int i = 0; i < 2; i++)
	{ 
		wait(NULL);
		pid_t pid = fork();
		wait(NULL);
		if(pid ==  0){
			wait(NULL);
   			//printf ("Hi I'm process %d and my parent is %d.\n", getpid (), getppid ());
   			newFork2(0, 1, data, data_size, data_per_process, i);
   			wait(NULL);
   			//int maxVal = max_of_array(data_size,data);
   			//value.sival_ptr = maxVal;
   			//sigqueue(getppid(), 40, value);
    		exit(0);
			
   		}
   		else if(pid > 0){
   			

			printf ("Hi(Main Parent) I'm process %d and my parent is %d.\n", getpid (), getppid ());
   		
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

	if(i >= n) { return 0; }
	pid_t pid = fork();
	wait(NULL);
	if(pid > 0){
		wait(NULL);
		pid_t pid2= fork();
		if(pid2 > 0){
			wait(NULL);
			printf ("Hi1 I'm process %d and my parent is %d.\n", getpid (), getppid ());
			if(counter == 0)
   			{	
   				int max1 = max_of_array((counter+2)*data_per_process+1,(counter+3)*data_per_process, data);
   				int min1 = min_of_array((counter+2)*data_per_process+1,(counter+3)*data_per_process, data);
   				int sum1 = sum_of_array((counter+2)*data_per_process+1,(counter+3)*data_per_process, data);

   				printf("Max: %d\n", max1);
   				wait(NULL);
   				for(int k =0; k< 3; k++)
   				{
   					printf("H: %d\n", h[k]);
   					printf("H2: %d\n", h2[k]);
   					// if(h[k] > max1){
   					// 	max1 = h[k];
   					// }
   				}
   				if(max1<h[0])
   					max1 = h[0];
   				

   				if(min1>h[1])
   					min1 = h[1];
   				

   				if(max1<h2[0])
   					max1 = h2[0];

   				if(min1>h2[1])
   					min1 = h2[1];
   				sum1 = sum1 + h[2] + h2[2];

   				maxValue.sival_ptr = max1;
   				minValue.sival_ptr = min1;
   				sumValue.sival_ptr = sum1;
   				sigqueue(getpid(), 41, maxValue);
   				sigqueue(getpid(), 41, minValue);
   				sigqueue(getpid(), 41, sumValue);
   				
   			}
   			else if(counter == 1)
   			{	
   			int max1 = max_of_array((counter+4)*data_per_process+1,((counter+5)*data_per_process)-2 ,data);
   			int min1 = min_of_array((counter+4)*data_per_process+1,((counter+5)*data_per_process)-2 ,data);
   			int sum1 = sum_of_array((counter+4)*data_per_process+1,((counter+5)*data_per_process)-2 ,data);
   			wait(NULL);
   			if(max1<h[0])
   					max1 = h[0];
   				

   				if(min1>h[1])
   					min1 = h[1];
   				

   				if(max1<h2[0])
   					max1 = h2[0];

   				if(min1>h2[1])
   					min1 = h2[1];
   				sum1 = sum1 + h[2] + h2[2];

   				maxValue.sival_ptr = max1;
   				minValue.sival_ptr = min1;
   				sumValue.sival_ptr = sum1;
   				sigqueue(getppid(), 40, maxValue);
   				sigqueue(getppid(), 40, minValue);
   				sigqueue(getppid(), 40, sumValue);
   			}
		}
		else if(pid2 == 0)
		{
			printf ("Hi2 I'm process %d and my parent is %d.\n", getpid (), getppid ());
			if(counter == 0)
   			{	
   				int max1 = max_of_array((counter+1)*data_per_process+1,(counter+2)*data_per_process, data);
   				int min1 =  min_of_array(counter+1*data_per_process+1,(counter+2)*data_per_process, data);
   				int sum1 =  sum_of_array(counter+1*data_per_process+1,(counter+2)*data_per_process, data);
   				printf("Max: %d\n Min: %d\n Sum: %d\n", max1, min1, sum1);
   				maxValue.sival_ptr = max1;
   				minValue.sival_ptr = min1;
   				sumValue.sival_ptr = sum1;
   				sigqueue(getppid(), 40, maxValue);
   				sigqueue(getppid(), 40, minValue);
   				sigqueue(getppid(), 40, sumValue);
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
   			int min1 =  min_of_array(counter*data_per_process,(counter+1)*data_per_process, data);
   			int sum1 =  sum_of_array(counter*data_per_process,(counter+1)*data_per_process, data);
   			printf("Max: %d\n Min: %d\n Sum: %d\n", max1, min1, sum1);
   			maxValue.sival_ptr = max1;
   			minValue.sival_ptr = min1;
   			sumValue.sival_ptr = sum1;
   			sigqueue(getppid(), 40, maxValue);
   			sigqueue(getppid(), 40, minValue);
   			sigqueue(getppid(), 40, sumValue);
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
int32_t sum_of_array(int32_t first, int32_t last, int32_t data[])
{
	int sum = 0;

	for (int i = first; i < last; i++)
	{
		sum += data[i];
	}

	return sum;
}

int32_t min_of_array(int32_t first,int32_t last, int32_t data[])
{
	int min = data[0];

	for (int i = first; i < last; i++)
	{
		if (min > data[i]) { min = data[i]; }
	}

	return min;
}

