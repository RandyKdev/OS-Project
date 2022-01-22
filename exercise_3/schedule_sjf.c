

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
   // if(list!=NULL && list->next!=NULL)  printf("%s",list->next->task->name);
}

void insert_node(struct node **head, Task *task){
    struct node *new = malloc(sizeof(struct node));
    struct node *temp = *head;
    new->task = task;
    if(*head ==NULL){
        new->next=*head;
        *head = new;
        return;
    }
    while(temp->next!=NULL){
        temp = temp->next;
    }
    new->next = temp->next;
    temp->next = new;
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

//calculate the waiting time
void WaitingTime(){
    int i =1;
    int sum =0;
    struct node *temp = scheduled;
    int len = length();
    WT = malloc(sizeof(int)*len);
    WT[0]  = 0;

    while(temp!=NULL){
        WT[i] = temp->task->burst + WT[i-1];
        sum +=WT[i];
        temp = temp->next;
        i++;
    }
    printf("The average Waiting Time is %f", (float)(sum/len));
}

//calculate the turn around time
void turnAroundTime(){
    struct node * temp = scheduled;
    int sum=0;
    int i =0;

    while(temp!=NULL){
        sum += WT[i] + temp->task->burst;
        i++;
        temp=temp->next; 
    }

    printf("The average turn around time is %f", (float)(sum/length()));
}

// invoke the scheduler
void schedule(){
    struct node *shortest = list;
    struct node *temp = list;
   // printf("%d", list->next->task->burst);
while(temp !=NULL){
    while(temp != NULL){
        if(temp->task->burst< shortest->task->burst){
            shortest = temp;
        }
        temp = temp->next;
    }
        run(shortest->task, shortest->task->burst);
        insert_node(&scheduled, shortest->task);
        delete(&list, shortest->task);
        temp = list;
        shortest = list;
   }
}


