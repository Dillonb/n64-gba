#ifndef N64_GBA_LOG_H
#define N64_GBA_LOG_H
#include <libdragon.h>
#include <stdio.h>

#define LOG_VERBOSITY_WARN 1
#define LOG_VERBOSITY_INFO 2
#define LOG_VERBOSITY_DEBUG 3

static const int gba_log_verbosity = LOG_VERBOSITY_WARN;

#ifdef LOG_ENABLED
#define logfatal(message,...) if (1) {printf("[FATAL]  " message "\n", ##__VA_ARGS__); while(true) {console_render();}}
#define logwarn(message,...) if (gba_log_verbosity >= LOG_VERBOSITY_WARN) {printf("[WARN]  " message "\n", ##__VA_ARGS__); }
#define loginfo(message,...) if (gba_log_verbosity >= LOG_VERBOSITY_INFO) {printf("[INFO]  " message "\n", ##__VA_ARGS__); }
#define logdebug(message,...) if (gba_log_verbosity >= LOG_VERBOSITY_DEBUG) {printf("[DEBUG] " message "\n", ##__VA_ARGS__); }

#define unimplemented(condition, message) if (condition) { logfatal("UNIMPLEMENTED CASE DETECTED: %s", message) }
#else
#define logfatal(message,...) if (1) {printf("[FATAL]  " message "\n", ##__VA_ARGS__); while(true) {console_render();}}
#define logwarn(message,...) while(0){}
#define loginfo(message,...) while(0){}
#define logdebug(message,...) while(0){}

#define unimplemented(condition, message) if (condition) { logfatal("UNIMPLEMENTED CASE DETECTED: %s", message) }
#endif

#endif //N64_GBA_LOG_H
