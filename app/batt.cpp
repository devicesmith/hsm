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


int powerOn {' '};
int buttonState {' '};
int ledIndicator[5] {' ', ' ', ' ', ' ', ' '};

void updateDisplay()
{
    static bool hasPrinted {false};
    if(!hasPrinted)
    {
        hasPrinted = true;
        printf("Power Button Level\n");
    }

    printf(" [%c]  [%c]  [%c][%c][%c][%c][%c]\r",
           powerOn, buttonState,
           ledIndicator[0], ledIndicator[1], ledIndicator[2],
           ledIndicator[3], ledIndicator[4]);
}

void showPowerLed(bool on)
{
    if(on)
    {
        powerOn = 'X';
    }
    else
    {
        powerOn = ' ';
    }
}

void showPowerOn()
{
    for(int i = 0; i < 5; i++) ledIndicator[i] = ' ';
    ledIndicator[2] = 'X';
    updateDisplay();
}

void showPowerOff()
{
    for(int i = 0; i < 5; i++) ledIndicator[i] = ' ';
    updateDisplay();
}

void showChargeLevel(int ledOnIndex)
{
    for(int i = 0; i < 5; i++) ledIndicator[i] = ' ';
    for(int i = 0; i < ledOnIndex; i++)
    {
        ledIndicator[i] = 'X';
    }
    updateDisplay();
}

void showButton(bool on)
{
    if(on) buttonState = 'X';
    else buttonState = ' ';
    updateDisplay();
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
      SIG_BUTTON_UP,
      SIG_BUTTON_SHORT,
      SIG_BUTTON_LONG,
      SIG_POWER,
      SIG_TICK,
	  SIG_LAST
    };

    void InitStateMachine(hsm_batt_state * state, state_handler initialState);
    static hsm_state_result RootState(struct hsm_batt_state *self, struct hsm_batt_event const *e);
    static hsm_state_result s0(struct hsm_batt_state* self, struct hsm_batt_event const * e);
    static hsm_state_result PowerOnState(struct hsm_batt_state* self, struct hsm_batt_event const * e);
    static hsm_state_result PowerOffState(struct hsm_batt_state* self, struct hsm_batt_event const * e);

};

#ifdef HSM_DEBUG_LOGGING
#define stringify( name ) # name
const char* signalNames[] = {
  stringify(UNKNOWN),
  stringify(INIT),
  stringify(SILENT),
  stringify(ENTRY),
  stringify(EXIT),
  stringify(INITIAL),
  stringify(BUTTON),
  stringify(BUTTON_UP),
  stringify(BUTTON_SHORT),
  stringify(BUTTON_LONG),
  stringify(POWER),
  stringify(TICK),
  stringify(LAST)
};
#endif

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
    static int buttonTickCount {};
    static bool buttonDown {false};
    static int ledOnIndex {0};

    HSM_DEBUG_PRINT_EVENT(e);
    switch(e->signal)
    {
        case HSM_SIG_ENTRY:
            return HANDLE_STATE();
        case HSM_SIG_EXIT:
            return HANDLE_STATE();
        case HSM_SIG_INITIAL:
            return CHANGE_STATE(self, &BatteryHSM::PowerOffState);
        case SIG_TICK:
            buttonTickCount++;
            updateDisplay();
            return HANDLE_STATE();
        case SIG_BUTTON:
            showButton(true);
            if(!buttonDown)
            {
                buttonTickCount = 0;
                ledOnIndex = 1;
                buttonDown = true;
                showChargeLevel(ledOnIndex);
            }
            else
            {
                //printf("tickCount:%d\n", tickCount);
                if(buttonTickCount > 100)
                {
                    if(++ledOnIndex <= 5)
                    {
                        showChargeLevel(ledOnIndex);
                        buttonTickCount = 0;
                    }
                }
            }
            return HANDLE_STATE();
        case SIG_BUTTON_UP:
//            cout << "Button down for: " << tickCount << "\n";
            buttonDown = false;
            showChargeLevel(0);
            showButton(false);
            return HANDLE_STATE();
    }
    return HANDLE_SUPER_STATE(self, &BatteryHSM::RootState);
}

