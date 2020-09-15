/***************************************************************************************
    Name    : Stepper_Motor_Control_Using_LCD
    Created : September 2, 2020
    Last Modified: September 13, 2020
    Version : 1.4
 ***************************************************************************************
 Additions:
 Option to change RPM, Motor_Steps, Micro_Steps
 All values saved in EEPROM
 ***************************************************************************************
 References :
 1-Menu code for LCD from Paul Siewert. 
    Link:.https://www.instructables.com/id/Arduino-Uno-Menu-Template/
 2-BasicStepperDriver library from laurb9. 
    Link:.https://github.com/laurb9/StepperDriver
 ***************************************************************************************
 Steps:
 1:Download and install "BasicStepperDriver.h" Library
 2:Connect LCD Keypad shield to arduino Board [Check connection with respect to 5v and GND]
 3:Connect Power to Stepper/Servo motor as recommended by the User manual of stepper/servo
 4:Connections for Stepper/Servo with Arduino Mega2560
   a:Connect Pulse-/DIR-/Enable- to GND
   b:Connect Pulse+ to 48
   c:Connect DIR+ to 50
   d:Connect Enable+ to 52
 5:Check all the connection 
 6:Connect arduino Mega 2560 to your PC/LAPTOP and select appropriate COM port
 7:Upload the Program and wait till its done uploading
 8:Once uploding is completed you are set to start changing the setting parameters as per your need
 9:There are 8 menu items available to change the settings
 10:Value displayed in START menu is incremented/Decremented when the motor moves
 11:DIRECTION menu indicates CW(clockwise)/CCW(counter clockwise direction)
 12:DEGREE menu is used to set the degree/angle for which the rotation is desired
 13:NO.OF CYCLES menu will determine mow many times the rotation will happen when button is pressed
 14:RPM menu is used to set the desired RPM of the motor [Range:1 - 1000]
 15:Micro_Step menu is used to set the microstep value configured on the motor driver
 16:Motor_Step menu is used to set the steps taken by motor to complete 1 Rotation
 17:SET_ZERO menu is used to clear values position, and degree
 18:Key map and function
    a:Right key - Enter specified option
    b:Left key - Go back to main menu
    c:Up key - Increment values
    d:Down key - Decrement values
 19:Use RESET key to Implement changes   
 ***************************************************************************************/

/*Libraries */
#include <Arduino.h>
#include "BasicStepperDriver.h"   //External library 
#include <Wire.h>                 //Inbuilt Library
#include <LiquidCrystal.h>        //Inbuilt Library
#include <EEPROM.h>               //Inbuilt Library

///////////////////////////////////////////////////////////////////////
                        /* Eeprom */
int address = 0;

int readIntFromEEPROM(int address)
{
  byte byte1 = EEPROM.read(address);
  byte byte2 = EEPROM.read(address + 1);
  return (byte1 << 8) + byte2;
}

void writeIntIntoEEPROM(int address, int number)
{ 
  byte byte1 = number >> 8;
  byte byte2 = number & 0xFF;
  EEPROM.write(address, byte1);
  EEPROM.write(address + 1, byte2);
}
///////////////////////////////////////////////////////////////////////
                        /* Stepper */
// Motor steps per revolution. Most steppers are 200 steps or 1.8 degrees/step
int Motor_Step_Eeprom_Address = 2; //Eeprom address
int MOTOR_STEPS=abs(readIntFromEEPROM(Motor_Step_Eeprom_Address));// EEPROM value Read

int RPM_Eeprom_Address = 6; //Eeprom address
int RPM=abs(readIntFromEEPROM(RPM_Eeprom_Address));// EEPROM value Read


// Since microstepping is set externally, make sure this matches the selected mode
// If it doesn't, the motor will move at a different RPM than chosen
// 1=full step, 2=half step etc.
int MICROSTEPS_Eeprom_Address = 10; //Eeprom address
int MICROSTEPS=abs(readIntFromEEPROM(MICROSTEPS_Eeprom_Address));// EEPROM value Read


