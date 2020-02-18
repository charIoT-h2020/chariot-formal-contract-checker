/////////////////////////////////
//
// Library   : ForwardNumerics
// Unit      : Floating
// File      : BaseTaylorFloatingGeneric.h
// Author    : Franck Vedrine
// Copyright : CEA LIST - 2015, 2016
//
// Description :
//   Definition of a class of floating points with mathematic functions
//   defined by Taylor approximations
//

#ifndef Numerics_BaseTaylorFloatingGenericH
#define Numerics_BaseTaylorFloatingGenericH

#include "ForwardNumerics/BaseFloatingGeneric.h"

namespace Numerics {

#define DefineGeneric
#include "ForwardNumerics/BaseTaylorFloating.inch"
#undef DefineGeneric

} // end of namespace Numerics

#endif // Numerics_BaseTaylorFloatingGenericH

