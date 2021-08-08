#include "../inc/HSMFrameWork.h"

#include <iostream>

// Must match enum
#define stringify( name ) # name
const char* signalNames[] = {
stringify(UNKNOWN),
stringify(INIT),
stringify(SILENT),
stringify(ENTRY),
stringify(EXIT),
stringify(INITIAL),
stringify(A),
stringify(B),
stringify(C),
stringify(D),
stringify(E),
stringify(F),
stringify(G),
stringify(H),
stringify(I),
stringify(J),
stringify(K),
stringify(L),
stringify(TRANSITION)
};

//int signal_filter[10] = {1, 2, 3, 4, 5};
int signal_filter[10] = {2};

bool print_signal = true;

// history for state tracking
state_handler transition_history[20];
int _ti {0};

//struct hsm_event* hsmEventQueue[EVENT_QUEUE_DEPTH];
struct hsm_event_pool hsmEventPool;
struct hsm_event baseEntryEvent;
struct hsm_event baseExitEvent;
struct hsm_event baseSilentEvent;
struct hsm_event baseInitialEvent;

struct fifo_data fifoData;

bool semaphore_locked;
void semaphore_get(void)
{
  while(semaphore_locked)
  {
//    TRACE(_T("WAIT ON SEMAPHORE"));
  }
  semaphore_locked = true;
}

void semaphore_release(void)
{
  semaphore_locked = false;
}

void fifoInit(struct hsm_event ** queue, int depth)
{
  semaphore_locked = false;
  fifoData.head = 0;
  fifoData.tail = 0;
  fifoData.size = 0;
  fifoData.depth = depth;
  fifoData.hsmEvents = queue;
}

int fifoDepth()
{
  return fifoData.depth;
}

int fifoSize()
{
  return fifoData.size;
}

bool fifoPush(struct hsm_event* e)
{
  semaphore_get();

  if(fifoData.size < fifoData.depth)
  {
    fifoData.hsmEvents[fifoData.tail] = e;
    fifoData.tail = (fifoData.tail + 1) % fifoData.depth;
    fifoData.size++;

    semaphore_release();
    return true;
  }
  semaphore_release();
  return false;
}

struct hsm_event* fifoPop(void)
{
  semaphore_get();
  hsm_event* e = NULL;

  if(fifoData.size > 0)
  {
    e = fifoData.hsmEvents[fifoData.head];
    fifoData.head = (fifoData.head + 1) % fifoData.depth;
    fifoData.size--;
  }
  semaphore_release();
  return e;
}

bool hsmEventPoolInit(hsm_event * eventArray, int arrayLength)
{
  int i;
  for(i = 0; i < arrayLength; i++)
  {
    eventArray[i].inUse = false;
  }
  hsmEventPool.eventArray = eventArray;
  hsmEventPool.arrayLength = arrayLength;
  
  return true;
}

bool hsmEventQueueInit(hsm_event ** eventQueueArray, int arrayLength)
{
  fifoInit(eventQueueArray, arrayLength);

  return true;
}

hsm_event * hsmEventNew(void)
{
  int i;

  //printf("hsmEventNew - array length:%d\n", hsmEventPool.arrayLength);

  for(i=0; i < hsmEventPool.arrayLength; i++)
  {
    //printf("hsmEventNew:%d (%d)\n", i, hsmEventPool.eventArray[i].inUse);
    if(!hsmEventPool.eventArray[i].inUse)
    {
      hsmEventPool.eventArray[i].inUse = true;
      return &hsmEventPool.eventArray[i];
    }
  }
  return 0;
}

bool hsmEventDelete(hsm_event * e)
{
  int i;

  for(i = 0; i < hsmEventPool.arrayLength; i++)
  {
    if(e == &hsmEventPool.eventArray[i] )
    {
      hsmEventPool.eventArray[i].inUse = false;
      return true;
    }
  }
  return false;
}

