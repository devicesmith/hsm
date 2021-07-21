//
//  hsm.cpp
//  hsm
//
//  Created by Matthew McFadden on 10/29/15.
//  Copyright © 2015 Device Smith. All rights reserved.
//

#include "hsm.h"

//int signal_filter[10] = {1, 2, 3, 4, 5};
int signal_filter[10] = {5};
bool print_signal = false;

int incrWithWrap(int var, int max)
{
   var  = var < max ? var + 1 : 0;
   return var;
}

//
// hsm_state_t
//
void hsm_state_t::InQueuePush(hsm_event *e)
{
	this->inQueue.push(e);
}

hsm_event_t * hsm_state_t::InQueuePop()
{
	hsm_event_t * e = this->inQueue.front();
	this->inQueue.pop();
	return e;
}

size_t hsm_state_t::InQueueGetSize()
{
	return this->inQueue.size();
}

void hsm_state_t::OutQueuePush(hsm_event *e)
{
	this->outQueue.push(e);
}

hsm_event_t * hsm_state_t::OutQueuePop()
{
	hsm_event_t * e = this->outQueue.front();
	this->outQueue.pop();
	return e;
}

size_t hsm_state_t::OutQueueGetSize()
{
	return this->outQueue.size();
}

state_handler_t hsm_state_t::GetStateHandler()
{
	return stateHandler;
}

void hsm_state_t::SetStateHandler(state_handler_t st)
{
	stateHandler = st;
}

hsm_state_result_t hsm_state_t::StateHandler(hsm_event_t const * e)
{
	return stateHandler(this,e);
}

hsm_state_t:: ~hsm_state_t()
{

}


//
// HSM
//
HSM::HSM()
{
    silentEvent.signal = HSM_SIG_SILENT;
    entryEvent.signal = HSM_SIG_ENTRY;
    exitEvent.signal = HSM_SIG_EXIT;
    initEvent.signal = HSM_SIG_INITIAL_TRANS;
}

hsm_state_result_t HSM::callStateHandler(hsm_state_t * state,
										hsm_event_t const * e,
										bool log)
{
	hsm_state_result_t result;
	//state_handler_t prevState = state->stateHandler;
	state_handler_t prevState = state->GetStateHandler();

	result = state->StateHandler(e);

	return result;
}

hsm_state_result_t HSM::rootState(hsm_state_t * me,
                                  hsm_event_t const *hsmEvent)
{
	HSM_DEBUG_LOG_STATE_EVENT(me, hsmEvent);
	return IGNORE_STATE();
}


int HSM::CheckForHandlerInPath(hsm_state_t * state,
                               hsm_state_t pathToStateArray[],
                               int pathToStateArrayDepth)
{
  for(int i = 0; i < pathToStateArrayDepth; ++i)
  {
	if (state->GetStateHandler() == pathToStateArray[i].GetStateHandler())
    {
      return i;
    }
  }

  return -1;
}

// Returns the depth, ie number of states, including rootState
int HSM::DiscoverHierarchyToRootState(hsm_state_t * state,
                                       hsm_state_t pathToTargetStateArray[],
                                       int pathToTargetMaxDepth )
{
    if( pathToTargetStateArray == nullptr ||
        pathToTargetMaxDepth < 1 )
    {
        return 0;
    }

    hsm_state_result_t stateResult;
    int index = 0;

    hsm_state_t originalState;
	originalState.SetStateHandler(state->GetStateHandler());

    do
    {
        pathToTargetStateArray[index++] = *state;
        stateResult = callStateHandler(state, &silentEvent, false);
    } while (stateResult == HSM_STATE_DO_SUPERSTATE && index <
             pathToTargetMaxDepth);

	state->SetStateHandler(originalState.GetStateHandler());

    return index;

}

// Returns the depth, ie number of states, including rootState
int HSM::DiscoverHierarchy(hsm_state_t * topState,
                            hsm_state_t * bottomState,
                            hsm_state_t pathToTargetStateArray[],
                            int pathToTargetMaxDepth )
{
   if(  pathToTargetStateArray == nullptr ||
        pathToTargetMaxDepth < 1 )
    {
        return 0;
    }

    hsm_state_result_t stateResult;
    int index = 0;

    hsm_state_t originalState;
	originalState.SetStateHandler(bottomState->GetStateHandler());

	while( (bottomState->GetStateHandler() != topState->GetStateHandler()) &&
           index < pathToTargetMaxDepth)
    {
        pathToTargetStateArray[index++] = *bottomState;
        stateResult = callStateHandler(bottomState, &silentEvent, false);
    }

	bottomState->SetStateHandler(originalState.GetStateHandler());

    return index;

}

void HSM::SetInitialState(state_handler_t initialState)
{
   // Call Init to top, root state as per spec
   hsm_state_t topState;
   //topState.stateHandler = rootState;
   topState.SetStateHandler(rootState);
   callStateHandler(&topState, &initEvent, true);

   hsm_state_t pathToTargetState[STATE_DEPTH_MAX];

   hsm_state_t destinationState;
   destinationState.SetStateHandler(initialState);
   GetStateData()->SetStateHandler(initialState);

   do
   {
      int depth = DiscoverHierarchy(&topState, &destinationState,
                                    pathToTargetState,
                                    ARRAY_LENGTH(pathToTargetState));
      int index = depth - 1;
      // Call Entry event from first state to initial state
      do {
          callStateHandler(&pathToTargetState[index], &entryEvent, true);
      }	while (pathToTargetState[index--].GetStateHandler() !=
				GetStateData()->GetStateHandler());

	  topState.SetStateHandler(pathToTargetState[0].GetStateHandler());
	  destinationState.SetStateHandler(pathToTargetState[0].GetStateHandler());
	  GetStateData()->SetStateHandler(pathToTargetState[0].GetStateHandler());

   } while (callStateHandler(GetStateData(), &initEvent, true) ==
            HSM_STATE_CHANGED);
}

