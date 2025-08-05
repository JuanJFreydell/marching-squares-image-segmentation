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

    // 6. Capture the normalized grid as txt values and outputs a txt file of it.
    int writeNGridFile = writeNormalizedGridToTxt(fileStem, normalizedGrid, height, width);

    // 7. Capture the grid with the threshold applied (visuatlization using integers)
    int writeThresholdNGridFile = writeThresholdGridToTxt(fileStem, normalizedGrid, height, width);

    // 8. generate a "cellGrid" (a 2D array of dimensions Height-1 and Width-1), where each item is a cell_t.
    int contourGridHeight = height-1;
    int contourGridWidth = width-1;
    cell_t** cellGrid = malloc((contourGridWidth)* sizeof(cell_t*));
    for (int row = 0; row < height; row++){
        cellGrid[row] = malloc((contourGridHeight) * sizeof(cell_t));
    }

    // 9. Using the normalized grid values, fill the values of the cell grid with each cell's calculated case value. The case value is a bit number calculated from the number of true/false corners.
    fillCellGrid(normalizedGrid, cellGrid, contourGridHeight, contourGridWidth);

    // 10. Capture the contour grid with the caseValue (binary index values) printed.
    int writeCGridFile = writeContourGridToTxt(fileStem, cellGrid, contourGridHeight, contourGridWidth);
    
    // 11. Write a svg file by iterating over the contour grid. This function uses helper functions that access each cell's caseValue to generate a SVG according to the SingleLineInstruction lookup table.
    char SVGFileName[500] = "";
    writeSVG(fileStem, cellGrid, contourGridHeight, contourGridWidth, SVGFileName);

    // 12. rasterize a file with the svg layer on top of the png file. using magick's command: convert original_image.png contours.svg -layers composite output.png
    char rasterizedSVGFileName[500] = "";
    int rasterizeSuccess = rasterize(fileStem, sizeof(fileStem), SVGFileName, sizeof(SVGFileName), rasterizedSVGFileName, sizeof(rasterizedSVGFileName));
    if(rasterizeSuccess == 0){
        printf("SVG successfully rasterized.\n");
    }

    // 13. Overlay the two images together.
    int overlaySuccess = createFinalOutputImage(fileStem, rasterizedSVGFileName, ImageToConvertFilename);
    if(rasterizeSuccess == 0){
        printf("Final output image successfully generated.\n");
    }

    // 14. Free all mallocs.
    for (int i = 0; i < height; i++) free(normalizedGrid[i]);
    free(normalizedGrid);

    for (int i = 0; i < height - 1; i++) free(cellGrid[i]);
    free(cellGrid);

    fgets(ImageToConvertFilename, sizeof(ImageToConvertFilename), stdin);
    fgets(ImageToConvertFilename, sizeof(ImageToConvertFilename), stdin);

    return 1;
}