void hsmInitStateMachine(void)
{
  baseEntryEvent.signal  = HSM_SIG_ENTRY;
  baseExitEvent.signal   = HSM_SIG_EXIT;
  baseSilentEvent.signal = HSM_SIG_SILENT;
  baseInitialEvent.signal = HSM_SIG_INITIAL;
}

int hsmDiscoverHierarch(struct hsm_state* state, state_handler *path, int pathDepth)
{
  //printf("==>hsmDiscoverHierarch<==\n");
  if(!path)
  {
    return false;
  }

  hsm_state_result stateResult;
  int index = 0;
  //struct hsm_state initialState = *state;
  struct hsm_state currentState = *state;

  do
  {
    path[index++] = currentState.stateHandler;
    // Call super state and update state->handler
    stateResult = currentState.stateHandler(&currentState, &baseSilentEvent);
  } while((stateResult == STATE_DO_SUPERSTATE) && (index < pathDepth));

  //HSM_DEBUG_NEWLINE();

  return index;
}

int hsmCheckForHandlerInPath(state_handler* stateHandler, state_handler* path, int pathDepth)
{
  for(int i = 0; i < pathDepth; i++)
  {
    if(*stateHandler == path[i])
    {
      return i;
    }
  }

  return -1;
}

// Correctly transition to initial state within the state machine.
void hsmInitialState(struct hsm_state* state, state_handler stateHandler)
{
  //printf("==>hsmInitialState<==\n");
  state_handler path[10];
  int index = 0;

  state_handler endHandler = stateHandler;
  state->stateHandler = stateHandler;
  index = hsmDiscoverHierarch(state, path, sizeof(path)/sizeof(*path));

  do
  {
    path[--index](state, &baseEntryEvent);
    HSM_DEBUG_LOG_TRANSITION(path[index]);
  } while(path[index] != endHandler);

  //hsmHandleEvent(state, &baseInitialEvent);
  while( hsmHandleEvent(state, &baseInitialEvent) == PROCESS_STATE_CHANGED );

}

