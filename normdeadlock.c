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



void* fun0(void* arg){
  pthread_mutex_lock(&resource[0]);
  sleep(1);
  pthread_mutex_lock(&resource[1]);
  pthread_mutex_unlock(&resource[0]);
  pthread_mutex_unlock(&resource[1]);
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