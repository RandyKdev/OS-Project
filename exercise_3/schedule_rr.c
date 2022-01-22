

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
 struct node *copyList;
 struct node *scheduled = NULL;

 int *WT;

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
    insert(&copyList, task);
}

//insert a scheduled task into the scheduled list;
void s(char *name, int priority, int burst,int ids){
    Task *task;
    
    task = malloc(sizeof(Task));
    task->burst = burst;
    task->name =malloc(sizeof(char)*3);
    strcpy(task->name,name);
    task->priority = priority;
    task->tid = ids;
    insert(&scheduled, task);
}


int length(){
  int count=0;
   struct node *temp=copyList;
    while(temp  != NULL){
        count ++;
        temp = temp->next;
    }  
    return count;
}


void initialize(int n){
    for(int i=0; i<n; i++){
        WT[i]=0;
    }
}

void waitingTime(){
    int len  = length();
    WT = malloc(sizeof(int)*len);
    struct node *temp =scheduled;
    struct node *ptemp =scheduled->next ;
    int completion =0;
    initialize(len);
    int burstTotal = 0;
   printf("\n\n");
    while(temp!=NULL){
        while(ptemp!=NULL){
            completion += ptemp->task->burst;
            if(ptemp->task->tid == temp->task->tid){
                burstTotal +=ptemp->task->burst;    //since burst times in the scheduled list have been slice we sum the slices to get the total
                WT[temp->task->tid] = completion-burstTotal ;//WT = Completion - Burst;
            }
            ptemp = ptemp->next;
        }
        completion = 0;
        burstTotal=0;
        ptemp = scheduled;
        temp = temp->next;
    }
    float avgSum = 0;
   
    for(int i=0; i<len; i++){
        avgSum +=WT[i];
    }
    printf("\nThe average Waiting time is %f\n", (float)(avgSum/len));
}

void avgTurnAroundTime(){
struct node *temp = copyList;
    int sum=0;
   
    while(temp!=NULL){
        sum += WT[temp->task->tid] + temp->task->burst;
        temp=temp->next; 
    }

    printf("The average turn around time is %f", (float)(sum/length()));
}

void sendToEnd(struct node *l){
    struct node *temp = list;
    while(temp->next !=NULL){
        temp =temp->next;
    }
    temp->next = l;
}



// invoke the scheduler
void schedule(){
    
    struct node *temp = list;
    while(temp!=NULL){
        if(temp->task->burst<=10){
            run(temp->task, temp->task->burst);
            s(temp->task->name, (temp->task->priority), temp->task->burst,temp->task->tid);
            delete(&list, temp->task);
            free(temp);
        }else{
            run(temp->task, 10);
            temp->task->burst =temp->task->burst -10;
            s(temp->task->name, (temp->task->priority), 10, temp->task->tid );
            sendToEnd(temp);
            delete(&list,temp->task);
            temp->next=NULL;
        }
        temp=list;
    }
  waitingTime();
}



