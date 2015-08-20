/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>
#include "compiler.h"
#include "board.h"
#include "power_clocks_lib.h"
#include "gpio.h"
#include "pm_uc3c.h"
#include "scif_uc3c.h"
#include "intc.h"
#include "can.h"
#include "canif.h"
#include "usart.h"
#include "print_funcs.h"
//#include "conf_can_example.h"

int main (void)
{
	/* Insert system clock initialization code here (sysclk_init()). */
    sysclk_init();
    
    print_dbg("Clock initialized");
    
    /* Setup generic clock for CAN */
    scif_gc_setup(AVR32_SCIF_GCLK_CANIF,
            SCIF_GCCTRL_CPUCLOCK,
            AVR32_SCIF_GC_NO_DIV_CLOCK,
            0);
    print_dbg("Generic clock setup");
    
    /* Enable generic clock */
    scif_gc_enable(AVR32_SCIF_GCLK_CANIF);
    
    print_dbg("Generic clock enabled");
    

	board_init();
    
    print_dbg("Board initialized");

	/* Insert application code here, after the board has been initialized. */
}
