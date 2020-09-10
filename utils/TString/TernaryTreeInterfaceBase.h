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
// File      : TernaryTreeInterfaceBase.h
// Description :
//   Definition of common utilities to the collection dictionaries based on ternary trees.
//

#ifndef COL_TernaryTreeInterfaceBaseH
#define COL_TernaryTreeInterfaceBaseH

#include "Collection/VirtualCollection/VirtualMapCollection.h"
#include "TString/TernaryTreeBase.h"

namespace COL { namespace DTernaryTree {

class BaseTernaryTreeMethods {
  public:
   // The template part of the tremove method comes from the special behavior of SuppressionNotification
   template <class TypeTree>
   static void tremove(TypeTree& thisTree,
         const COL::VirtualCollection::ExtendedSuppressParameters& parameters,
         typename TypeTree::Cursor* cursor);
   // The template part of the treplace method mainly comes TypeNode
   template <class TypeTree, class TypeNode>
   static void treplace(TypeTree& thisTree, TypeNode* element,
         const COL::VirtualCollection::ExtendedReplaceParameters& parameters,
         typename TypeTree::Cursor* cursor);

   template <class TypeTree, class Execute, class TemplateParameters>
   static bool foreachKeyDo(const TypeTree& tree, TemplateParameters, Execute& fun,
         const COL::VirtualCollection::ExtendedLocateParameters& parameters, const typename TypeTree::Cursor* start=nullptr,
         const typename TypeTree::Cursor* end=nullptr);
   template <class TypeTree, class Execute, class TemplateParameters>
   static bool foreachKeyDo(const TypeTree& thisTree, TemplateParameters, Execute& fun);
   template <class TypeTree, class Execute, class TemplateParameters>
   static bool foreachKeyReverseDo(const TypeTree& tree, TemplateParameters, Execute& fun,
         const COL::VirtualCollection::ExtendedLocateParameters& parameters, const typename TypeTree::Cursor* start=nullptr,
         const typename TypeTree::Cursor* end=nullptr);
   template <class TypeTree, class Execute, class TemplateParameters>
   static bool foreachKeyReverseDo(const TypeTree& thisTree, TemplateParameters, Execute& fun);
   
