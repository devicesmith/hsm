#include "HSMFrameWork.h"

#include <iostream>

int signal_filter[10] = {1, 2, 3, 4, 5};
//int signal_filter[10] = {5};

bool print_signal = true;

//struct hsm_event* hsmEventQueue[EVENT_QUEUE_DEPTH];
struct hsm_event_pool hsmEventPool;
struct hsm_event baseEntryEvent;
struct hsm_event baseExitEvent;
struct hsm_event baseSilentEvent;

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
  hsm_event* e;

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

  for(i=0; i < hsmEventPool.arrayLength; i++)
  {
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
}

int hsmDiscoverHierarch(struct hsm_state* state, state_handler *path, int pathDepth)
{
  if(!path || *path == 0)
  {
    return false;
  }

  hsm_state_result stateResult;
  int index = 0;
  struct hsm_state* initialState = state;
  do
  {
    path[index++] = state->stateHandler;
    // Call super state and update state->handler
    stateResult = state->stateHandler(state, &baseSilentEvent);
  } while((stateResult == STATE_DO_SUPERSTATE) && (index < pathDepth));

  //printf("\n");
  state = initialState;
  
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
  state_handler path[10];
  int index = 0;
  
  state_handler endHandler = stateHandler;
  state->stateHandler = stateHandler;
  index = hsmDiscoverHierarch(state, path, sizeof(path)/sizeof(*path));
  printf("\n");
  do
  {
    path[--index](state, &baseEntryEvent);
  } while(path[index] != endHandler);

  state->stateHandler = stateHandler;
  printf("\n");
}

void hsmProcess(struct hsm_state * self)
{
  hsm_state_result currentResult;
  state_handler startHandler;
  state_handler destinationHandler;
  struct hsm_event * theEvent;
  int index;
  state_handler hierarchyPath[10];
  state_handler currentHandler;

  if(fifoSize() > 0)
  {
    theEvent = fifoPop();

    // remeber where we started from
    startHandler = self->stateHandler;

    // While asked to do the super state, do it.
    currentHandler = self->stateHandler;
    do
    {
      currentResult = currentHandler(self, theEvent);
      currentHandler = self->stateHandler;
    } while(currentResult == STATE_DO_SUPERSTATE);

    // State changed, figure out LCA and do the appropriate exit/entry events.
    if(currentResult == STATE_CHANGED)
    {
      // remember where we are going
      destinationHandler = self->stateHandler;

      // Get hierarchy path of destination
      index = hsmDiscoverHierarch(self, hierarchyPath,
                                  sizeof(hierarchyPath)/sizeof(*hierarchyPath));

      // exit to parent state
      self->stateHandler = startHandler;
      do
      {
        // state is in destination hierarchy, entry event on that state
        index = hsmCheckForHandlerInPath(&(self->stateHandler),
                                         hierarchyPath,
                                         sizeof(hierarchyPath)/sizeof(*hierarchyPath));
        if(index == 0)
        {
          // We at the right state
          break;
        }
        else if( index > 0)
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
        else 
        {
          // handler is not in heirarchy, so we are transitioning up to parent.
          // exit current state, but don't need to enter new state.
          self->stateHandler(self, &baseExitEvent);
        }
      } while(STATE_DO_SUPERSTATE == self->stateHandler(self, &baseSilentEvent));
    }
    else
    {
      self->stateHandler = startHandler;
    }
    hsmEventDelete(theEvent);
  }
  else
  {
    // call idle function
  }
}