// All the wires needed for full functionality
#define DIR 50
#define STEP 48
//Uncomment line to use enable/disable functionality
#define ENABLE 52

// 2-wire basic config, microstepping is hardwired on the driver
//BasicStepperDriver stepper(MOTOR_STEPS, DIR, STEP);

//Uncomment line to use enable/disable functionality
BasicStepperDriver stepper(MOTOR_STEPS, DIR, STEP, ENABLE);
///////////////////////////////////////////////////////////////////////////
                        /* LCD */
// You can have up to 10 menu items in the menuItems[] array below without having to change the base programming at all. Name them however you'd like. Beyond 10 items, you will have to add additional "cases" in the switch/case
// section of the operateMainMenu() function below. You will also have to add additional void functions (i.e. menuItem11, menuItem12, etc.) to the program.
String menuItems[] = {"Start", "Direction","Degree","No.of Cycles","RPM","Micro_Step","Motor_Step","Set Zero"};
String Direction[] = {"CW", "CCW"};

// Navigation button variables
int readKey;
int savedDistance = 0;

// Variable to store data for stepper movment //
int Start = 0;

int direct = 0;

int Degree_Eeprom_Address = 14; //Eeprom address
int Degree = abs(readIntFromEEPROM(Degree_Eeprom_Address));// EEPROM

int Cycles_Eeprom_Address = 18; //Eeprom address
int Cycles = abs(readIntFromEEPROM(Cycles_Eeprom_Address)); // EEPROM



int possition_Eeprom_Address = 22; //Eeprom address
int possition = abs(readIntFromEEPROM(possition_Eeprom_Address));// EEPROM

// Menu control variables
int menuPage = 0;
int maxMenuPages = 6;
//int maxMenuPages = round(((sizeof(menuItems) / sizeof(String)) / 2) + .5);


int cursorPosition = 0;

// Creates 3 custom characters for the menu display
byte downArrow[8] = {
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b10101, // * * *
  0b01110, //  ***
  0b00100  //   *
};

byte upArrow[8] = {
  0b00100, //   *
  0b01110, //  ***
  0b10101, // * * *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100  //   *
};

byte menuCursor[8] = {
  B01000, //  *
  B00100, //   *
  B00010, //    *
  B00001, //     *
  B00010, //    *
  B00100, //   *
  B01000, //  *
  B00000  //
};

// Setting the LCD shields pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

void setup() {

  // Initializes serial communication
  Serial.begin(9600);
  stepper.begin(RPM, MICROSTEPS);
  // Initializes and clears the LCD screen
  lcd.begin(16, 2);
  lcd.clear();

  // Creates the byte for the 3 custom characters
  lcd.createChar(0, menuCursor);
  lcd.createChar(1, upArrow);
  lcd.createChar(2, downArrow);

//  Serial.println(MOTOR_STEPS);
}

void loop() {
  mainMenuDraw();
  drawCursor();
  operateMainMenu();
}

// This function will generate the 2 menu items that can fit on the screen. They will change as you scroll through your menu. Up and down arrows will indicate your current menu position.
void mainMenuDraw() {
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print(menuItems[menuPage]);
  lcd.setCursor(1, 1);
  lcd.print(menuItems[menuPage + 1]);
  if (menuPage == 0) {
    lcd.setCursor(15, 1);
    lcd.write(byte(2));
  } else if (menuPage > 0 and menuPage < maxMenuPages) {
    lcd.setCursor(15, 1);
    lcd.write(byte(2));
    lcd.setCursor(15, 0);
    lcd.write(byte(1));
  } else if (menuPage == maxMenuPages) {
    lcd.setCursor(15, 0);
    lcd.write(byte(1));
  }
}

