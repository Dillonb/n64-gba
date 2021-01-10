#ifndef N64_GBA_LOG_H
#define N64_GBA_LOG_H
#include <libdragon.h>
#include <stdio.h>

#define LOG_VERBOSITY_WARN 1
#define LOG_VERBOSITY_INFO 2
#define LOG_VERBOSITY_DEBUG 3

static const int gba_log_verbosity = LOG_VERBOSITY_WARN;

#define logfatal(message,...) if (1) {printf("[FATAL]  " message "\n", ##__VA_ARGS__); while(true) {console_render();}}
#define logwarn(message,...) if (gba_log_verbosity >= LOG_VERBOSITY_WARN) {printf("[WARN]  " message "\n", ##__VA_ARGS__); console_render(); }
#define loginfo(message,...) if (gba_log_verbosity >= LOG_VERBOSITY_INFO) {printf("[INFO]  " message "\n", ##__VA_ARGS__); console_render(); }
#define logdebug(message,...) if (gba_log_verbosity >= LOG_VERBOSITY_DEBUG) {printf("[DEBUG] " message "\n", ##__VA_ARGS__); console_render(); }

#define unimplemented(condition, message) if (condition) { logfatal("UNIMPLEMENTED CASE DETECTED: %s", message) }

#endif //N64_GBA_LOG_H
