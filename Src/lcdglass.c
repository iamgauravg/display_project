/*
 * lcdglass.c
 *
 *  Created on: Oct 14, 2026
 *      Author: Gaurav.Gupta
 */

#include "lcdglass.h"

/**
  @verbatim
================================================================================
                              GLASS LCD MAPPING
================================================================================
               A
       _  ----------
     P|_| |\   |H  /|
         F| G  |  I |B
          |  \ | /  |
          --M-- --N--
          |   /| \  |
         E|  L |  J |C
       _  | /  |K  \|
     Q|_| -----------
              D

A LCD character coding is based on the following matrix:
             COM0    COM1    COM4    COM5     COM6    COM7
  SEG(n)    { 0 ,     0 ,     I ,     B ,      C ,     J }
  SEG(n+1)  { D ,     K ,     A ,     H ,      M ,     N }
  SEG(n+2)  { Q ,     L ,     G ,     F ,      P ,     E }

The character A for example is:
-----------------------------------------------------------
             COM0    COM1     COM4    COM5    COM6     COM7
  SEG(n)    { 0 ,     0 ,      0 ,     1 ,     1 ,      0 }
  SEG(n+1)  { 0 ,     0 ,      1 ,     0 ,     1 ,      1 }
  SEG(n+2)  { 0 ,     0 ,      0 ,     1 ,     0 ,      1 }
   --------------------------------------------------------
           =  0       0        2       5       3        6 hex

   => 'A' = 0x002536

  @endverbatim
  */

uint32_t Digit[6];     /* Digit frame buffer */
uint8_t com_list[6] = { 0, 2, 8, 10, 12, 14 };
/* Letters and number map of the custom LCD 8x40(STM8L152D-EVAL evaluation board) */

__IO const uint32_t Mask[] =
  {
    0x00F00000, 0x000F0000, 0x0000F000, 0x00000F00, 0x000000F0, 0x0000000F
  };

const uint32_t segment_mask[7] = {
		0xE00, 0x7000, 0x38000, 0x1C0000, 0x7, 0x38, 0x1C0
};

const uint32_t segment_pos[7] = {
		9, 12, 15, 18, 0, 3, 6
};

const uint8_t Shift[6] =
  {
    20, 16, 12, 8, 4, 0
  };

__IO const uint32_t LetterMap[26] =
  {
    /*  A            B          C           D           E            F   */
    0x00002536, 0x00202536, 0x00202404, 0x00222310, 0x00202426, 0x00002426,
    /*   G            H          I            J           K         L    */
    0x00202416, 0x00000536, 0x00222200, 0x00200114, 0x00001425, 0x00200404,
    /*   M          N           O            P          Q         R      */
    0x00005514, 0x00004515, 0x00202514, 0x00002526, 0x00002532, 0x00002527,
    /*   S           T           U           V           W           X   */
    0x00202432, 0x00022200, 0x00200514, 0x00041404, 0x00040515, 0x00045001,
    /*  Y          Z            Space*/
    0x00025000, 0x00243000
  };


__IO const uint32_t NumberMap[10] =
  {
    /*   0           1           2          3           4   */
    0x00202514, 0x00000110, 0x00202126, 0x00202132, 0x00000532,
    /*    5         6            7          8           9   */
    0x00202432, 0x00202436, 0x00002110, 0x00202536, 0x00202532
  };


