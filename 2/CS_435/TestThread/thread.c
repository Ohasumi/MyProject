#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
void *thread_function(void *arg) {
  printf("Hello from the new thread!\n");
  printf("Wait!\n");
  sleep(5);
  
  return (void*)10;
}

int main() {
  pthread_t thread_id;
  int result = pthread_create(&thread_id, NULL, thread_function, NULL);
  if (result != 0) {
    perror("Error creating thread");
    return 1;
  }
  printf("Hello from the main thread!\n");

  
  pthread_join(thread_id, NULL);

  sleep(2);
  printf("End\n");
  return 0;
}
