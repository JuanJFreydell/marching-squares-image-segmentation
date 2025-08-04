// 1. take input on image filename
// 2. generate command to convert to PGM - generatePMGConversionCommand
// 3. execute command 
// 4. Parse the PGM file to get the Height, Width and Scale.
// 5. generate a 2D normalized grid of integers between 0.0 and 1.0 reading the PGM file and using Height, Width and Scale.
// 6. define type cell_s.
// 7. generate a "cellGrid" (a 2D array of dimensions Height-1 and Width-1), where each item is a cell_s,
// 8. declare a function called getContouringCase that returns an array of LineInstructions which is a pair of X,Y offsets for 2 points.
// 9. declare a function GenerateSingleLine that takes a struct LineInstructions and a cell, it calculates the 2 points and adds a line to the svg file.
// 10. the GeneratecellLines function calls the GetControuringCase function and gets back an array of SingleLineInstructions which is a pair of X,Y offsets for 2 points. For every item in the array we call GenerateSingleLine passing the LineInstructions, and the cell coordinates.
// 11. define a function called WriteSVG that takes in a contouring_grid and a finalFileName, it iterates through the contouring_grid and for each cell it calls the GeneratecellLines function.
// 12. rasterize a file with the svg layer on top of the png file. using magick's command: convert original_image.pgm contours.svg -layers composite output.png

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "marchingsquareslib.h" // our custom-written library for the marching squares structs and f().

int main() {
    // 1. take input on image filename
    char ImageToConvertFilename[512];
    getImageToConvert(ImageToConvertFilename);
    printf("Accessing file: %s\n", ImageToConvertFilename);

    // 2. generate command to convert to PGM and store the PGM output file name. Also captures the file stem name.
    char ConvertedPGMFileName[500] = "";
    char convertToPGMCommand[500] = "";
    char fileStem[500] = "";
    generatePGMConversionCommand(ImageToConvertFilename, ConvertedPGMFileName, sizeof(ConvertedPGMFileName), convertToPGMCommand, sizeof(convertToPGMCommand), fileStem, sizeof(fileStem));
    printf("Running command \"%s\" to convert input into PM2. \n", convertToPGMCommand);

    // 3. execute command 
    int commandResult = executePGMCommand(convertToPGMCommand);

    // 4. Parse the PGM file to get the Height, Width and Scale from its header.
    int height = 0;
    int width = 0;
    int scale = 0;
    parsePGMFile(ConvertedPGMFileName, &height, &width, &scale);
    printf("Height = %d, Width = %d, Scale = %d \n", height, width, scale);

    // 5. Parse the remainder of the PGM file to generate a 2D array of normalized pixel values (as floats) between 0.0 and 1.0, using Height, Width and Scale.    
    float** normalizedGrid = generateNormalizedGrid(ConvertedPGMFileName, height, width, scale); 

    // Captures the normalized grid as txt values and outputs a txt file of it.
    int writeNGridFile = writeNormalizedGridToTxt(fileStem, normalizedGrid, height, width);

    // Capture the grid with the threshold applied (visuatlization using integers)
    int writeThresholdNGridFile = writeThresholdGridToTxt(fileStem, normalizedGrid, height, width);

    // 6. define type cell_t. - defined above

    // 7. generate a "cellGrid" (a 2D array of dimensions Height-1 and Width-1), where each item is a cell_t.
    int contourGridHeight = height-1;
    int contourGridWidth = width-1;
    cell_t** cellGrid = malloc((contourGridWidth)* sizeof(cell_t*));
    for (int row = 0; row < height; row++){
        cellGrid[row] = malloc((contourGridHeight) * sizeof(cell_t));
    }

    // Using the normalized grid values, fill the values of the cell grid with each cell's calculated case value. The case value is a bit number calculated from the number of true/false corners.
    fillCellGrid(normalizedGrid, cellGrid, contourGridHeight, contourGridWidth);
    int writeCGridFile = writeContourGridToTxt(fileStem, cellGrid, contourGridHeight, contourGridWidth); // prints the binaryIndex values.
    
    // 8. declare a function called getContouringCase that takes a cell_t and returns an array of SingleLineInstructions. Each SingleLineInstructions is a pair of X,Y offsets for 2 points.
    // // Initialize the lookup table for the contouring cases.
    // Case **lookupTable = malloc(NUM_OF_CONTOUR_CASES * sizeof(Case*));
    // initializeLookupTable(lookupTable);
    // for(int i = 0; i < contourGridHeight; i++){
    //     for(int j = 0; j < contourGridWidth; j++){
    //         cell_t cell = cellGrid[i][j];
    //         Case *cellCasePtr = getContouringCase(cell);
    //         int numberOfLines = cellCasePtr->numOfLines;
    //         cellCasePtr->sliArray
    //         for(int k = 0; k = numberOfLines; k++){
    //             drawLine();
    //         }
    //     }
    // }

    // // 9. Declare a function GenerateSingleLine that takes a struct SingleLineInstructions and a cell, it calculates the 2 points and adds a line to the svg file.
    // void GenerateSingleLine(LineInstructions lineInstructions, cell_s cell);

    // // 10. the GeneratecellLines takes a cell, calls the GetControuringCase function, and gets back an array of SingleLineInstructions. For each set of SingleLineInstructions, it calls the GenerateSingleLine function.
    // void GeneratecellLines(cell_s cell);

    // // 11. define a function called WriteSVG that takes in a cellGrid and a finalFileName, it iterates through the contouring_grid and for each cell it calls the GeneratecellLines function.
    // void WriteSVG(cell_s cellGrid, char* finalFileName);

    // // 12. rasterize a file with the svg layer on top of the png file. using magick's command: convert original_image.pgm contours.svg -layers composite output.png
    // void rasterize();

    for (int i = 0; i < height; i++) free(normalizedGrid[i]);
    free(normalizedGrid);

    for (int i = 0; i < height - 1; i++) free(cellGrid[i]);
    free(cellGrid);

    fgets(ImageToConvertFilename, sizeof(ImageToConvertFilename), stdin);
    fgets(ImageToConvertFilename, sizeof(ImageToConvertFilename), stdin);

    return 1;
}
