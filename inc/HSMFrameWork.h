#ifndef __HSMFRAMEWORK_H__
#define __HSMFRAMEWORK_H__

#define ARRAY_LENGTH(array) (sizeof(array)/sizeof(*(array)))

typedef enum 
{
  STATE_IGNORED,
  STATE_HANDLED,
  STATE_CHANGED,
  STATE_DO_SUPERSTATE
} hsm_state_result;

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
  hsm_signal signal;
  bool inUse;
};

struct hsm_event_pool
{
  hsm_event * eventArray;
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
  hsm_event** hsmEvents;
};

void hsmInitStateMachine(void);
void hsmInitialState(struct hsm_state* state, state_handler stateHandler);
void hsmProcess(hsm_state * self);
  
void fifoInit(struct hsm_event **eventStart, int depth);
int fifoDepth();
int fifoSize();
bool fifoPush(hsm_event* e);
struct hsm_event* fifoPop(void);
bool hsmEventQueueInit(hsm_event ** eventQueueArray, int arrayLength);

bool hsmEventPoolInit(hsm_event * eventArray, int arrayLength);
hsm_event * hsmEventNew(void);
void hsmEventDelete(void *);

extern void (*pLogCallback)(char *);

#define HSM_DEBUG_LOGGING

#ifdef HSM_DEBUG_LOGGING

extern int signal_filter[10];
extern bool print_signal;

#define HSM_DEBUG_LOG_STATE_EVENT(stateData, e) { \
	int *f = std::find(std::begin(signal_filter), std::end(signal_filter), e->signal); \
	print_signal = (f == std::end(signal_filter)); \
	if (print_signal) printf("%s(e:%d) -> ", __func__, (e)->signal); }

#define HSM_DEBUG_PRINT(x) ({if(print_signal) printf("%s [%s] ", __func__, (x));})

#else
#define HSM_DEBUG_LOG_STATE_EVENT(stateData, e) (void(stateData), void(e))
#define HSM_DEBUG_PRINT(x) (void(x))
#endif


#define CHANGE_STATE(current_state, new_state) (current_state->stateHandler = new_state), \
					HSM_DEBUG_PRINT("CHANGE_STATE"), STATE_CHANGED

#define HANDLE_STATE() (HSM_DEBUG_PRINT("HANDLE_STATE"), STATE_HANDLED)

#define IGNORE_STATE(x) (void(x), HSM_DEBUG_PRINT("IGNORE_STATE"), STATE_IGNORED)

#define HANDLE_SUPER_STATE(state, super_state) (state->stateHandler = super_state),\
													 HSM_DEBUG_PRINT("HANDLE_SUPER_STATE"), \
													 STATE_DO_SUPERSTATE



#endif
