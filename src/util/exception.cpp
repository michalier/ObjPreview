#include "exception.hpp"

exc::exc(int line, const char* where)
{
  this->msg = string(where) + ":" + to_string(line);
}
  
exc::exc(const char* what, int line, const char* where)
{
  this->msg = string(what) + " " + string(where) + ":" + to_string(line);
}

exc::exc(string what, int line, const char* where)
{
  this->msg = what + " " + string(where) + ":" + to_string(line);
} 

const char* exc::what() const noexcept
{
  return msg.c_str();
}