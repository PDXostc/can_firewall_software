/*
 * conf_debug.c
 *
 * Created: 11/12/2015 2:09:58 PM
 *  Author: smiller6
 */ 
#include "conf_debug.h"

void print_array_uint8(uint8_t *arr, int length)
{
	PRINT_NEWLINE()
	print_dbg("Array Values: ");
	for (int i = 0; i < length; i++)
	{
		print_dbg_char_hex(arr[i]);
		print_dbg(" | ");
	}
}