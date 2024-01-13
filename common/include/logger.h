#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <cstdio>
#include <cstdarg>
#include "timer.h"
#include "cs.hpp"
#include <iostream>

struct debugger
{
    template<typename T> debugger& operator , (const T& v)
    {
        std::cout<<v<<" ";
        return *this;
    }
};

#define DEBUG_PRINT

#ifdef DEBUG_PRINT
#define debug(debugtype, args...) if(debugtype){std::cout<<"> ";Util::Logger::dbg,args;std::cout<<std::endl;}
#else
#define debug(args...) {}
#endif

namespace Util
{
  class Logger
  {
  private:
    FILE*     logF;
    CS&       logCS;
    Timer     timer;
    static CS stdoutCS;

  public:
    enum Mode
    {
      Write,
      Append
    };

    Logger(const char* fileName,
           Mode        mode,
           CS&         logCS);

    ~Logger();

    void add(const char* format, ...);

    static void toStdOut(const char* format, ...);

    static void abort(const char* format, ...);
    
    static struct debugger dbg;
  }; // class Logger
} // namespace Util

#endif // LOGGER_HPP
