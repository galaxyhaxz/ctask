#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "coroutine.h"

void FunctionA(void *args)
{
	int i = 0;
	while (1) {
		printf("Inside of A - count %d\n", i);
		TaskYield(1);
		i++;
	}
	/* no return */
}

void FunctionB(void *args)
{
	int i = 0;
	while (1) {
		printf("Inside of B - count %d\n", i);
		TaskYield(1);
		i--;
	}
	/* no return */
}

int main(int argc, char *argv[])
{
	printf("Starting task system\n");

	TaskCreate(FunctionA, NULL);
	TaskCreate(FunctionB, NULL);

	for (int i = 0; i < 100; i++) {
		TaskHandler();
	}

	TaskCleanup();

	return 0;
}
