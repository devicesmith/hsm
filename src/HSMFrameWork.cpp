#include "../inc/HSMFrameWork.h"

#include <iostream>

//int signal_filter[10] = {1, 2, 3, 4, 5};
int signal_filter[10] = {2};

bool print_signal = true;

#ifdef HSM_DEBUG_LOGGING
// history for state tracking
state_handler transition_history[HISTORY_LENGTH];
struct hsm_event event_history[HISTORY_LENGTH];
int _ti {0};
#endif

//struct hsm_event* hsmEventQueue[EVENT_QUEUE_DEPTH];
struct hsm_event_pool hsmEventPool;
struct hsm_event baseEntryEvent;
struct hsm_event baseExitEvent;
struct hsm_event baseSilentEvent;
struct hsm_event baseInitialEvent;

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

void queueInit(struct fifo_data* fifo, struct hsm_event ** queue, int depth)
{
  semaphore_locked = false;
  fifo->head = 0;
  fifo->tail = 0;
  fifo->size = 0;
  fifo->depth = depth;
  fifo->hsmEvents = queue;
}

int queueDepth(struct fifo_data* fifo)
{
  return fifo->depth;
}

int queueSize(struct fifo_data* fifo)
{
  return fifo->size;
}

bool queuePush(struct fifo_data* fifo, struct hsm_event* e)
{
  semaphore_get();

  if(fifo->size < fifo->depth)
  {
    fifo->hsmEvents[fifo->tail] = e;
    fifo->tail = (fifo->tail + 1) % fifo->depth;
    fifo->size++;

    semaphore_release();
    return true;
  }
  semaphore_release();
  return false;
}

struct hsm_event* queuePop(struct fifo_data* fifo)
{
  semaphore_get();
  hsm_event* e = NULL;

  if(fifo->size > 0)
  {
    e = fifo->hsmEvents[fifo->head];
    fifo->head = (fifo->head + 1) % fifo->depth;
    fifo->size--;
  }
  semaphore_release();
  return e;
}

#if 0
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
#endif

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

