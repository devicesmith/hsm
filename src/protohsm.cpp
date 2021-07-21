#include "protohsm.h"
#include <cctype>
#include <cstdint>

// global events
// Events for all
proto_event_t tickEvent;
// Events for Proto HSM
proto_event_t powerConnectedEvent;
proto_event_t powerDisConnectedEvent;
proto_event_t chargeLevelEvent;
proto_event_t buttonLongEvent;
proto_event_t buttonShortEvent;

// Events for LED HSM
proto_event_t showButtonOnEvent;
proto_event_t showButtonOffEvent;
proto_event_t showChargeLevelEvent;
proto_event_t showPowerOnEvent;
proto_event_t showPowerOffEvent;
proto_event_t updateDisplay;

// Button Manager HSM
proto_event_t kbHitEvent;


//
//
//
ProtoHSM::ProtoHSM()
{
}

hsm_state_t * ProtoHSM::GetStateData()
{
	return &protoStateData;
}

hsm_state_result_t ProtoHSM::ProtoHSMTopState(hsm_state_t *stateData, hsm_event_t const *e)
{
	HSM_DEBUG_LOG_STATE_EVENT(stateData, e);
	static int tickCount = 0;

	switch (e->signal)
	{
		case HSM_SIG_ENTRY:
		{
			return HANDLE_STATE();
		}
		case HSM_SIG_EXIT:
		{
			return HANDLE_STATE();
		}
		case HSM_SIG_INITIAL_TRANS:
		{
			return HANDLE_STATE();
		}
		case SIG_TICK_MS:
		{
			return HANDLE_STATE();
		}
	}
	return HANDLE_SUPER_STATE(stateData, &HSM::rootState);
}

hsm_state_result_t ProtoHSM::NotCharging(hsm_state_t *stateData, hsm_event_t const *e)
{
	HSM_DEBUG_LOG_STATE_EVENT(stateData, e);

	switch (e->signal)
	{
		case HSM_SIG_ENTRY:
		{
			PROTOHSM_STATE_DATA_CAST(stateData)->powerConnected = false;
			PROTOHSM_STATE_DATA_CAST(stateData)->chargeLevel = 0;
			stateData->OutQueuePush(&showPowerOffEvent);
			return HANDLE_STATE();
		}
		case HSM_SIG_EXIT:
		{
			return HANDLE_STATE();
		}
		case HSM_SIG_INITIAL_TRANS:
		{
			return HANDLE_STATE();
		}
		case SIG_POWER_CONNECTED:
		{
			return CHANGE_STATE(stateData, &ProtoHSM::Charging);
		}
		case SIG_KBHIT:
		{
			if (toupper(PROTOHSM_EVENT_CAST(e)->key) == 'P')
			{
				return CHANGE_STATE(stateData, &ProtoHSM::Charging);
			}
			else if (toupper(PROTOHSM_EVENT_CAST(e)->key) == 'B')
			{
				stateData->OutQueuePush(&showButtonOffEvent);
				return HANDLE_STATE();
			}
			else if (toupper(PROTOHSM_EVENT_CAST(e)->key) == '+')
			{
				return HANDLE_STATE();
			}
			else if (toupper(PROTOHSM_EVENT_CAST(e)->key) == '-')
			{
				return HANDLE_STATE();
			}
			else if (toupper(PROTOHSM_EVENT_CAST(e)->key) >= '0' &&
					 toupper(PROTOHSM_EVENT_CAST(e)->key) <= '9')
			{
				return HANDLE_STATE();
			}
			return HANDLE_STATE();
		}
	}
	return HANDLE_SUPER_STATE(stateData, &ProtoHSM::ProtoHSMTopState);
}

hsm_state_result_t ProtoHSM::Charging(hsm_state_t *stateData, hsm_event_t const *e)
{
	HSM_DEBUG_LOG_STATE_EVENT(stateData, e);
	switch (e->signal)
	{
		case HSM_SIG_ENTRY:
		{
			PROTOHSM_STATE_DATA_CAST(stateData)->powerConnected = true;
			PROTOHSM_STATE_DATA_CAST(stateData)->chargeLevel = 0;
			stateData->OutQueuePush(&showPowerOnEvent);

			return HANDLE_STATE();
		}
		case HSM_SIG_EXIT:
		{
			return HANDLE_STATE();
		}
		case HSM_SIG_INITIAL_TRANS:
		{
			return HANDLE_STATE();
		}
		case SIG_POWER_DISCONNECTED:
		{
			return CHANGE_STATE(stateData, &ProtoHSM::NotCharging);
		}
	}
	return HANDLE_SUPER_STATE(stateData, &ProtoHSM::ProtoHSMTopState);
}