void lcdglass_init(void)
{
	// clock to PWR module - required for VLCD on APB1 bus (how to determine this?)
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	// enable write access to backup domain
	PWR->CR |= PWR_CR_DBP;

	// clock to LCD module
	RCC->APB1ENR |= RCC_APB1ENR_LCDEN;


	// enable GPIO clocks as ports ABCDE are used
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	RCC->AHBENR |= RCC_AHBENR_GPIODEN;
	RCC->AHBENR |= RCC_AHBENR_GPIOEEN;

	////////////////////////////////////
	////////////// Port A //////////////
	////////////////////////////////////

	// all SEG and COM are alternate function
	// mode : PA 1, 2, 3, 6, 7, 8, 9 , 10, 15
	MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODER1_Msk | GPIO_MODER_MODER2_Msk | GPIO_MODER_MODER3_Msk | GPIO_MODER_MODER6_Msk | GPIO_MODER_MODER7_Msk | GPIO_MODER_MODER8_Msk | GPIO_MODER_MODER9_Msk | GPIO_MODER_MODER10_Msk | GPIO_MODER_MODER15_Msk ,
					  GPIO_MODER_MODER1_1   | GPIO_MODER_MODER2_1 | GPIO_MODER_MODER3_1 | GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1 | GPIO_MODER_MODER8_1 | GPIO_MODER_MODER9_1 | GPIO_MODER_MODER10_1 | GPIO_MODER_MODER15_1 );

	// AI: otype set this to push pull
	// ospeed, pupd

	// Select AF11
	MODIFY_REG( GPIOA->AFR[0], GPIO_AFRL_AFSEL1_Msk, 11U << GPIO_AFRL_AFSEL1_Pos);
	MODIFY_REG( GPIOA->AFR[0], GPIO_AFRL_AFSEL2_Msk, 11U << GPIO_AFRL_AFSEL2_Pos);
	MODIFY_REG( GPIOA->AFR[0], GPIO_AFRL_AFSEL3_Msk, 11U << GPIO_AFRL_AFSEL3_Pos);
	MODIFY_REG( GPIOA->AFR[0], GPIO_AFRL_AFSEL6_Msk, 11U << GPIO_AFRL_AFSEL6_Pos);
	MODIFY_REG( GPIOA->AFR[0], GPIO_AFRL_AFSEL7_Msk, 11U << GPIO_AFRL_AFSEL7_Pos);
	MODIFY_REG( GPIOA->AFR[1], GPIO_AFRH_AFSEL8_Msk, 11U << GPIO_AFRH_AFSEL8_Pos);
	MODIFY_REG( GPIOA->AFR[1], GPIO_AFRH_AFSEL9_Msk, 11U << GPIO_AFRH_AFSEL9_Pos);
	MODIFY_REG( GPIOA->AFR[1], GPIO_AFRH_AFSEL10_Msk, 11U << GPIO_AFRH_AFSEL10_Pos);
	MODIFY_REG( GPIOA->AFR[1], GPIO_AFRH_AFSEL15_Msk, 11U << GPIO_AFRH_AFSEL15_Pos);

	////////////////////////////////////
	////////////// Port B //////////////
	////////////////////////////////////

	// all SEG and COM are alternate function
	// mode : PB 0, 1, 3, 4, 5, 8, 9, 10, 11, 12, 13, 14, 15
	MODIFY_REG(GPIOB->MODER, GPIO_MODER_MODER0_Msk | GPIO_MODER_MODER1_Msk | GPIO_MODER_MODER3_Msk | GPIO_MODER_MODER4_Msk | GPIO_MODER_MODER5_Msk | GPIO_MODER_MODER8_Msk | GPIO_MODER_MODER9_Msk | GPIO_MODER_MODER10_Msk | GPIO_MODER_MODER11_Msk | GPIO_MODER_MODER12_Msk | GPIO_MODER_MODER13_Msk | GPIO_MODER_MODER14_Msk | GPIO_MODER_MODER15_Msk ,
					  	  	 GPIO_MODER_MODER0_1   | GPIO_MODER_MODER1_1   | GPIO_MODER_MODER3_1   | GPIO_MODER_MODER4_1   | GPIO_MODER_MODER5_1   | GPIO_MODER_MODER8_1   | GPIO_MODER_MODER9_1   | GPIO_MODER_MODER10_1   | GPIO_MODER_MODER11_1   | GPIO_MODER_MODER12_1   | GPIO_MODER_MODER13_1   | GPIO_MODER_MODER14_1   | GPIO_MODER_MODER15_1 );

	// AI: otype set this to push pull
	// ospeed, pupd

	// Select AF11
	MODIFY_REG( GPIOB->AFR[0], GPIO_AFRL_AFSEL0_Msk, 11U << GPIO_AFRL_AFSEL0_Pos);
	MODIFY_REG( GPIOB->AFR[0], GPIO_AFRL_AFSEL1_Msk, 11U << GPIO_AFRL_AFSEL1_Pos);
	MODIFY_REG( GPIOB->AFR[0], GPIO_AFRL_AFSEL3_Msk, 11U << GPIO_AFRL_AFSEL3_Pos);
	MODIFY_REG( GPIOB->AFR[0], GPIO_AFRL_AFSEL4_Msk, 11U << GPIO_AFRL_AFSEL4_Pos);
	MODIFY_REG( GPIOB->AFR[0], GPIO_AFRL_AFSEL5_Msk, 11U << GPIO_AFRL_AFSEL5_Pos);
	MODIFY_REG( GPIOB->AFR[1], GPIO_AFRH_AFSEL8_Msk, 11U << GPIO_AFRH_AFSEL8_Pos);
	MODIFY_REG( GPIOB->AFR[1], GPIO_AFRH_AFSEL9_Msk, 11U << GPIO_AFRH_AFSEL9_Pos);
	MODIFY_REG( GPIOB->AFR[1], GPIO_AFRH_AFSEL10_Msk, 11U << GPIO_AFRH_AFSEL10_Pos);
	MODIFY_REG( GPIOB->AFR[1], GPIO_AFRH_AFSEL11_Msk, 11U << GPIO_AFRH_AFSEL11_Pos);
	MODIFY_REG( GPIOB->AFR[1], GPIO_AFRH_AFSEL12_Msk, 11U << GPIO_AFRH_AFSEL12_Pos);
	MODIFY_REG( GPIOB->AFR[1], GPIO_AFRH_AFSEL13_Msk, 11U << GPIO_AFRH_AFSEL13_Pos);
	MODIFY_REG( GPIOB->AFR[1], GPIO_AFRH_AFSEL14_Msk, 11U << GPIO_AFRH_AFSEL14_Pos);
	MODIFY_REG( GPIOB->AFR[1], GPIO_AFRH_AFSEL15_Msk, 11U << GPIO_AFRH_AFSEL15_Pos);

	////////////////////////////////////
	////////////// Port C //////////////
	////////////////////////////////////

	// all SEG and COM are alternate function
	// mode : PC 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12
	MODIFY_REG(GPIOC->MODER, GPIO_MODER_MODER0_Msk | GPIO_MODER_MODER1_Msk | GPIO_MODER_MODER2_Msk | GPIO_MODER_MODER3_Msk | GPIO_MODER_MODER4_Msk | GPIO_MODER_MODER5_Msk | GPIO_MODER_MODER6_Msk | GPIO_MODER_MODER7_Msk | GPIO_MODER_MODER8_Msk | GPIO_MODER_MODER9_Msk | GPIO_MODER_MODER10_Msk | GPIO_MODER_MODER11_Msk | GPIO_MODER_MODER12_Msk ,
					  	  	 GPIO_MODER_MODER0_1   | GPIO_MODER_MODER1_1   | GPIO_MODER_MODER2_1   | GPIO_MODER_MODER3_1   | GPIO_MODER_MODER4_1   | GPIO_MODER_MODER5_1   | GPIO_MODER_MODER6_1   | GPIO_MODER_MODER7_1   | GPIO_MODER_MODER8_1   | GPIO_MODER_MODER9_1   | GPIO_MODER_MODER10_1   | GPIO_MODER_MODER11_1   | GPIO_MODER_MODER12_1   );

	// AI: otype set this to push pull
	// ospeed, pupd

	// Select AF11
	MODIFY_REG( GPIOC->AFR[0], GPIO_AFRL_AFSEL0_Msk, 11U << GPIO_AFRL_AFSEL0_Pos);
	MODIFY_REG( GPIOC->AFR[0], GPIO_AFRL_AFSEL1_Msk, 11U << GPIO_AFRL_AFSEL1_Pos);
	MODIFY_REG( GPIOC->AFR[0], GPIO_AFRL_AFSEL2_Msk, 11U << GPIO_AFRL_AFSEL2_Pos);
	MODIFY_REG( GPIOC->AFR[0], GPIO_AFRL_AFSEL3_Msk, 11U << GPIO_AFRL_AFSEL3_Pos);
	MODIFY_REG( GPIOC->AFR[0], GPIO_AFRL_AFSEL4_Msk, 11U << GPIO_AFRL_AFSEL4_Pos);
	MODIFY_REG( GPIOC->AFR[0], GPIO_AFRL_AFSEL5_Msk, 11U << GPIO_AFRL_AFSEL5_Pos);
	MODIFY_REG( GPIOC->AFR[0], GPIO_AFRL_AFSEL6_Msk, 11U << GPIO_AFRL_AFSEL6_Pos);
	MODIFY_REG( GPIOC->AFR[0], GPIO_AFRL_AFSEL7_Msk, 11U << GPIO_AFRL_AFSEL7_Pos);
	MODIFY_REG( GPIOC->AFR[1], GPIO_AFRH_AFSEL8_Msk, 11U << GPIO_AFRH_AFSEL8_Pos);
	MODIFY_REG( GPIOC->AFR[1], GPIO_AFRH_AFSEL9_Msk, 11U << GPIO_AFRH_AFSEL9_Pos);
	MODIFY_REG( GPIOC->AFR[1], GPIO_AFRH_AFSEL10_Msk, 11U << GPIO_AFRH_AFSEL10_Pos);
	MODIFY_REG( GPIOC->AFR[1], GPIO_AFRH_AFSEL11_Msk, 11U << GPIO_AFRH_AFSEL11_Pos);
	MODIFY_REG( GPIOC->AFR[1], GPIO_AFRH_AFSEL12_Msk, 11U << GPIO_AFRH_AFSEL12_Pos);

	////////////////////////////////////
	////////////// Port D //////////////
	////////////////////////////////////

	// all SEG and COM are alternate function
	// mode : PD 2, 8, 9, 10, 11, 12, 13, 14, 15
	MODIFY_REG(GPIOD->MODER,
	           GPIO_MODER_MODER2_Msk  | GPIO_MODER_MODER8_Msk  | GPIO_MODER_MODER9_Msk  |
	           GPIO_MODER_MODER10_Msk | GPIO_MODER_MODER11_Msk | GPIO_MODER_MODER12_Msk |
	           GPIO_MODER_MODER13_Msk | GPIO_MODER_MODER14_Msk | GPIO_MODER_MODER15_Msk,

	           GPIO_MODER_MODER2_1    | GPIO_MODER_MODER8_1    | GPIO_MODER_MODER9_1    |
	           GPIO_MODER_MODER10_1   | GPIO_MODER_MODER11_1   | GPIO_MODER_MODER12_1   |
	           GPIO_MODER_MODER13_1   | GPIO_MODER_MODER14_1   | GPIO_MODER_MODER15_1);
	// AI: otype set this to push pull
	// ospeed, pupd

	// 1. Configure AFRL (AFR[0]) for Pin 2
	MODIFY_REG(GPIOD->AFR[0],
	           GPIO_AFRL_AFSEL2_Msk,
	           (11U << GPIO_AFRL_AFSEL2_Pos));

	// 2. Configure AFRH (AFR[1]) for Pins 8, 9, 10, 11, 12, 13, 14, 15
	MODIFY_REG(GPIOD->AFR[1],
	           GPIO_AFRH_AFSEL8_Msk  | GPIO_AFRH_AFSEL9_Msk  | GPIO_AFRH_AFSEL10_Msk |
	           GPIO_AFRH_AFSEL11_Msk | GPIO_AFRH_AFSEL12_Msk | GPIO_AFRH_AFSEL13_Msk |
	           GPIO_AFRH_AFSEL14_Msk | GPIO_AFRH_AFSEL15_Msk,

	           (11U << GPIO_AFRH_AFSEL8_Pos)  | (11U << GPIO_AFRH_AFSEL9_Pos)  |
	           (11U << GPIO_AFRH_AFSEL10_Pos) | (11U << GPIO_AFRH_AFSEL11_Pos) |
	           (11U << GPIO_AFRH_AFSEL12_Pos) | (11U << GPIO_AFRH_AFSEL13_Pos) |
	           (11U << GPIO_AFRH_AFSEL14_Pos) | (11U << GPIO_AFRH_AFSEL15_Pos));

	////////////////////////////////////
	////////////// Port E //////////////
	////////////////////////////////////

	// all SEG and COM are alternate function
	// mode : PE 0, 1, 2, 3
	MODIFY_REG(GPIOE->MODER,
	           GPIO_MODER_MODER0_Msk  | GPIO_MODER_MODER1_Msk  |
	           GPIO_MODER_MODER2_Msk  | GPIO_MODER_MODER3_Msk,

	           GPIO_MODER_MODER0_1    | GPIO_MODER_MODER1_1    |
	           GPIO_MODER_MODER2_1    | GPIO_MODER_MODER3_1);
	// AI: otype set this to push pull
	// ospeed, pupd

	// Configure AFRL (AFR[0]) for Pins 0, 1, 2, 3
	MODIFY_REG(GPIOE->AFR[0],
	           GPIO_AFRL_AFSEL0_Msk  | GPIO_AFRL_AFSEL1_Msk  |
	           GPIO_AFRL_AFSEL2_Msk  | GPIO_AFRL_AFSEL3_Msk,

	           (11U << GPIO_AFRL_AFSEL0_Pos)  | (11U << GPIO_AFRL_AFSEL1_Pos)  |
	           (11U << GPIO_AFRL_AFSEL2_Pos)  | (11U << GPIO_AFRL_AFSEL3_Pos));

	// Select LSE clock for LCD
	MODIFY_REG(RCC->CSR, RCC_CSR_RTCSEL_HSE_Msk, RCC_CSR_RTCSEL_LSE ); // HSE is 11 so use it as mask and LSE 01;
	// turn LSEON
	RCC->CSR |= RCC_CSR_LSEON;
	// check LSE running
	while( !(RCC->CSR & RCC_CSR_LSERDY));


	// DIV and Bias
	MODIFY_REG(LCD->CR, LCD_CR_DUTY_Msk, LCD_CR_DUTY_2 ); 	// 100 = 1/8 duty
	MODIFY_REG(LCD->CR, LCD_CR_BIAS_Msk, 0 ); 	// 00 = 1/4 bias

	// prescaler and div
	MODIFY_REG(LCD->FCR, LCD_FCR_PS_Msk, 3U << LCD_FCR_PS_Pos); 	// 0011: ck_ps = LCDCLK/4
	MODIFY_REG(LCD->FCR, LCD_FCR_DIV_Msk, 0U << LCD_FCR_DIV_Pos); 	// 0001: ck_div = ck_ps/16
	MODIFY_REG(LCD->FCR, LCD_FCR_CC_Msk, 7U << LCD_FCR_CC_Pos); 	// 0001: ck_div = ck_ps/16

	// wait for sychronization
	while(!(LCD->SR & LCD_SR_FCRSR));

	// enable LCD and wait for enabled
	LCD->CR |= LCD_CR_LCDEN;
	while(!(LCD->SR & LCD_SR_ENS));



}


void lcd_display_character(char my_char, uint8_t my_position)
{
	// find char to com/seg mapping pattern
	uint32_t pattern;
	if(my_char == ' ')
		pattern = 0x0;
	else
		pattern = LetterMap[my_char - 'A'];
	// do I need to convert pattern to digits?
	// display pattern/digits on screen

	for(int i = 0; i < 6; i++)
	{
		Digit[i] = (pattern & Mask[i] ) >> (4*(5 -i));
		MODIFY_REG( LCD->RAM[com_list[i]], segment_mask[my_position] , Digit[i] << (segment_pos[my_position]));
	}

}

void lcd_display_string(char * my_char)
{
	uint8_t string_length = strlen(my_char);

	for(int i = 0; i < string_length; i++)
	{
		lcd_display_character(my_char[i], i);
	}
}