   template <class TypeTree, class Execute, class TemplateParameters>
   static bool foreachDo(const TypeTree& tree, TemplateParameters, Execute& fun,
         const COL::VirtualCollection::ExtendedLocateParameters& parameters, const typename TypeTree::Cursor* start=nullptr,
         const typename TypeTree::Cursor* end=nullptr);
   template <class TypeTree, class Execute, class TemplateParameters>
   static bool foreachDo(const TypeTree& thisTree, TemplateParameters, Execute& fun);
   template <class TypeTree, class Execute, class TemplateParameters>
   static bool foreachReverseDo(const TypeTree& tree, TemplateParameters, Execute& fun,
         const COL::VirtualCollection::ExtendedLocateParameters& parameters, const typename TypeTree::Cursor* start=nullptr,
         const typename TypeTree::Cursor* end=nullptr);
   template <class TypeTree, class Execute, class TemplateParameters>
   static bool foreachReverseDo(const TypeTree& thisTree, TemplateParameters, Execute& fun);
};

template <class TypeSubString>
class BaseTernaryTree : public COL::TVirtualMapCollection<COL::LightCopyKeyTraits<TypeSubString> >,
      public BaseTernaryTreeMethods {
  protected:
   typedef COL::LightCopyKeyTraits<TypeSubString> SubStringKeyTraits;
  public:
   class Key {
     public:
      typedef const STG::SubString& KeyType;
      typedef STG::SubString ControlKeyType;
      static ComparisonResult compare(KeyType fstKey, KeyType sndKey) { return fstKey.compare(sndKey); }
      static const STG::SubString& key(const EnhancedObject&)
         {  AssumeUncalled const STG::SubString* result = nullptr; return *result; }
      static const STG::SubString& copy(const ControlKeyType& key) { return key; }
      static const STG::SubString& store(KeyType key) { return key; }
   };

  private:
   typedef COL::TVirtualMapCollection<SubStringKeyTraits> inherited;
   typedef BaseTernaryTree<TypeSubString> thisType;
   mutable int uNodeCursors;
   
  protected:
   typedef typename COL::TVirtualMapCollection<SubStringKeyTraits>::Cursor LocateCursor;
   typedef COL::TVirtualMapCollection<SubStringKeyTraits> interfaceType;
   const int& getNodeCursors() const { return uNodeCursors; }

  protected:
   class NormalizeNotification;

  public:
   BaseTernaryTree() : uNodeCursors(0) {}
   BaseTernaryTree(const thisType& source,
         COL::VirtualCollection::AddMode duplicateMode=COL::VirtualCollection::AMNoDuplicate)
      :  inherited(source, duplicateMode), uNodeCursors(0) {}
   BaseTernaryTree& operator=(const thisType& source)
      {  inherited::operator=(source);
         if (this != &source)
            uNodeCursors = 0;
         return *this;
      }

   class Cursor : public COL::TVirtualMapCollectionCursor<SubStringKeyTraits>, public ExtendedParameters {
     protected:
      typedef COL::LightCopyKeyTraits<TypeSubString> SubStringKeyTraits;

     private:
      typedef COL::TVirtualMapCollectionCursor<SubStringKeyTraits> inherited;

     protected:
      enum Mode { MExact, MLeft, MRight };
      DefineExtendedParameters(2, ExtendedParameters)
      void setIntermediary(Mode mode) { setOwnField(mode); }
      bool isExactCursor() const { return queryOwnField() == MExact; }
      bool isNodeCursor() const { return queryOwnField() != MExact; }
      virtual void _setFinal() { clearOwnField(); }
      void setTreeMode(const BaseTernaryTree<TypeSubString>& tree, ImplBinaryNode::Balance balance)
         {  AssumeCondition(balance != ImplBinaryNode::BAll)
            tree.uNodeCursors 
               += (((balance != ImplBinaryNode::BEqual) ? 1 : 0)
                     + ((queryOwnField() != MExact) ? 1 : 0));
            setOwnField((balance == ImplBinaryNode::BLeft) ? MLeft
                  : ((balance == ImplBinaryNode::BRight) ? MRight : MExact));
         }
      void releaseMode(const BaseTernaryTree<TypeSubString>& tree)
         {  if (queryOwnField() != MExact) {
               --tree.uNodeCursors;
               clearOwnField();
             };
         }
      bool isLeftCursor() const { return queryOwnField() == MLeft; }
      bool isRightCursor() const { return queryOwnField() == MRight; }
      friend class NormalizeNotification;
      friend class BaseTernaryTree<TypeSubString>;
      friend class BaseTernaryTreeMethods;

      virtual ComparisonResult _compare(const EnhancedObject& asource) const override
         {  return inherited::_compare(asource); }
      Mode getMode() const { return (Mode) queryOwnField(); }

     public:
      Cursor(const BaseTernaryTree<TypeSubString>& support) : inherited(support) {}
      Cursor(const Cursor& source) : inherited(source), ExtendedParameters()
         {  mergeOwnField(source.queryOwnField());
            if (inherited::hasSupport() && hasOwnField())
               (const_cast<BaseTernaryTree<TypeSubString>&>((const BaseTernaryTree<TypeSubString>&) inherited::getSupport())).uNodeCursors++;
         }
      Cursor& operator=(const Cursor& source)
         {  inherited::operator=(source);
            if (inherited::hasSupport() && hasOwnField())
               (const_cast<BaseTernaryTree<TypeSubString>&>((const BaseTernaryTree<TypeSubString>&) inherited::getSupport())).uNodeCursors--;
            setOwnField(source.queryOwnField());
            if (inherited::hasSupport() && hasOwnField())
               (const_cast<BaseTernaryTree<TypeSubString>&>((const BaseTernaryTree<TypeSubString>&) inherited::getSupport())).uNodeCursors++;
            return *this;
         }
      virtual ~Cursor()
         {  if (inherited::hasSupport() && hasOwnField())
               (const_cast<BaseTernaryTree<TypeSubString>&>((const BaseTernaryTree<TypeSubString>&) inherited::getSupport())).uNodeCursors--;
         }
      DefineCopy(Cursor)
      DDefineAssign(Cursor)
      virtual bool isValid() const override { return inherited::isValid(); }
   };
   friend class BaseTernaryTreeMethods;
   friend class BaseTernaryTree<TypeSubString>::Cursor;

  protected:
   typedef typename inherited::CursorNotification CursorNotification;
   class NormalizeNotification : public CursorNotification {
     private:
      typedef CursorNotification inherited;

     protected:
      void update(Cursor& cursor)
         {  if (cursor.hasOwnField())
               cursor._setFinal();
         }

     public:
      NormalizeNotification() {}
      NormalizeNotification(const NormalizeNotification& source) : inherited(source) {}
      DefineCursorNotificationMethods(NormalizeNotification, Cursor)
   };
   friend class NormalizeNotification;

   void normalizeCursors(const Cursor* cursor=nullptr)
      {  int lockCursor = cursor ? (cursor->isNodeCursor() ? 1 : 0) : 0;
         if (uNodeCursors > lockCursor) {
            NormalizeNotification normalization;
            if (cursor)
               normalization.setOrigin(*cursor);
            inherited::notifyCursorUpdate(normalization);
            uNodeCursors = lockCursor;
         };
      }
};

#ifdef _MSC_VER
template <class TypeSubString>
class MscBaseTernaryTree : public BaseTernaryTree<TypeSubString> {
  public:
   MscBaseTernaryTree() {}
   MscBaseTernaryTree(const MscBaseTernaryTree<TypeSubString>& source, AddMode duplicateMode=AMNoDuplicate)
      : BaseTernaryTree<TypeSubString>(source, duplicateMode) {}
};
#endif

template <class TypeSubString, class TypeLocationResult>
class TKeyHandler : public DVirtualCollection::TKeyHandler<COL::LightCopyKeyTraits<TypeSubString> > {
  private:
   typedef COL::LightCopyKeyTraits<TypeSubString> SubStringKeyTraits;
   typedef DVirtualCollection::TKeyHandler<SubStringKeyTraits> inherited;
   TypeLocationResult lrLocation;

