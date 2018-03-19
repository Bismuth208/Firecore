#ifndef _TASKMANAGER_H
#define _TASKMANAGER_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_TASKS   0xFE
#define NULL_TASK   0xFF
#define SEC         *1000

//#define SHRINK_FACTOR 1.24733f

//------------------------ Usefull LIB Flags --------------------------------//
/*
 * If you are shure what:
 *   - you have enougth memory (bazillions bazillions bytes);
 *   - no memory leak;
 *   - you know what you do;
 *   - need a few more programm memory;
 *   - other unnown reasons.
 * When, set USE_MEM_PANIC to 0
 */
#define USE_MEM_PANIC 0

/*
 * If set this to 0, then:
 *   - memalloc;
 *   - realloc
 *   - free.
 * will be unavaliable!
 *
 * Only static memory managment!
 * Created for minimize ROM usage.
 * Use only when you know final number of tasks!
 * And tTaskStates.tTaskStatesArr must be init correctly!
 *
 * Like this:
 *  tTaskStates someName = {NULL, 0};
 *  tTaskStatesArr someNameArr[X];
 *  someName.pArr = &someNameArr;
 * where X is number of elements(Tasks);
 */
#define USE_DYNAMIC_MEM 0

/*
 * Set this one to 1 and it will periodically call
 * defragTasksMemory() func by default.
 * And it not depend on current task array!
 */
#define USE_AUTO_DEFRAG 0
// Set in seconds how often defragTasksMemory() will call
// NOTE: works only if USE_AUTO_DEFRAG is 1 !
#define AUTO_DEFRAG_TIMEOUT 10 SEC

/*
 * Set this one to 1 and it will periodically call
 * rmDuplicateTasks() func by default.
 * And it not depend on current task array!
 */
#define USE_AUTO_GEMINI 0
// Set in seconds how often rmDuplicateTasks() will call
// NOTE: works only if USE_AUTO_GEMINI is 1 !
#define AUTO_GEMINI_TIMEOUT 12 SEC

/*
 * If you are shure what:
 *   - you wouldn't add new task in IRQ;
 *   - task array will be always filled at least by one task;
 *   - other unnown reasons.
 * When, set USE_NO_TASK_PANIC to 0
 */
#define USE_NO_TASK_PANIC 0


/*
 * Uncomment this one and enable otuput errors on screen.
 * Add depedencies from gfx lib.
 *
 * NOTE: place your specific way to print error codes in panic() func!
 */
//#define USE_GFX_LIB

/*
 * Select search methode.
 * LINEAR is reque less ROM and RAM, but slow
 */
#define LINEAR_SEARCH


#ifdef USE_GFX_LIB
// in rgb 565 color space
#define GFX_COLOR_BLUE   0x001F  //   0,   0, 255
#define GFX_COLOR_WHITE  0xFFFF  // 255, 255, 255

// screen size in pixels
#define GFX_SCR_W        160
#define GFX_SCR_H        128
#endif
//---------------------------------------------------------------------------//

// PAA - pointer acess array
#define PAA pCurrentArrTasks->pArr
// PAC - pointer acess count
#define PAC pCurrentArrTasks->tasksCount

#if 1
 #define TIMER_FUNC  uptime()
#else
 #define TIMER_FUNC // place here your system uptime timer function
#endif

//----------- ERROR CODES --------------//
//  NAME:                  ERR CODE:    WHY IT`S HAPPEN:
#define NO_TASKS_FAIL           0x00    // tasks list is empty!
#define OVER_LIMIT_FAIL         0x01    // more than: 0xFE
#define OVER_RANGE_FAIL         0x02    // more than: maxTasks
#define ALLOC_FAIL              0x03    // not enougth RAM

//                                      WHERE IT`S HAPPEN:
#define CHECK_TASKS_FAIL        0x00    // runTasks()
#define ADD_FAIL                0x10    // addTask()
#define ADD_TO_ARR_FAIL         0x20    // addTaskToArr()
#define DEFRAG_FAIL             0x30    // defragTasksMemory()
#define FIND_TASK_FAIL          0x40    // searchTask()
//--------------------------------------//

//---------- Bool definitions --------------//
#ifndef bool
 #define bool uint8_t
#endif

#ifndef true
 #define true 1
#endif

#ifndef false
 #define false 0
#endif
//------------------------------------------//

