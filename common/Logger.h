//
//  Logger.hpp
//  Graphoffline
//
//  Created by Олег on 14.12.15.
//
//

#ifndef Logger_hpp
#define Logger_hpp

#include <stdio.h>
#include <sstream>
#include "IAlgorithm.h"

class Logger
{
public:
    enum LogLevel {LE_NOTHING, LE_ERROR, LE_WARNING, LE_INFORMATION};
    
    Logger(LogLevel level);
    ~Logger();
    
    // LE_NOTHING by defaults.
    static void SetEnableLog(LogLevel topLogLevel);
    static LogLevel GetEnableLog();
    static void SetEnableLog(const std::string& filename);
    
    // Return output stream for log.
    std::ostringstream& GetOutputStream();
private:
    static LogLevel m_topLevel;
    static std::string m_logFilename;
    
    LogLevel m_level;
    std::ostringstream m_outputStream;
};


#define LOG_ERROR(str)   Logger(Logger::LE_ERROR).GetOutputStream() << "ERROR:" << str << std::endl;
#define LOG_WARNING(str) Logger(Logger::LE_WARNING).GetOutputStream() << "WARNING:" << str << std::endl;
#define LOG_INFO(str)    Logger(Logger::LE_INFORMATION).GetOutputStream() << str << std::endl;

std::ostream& operator<< (std::ostream& stream, const AlgorithmResult& result);



#endif /* Logger_hpp */