  protected:
   TemplateDefineExtendedParameters(1, inherited)
   void setOpen() { mergeOwnField(1); }
       
  public:
   TKeyHandler(const TypeSubString& key)
      :  inherited(key), lrLocation(key) { inherited::setOpen(); }
   TKeyHandler(const TKeyHandler<TypeSubString, TypeLocationResult>& source)
      :  inherited(source), lrLocation(source.lrLocation) { inherited::setOpen(); }

   bool hasLocate() const { return inherited::isOpen(); }
   TypeLocationResult& getLocation()
      {  AssumeCondition(inherited::isOpen()) return lrLocation; }
   const TypeLocationResult& getLocation() const
      {  AssumeCondition(inherited::isOpen()) return lrLocation; }
   void setLocation(const TypeLocationResult& location)
      {  AssumeCondition(inherited::isOpen()) lrLocation = location; }

   bool isClosed() const { return !hasOwnField(); }
   bool isOpen() const { return hasOwnField(); }
};

template <class TypeSubString, class TypeBase, class TypeLocationResult>
class TMapParameters : public TypeBase, public TKeyHandler<TypeSubString, TypeLocationResult> {
  private:
   typedef TMapParameters<TypeSubString, TypeBase, TypeLocationResult> thisType;
   typedef TypeBase inherited;
   typedef TKeyHandler<TypeSubString, TypeLocationResult> inheritedKey;

  public:
   TMapParameters(const TypeSubString& key) : inheritedKey(key) { TypeBase::setMap(); }
   TMapParameters(const TypeBase& source, const TypeSubString& key)
      : inherited(source), inheritedKey(key) { TypeBase::setMap(); }
   TMapParameters(const thisType& source) = default;
   thisType& operator=(const thisType& source) { return (thisType&) inherited::operator=(source); }
   Template3DefineCopy(TMapParameters, TypeSubString, TypeBase, TypeLocationResult)
   DTemplate3DefineAssign(TMapParameters, TypeSubString, TypeBase, TypeLocationResult)

   TMapParameters<TypeSubString, TypeBase, TypeLocationResult>& setLocation(const TypeLocationResult& location)
      {  inheritedKey::setLocation(location); return *this; }
   virtual const DVirtualCollection::VirtualKeyHandler& getKeyHandler() const override
      {  return (const inheritedKey&) *this; }
};

/*************************************************/
/* Definition of the template class TTernaryTree */
/*************************************************/

template <class TypeSubString, class TypeImplTree, class TypeFollowingCell>
class TTernaryTreeCursor;

template <class TypeSubString, class TypeImplTree, class TypeFollowingCell>
class TTernaryTree : public BaseTernaryTree<TypeSubString>, protected TypeImplTree, public STG::DTernaryTree::InterfaceConstants {
  public:
   typedef TTernaryTreeCursor<TypeSubString, TypeImplTree, TypeFollowingCell> Cursor;
   typedef TypeSubString SubStringKey;

  private:
   typedef COL::DTernaryTree::BaseTernaryTree<TypeSubString> inherited;
   typedef TTernaryTree<TypeSubString, TypeImplTree, TypeFollowingCell> thisType;

  protected:
   typedef TypeImplTree inheritedImplementation;
   typedef TTernaryTreeCursor<TypeSubString, TypeImplTree, TypeFollowingCell> thisCursorType;
   typedef ::STG::DTernaryTree::BaseTernaryTree Base;

   void positionCursor(typename inheritedImplementation::Cursor& cursor,
         const typename inherited::ExtendedLocateParameters& parameters,
         const thisCursorType* sourceCursor, TypeSubString* key=nullptr) const;

   friend class BaseTernaryTreeMethods;

  public:
   class InheritedLocationResultAccess;
#if !defined(_MSC_VER) || (_MSC_VER < 1310)
   class ExtendedMapInsertionParameters;
#endif
   class LocationResult : public STG::DTernaryTree::InterfaceConstants {
     private:
      typedef LocationResult thisType;
   
      LocationSearchResult lsrResult;
      TypeFollowingCell* pfcNode;
      TypeSubString ssInsertionKey;
      const TypeSubString& ssKey;

      class CursorImplementation : public TypeImplTree::Cursor {
        public:
#ifdef __GNUC__
         CursorImplementation(const TypeImplTree& implTree) : TypeImplTree::Cursor(implTree) {}
#endif
         TypeFollowingCell* getReference() const
            {  return TypeImplTree::Cursor::getReference(); }
      };

      friend class TTernaryTree<TypeSubString, TypeImplTree, TypeFollowingCell>;

