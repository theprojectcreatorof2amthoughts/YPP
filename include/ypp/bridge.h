#ifndef YPP_BRIDGE_H
#define YPP_BRIDGE_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef enum { YPP_VALUE_VOID=0, YPP_VALUE_NUMBER=1, YPP_VALUE_STRING=2, YPP_VALUE_BOOLEAN=3 } YPPValueKind;
typedef struct {
    YPPValueKind kind;
    double number_value;
    const char *string_value;
    int boolean_value;
} YPPValue;
typedef int (*YPPNativeFunction)(const YPPValue *arguments, size_t argument_count,
                                 YPPValue *result, char *error_message, size_t error_capacity);
/* Register a host C/C++ function callable from YPP code. Returned strings are copied before callback return. */
int ypp_register_native(const char *name, YPPNativeFunction function);
#ifdef __cplusplus
}
#endif
#endif
