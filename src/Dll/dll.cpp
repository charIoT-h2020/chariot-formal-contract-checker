#include "dll.h"

#ifdef __unix__
# include <dlfcn.h>
#elif _WIN32
# include <windows.h>
#else
# error \
  Unrecognized platform. Please implement functions of this file for your platform.
#endif

#ifdef __unix__

namespace DLL
{


Library::Library() : _lib(nullptr) {}


Library::Library(const char* libName)
  : _lib(nullptr)
  { _lib = dlopen(libName, RTLD_LAZY);}


void
Library::setFromFile(const char* libName)
  { AssumeCondition(!_lib)
    _lib = dlopen(libName, RTLD_LAZY);
  }


Library::~Library()
  { if (_lib) dlclose(_lib); }


bool Library::isOpen() const { return _lib; }

DLL::Library::operator bool() const { return _lib; }

void Library::loadSymbol(const char* symbol, void** fun)
{
  AssumeCondition(_lib)
  *fun = dlsym(_lib, symbol);
  char* err = dlerror();
  AssumeCondition(!err)
}

} // DLL

#elif _WIN32

namespace DLL
{

Library::Library(const char* libName)
  { _lib = LoadLibrary(libName); }

void
Library::setFromFile(const char* libName)
  { if (_lib)
     throw STG::EReadError();
    _lib = LoadLibrary(libName);
  }

Library::~Library() { FreeLibrary(_lib); }

bool Library::isOpen() const
  { return _lib; }

void Library::loadSymbol(const char* symbol, void** fun)
{
  *fun = reinterpret_cast<void*>(GetProcAddress(_lib, symbol));
  if (*fun)
     return;
  static STG::SString errorMessage;
  errorMessage.clear();
  STG::DIOObject::TOSRSubString out(errorMessage);
  out << "Unable to load symbol: Error "
      << (long unsigned int) GetLastError();
  throw STG::EReadError(errorMessage.queryChunk().string);
}

} // DLL

#endif
