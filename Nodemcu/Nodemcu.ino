/**
 * @file    main.cpp
 * @author  Joao Pedro da Silva (joaopedro2080@hotmai.com)
 * @brief   This file contains the main program of the Nodemcu
 * @version 1.0
 * @date    2020-09-06
 * 
 * @copyright Copyright (c) 2020
 * 
 */

/****************************************************************************************************************/
/*                                               Includes                                                       */
/****************************************************************************************************************/

#include <Arduino.h>
#include <FirebaseESP8266.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <time.h>

/****************************************************************************************************************/
/*                                               Networking parameters                                          */
/****************************************************************************************************************/

#define FIREBASE_HOST           "https://securitydoor-ec560.firebaseio.com/"//  link da base de dasos https://securitydoor-ec560.firebaseio.com/
#define FIREBASE_AUTH           "6Swfd2kSHDqQKpiRlwIRuV6I9lda8X8SPoVWP540" //  Chava secreta 6Swfd2kSHDqQKpiRlwIRuV6I9lda8X8SPoVWP540
#define WIFI_SSID               "UPC1377338"    // Name of wifi
#define WIFI_PASSWORD           "MGMJBJXM"   // password

/****************************************************************************************************************/
/*                                               Important macros                                               */
/****************************************************************************************************************/

#define TIME_ZONE               (2)
#define WAIT_1_SECOND           (1000)
#define WAIT_500_MS             (500)
#define NO_OF_SECONDS           (3600)
#define NODE_MCU_ARDUINO_PIN    (D0)
#define RX_PIN                  (D1)
#define TX_PIN                  (D2)
#define DAYLIGHT_OFFSET_SECONDS (0)

/****************************************************************************************************************/
/*                                             Modules class objects                                            */
/****************************************************************************************************************/

/*Firebase class data object initialization*/
FirebaseData firebaseData;

/*Software serial communication class object initialization*/
SoftwareSerial nodemcu_serial(RX_PIN, TX_PIN);

/****************************************************************************************************************/
/*                                               Global variables                                               */
/****************************************************************************************************************/

uint32_t timezone = TIME_ZONE*NO_OF_SECONDS;

void setup() 
{
  /*Set the serial monitor communication speed to 9600*/
  Serial.begin(9600);

  /*Set the serial communication channel with Arduino to 9600*/
  nodemcu_serial.begin(9600);

  /*Set pin D0 as OUTPUT to indicate Nodemcu status with Arduino mega*/
  pinMode(NODE_MCU_ARDUINO_PIN,OUTPUT);

  /*Set D0 pin to LOW level to indicate not ready status*/
  digitalWrite(NODE_MCU_ARDUINO_PIN,LOW);

  /*Setup the WiFi connection*/
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  /*Connecting to the WiFi network*/
  Serial.print("Connecting");
  while(WiFi.status() != WL_CONNECTED)
  {
    /*Waiting to establish the WiFi connection*/
      Serial.print(".");

      /*Wait for 0.5 seond*/
      delay(WAIT_500_MS);
  }
  /*WiFi connection completed*/
  Serial.println("Connected!");

  /*Printing the local IP address of the ESP*/
  Serial.print("The IP Address is:");
  Serial.println(WiFi.localIP());

  /*Establish a connection with Firebase server*/
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  /*Getting the real time from Network time protocol "NTP"*/
  configTime(timezone, DAYLIGHT_OFFSET_SECONDS, "pool.ntp.org", "time.nist.gov");

  /*Waiting for real time*/
  Serial.print("Waiting for internet time");

  /*Wait until the real time comes*/
  while(!time(nullptr))
  {
    Serial.print(".");
    delay(WAIT_1_SECOND);
  }

  /*The real time has come*/
  Serial.println("...Time response OK!");

  /*Set D0 pin to high to indicate ready staus*/
  digitalWrite(NODE_MCU_ARDUINO_PIN,HIGH);
}
void loop() 
{
  /*Variable used in getting the user name*/
  String user_name = "";

  /*Variable used to get the real date*/
  String date_now; 

  /*Variable used to get the real time*/
  String time_now;

  /*Variable used in getting data & time now*/
  time_t now = time(nullptr);

  /*Structure to seperate date & time*/
  struct tm* p_tm = localtime(&now);

  /*Setting the real date*/
  date_now = String(p_tm -> tm_mday) + "/" + String(p_tm -> tm_mon+1) + "/" + String(p_tm -> tm_year+1900);

  /*Setting the real time*/
  time_now = String(p_tm -> tm_hour) + ":" + String(p_tm -> tm_min) + ":" + String(p_tm -> tm_sec); 

  /*Check if there's a new data over communication port or not*/
  if(nodemcu_serial.available())
  {
    /*Get the user number send from Arduino*/
    user_name = nodemcu_serial.readString();

    /*Print the entered user number to the console*/
    Serial.print("The entered user number is:");
    Serial.println(user_name);

    /*Set the user entry time and date at firebase database*/
    Firebase.setString(firebaseData, user_name + " entered at", date_now + " || " + time_now);
  }
  else
  {
    /*Uploading the real date & time to firebase database*/
    Firebase.setString(firebaseData, "1- Date", date_now);
    Firebase.setString(firebaseData, "2- Time", time_now);
  }
}