// When called, this function will erase the current cursor and redraw it based on the cursorPosition and menuPage variables.
void drawCursor() {
  for (int x = 0; x < 2; x++) {     // Erases current cursor
    lcd.setCursor(0, x);
    lcd.print(" ");
  }

  // The menu is set up to be progressive (menuPage 0 = Item 1 & Item 2, menuPage 1 = Item 2 & Item 3, menuPage 2 = Item 3 & Item 4), so
  // in order to determine where the cursor should be you need to see if you are at an odd or even menu page and an odd or even cursor position.
  if (menuPage % 2 == 0) {
    if (cursorPosition % 2 == 0) {  // If the menu page is even and the cursor position is even that means the cursor should be on line 1
      lcd.setCursor(0, 0);
      lcd.write(byte(0));
    }
    if (cursorPosition % 2 != 0) {  // If the menu page is even and the cursor position is odd that means the cursor should be on line 2
      lcd.setCursor(0, 1);
      lcd.write(byte(0));
    }
  }
  if (menuPage % 2 != 0) {
    if (cursorPosition % 2 == 0) {  // If the menu page is odd and the cursor position is even that means the cursor should be on line 2
      lcd.setCursor(0, 1);
      lcd.write(byte(0));
    }
    if (cursorPosition % 2 != 0) {  // If the menu page is odd and the cursor position is odd that means the cursor should be on line 1
      lcd.setCursor(0, 0);
      lcd.write(byte(0));
    }
  }
}


void operateMainMenu() {
  int activeButton = 0;
  while (activeButton == 0) {
    int button;
    readKey = analogRead(0);
    if (readKey < 790) {
      delay(100);
      readKey = analogRead(0);
    }
    button = evaluateButton(readKey);
    switch (button) {
      case 0: // When button returns as 0 there is no action taken
        break;
      case 1:  // This case will execute if the "forward" button is pressed
        button = 0;
        switch (cursorPosition) 
        { // The case that is selected here is dependent on which menu page you are on and where the cursor is.
          case 0:
            menuItem1(sizeof("Start"));
            break;
          case 1:
            menuItem2(sizeof("Direction"));
            break;
          case 2:
            menuItem3(sizeof("Degree"));
            break;
          case 3:
            menuItem4(sizeof("No.of Cycles"));
            break;
          case 4:
            menuItem5(sizeof("RPM"));
            break;
          case 5:
            menuItem6(sizeof("Micro_Step"));
            break;
          case 6:
            menuItem7(sizeof("Motor_Step"));
            break;
          case 7:
            menuItem8(sizeof("Set Zero"));
            break;
        }
        activeButton = 1;
        mainMenuDraw();
        drawCursor();
        break;
      case 2:
        button = 0;
        if (menuPage == 0) 
        {
          cursorPosition = cursorPosition - 1;
          cursorPosition = constrain(cursorPosition, 0, ((sizeof(menuItems) / sizeof(String)) - 1));
        }
        if (menuPage % 2 == 0 and cursorPosition % 2 == 0) 
        {
          menuPage = menuPage - 1;
          menuPage = constrain(menuPage, 0, maxMenuPages);
        }

        if (menuPage % 2 != 0 and cursorPosition % 2 != 0) 
        {
          menuPage = menuPage - 1;
          menuPage = constrain(menuPage, 0, maxMenuPages);
        }

        cursorPosition = cursorPosition - 1;
        cursorPosition = constrain(cursorPosition, 0, ((sizeof(menuItems) / sizeof(String)) - 1));

        mainMenuDraw();
        drawCursor();
        activeButton = 1;
        break;
      case 3:
        button = 0;
        if (menuPage % 2 == 0 and cursorPosition % 2 != 0) 
        {
          menuPage = menuPage + 1;
          menuPage = constrain(menuPage, 0, maxMenuPages);
        }

        if (menuPage % 2 != 0 and cursorPosition % 2 == 0) 
        {
          menuPage = menuPage + 1;
          menuPage = constrain(menuPage, 0, maxMenuPages);
        }

        cursorPosition = cursorPosition + 1;
        cursorPosition = constrain(cursorPosition, 0, ((sizeof(menuItems) / sizeof(String)) - 1));
        mainMenuDraw();
        drawCursor();
        activeButton = 1;
        break;
    }
  }
}

