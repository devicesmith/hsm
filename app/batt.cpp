//
// Matt McFadden
// batt.cpp
// Experimental battery state machine
//
// Behavior:
// - single short press:
//   1. show battery charge level
// - Five sec press when off :
//   1. Increment 5 leds from 0 to all on, growing the LEDs every 1 second
//   2. if let go before 5 seconds go back to off
//   3. if 5 seconds, turn battery on. Show on with 3rd (center) LED always on.
// - Five sec press when on:
//   1. walk LEDs to all off, decrementing each second
//   2. if let go before, back to on
// - Double press
//   1. flash all of them?
// - Super long press (> 10 sec)
//   1. cylon mode
//   2. any press goes back to on
#include "../inc/inc.hpp"
#include <iostream>

#include <unistd.h>
#include <stdio.h>
#include <sys/select.h>
#include <termios.h>

#include <time.h>

#include "../inc/HSMFrameWork.h"

using namespace std;

int _kbhit() {
  static const int STDIN = 0;
  static bool initialized = false;

  if (! initialized) {
      // Use termios to turn off line buffering
      termios term;
      tcgetattr(STDIN, &term);
      term.c_lflag &= ~ICANON;
      term.c_lflag &= ~ECHO;
      tcsetattr(STDIN, TCSANOW, &term);
      //tcsetattr(STDIN, TCSAFLUSH, &term);
      setbuf(stdin, NULL);
      initialized = true;
  }

  timeval timeout;
  fd_set rdset;

  FD_ZERO(&rdset);
  FD_SET(STDIN, &rdset);
  timeout.tv_sec  = 0;
  timeout.tv_usec = 0;

  return select(STDIN + 1, &rdset, NULL, NULL, &timeout);
}



class BatteryHSM
{
  public:
    struct hsm_batt_state : hsm_state
    {
        int foo;
    } state;

    // Extend the base event
    struct hsm_batt_event : hsm_event
    {
        int extraEventInfo;
    };

    // Extend the signal
    enum hsm_batt_signal
    {
      SIG_BUTTON = HSM_SIG_USER,
      SIG_POWER,
      SIG_TICK,
	  SIG_LAST
    };

    void InitStateMachine(hsm_batt_state * state, state_handler initialState);
    static hsm_state_result RootState(struct hsm_batt_state *self, struct hsm_batt_event const *e);
    static hsm_state_result s0(struct hsm_batt_state* self, struct hsm_batt_event const * e);
    static hsm_state_result OnState(struct hsm_batt_state* self, struct hsm_batt_event const * e);
    static hsm_state_result OffState(struct hsm_batt_state* self, struct hsm_batt_event const * e);

};

#define stringify( name ) # name
const char* signalNames[] = {
stringify(UNKNOWN),
stringify(INIT),
stringify(SILENT),
stringify(ENTRY),
stringify(EXIT),
stringify(INITIAL),
stringify(BUTTON),
stringify(POWER),
stringify(TICK),
stringify(LAST)
};


// Allocate Event Pool
struct BatteryHSM::hsm_batt_event battEventPool[10];
// Allocate Event Queue
struct hsm_event* battEventQueue[10];
struct fifo_data fifoData;

void BatteryHSM::InitStateMachine(struct hsm_batt_state* state, state_handler initialState)
{
  hsmEventPoolInit(battEventPool, ARRAY_LENGTH(battEventPool));
  hsmEventQueueInit(&fifoData, battEventQueue, ARRAY_LENGTH(battEventQueue));
  hsmInitStateMachine();

  // set intial state vars here
  state->foo = 1;
  hsmInitialState(state, initialState);

}

hsm_state_result BatteryHSM::RootState(struct hsm_batt_state* self, struct hsm_batt_event const * e)
{
  return HANDLE_STATE();
}

