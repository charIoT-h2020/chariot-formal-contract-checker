/////////////////////////////////
//
// Library   : Static Analysis
// Unit      : domsec
// File      : domsec_callback.h
// Copyright : CEA LIST - 2020
//
// Description :
//   Definition of a generic C Interface to manipulate abstract domains
//   = interval, disjunctions, ...
//

#pragma once

#ifdef _MSC_VER
#pragma pack(push,4)
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define DLL_API
#define DefineStartGenericFunctions struct _DomainElementFunctions {
#define DefineEndGenericFunctions };
#define DefineDomainPrefix(x) (*x)
#include "AbstractDomain.inch"
#undef DefineLoaderPrefix
#undef DefineStartGenericFunctions
#undef DefineEndGenericFunctions
#undef DLL_API

#ifdef __cplusplus
}
#endif

#ifdef _MSC_VER
#pragma pack(pop)
#endif

