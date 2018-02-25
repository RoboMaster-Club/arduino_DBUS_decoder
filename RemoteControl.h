// RemoteControl.h

#ifndef _REMOTECONTROL_h
#define _REMOTECONTROL_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

typedef struct
{
	int16_t ch0;
	int16_t ch1;
	int16_t ch2;
	int16_t ch3;
	int8_t s1;
	int8_t s2;
}Remote;
typedef struct
{
	int16_t x;
	int16_t y;
	int16_t z;
	uint8_t last_press_l;
	uint8_t last_press_r;
	uint8_t press_l;
	uint8_t press_r;
}Mouse;
typedef	struct
{
	uint16_t v;
}Key;

typedef struct
{
	Remote rc;
	Mouse mouse;
	Key key;
}RC_Ctl_t;

#endif

