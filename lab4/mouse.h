#include <lcom/lcf.h>

/*
Enables stream mode data reporting.

Enables stream mode data reporting, by sending the respective command to the mouse.

This function is provided by the LCF.

In order to score points for your Lab 4 grade, you should implement this functionality yourself. This can be done by defining your own function with the same functionality, or just by adding this functionality somewhere in your code
*/
int mouse_enable_data_reporting_original();	



/*
Handles mouse interrutps.

Reads the status register and the output buffer (OB).

If there was some error, the byte read from the OB should be discarded.

Must be defined using parenthesis around the function name.
*/
void (mouse_ih)(void);