     protected:
      void setSearchResult(LocationSearchResult searchResult) { lsrResult = searchResult; }
      void setBinarySearchResult(LocationSearchResult searchResult)
         {  lsrResult = (searchResult == LSRExact) ? LSRExact : LSRUndefined; }
      void setNodeCursor(typename TypeImplTree::Cursor& cursor)
         {  pfcNode = ((CursorImplementation&) cursor).getReference(); }
      TypeSubString& getInsertionKey() { return ssInsertionKey; }
      const TypeSubString& getInsertionKey() const { return ssInsertionKey; }
      bool isOnSuffix() const
         {  AssumeCondition(lsrResult != LSRUndefined)
            return (lsrResult == LSRLess) || (lsrResult == LSRGreater) || (lsrResult == LSRNewSuffix);
         }
      bool isStrictLess() const { return lsrResult == LSRLess; }
      bool isLess() const { return (lsrResult == LSRLess) || (lsrResult == LSRLessSuffix); }
      bool isStrictGreater() const { return lsrResult == LSRGreater; }
      bool isGreater() const { return (lsrResult == LSRGreater) || (lsrResult == LSRGreaterSuffix); }
      bool isExact() const { return lsrResult == LSRExact; }
      bool isDifferent() const { return (lsrResult != LSRExact) && (lsrResult != LSRUndefined); }
      const LocationSearchResult& getSearchResult() const { return lsrResult; }
      bool doesKnowCursor(typename TypeImplTree::Cursor& cursor) const
         {  return cursor.isValid()
               && (((CursorImplementation&) cursor).getReference() == pfcNode);
         }
      const TypeSubString& getInitialKey() const { return ssKey; }

     public:
      LocationResult(const TypeSubString& key)
         :  lsrResult(LSRUndefined), pfcNode(nullptr), ssInsertionKey(key), ssKey(key) {}
      LocationResult(const thisType& source) = default;
      thisType& operator=(const thisType& source)
         {  AssumeCondition(&ssKey == &source.ssKey);
            lsrResult = source.lsrResult;
            pfcNode = source.pfcNode;
            ssInsertionKey = source.ssInsertionKey;
            return *this;
         }
      bool isValid() const { return (pfcNode != nullptr) && (lsrResult != LSRUndefined); }
      operator bool() const { return lsrResult == LSRExact; }
#ifndef _MSC_VER
      ExtendedMapInsertionParameters queryInsertionParameters();
#else
      ExtendedMapInsertionParameters queryInsertionParameters()
         {  COL::VirtualCollection::RelativePosition pos = (isLess()
               ? COL::VirtualCollection::RPBefore : (isGreater()
               ? COL::VirtualCollection::RPAfter : COL::VirtualCollection::RPUndefined));
            return (const TTernaryTree::ExtendedMapInsertionParameters&)
               TTernaryTree::ExtendedMapInsertionParameters(
                  getInitialKey()).setLocation(*this).setRelativePosition(pos);
         }
#endif
      friend class InheritedLocationResultAccess;
      InheritedLocationResultAccess asSortedLocation() const;
   };

  protected:
   typedef LocationResult OwnLocationResult;

  public:
   class InheritedLocationResultAccess : public VirtualSortedCollection::LocationResult {
     private:
      typedef VirtualSortedCollection::LocationResult inherited;
      
     public:
      InheritedLocationResultAccess(const OwnLocationResult& result)
#ifndef __GNUC__
         :  inherited(result.isExact() ? COL::VirtualCollection::RPExact : (result.isLess()
                                       ? COL::VirtualCollection::RPBefore : (result.isGreater()
                                       ? COL::VirtualCollection::RPAfter : COL::VirtualCollection::RPUndefined)))
#else
         :  inherited(result.isExact() ? (COL::VirtualCollection::RelativePosition) 2 : (result.isLess()
                                       ? (COL::VirtualCollection::RelativePosition) 1 : (result.isGreater()
                                       ? (COL::VirtualCollection::RelativePosition) 3 : (COL::VirtualCollection::RelativePosition) 0)))
#endif
         {  if (!result.isExact()) {
               ExtendedMapInsertionParameters* insertionParameters
                  = new ExtendedMapInsertionParameters(result.getInitialKey());
               insertion().absorbElement(insertionParameters);
               insertionParameters->setLocation(result);
            };
         }
      InheritedLocationResultAccess(const InheritedLocationResultAccess& source) = default;
   };

