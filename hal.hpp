#ifndef _EAMA_HAL_HPP_
#define _EAMA_HAL_HPP_

extern "C" {
  #include "lib/tamalib/tamalib.h"
}

extern hal_t hal;
void pollButtons();

#endif // _EAMA_HAL_HPP_
