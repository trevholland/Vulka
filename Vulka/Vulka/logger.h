#pragma once

#include <iostream>
#include <Windows.h>
#include <cstdio>
#include <cstdarg>
#if __cplusplus < 201703L
#include <memory>
#endif

#define LOG_COLOR_BLACK         0x0
#define LOG_COLOR_DARK_BLUE     0x1
#define LOG_COLOR_DARK_GREEN    0x2
#define LOG_COLOR_DARK_CYAN     0x3
#define LOG_COLOR_DARK_RED      0x4
#define LOG_COLOR_DARK_MAGENTA  0x5
#define LOG_COLOR_DARK_YELLOW   0x6
#define LOG_COLOR_LIGHT_GRAY    0x7
#define LOG_COLOR_GRAY          0x8
#define LOG_COLOR_BLUE          0x9
#define LOG_COLOR_GREEN         0xA
#define LOG_COLOR_CYAN          0xB
#define LOG_COLOR_RED           0xC
#define LOG_COLOR_MAGENTA       0xD
#define LOG_COLOR_YELLOW        0xE
#define LOG_COLOR_WHITE         0xF

#define LOG_COLOR_CODE(text,background) ((background<<4)|text)

#define LOG_COLOR_DEFAULT   LOG_COLOR_LIGHT_GRAY
#define LOG_COLOR_DEBUG     LOG_COLOR_DARK_CYAN
#define LOG_COLOR_WARN      LOG_COLOR_CODE(LOG_COLOR_YELLOW, LOG_COLOR_GRAY)
#define LOG_COLOR_ERROR     LOG_COLOR_RED

class Logger
{
public:
    Logger()
    {
        hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
        herr = GetStdHandle(STD_ERROR_HANDLE);
    }

    /*
    Log formatted text in the default color.
    There isn't a new line at the end of this log.
    Use logn() to get a newline at the end (or add it yourself).
    */
    void log(const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        this->log_internal(format(fmt, args), LOG_COLOR_DEFAULT);
        va_end(args);
    }

    /*
    Log formatted text in the default color and add a new line at the end.
    */
    void logn(const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        this->log_internal(format(fmt, args), LOG_COLOR_DEFAULT, true);
        va_end(args);
    }

    /*
    Log formatted text in the specified color.
    There isn't a new line at the end of this log.
    Use logn() to get a newline at the end (or add it yourself).
    */
    void log(WORD color, const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        this->log_internal(format(fmt, args), color);
        va_end(args);
    }

    /*
    Log formatted text in the specified color and add a new line at the end.
    */
    void logn(WORD color, const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        this->log_internal(format(fmt, args), color, true);
        va_end(args);
    }

    /*
    Log formatted text in the specified text color and background color.
    There isn't a new line at the end of this log.
    Use logn() to get a newline at the end (or add it yourself).
    */
    void log(WORD textColor, WORD backgroundColor, const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        this->log_internal(format(fmt, args), LOG_COLOR_CODE(textColor, backgroundColor));
        va_end(args);
    }

    /*
    Log formatted text in the specified text color and background color
    and add a new line at the end.
    */
    void logn(WORD textColor, WORD backgroundColor, const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        this->log_internal(format(fmt, args), LOG_COLOR_CODE(textColor, backgroundColor), true);
        va_end(args);
    }

    /*
    Log debug text.
    Always appends a new line at the end.
    */
    void debug(const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        this->log_internal(format(fmt, args), LOG_COLOR_DEBUG, true);
        va_end(args);
    }

    /*
    Log a warning.
    Always append a new line at the end.
    */
    void warn(const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        this->log_internal(format(fmt, args), LOG_COLOR_WARN, true);
        va_end(args);
    }

    /*
    Log an error and stream to std::cerr.
    Always append a new line at the end.
    */
    void error(const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        std::string logstr = format(fmt, args);
        this->error_internal(logstr, LOG_COLOR_ERROR);
        va_end(args);
    }

    /*
    ! ! !WARNING! ! ! ... DON'T ACTUALLY USE THIS ... ! ! !WARNING! ! !
    */
    void vulkawarn(const char* fmt, ...)
    {
        log(LOG_COLOR_BLACK, LOG_COLOR_DARK_YELLOW, " ! ! !WARNING! ! ! ");
        va_list args;
        va_start(args, fmt);
        std::string logstr = format(fmt, args);
        this->log_internal(logstr, LOG_COLOR_CODE(LOG_COLOR_DARK_RED, LOG_COLOR_LIGHT_GRAY));
        va_end(args);
        logn(LOG_COLOR_BLACK, LOG_COLOR_DARK_YELLOW, " ! ! !WARNING! ! ! ");
    }

private:
    template <typename... Args>
    void log_internal(const std::string& text, WORD color, bool newline=false)
    {
        // cache the current screen buffer info
        GetConsoleScreenBufferInfo(hstdout, &csbi);
        // set the specified color
        SetConsoleTextAttribute(hstdout, color);
        // out the text
        std::cout << text.c_str();
        if (newline)
        {
            std::cout << std::endl;
        }
        // set the console data back
        SetConsoleTextAttribute(hstdout, csbi.wAttributes);
        // TODO: Could be better to not set it back since we specify a color every time. Not sure...
    }

    template <typename... Args>
    void error_internal(const std::string& text, WORD color)
    {
        // cache the current screen buffer info
        GetConsoleScreenBufferInfo(herr, &csbi);
        // set the specified color
        SetConsoleTextAttribute(herr, color);
        std::cerr << text.c_str() << std::endl;
        // set the console data back
        SetConsoleTextAttribute(herr, csbi.wAttributes);
        // TODO: Could be better to not set it back since we specify a color every time. Not sure...
    }

    std::string format(const char* fmt, va_list args)
    {
        // at some point, it might be worth checking the resulting string length.
        // 128 could be too much.
        char buf[128]; 
        const auto r = std::vsnprintf(buf, sizeof buf, fmt, args);
        if (r < 0)
        {
            // conversion failed
            return {};
        }
        const size_t len = r;
        if (len < sizeof buf)
        {
            // we fit in the buffer
            return { buf, len };
        }
#if __cplusplus >= 201703L
        // C++17: Create a string and write to its underlying array
        std::string s(len, '\0');
        std::vsnprintf(s.data(), len + 1, fmt, args);
        return s;
#else
        // C++11 or C++14: We need to allocate scratch memory
        auto vbuf = std::unique_ptr<char[]>(new char[len + 1]);
        std::vsnprintf(vbuf.get(), len + 1, fmt, args);
        return { vbuf.get(), len };
#endif
    }

    HANDLE hstdout;
    HANDLE herr;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
};

/* 
* Anyone using this logger should access it from here.
* It is purposelynot a singleton because it might make sense
* to instantiate more than one in the future.
*/
Logger logger;