// This function is called whenever a button press is evaluated. The LCD shield works by observing a voltage drop across the buttons all hooked up to A0.
int evaluateButton(int x) {
  int result = 0;
  if (x < 50) 
  {
    result = 1; // right
  } else if (x < 195) 
  {
    result = 2; // up
  } else if (x < 380) 
  {
    result = 3; // down
  } else if (x < 790) 
  {
    result = 4; // left
  }
  return result;
}

// If there are common usage instructions on more than 1 of your menu items you can call this function from the sub
// menus to make things a little more simplified. If you don't have common instructions or verbage on multiple menus
// I would just delete this void. You must also delete the drawInstructions()function calls from your sub menu functions.
void drawInstructions() {
  lcd.setCursor(0, 1); // Set cursor to the bottom line
  lcd.print("Use ");
  lcd.write(byte(1)); // Up arrow
  lcd.print("/");
  lcd.write(byte(2)); // Down arrow
  lcd.print(" buttons");
}

void menuItem1(int str_len) { //Start
  int activeButton = 0;
  
  lcd.clear();
  lcd.setCursor(8-(str_len/2), 0);
  lcd.print("Start");
  display_value(possition);
  while (activeButton == 0) {
    int button;
    readKey = analogRead(0);
    if (readKey < 790) {
      delay(100);
      readKey = analogRead(0);
    }
    button = evaluateButton(readKey);
    switch (button) {
      case 1:  // This case will execute if the "Forward" button is pressed
        button = 0;
        
        break;
      case 2:  // This case will execute if the "Up" button is pressed
        button = 0;
        stepper.enable();
        stepper.rotate(Degree);
        stepper.disable();
        if((possition + Degree) >= 360) //after 1 rotation set value of position to wrap around position
        {
          possition = possition - 360;
        }
        possition = possition + Degree;
        display_value(possition);
        break;
      case 3:  // This case will execute if the "Down" button is pressed
      // Rotation for Counter clock wise direction
        button = 0;
        stepper.enable();
        stepper.rotate(-Degree);
        stepper.disable();
        
        possition = possition - Degree;
        if((possition - Degree) < 0)
        {
          possition = 360 - abs(possition);
          if(possition == 360)//during reverse roatation if position is 360 print it as 0
          {
            possition = 0;
          }
        }
        display_value(possition);
        break;    
      case 4:  // This case will execute if the "back" button is pressed
        button = 0;
        writeIntIntoEEPROM(possition_Eeprom_Address, possition);
        activeButton = 1;
        break;
    }
  }
}

void menuItem2(int str_len) { //Direction
  int activeButton = 0;

  lcd.clear();
  lcd.setCursor(8-(str_len/2), 0);
  lcd.print("Direction");
  lcd.setCursor(0, 1);
  lcd.write(byte(0));
  lcd.setCursor(1, 1);
  lcd.print(Direction[direct]);
  while (activeButton == 0) {
    int button;
    readKey = analogRead(0);
    if (readKey < 790) {
      delay(100);
      readKey = analogRead(0);
    }
    button = evaluateButton(readKey);
    switch (button) {
      case 0:  // This case will execute if the "back" button is pressed
        button = 0;
        break;
      case 1:  // This case will execute if the "back" button is pressed
        break;      
      case 2:  // This case will execute if the "Down" button is pressed
        button = 0;
        if(direct == 1)
        {
          direct = 0;
        }
        for (int x = 1; x < 16; x++) // Erases current cursor
        {     
          lcd.setCursor(x, 1);
          lcd.print(" ");
        }
        lcd.setCursor(1, 1);
        lcd.print(Direction[direct]);
        break;
      case 3:  // This case will execute if the "up" button is pressed
        button = 0;
        if(direct == 0)
        {
          direct = 1;
        }
        for (int x = 1; x < 16; x++) // Erases current cursor
        {     
          lcd.setCursor(x, 1);
          lcd.print(" ");
        }
        lcd.setCursor(1, 1);
        lcd.print(Direction[direct]);
        break;
      case 4:  // This case will execute if the "back" button is pressed
        button = 0;
        activeButton = 1;
        break;
    }
  }
}

