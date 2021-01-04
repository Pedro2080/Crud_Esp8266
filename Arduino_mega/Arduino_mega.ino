/**
 * @file    Arduino_mega
 * @author  Joao Pedro da Silva (joaopedro2080@hotmail.com)
 * @brief   This file contains the main functions which are the entry points to the system program 
 * @version 1.0
 * @date    2020-09-03
 * 
 * @copyright Copyright (c) 2020
 * 
 */

/****************************************************************************************************************/
/*                                               Includes                                                       */
/****************************************************************************************************************/

#include <Arduino.h>
#include "Security_APP.h"

/**
 * @brief This function is responsible for initializing the system.
 * 
 */
void setup() 
{
  /*Initialize the security system*/
  systemInit();

  /*Return from the function*/
  return;
}

/**
 * @brief This function is responsible for triggering the state machine of the system in an infinite looping way.
 * 
 */
void loop()
{
  /*Run the security system state machine*/
  systemStateMachine();

  /*Return from the function*/
  return;
}
