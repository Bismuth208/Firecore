/* 
 * Each task execute on it timeout.
 * 
 * Povide time-shifted multitask.
 * 
 * It allows you to run undemanding tasks on time,
 * and left more CPU resources to difficult tasks,
 * which will has low influence to other tasks, 
 * if call timeout is low(rare).
 * 
 * System uptime time couter requred.
 * You can change it in TIMER_FUNC define,
 * but it must return milliseconds!
 * 
 * Minimal time unit: 1 millis
 * Maximum tasks:     254 (0xFF reserved as NULL)
 * RAM per task:      9 bytes (for AVR only!)
 * Language:          C
 * 
 * Author: Antonov Alexandr (Bismuth208)
 * Date:      7 Dec, 2015
 * Last edit: 29 June, 2017
 * e-mail: bismuth20883@gmail.com
 * 
 * 1 tab = 2 spaces
*/

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __AVR__
#include <avr/pgmspace.h>
#else
#define PROGMEM
#endif
#include "utility/systicktimer.h"

#include "taskmanager.h"

//---------------------------------------------------------------------------//

#if !USE_DYNAMIC_MEM
// Used only in static mem managment
static uint8_t maxTasks = 0;
#endif

static bool resetTaskCount = false;

static taskStates_t *pCurrentArrTasks = NULL;  // pointer to current tasks array

#ifdef USE_MEM_PANIC
const uint8_t textPanic[] /*PROGMEM*/ = "Sorry...\nMemory panic :(\n\n";
const uint8_t textError[] /*PROGMEM*/ = "Error code: ";
#endif

#ifdef USE_GFX_LIB
fPrint_t fpPrint = NULL;
fFillRect_t fpFillRect = NULL;
fSetCursor_t fpSetCursor = NULL;
fSetTextSize_t fpSetTextSize = NULL;
fDrawFastVLine_t fpDrawFastVLine = NULL;
#endif

//------------------- Main loop reincarnation -------------------------------//
/**
 * @brief  main cycle
 * @param  None
 * @retval None
 */
__attribute__ ((noreturn)) void runTasks(void)
{
  uint8_t count =0;
  
  // store pointer in RAM, to reduce instructions
  taskStatesArr_t *pCurArr = &PAA[0];
  
#if USE_AUTO_DEFRAG
  uint32_t defragNextMs=0;
#endif
#if USE_AUTO_GEMINI
  uint32_t geminiNextMs=0;
#endif

  for(;;) {
#if USE_AUTO_DEFRAG
    if(TIMER_FUNC >= defragNextMs) {
      defragTasksMemory();
      defragNextMs = TIMER_FUNC + AUTO_DEFRAG_TIMEOUT;
    }
#endif
#if USE_AUTO_GEMINI
    if(TIMER_FUNC >= geminiNextMs) {
      rmSameTasks();
      geminiNextMs = TIMER_FUNC + AUTO_GEMINI_TIMEOUT;
    }
#endif
    
    if(PAC) {
      // Have func?
      if(pCurArr->pTaskFunc) { // problems in future see i here
        if(pCurArr->execute) { // need execute?
          // check timeout
          if(TIMER_FUNC >= pCurArr->nextCallTime) {
            pCurArr->pTaskFunc(); // execute
            
            if(resetTaskCount) {
              resetTaskCount = false;
            } else {
              // get time of next exec
              pCurArr->nextCallTime = TIMER_FUNC + pCurArr->timeToRunTask;
            }
          }
        }
      }

      ++pCurArr;
      if(++count >= PAC) {
        count =0;
        pCurArr = &PAA[0];
      }
    }
  }
}

//---------------------------------------------------------------------------//
/**
 * @brief  add one task for execution
 * @param  pTask:  pointer to task function
 * @param  timeToCheckTask: time period, when run task
 * @param  exec: need execute or not (aka reserve mem for future)
 * @retval None
 */
