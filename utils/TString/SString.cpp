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
// Unit      : sub-string
// File      : SString.cpp
// Description :
//   Implementation of the class SString that defines full strings.
//

#include "TString/SString.h"
// #include "TString/String.hpp"

namespace STG {}

#include <time.h>
#include "TString/StringRep.template"
#include "TString/SString.template"

namespace STG {

namespace DIOObject {
   /* class IFStream */

#define DefineImplementsIStreamMethods
#define DefineTypeObject IFStream
#define DefineLinkStream fsIn
#include "StandardClasses/IOStreamMethods.incc"
#undef DefineLinkStream
#undef DefineTypeObject
#undef DefineImplementsIStreamMethods

   /* class OFStream */

#define DefineImplementsOStreamMethods
#define DefineTypeObject OFStream
#define DefineLinkStream ofsOut
#include "StandardClasses/IOStreamMethods.incc"
#undef DefineLinkStream
#undef DefineTypeObject

#define DefineTypeObject FStream
#define DefineLinkStream fsOut
#include "StandardClasses/IOStreamMethods.incc"
#undef DefineLinkStream
#undef DefineTypeObject
#undef DefineImplementsOStreamMethods

} // end of namespace DIOObject

} // end of namespace STG

template class STG::Lexer::TTextBuffer<STG::TSubString<char, STG::GlobalPolicy, STG::TSubStringTraits<char> > >;
template class STG::DIOObject::TISSubString<STG::TSubString<char, STG::GlobalPolicy, STG::TSubStringTraits<char> > >;
template class STG::DIOObject::TOSSubString<STG::TSubString<char, STG::GlobalPolicy, STG::TSubStringTraits<char> > >;
template class STG::DIOObject::TOSRSubString<STG::TSubString<char, STG::GlobalPolicy, STG::TSubStringTraits<char> > >;

