/* simple Logger class */
#pragma once
#include <cstdio>

enum LogSeverityLevel : unsigned int {
  LOG_SEVERITY_UNKNOWN = 0,
  LOG_SEVERITY_VERBOSE = 1,
  LOG_SEVERITY_MESSAGE = 2,
  LOG_SEVERITY_INFO = 3,
  LOG_SEVERITY_WARNING = 4,
  LOG_SEVERITY_ERROR = 5,
  MAX_LOG_SEVERITY_LEVEL
};

class Logger {
 public:
  /* log if input log level is equal or smaller to log level set */
  template <typename... Args>
  static void Log(LogSeverityLevel logLevel, const char* s, Args... args) {
    if (logLevel <= mLogLevel) {
      std::printf(s, args...);
      /* force output, i.e. for Eclipse */
      std::fflush(stdout);
    }
  }

  static void SetLogLevel(LogSeverityLevel logLevel) {
    logLevel < MAX_LOG_SEVERITY_LEVEL ? mLogLevel = logLevel
                                      : mLogLevel = LOG_SEVERITY_ERROR;
  }

 private:
  static LogSeverityLevel mLogLevel;
};
