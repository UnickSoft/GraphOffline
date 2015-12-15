//
//  Logger.cpp
//  Graphoffline
//
//  Created by Олег on 14.12.15.
//
//

#include "Logger.h"
#include <fstream>

Logger::LogLevel Logger::m_topLevel = LE_NOTHING;
std::string Logger::m_logFilename;

Logger::Logger(Logger::LogLevel level)
{
    m_level = level;
}

Logger::~Logger()
{
    if (m_level <= m_topLevel)
    {
        fprintf(stderr, "%s", m_outputStream.str().c_str());
        fflush(stderr);
        
        if (!m_logFilename.empty())
        {
            std::ofstream logFile (m_logFilename);
            logFile << m_outputStream.rdbuf();
        }
    }
}

// LE_NOTHING by defaults.
void Logger::SetEnableLog(LogLevel topLogLevel)
{
    m_topLevel = topLogLevel;
}

Logger::LogLevel Logger::GetEnableLog()
{
    return m_topLevel;
}

void Logger::SetEnableLog(const std::string& filename)
{
    m_logFilename = filename;
}

// Return output stream for log.
std::ostringstream& Logger::GetOutputStream()
{
    return m_outputStream;
}
