#include "catch.hpp"
#include "inc.hpp"
#include "HSMFrameWork.h"


class TestHSM
{
public:
	// Extend the base state variable
	struct hsm_test_state : hsm_state
	{
		int extraStateInfo;
	} state;

	// Extend the base event
	struct hsm_test_event : hsm_event
	{
		int extraEventInfo;
	};

	// Extend the signal
	enum hsm_test_signal
	{
		TEST_SIG_START = HSM_SIG_USER,
		TEST_SIG_A,
		TEST_SIG_B,
		TEST_SIG_C,
		TEST_SIG_D,
		TEST_SIG_E,
		TEST_SIG_F,
		TEST_SIG_G,
		TEST_SIG_H,
		TEST_SIG_I,
		TEST_SIG_J,
		TEST_SIG_K,
		TEST_SIG_L,
		TEST_SIG_TRANSITION,
		TEST_SIG_LAST
	};

	// Init our test machine
	//void InitStateMachine(hsm_test_state * initialState);
	void InitStateMachine(hsm_state * state, state_handler initialState);

	// States in test
	static hsm_state_result RootState(struct hsm_state *self, struct hsm_event const *e);
	static hsm_state_result s0(struct hsm_state* self, struct hsm_event const * hsmEvent);
  	static hsm_state_result s1(struct hsm_state* self, struct hsm_event const * hsmEvent);
	static hsm_state_result s11(struct hsm_state* self, struct hsm_event const * hsmEvent);
	static hsm_state_result s12(struct hsm_state* self, struct hsm_event const * hsmEvent);
	static hsm_state_result s13(struct hsm_state* self, struct hsm_event const * hsmEvent);
	static hsm_state_result s2(struct hsm_state* self, struct hsm_event const * hsmEvent);
	static hsm_state_result s21(struct hsm_state* self, struct hsm_event const * hsmEvent);
	static hsm_state_result s211(struct hsm_state* self, struct hsm_event const * hsmEvent);
	static hsm_state_result s22(struct hsm_state* self, struct hsm_event const * hsmEvent);

	static hsm_state_result state0(struct hsm_state* self, struct hsm_event const * hsmEvent);
	static hsm_state_result state1(struct hsm_state* self, struct hsm_event const * hsmEvent);
	static hsm_state_result state2(struct hsm_state* self, struct hsm_event const * hsmEvent);
	static hsm_state_result state3(struct hsm_state* self, struct hsm_event const * hsmEvent);

};

//
// Implementation
//

// Main State Variables
struct TestHSM::hsm_test_state hsmTestStateMachine;
// Allocate Event Pool
struct TestHSM::hsm_test_event hsmTestEventPool[10];
// Allocate Event Qeueu
struct hsm_event* hsmTestEventQueue[10];

//void TestHSM::InitStateMachine(hsm_test_state * initialState)
void TestHSM::InitStateMachine(struct hsm_state* state, state_handler initialState)
{
	hsmEventPoolInit(hsmTestEventPool, sizeof(hsmTestEventPool));
	hsmEventQueueInit(hsmTestEventQueue, sizeof(hsmTestEventQueue));
	hsmInitStateMachine();
	hsmInitialState(state, initialState);
}

hsm_state_result TestHSM::RootState(struct hsm_state* self, struct hsm_event const * e)
{
	return HANDLE_STATE();
}

