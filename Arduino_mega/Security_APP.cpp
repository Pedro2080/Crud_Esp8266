/**
 * @file    Security_APP.cpp
 * @author  Joao Pedro da Silva (joaopedro2080@hotmail.com)
 * @brief   This file contains system functions definitions and describes its state machine
 * @version 1.0
 * @date    2020-09-02
 * 
 * @copyright Copyright (c) 2020
 * 
 */

/****************************************************************************************************************/
/*                                               Includes                                                       */
/****************************************************************************************************************/

#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <Servo.h>
#include "Security_APP.h"

/****************************************************************************************************************/
/*                                               Important macros                                               */
/****************************************************************************************************************/

#define INIT_WAIT_PERIOD                    (5000)
#define CHECK_FOR_10_SECONDS                (200)
#define WAIT_50_MS                          (50)
#define WAIT_2_SECONDS                      (2000)
#define WAIT_5_SECONDS                      (5000)
#define SERVO_DOOR_LOCK                     (0)
#define SERVO_DOOR_OPEN                     (180)
#define USER_PIN_LENGTH                     (4)
#define PIN_ENTER_TRIALS                    (3)
#define PIN_FOUND                           (1)
#define PIN_NOT_FOUND                       (0)
#define USER_OFFSET                         (1)
#define MAX_PIN_LENGTH                      (16)
#define MAX_RFID_BYTES                      (4)
#define RFID_BYTE_EQUALS_TEN                (10)

/****************************************************************************************************************/
/*                                               Users register                                                 */
/****************************************************************************************************************/

/*Users number*/
#define USERS_NUMBER                        (3)

/*
 * Important note: While adding PIN number if you want to use alphabets it must be in uppercase not lowercase.
 * Ex: Use "A" not "a", "F" not "f" and so on.
 * 
 * Important note: While adding RFID card number (ID) insert the alphabets in lowercase not uppercase.
 * Ex: Use "a" not "A", "f" not "F" and so on.
 */

/*User_1*/
#define Lewandowski_NAME                         "Lewandowski"
#define Lewandowski_PIN                          "5555"
#define Lewandowski_RFID_CARD                    "c976133f"  

/*User_2*/
#define Pedro_NAME                                "Pedro"
#define Pedro_PIN                                 "1234"
#define Pedro_RFID_CARD                           "025c5e28" 



/*User_3*/
#define Silva_PIN                                   "8888"
#define Silva_RFID_CARD                             "dea95415"
#define Silva_NAME                                  "Silva"


/*Users names variable (Private)*/
static const char* names[USERS_NUMBER] = {Lewandowski_NAME, Pedro_NAME,Silva_NAME};

/*Users pin variable (private)*/
static String pins[USERS_NUMBER] = {Lewandowski_PIN, Pedro_PIN,Silva_PIN};

/*Users RFID variable (private)*/
static String rfids[USERS_NUMBER] = {Lewandowski_RFID_CARD, Pedro_RFID_CARD,Silva_RFID_CARD};

/****************************************************************************************************************/
/*                                               Global variables                                               */
/****************************************************************************************************************/

/*Keypad keymap definition (Private)*/
static byte KPD_keymap[KPD_ROWS][KPD_COLS] = {
                                               {'1','2','3','A'},
                                               {'4','5','6','B'},
                                               {'7','8','9','C'},
                                               {'*','0','#','D'}
                                             };

/*Keypad pins definition (Private)*/
static byte KPD_cols[KPD_COLS] = {KPD_C1_PIN, KPD_C2_PIN, KPD_C3_PIN, KPD_C4_PIN};
static byte KPD_rows[KPD_ROWS] = {KPD_R1_PIN, KPD_R2_PIN, KPD_R3_PIN, KPD_R4_PIN};

/*Temporary array to compare entered number with users pin*/
static String pin_temp_arr[MAX_PIN_LENGTH] = " ";

/*Variable used to hold the number of the user*/
static byte user_number = 0;

/****************************************************************************************************************/
/*                                             Modules class objects                                            */
/****************************************************************************************************************/