hsm_state_result BatteryHSM::s0(struct hsm_batt_state* self, struct hsm_batt_event const * e)
{
  HSM_DEBUG_PRINT_EVENT(e);
  switch(e->signal)
  {
      case HSM_SIG_ENTRY:
          return HANDLE_STATE();
      case HSM_SIG_EXIT:
          return HANDLE_STATE();
      case HSM_SIG_INITIAL:
          return CHANGE_STATE(self, &BatteryHSM::OffState);
      case SIG_TICK:
          cout << "TICK\n";
          return HANDLE_STATE();
  }
  return HANDLE_SUPER_STATE(self, &BatteryHSM::RootState);
}

hsm_state_result BatteryHSM::OnState(struct hsm_batt_state* self, struct hsm_batt_event const * e)
{
    HSM_DEBUG_PRINT_EVENT(e);
    switch(e->signal)
    {
        case HSM_SIG_ENTRY:
            return HANDLE_STATE();
        case HSM_SIG_EXIT:
            return HANDLE_STATE();
        case HSM_SIG_INITIAL:
            return HANDLE_STATE();
        case SIG_TICK:
            return HANDLE_SUPER_STATE(self, &BatteryHSM::s0);
        case SIG_BUTTON:
            return HANDLE_STATE();
    }
    return HANDLE_SUPER_STATE(self, &BatteryHSM::s0);
}

hsm_state_result BatteryHSM::OffState(struct hsm_batt_state* self, struct hsm_batt_event const * e)
{
    HSM_DEBUG_PRINT_EVENT(e);
    switch(e->signal)
    {
        case HSM_SIG_ENTRY:
            return HANDLE_STATE();
        case HSM_SIG_EXIT:
            return HANDLE_STATE();
        case HSM_SIG_INITIAL:
            return HANDLE_STATE();
        case SIG_TICK:
            return HANDLE_SUPER_STATE(self, &BatteryHSM::s0);
        case SIG_BUTTON:
            return HANDLE_STATE();
    }
    return HANDLE_SUPER_STATE(self, &BatteryHSM::s0);
}

int get_button_input() {
	if (_kbhit()) {
		return getchar();
	}
	return 0;
}

int main(int argc, char ** argv)
{
    cout << "Batt main() running\n";

    BatteryHSM batt;

    batt.InitStateMachine(&batt.state, (state_handler)&batt.s0);

    while(true)
    {
        int inChar = get_button_input();
        if(inChar == 'b')
        {
            cout << "BUTTON\n";
            BatteryHSM::hsm_batt_event* e = (BatteryHSM::hsm_batt_event*)hsmEventNew();
            e->signal = static_cast<hsm_signal>(BatteryHSM::hsm_batt_signal::SIG_BUTTON);
            queuePush(&fifoData, e);
        }
        if(inChar == 'p')
        {
            cout << "POWER\n";
            BatteryHSM::hsm_batt_event* e = (BatteryHSM::hsm_batt_event*)hsmEventNew();
            e->signal = static_cast<hsm_signal>(BatteryHSM::hsm_batt_signal::SIG_POWER);
            queuePush(&fifoData, e);
        }

        if(inChar == 'q') break;

        if(inChar != 0) cout <<"got:" << inChar << "\n";

        //static uint32_t updateDisplayTime_ms = 0;
        static clock_t t = clock();
        if((float)(clock() - t)/CLOCKS_PER_SEC > 1)
        {
            t = clock();

            BatteryHSM::hsm_batt_event* e = (BatteryHSM::hsm_batt_event*)hsmEventNew();
            e->signal = static_cast<hsm_signal>(BatteryHSM::hsm_batt_signal::SIG_TICK);
            queuePush(&fifoData, e);
        }

        hsmProcess(&batt.state, &fifoData);

        usleep(100);
    }

    return 0;
}

#if 0

int GetButtonInput() {
	if (_kbhit()) {
		return getchar();
	}
	return 0;
}

#define TICK_COUNT_START 100000

