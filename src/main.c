// Sample code for ECE 198

// Written by Bernie Roehl, August 2021

// This file contains code for a number of different examples.
// Each one is surrounded by an #ifdef ... #endif block inside of main().

// To run a particular example, you should remove the comment (//) in
// front of exactly ONE of the following lines:

#define BUTTON_BLINK
// #define LIGHT_SCHEDULER
// #define TIME_RAND
// #define KEYPAD
// #define KEYPAD_CONTROL
// #define SEVEN_SEGMENT
// #define KEYPAD_SEVEN_SEGMENT
// #define COLOR_LED
// #define ROTARY_ENCODER
// #define ANALOG
// #define PWM

#include <stdbool.h> // booleans, i.e. true and false
#include <stdio.h>   // sprintf() function
#include <stdlib.h>  // srand() and random() functions

#include "ece198.h"



//code for controlling LED matrix adapted from library on controllerstech.com

//declarations
void write_byte(uint8_t byte);

void write_max(uint8_t address, uint8_t data);

void max_init(void);


//definitions
// sends one byte
void write_byte(uint8_t byte)
{
	    for(int i=8;i>0;--i)
          {
	    	 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, 0);  // Pull the CLK LOW
	    	 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, byte&(0x01<<(i-1)));// Write one BIT data MSB first
             HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, 1);  // Pull the CLK HIGH
           }
}

// initializes one row for editing
void write_max (uint8_t address, uint8_t data)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 0);   // Pull the CS pin LOW
    for (int i=0;i<3; i++)
	{
		write_byte (0);
	    write_byte (0); 
	}
    write_byte (address);
	write_byte (data); 

	HAL_GPIO_WritePin (GPIOB, GPIO_PIN_5, 1);  // pull the CS pin HIGH
}

// initializing default values
void max_init(void)
{
    write_max(0x09, 0x00);       //  no decoding
    write_max(0x0a, 0x03);       //  brightness intensity
    write_max(0x0b, 0x07);       //  scan limit = 8 LEDs
    write_max(0x0c, 0x01);       //  power down =0，normal mode = 1
    write_max(0x0f, 0x00);       //  no test display
}


// original code for mazeGame
// declarations, explanations come in definitions
int binaryRow(const bool LEDArray[49], int row);

void setLEDArray(
    const bool mazeArray[361],
    bool LEDArray[49],
    const unsigned int posx,
    const unsigned int posy
);

void clearLED( 
);

void renderLED( 
    const bool LEDArray[49]
);

void atVictoryPosition(
    const unsigned int posx, 
    const unsigned int poxy, 
    const unsigned int winx, 
    const unsigned int winy,
    bool LEDArray[49],
    bool *mazeWon
);

///*bool legalMove(bool LEDArray[49], char moveType[]);
//whenever we use a move method we need to call setLEDArray right after exiting the move method
void moveUp(
    unsigned int *posy, 
    const bool LEDArray[49]
);

void moveDown(
    unsigned int *posy, 
    const bool LEDArray[49]
);

void moveLeft(
    unsigned int *posx, 
    const bool LEDArray[49]
);

void moveRight(
    unsigned int *posx, 
    const bool LEDArray[49]
);

//function definitions
// display current state of led maze
void setLEDArray(
    const bool mazeArray[361],
    bool LEDArray[49],
    const unsigned int posx,
    const unsigned int posy
) {
    
    for(unsigned int row = 0; row < 7; row++){
        //replace 9 with whatever the max column of mazeArray is
        //checks if the entire row is out of bounds and should be 1
            if (posx - 3 + row < 0 || posx - 3 + row > 18){
                for(unsigned int i = 0; i < 7; i++){
                    LEDArray[row*7+i] = 1;
                }
            }
            else{
                for(unsigned int column = 0; column < 7; column++){
                    if (posy - 3 + column < 0 || posy - 3 + column > 18){
                        LEDArray[row*7 + column] = 1;
                    }
                    else{
                        LEDArray[row*7 + column] = mazeArray[(posx - 3 + row)*19 + posy - 3 + column];
                    }
                }
            }
    }
    LEDArray[24] = 1;
}

// function that takes array and returns binary sequence to be rendered on matrix
// will represent maze
int binaryRow(const bool LEDArray[49], int row){

    if (row == 7){
        return 0;
    }
    int LEDRow = 0;

    for(int i = 0; i <7; ++i){

        if(LEDArray[row*7 + i] == 1){
            LEDRow += ( 1 << (7-i));
        }

    }

    return(LEDRow);

}

