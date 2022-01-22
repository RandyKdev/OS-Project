


#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "list.h"
#include "cpu.h"
#include "task.h"
#include "schedulers.h"

#define MIN_PRIORITY 1
#define MAX_PRIORITY 10

// list of processes
 struct node *list = NULL;

 //waiting time array
 int *WT;

//static counter to hold process id
static int id=0;

// add a task to the list 
void add(char *name, int priority, int burst){
    Task *task;
    
    task = malloc(sizeof(Task));
    task->burst = burst;
    task->name =malloc(sizeof(char)*3);
    strcpy(task->name,name);
    task->priority = priority;
    task->tid = id++;
    insert(&list, task);
}

//get the length of the list to determine how large
//the waiting time array will be
int length(){
  int count=0;
   struct node *temp=list;
    while(temp  != NULL){
        count ++;
        temp = temp->next;
    }  
    return count;
}

// calculate average waiting times
void waitingTime(){
    WT = malloc(sizeof(int)*length());
    int i =1;
    WT[0]=0;
    float sum=0.0;
    struct node *temp=list;
    while(temp  != NULL){
        WT[i] = temp->task->burst + WT[i-1];
        sum +=WT[i];
        temp = temp->next;
        i++;
    }
    printf("The average waiting time is %f", (float)(sum/length()));
}


// calculate average turn around time;
void turnAroundTime(){
    struct node * temp = list;
    int sum=0;
    int i =0;

    while(temp!=NULL){

        sum += WT[i] + temp->task->burst; // adding the burst time to waiting time
        i++;
        temp=temp->next; 
    }

    printf("The average turn around time is %f", (float)(sum/length()));
}

//response time
float responseTime(){
    int sum =0;
    int len = length();
    for(int i=0; i<len; i++){
        sum+=WT[i];
    }
    return (float)(sum/len);
}

// invoke the scheduler
void schedule(){
  struct node *temp = list;
  while(temp !=NULL){
      run(temp->task,temp->task->burst);
      temp =temp->next;
  }

  waitingTime();
}

