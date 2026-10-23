#ifndef NES_H
#define NES_H

#include <stdbool.h>
#include <stdint.h>

void nes_run_frame(void);
const uint32_t *nes_get_framebuffer(void);
void nes_set_button(int button, bool pressed);

#endif