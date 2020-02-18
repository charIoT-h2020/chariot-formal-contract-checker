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
// File      : TernaryTreeParentInterface.h
// Description :
//   Definition of a collection interface to the dictionaries based on ternary trees.
//

#ifndef COL_TernaryTreeParentInterfaceH
#define COL_TernaryTreeParentInterfaceH

#include "TString/TernaryTreeInterfaceBase.h"
#include "TString/TernaryTreeParent.h"

namespace COL {

#define DefTemplate template <class TypeSubString>
#define DefTypeCollection ParentTernaryTree<TypeSubString>
#define DefTypeCursor ParentTernaryTreeCursor<TypeSubString>
#define DefTypeSorted
#define DefTypeKey const TypeSubString&

template <class TypeSubString>
class ParentTernaryTreeCursor;

template <class TypeSubString>
class ParentTernaryTree : public DTernaryTree::TTernaryTree<TypeSubString,
      STG::TImplParentTernaryTree<TypeSubString>, STG::DTernaryTree::TParentFollowingCell<TypeSubString> > {
  private:
   typedef DTernaryTree::TTernaryTree<TypeSubString, STG::TImplParentTernaryTree<TypeSubString>,
         STG::DTernaryTree::TParentFollowingCell<TypeSubString> > inherited;
   typedef ParentTernaryTree<TypeSubString> thisType;
   typedef DTernaryTree::TTernaryTreeCursor<TypeSubString,
         STG::TImplParentTernaryTree<TypeSubString>,
         STG::DTernaryTree::TParentFollowingCell<TypeSubString> > thisCursorType;
     
   class LocationResultAccess : public inherited::LocationResult {
#ifdef __GNUC__
      LocationResultAccess(const TypeSubString& key) : LocationResult(key) {}
#endif
     public:
      friend class ParentTernaryTree<TypeSubString>;
   };

  public:
   typedef TypeSubString SubStringKey;
   typedef typename inherited::ExtendedLocateParameters ExtendedLocateParameters;
   typedef typename inherited::ExtendedInsertionParameters ExtendedInsertionParameters;
   typedef typename inherited::ExtendedSuppressParameters ExtendedSuppressParameters;
   typedef typename inherited::ExtendedReplaceParameters ExtendedReplaceParameters;
   typedef typename inherited::LocationResult LocationResult;
   typedef typename inherited::RelativePosition RelativePosition;
   typedef typename inherited::RemoveMode RemoveMode;
   typedef typename inherited::AddMode AddMode;
   typedef typename inherited::CursorNotification CursorNotification;
   typedef typename inherited::ExtendedMapLocateParameters ExtendedMapLocateParameters;
   typedef typename inherited::ExtendedMapInsertionParameters ExtendedMapInsertionParameters;
   typedef typename inherited::ExtendedMapSuppressParameters ExtendedMapSuppressParameters;
   typedef typename inherited::ExtendedMapReplaceParameters ExtendedMapReplaceParameters;
   
   typedef typename STG::TImplParentTernaryTree<TypeSubString>::Element Node;
   typedef COL::DVirtualMapCollection::Access::TMapInitialValues<Node,
         COL::LightCopyKeyTraits<TypeSubString>, SimpleCast> InitialValues;
   typedef typename COL::DVirtualMapCollection::Access::TMapInitialNewValues<Node,
         COL::LightCopyKeyTraits<TypeSubString>, SimpleCast> InitialNewValues;
   TemplateDefineCollectionForAbstractCollect(ParentTernaryTree, ParentTernaryTreeCursor, TypeSubString)

  protected:
   friend class COL::DTernaryTree::BaseTernaryTreeMethods;
   friend class COL::DTernaryTree::BaseTernaryTree<TypeSubString>;
   class SuppressionNotification;
   friend class SuppressionNotification;
   class SplitSuffixNotification;
   friend class SplitSuffixNotification;
   typedef typename inherited::SuppressionNotification ImplSuppressionNotification;
   typedef typename inherited::SplitSuffixNotification ImplSplitSuffixNotification;
   friend class ParentTernaryTreeCursor<TypeSubString>;

   virtual ComparisonResult _compare(const EnhancedObject& asource) const override
      {  return inherited::_compare(asource); }

   virtual void _fullAssign(const VirtualCollection& source, const ExtendedReplaceParameters& parameters) override
      {  VirtualCollection::pfullAssign(source, parameters); }
   void _fullAssign(const thisType& source, const ExtendedReplaceParameters& parameters);

      // query methods
   virtual VirtualSortedCollection::LocationResult _locateKey(const TypeSubString& key,
         const COL::VirtualCollection::ExtendedLocateParameters& parameters,
         typename inherited::LocateCursor* cursor,
         const typename inherited::LocateCursor* start,
         const typename inherited::LocateCursor* end) const override
      {  return inherited::_locateKey(key, parameters, cursor, start, end); }
   LocationResult _locateKey(const TypeSubString& key, const ExtendedLocateParameters& parameters,
         Cursor* cursor=nullptr, const Cursor* start=nullptr, const Cursor* end=nullptr) const
      {  return inherited::_locateKey(key, parameters, (thisCursorType*) cursor, (thisCursorType*) start, (thisCursorType*) end); }

   virtual TypeSubString _queryKey(const ExtendedLocateParameters& parameters,
         const VirtualCollectionCursor* cursor) const override
      {  return inherited::_queryKey(parameters, cursor); }
   TypeSubString _queryKey(const ExtendedLocateParameters& parameters,
         const Cursor* cursor=nullptr) const
      {  return inherited::_queryKey(parameters, (thisCursorType*) cursor); }

      // insertion methods
   virtual void _add(EnhancedObject* anObject,
         const COL::VirtualCollection::ExtendedInsertionParameters& parameters,
         VirtualCollectionCursor* cursor) override;
   void _add(Node* newElement, const ExtendedInsertionParameters& parameters,
         Cursor* cursor=nullptr);
   virtual void _addAll(const VirtualCollection& source,
         const COL::VirtualCollection::ExtendedInsertionParameters& parameters,
         VirtualCollectionCursor* cursor, const VirtualCollectionCursor* startSource,
         const VirtualCollectionCursor* endSource) override
      {  VirtualCollection::paddAll(source, parameters, cursor, startSource, endSource); }
   void _addAll(const thisType& source, const ExtendedInsertionParameters& parameters,
         Cursor* cursor=nullptr, const Cursor* startSource=nullptr, const Cursor* endSource=nullptr);

      // suppression methods
   void inherited_remove(const COL::VirtualCollection::ExtendedSuppressParameters& parameters,
         VirtualCollectionCursor* cursor)
      {  inherited::_remove(parameters, cursor); }
   virtual void _remove(const COL::VirtualCollection::ExtendedSuppressParameters& parameters,
         VirtualCollectionCursor* cursor) override;
   void _remove(const ExtendedSuppressParameters& parameters, Cursor* cursor=nullptr);

      // replace methods
   void inheritedReplace(EnhancedObject* anObject,
         const COL::VirtualCollection::ExtendedReplaceParameters& parameters,
         VirtualCollectionCursor* cursor)
      {  inherited::_replace(anObject, parameters, cursor); }
   virtual void _replace(EnhancedObject* anObject,
         const COL::VirtualCollection::ExtendedReplaceParameters& parameters,
         VirtualCollectionCursor* cursor) override;
   void _replace(Node* element, const ExtendedReplaceParameters& parameters,
         Cursor* cursor=nullptr);

   virtual EnhancedObject* _getElement(
         const COL::VirtualCollection::ExtendedLocateParameters& parameters,
         const VirtualCollectionCursor* cursor) const override
      {  return inherited::_getElement(parameters, cursor); }
   Node* _getElement(const ExtendedLocateParameters& parameters,
         const Cursor* cursor=nullptr) const
      {  return (Node*) inherited::_getElement(parameters, (const thisCursorType*) cursor); }

  public:
   ParentTernaryTree() {}
   ParentTernaryTree(const InitialValues& initialValues);
   ParentTernaryTree(const InitialNewValues& initialValues);
   ParentTernaryTree(const thisType& source,
         COL::VirtualCollection::AddMode duplicateMode=COL::VirtualCollection::AMNoDuplicate,
         const VirtualCast* retrieveRegistrationFromCopy=nullptr)
      : inherited(source, duplicateMode, retrieveRegistrationFromCopy) {}
   thisType& operator=(const thisType& source)
      {  thisType::_fullAssign(source, ExtendedReplaceParameters());
         return *this;
      }

   TemplateDefineCopy(ParentTernaryTree, TypeSubString)
   DTemplateDefineAssign(ParentTernaryTree, TypeSubString)

   #define DefJustDeclare
   #define DefTypeElement Node
   #define DefExcludeElement
   #include "Collection/ELMCollection.inch"
   #include "Collection/ELMMapCollection.inch"
   #undef DefExcludeElement
   #undef DefTypeElement
   #undef DefJustDeclare

   template <class Execute, class TemplateParameters>
   bool foreachDo(TemplateParameters params, Execute& fun, const ExtendedLocateParameters& parameters,
         const ParentTernaryTreeCursor<TypeSubString>* start=nullptr,
         const ParentTernaryTreeCursor<TypeSubString>* end=nullptr) const
      {  return DTernaryTree::BaseTernaryTreeMethods::foreachDo((const inherited&) *this, params, fun,
            parameters, (const thisCursorType*) start, (const thisCursorType*) end);
      }
   template <class Execute, class TemplateParameters>
   bool foreachDo(TemplateParameters params, Execute& fun) const
      {  return DTernaryTree::BaseTernaryTreeMethods::foreachDo((const inherited&) *this, params, fun); }
   template <class Execute, class TemplateParameters>
   bool foreachReverseDo(TemplateParameters params, Execute& fun, const ExtendedLocateParameters& parameters,
         const ParentTernaryTreeCursor<TypeSubString>* start=nullptr,
         const ParentTernaryTreeCursor<TypeSubString>* end=nullptr) const
      {  return DTernaryTree::BaseTernaryTreeMethods::foreachReverseDo((const inherited&) *this, params, fun,
            parameters, (const thisCursorType*) start, (const thisCursorType*) end);
      }
   template <class Execute, class TemplateParameters>
   bool foreachReverseDo(TemplateParameters params, Execute& fun) const
      {  return DTernaryTree::BaseTernaryTreeMethods::foreachReverseDo((const inherited&) *this, params, fun); }
      
   template <class Execute, class TemplateParameters>
   bool foreachKeyDo(TemplateParameters params, Execute& fun, const ExtendedLocateParameters& parameters,
         const ParentTernaryTreeCursor<TypeSubString>* start=nullptr,
         const ParentTernaryTreeCursor<TypeSubString>* end=nullptr) const
      {  return DTernaryTree::BaseTernaryTreeMethods::foreachKeyDo((const inherited&) *this, params, fun,
            parameters, (const thisCursorType*) start, (const thisCursorType*) end);
      }
   template <class Execute, class TemplateParameters>
   bool foreachKeyDo(TemplateParameters params, Execute& fun) const
      {  return DTernaryTree::BaseTernaryTreeMethods::foreachKeyDo((const inherited&) *this, params, fun); }
   template <class Execute, class TemplateParameters>
   bool foreachKeyReverseDo(TemplateParameters params, Execute& fun, const ExtendedLocateParameters& parameters,
         const ParentTernaryTreeCursor<TypeSubString>* start=nullptr,
         const ParentTernaryTreeCursor<TypeSubString>* end=nullptr) const
      {  return DTernaryTree::BaseTernaryTreeMethods::foreachKeyReverseDo((const inherited&) *this, params, fun,
            parameters, (const thisCursorType*) start, (const thisCursorType*) end);
      }
   template <class Execute, class TemplateParameters>
   bool foreachKeyReverseDo(TemplateParameters params, Execute& fun) const
      {  return DTernaryTree::BaseTernaryTreeMethods::foreachKeyReverseDo((const inherited&) *this, params, fun); }
      
   class DescentCursor : public DTernaryTree::TDescentTernaryTreeCursor<TypeSubString,
         STG::TImplParentTernaryTree<TypeSubString>,
         STG::DTernaryTree::TParentFollowingCell<TypeSubString> > {
     private:
      typedef DTernaryTree::TDescentTernaryTreeCursor<TypeSubString,
            STG::TImplParentTernaryTree<TypeSubString>,
            STG::DTernaryTree::TParentFollowingCell<TypeSubString> > inherited;

     public:
      DescentCursor(const ParentTernaryTree<TypeSubString>& support) : inherited(support) {}
      DescentCursor(const DescentCursor& source) = default;
      DefineCopy(DescentCursor)
      
      EnhancedObject* getSElement() const { return inherited::_getSElement(); }
      const EnhancedObject* getElement() const { return getSElement(); }
      const EnhancedObject& elementAt() const { return *getSElement(); }
      EnhancedObject& elementSAt() const { return *getSElement(); }
      virtual bool isValid() const override { return inherited::isValid(); }
   };
};

template <class TypeSubString>
class ParentTernaryTreeCursor : public DTernaryTree::TTernaryTreeCursor<TypeSubString, 
      STG::TImplParentTernaryTree<TypeSubString>, STG::DTernaryTree::TParentFollowingCell<TypeSubString> > {
  private:
   typedef DTernaryTree::TTernaryTreeCursor<TypeSubString,
         STG::TImplParentTernaryTree<TypeSubString>,
         STG::DTernaryTree::TParentFollowingCell<TypeSubString> > inherited;
   typedef ParentTernaryTreeCursor<TypeSubString> thisType;
   friend class ParentTernaryTree<TypeSubString>::SuppressionNotification;
   friend class ParentTernaryTree<TypeSubString>::SplitSuffixNotification;

   class CellAccess : public STG::TImplParentTernaryTree<TypeSubString>::Element {
     public:
#ifdef __GNUC__
      CellAccess() {}
#endif
      STG::DTernaryTree::TParentFollowingCell<TypeSubString>* parent() const
         { return STG::TImplParentTernaryTree<TypeSubString>::Element::parent(); }
   };
   class CursorAccess : public STG::TImplParentTernaryTree<TypeSubString>::Cursor {
     public:
#ifdef __GNUC__
      CursorAccess(const STG::TImplParentTernaryTree<TypeSubString>& implTree)
         : STG::TImplParentTernaryTree<TypeSubString>::Cursor(implTree) {}
#endif
      void setToReference(const typename ParentTernaryTree<TypeSubString>::Node& element)
         {  STG::TImplParentTernaryTree<TypeSubString>::Cursor::setToReference(
               ((const CellAccess&) element).parent());
         }
   };

   typedef typename inherited::CursorImplementation CursorImplementation;
   class CursorImplementationAccess : public CursorImplementation {
#ifdef __GNUC__
      CursorImplementationAccess(const STG::TImplParentTernaryTree<TypeSubString>& tree)
         :  CursorImplementation(tree) {}
#endif
     public:
      friend class ParentTernaryTree<TypeSubString>;
   };
   friend class CursorImplementationAccess;

  protected:
   virtual void _gotoReference(const EnhancedObject& element) override
      {  _gotoReference((const typename ParentTernaryTree<TypeSubString>::Node&) element); }

   void _gotoReference(const typename ParentTernaryTree<TypeSubString>::Node& element)
      {  ((CursorAccess&) inherited::getImplementation()).setToReference(element);
         inherited::releaseMode(getSupport());
      }

  public:
   ParentTernaryTreeCursor(const ParentTernaryTree<TypeSubString>& support) : inherited(support) {}
   ParentTernaryTreeCursor(const typename ParentTernaryTree<TypeSubString>::DescentCursor& descentCursor)
      : inherited(descentCursor, typename inherited::Descent()) {}
   ParentTernaryTreeCursor(const thisType& source) : inherited(source) {}
   TemplateDefineCopy(ParentTernaryTreeCursor, TypeSubString)
   TemplateDefineCursorForAbstractCollect(ParentTernaryTree, ParentTernaryTreeCursor, TypeSubString)

   typedef typename inherited::Position Position;
   typedef typename inherited::ExtendedLocateParameters ExtendedLocateParameters;
#define DefCursor
#define DefTypeFinal ParentTernaryTreeCursor<TypeSubString>::
#define DefTypeElement typename ParentTernaryTree<TypeSubString>::Node
#define DefTypeCastToCollectHandler(element) element
#define DefTypeCCastToCollectHandler(element) element
#define DefExcludeElement
#include "Collection/ELMCollection.inch"
#undef DefExcludeElement
#undef DefTypeCastToCollectHandler
#undef DefTypeCCastToCollectHandler
#undef DefTypeElement
#undef DefTypeFinal
#undef DefCursor
};

TemplateInlineCollectionForAbstractCollect(ParentTernaryTree, ParentTernaryTreeCursor, TypeSubString)
   
/* Inline methods of the class ParentTernaryTree */

template <class TypeSubString>
inline void
ParentTernaryTree<TypeSubString>::_add(EnhancedObject* anObject,
      const COL::VirtualCollection::ExtendedInsertionParameters& parameters,
      VirtualCollectionCursor* cursor)
   { inherited::_add((Node*) anObject, parameters, (thisCursorType*) cursor); }

template <class TypeSubString>
inline void
ParentTernaryTree<TypeSubString>::_remove(const COL::VirtualCollection::ExtendedSuppressParameters& parameters,
      VirtualCollectionCursor* cursor)
   { inherited::_remove(parameters, (thisCursorType*) cursor); }

template <class TypeSubString>
inline void
ParentTernaryTree<TypeSubString>::_replace(EnhancedObject* anObject,
      const COL::VirtualCollection::ExtendedReplaceParameters& parameters,
      VirtualCollectionCursor* cursor)
   { inherited::_replace((Node*) anObject, parameters, (thisCursorType*) cursor); }

template <class TypeSubString>
class ParentTernaryTree<TypeSubString>::SuppressionNotification
   :  public ParentTernaryTree<TypeSubString>::CursorNotification,
      public ParentTernaryTree<TypeSubString>::ImplSuppressionNotification {
  private:
   typedef typename ParentTernaryTree<TypeSubString>::ImplSuppressionNotification inheritedImplementation;
   typedef typename ParentTernaryTree<TypeSubString>::CursorNotification inherited;

  protected:
   virtual ComparisonResult _compare(const EnhancedObject& asource) const override
      {  return inherited::_compare(asource); }
   void update(ParentTernaryTreeCursor<TypeSubString>& cursor)
      {  typename STG::TImplParentTernaryTree<TypeSubString>::Cursor& implCursor = cursor.getImplementation();
         AssumeCondition(cursor.isExactCursor() && implCursor.isFinalValid())
         inheritedImplementation::applyFinal(implCursor);
      }

  public:
   SuppressionNotification() {}
   SuppressionNotification(const SuppressionNotification& source) = default;
   SuppressionNotification& operator=(const SuppressionNotification& source) = default;
   DDefineAssign(SuppressionNotification)
   DefineCursorNotificationMethods(SuppressionNotification, ParentTernaryTreeCursor<TypeSubString>)
   virtual bool isValid() const override
      {  return inherited::isValid() && inheritedImplementation::isValid(); }
};

template <class TypeSubString>
class ParentTernaryTree<TypeSubString>::SplitSuffixNotification
   :  public ParentTernaryTree<TypeSubString>::CursorNotification,
      public ParentTernaryTree<TypeSubString>::ImplSplitSuffixNotification {
  private:
   typedef typename ParentTernaryTree<TypeSubString>::ImplSplitSuffixNotification inheritedImplementation;
   typedef typename ParentTernaryTree<TypeSubString>::CursorNotification inherited;
   typedef typename ParentTernaryTree<TypeSubString>::FollowingCell FollowingCell;

  protected:
   virtual ComparisonResult _compare(const EnhancedObject& asource) const override
      {  return inherited::_compare(asource); }
   void update(ParentTernaryTreeCursor<TypeSubString>& cursor)
      {  typename STG::TImplParentTernaryTree<TypeSubString>::Cursor& implCursor = cursor.getImplementation();
         AssumeCondition(cursor.isExactCursor())
         inheritedImplementation::applyFinal(implCursor);
         AssumeCondition(implCursor.isFinalValid())
      }

  public:
   SplitSuffixNotification(FollowingCell& oldReference) : inheritedImplementation(oldReference) {}
   SplitSuffixNotification(const SplitSuffixNotification& source) = default;
   SplitSuffixNotification& operator=(const SplitSuffixNotification& source) = default;
   DDefineAssign(SplitSuffixNotification)
   DefineCursorNotificationMethods(SplitSuffixNotification, ParentTernaryTreeCursor<TypeSubString>)
   virtual bool isValid() const override
      {  return inherited::isValid() && inheritedImplementation::isValid(); }
};

#define DefTypeFinal ParentTernaryTree<TypeSubString>::
#define DefJustInline
#define DefTypeElement typename ParentTernaryTree<TypeSubString>::Node
#define DefTypeCastToCollectHandler(element) element
#define DefTypeCCastToCollectHandler(element) element
#define DefExcludeElement
#include "Collection/ELMCollection.inch"
#include "Collection/ELMMapCollection.inch"
#undef DefExcludeElement
#undef DefTypeCastToCollectHandler
#undef DefTypeCCastToCollectHandler
#undef DefTypeElement
#undef DefJustInline
#undef DefTypeFinal

#undef DefTemplate
#undef DefTypeCollection
#undef DefTypeCursor
#undef DefTypeSorted
#undef DefTypeKey

/*******************************************************/
/* Definition of the template class TParentTernaryTree */
/*******************************************************/

template <class TypeSubString>
class SimpleParentTernaryTreeCast {
  public:
   typedef typename ParentTernaryTree<TypeSubString>::Node Base;
   static typename ParentTernaryTree<TypeSubString>::Node* castFrom(
         typename ParentTernaryTree<TypeSubString>::Node* source) { return source; }
   static const typename ParentTernaryTree<TypeSubString>::Node* castFrom(
         const typename ParentTernaryTree<TypeSubString>::Node* source) { return source; }
   static typename ParentTernaryTree<TypeSubString>::Node& castFrom(
         typename ParentTernaryTree<TypeSubString>::Node& source) { return source; }
   static const typename ParentTernaryTree<TypeSubString>::Node& castFrom(
         const typename ParentTernaryTree<TypeSubString>::Node& source) { return source; }

