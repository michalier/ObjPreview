#pragma once
#include <exception>
#include <string>

using namespace std;

class exc : public exception
{
protected:
  string msg;

public:
  exc() = delete;
  exc(int line, const char* where);
  exc(const char* what, int line, const char* where);
  exc(string what, int line, const char* where);

  virtual const char* what() const noexcept;
};