void menuItem3(int str_len) { //Degree
  int activeButton = 0;

  lcd.clear();
  lcd.setCursor(8-(str_len/2), 0);
  lcd.print("Degree");
  display_value(Degree);
  while (activeButton == 0) {
    int button;
    readKey = analogRead(0);
    if (readKey < 790) {
      delay(100);
      readKey = analogRead(0);
    }
    button = evaluateButton(readKey);
    switch (button) {
      case 0:  // This case will execute if the "Forward" button is pressed
        break;
      case 1:  // This case will execute if the "Forward" button is pressed
        break;
      case 2:  // This case will execute if the "Up" button is pressed
        button = 0;
        if(Degree<360)
        {
          Degree++;
        }else
        {
          Degree=0;
        }
        display_value(Degree);
        break;
      case 3:  // This case will execute if the "Down" button is pressed
        button = 0;
        if(Degree>0)
        {
          Degree--;
        }else
        {
          Degree = 360;
        }
        display_value(Degree);
        break;    
      case 4:  // This case will execute if the "back" button is pressed
        button = 0;
        writeIntIntoEEPROM(Degree_Eeprom_Address, Degree);
        activeButton = 1;
        break;
    }
  }
}

void menuItem4(int str_len) { //No.of Cycles
  int activeButton = 0;

  lcd.clear();
  lcd.setCursor(8-(str_len/2), 0);
  lcd.print("Cycles");
  display_value(Cycles);
  while (activeButton == 0) {
    int button;
    readKey = analogRead(0);
    if (readKey < 790) {
      delay(100);
      readKey = analogRead(0);
    }
    button = evaluateButton(readKey);
    switch (button) {
      case 1:  // This case will execute if the "Up" button is pressed
        break;
      case 2:  // This case will execute if the "Up" button is pressed
        button = 0;
        if(Cycles < 360)//cycles can not be greater that 360
        {
          Cycles++;   
        }else {Cycles = 1;}
        display_value(Cycles);
      break;
      case 3:  // This case will execute if the "Down" button is pressed
        button = 0;
        if(Cycles>1)
        {
          Cycles--;
        }else{Cycles = 360;}
        display_value(Cycles);
      break;
      case 4:  // This case will execute if the "back" button is pressed
        button = 0;
        writeIntIntoEEPROM(Cycles_Eeprom_Address, Cycles);
        activeButton = 1;
      break;
    }
  }
}

void menuItem5(int str_len) { //RPM
  int activeButton = 0;

  lcd.clear();
  lcd.setCursor(8-(str_len/2), 0);
  lcd.print("RPM");
  display_value(RPM);
  while (activeButton == 0) {
    int button;
    readKey = analogRead(0);
    if (readKey < 790) {
      delay(100);
      readKey = analogRead(0);
    }
    button = evaluateButton(readKey);
    switch (button) {
      case 0:  // This case will execute if the "back" button is pressed
        break;
      case 1:  // This case will execute if the "back" button is pressed
        button = 0;
        break;
      case 2:  // This case will execute if the "back" button is pressed
        button = 0;
        if(RPM<1000)
        {
          RPM++;
        }
//        else
//        {
//          MICROSTEPS=1;
//        }
        display_value(RPM);
        break;
      case 3:  // This case will execute if the "back" button is pressed
        button = 0;
        if(RPM>1)
        {
          RPM--;
        }
//        else
//        {
//          MICROSTEPS=32;
//        }
        display_value(RPM);
        break;
      case 4:  // This case will execute if the "back" button is pressed
        button = 0;
        writeIntIntoEEPROM(RPM_Eeprom_Address, RPM);
        activeButton = 1;
        break;
    }
  }
}

