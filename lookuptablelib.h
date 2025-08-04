#ifndef LOOKUPTABLELIB_H
#define LOOKUPTABLELIB_H

// Global variables and constants
#define MAX_SINGLE_LINE_INSTRUCTIONS 2

/** Structs **/

/* A point used in a single line instruction. */
typedef struct SLIPoint_t{
    float x, y;
} Point;

/* An instruction for a single line, with x and y offsets. Each case may have 0, 1 or 2 of these. */
typedef struct SingleLineInstruction_t{
    Point point1;
    Point point2;
} SingleLineInstruction;

/* A case has 0, 1, or 2 single line instructions mapped to it. */
typedef struct Case_t{
    int numOfLines;
    SingleLineInstruction sliArray[MAX_SINGLE_LINE_INSTRUCTIONS];
} Case;

extern const Point emptyPoint;
extern const Point midTop;
extern const Point midLeft;
extern const Point midRight;
extern const Point midBottom;
extern const Case lookupTable[]; // look up table => array of cases 0 thru 15
#endif