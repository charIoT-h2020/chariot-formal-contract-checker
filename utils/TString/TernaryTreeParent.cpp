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
// File      : TernaryTreeParent.cpp
// Description :
//   Implementation of a template class of TernaryTreeParent whose nodes
//   have a back reference to their parent node.
//

// #include "TString/String.hpp"
#include "TString/TernaryTreeParent.h"

namespace STG {}

#include "TString/TernaryTreeParent.template"

namespace STG { namespace DTernaryTree {

template class BaseSuffixObject<SubString>;

template bool
BaseTernaryTree::setToFirst<TImplParentTernaryTree<SubString>::Cursor, TParentFollowingCell<SubString>, SubString>
   (TImplParentTernaryTree<SubString>::Cursor& cursor, TParentFollowingCell<SubString>* root, SubString* subString);
template bool
BaseTernaryTree::setToLast<TImplParentTernaryTree<SubString>::Cursor, TParentFollowingCell<SubString>, SubString>
   (TImplParentTernaryTree<SubString>::Cursor& cursor, TParentFollowingCell<SubString>* root, SubString* subString);
template bool
BaseTernaryTree::setToNext<TImplParentTernaryTree<SubString>::Cursor, TParentFollowingCell<SubString>, SubString>
   (TImplParentTernaryTree<SubString>::Cursor& cursor, TParentFollowingCell<SubString>* root, SubString* subString);
template bool
BaseTernaryTree::setToPrevious<TImplParentTernaryTree<SubString>::Cursor, TParentFollowingCell<SubString>, SubString>
   (TImplParentTernaryTree<SubString>::Cursor& cursor, TParentFollowingCell<SubString>* root, SubString* subString);

template class BaseSuffixObject<WideSubString>;

template bool
BaseTernaryTree::setToFirst<TImplParentTernaryTree<WideSubString>::Cursor, TParentFollowingCell<WideSubString>, WideSubString>
   (TImplParentTernaryTree<WideSubString>::Cursor& cursor, TParentFollowingCell<WideSubString>* root, WideSubString* wideSubString);
template bool
BaseTernaryTree::setToLast<TImplParentTernaryTree<WideSubString>::Cursor, TParentFollowingCell<WideSubString>, WideSubString>
   (TImplParentTernaryTree<WideSubString>::Cursor& cursor, TParentFollowingCell<WideSubString>* root, WideSubString* wideSubString);
template bool
BaseTernaryTree::setToNext<TImplParentTernaryTree<WideSubString>::Cursor, TParentFollowingCell<WideSubString>, WideSubString>
   (TImplParentTernaryTree<WideSubString>::Cursor& cursor, TParentFollowingCell<WideSubString>* root, WideSubString* wideSubString);
template bool
BaseTernaryTree::setToPrevious<TImplParentTernaryTree<WideSubString>::Cursor, TParentFollowingCell<WideSubString>, WideSubString>
   (TImplParentTernaryTree<WideSubString>::Cursor& cursor, TParentFollowingCell<WideSubString>* root, WideSubString* wideSubString);

}} // end of namespace STG::DTernaryTree

template STG::DTernaryTree::BaseTernaryTree::LocationResult
STG::DTernaryTree::BaseTernaryTree::tlocate<STG::TImplParentTernaryTree<STG::SubString> >(
      const STG::TImplParentTernaryTree<STG::SubString>&, STG::SubString&, STG::TImplParentTernaryTree<STG::SubString>::Cursor&);

template STG::DTernaryTree::BaseTernaryTree::LocationResult
STG::DTernaryTree::BaseTernaryTree::tlocate<STG::TImplParentTernaryTree<STG::WideSubString> >(
      const STG::TImplParentTernaryTree<STG::WideSubString>&, STG::WideSubString&, STG::TImplParentTernaryTree<STG::WideSubString>::Cursor&);

template class STG::TImplParentTernaryTree<STG::SubString>;
template class STG::TImplParentTernaryTree<STG::WideSubString>;