  public:
   typedef TKeyHandler<TypeSubString, LocationResult> KeyHandler;
   typedef TMapParameters<TypeSubString, VirtualCollection::ExtendedLocateParameters, LocationResult>
         ExtendedMapLocateParameters;
   typedef TMapParameters<TypeSubString, VirtualCollection::ExtendedSuppressParameters, LocationResult>
         ExtendedMapSuppressParameters;
   typedef TMapParameters<TypeSubString, VirtualCollection::ExtendedReplaceParameters, LocationResult>
         ExtendedMapReplaceParameters;
   class ExtendedMapInsertionParameters
      :  public TMapParameters<TypeSubString, VirtualCollection::ExtendedInsertionParameters, LocationResult> {
     private:
      typedef TMapParameters<TypeSubString, VirtualCollection::ExtendedInsertionParameters, LocationResult> inherited;

     public:
      ExtendedMapInsertionParameters(const TypeSubString& key) : inherited(key) {}
      ExtendedMapInsertionParameters(const VirtualCollection::ExtendedInsertionParameters& source,
            const TypeSubString& key)
         :  inherited(source, key) {}
      ExtendedMapInsertionParameters(const ExtendedMapInsertionParameters& source) = default;
      DefineCopy(ExtendedMapInsertionParameters)

      ExtendedMapInsertionParameters& setLocation(const LocationResult& lrLocation)
         { return (ExtendedMapInsertionParameters&) inherited::setLocation(lrLocation); }
   };
 
  protected:
   virtual ComparisonResult _compare(const EnhancedObject& asource) const override
      {  return inherited::_compare(asource); }

   virtual void _fullAssign(const COL::VirtualCollection& source,
         const COL::VirtualCollection::ExtendedReplaceParameters& parameters) override
      {  COL::VirtualCollection::pfullAssign(source, parameters); }

      // query methods
   virtual typename inherited::LocationResult _locateKey(const TypeSubString& key,
         const COL::VirtualCollection::ExtendedLocateParameters& parameters,
         typename inherited::LocateCursor* cursor,
         const typename inherited::LocateCursor* start,
         const typename inherited::LocateCursor* end) const override;
   LocationResult _locateKey(const TypeSubString& key,
         const COL::VirtualCollection::ExtendedLocateParameters& parameters,
         thisCursorType* cursor=nullptr, const thisCursorType* start=nullptr,
         const thisCursorType* end=nullptr) const;

   virtual TypeSubString _queryKey(const COL::VirtualCollection::ExtendedLocateParameters& parameters,
         const COL::VirtualCollectionCursor* cursor) const override;
   TypeSubString _queryKey(const COL::VirtualCollection::ExtendedLocateParameters& parameters,
         const thisCursorType* cursor=nullptr) const;

      // move methods
   virtual void _moveTo(COL::VirtualCollection& destination,
         const COL::VirtualCollection::ExtendedReplaceParameters& parameters,
         COL::VirtualCollectionCursor* cursor, COL::VirtualCollectionCursor* destinationCursor) override;
   void _moveTo(thisType& destination, const COL::VirtualCollection::ExtendedReplaceParameters& parameters,
         thisCursorType* cursor=nullptr, thisCursorType* destinationCursor=nullptr);

   virtual void _moveAllTo(COL::VirtualCollection& destination,
         const COL::VirtualCollection::ExtendedReplaceParameters& parameters,
         const COL::VirtualCollectionCursor* startCursor=nullptr, const COL::VirtualCollectionCursor* endCursor=nullptr,
         COL::VirtualCollectionCursor* destinationCursor=nullptr) override
      {  COL::VirtualCollection::pmoveAllTo(destination, parameters, startCursor, endCursor, destinationCursor); }
   virtual void _swap(COL::VirtualCollection& source) override
      {  COL::VirtualCollection::InvalidateNotification invalidation;
         inherited::notifyCursorUpdate(invalidation);
         ((thisType&) source).notifyCursorUpdate(invalidation);
         inheritedImplementation::swap((thisType&) source);
      }

      // remove methods
   virtual void _removeAll(const COL::VirtualCollection::ExtendedSuppressParameters& parameters,
         const COL::VirtualCollectionCursor* start, const COL::VirtualCollectionCursor* end) override;
   void _removeAll(const COL::VirtualCollection::ExtendedSuppressParameters& parameters,
         const thisCursorType* start=nullptr, const thisCursorType* end=nullptr);

      // query methods
   virtual int _queryCount(const COL::VirtualCollection::ExtendedLocateParameters& parameters,
         const COL::VirtualCollectionCursor* start, const COL::VirtualCollectionCursor* end) const override;
   int _queryCount(const COL::VirtualCollection::ExtendedLocateParameters& parameters,
         const thisCursorType* start=nullptr, const thisCursorType* end=nullptr) const;
   virtual EnhancedObject* _getElement(const COL::VirtualCollection::ExtendedLocateParameters& parameters,
         const COL::VirtualCollectionCursor* cursor) const override;
   EnhancedObject* _getElement(const COL::VirtualCollection::ExtendedLocateParameters& parameters,
         const thisCursorType* cursor=nullptr) const;

   const inheritedImplementation& getImplementation() const { return (const inheritedImplementation&) *this; }
   friend class TTernaryTreeCursor<TypeSubString, TypeImplTree, TypeFollowingCell>;