   static typename ParentTernaryTree<TypeSubString>::Node* castTo(
         typename ParentTernaryTree<TypeSubString>::Node* source) { return source; }
   static const typename ParentTernaryTree<TypeSubString>::Node* castTo(const
         typename ParentTernaryTree<TypeSubString>::Node* source) { return source; }
   static typename ParentTernaryTree<TypeSubString>::Node& castTo(
         typename ParentTernaryTree<TypeSubString>::Node& source) { return source; }
   static const typename ParentTernaryTree<TypeSubString>::Node& castTo(const
         typename ParentTernaryTree<TypeSubString>::Node& source) { return source; }
};

template<class TypeSubString, class TypeElement, class Cast>
class TParentTernaryTreeCursor;

#define DefTypeElement TypeElement
#define DefTypeCollection TParentTernaryTree<TypeSubString, TypeElement, Cast>
#define DefTypeCastToCollectHandler Cast::castTo
#define DefTypeCCastToCollectHandler Cast::castTo
#define DefTypeCastFromCollectHandler (TypeElement*) Cast::castFrom
#define DefTypeCCastFromCollectHandler (const TypeElement*) Cast::castFrom
#define DefTypeInheritedCollection ParentTernaryTree<TypeSubString>
#define DefTypeCursor TParentTernaryTreeCursor<TypeSubString, TypeElement, Cast>
#define DefTypeSorted
#define DefTypeKey const TypeSubString&
#define DefExcludeElement

template<class TypeSubString, class TypeElement, class Cast=SimpleParentTernaryTreeCast<TypeSubString> >
class TParentTernaryTree : public ParentTernaryTree<TypeSubString> {
  public:
   typedef TypeSubString SubStringKey;
   typedef TypeElement Node;
   typedef COL::DVirtualMapCollection::Access::TMapInitialValues<TypeElement,
         COL::LightCopyKeyTraits<TypeSubString>, Cast> InitialValues;
   typedef COL::DVirtualMapCollection::Access::TMapInitialNewValues<TypeElement,
         COL::LightCopyKeyTraits<TypeSubString>, Cast> InitialNewValues;

