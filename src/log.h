/*=============================
/* LOG 
/*
/*
/*===========================*/
#pragma once 
#include <chrono>
#include <iomanip>
#include <iostream>
// Workaround GCC 4.7.2 not recognizing noinline attribute
#ifndef NOINLINE_ATTRIBUTE
#ifdef __ICC
#define NOINLINE_ATTRIBUTE __attribute__(( noinline ))
#else
#define NOINLINE_ATTRIBUTE
#endif // __ICC
#endif // NOINLINE_ATTRIBUTE

struct None { };

template<typename List>
struct LogData {
  List list;
};

template<typename Begin, typename Value>
constexpr LogData<std::pair<Begin&&, Value&&>> operator<<(LogData<Begin>&& begin,
                                                          Value&& value) noexcept
{
  return {{ std::forward<Begin>(begin.list), std::forward<Value>(value) }};
}

template<typename Begin, size_t n>
constexpr LogData<std::pair<Begin&&, const char*>> operator<<(LogData<Begin>&& begin,
                                                              const char (&value)[n]) noexcept
{
  return {{ std::forward<Begin>(begin.list), value }};
}

typedef std::ostream& (*PfnManipulator)(std::ostream&);

template<typename Begin>
constexpr LogData<std::pair<Begin&&, PfnManipulator>> operator<<(LogData<Begin>&& begin,
                                                                 PfnManipulator value) noexcept
{
  return {{ std::forward<Begin>(begin.list), value }};
}

  template <typename Begin, typename Last>
void output(std::ostream& os, std::pair<Begin, Last>&& data)
{
  output(os, std::move(data.first));
  os << data.second;
}

inline void output(std::ostream& os, None)
{ }
template<typename List>
void Log(const char* file, int line,
         LogData<List>&& data) NOINLINE_ATTRIBUTE
{
  auto now = std::chrono::system_clock::now();
  std::time_t t_c = std::chrono::system_clock::to_time_t(now);
  std::cout << std::put_time(std::localtime(&t_c), "%F %T ") << file << ":" << line << ": ";
  output(std::cout, std::move(data.list));
  std::cout << std::endl;
}

#define LOG(msg) (Log(__FILE__, __LINE__, LogData<None>() << msg))

