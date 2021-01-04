# Crud_Esp8266

CRUD (Create read update and delete ) .

The system consists on Implementing a  Door Lock System to provides safety at Home that only authorized users have access to it,
in this the user can access the Home by entering a password through a keypad as well as by scanning an access card on RFID module.
When the correct PIN is entered the microcontroller will provide information to a Servo Motor, servo will perform the action with the LCD displaying ‘Welcome Home’, 
the Green Led will stay On and the concerned user is allowed to access Home, there is a security system installed consisting of an infrared presence sensor that acts 
to immediately stopping the action of the door if it detects movement in the range of the door, then door remains open. 

If another user arrives it will ask to enter the PIN again. In case the user enters a wrong PIN the Door will remain Closed the Red LED will turn On and status will be displayed
on LCD ‘Incorrect PIN’, then the user is not allowed to access Home. Therefore the system is programmed when the user enters for multiple times a Wrong PIN,
the Red LED will turn on and the LCD will show ‘Too many fails’ it will activate a Buzzer as an Intruder alert unless the owner stops it by scanning the registered Access Card on
RFID module. A secret reset sequence has been incorporated should the owner forgets the pin and wants to reset the password. Moreover, 
there is a provision of ‘backspace’ and ‘enter’ like functions while entering the PIN using ‘#’ and ‘*’ respectively. 
And store all the dates in Real time on a data base (Firebase).
On the Door Lock System, the user can create a new user to access the door. Retrieve (select), Update and destroy (delete) users.
Storing all the dates in Real time on a data base (Firebase), whose user accessed the door, and the date of access 
