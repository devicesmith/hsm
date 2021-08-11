#ifndef __HSMFRAMEWORK_H__
#define __HSMFRAMEWORK_H__

#include <stdbool.h>
#include <algorithm> // for find

#define ARRAY_LENGTH(array) (sizeof(array)/sizeof(*(array)))

typedef enum
{
  STATE_UNKNOWN,
  STATE_HANDLED,
  STATE_CHANGED,
  STATE_DO_SUPERSTATE
} hsm_state_result;

typedef enum
{
  PROCESS_HANDLED,
  PROCESS_STATE_CHANGED
} hsm_process_result;

enum hsm_signal
{
  HSM_SIG_INIT = 1,
  // Do not implement SIG_SILENT. Needed to fall through to superstate handler.
  HSM_SIG_SILENT,
  HSM_SIG_ENTRY,
  HSM_SIG_EXIT,
  HSM_SIG_INITIAL,
  HSM_SIG_USER
};

//typedef unsigned hsm_signal;

struct hsm_event
{
  enum hsm_signal signal;
  bool inUse;
};

struct hsm_event_pool
{
  struct hsm_event * eventArray;
  int arrayLength;
};

typedef hsm_state_result (*state_handler)(struct hsm_state * self, struct hsm_event const * hsmEvent);

 struct hsm_state
 {
   state_handler stateHandler;
 };

 struct fifo_data
{
  int head;
  int tail;
  int size;
  int depth;
  bool inUse;
  struct hsm_event** hsmEvents;
};

void hsmInitStateMachine(void);
void hsmInitialState(struct hsm_state* state, state_handler stateHandler);

hsm_process_result hsmHandleEvent(struct hsm_state * self, struct hsm_event * e);

void hsmProcess(struct hsm_state * self, struct fifo_data* fifo);

void queueInit(struct fifo_data* fifo, struct hsm_event **eventStart, int depth);
int queueDepth(struct fifo_data* fifo, struct hsm_event **events);
int queueSize(struct fifo_data* fifo, struct hsm_event **events);
bool queuePush(struct fifo_data* fifo, struct hsm_event* e);
struct hsm_event* queuePop(struct fifo_data* fifo, struct hsm_event** events);

void fifoInit(struct hsm_event **eventStart, int depth);
int fifoDepth();
int fifoSize();
bool fifoPush(struct hsm_event* e);
struct hsm_event* fifoPop(void);
bool hsmEventQueueInit(struct fifo_data* fifo, struct hsm_event ** eventQueueArray, int arrayLength);

bool hsmEventPoolInit(struct hsm_event * eventArray, int arrayLength);
struct hsm_event * hsmEventNew(void);
void hsmEventDelete(void *);

//extern void (*pLogCallback)(char *);

#define HSM_DEBUG_LOGGING
#ifdef HSM_DEBUG_LOGGING

#define HISTORY_LENGTH 20

extern int signal_filter[10];
extern bool print_signal;
extern state_handler transition_history[HISTORY_LENGTH];
extern struct hsm_event event_history[HISTORY_LENGTH];
extern int _ti;

//#define HSM_DEBUG_PRINT_HANDLER(x) ({if(print_signal) printf("%s:[%s];", __func__, (x));})
#define HSM_DEBUG_PRINT_HANDLER(x) (void(x))

#define HSM_DEBUG_NEWLINE() (printf("\n"))
//#define HSM_DEBUG_NEWLINE()

#define HSM_DEBUG_PRINTLN(s) (printf("%s\n", (s)))

//#define HSM_DEBUG_PRINT_EVENT(e) (void(e))
#define HSM_DEBUG_PRINT_EVENT(e) { \
	int *f = std::find(std::begin(signal_filter), std::end(signal_filter), (e)->signal); \
	print_signal = (f == std::end(signal_filter));                      \
	if (print_signal) printf("%s-%s;", __func__, signalNames[(e)->signal]); }

#define HSM_DEBUG_LOG_TRANSITION(sH, e) {           \
        event_history[_ti].signal = (e)->signal;    \
        transition_history[_ti++] = (sH);           \
    }

#define HSM_DEBUG_LOG_ZERO(sH) {                            \
    _ti = 0;                                                \
    for(int i = 0; i < HISTORY_LENGTH; i++) \
    {                                                       \
        transition_history[i] = {};                         \
    }                                                       \
}

#else
#define HSM_DEBUG_LOG_STATE_EVENT(stateData, e) (void(stateData), void(e))
#define HSM_DEBUG_PRINT_HANDLER(x) (void(x))
#define HSM_DEBUG_NEWLINE() // will this work? empty define needed here.
#define HSM_DEBUG_PRINT_EVENT(e) (void(e))
#endif

//
// State Handler macros
//
#define CHANGE_STATE(current_state, new_state) (current_state->stateHandler = (state_handler)new_state), \
        HSM_DEBUG_PRINT_HANDLER("CHANGE_STATE"),                        \
        STATE_CHANGED
#define HANDLE_STATE() (HSM_DEBUG_PRINT_HANDLER("HANDLE_STATE"), STATE_HANDLED)
#define IGNORE_STATE(x) (void(x), HSM_DEBUG_PRINT_HANDLER("IGNORE_STATE"), STATE_IGNORED)
#define HANDLE_SUPER_STATE(state, super_state) (state->stateHandler = (state_handler)super_state),\
													 HSM_DEBUG_PRINT_HANDLER("HANDLE_SUPER_STATE"), \
													 STATE_DO_SUPERSTATE

// Experiments


#endif