  private:
   typedef TParentTernaryTree<TypeSubString, TypeElement, Cast> thisType;
   typedef TParentTernaryTreeCursor<TypeSubString, TypeElement, Cast> thisCursorType;
   typedef ParentTernaryTree<TypeSubString> inherited;

  protected:
   virtual bool acceptElement(const EnhancedObject& aelement) const
      {  return dynamic_cast<const TypeElement*>(Cast::castFrom(
               (const typename inherited::Node*) &aelement));
      }
   typedef typename inherited::DescentCursor InheritedDescentCursor;
   virtual VirtualSortedCollection::LocationResult _locateKey(const TypeSubString& key,
         const COL::VirtualCollection::ExtendedLocateParameters& parameters,
         typename inherited::LocateCursor* cursor,
         const typename inherited::LocateCursor* start,
         const typename inherited::LocateCursor* end) const
      {  return inherited::_locateKey(key, parameters, cursor, start, end); }
   typename inherited::LocationResult _locateKey(const TypeSubString& key, const typename inherited::ExtendedLocateParameters& parameters,
         typename inherited::Cursor* cursor=nullptr, const typename inherited::Cursor* start=nullptr, const typename inherited::Cursor* end=nullptr) const
      {  return inherited::_locateKey(key, parameters, cursor, start, end); }

