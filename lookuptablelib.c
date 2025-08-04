#include "lookuptablelib.h"

/* Visualization of SLI points
    (0,0)   midTop        (1,0)
         *-------*------*
         |              |
midLeft  *              * midRight
         |              |
         *-------*------*
  (0, 1)      midBtm     (1,1)
*/

// Definition of global variables
const Point emptyPoint = {0, 0};
const Point midTop = {0.5, 0};
const Point midLeft = {0, 0.5};
const Point midRight = {1, 0.5};
const Point midBottom = {0.5, 1};
// Each case in this lookup table is {int numOfLines, SingleLineInstruction sliArray}
const Case lookupTable[] = { 
    {0, // Case 0 => no line, both points are 0
        {
            {emptyPoint, emptyPoint}
        }
    },
    {1, // Case 1
        {
            {midLeft, midBottom}
        }
    },
    {1, // Case 2
        {
            {midRight, midBottom}
        }
    },
    {1, // Case 3
        {
            {midLeft, midRight}
        }
    },
    {1, // Case 4
        {
            {midTop, midRight}
        }
    },
    {2, // Case 5
        {
            {midLeft, midTop}, 
            {midBottom, midRight}
        }
    },
    {1, // Case 6
        {
            {midTop, midBottom}
        }
    },
    {1, // Case 7
        {
            {midLeft, midTop}
        }
    },
    {1, // Case 8
        {
            {midLeft, midTop}
        }
    },
    {1, // Case 9
        {
            {midTop, midBottom}
        }
    },
    {10, // Case 10
        {
            {midLeft, midBottom}, 
            {midTop, midRight}
        }
    },
    {1, // Case 11
        {
            {midTop, midRight}
        }
    },
    {1, // Case 12
        {
            {midLeft, midRight}
        }
    },
    {1, // Case 13 
        {
            {midBottom, midRight}
        }
    },
    {1, // Case 14
        {
            {midLeft, midBottom}
        }
    },
    {0, // Case 15
        {
            {emptyPoint, emptyPoint}
        }
    }
};
