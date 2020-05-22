#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

/* Creates threads and prints out their stack locations to find when
   we will start trampling other threads' information */


typedef struct {
  const char *name;
  int id;
} threadName;

/* Entry-point for the threads. Input is ignored, output is null */
void *thread_entry(void *threadInfo){
  int p;
  threadName *info = (threadName *)threadInfo;

  /* Prints the address of the local variable allocated on the stack */
  printf("%s: Address of first variable x%x\n", info->name, (unsigned long) &p);
  return NULL;
}

int main(int argc, char *argv[]) {
  int creation_status=9999, attr_status=9999;
  pthread_t child;
  pthread_attr_t attr;
  threadName mainT, one, two, three;
  const char *mainTname = "main", *oneName = "one", *twoName = "two", *threeName = "three";

  mainT.name = mainTname; mainT.id = 0;
  one.name=oneName; one.id=1;
  two.name=twoName; two.id=1;
  three.name=threeName; three.id=1;  
    

  /* Create a new thread */
  attr_status = pthread_attr_init(&attr);
  if (attr_status != 0) {
    printf("Could not create an attribute. Error = %d\n", attr_status);
    return -1;
  }

  creation_status = pthread_create(&child, &attr, &thread_entry, (void *)&one);
  if (creation_status != 0){
    /* Could not create a thread. */
    printf("Could not create a thread. Error = %d\n", creation_status);
    return -2;
  }


  creation_status = pthread_create(&child, &attr, &thread_entry, (void *)&two);
  if (creation_status != 0){
    /* Could not create a thread. */
    printf("Could not create a thread. Error = %d\n", creation_status);
    return -2;
  }

  /* Success, we should print main threads's address as well, by just calling thread_entry here. */
  thread_entry(&mainT);

  /* Need to wait some time (2 seconds here) before returning or else
     the thread doesn't schedule */
  sleep(2);
  
  return 0;
}
