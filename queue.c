#include <threads.h>
#include "queue.h"
mtx_t mtx;
cnd_t waiting_list;
void *head;

typedef struct list_item{
    void *item;
    struct list_item *next;
}list_item;

typedef struct list_cv{
    cnd_t *cv;
    struct list_cv *next;
    void *to_deq;//the item that the thread would dequeue; 
}list_cv;

list_item *available_items;//pointer for the struct list_item that holds first item waiting in the queue without any thread assigned to it; 
list_cv *waiting_threads;//pointer for the struct list_cv that holds the cv in which the first thread is waiting for an item that will be assigned to it to dequeue;
mtx_t lock;


void insert_to_available_items(void *item){
    list_item *curr;
    if (available_items == NULL){
        available_items = (list_item*) malloc(sizeof(list_item));
        available_items->item = item;
    }
    else{   
        curr = available_items;
        while (curr->next != NULL){
            curr = curr->next;
        }
        curr->next = malloc(sizeof(list_item));
        (curr->next)->item = item;
    }
}

insert_to_waiting_threads(cnd_t *cv){
    list_cv *curr;
    if (waiting_threads == NULL){
        waiting_threads = (list_cv*) malloc(sizeof(list_cv));
        waiting_threads->cv = cv;
    }
    else{
        curr = waiting_threads;
        while (curr->next != NULL){
            curr = curr->next;
        }
        curr->next = malloc(sizeof(list_item));
        (curr->next)->cv = cv;
    }
}

void initQueue(void){
    available_items = NULL;
    list_cv = NULL;
    mtx_init(&lock, mtx_plain);//need to destroy it at the end; 
}
void destroyQueue(void);

void enqueue(void* item){
    mtx_lock(&lock);
    if (waiting_threads == NULL){
         insert_to_available_items(item);
    }
    else{
        waiting_threads->to_deq = item;
        cnd_signal(&waiting_threads.cv);
    }
    mtx_unlock(&lock);
}

void* dequeue(void){
    void *res;
    cnt_t cv;
    list_item *tmp_list_item;
    list_cv *tmp_list_cv;
    mtx_lock(&lock);
    if (available_items != NULL){
        tmp_list_item = available_items;
        available_items = available_items->next;
        res = tmp_list_item->item;
        free(tmp_list_item);
    }
    else{
        cnd_init(&cv);
        insert_to_waiting_threads(&cv);
        cnd_wait(&cv, &lock);
        tmp_list_cv = waiting_threads;
        waiting_threads = waiting_threads->next;
        res = tmp_list_cv->to_deq;
        free(tmp_list_cv);
    }
    mtx_unlock(&lock);
    return res;
}

bool tryDequeue(void** ptr){
    
}

size_t size(void);
size_t waiting(void);
size_t visited(void);