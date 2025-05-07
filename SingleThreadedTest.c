#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

// #define TEST_SCC
#define TEST_SIMPLE
// #define TEST_CALLBACK

#ifdef TEST_SCC

void bar();
void buz();

void foo() {
  bar();
  foo();
}

void bar() {
  pthread_t thread;
  pthread_create(&thread, NULL, NULL, NULL);
  buz();
  bar();
}

void buz() {
  foo();
  buz();
}

#elifdef TEST_CALLBACK
typedef void (*callback_t)(void);

callback_t callbacks[2];

int shared_data = 0;

void deep_increment() { shared_data++; }

void increment() {
  shared_data++;
  deep_increment();
}

void deep_decrement() { shared_data--; }

void decrement() {
  shared_data--;
  deep_decrement();
}

void deep_single_threaded() { }

void single_threaded() {
  deep_single_threaded();
}

void *thread_function(void *arg) {
  callbacks[0]();
  callbacks[1]();
  return NULL;
}

int main() {
  callbacks[0] = increment;
  callbacks[1] = decrement;

  single_threaded();

  pthread_t thread1, thread2;

  pthread_create(&thread1, NULL, thread_function, NULL);
  pthread_create(&thread2, NULL, thread_function, NULL);

  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);

  printf("Final value: %d\n", shared_data);
  return 0;
}

#elifdef  TEST_SIMPLE
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

void deep_single_threaded() {
  GV_RO++;
  GV_RW++;
}

void single_threaded() {
  GV_RW++;
  func_in_both_context_but_mt();
  deep_single_threaded();
}

void thread_creator2() {
  pthread_t tid;
  printf("%d", GV_RW);
  pthread_create(&tid, NULL, thread, NULL);
  pthread_join(tid, NULL);
}

void thread_creater2_caller() {
  thread_creator2();
}

int main() {
  single_threaded();
  thread_creator();
  thread_creater2_caller();

  pthread_t tid;
  pthread_create(&tid, NULL, thread, NULL);
  pthread_join(tid, NULL);

  return 0;
}
#endif