hsm_state_result TestHSM::s0(struct hsm_state* self, struct hsm_event const * e)
{
	switch(e->signal)
	{
		case HSM_SIG_ENTRY:
//			CAST_DIALOG(self)->WriteDisplay(_T("S0:Entry"));
//			return STATE_HANDLED;
			printf("S0:Entry\n");
			return HANDLE_STATE();
		case HSM_SIG_EXIT:
//			CAST_DIALOG(self)->WriteDisplay(_T("S0:Exit"));
			printf("S0:Exit\n");
			return HANDLE_STATE();
		case HSM_SIG_INITIAL:
			return HANDLE_STATE();
		case TEST_SIG_D:
			//self->stateHandler = s211;
			//return STATE_CHANGED;
			return CHANGE_STATE(self, &TestHSM::s211);
		case TEST_SIG_E:
			//self->stateHandler = s21;
			//return STATE_CHANGED;
			return CHANGE_STATE(self, &TestHSM::s21);
		case TEST_SIG_G:
			//self->stateHandler = s11;
			return STATE_CHANGED;
		case TEST_SIG_I:
			//self->stateHandler = s1;
			//return STATE_CHANGED;
			return CHANGE_STATE(self, &TestHSM::s1);
		case TEST_SIG_TRANSITION:
			//self->stateHandler = state0;
			//return STATE_CHANGED;
			return CHANGE_STATE(self, &TestHSM::state0);
	}
	return HANDLE_SUPER_STATE(self, &TestHSM::RootState);
	//self->stateHandler = RootState;
	//return STATE_DO_SUPERSTATE;
}

hsm_state_result TestHSM::s1(struct hsm_state* self, struct hsm_event const * hsmEvent)
{
  switch(hsmEvent->signal)
  {
    case HSM_SIG_ENTRY:
//      CAST_DIALOG(self)->WriteDisplay(_T("S1:Entry"));
		printf("S1:Entry\n");
      return HANDLE_STATE();
    case HSM_SIG_EXIT:
//      CAST_DIALOG(self)->WriteDisplay(_T("S1:Exit"));
      printf("S1:Exit\n");
      return HANDLE_STATE();
    case HSM_SIG_INITIAL:
      return HANDLE_STATE();
    case TEST_SIG_I:
      //self->stateHandler = s2;
      //return STATE_CHANGED;
	  return CHANGE_STATE(self, &TestHSM::s2);
  }
  //self->stateHandler = s0;
  //return STATE_DO_SUPERSTATE;
  return HANDLE_SUPER_STATE(self, &TestHSM::s0);
}

hsm_state_result TestHSM::s11(struct hsm_state* self, struct hsm_event const * hsmEvent)
{
  switch(hsmEvent->signal)
  {
    case HSM_SIG_ENTRY:
      printf("S11:Entry\n");
      return HANDLE_STATE();
    case HSM_SIG_EXIT:
      printf("S11:Exit\n");
      return HANDLE_STATE();
    case HSM_SIG_INITIAL:
      return HANDLE_STATE();
    case TEST_SIG_J:
      //self->stateHandler = s0;
      //return STATE_CHANGED;
	  return CHANGE_STATE(self, &TestHSM::s0);
    case TEST_SIG_H:
      //self->stateHandler = s211;
      //return STATE_CHANGED;
	  return CHANGE_STATE(self, &TestHSM::s211);
  }
  //self->stateHandler = s1;
  //return STATE_DO_SUPERSTATE;
  return HANDLE_SUPER_STATE(self, &TestHSM::s1);
}

hsm_state_result TestHSM::s12(struct hsm_state* self, struct hsm_event const * hsmEvent)
{
  switch(hsmEvent->signal)
  {
    case HSM_SIG_ENTRY:
      printf("S12:Entry\n");
      return HANDLE_STATE();
    case HSM_SIG_EXIT:
      printf("S12:Exit\n");
      return HANDLE_STATE();
    case HSM_SIG_INITIAL:
      return HANDLE_STATE();
    case TEST_SIG_K:
      //self->stateHandler = s13;
      //return STATE_CHANGED;
	  return CHANGE_STATE(self, &TestHSM::s13);
  }
  //self->stateHandler = s1;
  //return STATE_DO_SUPERSTATE;
  return HANDLE_SUPER_STATE(self, &TestHSM::s1);
}

hsm_state_result TestHSM::s13(struct hsm_state* self, struct hsm_event const * hsmEvent)
{
  switch(hsmEvent->signal)
  {
    case HSM_SIG_ENTRY:
      printf("S13:Entry\n");
      return HANDLE_STATE();
    case HSM_SIG_EXIT:
      printf("S13:Exit\n");
      return HANDLE_STATE();
    case HSM_SIG_INITIAL:
      return HANDLE_STATE();
    case TEST_SIG_K:
      //self->stateHandler = s12;
      //return STATE_CHANGED;
	  return CHANGE_STATE(self, &TestHSM::s12);
  }
  self->stateHandler = s1;
  return STATE_DO_SUPERSTATE;
}


