#include <string.h> // used for size_t parameter in f().

#ifndef MARCHINGSQUARESLIB_H
#define MARCHINGSQUARESLIB_H


// Global constants
#define THRESHOLD 0.5
#define MAX_SINGLE_LINE_INSTRUCTIONS 2
#define NUM_OF_CONTOUR_CASES 16; // 0 through 15
extern const char *PGMExtension;
extern const char *expectedFileFormat;

/** Structs **/
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

/* An instruction for a single line, with x and y offsets. Each case may have 0, 1 or 2 of these. */
typedef struct SingleLineInstruction_t{
    float xOffset, yOffset;
} SingleLineInstruction;

/* A case has 0, 1, or 2 single line instructions mapped to it. */
typedef struct Case_t{
    int numOfLines;
    SingleLineInstruction sliArray[MAX_SINGLE_LINE_INSTRUCTIONS];
} Case;

/** Functions **/
/* Initializes the lookup table for contouring cases */
void initializeLookupTable(Case *lookupTable);

/* Based on a cell's case index value, return its singleLineInstruction array.*/
SingleLineInstruction **getContouringCase(cell_t cell);

/* Gets input from the user, then returns a char pointer to the filename of the image to convert. */
void getImageToConvert(char *fileName);

/* A debugging helper function to capture the normalized grid as a text file. Formatted to 3 decimal places. */
int writeNormalizedGridToTxt(char *fileStem, float** normalizedGrid, int height, int width);

/* A debugging helper function to capture the normalized grid with a threshold applied so that 0 or 1 are now the possible values. Saved as a text file. */
int writeThresholdGridToTxt(char *fileStem, float** normalizedGrid, int height, int width);

/* A debugging helper function to capture the contour cell grid as a text file. */
int writeContourGridToTxt(char *fileStem, cell_t** contourGrid, int height, int width);

/* Generates the Magick program command for generating a PGM file. Also generates the expected PGM output file name. */
void generatePGMConversionCommand(const char *ImageToConvertFilename, char *ConvertedPGMFileName, size_t convertedSize, char *pgmCommand, size_t commandSize, char *fileStem, size_t fileStemSize);

/* Executes the PGM command. */
int executePGMCommand(char *convertToPGMCommand);

/* Parses the PGM file to get the Height, Width and Scale. */
void parsePGMFile(char *ConvertedPGMFileName, int *height, int *width, int *scale);

/* Generates a dynamically-allocated 2D array. */
float **generateNormalizedGrid(char *ConvertedPGMFileName, int height, int width, int scale);

/* Helper function used in fillCellGrid to calculate a binary index based on the corners of the cell. 
    The bit 0 or 1 shifted:
        3 => binary 1000 => decimal 2^3 = 8
        2 => binary 100 => decimal 2^2 = 4
        1 => binary 10 => 2^1 = 2
        0 (no shift) => binary 1 => decimal 1
*/
int calculateBinaryIndex(int top_left, int top_right, int bottom_right, int bottom_left);

/* Fills the contour cell grid. */
void fillCellGrid(float** normalizedGrid, cell_t **contourCellGrid, int cellGridHeight, int cellGridWeight);

#endif