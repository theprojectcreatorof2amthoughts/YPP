#ifndef YPP_EMBEDDED_LIBRARIES_H
#define YPP_EMBEDDED_LIBRARIES_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

const char *y_embedded_library(const char *name);
size_t y_embedded_library_count(void);
const char *y_embedded_library_name(size_t index);

#ifdef __cplusplus
}
#endif

#endif
