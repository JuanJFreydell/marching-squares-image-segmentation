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

// Global constants
#define THRESHOLD 0.5
const char *PGMExtension = ".pgm";
const char* expectedFileFormat = "PM2";

/* A contouring cell which represents the value based on its 4 corners. */
typedef struct {
    int x;
    int y;
    // Values listed from most significant to least significant bit (clockwise from top left)
    int top_left;
    int top_right;
    int bottom_right;
    int bottom_left;
    // Binary index
    int caseValue;
} cell_t;

/* Gets input from the user, then returns a char pointer to the filename of the image to convert. */
void getImageToConvert(char *fileName){
    printf("Enter the filename of the image to convert: ");
    scanf("%s", fileName);
}

/* A debugging helper function to capture the normalized grid as a text file. Formatted to 3 decimal places. */
int writeNormalizedGridToTxt(char *fileStem, float** normalizedGrid, int height, int width){
    // Create the file name with the PGM filename
    char normalizedGridFileName[500]= "";
    strcat(normalizedGridFileName, fileStem);
    strcat(normalizedGridFileName, "-NormalizedGrid.txt");
    
    // Open the file and write to it.
    FILE *file = fopen(normalizedGridFileName, "w"); 
    if (file == NULL) {
        printf("Error opening file");
        return 1;
    }

    for (int i = 0; i < height; i++){
        fprintf(file, "[");
        for (int j = 0; j < width; j++) {
            fprintf(file, " %.3f ", normalizedGrid[i][j]);
        }
        fprintf(file, "]\n");
    }
    fclose(file);
    return 0;
}

/* A debugging helper function to capture the normalized grid with a threshold applied so that 0 or 1 are now the possible values. Saved as a text file. */
int writeThresholdGridToTxt(char *fileStem, float** normalizedGrid, int height, int width){
    // Create the file name with the PGM filename
    char normalizedGridFileName[500]= "";
    strcat(normalizedGridFileName, fileStem);
    strcat(normalizedGridFileName, "-NormalizedGridwithThreshold.txt");
    
    // Open the file and write to it.
    FILE *file = fopen(normalizedGridFileName, "w"); 
    if (file == NULL) {
        printf("Error opening file");
        return 1;
    }

    for (int i = 0; i < height; i++){
        fprintf(file, "[");
        for (int j = 0; j < width; j++) {
            fprintf(file, " %d ", (normalizedGrid[i][j] > THRESHOLD? 1 : 0));
        }
        fprintf(file, "]\n");
    }
    fclose(file);
    return 0;
}


/* A debugging helper function to capture the contour cell grid as a text file. */
int writeContourGridToTxt(char *fileStem, cell_t** contourGrid, int height, int width){
    // Create the file name with the PGM filename
    char contourGridFileName[500]= "";
    strcat(contourGridFileName, fileStem);
    strcat(contourGridFileName, "-ContourCellGrid.txt");
    
    // Open the file and write to it.
    FILE *file = fopen(contourGridFileName, "w"); 
    if (file == NULL) {
        printf("Error opening file");
        return 1;
    }

    for (int i = 0; i < height; i++){
        fprintf(file, "[");
        for (int j = 0; j < width; j++) {
            cell_t temp = contourGrid[j][i];
            int tempCaseVal = temp.caseValue;
            fprintf(file, " %d ", tempCaseVal);
            // Print another space if the caseValue is a single digit, to keep formatting consistent.
            if(tempCaseVal < 10){
                fprintf(file, " ");
            }
        }
        fprintf(file, "]\n");
    }
    fclose(file);
    return 0;
}

/* Generates the Magick program command for generating a PGM file. Also generates the expected PGM output file name. */
void generatePGMConversionCommand(const char *ImageToConvertFilename, char *ConvertedPGMFileName, size_t convertedSize, char *pgmCommand, size_t commandSize, char *fileStem, size_t fileStemSize){
    // Find the file name up to its extension.
    long originalFilenameLength = strlen(ImageToConvertFilename); 
    char* ExtensionStart = ".";
    char* originalFileExtension = strstr(ImageToConvertFilename, ExtensionStart); // locates the first occurence of extensionStart
    int ExtensionLength = strlen(originalFileExtension);
    int OriginalStemLength = originalFilenameLength - ExtensionLength;

    // instantiate the ConvertedPGMFileName to the stem file.
    strncpy(ConvertedPGMFileName, ImageToConvertFilename, OriginalStemLength); // copy the file name up to "."
    ConvertedPGMFileName[OriginalStemLength] = '\0';
    strncpy(fileStem, ConvertedPGMFileName, OriginalStemLength + 1);

    // concatenate the .pgm extension to the ConvertedPGMFileName.
    strncat(ConvertedPGMFileName, PGMExtension, convertedSize - strlen(ConvertedPGMFileName) - 1);

    //magick input.jpg -compress none -define pgm:format=plain output.pgm
    snprintf(pgmCommand, commandSize, "magick %s -resize 256x256\\! -compress none -define pgm:format=plain %s", ImageToConvertFilename, ConvertedPGMFileName);
}

/* Executes the PGM command. */
int executePGMCommand(char *convertToPGMCommand){
    int success = system(convertToPGMCommand);
    if (success != 0) {
        fprintf(stderr, "Error running conversion command.\n");
        return 1;
    }
    return success;
}