void addTask(pFunc_t pTask, uint16_t timeToCheckTask, bool exec)
{
  // Add task by reallocate memory
  // for dynamic struct array with pointers to funtions.
  // After, place task and timeout to new index in array.
    
#if USE_MEM_PANIC
  if(PAC < MAX_TASKS) { // less than 254 tasks
      PAC++;   // increase total tasks
  } else {
      panic(OVER_LIMIT_FAIL | ADD_FAIL);
  }
#else
  PAC++;   // increase total tasks
#endif

#if USE_DYNAMIC_MEM
  // reallocate block of RAM for new task
  PAA = (taskStatesArr_t*) realloc (PAA, PAC * sizeof(taskStatesArr_t));
#else
  if(PAC > maxTasks) {
      panic(OVER_RANGE_FAIL | ADD_FAIL);
  }
#endif

  if(PAA != NULL) {
    // aaand place params to new index
    // why -1? because we can`t allocate 0 bytes :)
    taskStatesArr_t *ptr = &PAA[PAC-1]; // reduce instructions by acces pointer
    ptr->pTaskFunc = pTask;
    ptr->timeToRunTask = timeToCheckTask;
    ptr->nextCallTime = 0;//TIMER_FUNC;
    ptr->execute = exec;
    
#if USE_MEM_PANIC   
  } else {
    deleteAllTasks();
    panic(ALLOC_FAIL | ADD_FAIL);
  }
#else
  }
#endif    
}

#ifdef __AVR__
/**
 * @brief  add one task for execution from PROGMEM section
 * @param  pTaskP: pointer to task stucture
 * @retval None
 */
void addTask_P(const taskParams_t *pTaskP)
{
  // This fuction is really dengerous as no any checks!
  // Use it if you 100500% sure what you're doing!

  // aaand place params to new index
  // increase total tasks
  taskStatesArr_t *ptr = &PAA[PAC++]; // reduce instructions by acces pointer
  ptr->pTaskFunc = (pFunc_t)pgm_read_word(&pTaskP->task);
  ptr->timeToRunTask = pgm_read_word(&pTaskP->timeout);
  ptr->nextCallTime = 0; // every fuction will call immediately
  ptr->execute = true; // always true...
}
#endif /*__AVR__*/

/**
 * @brief  add one task for execution
 * @param  pTasksArr: pointer to array whith tasks
 * @param  pTask:  pointer to task function
 * @param  timeToCheckTask: time period, when run task
 * @param  exec: need execute or not (aka reserve mem for future)
 * @retval None
 */
void addTaskToArr(taskStates_t *pTasksArr, pFunc_t pTask,
                                  uint16_t timeToCheckTask, bool exec) 
{
  // Add task by reallocate memory
  // for dynamic struct array with pointers to funtions.
  // After, place task and timeout to new index in array.

#if USE_MEM_PANIC
  if (pTasksArr->tasksCount < MAX_TASKS) { // less than 254 tasks
      pTasksArr->tasksCount++;   // increase total tasks
  } else {
      panic(OVER_LIMIT_FAIL | ADD_TO_ARR_FAIL);
  }
#else
  pTasksArr->tasksCount++;   // increase total tasks
#endif

#if USE_DYNAMIC_MEM
  // reallocate block of RAM for new task
  pTasksArr->pArr = (taskStatesArr_t*) realloc (pTasksArr->pArr,
                                  pTasksArr->tasksCount * sizeof(taskStatesArr_t));
#else
  if(pTasksArr->tasksCount > maxTasks) {
    panic(OVER_RANGE_FAIL | ADD_TO_ARR_FAIL);
  }
#endif

  if(pTasksArr->pArr != NULL) {
    // aaand place params to new index
    // why -1? because we can`t allocate 0 bytes :)
    taskStatesArr_t *ptr = &pTasksArr->pArr[pTasksArr->tasksCount-1]; // same as addTask()
    ptr->pTaskFunc = pTask;
    ptr->timeToRunTask = timeToCheckTask;
    ptr->nextCallTime = 0;//TIMER_FUNC;
    ptr->execute = exec;
    
#if USE_MEM_PANIC
  } else {
    deleteAllTasks();
    panic(ALLOC_FAIL | ADD_TO_ARR_FAIL);
  }
#else
  }
#endif
}

#ifdef __AVR__
/**
 * @brief  add array of tasks for execution from PROGMEM section
 * @param  pArr: pointer to task stucture whith pointers to tasks
 * @param  size: number of tasks to load
 * @retval None
 */
void addTasksArray_P(tasksArr_t *pArr, uint8_t size)
{
  deleteAllTasks();
  
  do {
    // get ponter to task structure and load params from structure
    addTask_P(pgm_read_word(pArr));
    ++pArr;
  } while(--size);
}
#endif /*__AVR__*/

/**
 * @brief  remove all tasks
 * @param  none
 * @retval None
 */
