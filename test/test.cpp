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
    int foo;
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
	hsmEventPoolInit(hsmTestEventPool, ARRAY_LENGTH(hsmTestEventPool));
	hsmEventQueueInit(hsmTestEventQueue, ARRAY_LENGTH(hsmTestEventQueue));
	hsmInitStateMachine();
	hsmInitialState(state, initialState);
}

hsm_state_result TestHSM::RootState(struct hsm_state* self, struct hsm_event const * e)
{
	return HANDLE_STATE();
}

extern const char* signalNames[];
hsm_state_result TestHSM::s0(struct hsm_state* self, struct hsm_event const * e)
{
  HSM_DEBUG_PRINT_EVENT(e);
	switch(e->signal)
	{
		case HSM_SIG_ENTRY:
//			printf("(S0:Entry) ");
			return HANDLE_STATE();
		case HSM_SIG_EXIT:
//			printf("(S0:Exit) ");
			return HANDLE_STATE();
		case HSM_SIG_INITIAL:
      return CHANGE_STATE(self, &TestHSM::s11);
		case TEST_SIG_D:
			return CHANGE_STATE(self, &TestHSM::s211);
		case TEST_SIG_E:
			return CHANGE_STATE(self, &TestHSM::s21);
		case TEST_SIG_G:
			return STATE_CHANGED;
		case TEST_SIG_I:
			return CHANGE_STATE(self, &TestHSM::s1);
		//case TEST_SIG_TRANSITION:
		//	return CHANGE_STATE(self, &TestHSM::state0);
	}
	return HANDLE_SUPER_STATE(self, &TestHSM::RootState);
}

hsm_state_result TestHSM::s1(struct hsm_state* self, struct hsm_event const * hsmEvent)
{
  HSM_DEBUG_PRINT_EVENT(hsmEvent);
  switch(hsmEvent->signal)
  {
    case HSM_SIG_ENTRY:
//      printf("(S1:Entry) ");
      return HANDLE_STATE();
    case HSM_SIG_EXIT:
//      printf("(S1:Exit) ");
      return HANDLE_STATE();
    case HSM_SIG_INITIAL:
      return HANDLE_STATE();
    case TEST_SIG_I:
      return CHANGE_STATE(self, &TestHSM::s2);
  }
  return HANDLE_SUPER_STATE(self, &TestHSM::s0);
}

hsm_state_result TestHSM::s11(struct hsm_state* self, struct hsm_event const * hsmEvent)
{
  HSM_DEBUG_PRINT_EVENT(hsmEvent);
  switch(hsmEvent->signal)
  {
    case HSM_SIG_ENTRY:
//      printf("(S11:Entry) ");
      return HANDLE_STATE();
    case HSM_SIG_EXIT:
//      printf("(S11:Exit) ");
      return HANDLE_STATE();
    case HSM_SIG_INITIAL:
      return HANDLE_STATE();
    case TEST_SIG_J:
  	  return CHANGE_STATE(self, &TestHSM::s0);
    case TEST_SIG_H:
  	  return CHANGE_STATE(self, &TestHSM::s211);
  }
  return HANDLE_SUPER_STATE(self, &TestHSM::s1);
}

hsm_state_result TestHSM::s12(struct hsm_state* self, struct hsm_event const * hsmEvent)
{
  HSM_DEBUG_PRINT_EVENT(hsmEvent);
  switch(hsmEvent->signal)
  {
    case HSM_SIG_ENTRY:
//      printf("(S12:Entry) ");
      return HANDLE_STATE();
    case HSM_SIG_EXIT:
//      printf("(S12:Exit) ");
      return HANDLE_STATE();
    case HSM_SIG_INITIAL:
      return HANDLE_STATE();
    case TEST_SIG_K:
	  return CHANGE_STATE(self, &TestHSM::s13);
  }
  return HANDLE_SUPER_STATE(self, &TestHSM::s1);
}

hsm_state_result TestHSM::s13(struct hsm_state* self, struct hsm_event const * hsmEvent)
{
  HSM_DEBUG_PRINT_EVENT(hsmEvent);
  switch(hsmEvent->signal)
  {
    case HSM_SIG_ENTRY:
      printf("(S13:Entry) ");
      return HANDLE_STATE();
    case HSM_SIG_EXIT:
      printf("(S13:Exit) ");
      return HANDLE_STATE();
    case HSM_SIG_INITIAL:
      return HANDLE_STATE();
    case TEST_SIG_K:
  	  return CHANGE_STATE(self, &TestHSM::s12);
  }
  self->stateHandler = s1;
  return STATE_DO_SUPERSTATE;
}


