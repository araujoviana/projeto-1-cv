#ifndef INTERFACE_H
#define INTERFACE_H

//=================================================================================================

#include <stdbool.h>

//=================================================================================================

bool Interface_initialize(void);
bool Interface_convert_image(void);
bool Interface_load_image(const char* filename);

void Interface_render(void);
void Interface_shutdown(void);
void Interface_update_mouse_title(float mouse_x, float mouse_y);

//=================================================================================================

#endif