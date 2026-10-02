/*
  Each thread generates a data node, attaches it to a global list. This is reapeated for K times.
  There are num_threads threads. The value of "num_threads" is input by the student.
*/
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/param.h>
#include <sched.h>

#define K 200 // genreate a data node for K times in each thread

struct Node
{
    int data;
    struct Node* next;
};

struct list
{
     struct Node * header;
     struct Node * tail;
};

pthread_mutex_t    mutex_lock;

struct list *List;

struct Node* generate_data_node()
{
    struct Node *ptr;
    ptr = (struct Node *)malloc(sizeof(struct Node));    

    if( NULL != ptr ){
        ptr->next = NULL;
    }
    else {
        printf("Node allocation failed!\n");
    }
    return ptr;
}

void * producer_thread(struct list *lt)
{
    struct Node * ptr, tmp;
    int counter = 0;  

    /* generate and attach K nodes to the global list */
    while( counter  < K )
    {
        ptr = generate_data_node();

        if( NULL != ptr )
        {
                    ptr->data  = 1;//generate data
		    /* attache the generated node to the global list */
                    if( lt->header == NULL )
                    {
                        lt->header = lt->tail = ptr;
                    }
                    else
                    {
                        lt->tail->next = ptr;
                        lt->tail = ptr;
                    }                    
        }
        ++counter;
    }
}

int main(int argc, char* argv[])
{
    int i, num_threads;

    struct Node  *tmp,*next;
    struct timeval starttime, endtime;

    num_threads = atoi(argv[1]); //read num_threads from user
    pthread_t producer[num_threads];
    struct list* localList[num_threads];  //num_threads localList

    List = (struct list *)malloc(sizeof(struct list));
    if( NULL == List )
    {
       printf("End here\n");
       exit(0);	
    }
    List->header = List->tail = NULL;

    //init the local list
    for (i=0; i < num_threads; i++) {
	localList[i] = (struct list *)malloc(sizeof(struct list));
    	if( NULL == localList[i]) {
		printf("Local list %d end here\n", i);
		exit(0);
	}
	localList[i]->header = localList[i]->tail = NULL;
    }

    gettimeofday(&starttime,NULL); //get program start time
    for( i = 0; i < num_threads; i++ )
    {
        pthread_create(&(producer[i]), NULL, (void *) producer_thread, localList[i]); 
    }

    for( i = 0; i < num_threads; i++ )
    {
        if(producer[i] != 0)
        {
            pthread_join(producer[i],NULL);
        }
    }

    //merge localList into globalList
    for (i=0; i < num_threads; i++) {
	if (List->header == NULL) {
		List->header = localList[i]->header;
		List->tail = localList[i]->tail;
	} else {
		List->tail->next = localList[i]->header;
		List->tail = localList[i]->tail;
	}
    }

    gettimeofday(&endtime,NULL); //get the finish time

    if( List->header != NULL )
    {
        next = tmp = List->header;
        while( tmp != NULL )
        {  
           next = tmp->next;
           free(tmp);
           tmp = next;
        }            
    }
    /* calculate program runtime */
    printf("Total run time is %ld microseconds.\n", (endtime.tv_sec-starttime.tv_sec) * 1000000+(endtime.tv_usec-starttime.tv_usec));
    return 0; 
}