void deleteAllTasks(void)
{
#if USE_DYNAMIC_MEM
  free(PAA);
  PAA = NULL;
#else
  uint8_t *pBuf = (uint8_t*)PAA;
  uint16_t size = maxTasks * sizeof(taskStates_t);
  do {
    *pBuf++ = 0x00;
  } while(--size);
#endif /*USE_DYNAMIC_MEM*/
  PAC = 0;
  resetTaskCount = true;
}

/**
 * @brief  remove only single task from execution
 * @param  pTask: pointer to task what to remove
 * @retval None
 */
void deleteTask(pFunc_t pTask)
{
  uint8_t taskId = searchTask(pTask);

  if(taskId < NULL_TASK) {
    PAA[taskId].pTaskFunc = NULL;    // remove pointer
    //PAA[taskId].execute = false;     // clear exec flag
    //defragTasksMemory();
  }
  resetTaskCount = true;
}

/**
 * @brief  disable execution only for single task
 * @param  pTask: pointer to task what to disable
 * @retval None
 */
void disableTask(pFunc_t pTask)
{
  uint8_t taskId = searchTask(pTask);

  if(taskId < NULL_TASK) {
    PAA[taskId].execute = false;
  }
}

/**
 * @brief  disable execution for all tasks
 * @param  None
 * @retval None
 */
void disableAllTasks(void)
{
  for(uint8_t count = 0; count < maxTasks; count++) {
    PAA[count].execute = false;
  }
}

/**
 * @brief  eneable execution for all tasks
 * @param  None
 * @retval None
 */
void enableAllTasks(void)
{
  for(uint8_t count = 0; count < maxTasks; count++) {
    PAA[count].execute = true;
  }
}

/**
 * @brief  change execution flag only for single task
 * @param  pTask: pointer to task what to disable
 * @param  exec: new flag state
 * @retval None
 */
void updateTaskStatus(pFunc_t pTask, bool exec)
{
  uint8_t taskId = searchTask(pTask);

  if(taskId < NULL_TASK) {
    PAA[taskId].execute = exec;
  }

  // If we get here, when no such func in pCurrentArrTasks.
  // Need add task or return 0?
}

/**
 * @brief  change execution period only for single task
 * @param  pTask: pointer to task what to disable
 * @param  timeToCheckTask: new period
 * @retval None
 */
void updateTaskTimeCheck(pFunc_t pTask, uint16_t timeToCheckTask)
{
  uint8_t taskId = searchTask(pTask);

  if(taskId < NULL_TASK) {
    taskStatesArr_t *ptr = &PAA[taskId];
    ptr->timeToRunTask = timeToCheckTask;
    ptr->nextCallTime = TIMER_FUNC + timeToCheckTask;
  }

  // If we get here, when no such func in pCurrentArrTasks.
  // Need add task or return 0?
}

/**
 * @brief  replace one tast by onother one and update its params
 * @param  pOldTask: pointer to task what to replace
 * @param  pNewTask: pointer to new task
 * @param  timeToCheckTask: new period
 * @param  exec: new flag state
 * @retval None
 */
void replaceTask(pFunc_t pOldTask, pFunc_t pNewTask, uint16_t timeToCheckTask, bool exec)
{
  // Also it combine deleteTask().
  // Just call like this: replaceTask(taskToDelete, NULL, 0, false);

  uint8_t taskId = searchTask(pOldTask);

  if(taskId < NULL_TASK) {
    taskStatesArr_t *ptr = &PAA[taskId];
    ptr->pTaskFunc = pNewTask;
    ptr->nextCallTime = TIMER_FUNC + timeToCheckTask;
    ptr->timeToRunTask = timeToCheckTask;
    ptr->execute = exec;
  }

  // If we get here, when no such func or free holes left in pCurrentArrTasks.
  // Oh! Or maybe recursive call replaceTask(..) whith NULL?
  //addTask(newTask, timeToCheckTask, true);
}

/**
 * @brief  remove holes in memory
 * @param  none
 * @retval None
 */