hsm_state_result_t ProtoHSM::NotCharging_Off(hsm_state_t *stateData, hsm_event_t const *e)
{
	HSM_DEBUG_LOG_STATE_EVENT(stateData, e);
	switch (e->signal)
	{

	}
	return HANDLE_SUPER_STATE(stateData, &ProtoHSM::NotCharging);
}

hsm_state_result_t ProtoHSM::NotCharging_On(hsm_state_t *stateData, hsm_event_t const *e)
{
	HSM_DEBUG_LOG_STATE_EVENT(stateData, e);
	switch (e->signal)
	{

	}
	return HANDLE_SUPER_STATE(stateData, &ProtoHSM::NotCharging);
}


//
//
// LED Manager
//
//
hsm_state_t * LEDHSM::GetStateData()
{
	return &ledStateData;
}

hsm_state_result_t LEDHSM::LEDHSMTopState(hsm_state_t *stateData, hsm_event_t const *e)
{
	HSM_DEBUG_LOG_STATE_EVENT(stateData, e);

	switch (e->signal)
	{
		case HSM_SIG_ENTRY:
		{
			return HANDLE_STATE();
		}
		case HSM_SIG_EXIT:
		{
			return HANDLE_STATE();
		}
		case HSM_SIG_INITIAL_TRANS:
		{
			return HANDLE_STATE();
		}
		case SIG_POWER_CONNECTED:
		{
			((proto_led_state_data_t*)stateData)->ledState[proto_led_state_data_t::PWR] = 'X';

			return HANDLE_STATE();
		}
		case SIG_POWER_DISCONNECTED:
		{
			((proto_led_state_data_t*)stateData)->ledState[proto_led_state_data_t::PWR] = ' ';
			return HANDLE_STATE();
		}
		case SIG_UPDATE_DISPLAY:
		{
			proto_led_state_data_t *pd = (proto_led_state_data_t*)stateData;
			printf("\r");
			printf("Pwr [%c] ", pd->ledState[proto_led_state_data_t::PWR]);
			printf("Btn [%c] ", pd->ledState[proto_led_state_data_t::BTN]);
			printf("Chr [%c] ", pd->ledState[proto_led_state_data_t::CHG1]);
			printf("[%c] ",     pd->ledState[proto_led_state_data_t::CHG2]);
			printf("[%c] ",     pd->ledState[proto_led_state_data_t::CHG3]);
			printf("[%c] ",     pd->ledState[proto_led_state_data_t::CHG4]);
			printf("[%c] ",     pd->ledState[proto_led_state_data_t::CHG5]);
			return HANDLE_STATE();
		}
	}
	return HANDLE_SUPER_STATE(stateData, &HSM::rootState);
}

hsm_state_result_t LEDHSM::Off(hsm_state_t *stateData, hsm_event_t const *e)
{
	HSM_DEBUG_LOG_STATE_EVENT(stateData, e);

	switch (e->signal)
	{
		case HSM_SIG_ENTRY:
		{
			return HANDLE_STATE();
		}
		case HSM_SIG_EXIT:
		{
			return HANDLE_STATE();
		}
		case HSM_SIG_INITIAL_TRANS:
		{
			return HANDLE_STATE();
		}
		//case SIG_LONG_BUTTON_PRESS:
		//{
		//	return HANDLE_STATE();
		//}
		case SIG_SHORT_BUTTON_PRESS:
		{
			return CHANGE_STATE(stateData, &LEDHSM::Scroll);
		}
	}
	return HANDLE_SUPER_STATE(stateData, &LEDHSM::LEDHSMTopState);
}

