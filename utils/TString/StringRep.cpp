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
// File      : StringRep.cpp
// Description :
//   Implementation of a storage class that defines the support for the sub-strings.
//

#include "TString/StringRep.h"
// #include "TString/String.hpp"

namespace STG {}

#include "TString/StringRep.template"
#include "Pointer/ImplList.template"

namespace STG {

template class TRepository<char>;
template class TRepository<wchar_t>;
template class TListRepository<char>;
template class TListRepository<wchar_t>;
template class TListRepository<char, DListRepository::LocalRepositoryPolicy<char> >;
template class TListRepository<wchar_t, DListRepository::LocalRepositoryPolicy<wchar_t> >;

} // end of namespace STG

template class STG::DListRepository::TBasicRepository<char, STG::DListRepository::LightLocalRepositoryPolicy<char> >;
template class STG::DListRepository::TMoveNotification<char, STG::DListRepository::LightLocalRepositoryPolicy<char> >;
template class STG::DListRepository::TBasicRepository<wchar_t, STG::DListRepository::LightLocalRepositoryPolicy<wchar_t> >;
template class STG::DListRepository::TMoveNotification<wchar_t, STG::DListRepository::LightLocalRepositoryPolicy<wchar_t> >;
template class STG::DListRepository::TBasicRepository<char, STG::DListRepository::GlobalRepositoryPolicy<char> >;
template class STG::DListRepository::TMoveNotification<char, STG::DListRepository::GlobalRepositoryPolicy<char> >;
template class STG::DListRepository::TBasicRepository<wchar_t, STG::DListRepository::GlobalRepositoryPolicy<wchar_t> >;
template class STG::DListRepository::TMoveNotification<wchar_t, STG::DListRepository::GlobalRepositoryPolicy<wchar_t> >;
template class STG::DListRepository::LightLocalRepositoryPolicy<char>::MoveNotification;
template class STG::DListRepository::TPosition<char, STG::DListRepository::LightLocalRepositoryPolicy<char> >;
template class STG::DListRepository::TPosition<char, STG::DListRepository::GlobalRepositoryPolicy<char> >;
template class STG::DListRepository::LightLocalRepositoryPolicy<wchar_t>::MoveNotification;
template class STG::DListRepository::TPosition<wchar_t, STG::DListRepository::LightLocalRepositoryPolicy<wchar_t> >;
template class STG::DListRepository::TPosition<wchar_t, STG::DListRepository::GlobalRepositoryPolicy<wchar_t> >;

template class STG::DListRepository::LocalRepositoryPolicy<char>::BaseSubString;
template class STG::DListRepository::LocalRepositoryPolicy<wchar_t>::BaseSubString;
// template void STG::DListRepository::LocalRepositoryPolicy<wchar_t>::BaseSubString::decreaseEndTo(STG::Details::DListRepository::LocalRepositoryPolicy<wchar_t>::AtomicRepository*, int);
// template void STG::DListRepository::LocalRepositoryPolicy<char>::BaseSubString::decreaseEndTo(STG::Details::DListRepository::LocalRepositoryPolicy<char>::AtomicRepository*, int);
// template void STG::DListRepository::LocalRepositoryPolicy<wchar_t>::BaseSubString::increaseStartTo(STG::Details::DListRepository::LocalRepositoryPolicy<wchar_t>::AtomicRepository*, int);
// template void STG::DListRepository::LocalRepositoryPolicy<char>::BaseSubString::increaseStartTo(STG::Details::DListRepository::LocalRepositoryPolicy<char>::AtomicRepository*, int);

template class STG::DStringRep::TBasicRepository<char>;
template class STG::DStringRep::TBasicRepository<wchar_t>;

template bool COL::ImplList::foreachDo<std::function<bool (STG::DListRepository::LocalRepositoryPolicy<char>::AtomicRepository const&)>, TemplateElementCastParameters<STG::DListRepository::LocalRepositoryPolicy<char>::AtomicRepository, HandlerCast<STG::DListRepository::LocalRepositoryPolicy<char>::AtomicRepository, COL::ImplListElement> > >(TemplateElementCastParameters<STG::DListRepository::LocalRepositoryPolicy<char>::AtomicRepository, HandlerCast<STG::DListRepository::LocalRepositoryPolicy<char>::AtomicRepository, COL::ImplListElement> >, std::function<bool (STG::DListRepository::LocalRepositoryPolicy<char>::AtomicRepository const&)>&, COL::ImplListElement*, COL::ImplListElement*) const;
template bool COL::ImplList::foreachDo<std::function<bool (STG::DListRepository::LocalRepositoryPolicy<wchar_t>::AtomicRepository const&)>, TemplateElementCastParameters<STG::DListRepository::LocalRepositoryPolicy<wchar_t>::AtomicRepository, HandlerCast<STG::DListRepository::LocalRepositoryPolicy<wchar_t>::AtomicRepository, COL::ImplListElement> > >(TemplateElementCastParameters<STG::DListRepository::LocalRepositoryPolicy<wchar_t>::AtomicRepository, HandlerCast<STG::DListRepository::LocalRepositoryPolicy<wchar_t>::AtomicRepository, COL::ImplListElement> >, std::function<bool (STG::DListRepository::LocalRepositoryPolicy<wchar_t>::AtomicRepository const&)>&, COL::ImplListElement*, COL::ImplListElement*) const;

