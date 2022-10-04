#ifndef N64_GBA_LOG_H
#define N64_GBA_LOG_H
#include <libdragon.h>
#include <stdio.h>

#define LOG_VERBOSITY_WARN 1
#define LOG_VERBOSITY_INFO 2
#define LOG_VERBOSITY_DEBUG 3

//static const int gba_log_verbosity = LOG_VERBOSITY_DEBUG;
#define gba_log_verbosity LOG_VERBOSITY_WARN
#define LOG_ENABLED

#ifdef LOG_ENABLED
#define logfatal(message,...) if (1) {debugf("[FATAL]  " message "\n", ##__VA_ARGS__); while(true) {}}
#define logwarn(message,...) if (gba_log_verbosity >= LOG_VERBOSITY_WARN) {debugf("[WARN]  " message "\n", ##__VA_ARGS__); }
#define loginfo(message,...) if (gba_log_verbosity >= LOG_VERBOSITY_INFO) {debugf("[INFO]  " message "\n", ##__VA_ARGS__); }
#define logdebug(message,...) if (gba_log_verbosity >= LOG_VERBOSITY_DEBUG) {debugf("[DEBUG] " message "\n", ##__VA_ARGS__); }

#define unimplemented(condition, message) if (condition) { logfatal("UNIMPLEMENTED CASE DETECTED: %s", message) }
#else
#define logfatal(message,...) if (1) {debugf("[FATAL]  " message "\n", ##__VA_ARGS__); while(true) {}}
#define logwarn(message,...) while(0){}
#define loginfo(message,...) while(0){}
#define logdebug(message,...) while(0){}

#define unimplemented(condition, message) if (condition) { logfatal("UNIMPLEMENTED CASE DETECTED: %s", message) }
#endif

#endif //N64_GBA_LOG_H
