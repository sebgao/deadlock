#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/types.h>



#define nr_lock 100
pthread_mutex_t resource[nr_lock];
pthread_t task0, task1;


int selfid[1000];
int lselfid = 0;
int self2id(int self){
  int i = 0;
  for (i = 0; i < lselfid; ++i)
  {
    if(self == selfid[i])return i;
  }
  selfid[i] = self;
  lselfid ++;
  return i;
}
int lock2id(int lock){
  return self2id(lock);
}
#define getid() self2id(pthread_self())
#define getlockid(self) lock2id((unsigned int)self)


int map[1000];
int visited[1000];
void add_edge(int s, int t){
  map[s+1] = t+1;
}
void remove_edge(int s, int t){
  if(map[s+1] == t+1)
    map[s+1] = 0;
}
int is_there_a_loop(int s){
  memset(visited, 0, sizeof(visited));
  int cur = map[s+1];
  visited[cur] = 1;
  while(map[cur] != 0){
    cur = map[cur];
    if(visited[cur]){
      return 1;
    }
    visited[cur] = 1;
  }
  return 0;
}
void print_a_loop(int s){
  printf("Loop: ");
  memset(visited, 0, sizeof(visited));
  int cur = map[s+1];
  visited[cur] = 1;
  printf("%d", cur-1);
  while(map[cur] != 0){
    cur = map[cur];
    printf(" to %d", cur-1);
    if(visited[cur]){
      printf("\n");
      return;
    }
    visited[cur] = 1;
  }
}
void pthread_mutex_lock_extended(pthread_mutex_t *lock){
  int thread_id = getid();
  int lock_id = getlockid(lock);

  printf("Thread#%x: try to lock %x\n", thread_id, lock_id);
  add_edge(thread_id, lock_id);
  if(is_there_a_loop(thread_id))
  {
    printf("There is deadlock!\n");
    print_a_loop(thread_id);
    /*printf("Trying to recover\n");

    int i;
    for (i = 0; i < nr_lock; ++i)
    {
      pthread_mutex_unlock(&resource[i]);
    }*/

    exit(1);
  }else{
    pthread_mutex_lock(lock);
  }
  remove_edge(thread_id, lock_id);
  add_edge(lock_id, thread_id);
  printf("Thread#%x: %x locked\n", thread_id, lock_id);
}
void pthread_mutex_unlock_extended(pthread_mutex_t *lock){
  int thread_id = getid();
  int lock_id = getlockid(lock);

  printf("Thread#%x: try to unlock %x\n", thread_id, lock_id);
  
  remove_edge(lock_id, thread_id);
  pthread_mutex_unlock(lock);
  
  printf("Thread#%x: %x unlocked\n", thread_id, lock_id);
}

#define pthread_mutex_lock(res) pthread_mutex_lock_extended(res)
#define pthread_mutex_unlock(res) pthread_mutex_unlock_extended(res)



void* fun0(void* arg){
  pthread_mutex_lock(&resource[0]);
  sleep(1);
  pthread_mutex_lock(&resource[1]);
  pthread_mutex_unlock(&resource[0]);
  pthread_mutex_unlock(&resource[1]);
  pthread_exit(NULL);
}
void* fun1(void* arg){
  pthread_mutex_lock(&resource[1]);
  sleep(1);
  pthread_mutex_lock(&resource[0]);
  pthread_mutex_unlock(&resource[1]);
  pthread_mutex_unlock(&resource[0]);
  pthread_exit(NULL);
}

int main(){
  int i;
  for (i = 0; i < nr_lock; ++i)
  {
    pthread_mutex_init(&resource[i], NULL);
  }
  pthread_create(&task0, NULL, fun0, NULL);
  pthread_create(&task1, NULL, fun1, NULL);
  pthread_join(task0, NULL);
  pthread_join(task1, NULL);
}