void menuItem6(int str_len) { //Micro_Step
  int activeButton = 0;

  lcd.clear();
  lcd.setCursor(8-(str_len/2), 0);
  lcd.print("Micro_Step");
  display_value(MICROSTEPS);
  while (activeButton == 0) {
    int button;
    readKey = analogRead(0);
    if (readKey < 790) {
      delay(100);
      readKey = analogRead(0);
    }
    button = evaluateButton(readKey);
    switch (button) {
      case 0:  // This case will execute if the "back" button is pressed
        break;
      case 1:  // This case will execute if the "back" button is pressed
        button = 0;
        break;
      case 2:  // This case will execute if the "back" button is pressed
        button = 0;
        if(MICROSTEPS<32)
        {
          MICROSTEPS *= 2;
        }
//        else
//        {
//          MICROSTEPS=1;
//        }
        display_value(MICROSTEPS);
        break;
      case 3:  // This case will execute if the "back" button is pressed
        button = 0;
        if(MICROSTEPS>1)
        {
          MICROSTEPS /= 2;
        }
//        else
//        {
//          MICROSTEPS=32;
//        }
        display_value(MICROSTEPS);
        break;
      case 4:  // This case will execute if the "back" button is pressed
        button = 0;
        writeIntIntoEEPROM(MICROSTEPS_Eeprom_Address, MICROSTEPS);
        activeButton = 1;
        break;
    }
  }
}

void menuItem7(int str_len) { //MOTOR_STEP
  int activeButton = 0;

  lcd.clear();
  lcd.setCursor(8-(str_len/2), 0);
  lcd.print("Motor_Step");
  display_value(MOTOR_STEPS);
  while (activeButton == 0) {
    int button;
    readKey = analogRead(0);
    if (readKey < 790) {
      delay(100);
      readKey = analogRead(0);
    }
    button = evaluateButton(readKey);
    switch (button) {
      case 1:  // This case will execute if the "back" button is pressed
        button = 0;
        break;
      case 2:  // This case will execute if the "back" button is pressed
        button = 0;
        if(MOTOR_STEPS<1000)
        {
          MOTOR_STEPS++;
        }else
        {
          MOTOR_STEPS=0;
        }
        display_value(MOTOR_STEPS);
        break;
      case 3:  // This case will execute if the "back" button is pressed
        button = 0;
        if(MOTOR_STEPS>0)
        {
          MOTOR_STEPS--;
        }else
        {
          MOTOR_STEPS = 1000;
        }
        display_value(MOTOR_STEPS);
        break;
      case 4:  // This case will execute if the "back" button is pressed
        button = 0;
        writeIntIntoEEPROM(Motor_Step_Eeprom_Address, MOTOR_STEPS);
        activeButton = 1;
        break;
    }
  }
}

void menuItem8(int str_len) { //Set Zero
  int activeButton = 0;
  lcd.clear();
  lcd.setCursor(8-(str_len/2), 0);
  lcd.print("Set Zero");
  lcd.setCursor(0, 1);
  lcd.write(byte(0));
  lcd.setCursor(1, 1);
  lcd.print("UP Key to clear");
  while (activeButton == 0) {
    int button;
    
    readKey = analogRead(0);
    if (readKey < 790) {
      delay(100);
      readKey = analogRead(0);
    }
    button = evaluateButton(readKey);
    switch (button) {
      case 2:  // This case will execute if the "UP" button is pressed
        button = 0;
        Degree = 0;
        possition = 0;
        for (int x = 1; x < 16; x++) // Erases current cursor
        {     
          lcd.setCursor(x, 1);
          lcd.print(" ");
        }
        lcd.setCursor(1, 1);
        lcd.print("ALL Zero Done");
        break;
      case 4:  // This case will execute if the "back" button is pressed
        button = 0;
        lcd.noAutoscroll(); 
        activeButton = 1;
        break;
    }
  }
}

void display_value(int value)
{
  lcd.setCursor(0, 1);
  lcd.write(byte(0));
  lcd.setCursor(1, 1);
  for (int x = 1; x < 16; x++) // Erases current cursor
  {     
    lcd.setCursor(x, 1);
    lcd.print(" ");
  }
  lcd.setCursor(1, 1);
  lcd.print(value);
}


