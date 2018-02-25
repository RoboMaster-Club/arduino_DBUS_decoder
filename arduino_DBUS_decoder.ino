#include "RemoteControl.h"

int inputLen = 0;
byte inputBuffer[18] = { 0 };
// a useless buffer to dump bad data
byte uselessBuffer[18] = { 0 };
// Raw data when all keys are released. This is for data correction
byte idleData[] = { 0x0, 0x4, 0x20, 0x0, 0x1, 0xD8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 }; // [5] can be 0x98, 0xD8, or 0x58

RC_Ctl_t RC_CtrlData;
char* output = (char*)malloc(100 * sizeof(char));

// use interrupt to read data parallelly to avoid buffer overflow
void serialEvent() {
	if (Serial.available()) {
		inputLen = Serial.readBytes(inputBuffer, 18);
	}
}

void setup()
{
	// baud rate 100kHz, 8bit data, parity even, 1 stop bit
	Serial.begin(100000, SERIAL_8E1);
	while (!Serial.available()) {};
}

void loop()
{
	if (Serial.available()) {
		if (inputLen == 18) {
			//data correction
			bool correctFlag = false;
			int count = 0;
			int i = 0;
			for (; i < 18; i++) {
				count = 0;
				for (int j = 0; j < 18 && (idleData[j] == inputBuffer[(i + j) % 18] || (j == 5 && (inputBuffer[(i + j) % 18] == 0x98 || inputBuffer[(i + j) % 18] == 0x58))); j++) {
					count++;
				}
				if (count == 18) break;
			}

			if (count == 18 && i > 0) {
				Serial.print(i);
				Serial.println(" bits to correct");
				correctFlag = true;
				// dump i bits in the buffer as bad data
				Serial.readBytes(uselessBuffer, i);
			}

			if (correctFlag) return;

			// convert raw data into meaningful data
			RC_CtrlData.rc.ch0 = ((int16_t)inputBuffer[0] | ((int16_t)inputBuffer[1] << 8)) & 0x07FF;
			RC_CtrlData.rc.ch1 = (((int16_t)inputBuffer[1] >> 3) | ((int16_t)inputBuffer[2] << 5)) & 0x07FF;
			RC_CtrlData.rc.ch2 = (((int16_t)inputBuffer[2] >> 6) | ((int16_t)inputBuffer[3] << 2) |
				((int16_t)inputBuffer[4] << 10)) & 0x07FF;
			RC_CtrlData.rc.ch3 = (((int16_t)inputBuffer[4] >> 1) | ((int16_t)inputBuffer[5] << 7)) & 0x07FF;

			RC_CtrlData.rc.s1 = ((inputBuffer[5] >> 4) & 0x000C) >> 2;
			RC_CtrlData.rc.s2 = ((inputBuffer[5] >> 4) & 0x0003);

			RC_CtrlData.mouse.x = ((int16_t)inputBuffer[6]) | ((int16_t)inputBuffer[7] << 8);
			RC_CtrlData.mouse.y = ((int16_t)inputBuffer[8]) | ((int16_t)inputBuffer[9] << 8);
			RC_CtrlData.mouse.z = ((int16_t)inputBuffer[10]) | ((int16_t)inputBuffer[11] << 8);

			RC_CtrlData.mouse.press_l = inputBuffer[12];
			RC_CtrlData.mouse.press_r = inputBuffer[13];

			RC_CtrlData.key.v = ((int16_t)inputBuffer[14]) | ((int16_t)inputBuffer[15] << 8);

			//print result
			for (int i = 0; i < 18; i++) {
				Serial.print(inputBuffer[i], HEX);
				Serial.print(" ");
			}
			sprintf(output, "C0: %d, C1: %d, C2: %d, C3: %d, S1: %d, S2: %d, X: %d, Y: %d, Z: %d, L: %d, R: %d, K: %u", RC_CtrlData.rc.ch0, RC_CtrlData.rc.ch1, RC_CtrlData.rc.ch2, RC_CtrlData.rc.ch3, RC_CtrlData.rc.s1, RC_CtrlData.rc.s2, RC_CtrlData.mouse.x, RC_CtrlData.mouse.y, RC_CtrlData.mouse.z, RC_CtrlData.mouse.press_l, RC_CtrlData.mouse.press_r, RC_CtrlData.key.v);
			Serial.println(output);
			inputLen = 0;
		}
	}
}
