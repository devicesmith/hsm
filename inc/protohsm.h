#ifndef __PROTO_HSM_H__
#define __PROTO_HSM_H__

#include "hsm.h"

enum proto_signal
{
	// Events to all
	SIG_TICK_MS = HSM_SIG_USER,		// 5  : system clock tick, 1 ms
	// Events to ProtoHSM
	SIG_KBHIT,						// 6  : button was hit, with button info
	SIG_POWER_CONNECTED,			// 7
	SIG_POWER_DISCONNECTED,			// 8
	SIG_CHARGE_LEVEL,				// 9
	SIG_LONG_BUTTON_PRESS,			// 10
	SIG_SHORT_BUTTON_PRESS,			// 11
	// Events to LEDHSM
	SIG_SHOW_BUTTON,  				// 12 : 
	SIG_HIDE_BUTTON,				// 13.: 
	SIG_SHOW_CHARGE_LEVEL,			// 14 : 
	SIG_SHOW_SCROLL_CHARGE_LEVEL,	// 15 :
	SIG_SHOW_POWER,					// 16 :
	SIG_HIDE_POWER,					// 17 :
	SIG_UPDATE_DISPLAY				// 18 :
};

// Forward Declarations

//
// Proto HSM. 
//
struct proto_state_data_t : hsm_state_t
{
	bool powerConnected;
	int chargeLevel;
};

struct proto_event_t : public hsm_event_t
{
	int key;
};

class ProtoHSM : public HSM
{
public:
	ProtoHSM();
	static hsm_state_result_t ProtoHSMTopState(hsm_state_t *stateData, hsm_event_t const *e);
	static hsm_state_result_t Charging(hsm_state_t *stateData, hsm_event_t const *e);
	static hsm_state_result_t NotCharging(hsm_state_t *stateData, hsm_event_t const *e);
	static hsm_state_result_t NotCharging_Off(hsm_state_t *stateData, hsm_event_t const *e);
	static hsm_state_result_t NotCharging_On(hsm_state_t *stateData, hsm_event_t const *e);

	hsm_state_t * GetStateData();

	proto_state_data_t protoStateData;
};


#define PROTOHSM_STATE_DATA_CAST(x)	((proto_state_data_t*)(x))
#define PROTOHSM_EVENT_CAST(x) ((proto_event_t*)(x))

//
// LED HSM
//
struct proto_led_state_data_t : hsm_state_t
{
	enum LED_ID
	{
		PWR = 0,
		BTN = 1,
		CHG1 = 2,
		CHG2 = 3,
		CHG3 = 4,
		CHG4 = 5,
		CHG5 = 6
	};
	// Inputs:
	// - P			: Simulate plugging in the power
	// - 0 thru 9 . : Simulate level of charge
	// - +/-		: Simulate level of charge increase
	// - B          : Simulate button push 
	//              : short press: show level
	//              : long press: turn on or off
    // - 

	// Pwr [ ] Chg [ ] Lvl [ ][ ][ ][ ][ ]
	// Pwr: Plugged in or not . 	: X
	// Chg: Charging or discharging : C/D
	// Lvl: Level of charge         : X
	int ledState[7] = {' ', ' ', ' ', ' ', ' ', ' ', ' '};
};

class LEDHSM : public HSM
{
public:
	hsm_state_t * GetStateData();
	static hsm_state_result_t LEDHSMTopState(hsm_state_t *stateData, hsm_event_t const *e);
	static hsm_state_result_t Off(hsm_state_t *stateData, hsm_event_t const *e);
	static hsm_state_result_t Scroll(hsm_state_t *stateData, hsm_event_t const *e);
	static hsm_state_result_t ChargeProgress(hsm_state_t *stateData, hsm_event_t const *e);

	proto_led_state_data_t ledStateData;
};

struct button_state_data_t : hsm_state_t {

};

class ButtonManagerHSM : public HSM
{
public:
	hsm_state_t * GetStateData();
	static hsm_state_result_t ButtonManagerTopState(hsm_state_t * stateData, hsm_event_t const *e);
	static hsm_state_result_t ButtonManager(hsm_state_t * stateData, hsm_event_t const *e);

	button_state_data_t buttonStateData;
};

#endif
