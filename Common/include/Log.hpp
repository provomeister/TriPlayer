#ifndef LOG_HPP
#define LOG_HPP

#include <string>

namespace Log {
    // Log levels (level set will log it and everything below)
    enum class Level {
        Info = 0,
        Success = 1,
        Warning = 2,
        Error = 3,
        None = 4
    };

    // Returns log level (can be used to save some log calls)
    Level loggingLevel();

    // Adjust log level
    void setLogLevel(Level);

    // Open/close file for writing
    bool openFile(std::string, Level = Level::Info);
    void closeFile();

    // Log message
    void writeError(std::string);
    void writeInfo(std::string);
    void writeSuccess(std::string);
    void writeWarning(std::string);
}

#endif