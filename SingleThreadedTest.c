#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

int GV_RO = 0;
int GV_RW = 0;

void called_within_thread() {}

void *thread(void *arg) {
  called_within_thread();
  GV_RW = GV_RO;
  return NULL;
}

void func_in_both_context_but_mt() {}

void thread_creator() {
  pthread_t tid;
  pthread_create(&tid, NULL, thread, NULL);
  printf("%d", GV_RO);
  pthread_join(tid, NULL);
  func_in_both_context_but_mt();
}

void deep_single_threaded() { }

void single_threaded() {
  func_in_both_context_but_mt();
  deep_single_threaded();
}

void f2() {
  pthread_t tid;
  printf("%d", GV_RW);
  pthread_create(&tid, NULL, thread, NULL);
  pthread_join(tid, NULL);
}

void f1() {
  f2();
}

int main() {
  single_threaded();
  thread_creator();
  f1();

  pthread_t tid;
  pthread_create(&tid, NULL, thread, NULL);
  pthread_join(tid, NULL);

  return 0;
}