  public:
   TTernaryTree() {}
   TTernaryTree(const thisType& source,
         COL::VirtualCollection::AddMode duplicateMode=COL::VirtualCollection::AMNoDuplicate,
         const VirtualCast* retrieveRegistrationFromCopy=nullptr)
      :  inherited(source, duplicateMode),
         inheritedImplementation(source, duplicateMode==COL::VirtualCollection::AMDuplicate, retrieveRegistrationFromCopy) {}
   thisType& operator=(const thisType& source) { return (thisType&) inherited::operator=(source); }
   virtual bool isValid() const override
      {  return inherited::isValid() && inheritedImplementation::isValid(); }
   Template3DefineCopy(TTernaryTree, TypeSubString, TypeImplTree, TypeFollowingCell)
   DTemplate3DefineAssign(TTernaryTree, TypeSubString, TypeImplTree, TypeFollowingCell)
   StaticInheritConversions(thisType, inherited)
};

template <class TypeSubString, class TypeImplTree, class TypeFollowingCell>
inline typename TTernaryTree<TypeSubString, TypeImplTree, TypeFollowingCell>::InheritedLocationResultAccess
TTernaryTree<TypeSubString, TypeImplTree, TypeFollowingCell>::LocationResult::asSortedLocation() const
   {  return InheritedLocationResultAccess(*this); }


/*******************************************************/
/* Definition of the template class TTernaryTreeCursor */
/*******************************************************/

template <class TypeSubString, class TypeImplTree, class TypeFollowingCell>
class TTernaryTreeCursor : public BaseTernaryTree<TypeSubString>::Cursor {
  private:
   typedef TypeImplTree SupportImplementation;
   typedef typename COL::DTernaryTree::BaseTernaryTree<TypeSubString>::Cursor inherited;
   typedef TTernaryTreeCursor<TypeSubString, TypeImplTree, TypeFollowingCell> thisType;

  public:
   const TTernaryTree<TypeSubString, TypeImplTree, TypeFollowingCell>& getSupport() const
      {  return (const TTernaryTree<TypeSubString, TypeImplTree, TypeFollowingCell>&)
            TTernaryTree<TypeSubString, TypeImplTree, TypeFollowingCell>::CastCollectionHandler::castFrom(
                  inherited::getSSupport());
      }

  protected:
   friend class TTernaryTree<TypeSubString, TypeImplTree, TypeFollowingCell>::LocationResult;
   class CursorImplementation : public TypeImplTree::Cursor {
     private:
      typedef typename TypeImplTree::Cursor inherited;

     public:
      CursorImplementation(const TypeImplTree& tree) : inherited(tree) {}
      CursorImplementation(const CursorImplementation& source) = default;
      CursorImplementation& operator=(const CursorImplementation& source) = default;
      DefineCopy(CursorImplementation)
      typedef TypeFollowingCell Node;

      Node* getReference() const { return inherited::getReference(); }
   };

  private:
   CursorImplementation ciPosition;

  protected:
   void setFinal()
      {  thisType::_setFinal();
         inherited::releaseMode(getSupport());
      }
   void assumeFinal() 
      {  AssumeCondition(ciPosition.isFinalValid())
         inherited::_setFinal();
         inherited::releaseMode(getSupport());
      }
   virtual void _setFinal() override
      {  if (!inherited::isExactCursor()) {
            if (inherited::isLeftCursor()) {
               if (!ciPosition.setToLeftRighter())
                  ciPosition.setToPrevious(getSupport().getImplementation());
            }
            else {
               if (!ciPosition.setToRightLefter())
                  ciPosition.setToNext(getSupport().getImplementation());
            };
            inherited::_setFinal();
         };
      }
   const CursorImplementation& getFinal(CursorImplementation& copy) const
      {  if (!inherited::isExactCursor()) {
            copy = ciPosition;
            if (inherited::isLeftCursor() && !copy.setToLeftRighter())
               copy.setToPrevious(getSupport().getImplementation());
            else if (inherited::isRightCursor() && !copy.setToRightLefter())
               copy.setToNext(getSupport().getImplementation());
         };
         return !inherited::isExactCursor() ? copy : ciPosition;
      }

  protected:
   friend class COL::DTernaryTree::BaseTernaryTree<TypeSubString>;
   friend class BaseTernaryTreeMethods;
   friend class TTernaryTree<TypeSubString, TypeImplTree, TypeFollowingCell>;
   CursorImplementation& getImplementation() { return ciPosition; }
   const CursorImplementation& getImplementation() const { return ciPosition; }
   const TypeImplTree& getImplementationSupport() const { return getSupport().getImplementation(); }

   virtual ComparisonResult _compare(const EnhancedObject& asource) const override;
   virtual bool _isEqual(const AbstractCursor& cursor) const override;
   virtual EnhancedObject* _getSElement() const override;
   virtual TypeSubString _getKey() const;
   virtual bool _isPositionned(const COL::VirtualCollection::ExtendedLocateParameters& pos,
      const COL::VirtualCollectionCursor* cursor=nullptr) const override;
   virtual bool _position(const COL::VirtualCollectionCursor::Position& pos) override;