#define T(a) a##Task
#define TASK_N(a)     const taskParams_t T(a)
#define TASK(a,b)     TASK_N(a) PROGMEM = {.pFunc=a, .timeOut=b}
#define TASK_P(a)     (taskParams_t*)&T(a)
#define TASK_ARR_N(a) const tasksArr_t a##TasksArr[]
#define TASK_ARR(a)   TASK_ARR_N(a) PROGMEM
#define TASK_END      NULL

//------------------------------------------//

#ifdef __cplusplus
extern "C"{
#endif

//--------------------------- Little Help -----------------------------------//
typedef void (*pFunc_t)(void);

#ifdef USE_GFX_LIB
typedef void (*fPrint_t)(uint8_t*);
typedef void (*fSetCursor_t)(uint16_t, uint16_t);
typedef void (*fDrawFastVLine_t)(uint16_t, uint16_t, uint16_t, uint16_t);
typedef void (*fFillRect_t)(uint16_t, uint16_t, uint16_t, uint16_t, uint16_t);
#endif
//---------------------------------------------------------------------------//
    
//------------------------ Tasks Structures ---------------------------------//
#pragma pack(push, 1)
typedef struct {
#ifdef __AVR__
  union  {
    pFunc_t pFunc;    // on avr it get 2 bytes
    struct {
      uint8_t hi;
      uint8_t low;
    };
  };
#else
  pFunc_t pFunc;     // on 32-bit arch it get 4 bytes
#endif /* __AVR__*/
  
  uint16_t timeOut;   // 65,5 seconds will be enougth? (65535 millis / 1000)
} taskParams_t;
  
typedef struct {            // 9 bytes RAM(*)
  uint32_t nextCallTime;  	// when will be next func call
  taskParams_t task;
  uint8_t execute;      		// status flag; need exec or not
  /* // at this moment not need
  struct {
  	uint8_t execute 	:1;		// status flag; need exec or not
  	uint8_t freeRam 	:7;		// not implemented features
    //uint8_t priority 	:2;		// priority of task 0-3
  };
   */
  // 7 or 5 bytes align here
  //whole size: avr = 9 bytes, arm = 11 bytes
} taskFunc_t;

typedef struct {            // (2(*) + tasksCount * taskFunc_t) + 1 bytes RAM
  taskFunc_t *pArr;    // pointer to array whith tasks
  uint8_t tasksCount;       // Note: 0xFF reserved as NULL !
  // 1 or 3 bytes align here
  // whole size: avr = 4 bytes, arm = 8 bytes.
} tasksContainer_t;
#pragma pack(pop)
  
typedef const taskParams_t * const tasksArr_t;
  
// * On AVR arch only.
//---------------------------------------------------------------------------//
  
//------------------------ Function Prototypes ------------------------------//
void addTaskToArr(tasksContainer_t *pTasksArr, pFunc_t pTask, uint16_t timeToCheckTask, bool exec);
void addTask(pFunc_t pTask, uint16_t timeToCheckTask, bool exec);
void addTask_P(const taskParams_t *pTaskP);
void addTasksArray_P(tasksArr_t *pArr);
void replaceTask(pFunc_t pOldTask, pFunc_t pNewTask, uint16_t timeToCheckTask, bool exec);
void updateTaskStatus(pFunc_t oldTask, bool exec);
void updateTaskTimeCheck(pFunc_t pTask, uint16_t timeToCheckTask);
void disableTask(pFunc_t pTask);
void enableTask(pFunc_t pTask);
void disableAllTasks(void);
void enableAllTasks(void);
void deleteTask(pFunc_t pTask);
void deleteAllTasks(void);

__attribute__ ((noreturn)) void runTasks(void);

void initTasksArr(tasksContainer_t *tasks, taskFunc_t *taskArr, uint8_t maximumTasks);
tasksContainer_t *setTaskArray(tasksContainer_t *pNewTasksArr);
uint16_t *getCurrentTaskArray(void);
void setMaxTasks(uint8_t maximumTasks);
  
void rmSameTasks(void);  // still not ready
void defragTasksMemory(void);
uint8_t searchTask(pFunc_t pTask);
uint8_t avalibleTasks(void);
uint16_t avalibleRam(void);
__attribute__ ((noreturn)) void panic(uint8_t errorCode);
  
void printTasksMem(uint16_t offset);
  
#ifdef USE_GFX_LIB
void setGfxFunc(fPrint_t, fFillRect_t, fSetCursor_t, fDrawFastVLine_t);
#endif
//---------------------------------------------------------------------------//
    
#ifdef __cplusplus
} // extern "C"
#endif

#endif /*_TASKMANAGER_H*/
