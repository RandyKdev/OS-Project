


#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "list.h"
#include "cpu.h"
#include "task.h"
#include "schedulers.h"

#define MIN_PRIORITY 1
#define MAX_PRIORITY 10

 struct node *list = NULL;
 int *arr;
// add a task to the list 
static int id=0;
void add(char *name, int priority, int burst){
    Task *task;
    
    task = malloc(sizeof(Task));
    task->burst = burst;
    task->name =malloc(sizeof(char)*3);
    strcpy(task->name,name);
    task->priority = priority;
    task->tid = id++;
    insert(&list, task);
   // if(list!=NULL && list->next!=NULL)  printf("%s",list->next->task->name);
}

int length(){
  int count=0;
   struct node *temp=list;
    while(temp  != NULL){
        count ++;
        temp = temp->next;
    }  
    return count;
}

void waitingTime(){
    arr = malloc(sizeof(int)*length());
    int i =1;
    arr[0]=0;
    float sum=0.0;
    struct node *temp=list;
    while(temp  != NULL){
        arr[i] = temp->task->burst + arr[i-1];
        sum +=arr[i];
        temp = temp->next;
        i++;
    }


    printf("The average waiting time is %f", (float)(sum/length()));
}

void turnAroundTime(){
    struct node * temp = list;
    int sum=0;
    int i =0;

    while(temp!=NULL){
        sum += arr[i] + temp->task->burst;
        i++;
        temp=temp->next; 
    }

    printf("The average turn around time is %f", (float)(sum/length()));
}

float responseTime(){
    int sum =0;
    int len = length();
    for(int i=0; i<len; i++){
        sum+=arr[i];
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

