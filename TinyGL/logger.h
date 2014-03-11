#ifndef LOGGER_H
#define LOGGER_H

#include "singleton.h"

#include <iostream>
#include <string>
#include <vector>

class Logger : public Singleton<Logger>
{
private:
    friend class Singleton<Logger>;
    std::ostream* m_pStream;
    std::vector<std::string> m_vLog;

    Logger() {m_pStream = NULL;}
    ~Logger() {m_pStream = NULL;}

public:
    void log(std::string);
    void warn(std::string);
    void error(std::string);
    void setLogStream(std::ostream*);
};

#endif // LOGGER_H
