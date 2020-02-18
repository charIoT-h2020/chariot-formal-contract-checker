#pragma once

#include "TString/String.hpp"

namespace DLL
{

class Library
{
public:
#ifdef __unix__
  typedef void* type;
#elif _WIN32
  typedef HMODULE type;
#endif

  Library();
  Library(Library&& source)
    : _lib(source._lib) { source._lib = 0; }
  Library(const char* libName);
  ~Library();

  bool isOpen() const;
  operator bool() const;
  void setFromFile(const char* libName);

  template<typename _Func>
  inline void loadSymbol(const char* symbol, _Func* fun)
    { loadSymbol(symbol, reinterpret_cast<void**>(fun));}

private:
  void loadSymbol(const char* symbol, void** fun);
  type _lib;
};

}

