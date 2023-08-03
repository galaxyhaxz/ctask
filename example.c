#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "coroutine.h"

static int sgTaskRunState = 2;

void FunctionA(void *args)
{
	for (int i = 0; i <= 5; i++) {
		printf("Inside of A - count up %d\n", i);
		TaskYield(1);
	}
	sgTaskRunState--;
}

void FunctionB(void *args)
{
	for (int i = 5; i >= 0; i--) {
		printf("Inside of B - count down %d\n", i);
		TaskYield(1);
	}
	sgTaskRunState--;
}

int main(int argc, char *argv[])
{
	printf("Starting task system\n");

	TaskCreate(FunctionA, NULL);
	TaskCreate(FunctionB, NULL);

	while (sgTaskRunState != 0) {
		TaskHandler();
	}

	printf("All tasks completed!\n");
	TaskCleanup();

	return 0;
}
