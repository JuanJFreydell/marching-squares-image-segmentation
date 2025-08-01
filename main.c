#include <stdio.h>

char* generateCommandToConvertToPGM(char* inputFile, char* outputFile);

int main() {
    printf("Enter the path of the image to convert: ");

    // 1. take input on image 
    // 2. generate command to convert to PGM
    // 3. execute command 
    // 4. Parse the PGM file to get the Heigh, Width and Scale.
    // 5. to generate a 2D normalized integers between 0.0 and 1.0 using Height, Width and Scale.
    // 6. define type contouring_cell_s. 
    // 7. generate a "Contouring grid" (a 2D array of dimensions Height-1 and Width-1), where each item is a contouring_cell_s,
    // 9. define a bitwise OR function that takes in 4 vertix values and returns a contour case
    // 10. define a function "GetControuringCase"that takes 4 vertices of a voxel and returns a contour case.
    // 11. Define a function GenerateSingleLine that takes a struct LineInstructions and a voxel, it calculates the 2 points and adds a line to the svg file.
    // 12  Define a function called getContouringCase that returns an array of LineInstructions which is a pair of X,Y offsets for 2 points.
    // 13. The GenerateVoxelLines function calls the GetControuringCase function and gets back an array of SingleLineInstructions which is a pair of X,Y offsets for 2 points. For every item in the array we call GenerateSingleLine passing the LineInstructions, and the voxel coordinates.
    // 14. Define a function called WriteSVG that takes in a contouring_grid and a finalFileName, it iterates through the contouring_grid and for each voxel it calls the GenerateVoxelLines function.
    // 15 rasterize a file with the svg layer on top of the png file. using magick's command: convert original_image.pgm contours.svg -layers composite output.png

    char* pathImageToConvert;

    // 1. take input on image
    scanf("%s", pathImageToConvert);

    // 2. generate command to convert to PGM

}

char* generateCommand(char* inputFile, char* outputFile){

}