void defragTasksMemory(void)
{
  // After some time of work may appear many holes
  // more than need. It eat much of memory, and that
  // is why need to remove them.

  bool defraged = false;
  uint8_t nullCount =0;
  uint8_t i=0, j=0;
  
  taskStatesArr_t tmpTask;
  
  // First: find all NULL pointers and move them to end of array
#if 1
  // bubble sort
  for(i = 0; i < (PAC); i++) {
    for(j = 0; j < (PAC - i - 1); j++) {
      if(PAA[j].pTaskFunc == NULL) {
        
        tmpTask = PAA[j];
        PAA[j] = PAA[j + 1];
        PAA[j + 1] = tmpTask;
        //memcpy(&tmpTask, &PAA[j], sizeof(tTaskStatesArr));
        //memcpy(&PAA[j], &PAA[j + 1], sizeof(tTaskStatesArr));
        //memcpy(&PAA[j + 1], &tmpTask, sizeof(tTaskStatesArr));
        defraged = true;
      }
    }
  }
#else
  // another type of sorting
#endif
  // Second: if some NULL was finded
  // when cut them off.
  if(defraged) {
    // search all NULL ponters from end of array
    for(i=PAC-1; i > 1; i--) {
      if(PAA[i].pTaskFunc != NULL) {
        break;
      } else {
        nullCount++; // count how much NULL`s need to cut off
      }
    }
    
    PAC -= nullCount; // Remove waste NULL`s
    
#if USE_DYNAMIC_MEM
    // free some RAM
    PAA = (taskStatesArr_t*)realloc(PAA, PAC * sizeof(taskStatesArr_t));
#if USE_MEM_PANIC
    if(PAA == NULL) {
      deleteAllTasks();
      panic(ALLOC_FAIL | DEFRAG_FAIL);
    }
#endif /*USE_MEM_PANIC*/
#endif /*USE_DYNAMIC_MEM*/
  }
}

/**
 * @brief  remove same tasks in memory
 * @param  none
 * @warning  still work unstable!
 * @retval None
 */
void rmSameTasks(void)
{
  // If by some reasons in task arr are two or more
  // same tasks - remove them
  
  uint8_t i, j;
  
  //printTasksMem(0);
  //defragTasksMemory();
  //printTasksMem(40);

  for (i=0; i<PAC; i++) { //
    for (j=1; j<(PAC-i); j++) {
      if((PAA[i].pTaskFunc) && (PAA[i+j].pTaskFunc)) {
        if(PAA[i].pTaskFunc == PAA[i+j].pTaskFunc) {
          
          PAA[i+j].pTaskFunc = NULL;
        }
      }
    }
  }
  
  //printTasksMem(80);
  //defragTasksMemory();
  //printTasksMem(120);
}

//==================== SEARCH FUNCTION ======================================//
#ifdef LINEAR_SEARCH
/**
 * @brief  search task in current tasks array
 * @param  pTask: pointer to task function
 * @retval number id in tasks array
 */
uint8_t searchTask(pFunc_t pTask)
{
#ifdef __AVR__
  addrCompare_t tmpOne, tmpTwo;
  
  tmpOne.pFunc = (uint16_t)pTask; // tmpOne.pFunc use only r20,r21
  taskStatesArr_t *ptr = &PAA[0]; // store pointer to X register
  //uint8_t count = PAC;
  
  for(uint8_t count = 0; count < PAC; count++) {
  //do {
    tmpTwo.pFunc = (uint16_t)ptr->pTaskFunc; // store addr to r24,r25
    // compare addr separetly, reque less instructions
    if(tmpOne.pFuncHi == tmpTwo.pFuncHi) { // compare r24,r21
      if(tmpOne.pFuncLow == tmpTwo.pFuncLow) { // compare r25,r20
        return count; // ok, this is it!
      }
    }
    ++ptr;
  };// while(--count);
#else
  for(uint8_t count=0; count < PAC; count++) {
    if(PAA[count].pTaskFunc == pTask) {
      return count; // ok, this is it!
    }
  }
#endif

  return NULL_TASK;  // no such func
}
#endif /*LINEAR_SEARCH*/
//===========================================================================//
/**
 * @brief  init base pointers
 * @param  tasks: pointer to current task states
 * @param  taskArr: pointer to array whith tasks
 * @param  maximumTasks: maximum possible tasks in task array
 * @warning  all pointers must be inited propertly otherwise: undefined bahavor!
 * @retval none
 */
void initTasksArr(taskStates_t *tasks, taskStatesArr_t *taskArr, uint8_t maximumTasks)
{
  pCurrentArrTasks = tasks;
  PAA = taskArr;
#if !USE_DYNAMIC_MEM
  maxTasks = maximumTasks;
#endif
}

