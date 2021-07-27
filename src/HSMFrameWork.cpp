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
stringify(TRANSITION),
};

//int signal_filter[10] = {1, 2, 3, 4, 5};
int signal_filter[10] = {2};

bool print_signal = true;

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
  } while(path[index] != endHandler);

  hsmHandleEvent(state, &baseInitialEvent);

  //state->stateHandler = stateHandler;
}

void hsmHandleEvent(struct hsm_state *self, struct hsm_event * theEvent)
{
  // remember where we started from
  state_handler stateWithSignalHandler = {};
  state_handler startHandler = self->stateHandler;

  hsm_state_result currentResult;
  // While asked to do the super state, do it.
  // i.e. while event signal not handled
  state_handler currentHandler = self->stateHandler;
  do
  {
    stateWithSignalHandler = self->stateHandler;
    currentResult = currentHandler(self, theEvent);
    currentHandler = self->stateHandler;
  } while(currentResult == STATE_DO_SUPERSTATE);

  state_handler hierarchyPath[10] = {};

  // State changed, figure out LCA and do the appropriate exit/entry events.
  if(currentResult == STATE_CHANGED)
  {
    // Get hierarchy path of destination
    // self->stateHandler points to destination state
    hsmDiscoverHierarch(self, hierarchyPath,
                        sizeof(hierarchyPath)/sizeof(*hierarchyPath));

    // start with current state
    self->stateHandler = startHandler;
    do
    {

      int index = hsmCheckForHandlerInPath(&(self->stateHandler),
                                           hierarchyPath,
                                           sizeof(hierarchyPath)/sizeof(*hierarchyPath));
      if(index == 0)
      {
        // Destination is source, i.e. self transition
        if (currentHandler == stateWithSignalHandler)
        {
          self->stateHandler(self, &baseExitEvent);
          self->stateHandler(self, &baseEntryEvent);
        }
        break;
      }
      else if (index < 0)
      {
        // handler is not in heirarchy, so we are transitioning up to parent.
        // exit current state, but don't need to enter new state.
        self->stateHandler(self, &baseExitEvent);

      }
      else if( index > 0) // state is in destination hierarchy, entry event on that state
      {
        state_handler* currentLCAHandler = &(self->stateHandler);
        self->stateHandler = startHandler;

        // Do Exit Events up to LCA on current side.
        while(self->stateHandler != *currentLCAHandler)
        {
          self->stateHandler(self, &baseExitEvent);
          self->stateHandler(self, &baseSilentEvent);
        }

        // Do Entry Events down from LCA on destination side.
        int entryIndex;
        for(entryIndex = index-1; entryIndex >= 0; entryIndex--)
        {
          hierarchyPath[entryIndex](self, &baseEntryEvent);
        }
        self->stateHandler = hierarchyPath[0];
        break;
      }
    } while(STATE_DO_SUPERSTATE == self->stateHandler(self, &baseSilentEvent));
  }
  else
  {
    self->stateHandler = startHandler;
  }
}

void hsmProcess(struct hsm_state * self)
{
  struct hsm_event * theEvent;

  if(fifoSize() > 0)
  {
    theEvent = fifoPop();
    hsmHandleEvent(self, theEvent);
    hsmHandleEvent(self, &baseInitialEvent);
    hsmEventDelete(theEvent);
    //HSM_DEBUG_NEWLINE();
  }
  else
  {
    // call idle function (macro)
  }
}
