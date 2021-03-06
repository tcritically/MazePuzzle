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


//turns all LEDs off
void clearLED( 
);


//calls write_max on each row in LEDArray to display it
void renderLED( 
    const bool LEDArray[49]
);


//checks if player is at victory position
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
//converts one row gol array to matrix displayable binary sequence
uint8_t GOLbinaryRow(
    const bool GOLArray[8][8], 
    int row
);

//calls write_bit to display a row on the matrix
void GOLwrite_max (
    uint8_t address, 
    uint8_t data
);

//calls write_max for each row to display array on the matrix
void renderGOL(
    const bool GOLArray[8][8] 
);

//sets display array based on larger world array and calls render
void drawGOL(
    const bool world[50][50],
    bool GOLArray[8][8],
    const unsigned int posx,
    const unsigned int posy
);

//advances state of GOL simulation
void stepGOL(
    bool world[50][50], 
    int width, 
    int height
);

//definitions

uint8_t GOLbinaryRow(const bool GOLArray[8][8], int row){

    int GOLRow = 0;

    //for each entry in the row of the array, bitshift it into the binary version of the row
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
    //skip the first two matrices
    for (int i=0;i<2; i++)
	{
		write_byte (0);
	    write_byte (0); 
	}
    //write data on third
    write_byte (address);
	write_byte (data); 
    //skip fourth matrix
    for (int i=0;i<1; i++)
	{
		write_byte (0);
	    write_byte (0); 
	}

	HAL_GPIO_WritePin (GPIOB, GPIO_PIN_5, 1);  // pull the CS pin HIGH
}

void renderGOL(const bool GOLArray[8][8]){
    
    //for each of the 8 rows, call write_max to display the row
    for(int i = 0; i < 8; ++i){
        write_max(i+1, GOLbinaryRow(GOLArray, i));

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

    //temp array representing new state
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
                    
    //don't count itself as a neighbour  
      if (world[x][y]){
          --count;
      }
      //if 3 or 2 neighbours
      if(count == 3 | count == 2){
          neworld[x][y] = 1;
      }
      else{
          neworld[x][y] = 0;
      }
      
 
    }
    } 
 
    //replace world array with new state
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
            //button up speeds up simulation
            if (!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_13)) {
                while(!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_13)){
                }
                delay/=2;

                }
            // BUTTON DOWN slows down
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

                    // button up moves display
                    if (!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_13)) {
                        while(!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_13)){
                        }
                        if(posy>0){
                            --posy;
                        }
                        drawGOL(world, GOLdisplay, posx, posy);

                    }
                    // BUTTON DOWN moves display
                    if (!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)) {
                        while(!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)){
                        }
                        if(posy<43){
                            ++posy;
                        }
                        drawGOL(world, GOLdisplay, posx, posy);

                    }
                    // BUTTON LEFT moves display
                    if (!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14)) {
                        while(!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14)){
                        }
                        if(posx>0){
                            --posx;
                        }
                        drawGOL(world, GOLdisplay, posx, posy);

                    }
                    // BUTTON RIGHT moves display
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





// This function is called by the HAL once every millisecond
void SysTick_Handler(void)
{
    HAL_IncTick(); // tell HAL that a new tick has happened
    // we can do other things in here too if we need to, but be careful
}
