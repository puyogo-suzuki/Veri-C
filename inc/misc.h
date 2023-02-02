#pragma once
#include <stdlib.h>
#include <string.h>
#define nullptr NULL

static inline void* memdup(const void* mem, size_t size) { 
    if (size == 0) return nullptr;
   void* out = malloc(size);

   if(out != nullptr)
       memcpy(out, mem, size);

   return out;
}
