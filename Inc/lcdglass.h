/*
 * lcdglass.h
 *
 *  Created on: Oct 14, 2026
 *      Author: Gaurav.Gupta
 */

#ifndef LCDGLASS_H_
#define LCDGLASS_H_

#include <stm32l1xx.h>
#include <string.h>





void lcdglass_init(void);
void lcd_display_character(char my_char, uint8_t my_position);
void lcd_display_string(char * my_char);

#endif /* LCDGLASS_H_ */
