/**
 * @file    Security_APP.h
 * @author  Joao Pedro da Silva (joaopedro2080@hotmail.com)
 * @brief   This file contains system components configurations and functions prototypes
 * @version 1.0
 * @date    2020-09-02
 * 
 * @copyright Copyright (c) 2020
 * 
 */

/*Header file guard*/
#ifndef __SECURITY_H__
#define __SECURITY_H__

/****************************************************************************************************************/
/*                                      System important macros                                                 */
/****************************************************************************************************************/

/*LCD module configuration*/
#define LCD_COLS               (16)
#define LCD_ROWS               (2)
#define LCD_ROW1               (1)
#define LCD_COL0               (0)
#define LCD_RS_PIN             (22)
#define LCD_EN_PIN             (24)
#define LCD_D4_PIN             (26)
#define LCD_D5_PIN             (28)
#define LCD_D6_PIN             (30)
#define LCD_D7_PIN             (32)

/*Keypad module configuration*/
#define KPD_COLS               (4)
#define KPD_ROWS               (4)
#define KPD_C1_PIN             (23)
#define KPD_C2_PIN             (25)
#define KPD_C3_PIN             (27)
#define KPD_C4_PIN             (29)
#define KPD_R1_PIN             (31)
#define KPD_R2_PIN             (33)
#define KPD_R3_PIN             (35)
#define KPD_R4_PIN             (37)

/*Servo module configuration*/
#define SERVO_PIN              (2)

/*LED module On/Off configuration*/
#define LED_OFF_PIN            (3)
#define LED_ON_PIN             (4)

/*Buzzer module configuration*/
#define BUZZER_PIN             (5)

/*IR module configuration*/
#define IR_PIN                 (6)

/*RFID-RC522 module configuration*/
#define RFID_RST_PIN           (7)
#define RFID_SS_PIN            (53) /*SS is the SDA pin*/

/*Nodemcu module configuration*/
#define NODE_MCU_PIN           (8) /*It's D0 pin on nodemcu*/

/****************************************************************************************************************/
/*                                   System functions' prototypes                                               */
/****************************************************************************************************************/

/**
 * @brief This function is responsible for initializing the whole security system components.
 * 
 */
void systemInit(void);

/**
 * @brief This function is responsible for triggering the state machine of the system and monitor it.
 * 
 */
void systemStateMachine(void);

#endif/*End of file*/