  public:
   TTernaryTreeCursor(const TTernaryTree<TypeSubString, TypeImplTree, TypeFollowingCell>& support)
      :  inherited(support), ciPosition((const SupportImplementation&) support) {}
   TTernaryTreeCursor(const thisType& source) = default;
   class Descent {};
   TTernaryTreeCursor(const thisType& source, Descent)
      : inherited(source), ciPosition(source.ciPosition)
      {  AssumeCondition(ciPosition.isFinalValid() && inherited::isExactCursor()) }
   thisType& operator=(const thisType& source) = default;

   Template3DefineCopy(TTernaryTreeCursor, TypeSubString, TypeImplTree, TypeFollowingCell)
   DTemplate3DefineAssign(TTernaryTreeCursor, TypeSubString, TypeImplTree, TypeFollowingCell)
   void swap(thisType& source)
      {  inherited::swap((inherited&) source);
         ciPosition.swap(source.ciPosition);
      }

   virtual bool isValid() const override
      {  bool result = false;
         if (inherited::isValid() && ciPosition.isValid()) {
            if ((result = inherited::isExactCursor()) == false)
               result = inherited::isLeftCursor()
                  ? !ciPosition.isInnerLeft() : !ciPosition.isInnerRight();
         };
         return result;
      }
   bool isOnLocation() const
      {  bool result = false;
         if (inherited::isValid() && ciPosition.isValid())
            result = inherited::isExactCursor();
         return result;
      }
   TypeSubString getKey() const { return thisType::_getKey(); }
   void invalidate()
      {  ciPosition.invalidate();
         inherited::releaseMode(getSupport());
      }
};

/* inline methods of the template class TTernaryTree */

#ifndef _MSC_VER
template <class TypeSubString, class TypeImplTree, class TypeFollowingCell>
inline typename TTernaryTree<TypeSubString, TypeImplTree, TypeFollowingCell>::ExtendedMapInsertionParameters
TTernaryTree<TypeSubString, TypeImplTree, TypeFollowingCell>::LocationResult::queryInsertionParameters()
   {  COL::VirtualCollection::RelativePosition pos = (isLess()
         ? COL::VirtualCollection::RPBefore : (isGreater()
         ? COL::VirtualCollection::RPAfter : COL::VirtualCollection::RPUndefined));
      ExtendedMapInsertionParameters result(getInitialKey());
      result.setLocation(*this).setRelativePosition(pos);
      return result;
   }
#endif

template <class TypeSubString, class TypeImplTree, class TypeFollowingCell>
inline typename COL::DTernaryTree::BaseTernaryTree<TypeSubString>::LocationResult
TTernaryTree<TypeSubString, TypeImplTree, TypeFollowingCell>::_locateKey(const TypeSubString& key,
      const COL::VirtualCollection::ExtendedLocateParameters& parameters,
      typename inherited::LocateCursor* cursor, const typename inherited::LocateCursor* start,
      const typename inherited::LocateCursor* end) const {
   LocationResult result = _locateKey(key,
      parameters, (thisCursorType*) cursor, (const thisCursorType*) start,
      (const thisCursorType*) end);
   return InheritedLocationResultAccess(result);
}

template <class TypeSubString, class TypeImplTree, class TypeFollowingCell>
inline TypeSubString
TTernaryTree<TypeSubString, TypeImplTree, TypeFollowingCell>::_queryKey(
      const COL::VirtualCollection::ExtendedLocateParameters& parameters,
      const COL::VirtualCollectionCursor* cursor) const
   {  return _queryKey(parameters, const_cast<thisCursorType*>((const thisCursorType*) cursor)); }

template <class TypeSubString, class TypeImplTree, class TypeFollowingCell>
inline void
TTernaryTree<TypeSubString, TypeImplTree, TypeFollowingCell>::_moveTo(
      COL::VirtualCollection& destination,
      const COL::VirtualCollection::ExtendedReplaceParameters& parameters,
      COL::VirtualCollectionCursor* cursor, VirtualCollectionCursor* destinationCursor) {
   AssumeCondition(dynamic_cast<inherited*>(&destination))
   inherited::pmoveTo((inherited&) destination, parameters,
         (thisCursorType*) cursor, (typename inherited::Cursor*) destinationCursor);
}

template <class TypeSubString, class TypeImplTree, class TypeFollowingCell>
inline void
TTernaryTree<TypeSubString, TypeImplTree, TypeFollowingCell>::_moveTo(thisType& destination,
      const COL::VirtualCollection::ExtendedReplaceParameters& parameters,
   thisCursorType* cursor, thisCursorType* destinationCursor)
{  inherited::pmoveTo(destination, parameters, cursor, destinationCursor); }

template <class TypeSubString, class TypeImplTree, class TypeFollowingCell>
inline void
TTernaryTree<TypeSubString, TypeImplTree, TypeFollowingCell>::_removeAll(
      const COL::VirtualCollection::ExtendedSuppressParameters& parameters,
      const VirtualCollectionCursor* start, const VirtualCollectionCursor* end)
   { _removeAll(parameters, const_cast<thisCursorType*>((const thisCursorType*) start),
         const_cast<thisCursorType*>((const thisCursorType*) end));
   }

template <class TypeSubString, class TypeImplTree, class TypeFollowingCell>
inline int
TTernaryTree<TypeSubString, TypeImplTree, TypeFollowingCell>::_queryCount(
      const COL::VirtualCollection::ExtendedLocateParameters& parameters,
   const thisCursorType* start, const thisCursorType* end) const
   {  return (!start && !end) ? inheritedImplementation::count()
         : COL::VirtualCollection::pqueryCount(parameters, start, end);
   }

template <class TypeSubString, class TypeImplTree, class TypeFollowingCell>
inline int
TTernaryTree<TypeSubString, TypeImplTree, TypeFollowingCell>::_queryCount(
      const COL::VirtualCollection::ExtendedLocateParameters& parameters,
      const VirtualCollectionCursor* start, const VirtualCollectionCursor* end) const
   {  return _queryCount(parameters, (const thisCursorType*) start, (const thisCursorType*) end); }

template <class TypeSubString, class TypeImplTree, class TypeFollowingCell>
inline EnhancedObject*
TTernaryTree<TypeSubString, TypeImplTree, TypeFollowingCell>::_getElement(
      const COL::VirtualCollection::ExtendedLocateParameters& parameters,
      const VirtualCollectionCursor* cursor) const
   { return _getElement(parameters, const_cast<thisCursorType*>((const thisCursorType*) cursor)); }

/**************************************************************/   
/* Definition of the template class TDescentTernaryTreeCursor */
/**************************************************************/   

template <class TypeSubString, class TypeImplTree, class TypeFollowingCell>
class TDescentTernaryTreeCursor : public TTernaryTreeCursor<TypeSubString, TypeImplTree, TypeFollowingCell> {
  private:
   typedef TypeImplTree SupportImplementation;
   typedef TTernaryTreeCursor<TypeSubString, TypeImplTree, TypeFollowingCell> inherited;
   typedef TDescentTernaryTreeCursor<TypeSubString, TypeImplTree, TypeFollowingCell> thisType;
   int uSuffixPosition;