// resetting maze
void clearLED() {
    //render code
    for(int i = 1; i < 9; ++i){

        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 0);   // Pull the CS pin LOW
        for (int j=0;j<3; j++)
	    {
		    write_byte (i);
	        write_byte (0); 
	    }

	HAL_GPIO_WritePin (GPIOB, GPIO_PIN_5, 1);

    }

}

// renders each row 
void renderLED(
    const bool LEDArray[49]
) {
    //render code
    for(int i = 0; i < 8; ++i){

        write_max(i+1, binaryRow(LEDArray, i));

    }

}

// function for checking if at victory position
void atVictoryPosition(
    const unsigned int posx, 
    const unsigned int posy, 
    const unsigned int winx, 
    const unsigned int winy,
    bool LEDArray[49],
    bool *mazeWon
) {
    //if at win location, set all LEDs to 1
    if (posx == winx && posy == winy) {
        for(int i = 0; i < 49; ++i){
            LEDArray[i] = 1;
        }
        *mazeWon = 1;
    }
}

// strings are weird, we could use ints for move types instead?
// function checks if move made is possible
//deprecated, will use 4 different functions, one for each direction that checks legal AND makes move
/*
bool legalMove(bool LEDArray[49], char moveType[]) {
    bool return_value = true;
    // note: player is at index 4
    if (moveType == "RIGHT") {
        return_value = (LEDArray[5] == 0);
    } else if (moveType == "LEFT") {
        return_value = (LEDArray[3] == 0);
    } else if (moveType == "UP") {
        return_value = (LEDArray[1] == 0);
    } else {
        //moveTYPE == down
        return_value = (LEDArray[7] == 0);
    }
    return return_value;
}*/

//player is at index 24
//row 0 is the lowest row
//column 0 is the leftmost

// check if legal move, then update position
void moveUp(
    unsigned int *posy, 
    const bool LEDArray[49]
){
    if(LEDArray[23] == 0){
        --*posy;
    }
}

void moveDown(
    unsigned int *posy, 
    const bool LEDArray[49]
){
    if(LEDArray[25] == 0){
        ++*posy;
    }
}

void moveLeft(
    unsigned int *posx, 
    const bool LEDArray[49]
){
    if(LEDArray[17] == 0){
        --*posx;
    }
}

void moveRight(
    unsigned int *posx, 
    const bool LEDArray[49]
){
    if(LEDArray[31] == 0){
        ++*posx;
    }
}

//Entirely original code for GOL
//declarations
uint8_t GOLbinaryRow(
    const bool GOLArray[8][8], 
    int row
);

void GOLwrite_max (
    uint8_t address, 
    uint8_t data
);

void renderGOL(
    const bool GOLArray[8][8] 
);

void drawGOL(
    const bool world[50][50],
    bool GOLArray[8][8],
    const unsigned int posx,
    const unsigned int posy
);

void stepGOL(
    bool world[50][50], 
    int width, 
    int height
);

//definitions

uint8_t GOLbinaryRow(const bool GOLArray[8][8], int row){

    int GOLRow = 0;

    for(int i = 0; i < 8; ++i){

        if(GOLArray[i][row] == 1){
            GOLRow += ( 1 << (7-i));
        }
        


    }

    return(GOLRow);

}

void GOLwrite_max (uint8_t address, uint8_t data)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 0);   // Pull the CS pin LOW
    for (int i=0;i<2; i++)
	{
		write_byte (0);
	    write_byte (0); 
	}
    write_byte (address);
	write_byte (data); 
    for (int i=0;i<1; i++)
	{
		write_byte (0);
	    write_byte (0); 
	}

	HAL_GPIO_WritePin (GPIOB, GPIO_PIN_5, 1);  // pull the CS pin HIGH
}

void renderGOL(const bool GOLArray[8][8]){
    
    for(int i = 0; i < 8; ++i){
        GOLwrite_max(i+1, GOLbinaryRow(GOLArray, i));

    }
    


}

void drawGOL(const bool world[50][50], bool GOLArray[8][8], const unsigned int posx, const unsigned int posy){

    for(unsigned int row = 0; row < 8; row++){
        //replace 9 with whatever the max column of mazeArray is
        //checks if the entire row is out of bounds and should be 1
            if (posx + row < 0 || posx + row > 49){
                for(unsigned int i = 0; i < 8; i++){
                    GOLArray[i][row] = 1;
                }
            }
            else{
                for(unsigned int column = 0; column < 8; column++){
                    if (posy + column < 0 || posy + column > 49){
                        GOLArray[column][row] = 1;
                    }
                    else{
                        GOLArray[column][row] = world[posx + row][posy + column];
                    }
                }
            }
    }
    renderGOL(GOLArray);
}

