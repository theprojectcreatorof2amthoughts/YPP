#ifndef YPP_EMBEDDED_LIBRARIES_H
#define YPP_EMBEDDED_LIBRARIES_H
#include <stddef.h>
const char *y_embedded_library(const char *name);
size_t y_embedded_library_count(void);
const char *y_embedded_library_name(size_t index);
#endif
