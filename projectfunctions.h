/*
 * projectfunctions.h
 *
 *  Created on: Apr 4, 2023
 *      Author: edgelyj
 */

#ifndef INC_PROJECTFUNCTIONS_H_
#define INC_PROJECTFUNCTIONS_H_

#define SHORTDELAY 250000
#define DELAY 500000
#define LONGDELAY 1000000

//A row pin will read HIGH if a button is pressed, i.e., the circuit will be closed
//When we turn off the columns, eventually, the row pin will lose power
//Once it loses power, we know which column pin the row pin depends on
//The for loop will only run twice, once for column = 1, and again for column = 2
//If the row pin still has power after the second iteration, it can only be column 3, so the function exits
//Row 1 represents the rightmost row, row 2 the middle, and row 3 the leftmost
uint8_t column_determiner(uint32_t GPIO_PIN_x)
{
	uint8_t column = 1;
	for (column = 1; column < 3; column++)
	{
		column_signal_adjuster(column);
		if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_x) == 0)
		{
			break;
		}
	}
	column_signal_adjuster(0);
	return column;
}

//column_signal_adjuster turns on/off the power of the column pins for the keypad
void column_signal_adjuster(uint8_t stage){
	switch (stage)
	{
	case 0:
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11, 1);
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, 1);
		break;
	case 1:
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 0);
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11, 1);
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, 1);
		break;
	case 2:
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11, 0);
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, 1);
		break;
	case 3:
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11, 1);
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, 0);
		break;
	}
}

//Simple function that takes the specified port and pin
//If the function detects that the port has no signal, it returns 1
//Works for pull-down
uint32_t delay(uint32_t time)
{
	for (uint32_t i = 0; i < time; i++){}
}

//Taking the row pin, and the column determined from column_determiner, we can identify the key
//This is because there is only 1 combo of row/column per key
//Due to the nature of the column calculated, the math to determine majority of the keys is X - column
//Example: Pin 13 is on, and column is 1
//This means it is the second row from the top, which is 4, 5, 6
//Column 1 is right most column, so this means pin 6. 7 - 1 = 6.
uint8_t key_identifier(uint32_t GPIO_PIN_x, uint8_t column)
{
	uint8_t keyPressed;
	if (GPIO_PIN_x == GPIO_PIN_10){
		switch (column)
		{
		case 3: // This is actually the *
			return 12;
		case 2: // 0
			return 0;
		case 1: // This is actually the #
			return 11;
		}
	}
	if (GPIO_PIN_x == GPIO_PIN_11) // BOTTOM ROW
	{
		keyPressed = 10 - column;
		return keyPressed;
	}
	if (GPIO_PIN_x == GPIO_PIN_13) // MIDDLE ROW
	{
		keyPressed = 7 - column;
		return keyPressed;
	}
	if (GPIO_PIN_x == GPIO_PIN_15) // LOW ROW
	{
		keyPressed = 4 - column;
		return keyPressed;
	}

}
//Makes the LEDs blink. Values greater than but less than 10 will call led_blinker again to make the LED blink
//multiple times in a row.
void led_blinker(uint8_t number)
{
	if (number == 0){
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, 0);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, 0);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, 0);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, 0);
	}
	if (number == 1){
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, 0);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, 1);
		for (uint32_t i = 0; i < DELAY; i++);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, 0);
		for (uint32_t i = 0; i < DELAY; i++);
	}
	if (number == 2){
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, 0);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, 1);
		for (uint32_t i = 0; i < DELAY; i++);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, 0);
		for (uint32_t i = 0; i < DELAY; i++);
	}
	if (number == 3){
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, 0);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, 1);
		for (uint32_t i = 0; i < DELAY; i++);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, 0);
		for (uint32_t i = 0; i < DELAY; i++);
	}
	if (number <= 6){
		led_blinker(number - 3);
		led_blinker(number - 3);
	}
	else if (number <= 9){
		led_blinker(number - 3);
		led_blinker(number - 3);
	}
}

//function seven_segment_control controls the signal to the pins for the seven segment display
//A 1 represents OFF, and a 0 means that the pin will be on
//seven_segment_control(8) will turn on all the pins, and seven_segment_control(10) will turn off the pins
//The input, number, is calculated from the key_identifier function
void seven_segment_control(uint8_t number)
{
	//All pins are on port D, from Pin 0 to Pin 7. Pin 0 controls the decimal point.
	uint32_t sevenSegmentMap[8] = {GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3,
							   GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7,};
	switch (number)
	{
		case 0:
			seven_segment_control(8);
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, 1);	//Middle horizontal
			return;
		case 1:
			seven_segment_control(10);
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, 0);	//Bottom right vertical
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, 0);	//Top right vertical
			return;
		case 2:
			seven_segment_control(8);
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, 1);	//Botton Right Vertical
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, 1);	//Top Left Vertical
			return;
		case 3:
			seven_segment_control(8);
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, 1);	//Bottom Left Vertical
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, 1);	//Top Left Vertical
			return;
		case 4:
			seven_segment_control(8);
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, 1);	//Top Horizontal
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, 1);	//Bottom Left Vertical
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, 1);	//Bottom Horizontal
			return;
		case 5:
			seven_segment_control(8);
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, 1);	//Top Right Vertical
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, 1);	//Bottom Left Vertical
			return;
		case 6:
			seven_segment_control(8);
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, 1);	//Top Right Vertical
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, 1);	//Top horizontal
			return;
		case 7:
			seven_segment_control(10);
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, 0);
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, 0);
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, 0);	//Top horizontal
			return;
		case 8: //All on - allows for quicker simpler code
			for (uint8_t i = 1; i < 8; i++)
			{
				HAL_GPIO_WritePin(GPIOD, sevenSegmentMap[i], 0);
			}
			return;
		case 9:
			seven_segment_control(8);
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, 1);
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, 1);
			return;
		case 10: //Reset
			for (uint8_t i = 1; i < 8; i++)
			{
				HAL_GPIO_WritePin(GPIOD, sevenSegmentMap[i], 1);
			}
			return;
	}
}

//main_two exists just to simplify the code a little bit
//It takes the input of GPIO_PIN_x, which will be either B10, B11, B13, or B15, the row pins for the keypad

uint8_t key_finder_function(GPIO_PIN_x)
{
	uint8_t columnPressed;
	uint8_t keyPressed;
	columnPressed = column_determiner(GPIO_PIN_x);
	keyPressed = key_identifier(GPIO_PIN_x, columnPressed);
	return keyPressed;
}
#endif /* INC_PROJECTFUNCTIONS_H_ */
