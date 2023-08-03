#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>

#ifdef _X86_
#define STACK_PTR 4
//#define SetStack(stack) __asm__ volatile("mov %0, %%esp" : : "r"(stack))
#elif defined(__mips__)
#define STACK_PTR 8
//#define SetStack(stack) __asm__ volatile("move $sp, %0" : : "r"(stack))
#else
#error "CPU not yet implemented"
#endif

typedef struct AppTask {
	jmp_buf env;
	void (*func)(void *);
	void *args;
	int state;
	int yieldtime;
	void *stack;
	int stack_size;
	struct AppTask *pNext;
} AppTask;

static jmp_buf sgMainEnv;
static AppTask *sgpTaskList;
static AppTask *sgpCurrTask;

void TaskCreate(void (*func)(void *), void *args)
{
	AppTask *task;

	task = (AppTask *)malloc(sizeof(AppTask));
	memset(task, 0, sizeof(AppTask));
	task->func = func;
	task->args = args;
	task->state = 0;
	task->yieldtime = 1;
	task->stack = NULL;
	task->stack_size = 0;

	if (sgpTaskList != NULL) {
		task->pNext = sgpTaskList;
	}
	sgpTaskList = task;
}

void TaskCleanup()
{
	AppTask *list, *next;

	list = sgpTaskList;
	while (list != NULL) {
		next = list->pNext;
		free(list);
		list = next;
	}
	sgpTaskList = NULL;
}

void TaskYield(int yieldtime)
{
	if (setjmp(sgpCurrTask->env) == 0) {
		sgpCurrTask->yieldtime = yieldtime;
		int size = sgMainEnv[STACK_PTR] - sgpCurrTask->env[STACK_PTR];
		if (sgpCurrTask->stack && size > sgpCurrTask->stack_size) {
			free(sgpCurrTask->stack);
		}
		if (!sgpCurrTask->stack) {
			sgpCurrTask->stack_size = size;
			sgpCurrTask->stack = malloc(sgpCurrTask->stack_size);
		}
		memcpy(sgpCurrTask->stack, (void *)sgpCurrTask->env[STACK_PTR], sgpCurrTask->stack_size);
		longjmp(sgMainEnv, 1); // goto TASKJMP
	} else {
// SLEEPJMP:
		//int new_stack = sgpCurrTask->env[STACK_PTR] + 16;
		//memcpy((void *)new_stack, sgpCurrTask->stack, sgpCurrTask->stack_size);
		//t->env[STACK_PTR] = new_stack;
	}
}

static void TaskCaller()
{
	sgpCurrTask->func(sgpCurrTask->args);
	sgpCurrTask->state = 2;
	longjmp(sgMainEnv, 1); // goto TASKJMP
}

static void TaskJump()
{
	if (setjmp(sgMainEnv) == 0) {
		if (sgpCurrTask->state == 0) {
			sgpCurrTask->state = 1;
			// SetStack(sgpCurrTask->stack);
			TaskCaller();
		} else {
			static jmp_buf tempenv;
			if (setjmp(tempenv) == 0) {
				tempenv[STACK_PTR] -= sgpCurrTask->stack_size + 16;
				longjmp(tempenv, 1);
			} else {
				int new_stack = tempenv[STACK_PTR] + 16;
				memcpy((void *)new_stack, sgpCurrTask->stack, sgpCurrTask->stack_size);
				sgpCurrTask->env[STACK_PTR] = new_stack;
				longjmp(sgpCurrTask->env, 1); // goto SLEEPJMP
			}
		}
	} else {
// TASKJMP:
	}
}

void TaskHandler()
{
	AppTask *list, *prev;

	/* your time delaying code here */
	/* such as FPS counter to delay by frame or milliseconds */

	list = sgpTaskList;
	prev = NULL;
	while (list != NULL) {
		sgpCurrTask = list;
		list->yieldtime--;
		if (list->yieldtime == 0) {
			TaskJump();
			if (list->state == 2) {
				if (list->pNext != NULL) {
					if (prev != NULL) {
						prev->pNext = list->pNext;
					} else {
						sgpTaskList = list->pNext;
					}
				} else {
					if (prev != NULL) {
						prev->pNext = NULL;
					} else {
						sgpTaskList = NULL;
					}
				}
				free(list);
				if (prev != NULL) {
					list = prev->pNext;
				} else {
					list = sgpTaskList;
				}
				continue;
			}
		}
		prev = list;
		list = list->pNext;
	}
}