hsm_state_result BatteryHSM::PowerOnState(struct hsm_batt_state* self, struct hsm_batt_event const * e)
{
    HSM_DEBUG_PRINT_EVENT(e);
    switch(e->signal)
    {
        case HSM_SIG_ENTRY:
            showPowerLed(true);
            return HANDLE_STATE();
        case HSM_SIG_EXIT:
            return HANDLE_STATE();
        case HSM_SIG_INITIAL:
            return HANDLE_STATE();
        case SIG_TICK:
            return HANDLE_SUPER_STATE(self, &BatteryHSM::s0);
        //case SIG_BUTTON:
        //    return HANDLE_STATE();
        case SIG_POWER:
            return CHANGE_STATE(self, &BatteryHSM::PowerOffState);
    }
    return HANDLE_SUPER_STATE(self, &BatteryHSM::s0);
}

hsm_state_result BatteryHSM::PowerOffState(struct hsm_batt_state* self, struct hsm_batt_event const * e)
{
    HSM_DEBUG_PRINT_EVENT(e);
    switch(e->signal)
    {
        case HSM_SIG_ENTRY:
            showPowerLed(false);
            return HANDLE_STATE();
        case HSM_SIG_EXIT:
            return HANDLE_STATE();
        case HSM_SIG_INITIAL:
            return HANDLE_STATE();
        case SIG_TICK:
            return HANDLE_SUPER_STATE(self, &BatteryHSM::s0);
        case SIG_BUTTON_SHORT:
            return HANDLE_STATE();
        case SIG_BUTTON_LONG:
            return HANDLE_STATE();
        case SIG_POWER:
            return CHANGE_STATE(self, &BatteryHSM::PowerOnState);
    }
    return HANDLE_SUPER_STATE(self, &BatteryHSM::s0);
}

int get_button_input() {
    if (_kbhit()) {
        return getchar();
    }
    return 0;
}

double getTimeStamp()
{
    struct timespec currentTime;
    if(clock_gettime(CLOCK_MONOTONIC, &currentTime) >= 0)
    {
        //return 1e9 * currentTime.tv_sec + currentTime.tv_nsec; // nsec
        //return 1e6 * currentTime.tv_sec + currentTime.tv_nsec * 1e-3; // usec
        return 1e3 * currentTime.tv_sec + currentTime.tv_nsec * 1e-6; // msec
        //return currentTime.tv_sec + currentTime.tv_nsec * 1e-9; // sec
    }
    return -1.0;
}

int main(int argc, char ** argv)
{
    cout << "Batt main() running\n";

    BatteryHSM batt;

    batt.InitStateMachine(&batt.state, (state_handler)&batt.s0);

    while(true)
    {
        static bool buttonDown {false};
        static double buttonDownStart {0.0};

        int inChar = get_button_input();
  //      if(inChar != 0)
  //      {
  //          cout <<"got:" << inChar << ":(" << (char)inChar << ")" << "\n";
  //      }

        if(inChar == 'b')
        {
            buttonDown = true;
            buttonDownStart = getTimeStamp();

//            cout << "BUTTON DOWN\n";
            BatteryHSM::hsm_batt_event* e = (BatteryHSM::hsm_batt_event*)hsmEventNew();
            e->signal = static_cast<hsm_signal>(BatteryHSM::hsm_batt_signal::SIG_BUTTON);
            queuePush(&fifoData, e);
        }
        else if(inChar == 'p')
        {
            //cout << "POWER\n";
            BatteryHSM::hsm_batt_event* e = (BatteryHSM::hsm_batt_event*)hsmEventNew();
            e->signal = static_cast<hsm_signal>(BatteryHSM::hsm_batt_signal::SIG_POWER);
            queuePush(&fifoData, e);
        }
        else if(inChar == 'q')
        {
            break;
        }
        else
        {
            //if(buttonDown && (getTimeStamp() - buttonDownStart) > 100.0) // 100 ms
            if(buttonDown && (getTimeStamp() - buttonDownStart) > 50.0)
            {
                //cout << "\nmain -> BUTTON UP\n";

                buttonDown = false;
                BatteryHSM::hsm_batt_event* e = (BatteryHSM::hsm_batt_event*)hsmEventNew();
                e->signal = static_cast<hsm_signal>(BatteryHSM::hsm_batt_signal::SIG_BUTTON_UP);
                queuePush(&fifoData, e);
            }
        }

        static double t = getTimeStamp();
        if((getTimeStamp() - t) > 10.0) // 1000 ms
        {
            t = getTimeStamp();
            BatteryHSM::hsm_batt_event* e = (BatteryHSM::hsm_batt_event*)hsmEventNew();
            e->signal = static_cast<hsm_signal>(BatteryHSM::hsm_batt_signal::SIG_TICK);
            queuePush(&fifoData, e);
        }


        hsmProcess(&batt.state, &fifoData);

        //sleep(0.100);
        usleep(1000.0); // 1 ms
    }

    return 0;
}