/*LCD module*/
/*Liquid Crystal Display (LCD) class object initialization*/
LiquidCrystal LCD(LCD_RS_PIN, LCD_EN_PIN, LCD_D4_PIN, LCD_D5_PIN, LCD_D6_PIN, LCD_D7_PIN);

/*Keypad module*/
/*Keypad class object initialization*/
Keypad KPD(makeKeymap(KPD_keymap), KPD_rows, KPD_cols, KPD_ROWS, KPD_COLS);

/*Servo module*/
/*Servo class object initialization*/
Servo servo;

/*RFID-RC522 module*/
/*RFID-RC522 class object initialization*/
MFRC522 RFID(RFID_SS_PIN, RFID_RST_PIN);

/****************************************************************************************************************/
/*                                           System functions' definitions                                      */
/****************************************************************************************************************/

void systemInit(void)
{
  /*Begin the serial communication port with Nodemcu over Serial1 port with speed of 9600bps*/
  Serial1.begin(9600);

  /*Begin the SPI communication*/
  SPI.begin();

  /*LEDs initialization as output pins*/
  pinMode(LED_ON_PIN, OUTPUT);
  pinMode(LED_OFF_PIN, OUTPUT);

  /*Buzzer pin initialization as output*/
  pinMode(BUZZER_PIN, OUTPUT);

  /*IR sensor pin initialization as input*/
  pinMode(IR_PIN, INPUT);
  
  /*Nodemcu pin initialization as input*/
  pinMode(NODE_MCU_PIN,INPUT);

  /*Attaching servo to Arduino pin*/
  servo.attach(SERVO_PIN);

  /*Make sure that the door is locked by the servo*/
  servo.write(SERVO_DOOR_LOCK);

  /*Initializing the RFID-RC522 module*/
  RFID.PCD_Init();

  /*Setting LCD columns and rows*/
  LCD.begin(LCD_COLS, LCD_ROWS);

  /*Make the LCD cursor blink*/
  LCD.blink();

  /*Show "Please wait" message on the LCD*/
  LCD.print("Please wait...");

  /*Waiting for Nodemcu to get ready*/
  while(!digitalRead(NODE_MCU_PIN));

  /*Clear the LCD*/
  LCD.clear();

  /*Show "Enter your pin:" message over the LCD*/
  LCD.print("Enter your pin:");

  /*After showing the enter pin message set the cursor to the second row and first column*/
  LCD.setCursor(LCD_COL0,LCD_ROW1);

  /*Return from the function*/
  return;
}