/* Parses the PGM file to get the Height, Width and Scale. */
void parsePGMFile(char *ConvertedPGMFileName, int *height, int *width, int *scale){
    FILE *file = fopen(ConvertedPGMFileName, "r");
    if (file == NULL){
        printf("Unable to open converted file %s.", ConvertedPGMFileName);
        return;
    }
    
    char heightStr[10] = "";
    char widthStr[10] = "";
    char scaleStr[10] = "";

    char line[256];
    char* height_temp;
    char* width_temp;
    for (int i = 0; i < 3; i++){
        fgets(line, sizeof(line), file);
        // For the header, check that the file format is P2.
        if (i == 0){
            line[strcspn(line, "\n")] = '\0'; // Replace any newline char with a terminal char.
            if(!strcmp(expectedFileFormat, line)){
                printf("error, file is not in P2 format");
            }
        }
        // For the second row, the number of columns is listed, then the number of rows.
        if (i == 1){
            height_temp = strtok(line, " ");
            width_temp = strtok(NULL,"\n");
            if(height_temp && width_temp){
                strcpy(heightStr, height_temp);
                heightStr[sizeof(heightStr)-1] = '\0';
                strcpy(widthStr, width_temp);
                widthStr[sizeof(widthStr)-1] = '\0';
            }
        }
        // For the third row, get the scale (the maximum value of a pixel).
        if (i == 2){
            char* s = strtok(line, "\n");
            if(s){
                strncpy(scaleStr,s, sizeof(scaleStr) - 1);
                scaleStr[sizeof(scaleStr) - 1] = '\0';
            }   
        }
    }
    fclose(file);

    *height = atoi(heightStr);
    *width = atoi(widthStr);
    *scale = atoi(scaleStr);
}

/* Generates a dynamically-allocated 2D array. */
float **generateNormalizedGrid(char *ConvertedPGMFileName, int height, int width, int scale){
    // Malloc the grid's rows and columns.
    float **normalizedGrid = malloc(height * sizeof(float*));
    for (int i = 0; i < height; i++){
        normalizedGrid[i] = malloc(width * sizeof(float));
    }

    // Open the file and skip the first 3 rows (the header has already been parsed.)
    FILE *file = fopen(ConvertedPGMFileName, "r");
    if (file == NULL){
        return 0;
        printf("Unable to open converted file %s.", ConvertedPGMFileName);
    }
    char line[256];
    for (int skip = 0; skip < 3; skip++){
        fgets(line, sizeof(line), file);
    }

    // Calculate the normalized pixel values for the grid and fill the grid.
    char* pixel;
    int j = 0;
    int i = 0;
    int pixelIndex = 0;
    while (fgets(line,sizeof(line), file)){
        pixel = strtok(line," \n");
        while(pixel != NULL && pixelIndex < (height * width)){
            i = pixelIndex / width; // rows
            j = pixelIndex % width; // columns
            
            normalizedGrid[i][j] = atoi(pixel) / (float) scale;

            pixel = strtok(NULL," \n");
            pixelIndex++;
        }
    }
    fclose(file);

    return normalizedGrid;
}

/* Helper function used in fillCellGrid to calculate a binary index based on the corners of the cell. 
    The bit 0 or 1 shifted:
        3 => binary 1000 => decimal 2^3 = 8
        2 => binary 100 => decimal 2^2 = 4
        1 => binary 10 => 2^1 = 2
        0 (no shift) => binary 1 => decimal 1
*/
int calculateBinaryIndex(int top_left, int top_right, int bottom_right, int bottom_left){
    int finalValue = (top_left << 3) + (top_right << 2) + (bottom_right << 1) + (bottom_left << 0);
    return finalValue;
}

/* Fills the contour cell grid. */
void fillCellGrid(float** normalizedGrid, cell_t **contourCellGrid, int cellGridHeight, int cellGridWeight){
    // Set indexes to traverse and access the normalizedGrid. This starts at the top_left corner of each cell.
    int nRow = 0;

    for(int row = 0; row < cellGridHeight; row++){
        int nCol = 0; // reset to 0 after each row.
        for(int col = 0; col < cellGridWeight; col++){
            int x = col;
            int y = row;

            // Apply threshold and calculate values.
            int top_left = (normalizedGrid[nRow][nCol] >= THRESHOLD? 1 : 0);
            int top_right = (normalizedGrid[nRow][(nCol+1)] >= THRESHOLD? 1 : 0);
            int bottom_right = (normalizedGrid[(nRow+1)][nCol] >= THRESHOLD? 1 : 0);
            int bottom_left = (normalizedGrid[(nRow+1)][(nCol+1)] >= THRESHOLD? 1 : 0);

            int caseValue = calculateBinaryIndex(top_left, top_right, bottom_right, bottom_left);
            cell_t cell = {x, y, top_left, top_right, bottom_right, bottom_left, caseValue};
            contourCellGrid[col][row] = cell;
            nCol++;
        }
        nRow++;
    }
}

/* A instruction for a single line, with [x1, y1] as the start point and [x2, y2] as the end point. */
typedef struct SingleLineInstruction_t{
    float x1, y1;  // start point
    float x2, y2;  // end point
} SingleLineInstruction;

// SingleLineInstruction* getContouringCase(cell_t cell){

// }

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
    
    // 8. declare a function called getContouringCase that takes a cell_t and returns an array of SingleLineInstructions. Each SingleLineInstructions is a pair of X,Y offsets for 2 points. => Interpolation.
    // for(int i = 0; i < contourCellHeight; i++){
    //     for(int j = 0; j < contourCellWidth; j++){
    //         cell_t cell = cellGrid[i][j];
    //         SingleLineInstruction sli = getContouringCase(cell);
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

// single getContouringCase(cell_t cell)