hsm_state_result TestHSM::s2(struct hsm_state* self, struct hsm_event const * hsmEvent)
{
  HSM_DEBUG_PRINT_EVENT(hsmEvent);
  switch(hsmEvent->signal)
  {
    case HSM_SIG_ENTRY:
      printf("(S2:Entry) ");
      return HANDLE_STATE();
    case HSM_SIG_EXIT:
      printf("(S2:Exit) ");
      return HANDLE_STATE();
    case HSM_SIG_INITIAL:
      return HANDLE_STATE();
    case TEST_SIG_G:
      return CHANGE_STATE(self, &TestHSM::s11);
    case TEST_SIG_I:
      return CHANGE_STATE(self, &TestHSM::s1);
  }
  return HANDLE_SUPER_STATE(self, &TestHSM::s0);
}


// Initial State
hsm_state_result TestHSM::s21(struct hsm_state* self, struct hsm_event const * hsmEvent)
{
  HSM_DEBUG_PRINT_EVENT(hsmEvent);
  switch(hsmEvent->signal)
  {
    case HSM_SIG_ENTRY:
		printf("(S21:Entry) ");
      return HANDLE_STATE();
    case HSM_SIG_EXIT:
		printf("(S21:Exit) ");
      return HANDLE_STATE();
    case HSM_SIG_INITIAL:
      return HANDLE_STATE();
    case TEST_SIG_A:
      return CHANGE_STATE(self, &TestHSM::s211);
    case TEST_SIG_F:
      return CHANGE_STATE(self, &TestHSM::s22);
  }
  return HANDLE_SUPER_STATE(self, &TestHSM::s2);
}

hsm_state_result TestHSM::s211(struct hsm_state* self, struct hsm_event const * hsmEvent)
{
  HSM_DEBUG_PRINT_EVENT(hsmEvent);
	switch(hsmEvent->signal)
	{
		case HSM_SIG_ENTRY:
			printf("(S211:Entry) ");
			return HANDLE_STATE();
		case HSM_SIG_EXIT:
			printf("(S211:Exit) ");
			return HANDLE_STATE();
		case HSM_SIG_INITIAL:
			return HANDLE_STATE();
		case TEST_SIG_A:
			return HANDLE_STATE();
		case TEST_SIG_B:
			return CHANGE_STATE(self, &TestHSM::s21);
		case TEST_SIG_C:
			return CHANGE_STATE(self, &TestHSM::s2);
	}
	return HANDLE_SUPER_STATE(self, &TestHSM::s21);
}


hsm_state_result TestHSM::s22(struct hsm_state* self, struct hsm_event const * hsmEvent)
{
  HSM_DEBUG_PRINT_EVENT(hsmEvent);
  switch(hsmEvent->signal)
  {
    case HSM_SIG_ENTRY:
      printf("(S22:Entry) ");
      return HANDLE_STATE();
    case HSM_SIG_EXIT:
      printf("(S22:Exit) ");
      return HANDLE_STATE();
    case HSM_SIG_INITIAL:
      return HANDLE_STATE();
    case TEST_SIG_L:
      return CHANGE_STATE(self, &TestHSM::s13);
  }
  return HANDLE_SUPER_STATE(self, &TestHSM::s2);
}

hsm_state_result TestHSM::state0(struct hsm_state* self, struct hsm_event const * hsmEvent)
{
  HSM_DEBUG_PRINT_EVENT(hsmEvent);
  switch(hsmEvent->signal)
  {
    case HSM_SIG_ENTRY:
      printf("(state0:Entry) ");
	  return HANDLE_STATE();
    case HSM_SIG_EXIT:
      printf("(state0:Exit) ");
      return HANDLE_STATE();
    case HSM_SIG_INITIAL:
      return HANDLE_STATE();
    case TEST_SIG_TRANSITION:
      //self->stateHandler = state1;
      //return STATE_CHANGED;
      return CHANGE_STATE(self, &TestHSM::state1);
  }
  return HANDLE_SUPER_STATE(self, &TestHSM::RootState);
}

