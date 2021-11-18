#include <iostream>
void setLEDArray(
    const bool mazeArray[100],
    bool LEDArray[49],
    const unsigned int posx,
    const unsigned int posy
);
void setLEDArray(
    const bool mazeArray[100],
    bool LEDArray[49],
    const unsigned int posx,
    const unsigned int posy
) {
    
    for(unsigned int row = 0; row < 7; row++){
        if (posx - 3 + row < 0){
            for (unsigned int i = 0; i < 7; i++){
                LEDArray[row*7+i] = 1;
            }
        }    
        else {
        //replace 9 with whatever the max column of mazeArray is
        //checks if the entire row is out of bounds and should be 1
        if (posx - 3 + row < 0 || posx - 3 + row > 9){
            for(unsigned int i = 0; i < 7; i++){
                LEDArray[row*7+i] = 1;
            }
        }
        else{
            for(unsigned int column = 0; column < 7; column++){
                if (posy - 3 + column < 0 || posy - 3 + column > 9){
                    LEDArray[row*7 + column] = 1;
                }
                else{
                    LEDArray[row*7 + column] = mazeArray[(posx - 3 + row)*10 + posy - 3 + column];
                }
            }
        }    
    }
}
}
int main(void)
{
    // MAZE CODE //
    ///////////////

    // C does not like variable sizes for arrays
    // large array (10x10), constant (sample values in)
    const bool mazeArray[100] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                 0, 1, 1, 0, 0, 0, 0, 0, 0, 0,
                                 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
                                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                 1, 1, 1, 1, 1, 1, 1, 0, 0, 0,
                                 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
                                 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
    };

    // smaller array (7x7)
    bool LEDArray[49]  = {};

    //current player position
    // note: since some of the functions use *, make sure to put variables
    // in functions with & in front
    unsigned int posx = 0;
    unsigned int posy = 0;

    //winning position, constants
    const unsigned int winx = 0;
    const unsigned int winy = 0;

    //array of pins, constant
    const unsigned int pinArray[49] = {};

    //current column to render
    unsigned int column = 6;

    // test 
    setLEDArray(mazeArray, LEDArray, 0, 0);
    for (int i = 0; i<7; ++i) {
        for (int j = 0; j < 7; ++j) {
            if(LEDArray[i * 7 + j]){
                std::cout <<1;
            }
            else{
                std::cout <<0;
            }
        }
        std::cout << std::endl;
    }
}