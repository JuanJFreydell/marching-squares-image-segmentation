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
const char *PGMExtension = ".pgm";

/* Gets input from the user, then returns a char pointer to the filename of the image to convert. */
void getImageToConvert(char *fileName){
    printf("Enter the filename of the image to convert: ");
    scanf("%s", fileName);
}

/* A debugging helper function to capture the normalized grid as a text file. */
int writeNormalizedGridToTxt(char *ConvertedPGMFileName, float** normalizedGrid, int height, int width){
    // Create the file name with the PGM filename
    char normalizedGridFileName[500]= "";
    strcat(normalizedGridFileName, ConvertedPGMFileName);
    strcat(normalizedGridFileName, "NormalizedGrid.txt");
    // Open the file and write to it.
    FILE *file = fopen(normalizedGridFileName, "w"); 
    if (file == NULL) {
        printf("Error opening file");
        return 1;
    }

    for (int i = 0; i < height; i++){
        fprintf(file, "[");
        for (int j = 0; j < width; j++) {
            fprintf(file, " %.0f ", normalizedGrid[i][j]);
        }
        fprintf(file, "]\n");
    }
    fclose(file);
    return 0;
}

/* Generates the Magick program command for generating a PGM file. Also generates the expected PGM output file name. */
void *generatePGMConversionCommand(const char *ImageToConvertFilename, char *ConvertedPGMFileName, size_t convertedSize, char *pgmCommand, size_t commandSize){
    // Find the file name up to its extension.
    long originalFilenameLength = strlen(ImageToConvertFilename); 
    char* ExtensionStart = ".";
    char* originalFileExtension = strstr(ImageToConvertFilename, ExtensionStart); // locates the first occurence of extensionStart
    int ExtensionLength = strlen(originalFileExtension);
    int OriginalStemLength = originalFilenameLength - ExtensionLength;

    // instantiate the ConvertedPGMFileName to the stem file.
    strncpy(ConvertedPGMFileName, ImageToConvertFilename, OriginalStemLength); // copy the file name up to "."
    ConvertedPGMFileName[OriginalStemLength] = '\0';

    // concatenate the .pgm extension to the ConvertedPGMFileName.
    strncat(ConvertedPGMFileName, PGMExtension, convertedSize - strlen(ConvertedPGMFileName) - 1);

    //magick input.jpg -compress none -define pgm:format=plain output.pgm
    snprintf(pgmCommand, commandSize, "magick %s -resize 256x256\\! -compress none -define pgm:format=plain %s", ImageToConvertFilename, ConvertedPGMFileName);
}

int main() {
    // 1. take input on image filename
    char ImageToConvertFilename[512];
    getImageToConvert(ImageToConvertFilename);
    printf("Accessing file: %s\n", ImageToConvertFilename);

    // 2. generate command to convert to PGM and store the PGM output file name.
    char ConvertedPGMFileName[500] = "";
    char convertToPGMCommand[500] = "";
    generatePGMConversionCommand(ImageToConvertFilename, ConvertedPGMFileName, sizeof(ConvertedPGMFileName), convertToPGMCommand, sizeof(convertToPGMCommand));
    printf("Running command \"%s\" to convert input into PM2. \n", convertToPGMCommand);

    // 3. execute command 
    int commandResult = system(convertToPGMCommand);
    if (commandResult != 0) {
        fprintf(stderr, "Error running conversion command.\n");
        return 1;
    }

    // 4. Parse the PGM file to get the Height, Width and Scale.
    FILE *file = fopen(ConvertedPGMFileName, "r");

    if (file == NULL){
        return 0;
        printf("Unable to open converted file %s.", ConvertedPGMFileName);
    }
    
    char line[256];
    char* expectedFileFormat = "PM2";
    char* Height_temp;
    char* Width_temp;
    char Height[10];
    char Width[10];
    char Scale[10];
    for (int i = 0; i < 3; i++){
        fgets(line, sizeof(line), file);
        // For the header, check that the file format is P2.
        if (i == 0){
            line[strcspn(line, "\n")] = '\0'; // Replace any newline char with a terminal char.
            if(!strcmp(expectedFileFormat, line)){
                printf("error, file is not in PM2 format");
                return 0;
            }
        }
        // For the second row, the number of columns is listed, then the number of rows.
        if (i == 1){
            Height_temp = strtok(line, " ");
            Width_temp = strtok(NULL,"\n");
            if(Height_temp && Width_temp){
                strcpy(Height, Height_temp);
                Height[sizeof(Height)-1] = '\0';
                strcpy(Width, Width_temp);
                Width[sizeof(Width)-1] = '\0';
            }
        }
        // For the third row, get the scale (the maximum value of a pixel).
        if (i == 2){
            char* s = strtok(line, "\n");
            if(s){
                strncpy(Scale,s,sizeof(Scale) - 1);
                Scale[sizeof(Scale) - 1] = '\0';
            }
            printf("Height = %s, Width = %s, Scale = %s \n", Height, Width, Scale);
        }
    }

    // getPGMHeader(PGMFilename, &height, &width, &scale);

    // 5. to generate a 2D normalized integers between 0.0 and 1.0 reading the PGM file and using Height, Width and Scale.    
    
    int height = atoi(Height);
    int width = atoi(Width);
    int scale = atoi(Scale);

    float** normalizedGrid = malloc(height * sizeof(float*));

    for (int i = 0; i < height; i++){
        normalizedGrid[i] = malloc(width * sizeof(float));
    }

    char* pixel;
    int j = 0;
    int i = 0;
    int pixelIndex = 0;

    while (fgets(line,sizeof(line),file)){
        pixel = strtok(line," \n");
        while(pixel != NULL && pixelIndex < (height * width)){
            i = pixelIndex / width; // rows
            j = pixelIndex % width; // columns
            
            normalizedGrid[i][j] = atoi(pixel) / (float) scale;

            pixel = strtok(NULL," \n");
            pixelIndex++;
        }
    }

    // Captures the normalized grid as txt values and outputs a txt file of it.
    int writeFile = writeNormalizedGridToTxt(ConvertedPGMFileName, normalizedGrid, height, width);

    // 6. define type cell_t. 
    typedef struct {
        int x;
        int y;
        int top_right;
        int top_left;
        int bottom_right;
        int bottom_left;
    } cell_t;

    // 7. generate a "cellGrid" (a 2D array of dimensions Height-1 and Width-1), where each item is a cell_t,
    cell_t** cellGrid = malloc((height -1)* sizeof(cell_t*));

    for (int i = 0; i < height; i++){
        cellGrid[i] = malloc((width -1) * sizeof(cell_t));
    }

    // // 8. declare a function called getContouringCase that takes a cell_t and returns an array of SingleLineInstructions. Each SingleLineInstructions is a pair of X,Y offsets for 2 points.
    // SingleLineInstructions* getContouringCase(cell_s cell);

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