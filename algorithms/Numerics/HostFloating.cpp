/////////////////////////////////
//
// Library   : Numerics
// Unit      : Floating
// File      : Floating.cpp
// Authors   : Franck Vedrine, Gilles Mouchard, Bruno Marre
// Copyright : CEA LIST - 2005 - 2008, 2014, 2015
//
// Description :
//   Implementation of a class of floating points with unbound size.
//

#include "Numerics/HostFloating.h"
// #include "Numerics/Numerics.hpp"

namespace Numerics {}

#include "Numerics/Integer.template"
#include "Numerics/Floating.template"
#include "Numerics/HostFloating.template"
// #include "Numerics/Numerics.template"

template class Numerics::TDoubleElement<Numerics::TFloatingBase<Numerics::DoubleTraits> >;
template class Numerics::TDoubleElement<Numerics::TFloatingBase<Numerics::FloatTraits> >;
template class Numerics::TDoubleElement<Numerics::TFloatingBase<Numerics::LongDoubleTraits> >;

