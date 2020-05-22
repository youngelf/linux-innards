#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

/* Creates threads and prints out their stack locations to find when
   we will start trampling other threads' information */


/* Define this to 1 to try to access the other thread's stack */
/* #define CRASH 1 */

/* Information that will be sent to every thread so it knows its own
 * name. Useful only for the user-visible printf. */
typedef struct {
	const char *name;
	int id;
} threadName;

/* Entry-point for the threads. Input is the thread's name, output is
 * null */
void *thread_entry(void *threadInfo){
	/* Unused local variable. Creating one to find the address of
	 * the stack */
	int local_variable = 42;
#ifdef CRASH
	int other_variable;
#endif
	threadName *info = (threadName *)threadInfo;

	/* Let's change our local variable so we can detect another
	 * thread. */
	local_variable = info->id;

	/* Prints the address of the local variable allocated on the
	 * stack */
	printf("%s: Address of first variable 0x%lx\n", info->name, (unsigned long) &local_variable);

	/* For the first thread only, try to read the second thread's
	 * information. This will segfault as a thread is wandering
	 * off its own stack: https://lwn.net/Articles/725832/ */
#ifdef CRASH
	if (info->id == 1) {
		other_variable = *(&local_variable - (0x8392704 >> 3));
		printf("%s: Other thread's variable = %d\n", info->name, other_variable);
	}
#endif

	/* Need to wait some time (2 seconds here) before returning or else
	   the thread finishes */
	sleep(2);

	return NULL;
}

/* Utility method to create a thread */
void create_thread(threadName *name){
	int creation_status=9999, attr_status=9999;
	pthread_t child;
	pthread_attr_t attr;

	/* Allocate thread attributes */
	attr_status = pthread_attr_init(&attr);
	if (attr_status != 0) {
		printf("Could not create an attribute. Error = %d\n", attr_status);
		_exit(-1);
	}

	creation_status = pthread_create(&child, &attr, &thread_entry, (void *)name);
	if (creation_status != 0){
		printf("Could not create a thread. Error = %d\n", creation_status);
		_exit(-2);
	}
}

/* Creates three threads which print out the address of their stacks,
 * waits for them to finish, and exits. */
int main(int argc, char *argv[]) {
	threadName mainT, one, two, three;
	const char *mainTname = "main", *oneName = "one", *twoName = "two", *threeName = "three";

	/* Thread-local names are initialized here manually. The 'main'
	 * thread cannot be called 'main' because that's the name of the
	 * standard C entrypoint (this method) */
	mainT.name = mainTname; mainT.id = 0;
	one.name=oneName; one.id=1;
	two.name=twoName; two.id=2;
	three.name=threeName; three.id=3;

	/* Create the first thread. Really, this is the second thread
	 * because the process starts out with one thread already, the main
	 * thread. */
	create_thread(&one);

	/* Second thread */
	create_thread(&two);

	/* Third thread */
	create_thread(&three);

	/* Success, we should print main threads's address as well, by just
	 * calling thread_entry here. */
	thread_entry(&mainT);

	/* Need to wait some time (2 seconds here) before returning or else
	   the thread doesn't schedule */
	sleep(2);

	return 0;
}
