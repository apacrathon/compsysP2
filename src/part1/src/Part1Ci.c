#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <math.h>

int h[3]; 
int h2[3];
int i =0;
int terminatedpid;
//signal handler
int sig_action_function(int sig, siginfo_t *info, void *ptr)
{
  
  if(sig == SIGINT)
  {

    printf("\nProcess %d confirms signal(%d) interrupt and will now terminate \n",getpid(),sig);
    terminatedpid = getpid();
      exit(0);
  }
  else if(sig == 40)
{
  union sigval value = info->si_value;

 if(h2[2] > -1)
  {
    for(int i = 0; i < 3; i++)
    {
        h[i] = -1;
        h2[i] = -1;
    }
  }
  else if(h[2] > -1)
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
}

int max_function(int array[], int first, int last)
{
    int maximum = array[first];
    for(int i = first;i<=last;i++)
    {
        if(array[i]>maximum)
            maximum = array[i];
    }
    return maximum;
}

int min_function(int array[], int first, int last)
{
    int minimum = array[first];
    for(int i = first;i<=last;i++)
    {
        if(array[i]<minimum)
            minimum = array[i];
    }
    return minimum;
}

int sum_function(int array[], int first, int last)
{
    int summation = 0;
    for(int i=first;i<=last;i++)
    {
        summation += array[i];
    }
    return summation;
}

int partd(int array[],int index1, int index2)
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

    
    int beginning=index1;
    int end=index2;
    int gap= ceil((end-beginning)/6);
    int child[6];
    pid_t pid=getpid();
    //creating 6 pipes to communicate with parent/child
    int maxd;
    int mind;
    int sumd;
    // spawning the children
        pid=fork();
        if (pid==0)
      {
            child[0]=getpid();
            pid=fork();
            if (pid==0)
            {
                child[2]=getpid();
                
            }
            else
            {
             pid=fork();
             if (pid==0)
                {
                child[3]=getpid();
                
                }
            }
        }
        else if (pid>0)
            {
                pid=fork();
                if (pid==0)
                {
                    child[1]=getpid();
                    pid=fork();
                    if (pid==0)
                    {
                        child[4]=getpid();        
                 }
                 else
               {pid=fork();
                 if (pid==0)
                {
                 child[5]=getpid();
                }    

                }
          }
      }

        //child 1
     if (getpid()==child[0])
    { 
        

        wait(NULL);
        wait(NULL);
        printf("Hi I'm process %d and my parent is %d\n",getpid(),getppid());
        maxd = max_function(array,beginning,gap);
        mind = min_function(array,beginning,gap);
        sumd = sum_function(array,beginning,gap);

        //compare with child 3 

        if(maxd<h[0])
            maxd = h[0];
        if(mind>h[1])
            mind = h[1];
        sumd = sumd + h[2];
        
        //compare new results with child 4
        if(maxd<h2[0])
            maxd = h2[0];
        if(mind>h2[1])
            mind = h2[1];
        sumd = sumd + h2[2];
    
                   
        //write final results to parent
        if(getppid() != 1)
        {
       	 	maxValue.sival_ptr =(int*) maxd;
	        minValue.sival_ptr =(int*) mind;
        	sumValue.sival_ptr =(int*) sumd;
        	sigqueue(getppid(), 40, maxValue);
        	sigqueue(getppid(), 40, minValue);
        	sigqueue(getppid(), 40, sumValue);
        }
        else if(getppid() == 1)
        {
            printf("My parent %d is terminated, I will terminate now too\n",getppid());
            exit(0);
        }
    }
    //child 2
    else if(getpid()==child[1])
    {
        
        wait(NULL);
        wait(NULL);
        printf("Hi I'm process %d and my parent is %d\n",getpid(),getppid());
        maxd = max_function(array,gap+1,2*gap);
        mind = min_function(array,gap+1,2*gap);
        sumd = sum_function(array,gap+1,2*gap);

        
        //compare values with child 5
        if(maxd<h[0])
            maxd = h[0];
        if(mind>h[1])
            mind = h[1];
        sumd = sumd + h[2];
        
        //compare new results with child 6
        if(maxd<h2[0])
            maxd = h2[0];
        if(mind>h2[1])
            mind = h2[1];
        sumd = sumd + h2[2];
        if(getppid() != 1)
        {
        	maxValue.sival_ptr =(int*) maxd;
        	minValue.sival_ptr =(int*) mind;
        	sumValue.sival_ptr =(int*) sumd;
        	sigqueue(getppid(), 40, maxValue);
        	sigqueue(getppid(), 40, minValue);
        	sigqueue(getppid(), 40, sumValue);
        }
        
    }
    //child 3
    else if (getpid()==child[2])
    {
        signal(SIGINT, SIG_IGN);
        sleep(1);
        printf("press Ctrl-C! 5 Seconds Remaining\n");
        sleep(5);
        
        maxd = max_function(array,2*gap+1, 3*gap);
        mind = min_function(array,2*gap+1, 3*gap);
        sumd = sum_function(array,2*gap+1, 3*gap);
        //write results to child 1
        if(getppid() == child[0])
        {
        printf("Hi I'm process %d and my parent is %d\n",getpid(),getppid());
	        maxValue.sival_ptr = (int*) maxd;
        	minValue.sival_ptr = (int*) mind;
        	sumValue.sival_ptr = (int*) sumd;
        	sigqueue(getppid(), 40, maxValue);
        	sigqueue(getppid(), 40, minValue);
        	sigqueue(getppid(), 40, sumValue);
        }
        else
        {
            printf("Hi I'm process %d and My parent %d is dead, I will now terminate\n",getpid(),child[0]);
            exit(0);
        }
    }
    //child 4
    else if (getpid()==child[3])
    {
        signal(SIGINT, SIG_IGN);
        sleep(5);   
        
        maxd = max_function(array,3*gap+1,4*gap);
        mind = min_function(array,3*gap+1,4*gap);
        sumd = sum_function(array,3*gap+1,4*gap);
        //write results to child 1
        if(getppid() == child[0])
        {
        	printf("Hi I'm process %d and my parent is %d\n",getpid(),getppid());
        	maxValue.sival_ptr = (int*) maxd;
        	minValue.sival_ptr = (int*) mind;
        	sumValue.sival_ptr = (int*) sumd;
        	sigqueue(getppid(), 40, maxValue);
        	sigqueue(getppid(), 40, minValue);
        	sigqueue(getppid(), 40, sumValue);
        }
        else
        {
            printf("Hi I'm process %d and My parent %d is dead, I will now terminate\n",getpid(),child[0]);
            exit(0);
        }
    }
    //child 5
    else if (getpid()==child[4])
    {
        printf("Hi I'm process %d and my parent is %d\n",getpid(),getppid());
        maxd = max_function(array,4*gap+1,5*gap);
        mind = min_function(array,4*gap+1,5*gap);
        sumd = sum_function(array,4*gap+1,5*gap);
        //write results to child 2
        if(getppid() != 1)
        {
        	maxValue.sival_ptr = (int*) maxd;
        	minValue.sival_ptr = (int*) mind;
        	sumValue.sival_ptr = (int*) sumd;
        	sigqueue(getppid(), 40, maxValue);
        	sigqueue(getppid(), 40, minValue);
        	sigqueue(getppid(), 40, sumValue);
        }
        else if(getppid() == 1)
        {
            printf("My parent %d is terminated, I will terminate now too\n",getppid());
            exit(0);
        }   
    }
    //child 6
    else if (getpid()==child[5])
    {
        printf("Hi I'm process %d and my parent is %d\n",getpid(),getppid());
        maxd = max_function(array,5*gap+1,end);
        mind = min_function(array,5*gap+1,end);
        sumd = sum_function(array,5*gap+1,end);
        //write results to child 2
        if(getppid != 1)
        {
        	maxValue.sival_ptr = (int*) maxd;
        	minValue.sival_ptr = (int*) mind;
        	sumValue.sival_ptr = (int*) sumd;
        	sigqueue(getppid(), 40, maxValue);
        	sigqueue(getppid(), 40, minValue);
        	sigqueue(getppid(), 40, sumValue);
        }
        else if(getppid() == 1)
        {
            printf("My parent %d is terminated, I will terminate now too\n",getppid());
            exit(0);
        }
    }
    //Parent
    else
    {
//waits for children
signal(SIGINT, SIG_IGN);
    for (int child=0; child<4;child++)
    {
        int status;
        pid_t pid=wait(&status);
    }   
sleep(6);
printf("hello im main parent %d\n",getpid());
int max1;
int min1;
int sum1;
// read max from child 1 and 2
if(h[0]<h2[0])
    max1 = h2[0];
else 
    max1 = h[0];


//display max of array
printf("Max = %d\n",max1);


//compute new min
if(h[1]<h2[1] && h[1] != -1)
    min1 = h[1];
else if(h2[1] == -1)
    min1 = h[1];
//display min
printf("Min = %d\n",min1);

//read sum from child 1 and 2
sum1 = h[2] + h2[2];

//display computed sum
printf("Sum = %d\n",sum1);

    }
}

int main(){

    for(int i = 0; i < 3; i++)
    {
        h[i] = -1;
        h2[i] = -1;
    }

FILE* nums = fopen("numbers.txt", "r");
int readnum = 0;
int counter = 0;
//scanning file for number of elements
while(fscanf(nums, "%d,", &readnum) > 0 )
{
    counter++;
}
fclose(nums);
//create array with size of elements
int array[counter];

FILE* num = fopen("numbers.txt", "r");
int readnums = 0;
int index = 0;
//put values of elements into the array
while(fscanf(num, "%d,", &readnums) > 0 )
{
    array[index] = readnums;
    index++;
}
fclose(num);
//run process
signal(SIGINT,sig_action_function);
partd(array,0,index-1);

return 0;
}