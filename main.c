#include <stdio.h>

char* generateCommandToConvertToPGM(char* inputFile, char* outputFile);

int main() {
    printf("Enter the path of the image to convert: ");

    // 1. take input on image 
    // 2. generate command to convert to PGM
    // 3. execute command 
    // 4. Parse the PGM file to get the Heigh, Width and Scale.
    // 5. to generate a 2D normalized integers between 0.0 and 1.0 using Height, Width and Scale.
    // 6. define type countrouring_cell_s. 
    // 7. generate a "Controuring grid" (a 2D array of dimensions Height-1 and Width-1), where each item is a contouring_cell_s,
    // 9. define a countrour line look up table
    // 10. funtion that iterates through the countrouring_grid to generate an svg based on controur lines.
    // 11. save the svg to a file.

    char* pathImageToConvert;

    // 1. take input on image
    scanf("%s", pathImageToConvert);

    // 2. generate command to convert to PGM

}

char* generateCommand(char* inputFile, char* outputFile){

}
