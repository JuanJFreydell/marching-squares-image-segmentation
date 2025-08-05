#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "marchingsquareslib.h"

// Definition of global variables
const char *PGMExtension = ".pgm";
const char *expectedFileFormat = "PM2";

void getImageToConvert(char *fileName){
    printf("Enter the filename of the image to convert: ");
    scanf("%s", fileName);
}

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

int executePGMCommand(char *convertToPGMCommand){
    int success = system(convertToPGMCommand);
    if (success != 0) {
        fprintf(stderr, "Error running conversion command.\n");
        return 1;
    }
    return success;
}

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

void writeSVG(cell_t **contourCellGrid, int cellGridHeight, int cellGridWidth){

    FILE* file;
    file = fopen("lines.svg", "w");
    fprintf(file,"<svg height=\"%d\" width=\"%d\" xmlns=\"http://www.w3.org/2000/svg\">\n", cellGridHeight, cellGridWidth);
    cell_t cell;
    Case caseInstance;
    SingleLineInstruction instruction;
    float x1;
    float y1;
    float x2;
    float y2;

    for (int i = 0; i < cellGridHeight; i++){
        for (int j = 0; j < cellGridWidth; j++) {
            cell = contourCellGrid[i][j];
            caseInstance = lookupTable[cell.caseValue];
            for (int n = 0; n < caseInstance.numOfLines; n++){
                fprintf(file, "{CaseInst NumofLines: %d, LineNum: %d, CaseVal: %d, cell x: %d and j: %d | cell y: %d and i:%d }\n", caseInstance.numOfLines, n, cell.caseValue, cell.x, j, cell.y, i);
                x1 = caseInstance.sliArray[n].point1.x + cell.x;
                y1 = caseInstance.sliArray[n].point1.y + cell.y;
                x2 = caseInstance.sliArray[n].point2.x + cell.x;
                y2 = caseInstance.sliArray[n].point2.y + cell.y;
                fprintf(file,"\t<line x1=\"%.4f\" y1=\"%.4f\" x2=\"%.4f\" y2=\"%.4f\" style=\"stroke:red;stroke-width:2\" />\n",x1,y1,x2,y2);
            }
        }
    }
    fprintf(file, "</svg>");
    fclose(file);
}