   virtual TypeSubString _queryKey(const typename inherited::ExtendedLocateParameters& parameters,
         const VirtualCollectionCursor* cursor) const
      {  return inherited::_queryKey(parameters, cursor); }
   TypeSubString _queryKey(const typename inherited::ExtendedLocateParameters& parameters,
         const typename inherited::Cursor* cursor=nullptr) const
      {  return inherited::_queryKey(parameters, cursor); }

  public:
   typedef typename inherited::Key InheritedKey;
   class Key : public InheritedKey {
     public:
      static const STG::SubString& key(const TypeElement&)
         {  AssumeUncalled const STG::SubString* result = nullptr; return *result; }
   };
   
   typedef typename inherited::ExtendedLocateParameters ExtendedLocateParameters;
   typedef typename inherited::ExtendedInsertionParameters ExtendedInsertionParameters;
   typedef typename inherited::ExtendedSuppressParameters ExtendedSuppressParameters;
   typedef typename inherited::ExtendedReplaceParameters ExtendedReplaceParameters;
   typedef typename inherited::LocationResult LocationResult;
   typedef typename inherited::RelativePosition RelativePosition;
   typedef typename inherited::RemoveMode RemoveMode;
   typedef typename inherited::AddMode AddMode;
   
   TParentTernaryTree() {}
   TParentTernaryTree(const InitialValues& initialValues)
      {  COL::DVirtualMapCollection::Access::TMapInsertInitialValue<
            TParentTernaryTree<TypeSubString, TypeElement, Cast>, TypeElement,
            COL::LightCopyKeyTraits<TypeSubString>, Cast> insertInitialValue(*this);
         initialValues.foreachDo(insertInitialValue);
      }
   TParentTernaryTree(const InitialNewValues& initialValues)
      {  COL::DVirtualMapCollection::Access::TMapInsertInitialNewValue<
            TParentTernaryTree<TypeSubString, TypeElement, Cast>, TypeElement,
            COL::LightCopyKeyTraits<TypeSubString>, Cast> insertInitialValue(*this);
         initialValues.foreachDo(insertInitialValue);
      }
   TParentTernaryTree(const thisType& source,
         COL::VirtualCollection::AddMode duplicateMode=COL::VirtualCollection::AMNoDuplicate,
         const VirtualCast* retrieveRegistrationFromCopy=nullptr)
      : inherited(source, duplicateMode, retrieveRegistrationFromCopy) {}
   Template3DefineCopy(TParentTernaryTree, TypeSubString, TypeElement, Cast)
   Template3DefineCollectionForAbstractCollect(TParentTernaryTree, TParentTernaryTreeCursor, TypeSubString, TypeElement, Cast)

