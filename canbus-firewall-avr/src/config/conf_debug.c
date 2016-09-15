/*
  Copyright (C) 2015, Jaguar Land Rover
  This program is licensed under the terms and conditions of the
  Mozilla Public License, version 2.0.  The full text of the 
  Mozilla Public License is at https://www.mozilla.org/MPL/2.0/
*/

/*
 * conf_debug.c
 *
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