hsm_state_result TestHSM::state1(struct hsm_state* self, struct hsm_event const * hsmEvent)
{
  HSM_DEBUG_PRINT_EVENT(hsmEvent);
  switch(hsmEvent->signal)
  {
    case HSM_SIG_ENTRY:
      printf("(state1:Entry) ");
      return HANDLE_STATE();
    case HSM_SIG_EXIT:
      printf("(state1:Exit) ");
      return HANDLE_STATE();
    case HSM_SIG_INITIAL:
      return HANDLE_STATE();
    case TEST_SIG_TRANSITION:
      HANDLE_SUPER_STATE(self, &TestHSM::state2);
  }
  return HANDLE_SUPER_STATE(self, &TestHSM::state0);
}

hsm_state_result TestHSM::state2(struct hsm_state* self, struct hsm_event const * hsmEvent)
{
  HSM_DEBUG_PRINT_EVENT(hsmEvent);
  switch(hsmEvent->signal)
  {
    case HSM_SIG_ENTRY:
      printf("(state2:Entry) ");
      return HANDLE_STATE();
    case HSM_SIG_EXIT:
      printf("(state2:Exit) ");
      return HANDLE_STATE();
    case HSM_SIG_INITIAL:
      return HANDLE_STATE();
    case TEST_SIG_TRANSITION:
      return CHANGE_STATE(self, &TestHSM::state3);
  }
  return HANDLE_SUPER_STATE(self, &TestHSM::state0);
}


hsm_state_result TestHSM::state3(struct hsm_state* self, struct hsm_event const * hsmEvent)
{
  HSM_DEBUG_PRINT_EVENT(hsmEvent);
  switch(hsmEvent->signal)
  {
    case HSM_SIG_ENTRY:
      printf("(state3:Entry) ");
      return HANDLE_STATE();
    case HSM_SIG_EXIT:
      printf("(state3:Exit) ");
      return HANDLE_STATE();
    case HSM_SIG_INITIAL:
      return HANDLE_STATE();
    case TEST_SIG_TRANSITION:
        return CHANGE_STATE(self, &TestHSM::state1);
  }
  return HANDLE_SUPER_STATE(self, &TestHSM::state0);
}


//
// Test Cases
//
TEST_CASE("Framework")
{

  TestHSM testHSM;

  SECTION("Event New")
  {
    struct hsm_test_event * e;

    testHSM.InitStateMachine(&testHSM.state, &testHSM.s0);

    for (int i = 0; i < ARRAY_LENGTH(hsmTestEventPool); i++)
    {
      e = (hsm_test_event*)hsmEventNew();
      REQUIRE(e != 0);
    }

    e = (hsm_test_event*)hsmEventNew();
    REQUIRE(e == 0);
  }

  printf("\n");

}

#if 0
TEST_CASE("Transitions")
{
  TestHSM testHSM;

  SECTION("Initial State")
  {
    testHSM.InitStateMachine(&testHSM.state, &testHSM.s0);
    REQUIRE(testHSM.state.stateHandler == &testHSM.s11);
  }
}

TEST_CASE("Two")
{
	TestHSM testHSM;

	SECTION("Initial State")
	{
		//testHSM
		REQUIRE(true);
    testHSM.InitStateMachine(&testHSM.state, &testHSM.s0);
		REQUIRE(testHSM.state.stateHandler == &testHSM.s0);
		hsmProcess(&testHSM.state);
    REQUIRE(testHSM.state.stateHandler == &testHSM.s0);
    hsmProcess(&testHSM.state);
	}

  SECTION("Initial State, complex")
	{
    testHSM.InitStateMachine(&testHSM.state, &testHSM.s21);
		REQUIRE(testHSM.state.stateHandler == &testHSM.s21);
		hsmProcess(&testHSM.state);
    REQUIRE(testHSM.state.stateHandler == &testHSM.s21);
	}

  SECTION("Transitions")
	{
    //printf("\n");
    testHSM.InitStateMachine(&testHSM.state, &testHSM.s21);
		REQUIRE(testHSM.state.stateHandler == &testHSM.s21);

    TestHSM::hsm_test_event * e = (TestHSM::hsm_test_event*)hsmEventNew();
    REQUIRE(e != 0);

//    e->signal = (hsm_signal)TestHSM::hsm_test_signal::TEST_SIG_A;
//    fifoPush(e);
//		hsmProcess(&testHSM.state);
    //REQUIRE(testHSM.state.stateHandler == &testHSM.s21);
	}
}
#endif
