


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
 struct node *copyList = NULL;
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

//length of the list of tasks
int length(){
  int count=0;
   struct node *temp=copyList;
    while(temp  != NULL){
      //  printf("%d",len);
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
    int burst =0;
    initialize(len);
    int total = 0;
    while(temp!=NULL){
        while(ptemp!=NULL){
            //completion - burst for every task;
            burst += ptemp->task->burst;
            if(ptemp->task->tid == temp->task->tid){
                total +=ptemp->task->burst;
                WT[temp->task->tid] = burst-total ;
            }
            ptemp = ptemp->next;
        }
        burst = 0;
        total=0;
        ptemp = scheduled;
        temp = temp->next;
    }
    float avgSum = 0;
   
    for(int i=0; i<len; i++){
        avgSum +=WT[i];
    }
    printf("%d",WT[3]);
    
    printf("\nThe average Waiting time is %.3f\n", (float)(avgSum/len));
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

void sendToEnd(struct node **l,struct node *e){
    struct node *temp = *l;
    while(temp->next !=NULL){
        temp =temp->next;
    }
    temp->next = e;
    // l->next = NULL;
}

void rr(struct node **l){
 struct node *temp = *l;
    while(temp!=NULL){
        if(temp->task->burst<=10){
            run(temp->task, temp->task->burst);
             s(temp->task->name,temp->task->priority , temp->task->burst,  temp->task->tid );
            delete(l, temp->task);
        }else{
            run(temp->task, 10);
            temp->task->burst -=10;
            s(temp->task->name,temp->task->priority , 10,  temp->task->tid );
            sendToEnd(l,temp);
            delete(l,temp->task);
            temp->next=NULL;
        }
        temp=*l;
    }
  
}   

// invoke the scheduler
void schedule(){
    
    struct node *greatest = list;
    struct node *temp = list->next;

    int count =0;
   // printf("%d", list->next->task->burst);
while(temp !=NULL){
    while(temp != NULL){
        if(temp->task->priority> greatest->task->priority){
            greatest = temp;
            count = 0;
        }else if(temp->task->priority == greatest->task->priority){
                count++;
        }
        temp = temp->next;
    }
    temp = list;
    if(count ==0){
        run(greatest->task, greatest->task->burst);
        //insert scheduled task into a new list;
        s( greatest->task->name,greatest->task->priority , greatest->task->burst,  greatest->task->tid );
        delete(&list, greatest->task);
    }else{
        struct node *newList = NULL;
        while(temp!=NULL){
            if(temp->task->priority == greatest->task->priority) {
            insert(&newList, temp->task);
            delete(&list, temp->task);
            }
        temp=temp->next;
        }
        rr(&newList);
    }
        if(list ==NULL){
            temp = NULL;
        }else{
        temp = list->next;
       
        }
        greatest = list;
   }
  waitingTime();
}


