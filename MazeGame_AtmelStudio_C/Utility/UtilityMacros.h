/*
 * UtilityMacros.h
 *
 * Created: 10/26/2013 9:02:45 PM
 *  Author: Compaq_Administrator
 */


#ifndef UTILITY_MACROS_H_
#define UTILITY_MACROS_H_

#define DO_PRAGMA(x) _Pragma (#x)
#define TODO(x) DO_PRAGMA(message ("TODO - " #x))

#define XSTR(x) STR(x)
#define STR(x) #x

#endif /* UTILITY_MACROS_H_ */