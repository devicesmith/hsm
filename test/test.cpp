#include "../inc/catch.hpp"
#include "../inc/inc.hpp"
#include "../inc/HSMFrameWork.h"


class TestHSM
{
  public:
    state_handler *transition_history;
    struct hsm_event *event_history;
    int *ti;

    // Extend the base state variable
    struct hsm_test_state : hsm_state
    {
      int foo;
    } state;

    // Extend the base event
    struct hsm_test_event : hsm_event
    {
      int extraEventInfo;
    };

    // Extend the signal
    enum hsm_test_signal
    {
      TEST_SIG_A = HSM_SIG_USER,
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

// For debugging
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

//
// Implementation
//

// Main State Variables
struct TestHSM::hsm_test_state hsmTestStateMachine;
// Allocate Event Pool
struct TestHSM::hsm_test_event hsmTestEventPool[10];
// Allocate Event Qeueu
struct hsm_event* hsmTestEventQueue[10];

struct fifo_data fifoData;


//void TestHSM::InitStateMachine(hsm_test_state * initialState)
void TestHSM::InitStateMachine(struct hsm_state* state, state_handler initialState)
{
  hsmEventPoolInit(hsmTestEventPool, ARRAY_LENGTH(hsmTestEventPool));
  hsmEventQueueInit(&fifoData, hsmTestEventQueue, ARRAY_LENGTH(hsmTestEventQueue));
  hsmInitStateMachine();

  ((hsm_test_state*)state)->foo = 0;
  hsmInitialState(state, initialState);
  //HSM_DEBUG_NEWLINE();
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
      return HANDLE_STATE();
    case HSM_SIG_EXIT:
      return HANDLE_STATE();
    case HSM_SIG_INITIAL:
      return CHANGE_STATE(self, &TestHSM::s11);
    case TEST_SIG_E:
      return CHANGE_STATE(self, &TestHSM::s11);
    case TEST_SIG_I:
      if (((TestHSM::hsm_test_state*)self)->foo == 1) {
        HSM_DEBUG_PRINTLN("(foo == 1), foo = 0");
        ((TestHSM::hsm_test_state*)self)->foo = 0;
      } else {
        HSM_DEBUG_PRINTLN("(foo == 0), do nothing");
      }
      return HANDLE_STATE();
  }
  return HANDLE_SUPER_STATE(self, &TestHSM::RootState);
}

hsm_state_result TestHSM::s1(struct hsm_state* self, struct hsm_event const * hsmEvent)
{
  HSM_DEBUG_PRINT_EVENT(hsmEvent);
  switch(hsmEvent->signal)
  {
    case HSM_SIG_ENTRY:
      return HANDLE_STATE();
    case HSM_SIG_EXIT:
      return HANDLE_STATE();
    case HSM_SIG_INITIAL:
      return CHANGE_STATE(self, &TestHSM::s11);
    case TEST_SIG_A:
      return CHANGE_STATE(self, &TestHSM::s1);
    case TEST_SIG_B:
      return CHANGE_STATE(self, &TestHSM::s11);
    case TEST_SIG_C:
      return CHANGE_STATE(self, &TestHSM::s2);
    case TEST_SIG_D:
      if (((TestHSM::hsm_test_state*)self)->foo == 0) {
        HSM_DEBUG_PRINTLN("(foo == 0), foo = 1");
        ((TestHSM::hsm_test_state*)self)->foo = 1;
        return CHANGE_STATE(self, &TestHSM::s0);
      } else {
        HSM_DEBUG_PRINTLN("(foo == 1), do nothing");
      }
      return HANDLE_STATE();
    case TEST_SIG_F:
      return CHANGE_STATE(self, &TestHSM::s211);
    case TEST_SIG_I:
      return HANDLE_STATE();
  }
  return HANDLE_SUPER_STATE(self, &TestHSM::s0);
}

hsm_state_result TestHSM::s11(struct hsm_state* self, struct hsm_event const * hsmEvent)
{
  HSM_DEBUG_PRINT_EVENT(hsmEvent);
  switch(hsmEvent->signal)
  {
    case HSM_SIG_ENTRY:
      return HANDLE_STATE();
    case HSM_SIG_EXIT:
      return HANDLE_STATE();
    case HSM_SIG_INITIAL:
      return HANDLE_STATE();
    case TEST_SIG_D:
      if (((hsm_test_state*)self)->foo == 1)
      {
        HSM_DEBUG_PRINTLN("(foo == 1) - change to s1");
        ((TestHSM::hsm_test_state*)self)->foo = 0;
        return CHANGE_STATE(self, &TestHSM::s1);
      }
      else {
        HSM_DEBUG_PRINTLN("(foo == 0) - do nothing");
      }
      break; // pass to superstate
    case TEST_SIG_G:
      return CHANGE_STATE(self, &TestHSM::s211);
    case TEST_SIG_H:
      return CHANGE_STATE(self, &TestHSM::s0);
  }
  return HANDLE_SUPER_STATE(self, &TestHSM::s1);
}

#if 0
hsm_state_result TestHSM::s12(struct hsm_state* self, struct hsm_event const * hsmEvent)
{
  HSM_DEBUG_PRINT_EVENT(hsmEvent);
  switch(hsmEvent->signal)
  {
    case HSM_SIG_ENTRY:
      return HANDLE_STATE();
    case HSM_SIG_EXIT:
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
      return HANDLE_STATE();
    case HSM_SIG_EXIT:
      return HANDLE_STATE();
    case HSM_SIG_INITIAL:
      return HANDLE_STATE();
    case TEST_SIG_K:
      return CHANGE_STATE(self, &TestHSM::s12);
  }
  self->stateHandler = s1;
  return STATE_DO_SUPERSTATE;
}
#endif

hsm_state_result TestHSM::s2(struct hsm_state* self, struct hsm_event const * hsmEvent)
{
  HSM_DEBUG_PRINT_EVENT(hsmEvent);
  switch(hsmEvent->signal)
  {
    case HSM_SIG_ENTRY:
      return HANDLE_STATE();
    case HSM_SIG_EXIT:
      return HANDLE_STATE();
    case HSM_SIG_INITIAL:
      return CHANGE_STATE(self, &TestHSM::s211);
    case TEST_SIG_C:
      return CHANGE_STATE(self, &TestHSM::s1);
    case TEST_SIG_F:
      return CHANGE_STATE(self, &TestHSM::s11);
    case TEST_SIG_I:
      if (((TestHSM::hsm_test_state*)self)->foo == 0) {
        HSM_DEBUG_PRINTLN("(foo == 0), foo = 1");
        ((TestHSM::hsm_test_state*)self)->foo = 1;
        return HANDLE_STATE();
      } else {
        HSM_DEBUG_PRINTLN("(foo == 1), do nothing ");
      }
      HANDLE_SUPER_STATE(self, &TestHSM::s0);
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
      return HANDLE_STATE();
    case HSM_SIG_EXIT:
      return HANDLE_STATE();
    case HSM_SIG_INITIAL:
      return CHANGE_STATE(self, &TestHSM::s211);
    case TEST_SIG_A:
      return CHANGE_STATE(self, &TestHSM::s21);
    case TEST_SIG_B:
      return CHANGE_STATE(self, &TestHSM::s211);
    case TEST_SIG_G:
      return CHANGE_STATE(self, &TestHSM::s1);
  }
  return HANDLE_SUPER_STATE(self, &TestHSM::s2);
}

hsm_state_result TestHSM::s211(struct hsm_state* self, struct hsm_event const * hsmEvent)
{
  HSM_DEBUG_PRINT_EVENT(hsmEvent);
  switch(hsmEvent->signal)
  {
    case HSM_SIG_ENTRY:
      return HANDLE_STATE();
    case HSM_SIG_EXIT:
      return HANDLE_STATE();
    case HSM_SIG_INITIAL:
      return HANDLE_STATE();
    case TEST_SIG_D:
      return CHANGE_STATE(self, &TestHSM::s21);
    case TEST_SIG_H:
      return CHANGE_STATE(self, &TestHSM::s0);
  }
  return HANDLE_SUPER_STATE(self, &TestHSM::s21);
}

#if 0
hsm_state_result TestHSM::s22(struct hsm_state* self, struct hsm_event const * hsmEvent)
{
  HSM_DEBUG_PRINT_EVENT(hsmEvent);
  switch(hsmEvent->signal)
  {
    case HSM_SIG_ENTRY:
      return HANDLE_STATE();
    case HSM_SIG_EXIT:
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
#endif

//
// Test Cases
//
#if 0
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
    HSM_DEBUG_NEWLINE();
  }
}

TEST_CASE("Transitions")
{
  TestHSM testHSM;

  SECTION("Initial State")
  {
    testHSM.InitStateMachine(&testHSM.state, &testHSM.s0);
    REQUIRE(testHSM.state.stateHandler == &testHSM.s11);
    HSM_DEBUG_NEWLINE();
  }
}

TEST_CASE("Two")
{
  TestHSM testHSM;

  SECTION("Initial State")
  {
    HSM_DEBUG_PRINTLN("==> Init to s0");
    testHSM.InitStateMachine(&testHSM.state, &testHSM.s0);
    REQUIRE(testHSM.state.stateHandler == &testHSM.s11);
    hsmProcess(&testHSM.state);
    REQUIRE(testHSM.state.stateHandler == &testHSM.s11);
    HSM_DEBUG_NEWLINE();
  }

  SECTION("Ignored event")
  {
    HSM_DEBUG_PRINTLN("==> Init to s0, Signal J (ignored)");
    testHSM.InitStateMachine(&testHSM.state, &testHSM.s0);
    TestHSM::hsm_test_event* e = (TestHSM::hsm_test_event*)hsmEventNew();
    e->signal = (hsm_signal)TestHSM::hsm_test_signal::TEST_SIG_J;
    fifoPush(e);
    hsmProcess(&testHSM.state);
    REQUIRE(testHSM.state.stateHandler == &testHSM.s11);
    HSM_DEBUG_NEWLINE();
  }
}
#endif


TEST_CASE("Full Transition Test")
{
  TestHSM testHSM;

  //testHSM.ti = &_ti;
  //testHSM.transition_history = transition_history;
  //testHSM.event_history      = event_history;

  SECTION("From s2")
  {
    HSM_DEBUG_PRINTLN("==> Init to s2");
    testHSM.InitStateMachine(&testHSM.state, &testHSM.s2);

    REQUIRE(testHSM.state.stateHandler == &testHSM.s211);
#ifdef HSM_DEBUG_LOGGING // FIXME: Switch this comparison to a wrapped macro
    REQUIRE(transition_history[0] == testHSM.RootState);
    REQUIRE(transition_history[1] == testHSM.s0);
    REQUIRE(transition_history[2] == testHSM.s2);
    REQUIRE(transition_history[3] == testHSM.s2); // 2nd for init event
    REQUIRE(transition_history[4] == testHSM.s21);
    REQUIRE(transition_history[5] == testHSM.s211);
    HSM_DEBUG_NEWLINE();
    HSM_DEBUG_LOG_ZERO(transition_history);
#endif

    HSM_DEBUG_PRINTLN("==> s211 Signal G");
    TestHSM::hsm_test_event* e = (TestHSM::hsm_test_event*)hsmEventNew();
    e->signal = (hsm_signal)TestHSM::hsm_test_signal::TEST_SIG_G;
    queuePush(&fifoData, e);
    hsmProcess(&testHSM.state, &fifoData);

    REQUIRE(testHSM.state.stateHandler == &testHSM.s11);
#ifdef HSM_DEBUG_LOGGING // FIXME: Switch this comparison to a wrapped macro
    REQUIRE(transition_history[0] == testHSM.s211);
    REQUIRE(transition_history[1] == testHSM.s21);
    REQUIRE(transition_history[2] == testHSM.s2);
    REQUIRE(transition_history[3] == testHSM.s1);
    REQUIRE(transition_history[4] == testHSM.s1);
    REQUIRE(transition_history[5] == testHSM.s11);
    HSM_DEBUG_NEWLINE();
    HSM_DEBUG_LOG_ZERO(transition_history);
#endif

    HSM_DEBUG_PRINTLN("==> s11 Signal I");
    e = (TestHSM::hsm_test_event*)hsmEventNew();
    e->signal = (hsm_signal)TestHSM::hsm_test_signal::TEST_SIG_I;
    queuePush(&fifoData, e);
    hsmProcess(&testHSM.state, &fifoData);
    REQUIRE(testHSM.state.stateHandler == &testHSM.s11);
#ifdef HSM_DEBUG_LOGGING
    REQUIRE(transition_history[0] == 0);
    HSM_DEBUG_NEWLINE();
    HSM_DEBUG_LOG_ZERO(transition_history);
#endif

    HSM_DEBUG_PRINTLN("==> s11 Signal A");
    e = (TestHSM::hsm_test_event*)hsmEventNew();
    e->signal = (hsm_signal)TestHSM::hsm_test_signal::TEST_SIG_A;
    queuePush(&fifoData, e);
    hsmProcess(&testHSM.state, &fifoData);
#ifdef HSM_DEBUG_LOGGING
    REQUIRE(testHSM.state.stateHandler == &testHSM.s11);
    REQUIRE(transition_history[0] == testHSM.s11);
    REQUIRE(transition_history[1] == testHSM.s1);
    REQUIRE(transition_history[2] == testHSM.s1);
    REQUIRE(transition_history[3] == testHSM.s1);
    REQUIRE(transition_history[4] == testHSM.s11);
    HSM_DEBUG_NEWLINE();
    HSM_DEBUG_LOG_ZERO(transition_history);
#endif

    HSM_DEBUG_PRINTLN("==> s11 Signal D");
    e = (TestHSM::hsm_test_event*)hsmEventNew();
    e->signal = (hsm_signal)TestHSM::hsm_test_signal::TEST_SIG_D;
    queuePush(&fifoData, e);
    hsmProcess(&testHSM.state, &fifoData);
    REQUIRE(testHSM.state.stateHandler == &testHSM.s11);
#ifdef HSM_DEBUG_LOGGING
    REQUIRE(transition_history[0] == testHSM.s11);
    REQUIRE(transition_history[1] == testHSM.s1);
    REQUIRE(transition_history[2] == testHSM.s0);
    REQUIRE(transition_history[3] == testHSM.s1);
    REQUIRE(transition_history[4] == testHSM.s11);
    REQUIRE(transition_history[5] == testHSM.s11);
    HSM_DEBUG_NEWLINE();
    HSM_DEBUG_LOG_ZERO(transition_history);
#endif

    HSM_DEBUG_PRINTLN("==> s11 second Signal D");
    e = (TestHSM::hsm_test_event*)hsmEventNew();
    e->signal = (hsm_signal)TestHSM::hsm_test_signal::TEST_SIG_D;
    queuePush(&fifoData, e);
    hsmProcess(&testHSM.state, &fifoData);
#ifdef HSM_DEBUG_LOGGING
    REQUIRE(testHSM.state.stateHandler == &testHSM.s11);
    REQUIRE(transition_history[0] == testHSM.s11);
    REQUIRE(transition_history[1] == testHSM.s1);
    REQUIRE(transition_history[2] == testHSM.s11);
    REQUIRE(transition_history[3] == testHSM.s11);
    HSM_DEBUG_NEWLINE();
    HSM_DEBUG_LOG_ZERO(transition_history);
#endif

    HSM_DEBUG_PRINTLN("==> s11 Signal C");
    e = (TestHSM::hsm_test_event*)hsmEventNew();
    e->signal = (hsm_signal)TestHSM::hsm_test_signal::TEST_SIG_C;
    queuePush(&fifoData, e);
    hsmProcess(&testHSM.state, &fifoData);
    REQUIRE(testHSM.state.stateHandler == &testHSM.s211);
#ifdef HSM_DEBUG_LOGGING
    REQUIRE(transition_history[0] == testHSM.s11);
    REQUIRE(transition_history[1] == testHSM.s1);
    REQUIRE(transition_history[2] == testHSM.s2);
    REQUIRE(transition_history[3] == testHSM.s2);
    REQUIRE(transition_history[4] == testHSM.s21);
    REQUIRE(transition_history[5] == testHSM.s211);
    REQUIRE(transition_history[6] == testHSM.s211);
    HSM_DEBUG_NEWLINE();
    HSM_DEBUG_LOG_ZERO(transition_history);
#endif

    HSM_DEBUG_PRINTLN("==> s211 Signal E");
    e = (TestHSM::hsm_test_event*)hsmEventNew();
    e->signal = (hsm_signal)TestHSM::hsm_test_signal::TEST_SIG_E;
    queuePush(&fifoData, e);
    hsmProcess(&testHSM.state, &fifoData);
    REQUIRE(testHSM.state.stateHandler == &testHSM.s11);
#ifdef HSM_DEBUG_LOGGING
    REQUIRE(transition_history[0] == testHSM.s211);
    REQUIRE(transition_history[1] == testHSM.s21);
    REQUIRE(transition_history[2] == testHSM.s2);
    REQUIRE(transition_history[3] == testHSM.s1);
    REQUIRE(transition_history[4] == testHSM.s11);
    REQUIRE(transition_history[5] == testHSM.s11);
    HSM_DEBUG_NEWLINE();
    HSM_DEBUG_LOG_ZERO(transition_history);
#endif

    HSM_DEBUG_PRINTLN("==> s11 2nd Signal E");
    e = (TestHSM::hsm_test_event*)hsmEventNew();
    e->signal = (hsm_signal)TestHSM::hsm_test_signal::TEST_SIG_E;
    queuePush(&fifoData, e);
    hsmProcess(&testHSM.state, &fifoData);
    REQUIRE(testHSM.state.stateHandler == &testHSM.s11);
#ifdef HSM_DEBUG_LOGGING
    REQUIRE(transition_history[0] == testHSM.s11);
    REQUIRE(transition_history[1] == testHSM.s1);
    REQUIRE(transition_history[2] == testHSM.s1);
    REQUIRE(transition_history[3] == testHSM.s11);
    //HSM_DEBUG_PRINTLN("\nShould be: s11-Exit;s1-EXIT;s1-ENTRY;s11-ENTRY;");
    HSM_DEBUG_NEWLINE();
    HSM_DEBUG_LOG_ZERO(transition_history);
#endif

    HSM_DEBUG_PRINTLN("==> s11 Signal G");
    e = (TestHSM::hsm_test_event*)hsmEventNew();
    e->signal = (hsm_signal)TestHSM::hsm_test_signal::TEST_SIG_G;
    queuePush(&fifoData, e);
    hsmProcess(&testHSM.state, &fifoData);
    REQUIRE(testHSM.state.stateHandler == &testHSM.s211);
#ifdef HSM_DEBUG_LOGGING
    REQUIRE(transition_history[0] == testHSM.s11);
    REQUIRE(transition_history[1] == testHSM.s1);
    REQUIRE(transition_history[2] == testHSM.s2);
    REQUIRE(transition_history[3] == testHSM.s21);
    REQUIRE(transition_history[4] == testHSM.s211);
    HSM_DEBUG_NEWLINE();
    HSM_DEBUG_LOG_ZERO(transition_history);
#endif

    HSM_DEBUG_PRINTLN("==> s211 Signal I");
    e = (TestHSM::hsm_test_event*)hsmEventNew();
    e->signal = (hsm_signal)TestHSM::hsm_test_signal::TEST_SIG_I;
    queuePush(&fifoData, e);
    hsmProcess(&testHSM.state, &fifoData);
#ifdef HSM_DEBUG_LOGGING
    REQUIRE(testHSM.state.stateHandler == &testHSM.s211);
    //REQUIRE(transition_history[0] == testHSM.s11);
    //REQUIRE(transition_history[1] == testHSM.s1);
    HSM_DEBUG_NEWLINE();
    HSM_DEBUG_LOG_ZERO(transition_history);
#endif

    HSM_DEBUG_PRINTLN("==> s211 2nd Signal I");
    e = (TestHSM::hsm_test_event*)hsmEventNew();
    e->signal = (hsm_signal)TestHSM::hsm_test_signal::TEST_SIG_I;
    queuePush(&fifoData, e);
    hsmProcess(&testHSM.state, &fifoData);
    REQUIRE(testHSM.state.stateHandler == &testHSM.s211);
#ifdef HSM_DEBUG_LOGGING
    //REQUIRE(transition_history[0] == testHSM.s11);
    //REQUIRE(transition_history[1] == testHSM.s1);
    HSM_DEBUG_NEWLINE();
#endif
  }
}