hsm_process_result hsmHandleEvent(struct hsm_state *self, struct hsm_event * theEvent)
{
  // remember where we started from
  state_handler eventStateHandler = {};
  state_handler beginStateHandler = self->stateHandler;

  hsm_state_result currentResult;
  // While asked to do the super state, do it.
  // i.e. while event signal not handled
  state_handler currentHandler = self->stateHandler;
  do
  {
    eventStateHandler = self->stateHandler;
    currentResult = currentHandler(self, theEvent);
    currentHandler = self->stateHandler;
  } while(currentResult == STATE_DO_SUPERSTATE);

  if(theEvent->signal == HSM_SIG_INITIAL)
  {
    HSM_DEBUG_LOG_TRANSITION(beginStateHandler);
  }

  state_handler exitHierarchyPath[10] = {};
  state_handler entryHierarchyPath[10] = {};

  // State changed, figure out LCA and do the appropriate exit/entry events.
  if(currentResult == STATE_CHANGED)
  {
    // self->stateHandler points to eventStateHandler.
    state_handler endStateHandler = self->stateHandler;

    // Get hierarchy path of destination
    // self->stateHandler points to destination state
    struct hsm_state endState {};
    endState.stateHandler = endStateHandler;
    hsmDiscoverHierarch(&endState, entryHierarchyPath,
                        sizeof(entryHierarchyPath)/sizeof(*entryHierarchyPath));

    struct hsm_state eventState {};
    eventState.stateHandler = eventStateHandler;
    hsmDiscoverHierarch(&eventState, exitHierarchyPath,
                        sizeof(exitHierarchyPath)/sizeof(*exitHierarchyPath));

    struct hsm_state beginState {};
    beginState.stateHandler = beginStateHandler;
    hsmDiscoverHierarch(&beginState, exitHierarchyPath,
                        sizeof(exitHierarchyPath)/sizeof(*exitHierarchyPath));

    int beginStateInEntryPos = hsmCheckForHandlerInPath(&(beginState.stateHandler), entryHierarchyPath,
                                                    sizeof(entryHierarchyPath)/sizeof(*entryHierarchyPath));

    int beginStateInExitPos = hsmCheckForHandlerInPath(&(beginState.stateHandler), exitHierarchyPath,
                                                     sizeof(exitHierarchyPath)/sizeof(*exitHierarchyPath));

    int eventStateInEntryPos = hsmCheckForHandlerInPath(&(eventState.stateHandler), entryHierarchyPath,
                                                    sizeof(entryHierarchyPath)/sizeof(*entryHierarchyPath));

    int endStateInExitPos = hsmCheckForHandlerInPath(&(eventState.stateHandler), exitHierarchyPath,
                                                    sizeof(exitHierarchyPath)/sizeof(*exitHierarchyPath));

    if(beginStateInEntryPos >= 0 && beginStateInExitPos >= 0) // No LCA will be needed
    {
      self->stateHandler = beginStateHandler;
      if(beginStateInExitPos < endStateInExitPos)
      {
        // Move up the hierarchy
        for(int exitIndex = beginStateInExitPos; self->stateHandler != endStateHandler; exitIndex--)
        {
          self->stateHandler = exitHierarchyPath[exitIndex];
          currentResult = self->stateHandler(self, &baseExitEvent);
          HSM_DEBUG_LOG_TRANSITION(self->stateHandler);
        }
      }
      else
      {
        // move down the hierarchy
        for(int entryIndex = beginStateInEntryPos - 1; self->stateHandler != endStateHandler; entryIndex--)
        {
          self->stateHandler = entryHierarchyPath[entryIndex];
          currentResult = self->stateHandler(self, &baseEntryEvent);
          HSM_DEBUG_LOG_TRANSITION(self->stateHandler);
        }
      }
    }
    else if(endStateHandler == eventStateHandler) // self transition
    {
      // Move up the hierarchy
      self->stateHandler = beginStateHandler;
      for(int exitIndex = beginStateInExitPos; self->stateHandler != endStateHandler; exitIndex--)
      {
        self->stateHandler = exitHierarchyPath[exitIndex];
        currentResult = self->stateHandler(self, &baseExitEvent);
        HSM_DEBUG_LOG_TRANSITION(self->stateHandler);
      }

      // FInish the self transition
      self->stateHandler(self, &baseEntryEvent);
      HSM_DEBUG_LOG_TRANSITION(self->stateHandler);
    }
    else // exits until LCR or eventHandler
    {
      // find LCA
      self->stateHandler = beginStateHandler;
      do
      {
        self->stateHandler(self, &baseExitEvent);
        HSM_DEBUG_LOG_TRANSITION(self->stateHandler);
        self->stateHandler(self, &baseSilentEvent);
      } while( hsmCheckForHandlerInPath(&(self->stateHandler),entryHierarchyPath,
                                        ARRAY_LENGTH(entryHierarchyPath)) < 0 );

      for(int entryIndex = 0; self->stateHandler != endStateHandler; entryIndex++)
      {
        self->stateHandler = entryHierarchyPath[entryIndex];
        HSM_DEBUG_LOG_TRANSITION(self->stateHandler);
        currentResult = self->stateHandler(self, &baseEntryEvent);
      }
    }

    return PROCESS_STATE_CHANGED;
  }
  self->stateHandler = beginStateHandler;
  return PROCESS_HANDLED;
}

void hsmProcess(struct hsm_state * self)
{
  struct hsm_event * theEvent;

  if(fifoSize() > 0)
  {
    theEvent = fifoPop();
    if( hsmHandleEvent(self, theEvent) == PROCESS_STATE_CHANGED)
    {
      while( hsmHandleEvent(self, &baseInitialEvent) == PROCESS_STATE_CHANGED );
    }

    hsmEventDelete(theEvent);
    //HSM_DEBUG_NEWLINE();
  }
  else
  {
    // call idle function (macro)
  }
}
