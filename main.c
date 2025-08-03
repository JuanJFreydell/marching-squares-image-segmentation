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


int main() {

    // 1. take input on image filename
    printf("Enter the filename of the image to convert: ");
    char ImageToConvertFilename[512];
    scanf("%s", ImageToConvertFilename);
 
    // 2. generate command to convert to PGM
    char PGMFilename[1024];
    snprintf(PGMFilename, sizeof(PGMFilename), "%s.pgm", ImageToConvertFilename);

    long originalFilenameLength;
    char ConvertedPGMFileName[500];
    char* PGMExtension = ".pgm";
    char* magickCommand = "magick";
    char* ExtensionStart = ".";
    char* originalFileExtension;
    int ExtensionLength;
    int OriginalStemLength;
    char convertToPGMCommand[500] = "";

    originalFilenameLength = strlen(ImageToConvertFilename);
    originalFileExtension = strstr(ImageToConvertFilename, ExtensionStart);
    ExtensionLength = strlen(originalFileExtension);
    OriginalStemLength = originalFilenameLength - ExtensionLength;

    // instantiate the ConvertedPGMFileName to the stem file.
    strncpy(ConvertedPGMFileName, ImageToConvertFilename, OriginalStemLength);
    ConvertedPGMFileName[OriginalStemLength] = '\0';

    // concatenate the .pgm extension to the ConvertedPGMFileName.
    snprintf(ConvertedPGMFileName, sizeof(ConvertedPGMFileName), "%s%s", ConvertedPGMFileName, PGMExtension);

    snprintf(convertToPGMCommand,sizeof(convertToPGMCommand), "%s %s %s", magickCommand, ImageToConvertFilename, ConvertedPGMFileName);

    printf("%s", convertToPGMCommand);

    fgets(ImageToConvertFilename, sizeof(ImageToConvertFilename), stdin);
    fgets(ImageToConvertFilename, sizeof(ImageToConvertFilename), stdin);

    // printf();
    // char* command = generatePMGConversionCommand(ImageToConvertFilename, PGMFilename); // this can be done inline;
    
    // // 3. execute command 
    // system(command); // add a check to ensure that the command is executed correctly.

    // // 4. Parse the PGM file to get the Height, Width and Scale.
    // getPGMHeader(PGMFilename, &height, &width, &scale);

    // // 5. to generate a 2D normalized integers between 0.0 and 1.0 reading the PGM file and using Height, Width and Scale.    
    // float** normalizedGrid = generateNormalizedGrid(PGMFilename, height, width, scale);

    // // 6. define type cell_s. 
    // typedef struct {
    //     int x;
    //     int y;
    //     int top_right;
    //     int top_left;
    //     int bottom_right;
    //     int bottom_left;
    // } cell_s;

    // // 7. generate a "cellGrid" (a 2D array of dimensions Height-1 and Width-1), where each item is a cell_s,
    // cell_s cellGrid[height-1][width-1];

    // // 8. declare a function called getContouringCase that takes a cell_s and returns an array of SingleLineInstructions. Each SingleLineInstructions is a pair of X,Y offsets for 2 points.
    // SingleLineInstructions* getContouringCase(cell_s cell);

    // // 9. Declare a function GenerateSingleLine that takes a struct SingleLineInstructions and a cell, it calculates the 2 points and adds a line to the svg file.
    // void GenerateSingleLine(LineInstructions lineInstructions, cell_s cell);

    // // 10. the GeneratecellLines takes a cell, calls the GetControuringCase function, and gets back an array of SingleLineInstructions. For each set of SingleLineInstructions, it calls the GenerateSingleLine function.
    // void GeneratecellLines(cell_s cell);

    // // 11. define a function called WriteSVG that takes in a cellGrid and a finalFileName, it iterates through the contouring_grid and for each cell it calls the GeneratecellLines function.
    // void WriteSVG(cell_s cellGrid, char* finalFileName);

    // // 12. rasterize a file with the svg layer on top of the png file. using magick's command: convert original_image.pgm contours.svg -layers composite output.png
    // void rasterize();
    return 1;
}