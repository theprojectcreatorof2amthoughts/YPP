#ifndef YPP_RUNTIME_H
#define YPP_RUNTIME_H
#include "ypp/bridge.h"
#ifdef __cplusplus
extern "C" {
#endif
int ypp_runtime_main(int argc, char **argv);
int ypp_runtime_check_file(const char *path);
#ifdef __cplusplus
}
#endif
#endif
