#ifndef BUTTON_FUNCTIONS_H_
#define BUTTON_FUNCTIONS_H_

#include "button.h"

void button_functions_init(void);

//Function and command tables
extern void (*button_func[BUTTONS_COUNT])(button *btn);




#endif /* BUTTON_FUNCTIONS_H_ */