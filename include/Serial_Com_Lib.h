/*
 * Serial_Com_Lib.h
 * Author: Kyle Crandall
 *
 * This header contains functions, structs and classes to aid in transmitting
 * numerical values over a serial or ethernet conneciton.
 */

#ifndef Serial_Com_Lib_h
#define Serial_Com_Lib_h

typedef unsigned char uchar_t;
typedef unsigned char uint8_t;

union charShort
{
	uchar_t c[2];
	short s;
};

typedef union charShort charShort_t;

void double_to_str(double d, uchar_t* c);
double str_to_double(uchar_t* c);

//The following funcitons are specific to the the TPAWT application of this library

void package_virtual_state(uchar_t* p, double* surface_norm, uchar_t terrain, double* virtual_force, double movement_scale, double user_scale);
void unpackage_virtual_state(uchar_t* p, double* surface_norm, uchar_t* terrain, double* virtual_force, double* movement_scale, double* user_scale);
void package_user_state(uchar_t* p, double* pos, double* user_offset, double* user_eye_offset, double* facing, double* velocity);
void unpackage_user_state(uchar_t* p, double* pos, double* user_offset, double* user_eye_offset, double* facing, double* velocity);

#endif