// Events for all
extern proto_event_t tickEvent;
// Events for Proto HSM
extern proto_event_t powerConnectedEvent;
extern proto_event_t powerDisConnectedEvent;
extern proto_event_t chargeLevelEvent;
extern proto_event_t buttonLongEvent;
extern proto_event_t buttonShortEvent;

// Events for LED HSM
extern proto_event_t showButtonOnEvent;
extern proto_event_t showButtonOffEvent;
extern proto_event_t showChargeLevelEvent;
extern proto_event_t showPowerOnEvent;
extern proto_event_t showPowerOffEvent;
extern proto_event_t updateDisplay;

// Button Manager HSM
extern proto_event_t kbHitEvent;

int main(int argc, char** argv) {

	// Events for all
	tickEvent.signal 				= SIG_TICK_MS;
	// Events for Proto HSM
	powerConnectedEvent.signal 		= SIG_POWER_CONNECTED;
	powerDisConnectedEvent.signal 	= SIG_POWER_DISCONNECTED;
	chargeLevelEvent.signal       	= SIG_CHARGE_LEVEL;
	buttonLongEvent.signal    		= SIG_LONG_BUTTON_PRESS;
	buttonShortEvent.signal			= SIG_SHORT_BUTTON_PRESS;

	// Events for LED HSM
	showButtonOnEvent.signal 		= SIG_SHOW_BUTTON;
	showButtonOffEvent.signal 		= SIG_HIDE_BUTTON;
	showChargeLevelEvent.signal		= SIG_SHOW_CHARGE_LEVEL;
	showPowerOnEvent.signal			= SIG_SHOW_POWER;
	showPowerOffEvent.signal		= SIG_HIDE_POWER;
	updateDisplay.signal 			= SIG_UPDATE_DISPLAY;

	// Events for button manager
	kbHitEvent.signal 				= SIG_KBHIT;

	hsm_event_t* anEvent;

	ProtoHSM protoHSM;
	protoHSM.SetInitialState(ProtoHSM::NotCharging);

	LEDHSM ledHSM;
	ledHSM.SetInitialState(LEDHSM::Off);

	ButtonManagerHSM buttonManagerHSM;
	buttonManagerHSM.SetInitialState(ButtonManagerHSM::ButtonManager);

	int tickCount = TICK_COUNT_START;

	cout << "Hit Q to quit" << endl;

	int inChar = 0;
	while(toupper(inChar) != 'Q') {
		inChar = GetButtonInput();

		if (inChar != 0) {
			kbHitEvent.key = inChar;
			//printf("[%c]\n", kbHitEvent.key);
			buttonManagerHSM.InQueuePush(&kbHitEvent);
		}

		static uint32_t updateDisplayTime_ms = 0;
		static clock_t t = clock();
		if ((float)(clock() - t)/CLOCKS_PER_SEC > 0.001 )
		{
			t = clock();
			ledHSM.InQueuePush(&tickEvent);
			buttonManagerHSM.InQueuePush(&tickEvent);
			if (++updateDisplayTime_ms > 100)
			{
				updateDisplayTime_ms = 0;
				ledHSM.InQueuePush(&updateDisplay);
				fflush(stdout);
			}
		}

		// Process all events in each State Machine's in queue
		buttonManagerHSM.Process();
		protoHSM.Process();
		ledHSM.Process();

		// Move items from state machine's out queue into in queues.
		if(buttonManagerHSM.OutQueueGetSize() > 0) {
			anEvent = buttonManagerHSM.OutQueuePop();
			protoHSM.InQueuePush(anEvent);
			ledHSM.InQueuePush(anEvent);
		}
		if(protoHSM.OutQueueGetSize() > 0) {
			ledHSM.InQueuePush(protoHSM.OutQueuePop());
		}
		if(ledHSM.OutQueueGetSize() > 0) {
			protoHSM.InQueuePush(ledHSM.OutQueuePop());
		}
  }

  printf("\nDone.\n");

  return 0;
}
#endif
