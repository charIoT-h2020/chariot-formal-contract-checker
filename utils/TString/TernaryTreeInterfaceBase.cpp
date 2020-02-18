/**************************************************************************/
/*                                                                        */
/*  Copyright (C) 2013-2019                                               */
/*    CEA (Commissariat a l'Energie Atomique et aux Energies              */
/*         Alternatives)                                                  */
/*                                                                        */
/*  you can redistribute it and/or modify it under the terms of the GNU   */
/*  Lesser General Public License as published by the Free Software       */
/*  Foundation, version 2.1.                                              */
/*                                                                        */
/*  It is distributed in the hope that it will be useful,                 */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of        */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         */
/*  GNU Lesser General Public License for more details.                   */
/*                                                                        */
/*  See the GNU Lesser General Public License version 2.1                 */
/*  for more details (enclosed in the file LICENSE).                      */
/*                                                                        */
/**************************************************************************/

/////////////////////////////////
//
// Library   : TString
// Unit      : dictionary
// File      : TernaryTreeInterfaceBase.cpp
// Description :
//   Implementation of common utilities to the collection dictionaries based on ternary trees.
//

#include "TString/TernaryTreeInterfaceBase.h"
// #include "TString/String.hpp"

namespace COL {}

#include "TString/TernaryTreeInterfaceBase.template"

template class COL::TVirtualMapCollection<COL::LightCopyKeyTraits<STG::SubString> >;
template class COL::TVirtualMapCollection<COL::LightCopyKeyTraits<STG::WideSubString> >;