   #include "Collection/ELMCollection.inch"
   #include "Collection/ELMMapCollection.inch"

   class VirtualExecute {
     public:
      virtual ~VirtualExecute() {}
      virtual bool operator()(const TypeSubString& ssKey, TypeElement& eSource) = 0;
   };

   template <class Execute>
      bool foreachDo(Execute& fun, const ExtendedLocateParameters& parameters,
         const Cursor* start=nullptr, const Cursor* end=nullptr) const
      {  return inherited::foreachDo(
            COL::VirtualCollection::TemplateForeachParameters<TypeElement, Cast>(), fun,
            parameters, start, end);
      }
   template <class Execute>
      bool foreachDo(Execute& fun) const
      {  return inherited::foreachDo(
            COL::VirtualCollection::TemplateForeachParameters<TypeElement, Cast>(), fun);
      }
   template <class Execute>
      bool foreachKeyDo(Execute& fun, const ExtendedLocateParameters& parameters,
         const Cursor* start=nullptr, const Cursor* end=nullptr) const
      {  return inherited::foreachKeyDo(
            COL::VirtualCollection::TemplateForeachParameters<TypeElement, Cast>(), fun,
            parameters, start, end);
      }
   template <class Execute>
      bool foreachKeyDo(Execute& fun) const
      {  return inherited::foreachKeyDo(
            COL::VirtualCollection::TemplateForeachParameters<TypeElement, Cast>(), fun);
      }
   bool vforeachDo(VirtualExecute& fun, const ExtendedLocateParameters& parameters,
      const Cursor* start=nullptr, const Cursor* end=nullptr) const
      {  return foreachKeyDo(fun, parameters, start, end); }
   bool vforeachDo(VirtualExecute& fun) const
      {  return foreachKeyDo(fun); }
   template <class Execute>
      bool foreachReverseDo(Execute& fun, const ExtendedLocateParameters& parameters,
         const Cursor* start=nullptr, const Cursor* end=nullptr) const
      {  return inherited::foreachReverseDo(
            COL::VirtualCollection::TemplateForeachParameters<TypeElement, Cast>(), fun,
            parameters, start, end);
      }
   template <class Execute>
      bool foreachReverseDo(Execute& fun) const
      {  return inherited::foreachReverseDo(
            COL::VirtualCollection::TemplateForeachParameters<TypeElement, Cast>(), fun);
      }
   template <class Execute>
      bool foreachKeyReverseDo(Execute& fun, const ExtendedLocateParameters& parameters,
         const Cursor* start=nullptr, const Cursor* end=nullptr) const
      {  return inherited::foreachKeyReverseDo(
            COL::VirtualCollection::TemplateForeachParameters<TypeElement, Cast>(), fun,
            parameters, start, end);
      }
   template <class Execute>
      bool foreachKeyReverseDo(Execute& fun) const
      {  return inherited::foreachKeyReverseDo(
            COL::VirtualCollection::TemplateForeachParameters<TypeElement, Cast>(), fun);
      }
   bool vforeachReverseDo(VirtualExecute& fun, const ExtendedLocateParameters& parameters,
      const Cursor* start=nullptr, const Cursor* end=nullptr) const
      {  return foreachKeyReverseDo(fun, parameters, start, end); }
   bool vforeachReverseDo(VirtualExecute& fun) const
      {  return foreachKeyReverseDo(fun); }