void stepGOL(bool world[50][50], int width, int height){

    bool neworld[50][50];
    for (int x = 0; x < width; x++){
        for (int y = 0; y < height; y++){
            int count = 0;
            for (int x1 = x - 1; x1 <= x + 1; ++x1){
                for (int y1 = y - 1; y1 <= y + 1; ++y1){
                    if (world[(x1 + width) % width][(y1 + height) % height]){
                        ++count;
                    }
                }
            }
                    
                        
      if (world[x][y]){
          --count;
      }
      if(count == 3 | count == 2){
          neworld[x][y] = 1;
      }
      else{
          neworld[x][y] = 0;
      }
      
 
    }
    } 
 
    for (int x = 0; x < width; ++x){
        for (int y = 0; y < height; ++y){
            world[x][y] = neworld[x][y];
        }
    }

}


int main(void)
{
    // MAZE CODE //
    ///////////////

    // C does not like variable sizes for arrays
    // large array (10x10), constant (sample values in)
    const bool mazeArray[361] = {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                                 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                                 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1,
                                 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1,
                                 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1,
                                 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1,
                                 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1,
                                 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
                                 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1,
                                 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1,
                                 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
                                 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1,
                                 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1,
                                 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 
                                 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1,
                                 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0,
                                 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1

    };

    //to track whether maze is won
    bool mazeWon = 0;
    //empty array for clearing out matrix at start
    bool blank49[49] = {};
    // smaller array (7x7)
    bool LEDArray[49]  = {};

    //current player position
    // note: since some of the functions use *, make sure to put variables
    // in functions with & in front
    unsigned int posx = 0;
    unsigned int posy = 1;

    //winning position, constants
    const unsigned int winx = 17;
    const unsigned int winy = 18;

    // while loop later on which is main game loop

    HAL_Init(); // initialize the Hardware Abstraction Layer

    // Peripherals (including GPIOs) are disabled by default to save power, so we
    // use the Reset and Clock Control registers to enable the GPIO peripherals that we're using.

    __HAL_RCC_GPIOA_CLK_ENABLE(); // enable port A (for the on-board LED, for example)
    __HAL_RCC_GPIOB_CLK_ENABLE(); // enable port B (for the rotary encoder inputs, for example)
    __HAL_RCC_GPIOC_CLK_ENABLE(); // enable port C (for the on-board blue pushbutton, for example)

    // initialize the pins to be input, output, alternate function, etc...

    InitializePin(GPIOB, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_3, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_PIN_RESET);  // on-board LED
    InitializePin(GPIOB, GPIO_PIN_1 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, GPIO_MODE_INPUT, GPIO_PULLUP, 0);
    

    // note: the on-board pushbutton is fine with the default values (no internal pull-up resistor
    // is required, since there's one on the board)

    // set up for serial communication to the host computer
    // (anything we write to the serial port will appear in the terminal (i.e. serial monitor) in VSCode)
    SerialSetup(9600);
    setLEDArray(mazeArray, LEDArray, posx, posy);
    max_init();
    clearLED();
    renderLED(blank49);
    renderLED(LEDArray);
    // as mentioned above, only one of the following code sections will be used
    // (depending on which of the #define statements at the top of this file has been uncommented)
    while (!mazeWon) {
        // render the maze
        // PIN VALUES ARE ARBITARY
        // buttons
        // BUTTON UP
        if (!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_13)) {
            // placeholder pin values
            while(!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_13)){
            }
            moveUp(&posy, LEDArray);
            setLEDArray(mazeArray, LEDArray, posx, posy);
            atVictoryPosition(posx, posy, winx, winy, LEDArray, &mazeWon);
            renderLED(LEDArray);

        }
        // BUTTON DOWN
        if (!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)) {
            // placeholder pin values
            while(!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)){
            }
            moveDown(&posy, LEDArray);
            setLEDArray(mazeArray, LEDArray, posx, posy);
            atVictoryPosition(posx, posy, winx, winy, LEDArray, &mazeWon);
            renderLED(LEDArray);

        }
        // BUTTON LEFT
        if (!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14)) {
            // placeholder pin values
            while(!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14)){
            }
            moveLeft(&posx, LEDArray);
            setLEDArray(mazeArray, LEDArray, posx, posy);
            atVictoryPosition(posx, posy, winx, winy, LEDArray, &mazeWon);
            renderLED(LEDArray);

        }
        // BUTTON RIGHT
        if (!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_15)) {
            // placeholder pin values
            while(!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_15)){
            }
            moveRight(&posx, LEDArray);
            setLEDArray(mazeArray, LEDArray, posx, posy);
            atVictoryPosition(posx, posy, winx, winy, LEDArray, &mazeWon);
            renderLED(LEDArray);

        }


    }

    // GOL CODE //
    ///////////////
    srand(HAL_GetTick());
    bool world[50][50];
    bool GOLdisplay[8][8];
    posx = 10;
    posy = 10;
    int delay = 1000;
    while(1){
        for (int x = 0; x < 50; x++){
            for (int y = 0; y < 50; y++){
                if (rand() % 100 > 90){
                    world[x][y] = 1;
                }
                else{
                    world[x][y] = 0;
                }
            }   
        }
        while(1){

            drawGOL(world, GOLdisplay, posx, posy);
            stepGOL(world, 50, 50);
            //button up
            if (!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_13)) {
                while(!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_13)){
                }
                delay/=2;

                }
            // BUTTON DOWN
            if (!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)) {
                while(!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)){    
                }
                delay*=2;

            }
            HAL_Delay(delay);
            if(!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)){
                while(!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)){
                }
                while(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)){

                    // button up
                    if (!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_13)) {
                        while(!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_13)){
                        }
                        if(posy>0){
                            --posy;
                        }
                        drawGOL(world, GOLdisplay, posx, posy);

                    }
                    // BUTTON DOWN
                    if (!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)) {
                        while(!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)){
                        }
                        if(posy<43){
                            ++posy;
                        }
                        drawGOL(world, GOLdisplay, posx, posy);

                    }
                    // BUTTON LEFT
                    if (!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14)) {
                        while(!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14)){
                        }
                        if(posx>0){
                            --posx;
                        }
                        drawGOL(world, GOLdisplay, posx, posy);

                    }
                    // BUTTON RIGHT
                    if (!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_15)) {
                        while(!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_15)){
                        }
                        if(posx<43){
                            ++posx;
                        }
                        drawGOL(world, GOLdisplay, posx, posy);

                    }
                    if(!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)){
                        break;
                    }
                }
            }

        }
    }

}



