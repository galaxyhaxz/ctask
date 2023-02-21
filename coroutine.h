#ifndef __COROUTINE_H__
#define __COROUTINE_H__

void TaskCreate(void (*func)(void *), void *args);
void TaskCleanup();
void TaskYield(int yieldtime);
void TaskHandler();

#endif /* __COROUTINE_H__ */
