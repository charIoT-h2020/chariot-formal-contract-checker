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
// File      : TernaryTreeInterface.cpp
// Description :
//   Implementation of a collection interface to the dictionaries based on ternary trees.
//

#include "TString/TernaryTreeInterface.h"
// #include "TString/String.hpp"

namespace COL {}

#include "TString/TernaryTreeInterface.template"

namespace COL { namespace DTernaryTree {

template class TTernaryTree< ::STG::SubString, ::STG::TImplTernaryTree< ::STG::SubString>,
   ::STG::DTernaryTree::TFollowingCell< ::STG::SubString> >;
template class TTernaryTreeCursor< ::STG::SubString, ::STG::TImplTernaryTree< ::STG::SubString>,
   ::STG::DTernaryTree::TFollowingCell< ::STG::SubString> >;
template class TTernaryTree< ::STG::WideSubString, ::STG::TImplTernaryTree< ::STG::WideSubString>,
   ::STG::DTernaryTree::TFollowingCell< ::STG::WideSubString> >;
template class TTernaryTreeCursor< ::STG::WideSubString, ::STG::TImplTernaryTree< ::STG::WideSubString>,
   ::STG::DTernaryTree::TFollowingCell< ::STG::WideSubString> >;

} // end of namespace DTernaryTree

template class TernaryTree<STG::SubString>;
template class TernaryTreeCursor<STG::SubString>;
template class TernaryTree<STG::WideSubString>;
template class TernaryTreeCursor<STG::WideSubString>;

} // end of namespace COL

template bool STG::DTernaryTree::BaseTernaryTree::setToSon
   <STG::TImplTernaryTree<STG::SubString>::Cursor,
    STG::DTernaryTree::TFollowingCell<STG::SubString>, STG::SubString>
   (STG::TImplTernaryTree<STG::SubString>::Cursor&, STG::SubString::Char,
    STG::DTernaryTree::TFollowingCell<STG::SubString> *, STG::SubString*);
template bool STG::DTernaryTree::BaseTernaryTree::setToSon
   <STG::TImplTernaryTree<STG::WideSubString>::Cursor,
    STG::DTernaryTree::TFollowingCell<STG::WideSubString>, STG::WideSubString>
   (STG::TImplTernaryTree<STG::WideSubString>::Cursor&, STG::WideSubString::Char, STG::DTernaryTree::TFollowingCell<STG::WideSubString> *, STG::WideSubString*);