#ifdef LIGHT_SCHEDULER
    // Turn on the LED five seconds after reset, and turn it off again five seconds later.

    while (true) {
        uint32_t now = HAL_GetTick();
        if (now > 5000 && now < 10000)
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, true);   // turn on LED
        else
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, false);  // turn off LED
    }
#endif

#ifdef TIME_RAND
    // This illustrates the use of HAL_GetTick() to get the current time,
    // plus the use of random() for random number generation.
    
    // Note that you must have "#include <stdlib.h>"" at the top of your main.c
    // in order to use the srand() and random() functions.

    // while (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13));  // wait for button press
    // srand(HAL_GetTick());  // set the random seed to be the time in ms that it took to press the button
    // if the line above is commented out, your program will get the same sequence of random numbers
    // every time you run it (which may be useful in some cases)

    while (true) // loop forever
    {
        while (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13));  // wait for button press

        // Display the time in milliseconds along with a random number.
        // We use the sprintf() function to put the formatted output into a buffer;
        // see https://www.tutorialspoint.com/c_standard_library/c_function_sprintf.htm for more
        // information about this function
        char buff[100];
        sprintf(buff, "Time: %lu ms   Random = %ld\r\n", HAL_GetTick(), random());
        // lu == "long unsigned", ld = "long decimal", where "long" is 32 bit and "decimal" implies signed
        SerialPuts(buff); // transmit the buffer to the host computer's serial monitor in VSCode/PlatformIO

        while (!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13));  // wait for button to be released
    }
#endif

#ifdef KEYPAD
    // Read buttons on the keypad and display them on the console.

    // this string contains the symbols on the external keypad
    // (they may be different for different keypads)
    char *keypad_symbols = "123A456B789C*0#D";
    // note that they're numbered from left to right and top to bottom, like reading words on a page

    InitializeKeypad();
    while (true)
    {
        while (ReadKeypad() < 0);   // wait for a valid key
        SerialPutc(keypad_symbols[ReadKeypad()]);  // look up its ASCII symbol and send it to the hsot
        while (ReadKeypad() >= 0);  // wait until key is released
    }
#endif

#ifdef KEYPAD_CONTROL
    // Use top-right button on 4x4 keypad (typically 'A') to toggle LED.

    InitializeKeypad();
    while (true)
    {
        while (ReadKeypad() < 0);   // wait for a valid key
        int key = ReadKeypad();
        if (key == 3)  // top-right key in a 4x4 keypad, usually 'A'
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);   // toggle LED on or off
         while (ReadKeypad() >= 0);  // wait until key is released
    }
#endif

