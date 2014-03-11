#include "logger.h"

void Logger::log(std::string s)
{
    if(s.empty())
        return;

    std::string str = "LOG: " + s;

    if(m_pStream != NULL)
        *m_pStream << str << std::endl;

    m_vLog.push_back(str + "\n");
}

void Logger::warn(std::string s)
{
    if(s.empty())
        return;

    std::string str = "WARNING: " + s;

    if(m_pStream != NULL)
        *m_pStream << str << std::endl;

    m_vLog.push_back(str + "\n");
}

void Logger::error(std::string s)
{
    if(s.empty())
        return;

    std::string str = "ERROR: " + s;

    if(m_pStream != NULL)
        *m_pStream << str << std::endl;

    m_vLog.push_back(str + "\n");
}

void Logger::setLogStream(std::ostream* s)
{
    m_pStream = s != NULL ? s : &std::cout;
}