hsm_state_result_t LEDHSM::Scroll(hsm_state_t *stateData, hsm_event_t const *e)
{
	HSM_DEBUG_LOG_STATE_EVENT(stateData, e);

	switch (e->signal)
	{
		case HSM_SIG_ENTRY:
		{
			((proto_led_state_data_t*)stateData)->ledState[proto_led_state_data_t::CHG1] = 'X';
			((proto_led_state_data_t*)stateData)->ledState[proto_led_state_data_t::CHG2] = 'X';

			return HANDLE_STATE();
		}
		case HSM_SIG_EXIT:
		{
			((proto_led_state_data_t*)stateData)->ledState[proto_led_state_data_t::CHG1] = ' ';
			((proto_led_state_data_t*)stateData)->ledState[proto_led_state_data_t::CHG2] = ' ';
			return HANDLE_STATE();
		}
		case HSM_SIG_INITIAL_TRANS:
		{
			return HANDLE_STATE();
		}
		case SIG_TICK_MS:
		{
			static uint32_t tickCount = 0;
			if (++tickCount > 5000)
			{
				tickCount = 0;
				return CHANGE_STATE(stateData, &LEDHSM::Off);
			}
			//printf("%d\n", ++tickCount);
			//return HANDLE_STATE();
			break; // fall through to let super state handle event too
		}
	}
	return HANDLE_SUPER_STATE(stateData, &LEDHSM::LEDHSMTopState);
}

hsm_state_result_t LEDHSM::ChargeProgress(hsm_state_t *stateData, hsm_event_t const *e)
{
	HSM_DEBUG_LOG_STATE_EVENT(stateData, e);

	switch (e->signal)
	{
		case HSM_SIG_ENTRY:
		{
			return HANDLE_STATE();
		}
		case HSM_SIG_EXIT:
		{
			return HANDLE_STATE();
		}
		case HSM_SIG_INITIAL_TRANS:
		{
			return HANDLE_STATE();
		}
	}
	return HANDLE_SUPER_STATE(stateData, &LEDHSM::LEDHSMTopState);
}

//
// Button Manager
//
hsm_state_t * ButtonManagerHSM::GetStateData()
{
	return &buttonStateData;
}

hsm_state_result_t ButtonManagerHSM::ButtonManagerTopState(hsm_state_t *stateData, hsm_event_t const *e)
{
	HSM_DEBUG_LOG_STATE_EVENT(stateData, e);

	switch (e->signal)
	{
		case HSM_SIG_ENTRY:
		{
			return HANDLE_STATE();
		}
		case HSM_SIG_EXIT:
		{
			return HANDLE_STATE();
		}
		case HSM_SIG_INITIAL_TRANS:
		{
			return HANDLE_STATE();
		}
	}
	return HANDLE_SUPER_STATE(stateData, &HSM::rootState);
}

hsm_state_result_t ButtonManagerHSM::ButtonManager(hsm_state_t *stateData, hsm_event_t const *e)
{
	static bool powerConnected = false;
	static bool buttonDown = false;
	static uint32_t buttonTickCount = 0;
	static uint32_t buttonDownCount = 0;

	HSM_DEBUG_LOG_STATE_EVENT(stateData, e);

	switch (e->signal)
	{
		case HSM_SIG_ENTRY:
		{
			return HANDLE_STATE();
		}
		case HSM_SIG_EXIT:
		{
			return HANDLE_STATE();
		}
		case HSM_SIG_INITIAL_TRANS:
		{
			return HANDLE_STATE();
		}
		case SIG_TICK_MS:
		{
			if (++buttonTickCount > 1000)
			{
				//printf("%d ", ++buttonTickCount);
				buttonTickCount = 0;
			}
			return HANDLE_STATE();
		}
		case SIG_KBHIT:
		{
			if (toupper(PROTOHSM_EVENT_CAST(e)->key) == 'P')
			{
				if (powerConnected)
				{
					stateData->OutQueuePush(&powerDisConnectedEvent);
					powerConnected = false;
				}
				else
				{
					stateData->OutQueuePush(&powerConnectedEvent);
					powerConnected = true;
				}
			}
			if (toupper(PROTOHSM_EVENT_CAST(e)->key) == 'B')  // Long Press
			{
				stateData->OutQueuePush(&buttonLongEvent);
			}
			if (PROTOHSM_EVENT_CAST(e)->key == 'b') // short press
			{
				stateData->OutQueuePush(&buttonShortEvent);
			}
			return HANDLE_STATE();
		}
	}
	return HANDLE_SUPER_STATE(stateData, &LEDHSM::LEDHSMTopState);
}
