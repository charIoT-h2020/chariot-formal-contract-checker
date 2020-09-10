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
// File      : TernaryTreeInterface.h
// Description :
//   Definition of a collection interface to the dictionaries based on ternary trees.
//

#ifndef COL_TernaryTreeInterfaceH
#define COL_TernaryTreeInterfaceH

#include "TString/TernaryTreeInterfaceBase.h"
#include "TString/TernaryTree.h"

namespace COL {

#define DefTemplate template <class TypeSubString>
#define DefTypeCollection TernaryTree<TypeSubString>
#define DefTypeCursor TernaryTreeCursor<TypeSubString>
#define DefTypeSorted
#define DefTypeKey const TypeSubString&

template <class TypeSubString>
class TernaryTreeCursor;

template <class TypeSubString>
class TernaryTree : public DTernaryTree::TTernaryTree<TypeSubString,
      STG::TImplTernaryTree<TypeSubString>, STG::DTernaryTree::TFollowingCell<TypeSubString> > {
  private:
   typedef DTernaryTree::TTernaryTree<TypeSubString, STG::TImplTernaryTree<TypeSubString>,
         STG::DTernaryTree::TFollowingCell<TypeSubString> > inherited;
   typedef DTernaryTree::TTernaryTreeCursor<TypeSubString, STG::TImplTernaryTree<TypeSubString>,
         STG::DTernaryTree::TFollowingCell<TypeSubString> > thisCursorType;
   typedef TernaryTree<TypeSubString> thisType;
   
   class LocationResultAccess : public inherited::LocationResult {
#ifdef __GNUC__
      LocationResultAccess(const TypeSubString& key) : LocationResult(key) {}
#endif
     public:
      friend class TernaryTree;
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
   
   class InsertionNotification;
   class SuppressionNotification;
   class SplitSuffixNotification;
   
   TemplateDefineCollectionForAbstractCollect(TernaryTree, TernaryTreeCursor, TypeSubString)

  protected:
#if !defined(_MSC_VER)
   friend class COL::DTernaryTree::BaseTernaryTreeMethods;
   friend class COL::DTernaryTree::BaseTernaryTree<TypeSubString>;
#else
  public:
   friend class BaseTernaryTreeMethods;
   friend class COL::DTernaryTree::MscBaseTernaryTree<TypeSubString>;
#endif
   void notifyCursorUpdate(CursorNotification& notification) const
      {  inherited::notifyCursorUpdate(notification); }
   void notifyCursorUpdate(CursorNotification& notification, const AbstractCursor& cursor) const
      {  inherited::notifyCursorUpdate(notification, cursor); }
   virtual ComparisonResult _compare(const EnhancedObject& asource) const override
      {  return inherited::_compare(asource); }
   virtual void _fullAssign(const VirtualCollection& source,
         const COL::VirtualCollection::ExtendedReplaceParameters& parameters) override
      {  inherited::_fullAssign(source, parameters); }
   void _fullAssign(const thisType& source, const ExtendedReplaceParameters& parameters);

      // query methods
   virtual typename DTernaryTree::BaseTernaryTree<TypeSubString>::LocationResult _locateKey(
         const TypeSubString& key, const COL::VirtualCollection::ExtendedLocateParameters& parameters,
         typename inherited::LocateCursor* cursor,
         const typename inherited::LocateCursor* start,
         const typename inherited::LocateCursor* end) const override
      {  return inherited::_locateKey(key, parameters, cursor, start, end); }
   LocationResult _locateKey(const TypeSubString& key, const ExtendedLocateParameters& parameters,
         Cursor* cursor=nullptr, const Cursor* start=nullptr, const Cursor* end=nullptr) const
      {  return inherited::_locateKey(key, parameters, (thisCursorType*) cursor, const_cast<thisCursorType*>((const thisCursorType*) start), const_cast<thisCursorType*>((const thisCursorType*) end)); }

   virtual TypeSubString _queryKey(const COL::VirtualCollection::ExtendedLocateParameters& parameters,
         const VirtualCollectionCursor* cursor) const override
      {  return inherited::_queryKey(parameters, cursor); }
   TypeSubString _queryKey(const ExtendedLocateParameters& parameters,
         const Cursor* cursor=nullptr) const
      {  return inherited::_queryKey(parameters, const_cast<thisCursorType*>((const thisCursorType*) cursor)); }

      // insertion methods
   virtual void _add(EnhancedObject* anObject,
         const COL::VirtualCollection::ExtendedInsertionParameters& parameters,
         VirtualCollectionCursor* cursor) override;
   void _add(EnhancedObject* newElement, const ExtendedInsertionParameters& parameters,
         Cursor* cursor=nullptr);
   virtual void _addAll(const VirtualCollection& source,
         const COL::VirtualCollection::ExtendedInsertionParameters& parameters,
         VirtualCollectionCursor* cursor, const VirtualCollectionCursor* startSource,
         const VirtualCollectionCursor* endSource) override;
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
   void _replace(EnhancedObject* element, const ExtendedReplaceParameters& parameters,
         Cursor* cursor=nullptr);

  public:
   TernaryTree() {}
   typedef typename inherited::InitialValues InitialValues;
   TernaryTree(const InitialValues& initialValues);
   typedef typename inherited::InitialNewValues InitialNewValues;
   TernaryTree(const InitialNewValues& initialValues);
   TernaryTree(const thisType& source,
         COL::VirtualCollection::AddMode duplicateMode=COL::VirtualCollection::AMNoDuplicate,
         const VirtualCast* retrieveRegistrationFromCopy=nullptr)
      : inherited(source, duplicateMode, retrieveRegistrationFromCopy) {}
   thisType& operator=(const thisType& source)
      {  thisType::_fullAssign(source, ExtendedReplaceParameters()); return *this; }

   TemplateDefineCopy(TernaryTree, TypeSubString)
   DTemplateDefineAssign(TernaryTree, TypeSubString)

   #define DefJustDeclare
   #define DefExcludeElement
   #include "Collection/ELMCollection.inch"
   #include "Collection/ELMMapCollection.inch"
   #undef DefExcludeElement
   #undef DefJustDeclare

   template <class Execute, class TemplateParameters>
   bool foreachDo(TemplateParameters params, Execute& fun,
         const COL::VirtualCollection::ExtendedLocateParameters& parameters,
         const TernaryTreeCursor<TypeSubString>* start=nullptr,
         const TernaryTreeCursor<TypeSubString>* end=nullptr) const
      {  return DTernaryTree::BaseTernaryTree<TypeSubString>::foreachDo((const inherited&) *this, params, fun,
            parameters, (const thisCursorType*) start, (const thisCursorType*) end);
      }
   template <class Execute, class TemplateParameters>
   bool foreachDo(TemplateParameters params, Execute& fun) const
      {  return DTernaryTree::BaseTernaryTree<TypeSubString>::foreachDo((const inherited&) *this, params, fun); }
   template <class Execute, class TemplateParameters>
   bool foreachReverseDo(TemplateParameters params, Execute& fun,
         const COL::VirtualCollection::ExtendedLocateParameters& parameters,
         const TernaryTreeCursor<TypeSubString>* start=nullptr,
         const TernaryTreeCursor<TypeSubString>* end=nullptr) const
      {  return DTernaryTree::BaseTernaryTree<TypeSubString>::foreachReverseDo((const inherited&) *this, params, fun,
            parameters, (const thisCursorType*) start, (const thisCursorType*) end);
      }
   template <class Execute, class TemplateParameters>
   bool foreachReverseDo(TemplateParameters params, Execute& fun) const
      {  return DTernaryTree::BaseTernaryTree<TypeSubString>::foreachReverseDo((const inherited&) *this, params, fun); }

   template <class Execute, class TemplateParameters>
   bool foreachKeyDo(TemplateParameters params, Execute& fun,
         const COL::VirtualCollection::ExtendedLocateParameters& parameters,
         const TernaryTreeCursor<TypeSubString>* start=nullptr,
         const TernaryTreeCursor<TypeSubString>* end=nullptr) const
      {  return DTernaryTree::BaseTernaryTree<TypeSubString>::foreachKeyDo((const inherited&) *this, params, fun,
            parameters, (const thisCursorType*) start, (const thisCursorType*) end);
      }
   template <class Execute, class TemplateParameters>
   bool foreachKeyDo(TemplateParameters params, Execute& fun) const
      {  return DTernaryTree::BaseTernaryTree<TypeSubString>::foreachKeyDo((const inherited&) *this, params, fun); }
   template <class Execute, class TemplateParameters>
   bool foreachKeyReverseDo(TemplateParameters params, Execute& fun,
         const COL::VirtualCollection::ExtendedLocateParameters& parameters,
         const TernaryTreeCursor<TypeSubString>* start=nullptr,
         const TernaryTreeCursor<TypeSubString>* end=nullptr) const
      {  return DTernaryTree::BaseTernaryTree<TypeSubString>::foreachKeyReverseDo((const inherited&) *this, params, fun,
            parameters, (const thisCursorType*) start, (const thisCursorType*) end);
      }
   template <class Execute, class TemplateParameters>
   bool foreachKeyReverseDo(TemplateParameters params, Execute& fun) const
      {  return DTernaryTree::BaseTernaryTree<TypeSubString>::foreachKeyReverseDo((const inherited&) *this, params, fun); }

   class DescentCursor : public DTernaryTree::TDescentTernaryTreeCursor<TypeSubString,
         STG::TImplTernaryTree<TypeSubString>, STG::DTernaryTree::TFollowingCell<TypeSubString> > {
     private:
      typedef DTernaryTree::TDescentTernaryTreeCursor<TypeSubString,
         STG::TImplTernaryTree<TypeSubString>,
         STG::DTernaryTree::TFollowingCell<TypeSubString> > inherited;

     public:
      DescentCursor(const TernaryTree<TypeSubString>& support) : inherited(support) {}
      DescentCursor(const DescentCursor& source) = default;
      DefineCopy(DescentCursor)
      
      EnhancedObject* getSElement() const { return inherited::_getSElement(); }
      const EnhancedObject* getElement() const { return getSElement(); }
      const EnhancedObject& elementAt() const { return *getSElement(); }
      EnhancedObject& elementSAt() const { return *getSElement(); }
   };
};

template <class TypeSubString>
class TernaryTreeCursor : public DTernaryTree::TTernaryTreeCursor<TypeSubString,
      STG::TImplTernaryTree<TypeSubString>, STG::DTernaryTree::TFollowingCell<TypeSubString> > {
  private:
   typedef DTernaryTree::TTernaryTreeCursor<TypeSubString,
         STG::TImplTernaryTree<TypeSubString>, STG::DTernaryTree::TFollowingCell<TypeSubString> > inherited;
   typedef TernaryTreeCursor<TypeSubString> thisType;

   friend class TernaryTree<TypeSubString>::InsertionNotification;
   friend class TernaryTree<TypeSubString>::SuppressionNotification;
   friend class TernaryTree<TypeSubString>::SplitSuffixNotification;
   typedef typename inherited::CursorImplementation CursorImplementation;

   class CursorImplementationAccess : public CursorImplementation {
#ifdef __GNUC__
      CursorImplementationAccess(const STG::TImplTernaryTree<TypeSubString>& tree) : CursorImplementation(tree) {}
#endif
     public:
      friend class TernaryTree<TypeSubString>;
   };

   friend class CursorImplementationAccess;

  public:
   TernaryTreeCursor(const TernaryTree<TypeSubString>& support) : inherited(support) {}
   TernaryTreeCursor(const typename TernaryTree<TypeSubString>::DescentCursor& descentCursor)
      : inherited(descentCursor, typename inherited::Descent()) {}
   TernaryTreeCursor(const thisType& source) = default;
   TemplateDefineCopy(TernaryTreeCursor, TypeSubString)
   TemplateDefineCursorForAbstractCollect(TernaryTree, TernaryTreeCursor, TypeSubString)

   typedef typename inherited::Position Position;
   typedef typename inherited::ExtendedLocateParameters ExtendedLocateParameters;
   #define DefCursor
   #define DefTypeFinal TernaryTreeCursor<TypeSubString>::
   #define DefExcludeElement
   #include "Collection/ELMCollection.inch"
   #undef DefExcludeElement
   #undef DefTypeFinal
   #undef DefCursor
};

TemplateInlineCollectionForAbstractCollect(TernaryTree, TernaryTreeCursor, TypeSubString)
   
/* Definition of the class TernaryTree::InsertionNotification */

template <class TypeSubString>
class TernaryTree<TypeSubString>::InsertionNotification
   :  public TernaryTree<TypeSubString>::CursorNotification,
      public STG::TImplTernaryTree<TypeSubString>::InsertionNotification {
  private:
   typedef typename STG::TImplTernaryTree<TypeSubString>::InsertionNotification inheritedImplementation;
   typedef typename TernaryTree<TypeSubString>::CursorNotification inherited;

  protected:
   void update(TernaryTreeCursor<TypeSubString>& cursor)
      {  inheritedImplementation::apply(cursor.getImplementation()); }

  public:
   InsertionNotification() {}
   InsertionNotification(const InsertionNotification& source) = default;
   InsertionNotification& operator=(const InsertionNotification& source) = default;
   DefineCursorNotificationMethods(InsertionNotification, TernaryTreeCursor<TypeSubString>)
   virtual bool isValid() const override
      {  return inherited::isValid() && inheritedImplementation::isValid(); }
};

template <class TypeSubString>
class TernaryTree<TypeSubString>::SuppressionNotification
   :  public TernaryTree<TypeSubString>::CursorNotification,
      public STG::TImplTernaryTree<TypeSubString>::SuppressionNotification {
  private:
   typedef typename STG::TImplTernaryTree<TypeSubString>::SuppressionNotification inheritedImplementation;
   typedef typename TernaryTree<TypeSubString>::CursorNotification inherited;

  protected:
   void update(TernaryTreeCursor<TypeSubString>& cursor)
      {  inheritedImplementation::apply(cursor.getImplementation()); }

  public:
   SuppressionNotification() {}
   SuppressionNotification(const SuppressionNotification& source) = default;
   DefineCursorNotificationMethods(SuppressionNotification, TernaryTreeCursor<TypeSubString>)
   virtual bool isValid() const override
      {  return inherited::isValid() && inheritedImplementation::isValid(); }
};

template <class TypeSubString>
class TernaryTree<TypeSubString>::SplitSuffixNotification
   :  public TernaryTree<TypeSubString>::CursorNotification,
      public STG::TImplTernaryTree<TypeSubString>::SplitSuffixNotification {
  private:
   typedef typename STG::TImplTernaryTree<TypeSubString>::SplitSuffixNotification inheritedImplementation;
   typedef typename TernaryTree<TypeSubString>::CursorNotification inherited;
   typedef typename TernaryTree<TypeSubString>::FollowingCell FollowingCell;

  protected:
   void update(TernaryTreeCursor<TypeSubString>& cursor)
      {  inheritedImplementation::applyFinal(cursor.getImplementation()); }

  public:
   SplitSuffixNotification(FollowingCell& oldReference) : inheritedImplementation(oldReference) {}
   SplitSuffixNotification(const SplitSuffixNotification& source) = default;
   DefineCursorNotificationMethods(SplitSuffixNotification, TernaryTreeCursor<TypeSubString>)
   virtual bool isValid() const override
      {  return inherited::isValid() && inheritedImplementation::isValid(); }
};

/* Définition des méthodes inlinées de la classe TernaryTree */

template <class TypeSubString>
inline void
TernaryTree<TypeSubString>::_add(EnhancedObject* anObject,
      const COL::VirtualCollection::ExtendedInsertionParameters& parameters,
      VirtualCollectionCursor* cursor)
   { inherited::_add(anObject, parameters, (thisCursorType*) cursor); }

template <class TypeSubString>
inline void
TernaryTree<TypeSubString>::_addAll(const VirtualCollection& source,
      const COL::VirtualCollection::ExtendedInsertionParameters& parameters,
      VirtualCollectionCursor* cursor, const VirtualCollectionCursor* startSource,
      const VirtualCollectionCursor* endSource) {
   AssumeCondition(dynamic_cast<const inherited*>(&source))
   if (dynamic_cast<const thisType*>(&source))
      _addAll((const thisType&) source, parameters, (Cursor*) cursor,
            (const Cursor*) startSource, (const Cursor*) endSource);
   else
      inherited::paddAll((const inherited&) source, parameters, (thisCursorType*) cursor,
            (const typename inherited::interfaceType::Cursor*) startSource,
            (const typename inherited::interfaceType::Cursor*) endSource);
}

template <class TypeSubString>
inline void
TernaryTree<TypeSubString>::_remove(const COL::VirtualCollection::ExtendedSuppressParameters& parameters,
      VirtualCollectionCursor* cursor)
   { inherited::_remove(parameters, (thisCursorType*) cursor); }

template <class TypeSubString>
inline void
TernaryTree<TypeSubString>::_replace(EnhancedObject* anObject,
      const COL::VirtualCollection::ExtendedReplaceParameters& parameters,
      VirtualCollectionCursor* cursor)
   { inherited::_replace(anObject, parameters, (thisCursorType*) cursor); }

#define DefTypeFinal TernaryTree::
#define DefJustInline
#define DefExcludeElement
#include "Collection/ELMCollection.inch"
#include "Collection/ELMMapCollection.inch"
#undef DefExcludeElement
#undef DefJustInline
#undef DefTypeFinal

#undef DefTemplate
#undef DefTypeCollection
#undef DefTypeCursor
#undef DefTypeSorted
#undef DefTypeKey

/*************************************************/
/* Definition of the template class TTernaryTree */
/*************************************************/

template<class TypeSubString, class TypeElement, class Cast>
class TTernaryTreeCursor;

#define DefTypeElement TypeElement
#define DefTypeCollection TTernaryTree<TypeSubString, TypeElement, Cast>
#define DefTypeCastToCollectHandler Cast::castTo
#define DefTypeCCastToCollectHandler Cast::castTo
#define DefTypeCastFromCollectHandler (TypeElement*) Cast::castFrom
#define DefTypeCCastFromCollectHandler (const TypeElement*) Cast::castFrom
#define DefTypeInheritedCollection TernaryTree<TypeSubString>
#define DefTypeCursor TTernaryTreeCursor<TypeSubString, TypeElement, Cast>
#define DefTypeSorted
#define DefTypeKey const TypeSubString&
#define DefExcludeElement

template<class TypeSubString, class TypeElement, class Cast=SimpleCast>
class TTernaryTree : public TernaryTree<TypeSubString> {
  public:
   typedef TypeSubString SubStringKey;
   typedef TypeElement Node;
   typedef COL::DVirtualMapCollection::Access::TMapInitialValues<TypeElement,
         COL::LightCopyKeyTraits<TypeSubString>, Cast> InitialValues;
   typedef COL::DVirtualMapCollection::Access::TMapInitialNewValues<TypeElement,
         COL::LightCopyKeyTraits<TypeSubString>, Cast> InitialNewValues;

  private:
   typedef TTernaryTree<TypeSubString, TypeElement, Cast> thisType;
   typedef TTernaryTreeCursor<TypeSubString, TypeElement, Cast> thisCursorType;
   typedef TernaryTree<TypeSubString> inherited;

  protected:
   virtual bool acceptElement(const EnhancedObject& aelement) const override
      {  return dynamic_cast<const TypeElement*>(Cast::castFrom(&aelement));  }
   typedef typename inherited::DescentCursor InheritedDescentCursor;
   virtual VirtualSortedCollection::LocationResult _locateKey(const TypeSubString& key,
         const COL::VirtualCollection::ExtendedLocateParameters& parameters,
         typename inherited::LocateCursor* cursor,
         const typename inherited::LocateCursor* start,
         const typename inherited::LocateCursor* end) const override
      {  return inherited::_locateKey(key, parameters, cursor, start, end); }
   typename inherited::LocationResult _locateKey(const TypeSubString& key, const typename inherited::ExtendedLocateParameters& parameters,
         typename inherited::Cursor* cursor=nullptr, const typename inherited::Cursor* start=nullptr, const typename inherited::Cursor* end=nullptr) const
      {  return inherited::_locateKey(key, parameters, cursor, start, end); }

   virtual TypeSubString _queryKey(const typename inherited::ExtendedLocateParameters& parameters,
         const VirtualCollectionCursor* cursor) const override
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
   
   TTernaryTree() {}
   TTernaryTree(const InitialValues& initialValues)
      {  COL::DVirtualMapCollection::Access::TMapInsertInitialValue<
            TTernaryTree<TypeSubString, TypeElement, Cast>, TypeElement,
            COL::LightCopyKeyTraits<TypeSubString>, Cast> insertInitialValue(*this);
         initialValues.foreachDo(insertInitialValue);
      }
   TTernaryTree(const InitialNewValues& initialValues)
      {  COL::DVirtualMapCollection::Access::TMapInsertInitialNewValue<
            TTernaryTree<TypeSubString, TypeElement, Cast>, TypeElement,
            COL::LightCopyKeyTraits<TypeSubString>, Cast> insertInitialValue(*this);
         initialValues.foreachDo(insertInitialValue);
      }
   TTernaryTree(const thisType& source,
         COL::VirtualCollection::AddMode duplicateMode=COL::VirtualCollection::AMNoDuplicate,
         const VirtualCast* retrieveRegistrationFromCopy=nullptr)
      :  inherited(source, duplicateMode, retrieveRegistrationFromCopy) {}
   Template3DefineCopy(TTernaryTree, TypeSubString, TypeElement, Cast)
   Template3DefineCollectionForAbstractCollect(TTernaryTree, TTernaryTreeCursor, TypeSubString, TypeElement, Cast)

   #include "Collection/ELMCollection.inch"
   #include "Collection/ELMMapCollection.inch"

   class VirtualExecute {
     public:
      virtual ~VirtualExecute() {}
      virtual bool operator()(const TypeSubString& key, TypeElement& source) = 0;
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
      DescentCursor(const TTernaryTree<TypeSubString, TypeElement, Cast>& support) : inherited(support) {}
      DescentCursor(const DescentCursor& source) = default;
      DefineCopy(DescentCursor)

      TypeElement* getSElement() const { return (TypeElement*) Cast::castFrom(inherited::_getSElement()); }
      const TypeElement* getElement() const { return getSElement(); }
      const TypeElement& elementAt() const { return *getSElement(); }
      TypeElement& elementSAt() const { return *getSElement(); }
   };
   friend class DescentCursor;
};

template<class TypeSubString, class TypeElement, class Cast=SimpleCast>
class TTernaryTreeCursor : public TernaryTreeCursor<TypeSubString> {
  private:
   typedef TTernaryTreeCursor<TypeSubString, TypeElement, Cast> thisType;
   typedef TernaryTreeCursor<TypeSubString> inherited;

  protected:
   virtual EnhancedObject* _getSElement() const override { return inherited::_getSElement(); }
   virtual void _gotoReference(const EnhancedObject& element) override
      {  inherited::_gotoReference(element); }
   void _gotoReference(const typename TTernaryTree<TypeSubString, TypeElement, Cast>::Node& element)
      {  inherited::_gotoReference(element); }
   virtual bool _isPositionned(const COL::VirtualCollection::ExtendedLocateParameters& pos,
         const COL::VirtualCollectionCursor* cursor=nullptr) const override
      {  return inherited::_isPositionned(pos, cursor); }
   
  public:
   typedef typename inherited::ExtendedLocateParameters ExtendedLocateParameters;
   typedef typename inherited::Position Position;
   
   TTernaryTreeCursor(const TTernaryTree<TypeSubString, TypeElement, Cast>& support) : inherited(support) {}
   TTernaryTreeCursor(const typename TTernaryTree<TypeSubString, TypeElement, Cast>::DescentCursor& descentCursor)
      :  inherited(descentCursor) {}
   TTernaryTreeCursor(const thisType& source) = default;
   Template3DefineCopy(TTernaryTreeCursor, TypeSubString, TypeElement, Cast)
   Template3DefineCursorForAbstractCollect(TTernaryTree, TTernaryTreeCursor, TypeSubString, TypeElement, Cast)

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

Template3InlineCollectionForAbstractCollect(TTernaryTree, TTernaryTreeCursor, TypeSubString, TypeElement, Cast)
Template2InlineCollectionIteratorForConcreteCollect(TTernaryTree, TypeSubString, TypeElement, Cast)

} // end of namespace COL

#endif // COL_TernaryTreeInterfaceH