hsm_state_result TestHSM::s2(struct hsm_state* self, struct hsm_event const * hsmEvent)
{
  switch(hsmEvent->signal)
  {
    case HSM_SIG_ENTRY:
		printf("S2:Entry\n");
//      return STATE_HANDLED;
		return HANDLE_STATE();
	case HSM_SIG_EXIT:
		printf("S2:Exit\n");
//      return STATE_HANDLED;
		return HANDLE_STATE();
    case HSM_SIG_INITIAL:
//      return STATE_HANDLED;
		return HANDLE_STATE();
    case TEST_SIG_G:
      //self->stateHandler = s11;
      //return STATE_CHANGED;
      //return CHANGE_STATE(self, &TestHSM::s11);
    case TEST_SIG_I:
      //self->stateHandler = s1;
      //return STATE_CHANGED;
	  return CHANGE_STATE(self, &TestHSM::s1);
  }
  //self->stateHandler = s0;
  //return STATE_DO_SUPERSTATE;
  return HANDLE_SUPER_STATE(self, &TestHSM::s0);
}


// Initial State
hsm_state_result TestHSM::s21(struct hsm_state* self, struct hsm_event const * hsmEvent)
{

  switch(hsmEvent->signal)
  {
    case HSM_SIG_ENTRY:
		printf("S21:Entry\n");
//      return STATE_HANDLED;
      return HANDLE_STATE();
    case HSM_SIG_EXIT:
		printf("S21:Exit\n");
//      return STATE_HANDLED;
      return HANDLE_STATE();
    case HSM_SIG_INITIAL:
//      return STATE_HANDLED;
      return HANDLE_STATE();
    case TEST_SIG_A:
//      self->stateHandler = s211;
//      return STATE_CHANGED;
		return CHANGE_STATE(self, &TestHSM::s211);
    case TEST_SIG_F:
//      self->stateHandler = s22;
//      return STATE_CHANGED;
		return CHANGE_STATE(self, &TestHSM::s22);
  }
//  self->stateHandler = s2;
//  return STATE_DO_SUPERSTATE;
  return HANDLE_SUPER_STATE(self, &TestHSM::s2);
}

hsm_state_result TestHSM::s211(struct hsm_state* self, struct hsm_event const * hsmEvent)
{
	switch(hsmEvent->signal)
	{
		case HSM_SIG_ENTRY:
			printf("S211:Entry\n");
			return HANDLE_STATE();
		case HSM_SIG_EXIT:
			printf("S211:Exit\n");
			return HANDLE_STATE();
		case HSM_SIG_INITIAL:
			return HANDLE_STATE();
		case TEST_SIG_A:
			return HANDLE_STATE();
		case TEST_SIG_B:
//      self->stateHandler = s21;
//      return STATE_CHANGED;
			return CHANGE_STATE(self, &TestHSM::s21);
		case TEST_SIG_C:
			//self->stateHandler = s2;
			//return STATE_CHANGED;
			return CHANGE_STATE(self, &TestHSM::s2);
	}
	//self->stateHandler = s21;
	//return STATE_DO_SUPERSTATE;
	return HANDLE_SUPER_STATE(self, &TestHSM::s21);

}


hsm_state_result TestHSM::s22(struct hsm_state* self, struct hsm_event const * hsmEvent)
{
  switch(hsmEvent->signal)
  {
    case HSM_SIG_ENTRY:
      printf("S22:Entry\n");
      return HANDLE_STATE();
    case HSM_SIG_EXIT:
      printf("S22:Exit\n");
      return HANDLE_STATE();
    case HSM_SIG_INITIAL:
      return HANDLE_STATE();
    case TEST_SIG_L:
      //self->stateHandler = s13;
      //return STATE_CHANGED;
      return CHANGE_STATE(self, &TestHSM::s13);
  }
  //self->stateHandler = s2;
  //return STATE_DO_SUPERSTATE;
  return HANDLE_SUPER_STATE(self, &TestHSM::s2);
}