   bool foreachDo(std::function<bool (const TypeElement&)> function) const
      {  return List::foreachDo(TemplateElementCastParameters<TypeElement, Cast>(), function); }
   bool foreachSDo(std::function<bool (TypeElement&)> function) const
      {  return List::foreachDo(TemplateElementCastParameters<TypeElement, Cast>(), function); }
   bool foreachReverseDo(std::function<bool (const TypeElement&)> function) const
      {  return List::foreachReverseDo(TemplateElementCastParameters<TypeElement, Cast>(), function); }
   bool foreachSReverseDo(std::function<bool (TypeElement&)> function) const
      {  return List::foreachReverseDo(TemplateElementCastParameters<TypeElement, Cast>(), function); }
   DeclareCollectionIteratorForConcreteCollect

   class DescentCursor : public InheritedDescentCursor {
     private:
      typedef InheritedDescentCursor inherited;

     public:
      DescentCursor(const TParentTernaryTree<TypeSubString, TypeElement, Cast>& support) : inherited(support) {}
      DescentCursor(const DescentCursor& source) : inherited(source) {}
      DefineCopy(DescentCursor)

      TypeElement* getSElement() const
         {  return (TypeElement*) Cast::castFrom((typename Cast::Base*) inherited::_getSElement()); }
      const TypeElement* getElement() const { return getSElement(); }
      const TypeElement& elementAt() const { return *getSElement(); }
      TypeElement& elementSAt() const { return *getSElement(); }
      virtual bool isValid() const { return inherited::isValid(); }
   };
   friend class DescentCursor;
};

#undef DefTypeCastFromCollectHandler
#undef DefTypeCCastFromCollectHandler
#define DefTypeCastFromCollectHandler(Element)                                \
   (TypeElement*) Cast::castFrom((typename ParentTernaryTree<TypeSubString>::Node*) Element)
#define DefTypeCCastFromCollectHandler(Element)                               \
   (const TypeElement*) Cast::castFrom((const typename ParentTernaryTree<TypeSubString>::Node*) Element)

template<class TypeSubString, class TypeElement, class Cast=SimpleParentTernaryTreeCast<TypeSubString> >
class TParentTernaryTreeCursor : public ParentTernaryTreeCursor<TypeSubString> {
  private:
   typedef TParentTernaryTreeCursor<TypeSubString, TypeElement, Cast> thisType;
   typedef ParentTernaryTreeCursor<TypeSubString> inherited;

