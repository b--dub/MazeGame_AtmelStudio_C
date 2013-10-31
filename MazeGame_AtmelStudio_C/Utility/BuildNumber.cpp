#ifndef BUILDINFO_H_ 
#include "buildinfo.h" 
#endif 
 
/************************************************************ 
DO NOT MODIFY 
Automatically Generated On 2013-10-30 11-48-PM by deploy-success99.bat 
*************************************************************/ 
 
#if defined(BUILDINFO_RAM) 
static const uint16_t BUILD_NUMBER = 32; 
#elif defined(BUILDINFO_EEMEM) 
static const uint16_t BUILD_NUMBER EEMEM = 32; 
#elif defined(BUILDINFO_PROGMEM) 
static const uint16_t BUILD_NUMBER PROGMEM = 32; 
#endif 
 
 
uint16_t GetBuildNumber() 
{ 
    uint16_t val; 
#if defined(BUILDINFO_RAM) 
    val = BUILD_NUMBER; 
#elif defined(BUILDINFO_EEMEM) 
    val = eeprom_read_word( &BUILD_NUMBER ); 
#elif defined(BUILDINFO_PROGMEM) 
    val = pgm_read_word(&BUILD_NUMBER); 
#endif 
    return val; 
} 
