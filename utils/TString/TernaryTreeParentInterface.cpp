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
// File      : TernaryTreeParentInterface.cpp
// Description :
//   Implementation of a collection interface to the dictionaries based on ternary trees.
//

#include "TString/TernaryTreeParentInterface.h"
// #include "TString/String.hpp"

namespace COL {}

#include "TString/TernaryTreeParentInterface.template"

namespace COL { namespace DTernaryTree {

template class TTernaryTree< ::STG::SubString, ::STG::TImplParentTernaryTree< ::STG::SubString>,
   ::STG::DTernaryTree::TParentFollowingCell< ::STG::SubString> >;
template class TTernaryTreeCursor< ::STG::SubString, ::STG::TImplParentTernaryTree< ::STG::SubString>,
   ::STG::DTernaryTree::TParentFollowingCell< ::STG::SubString> >;
template class TTernaryTree< ::STG::WideSubString, ::STG::TImplParentTernaryTree< ::STG::WideSubString>,
   ::STG::DTernaryTree::TParentFollowingCell< ::STG::WideSubString> >;
template class TTernaryTreeCursor< ::STG::WideSubString, ::STG::TImplParentTernaryTree< ::STG::WideSubString>,
   ::STG::DTernaryTree::TParentFollowingCell< ::STG::WideSubString> >;

} // end of namespace DTernaryTree

template class ParentTernaryTree<STG::SubString>;
template class ParentTernaryTreeCursor<STG::SubString>;

template class ParentTernaryTree<STG::WideSubString>;
template class ParentTernaryTreeCursor<STG::WideSubString>;

} // end of namespace COL

template bool STG::DTernaryTree::BaseTernaryTree::setToSon
   <STG::TImplParentTernaryTree<STG::SubString>::Cursor,
    STG::DTernaryTree::TParentFollowingCell<STG::SubString>, STG::SubString>
   (STG::TImplParentTernaryTree<STG::SubString>::Cursor&, STG::SubString::Char, STG::DTernaryTree::TParentFollowingCell<STG::SubString>*, STG::SubString*);
template bool STG::DTernaryTree::BaseTernaryTree::setToSon
   <STG::TImplParentTernaryTree<STG::WideSubString>::Cursor,
    STG::DTernaryTree::TParentFollowingCell<STG::WideSubString>, STG::WideSubString>
   (STG::TImplParentTernaryTree<STG::WideSubString>::Cursor&, STG::WideSubString::Char, STG::DTernaryTree::TParentFollowingCell<STG::WideSubString>*, STG::WideSubString*);