void systemStateMachine(void)
{
  /*System state machine as follows:
   * 1- Enter your pin number at the keypad then press '#'.
   * 2- Check the pin mumber entered.
   * 3- If the pin number is valid, Then you will be asked to scan your RFID card.
   * 4- If the RFID card scanned matches your entered pin number so the door will open by moving servo and display "welcome Home!" over the LCD, 
   *    turn the green LED ON and IR sensor will keep the door open as long as there's a movement.
   * 5- If there's no movement left close the door by moving servo in the opposite direction.
   * 6- If the scanned RFID doesn't match the entered pin number then the LCD will display "Wrong RFID!" and ask to enter your pin number again.
   * 7- If the pin isn't valid ask the user to enter it again for two times.
   * 8- If the user entered the pin three times wrong display "Too many fails!", turn the red LED ON and turn the buzzer ON, Until a valid RFID
   *    is scanned.  
   */
  
  /*Static local user pin trials counter*/
  static byte pin_trials_counter = 0; 
  
  /*Static local user pin checking flag*/
  static byte pin_flag = 0;

  /*Static local variable checking the entered pin numbers counter*/  
  static byte entered_counter = 0;

  /*Local variable used to get the pressed key*/
  byte keypadKey = KPD.getKey();
  
  /*Check if there's a key pressed*/
  if(keypadKey)
  {
    /*Check if the pressed key is '#' or not*/
    if(keypadKey != '#')
    {
      /*Store the entered key in the temporary array*/
      pin_temp_arr[entered_counter] = char(keypadKey);

      /*Increase the entered pin number counter*/
      entered_counter++;

      /*Display the entered number over the LCD*/
      LCD.write(keypadKey);
    }
    else
    {
      /*Local string variable used in user pin comparison*/
      String temp_pin = "";

      /*Local variable used in looping operations*/
      uint8_t iteration_var = 0;

      /*For loop used to transform the entered pin into string*/
      for(iteration_var = 0 ; iteration_var < entered_counter ; iteration_var++)
      {
        /*Store the entered pin into temp_pin variable*/
        temp_pin += pin_temp_arr[iteration_var];

        /*Reset the temp_arr for next operations*/
        pin_temp_arr[iteration_var] = " ";
      }

      /*For loop used to search in the users database for the entered pin*/
      for(iteration_var = 0 ; iteration_var < USERS_NUMBER ; iteration_var++)
      {
        /*Check if the entered pin is available or not*/
        if(temp_pin == pins[iteration_var])
        {
          /*Set the pin_flag variable to PIN_FOUND value if the entered pin is found in the database*/
          pin_flag = PIN_FOUND;

          /*Set the user_number variable with the iteration_var variable value*/
          user_number = iteration_var;
        }
        else
        {
          /*Do nothing*/
        }
      }

      /*Checking if the pin_flag is set to PIN_FOUND value or not*/
      if(pin_flag != PIN_FOUND)
      {
        /*If it's not set, then set it to PIN_NOT_FOUND value*/
        pin_flag = PIN_NOT_FOUND;
      }
      else
      {
        /*Do nothing*/
      }
    }

    /*Check if the entered pin numbers are correct and equal to the pin length after the '#' is pressed*/
    if( (pin_flag == PIN_FOUND) && (keypadKey == '#') )
    {
      /*Local variable used in timeout looping calculation*/
      uint8_t timeout_var = 0;

      /*Change the pin_flag status to PIN_NOT_FOUND for next operations*/
      pin_flag = PIN_NOT_FOUND;

      /*Clear the LCD*/
      LCD.clear();

      /*Display "Scan RFID card:" message*/
      LCD.print("Scan your card:");

      /*Waiting to scan the card*/
      while( (!RFID.PICC_IsNewCardPresent()) && (timeout_var < CHECK_FOR_10_SECONDS) )
      {
        /*Increase the timeout var*/
        timeout_var++;

        /*Wait for 50ms*/
        delay(WAIT_50_MS);
      }

      /*Checking if the timeout reached 10 seconds or not*/
      if(timeout_var == CHECK_FOR_10_SECONDS)
      {
        /*Clear the LCD*/
        LCD.clear();

        /*Display "Time out!" message*/
        LCD.print("Time out!");

        /*Wait for two seconds*/
        delay(WAIT_2_SECONDS);
      }
      else
      {
        /*Local string variable used in user rfid comparison*/
        String temp_rfid = "";

        /*Local variable used in looping operations*/
        uint8_t iteration_var = 0;

        /*Wait to read card serial data*/
        while(!RFID.PICC_ReadCardSerial());

        /*For loop used to transform the entered rfid into string*/
        for(iteration_var = 0 ; iteration_var < MAX_RFID_BYTES ; iteration_var++)
        {
          /*Store the RFID data byte in a temporary variable*/
          byte temp_var = RFID.uid.uidByte[iteration_var];

          /*Check if the byte value is less than ten or not*/
          if(temp_var < RFID_BYTE_EQUALS_TEN)
          {
            /*Add Zero string to the byte*/
            temp_rfid += ("0" + String(temp_var, HEX));
          }
          else
          {
            /*Store the entered RFID byte into temp_rfid variable*/
            temp_rfid += String(temp_var, HEX); 
          }           
        }
        
        /*Checking if the entered RFID is equal to the user RFID or not*/
        if(temp_rfid == rfids[user_number])
        {
          /*Send the user number to Nodemcu*/
          Serial1.write(names[user_number]);

          /*Clear the LCD*/
          LCD.clear();

          /*Display "Welcome Home!" message*/
          LCD.print("Welcome Home!");

          /*Open the door*/
          servo.write(SERVO_DOOR_OPEN);

          /*Turn On the green LED (ON LED) and turn Off the red LED (OFF LED)*/
          digitalWrite(LED_ON_PIN, HIGH);
          digitalWrite(LED_OFF_PIN, LOW);

          /*Wait for 2 seconds*/
          delay(WAIT_2_SECONDS);

          /*If there's a movement then keep the door open*/
          while(digitalRead(IR_PIN) == LOW)
          {
            /*Wait 5 seconds to make sure that the movement has gone*/
            delay(WAIT_5_SECONDS);
          }

          /*Close the door*/
          servo.write(SERVO_DOOR_LOCK);

          /*Turn OFF the green LED (ON LED)*/
          digitalWrite(LED_ON_PIN, LOW);
        }
        else
        {
          /*Clear the LCD*/
          LCD.clear();        

          /*Display "Wrong RFID!" message*/
          LCD.print("Wrong  Card!");

          /*Wait for 2 seconds*/
          delay(WAIT_2_SECONDS);
        }

        /*Stop reading the RFID tags*/
        RFID.PICC_HaltA();
      }
      
      /*Clear the LCD*/
      LCD.clear();

      /*Show "Enter your pin:" message over LCD*/
      LCD.print("Enter your pin:");

      /*After showing the enter pin message set the cursor to the second row and first column*/
      LCD.setCursor(LCD_COL0,LCD_ROW1);

      /*Reset the user entered pin trials counter*/
      pin_trials_counter = 0;

      /*Reset the entered pin numbers counter*/
      entered_counter = 0;      
    }
    else if( (pin_flag == PIN_NOT_FOUND) && (keypadKey == '#') && (pin_trials_counter < PIN_ENTER_TRIALS) )
    {
      /*Clear the LCD*/
      LCD.clear();

      /*Display "Wrong pin!" message over the LCD*/
      LCD.print("Wrong pin!");

      /*Set the cursor to the second row and first column*/
      LCD.setCursor(LCD_COL0,LCD_ROW1);

      /*Increase the user entered pin trials counter*/
      pin_trials_counter++;

      /*Reset the entered pin numbers counter*/
      entered_counter = 0;      
    }
    else
    {
      /*Do nothing*/      
    }
  }
  else
  {
    /*Checking if the user has violated the permitted trials of entering pin number*/
    if( pin_trials_counter == PIN_ENTER_TRIALS )
    {
      /*Clear the LCD*/
      LCD.clear();

      /*Display "Too many fails!" message*/
      LCD.print("Too many fails!");

      /*Make sure that the door is locked*/
      servo.write(SERVO_DOOR_LOCK);

      /*Turn Off the green LED (ON LED)*/
      digitalWrite(LED_ON_PIN, LOW);

      /*Turn On the red LED (ON LED)*/
      digitalWrite(LED_OFF_PIN, HIGH);

      /*Turn On the buzzer*/
      digitalWrite(BUZZER_PIN, HIGH);

      /*Wait for 2 seconds*/
      delay(WAIT_2_SECONDS);

      /*Clear the LCD*/
      LCD.clear();

      /*Display "System locked!" message*/
      LCD.print("System locked!");

      /*Reset the user entered pin trials counter*/
      pin_trials_counter = 0;

      /*Reset the entered pin numbers counter*/
      entered_counter = 0;      

      /*Infinite loop till the user scans an RFID card*/
      while(!RFID.PICC_IsNewCardPresent());

      /*Stop reading the RFID tags*/
      RFID.PICC_HaltA();

      /*Turn Off the red LED (ON LED)*/
      digitalWrite(LED_OFF_PIN, LOW);

      /*Turn Off the buzzer*/
      digitalWrite(BUZZER_PIN, LOW);

      /*Clear the LCD*/
      LCD.clear();

      /*Show "Enter your pin:" message over the LCD*/
      LCD.print("Enter your pin:");

      /*After showing the enter pin message set the cursor to the second row and first column*/
      LCD.setCursor(LCD_COL0,LCD_ROW1);
    }
    else
    {
      /*Do nothing*/
    }    
  }

  /*Return from the function*/
  return;
}