  protected:
   virtual EnhancedObject* _getSElement() const { return inherited::_getSElement(); }
   virtual void _gotoReference(const EnhancedObject& element)
      {  inherited::_gotoReference(element); }
   void _gotoReference(const typename TParentTernaryTree<TypeSubString, TypeElement, Cast>::Node& element)
      {  inherited::_gotoReference(element); }
   virtual bool _isPositionned(const COL::VirtualCollection::ExtendedLocateParameters& pos,
         const COL::VirtualCollectionCursor* cursor=nullptr) const
      {  return inherited::_isPositionned(pos, cursor); }
   
  public:
   TParentTernaryTreeCursor(const TParentTernaryTree<TypeSubString, TypeElement, Cast>& support) : inherited(support) {}
   TParentTernaryTreeCursor(const typename TParentTernaryTree<TypeSubString, TypeElement, Cast>::DescentCursor& descentCursor)
      : inherited(descentCursor) {}
   TParentTernaryTreeCursor(const thisType& source) = default;
   Template3DefineCopy(TParentTernaryTreeCursor, TypeSubString, TypeElement, Cast)
   Template3DefineCursorForAbstractCollect(TParentTernaryTree, TParentTernaryTreeCursor, TypeSubString, TypeElement, Cast)

   typedef typename inherited::Position Position;
   typedef typename inherited::ExtendedLocateParameters ExtendedLocateParameters;
#define DefCursor
   #include "Collection/ELMCollection.inch"
#undef DefCursor
};

#undef DefExcludeElement
#undef DefTypeKey
#undef DefTypeSorted
#undef DefTypeCursor
#undef DefTypeInheritedCollection
#undef DefTypeCastToCollectHandler
#undef DefTypeCCastToCollectHandler
#undef DefTypeCastFromCollectHandler
#undef DefTypeCCastFromCollectHandler
#undef DefTypeCollection
#undef DefTypeElement

Template3InlineCollectionForAbstractCollect(TParentTernaryTree, TParentTernaryTreeCursor, TypeSubString, TypeElement, Cast)
Template2InlineCollectionIteratorForConcreteCollect(TParentTernaryTree, TypeSubString, TypeElement, Cast)

} // end of namespace COL

#endif // COL_TernaryTreeInterfaceParentH

