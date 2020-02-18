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
// File      : TernaryTree.cpp
// Description :
//   Implementation of a template class of TernaryTree.
//

// #include "TString/String.hpp"
#include "TString/TernaryTree.h"

namespace STG {}

#include "TString/TernaryTree.template"

namespace STG { namespace DTernaryTree {

template bool
BaseTernaryTree::setToFirst<TImplTernaryTree<SubString>::Cursor, TFollowingCell<SubString>, SubString>
   (TImplTernaryTree<SubString>::Cursor& cursor, TFollowingCell<SubString>* root, SubString* subString);
template bool
BaseTernaryTree::setToLast<TImplTernaryTree<SubString>::Cursor, TFollowingCell<SubString>, SubString>
   (TImplTernaryTree<SubString>::Cursor& cursor, TFollowingCell<SubString>* root, SubString* subString);
template bool
BaseTernaryTree::setToNext<TImplTernaryTree<SubString>::Cursor, TFollowingCell<SubString>, SubString>
   (TImplTernaryTree<SubString>::Cursor& cursor, TFollowingCell<SubString>* root, SubString* subString);
template bool
BaseTernaryTree::setToPrevious<TImplTernaryTree<SubString>::Cursor, TFollowingCell<SubString>, SubString>
   (TImplTernaryTree<SubString>::Cursor& cursor, TFollowingCell<SubString>* root, SubString* subString);

template bool
BaseTernaryTree::setToFirst<TImplTernaryTree<WideSubString>::Cursor, TFollowingCell<WideSubString>, WideSubString>
   (TImplTernaryTree<WideSubString>::Cursor& cursor, TFollowingCell<WideSubString>* root, WideSubString* wideSubString);
template bool
BaseTernaryTree::setToLast<TImplTernaryTree<WideSubString>::Cursor, TFollowingCell<WideSubString>, WideSubString>
   (TImplTernaryTree<WideSubString>::Cursor& cursor, TFollowingCell<WideSubString>* root, WideSubString* wideSubString);
template bool
BaseTernaryTree::setToNext<TImplTernaryTree<WideSubString>::Cursor, TFollowingCell<WideSubString>, WideSubString>
   (TImplTernaryTree<WideSubString>::Cursor& cursor, TFollowingCell<WideSubString>* root, WideSubString* wideSubString);
template bool
BaseTernaryTree::setToPrevious<TImplTernaryTree<WideSubString>::Cursor, TFollowingCell<WideSubString>, WideSubString>
   (TImplTernaryTree<WideSubString>::Cursor& cursor, TFollowingCell<WideSubString>* root, WideSubString* wideSubString);

}} // end of namespace STG::DTernaryTree

template STG::DTernaryTree::BaseTernaryTree::LocationResult
STG::DTernaryTree::BaseTernaryTree::tlocate<STG::TImplTernaryTree<STG::SubString> >(
      const STG::TImplTernaryTree<STG::SubString>&, STG::SubString&, STG::TImplTernaryTree<STG::SubString>::Cursor&);
template STG::DTernaryTree::BaseTernaryTree::LocationResult
STG::DTernaryTree::BaseTernaryTree::tlocate<STG::TImplTernaryTree<STG::WideSubString> >(
      const STG::TImplTernaryTree<STG::WideSubString>&, STG::WideSubString&, STG::TImplTernaryTree<STG::WideSubString>::Cursor&);

template class STG::TImplTernaryTree<STG::SubString>;
template class STG::TImplTernaryTree<STG::WideSubString>;