bool hsmEventQueueInit(struct fifo_data* fifo, struct hsm_event ** eventQueueArray, int arrayLength)
{
  queueInit(fifo, eventQueueArray, arrayLength);

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

  return index - 1;
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

bool hsmCheckForSameHierarchy(state_handler* exitHierarchyPath, int exitPathDepth,
                              state_handler* entryHeiarchyPath, int entryPathDepth)
{
  bool exitPathIsLonger = (exitPathDepth >= entryPathDepth);

  state_handler* longerPath;
  state_handler* shorterPath;
  //int startLongDepth;
  int startShortDepth;
  bool sameHierarchy {false};
  int delta;

  if(exitPathIsLonger)
  {
    longerPath = exitHierarchyPath;
    shorterPath = entryHeiarchyPath;
    //startLongDepth = exitPathDepth;
    startShortDepth = entryPathDepth;
    delta = exitPathDepth - entryPathDepth;
  }
  else
  {
    longerPath = entryHeiarchyPath;
    shorterPath = exitHierarchyPath;
    //startLongDepth = entryPathDepth;
    startShortDepth = exitPathDepth;
    delta = entryPathDepth - exitPathDepth;
  }


  for(int i = startShortDepth; i >= 0; i--)
  {
    sameHierarchy = (shorterPath[i] == longerPath[i+delta]);
    if(!sameHierarchy) break;
  }

  return sameHierarchy;
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
    path[index](state, &baseEntryEvent);
    HSM_DEBUG_LOG_TRANSITION(path[index], &baseEntryEvent);
  } while(path[index--] != endHandler);

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

  //if(currentResult == STATE_CHANGED)
  if(theEvent->signal == HSM_SIG_INITIAL)
  {
    HSM_DEBUG_LOG_TRANSITION(beginStateHandler, theEvent);
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
    struct hsm_state beginState {};
    beginState.stateHandler = beginStateHandler;
    int exitDepth = hsmDiscoverHierarch(&beginState, exitHierarchyPath, ARRAY_LENGTH(exitHierarchyPath));

    struct hsm_state endState {};
    endState.stateHandler = endStateHandler;
    int entryDepth = hsmDiscoverHierarch(&endState, entryHierarchyPath, ARRAY_LENGTH(entryHierarchyPath));

    struct hsm_state eventState {};
    eventState.stateHandler = eventStateHandler;
//    hsmDiscoverHierarch(&eventState, exitHierarchyPath, ARRAY_LENGTH(exitHierarchyPath));

    bool sameHeirarchy = hsmCheckForSameHierarchy(exitHierarchyPath, exitDepth, entryHierarchyPath, entryDepth);


    int beginStateInEntryPos = hsmCheckForHandlerInPath(&(beginState.stateHandler), entryHierarchyPath,
                                                    sizeof(entryHierarchyPath)/sizeof(*entryHierarchyPath));
    int beginStateInExitPos = hsmCheckForHandlerInPath(&(beginState.stateHandler), exitHierarchyPath,
                                                     sizeof(exitHierarchyPath)/sizeof(*exitHierarchyPath));
    int endStateInExitPos = hsmCheckForHandlerInPath(&(endState.stateHandler), exitHierarchyPath,
                                                    sizeof(exitHierarchyPath)/sizeof(*exitHierarchyPath));
    int endStateInEntryPos = hsmCheckForHandlerInPath(&(endState.stateHandler), entryHierarchyPath,
                                                    sizeof(entryHierarchyPath)/sizeof(*entryHierarchyPath));
    int eventStateInEntryPos = hsmCheckForHandlerInPath(&(eventState.stateHandler), entryHierarchyPath,
                                                    sizeof(entryHierarchyPath)/sizeof(*entryHierarchyPath));

    //if(beginStateInEntryPos >= 0 && beginStateInExitPos >= 0) // No LCA will be needed
    if(sameHeirarchy)
    {
      self->stateHandler = beginStateHandler;
      if(beginStateInExitPos < endStateInExitPos)
      {
        // Move up the hierarchy
        int exitIndex = beginStateInExitPos;
        while (self->stateHandler != endStateHandler)
        {
          currentResult = self->stateHandler(self, &baseExitEvent);
          HSM_DEBUG_LOG_TRANSITION(self->stateHandler, &baseExitEvent);
          self->stateHandler = exitHierarchyPath[++exitIndex];
        }

        if(endStateHandler == eventStateHandler)
        {
          // FInish the self transition
          self->stateHandler(self, &baseExitEvent);
          HSM_DEBUG_LOG_TRANSITION(self->stateHandler, &baseEntryEvent);
          self->stateHandler(self, &baseEntryEvent);
          HSM_DEBUG_LOG_TRANSITION(self->stateHandler, &baseEntryEvent);
        }
      }
      else if(endStateInEntryPos < beginStateInEntryPos)
      {
        // move down the hierarchy
        for(int entryIndex = beginStateInEntryPos - 1; self->stateHandler != endStateHandler; entryIndex--)
        {
          self->stateHandler = entryHierarchyPath[entryIndex];
          currentResult = self->stateHandler(self, &baseEntryEvent);
          HSM_DEBUG_LOG_TRANSITION(self->stateHandler, &baseEntryEvent);
        }

        if(endStateHandler == eventStateHandler)
        {
          // FInish the self transition
          self->stateHandler(self, &baseExitEvent);
          HSM_DEBUG_LOG_TRANSITION(self->stateHandler, &baseEntryEvent);
          self->stateHandler(self, &baseEntryEvent);
          HSM_DEBUG_LOG_TRANSITION(self->stateHandler, &baseEntryEvent);
        }
      }
      else if(endStateHandler == beginStateHandler) // self transition
      {
        if(exitDepth > 0)
        {
          // exit up to just before state that handled the event
          self->stateHandler = beginStateHandler;
          while(self->stateHandler != eventStateHandler)
          {
            self->stateHandler(self, &baseExitEvent);
            HSM_DEBUG_LOG_TRANSITION(self->stateHandler, &baseExitEvent);
            self->stateHandler(self, &baseSilentEvent);
          }

          // enter down

           for(int entryIndex = eventStateInEntryPos - 1; self->stateHandler != endStateHandler; entryIndex--)
           {
             self->stateHandler = entryHierarchyPath[entryIndex];
             currentResult = self->stateHandler(self, &baseEntryEvent);
             HSM_DEBUG_LOG_TRANSITION(self->stateHandler, &baseEntryEvent);
           }
        }
        else
        {
          self->stateHandler(self, &baseExitEvent);
          HSM_DEBUG_LOG_TRANSITION(self->stateHandler, &baseExitEvent);
          self->stateHandler(self, &baseEntryEvent);
          HSM_DEBUG_LOG_TRANSITION(self->stateHandler, &baseEntryEvent);
        }
      }
    }
    else // exits until LCR or eventHandler
    {
      // Move up until at LCA
      self->stateHandler = beginStateHandler;
      do
      {
        self->stateHandler(self, &baseExitEvent);
        HSM_DEBUG_LOG_TRANSITION(self->stateHandler, &baseExitEvent);
        self->stateHandler(self, &baseSilentEvent);
      } while( hsmCheckForHandlerInPath(&(self->stateHandler),entryHierarchyPath,
                                        ARRAY_LENGTH(entryHierarchyPath)) < 0 );

      // At LCA. Find where to start
      int entryStart = hsmCheckForHandlerInPath(&(self->stateHandler), entryHierarchyPath,
                                                ARRAY_LENGTH(entryHierarchyPath));
      entryStart--;

      for(int entryIndex = entryStart; self->stateHandler != endStateHandler; entryIndex--)
      {
        self->stateHandler = entryHierarchyPath[entryIndex];
        HSM_DEBUG_LOG_TRANSITION(self->stateHandler, &baseEntryEvent);
        currentResult = self->stateHandler(self, &baseEntryEvent);
      }
    }

    return PROCESS_STATE_CHANGED;
  }
  self->stateHandler = beginStateHandler;
  return PROCESS_HANDLED;
}

void hsmProcess(struct hsm_state * self, struct fifo_data *fifo)
{
  struct hsm_event * theEvent;

  if(queueSize(fifo) > 0)
  {
    theEvent = queuePop(fifo);
    if(hsmHandleEvent(self, theEvent) == PROCESS_STATE_CHANGED)
    {
      while(hsmHandleEvent(self, &baseInitialEvent) == PROCESS_STATE_CHANGED);
    }

    hsmEventDelete(theEvent);
    //HSM_DEBUG_NEWLINE();
  }
  else
  {
    // call idle function (macro)
  }
}