hsm_state_result TestHSM::state0(struct hsm_state* self, struct hsm_event const * hsmEvent)
{
  switch(hsmEvent->signal)
  {
    case HSM_SIG_ENTRY:
      printf("state0:Entry\n");
	  return HANDLE_STATE();
    case HSM_SIG_EXIT:
      printf("state0:Exit\n");
      return HANDLE_STATE();
    case HSM_SIG_INITIAL:
      return HANDLE_STATE();
    case TEST_SIG_TRANSITION:
      //self->stateHandler = state1;
      //return STATE_CHANGED;
      return CHANGE_STATE(self, &TestHSM::state1);
  }
 // self->stateHandler = RootState;
  //return STATE_DO_SUPERSTATE;
  return HANDLE_SUPER_STATE(self, &TestHSM::RootState);
}

hsm_state_result TestHSM::state1(struct hsm_state* self, struct hsm_event const * hsmEvent)
{
  switch(hsmEvent->signal)
  {
    case HSM_SIG_ENTRY:
      //CAST_DIALOG(self)->WriteDisplay(_T("state1:Entry"));
		//return STATE_HANDLED;
      printf("state1:Entry\n");
	  return HANDLE_STATE();
    case HSM_SIG_EXIT:
      //CAST_DIALOG(self)->WriteDisplay(_T("state1:Exit"));
      //return STATE_HANDLED;
		printf("state1:Exit\n");
		return HANDLE_STATE();
    case HSM_SIG_INITIAL:
      //return STATE_HANDLED;
      return HANDLE_STATE();
    case TEST_SIG_TRANSITION:
      //self->stateHandler = state2;
      //return STATE_CHANGED;
      HANDLE_SUPER_STATE(self, &TestHSM::state2);
  }
  //self->stateHandler = state0;
  //return STATE_DO_SUPERSTATE;
  return HANDLE_SUPER_STATE(self, &TestHSM::state0);
}

hsm_state_result TestHSM::state2(struct hsm_state* self, struct hsm_event const * hsmEvent)
{
  switch(hsmEvent->signal)
  {
    case HSM_SIG_ENTRY:
      //CAST_DIALOG(self)->WriteDisplay(_T("state2:Entry"));
      //return STATE_HANDLED;
		printf("state2:Entry\n");
		return HANDLE_STATE();
    case HSM_SIG_EXIT:
      //CAST_DIALOG(self)->WriteDisplay(_T("state2:Exit"));
      //return STATE_HANDLED;
		printf("state2:Exit\n");
		return HANDLE_STATE();
    case HSM_SIG_INITIAL:
      return HANDLE_STATE();
    case TEST_SIG_TRANSITION:
      //self->stateHandler = state3;
      //return STATE_CHANGED;
      return CHANGE_STATE(self, &TestHSM::state3);
  }
  //self->stateHandler = state0;
  //return STATE_DO_SUPERSTATE;
  return HANDLE_SUPER_STATE(self, &TestHSM::state0);
}


hsm_state_result TestHSM::state3(struct hsm_state* self, struct hsm_event const * hsmEvent)
{
  switch(hsmEvent->signal)
  {
    case HSM_SIG_ENTRY:
      printf("state3:Entry\n");
      return HANDLE_STATE();
    case HSM_SIG_EXIT:
      printf("state3:Exit\n");
      return HANDLE_STATE();
    case HSM_SIG_INITIAL:
      return HANDLE_STATE();
    case TEST_SIG_TRANSITION:
      //self->stateHandler = state1;
      //return STATE_CHANGED;
      return CHANGE_STATE(self, &TestHSM::state1);
  }
  //self->stateHandler = state0;
  //return STATE_DO_SUPERSTATE;
  return HANDLE_SUPER_STATE(self, &TestHSM::state0);
}


//
// Test Cases
//
TEST_CASE("One")
{
	TestHSM testHSM;
	testHSM.InitStateMachine(&testHSM.state, &testHSM.s0);

	SECTION("Initial State")
	{
		//testHSM
		REQUIRE(true);
		REQUIRE(testHSM.state.stateHandler == &testHSM.s0);
		hsmProcess(&testHSM.state);

	}

}