#ifdef SEVEN_SEGMENT
    // Display the numbers 0 to 9 inclusive on the 7-segment display, pausing for a second between each one.
    // (remember that the GND connection on the display must go through a 220 ohm current-limiting resistor!)
    
    Initialize7Segment();
    while (true)
        for (int i = 0; i < 10; ++i)
        {
            Display7Segment(i);
            HAL_Delay(1000);  // 1000 milliseconds == 1 second
        }
#endif

#ifdef KEYPAD_SEVEN_SEGMENT
    // Combines the previous two examples, displaying numbers from the keypad on the 7-segment display.

    // this string contains the symbols on the external keypad
    // (they may be different for different keypads)
    char *keypad_symbols = "123A456B789C*0#D";
    // note that they're numbered from left to right and top to bottom, like reading words on a page

    InitializeKeypad();
    Initialize7Segment();
    while (true)
    {
        int key = ReadKeypad();
        if (key >= 0)
            Display7Segment(keypad_symbols[key]-'0');  // tricky code to convert ASCII digit to a number
    }
#endif

#ifdef COLOR_LED
    // Cycle through all 8 possible colors (including off and white) as the on-board button is pressed.
    // This example assumes that the color LED is connected to pins D11, D12 and D13.

    // Remember that each of those three pins must go through a 220 ohm current-limiting resistor!
    // Also remember that the longest pin on the LED should be hooked up to GND.

    InitializePin(GPIOA, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 0);  // initialize color LED output pins
    while (true) {
        for (int color = 0; color < 8; ++color) {
            // bottom three bits indicate which of the three LEDs should be on (eight possible combinations)
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, color & 0x01);  // blue  (hex 1 == 0001 binary)
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, color & 0x02);  // green (hex 2 == 0010 binary)
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, color & 0x04);  // red   (hex 4 == 0100 binary)

            while (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13));   // wait for button press 
            while (!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13));  // wait for button release
        }
    }
#endif

#ifdef ROTARY_ENCODER
    // Read values from the rotary encoder and update a count, which is displayed in the console.

    InitializePin(GPIOB, GPIO_PIN_5, GPIO_MODE_INPUT, GPIO_PULLUP, 0);   // initialize CLK pin
    InitializePin(GPIOB, GPIO_PIN_4, GPIO_MODE_INPUT, GPIO_PULLUP, 0);   // initialize DT pin
    InitializePin(GPIOB, GPIO_PIN_10, GPIO_MODE_INPUT, GPIO_PULLUP, 0);  // initialize SW pin
    
    bool previousClk = false;  // needed by ReadEncoder() to store the previous state of the CLK pin
    int count = 0;             // this gets incremented or decremented as we rotate the encoder

    while (true)
    {
        int delta = ReadEncoder(GPIOB, GPIO_PIN_5, GPIOB, GPIO_PIN_4, &previousClk);  // update the count by -1, 0 or +1
        if (delta != 0) {
            count += delta;
            char buff[100];
            sprintf(buff, "%d  \r", count);
            SerialPuts(buff);
        }
        bool sw = !HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10);  // read the push-switch on the encoder (active low, so we invert it using !)
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, sw);  // turn on LED when encoder switch is pushed in
    }
#endif

#ifdef ANALOG
    // Use the ADC (Analog to Digital Converter) to read voltage values from two pins.

    __HAL_RCC_ADC1_CLK_ENABLE();        // enable ADC 1
    ADC_HandleTypeDef adcInstance;      // this variable stores an instance of the ADC
    InitializeADC(&adcInstance, ADC1);  // initialize the ADC instance
    // Enables the input pins
    // (on this board, pin A0 is connected to channel 0 of ADC1, and A1 is connected to channel 1 of ADC1)
    InitializePin(GPIOA, GPIO_PIN_0 | GPIO_PIN_1, GPIO_MODE_ANALOG, GPIO_NOPULL, 0);   
    while (true)
    {
        // read the ADC values (0 -> 0V, 2^12 -> 3.3V)
        uint16_t raw0 = ReadADC(&adcInstance, ADC_CHANNEL_0);
        uint16_t raw1 = ReadADC(&adcInstance, ADC_CHANNEL_1);

        // print the ADC values
        char buff[100];
        sprintf(buff, "Channel0: %hu, Channel1: %hu\r\n", raw0, raw1);  // hu == "unsigned short" (16 bit)
        SerialPuts(buff);
    }
#endif



// This function is called by the HAL once every millisecond
void SysTick_Handler(void)
{
    HAL_IncTick(); // tell HAL that a new tick has happened
    // we can do other things in here too if we need to, but be careful
}