/**
 * @brief  init base pointer
 * @param  pNewTasksArr: pointer to current task states
 * @retval pointer to previous task states scturct
 */
taskStates_t *setTaskArray(taskStates_t *pNewTasksArr)
{
  taskStates_t *pOldTaskArr = pCurrentArrTasks; // make a copy
  pCurrentArrTasks = pNewTasksArr; // replace pointers
  return pOldTaskArr; // previous pointer
}

/**
 * @brief  get current task states
 * @param  none
 * @retval pointer to current task states
 */
uint16_t *getCurrentTaskArray(void)
{
  return (uint16_t *)pCurrentArrTasks;
}

#if !USE_DYNAMIC_MEM
// Use only when USE_DYNAMIC_MEM is 0
void setMaxTasks(uint8_t maximumTasks)
{
  maxTasks = maximumTasks;
}
#endif

// ------------------------------- helpfull -------------------------- //
/**
 * @brief  put error code to specific output system; stop exec functions and stay in it
 * @param  errorCode: code whith reason where and why panic happened
 * @retval none
 */
__attribute__ ((noreturn)) void panic(uint8_t errorCode)
{
#if USE_MEM_PANIC
  char errBuf[3];
  
  itoa(errorCode, errBuf, 16);
  
#ifdef USE_GFX_LIB
  fpSetTextSize(3);
  fpSetCursor(0,0);
    
  fpFillRect(0,0,GFX_SCR_W,GFX_SCR_H,COLOR_BLUE);
  fpPrint(textPanic);
  fpPrint(textError);
  
  fPrint(errBuf);
#else
  (void)errorCode;
  // place here some another way to print error code
#endif /*USE_GFX_LIB*/
#endif /*USE_MEM_PANIC*/
  
  while(1); // panic mode: on
}

/**
 * @brief  get number of tasks stored in current task array
 * @param  none
 * @retval number of tasks stored in current task array
 */
uint8_t avalibleTasks(void)
{
  return PAC;
}

/**
 * @brief  get size of avaliable RAM
 * @param  none
 * @retval number of free bytes in RAM
 */
uint16_t avalibleRam(void) // space between the heap and the stack
{
#ifdef __AVR__
  // GCC unicorn magic...
  extern uint16_t __heap_start, *__brkval;
  uint16_t v;
  return (uint16_t) &v - (__brkval == 0 ? (uint16_t) &__heap_start : (uint16_t) __brkval);
#else // another arch, don`t know how to check
  return 0;
#endif
}

#ifdef USE_GFX_LIB
/**
 * @brief  print all tasks adreses stored in RAM
 * @param  offset: position in pixels X ordinate
 * @warning  this is for debug only! and reque graphycs screen!
 * @retval none
 */
void printTasksMem(uint16_t offset)
{
  char buf[10];
  char adrBuf[5];
  
  fpSetTextSize(1);
  fpFillRect(offset, 0, 36, 8*PAC, GFX_COLOR_BLUE);
  fpDrawFastVLine(offset+37, 0, 8*PAC, GFX_COLOR_WHITE);
  
  for (uint8_t i=0; i<PAC; i++) {
    memset(buf, 0x00, 10);
    memset(adrBuf, 0x00, 5);

    fpSetCursor(offset, 8*i);
    
    strcat(buf, "0x");
    strcat(buf, itoa(PAA[i].pTaskFunc, adrBuf, 16));
    strcat(buf, "\n");
    
    fpPrint(buf);
  }
}

/**
 * @brief  setup GFX function pointers
 * @param  fPrint: draw text
 * @param  fFillRect: draw rectangle
 * @param  fSetCursor: set text cursor on X and Y position
 * @param  fSetTextSize: set size of text on screen
 * @param  fDrawFastVLine: draw vertical line
 * @retval none
 */
void setGfxFunc(fPrint_t fPrint, fFillRect_t fFillRect, fSetCursor_t fSetCursor,
                fSetTextSize_t fSetTextSize, fDrawFastVLine_t fDrawFastVLine)
{
  fpPrint = fPrint;
  fpFillRect = fFillRect;
  fpSetCursor = fSetCursor;
  fpSetTextSize = fSetTextSize;
  fpDrawFastVLine = fDrawFastVLine;
}
#endif /*USE_GFX_LIB*/
// ------------------------------------------------------------------- //