void HSM::OutQueuePush(hsm_event_t *e)
{
	GetStateData()->OutQueuePush(e);
}

hsm_event_t * HSM::OutQueuePop()
{
	hsm_event_t * e = GetStateData()->OutQueuePop();
	return e;
}

size_t HSM::OutQueueGetSize()
{
	return GetStateData()->OutQueueGetSize();
}

void HSM::InQueuePush(hsm_event_t *e)
{
	GetStateData()->InQueuePush(e);
}

hsm_event_t * HSM::InQueuePop()
{
	hsm_event_t * e = GetStateData()->InQueuePop();
	return e;
}

size_t HSM::InQueueGetSize()
{
	return GetStateData()->InQueueGetSize();
}

//
// struct hsm_state* me will be updated with new state if a transition occurred.
//
void HSM::Process()
{
	ProcessInQueue(GetStateData());

}

void HSM::ProcessInQueue(hsm_state_t * st)
{
   // remember current state
   hsm_state_t initialState;
   initialState.SetStateHandler(st->GetStateHandler());

   if(st->InQueueGetSize() > 0)
   {
	  hsm_event_t * e = st->InQueuePop();
      hsm_state_result_t currentResult;
      bool selfTrans = false;
      bool processing = false;
      bool backToSelfTop = false;
      bool backToSelfBottom = false;

    	hsm_state_t lastState;
    	hsm_state_t stateHandlingEvent;

      // Handle Event in this state or one of its parents
      // Also keep track of who did what to handle special
      // cases.
	  lastState.SetStateHandler(st->GetStateHandler());
      do
      {
         // remember what state handled event
		 stateHandlingEvent.SetStateHandler(st->GetStateHandler());
         // Handle event in hierarchy path
         currentResult = callStateHandler(st, e, true);
		 selfTrans = (st->GetStateHandler() == lastState.GetStateHandler());
		 lastState.SetStateHandler(st->GetStateHandler());
      } while(currentResult == HSM_STATE_DO_SUPERSTATE);

      backToSelfTop = ((st->GetStateHandler() == initialState.GetStateHandler()) &&
                       (st->GetStateHandler() != stateHandlingEvent.GetStateHandler()));

      if (currentResult == HSM_STATE_HANDLED ||
          currentResult == HSM_STATE_IGNORED )
      {
         // Event was handled (or ignored) and no transition
         // so stay in current state
		 st->SetStateHandler(initialState.GetStateHandler());
      }
      else if (currentResult == HSM_STATE_CHANGED)
      {
		hsm_state_t destinationState;
        hsm_state_t pathToTargetState[STATE_DEPTH_MAX];

		 destinationState.SetStateHandler(st->GetStateHandler());
		 st->SetStateHandler(initialState.GetStateHandler());

         processing = true;
         while(processing)
         {
            // Event handled and a transition occurred.
            // Needs copy constructor : memset(pathToTargetState, 0, sizeof(pathToTargetState));
            DiscoverHierarchyToRootState(&destinationState,
                                         pathToTargetState,
                                         ARRAY_LENGTH(pathToTargetState));

            // Walk through all the exit and entry actions from current state
            // to the destination state.
            do
            {
               int index = CheckForHandlerInPath(st, pathToTargetState,
                                                 ARRAY_LENGTH(pathToTargetState));

               if (index == 0) // At handling state or destination state.
               {
                  if (selfTrans)
                  {
                     // At destination, handle exit if appropriate
                     callStateHandler(st, &exitEvent, true);
                  }
                  else if (backToSelfTop)
                  {
                     // Transtioning to self. Setup exiting through parents
					 destinationState.SetStateHandler(stateHandlingEvent.GetStateHandler());
                     backToSelfTop = false;
                     backToSelfBottom = true;
                     break;
                  }
                  else if (backToSelfBottom)
                  {
                     // Transitioning to self, setup entering from parents
					 destinationState.SetStateHandler(lastState.GetStateHandler());
                     backToSelfBottom = false;
                     break;
                  }
                  else
                  {
                     // At the destination state. Call Init events
                     if (HSM_STATE_CHANGED != callStateHandler(&destinationState,
                                                               &initEvent, true))
                     {
						st->SetStateHandler(pathToTargetState[0].GetStateHandler());
                        processing = false;
                     }
                     break;
                  }
               }
               else if( index > 0) // Destination state is in hierachry.
               {
                  // Do Entry Events down from LCA on destination side.
                  int entryIndex;
                  for(entryIndex = index - 1; entryIndex >= 0; entryIndex--)
                  {
                     // Handle Entry Event with original hsm_state
					 st->SetStateHandler(pathToTargetState[entryIndex].GetStateHandler());
					 callStateHandler(st, &entryEvent, true);
                  }
                  // Handle Init event
				  st->SetStateHandler(pathToTargetState[0].GetStateHandler());
                  if (HSM_STATE_CHANGED == callStateHandler(st, &initEvent, true))
                  {
					 destinationState.SetStateHandler(st->GetStateHandler());
					 stateHandlingEvent.SetStateHandler(pathToTargetState[0].GetStateHandler());
                  }
                  else
                  {
                     processing = false;
                  }
				  st->SetStateHandler(pathToTargetState[0].GetStateHandler());
                  break;
               }
               else // In a different tree, head up hierarchy
               {
                  // Handler is not in heirarchy path, so call exit event.
                  callStateHandler(st, &exitEvent, true);
               }
               // transition up to parent
            } while (HSM_STATE_DO_SUPERSTATE ==
                     callStateHandler(st, &silentEvent, false));
         }
      }
   }
}