  public:
   TDescentTernaryTreeCursor(const TTernaryTree<TypeSubString, TypeImplTree, TypeFollowingCell>& support)
      : inherited(support), uSuffixPosition(-1) {}
   TDescentTernaryTreeCursor(const thisType& source)
      : inherited(source), uSuffixPosition(source.uSuffixPosition) {}
   thisType& operator=(const thisType& source) = default;
   Template3DefineCopy(TDescentTernaryTreeCursor, TypeSubString, TypeImplTree, TypeFollowingCell)
   DTemplate3DefineAssign(TDescentTernaryTreeCursor, TypeSubString, TypeImplTree, TypeFollowingCell)

   virtual bool isValid() const override
      {  typename TypeSubString::Char* suffix = nullptr;
         return (COL::DTernaryTree::BaseTernaryTree<TypeSubString>::Cursor::isValid()
               && inherited::isExactCursor() && inherited::getImplementation().isFinalValid()
            && (((suffix = inherited::getImplementation().suffix().getString()) == nullptr)
               || ((uSuffixPosition >= 1) && (suffix[uSuffixPosition-1] == '\0'))));
      }
   bool isValidPosition() const {  return COL::DTernaryTree::BaseTernaryTree<TypeSubString>::Cursor::isValid(); }
   bool setToSon(typename TypeSubString::Char achar)
      {  bool result = false;
         if (uSuffixPosition < 0) {
            if (((result = inherited::getImplementation().setToSon(achar,
                           inherited::getImplementationSupport())) != false)
                  && inherited::getImplementation().isFinalValid())
               uSuffixPosition = 0;
         }
         else {
            typename TypeSubString::Char* suffix = inherited::getImplementation().suffix().getString();
            AssumeCondition((suffix != nullptr) && (uSuffixPosition <= (int) strlen(suffix)))
            if ((result = (suffix[uSuffixPosition] == achar)) != false)
               ++uSuffixPosition;
         };
         return result;
      }
   bool isUpRoot() const { return inherited::getImplementation().isUpRoot(); }
   TypeSubString stringAt() const { return inherited::getImplementation().stringAt(); }
   TypeSubString prefixAt() const
      {  TypeSubString result = inherited::getImplementation().prefixAt();
         if (uSuffixPosition >= 0) {
            typename TypeSubString::Char* suffix = inherited::getImplementation().suffix().getString();
            AssumeCondition((suffix != nullptr) && (uSuffixPosition <= (int) strlen(suffix)))
            if (suffix != nullptr)
               result.cat(suffix, uSuffixPosition);
         };
         return result;
      }
   void invalidate() { inherited::invalidate(); uSuffixPosition = -1; }
};

}} // end of namespace COL::DTernaryTree

#endif // COL_TernaryTreeInterfaceBaseH
