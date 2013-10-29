/*
 * BuildInfo.cpp
 *
 * Created: 10/26/2013 9:02:45 PM
 *  Author: Compaq_Administrator
 */

#ifndef BUILDINFO_H_
#include "buildinfo.h"
#endif

#ifndef Arduino_h
#include <Arduino.h>
#endif

int GetFreeRam ()
{
	extern int __heap_start, *__brkval;
	int v;
	return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

void ShowBuildInfo()
{
	char buffer[HeaderMsgSize];
	GetBuildDate((void*)&buffer,sizeof(buffer));
	Serial.print("Date:");
	Serial.println((char*)&buffer);
	Serial.print("Build:");
	Serial.println(GetBuildNumber());
	Serial.print("Free RAM (B):");
	Serial.println( GetFreeRam());
	Serial.println();
}