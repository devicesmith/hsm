#include "inc.hpp"
#include <iostream>

#include <unistd.h>
#include <stdio.h>
#include <sys/select.h>
#include <termios.h>

#include <time.h>

#include "HSMFrameWork.h"

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

int main(int argc, char ** argv)
{

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
