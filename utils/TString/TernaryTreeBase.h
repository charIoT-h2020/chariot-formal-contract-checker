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
// File      : TernaryTreeBase.h
// Description :
//   Definition of base classes, common to the dictionary template classes
//   TImplTernaryTree and TImplParentTernaryTree.
//

#ifndef STG_TernaryTreeBaseH
#define STG_TernaryTreeBaseH

#include "TString/SString.h"

namespace STG {

template <class TypeSubString>
class TImplTernaryTree;

namespace DTernaryTree {

/*******************************************/
/* Definition of the class BaseTernaryTree */
/*******************************************/

class Constants {
  public:
   enum LocationResult
      { LRUndefined, LRLess, LRLessSuffix, LRNewSuffix, LRExact, LRGreaterSuffix, LRGreater };
};

class InterfaceConstants {
  public:
   typedef Constants::LocationResult LocationSearchResult;
   static const LocationSearchResult LSRUndefined = Constants::LRUndefined;
   static const LocationSearchResult LSRLess = Constants::LRLess;
   static const LocationSearchResult LSRLessSuffix = Constants::LRLessSuffix;
   static const LocationSearchResult LSRNewSuffix = Constants::LRNewSuffix;
   static const LocationSearchResult LSRExact = Constants::LRExact;
   static const LocationSearchResult LSRGreaterSuffix = Constants::LRGreaterSuffix;
   static const LocationSearchResult LSRGreater = Constants::LRGreater;
};

class BaseTernaryTree : public EnhancedObject, public Constants {
  private:
   template <class TypeCursor, class TypeSubString>
   static bool findNext(TypeCursor& cursor, TypeSubString* subString);
   template <class TypeCursor, class TypeSubString>
   static bool findPrevious(TypeCursor& cursor, TypeSubString* subString);

  public:
   class ExtendedComparisonResult : public ExtendedParameters {
     private:
      typedef ExtendedParameters inherited;
      enum Comparison {
         CUndefined, CLess, CLessSuffix, CLessFollowSuffix, CGreaterFollowSuffix,
         CEqual, CGreaterSuffix, CGreater
      };

     protected:
      DefineExtendedParameters(3, ExtendedParameters)

     public:
      ExtendedComparisonResult() {}
      ExtendedComparisonResult(const ExtendedComparisonResult& source)
         :  ExtendedParameters()
         {  mergeOwnField(source.queryOwnField()); }
      ExtendedComparisonResult& operator=(const ExtendedComparisonResult& source)
         {  setOwnField(source.queryOwnField()); return *this; }

      operator ComparisonResult() const
         {  Comparison compare = (Comparison) queryOwnField();
            return (compare < CLessFollowSuffix) ? ((compare >= CLess)
                  ? CRLess : CRNonComparable) : ((compare >= CGreaterSuffix)
                  ? CRGreater : CREqual);
         }

      ExtendedComparisonResult& setLess()          { setOwnField(CLess); return *this; }
      ExtendedComparisonResult& setLessSuffix()    { setOwnField(CLessSuffix); return *this; }
      ExtendedComparisonResult& setLessFollowSuffix()    { setOwnField(CGreaterFollowSuffix); return *this; }
      ExtendedComparisonResult& setGreaterFollowSuffix() { setOwnField(CGreaterFollowSuffix); return *this; }
      ExtendedComparisonResult& setEqual()         { setOwnField(CEqual); return *this; }
      ExtendedComparisonResult& setGreaterSuffix() { setOwnField(CGreaterSuffix); return *this; }
      ExtendedComparisonResult& setGreater()       { setOwnField(CGreater); return *this; }

      bool isValid() const { return hasOwnField(); }
      bool isLess() const { return queryOwnField() == CLess; }
      bool isLessSuffix() const { return queryOwnField() == CLessSuffix; }
      bool isLessFollowSuffix() const { return queryOwnField() == CLessFollowSuffix; }
      bool isGreaterFollowSuffix() const { return queryOwnField() == CGreaterFollowSuffix; }
      bool isEqual() const { return queryOwnField() == CEqual; }
      bool isGreaterSuffix() const { return queryOwnField() == CGreaterSuffix; }
      bool isGreater() const { return queryOwnField() == CGreater; }
   };

   enum TypeSuffix
      { TSNone /* none */ , TSSuffix /* only one possible suffix */ , TSFollowing /* several suffixes */ };

   class CursorMove : public ExtendedParameters {
     private:
      enum TypeMove { CMUndefined, CMDblBefore, CMBefore, CMAfter, CMDblAfter, CMInvalid };
      enum CursorKind { TCLeaf, TCNode };

     public:
      CursorMove() {}
      DefineExtendedParameters(4, ExtendedParameters)
      DefineSubExtendedParameters(TypeMove, 3, INHERITED)
      DefineSubExtendedParameters(CursorKind, 1, TypeMove)

      bool isNode() const { return hasCursorKindField(); }
      bool isLeaf() const { return !hasCursorKindField(); }
      template <class TypeTree>
      COL::ImplBinaryNode::Balance applyMove(typename TypeTree::Cursor& cursor,
            const TypeTree& tree, COL::ImplBinaryNode::Balance balance) const
         {  COL::ImplBinaryNode::Balance result = COL::ImplBinaryNode::BEqual;
            if (balance == COL::ImplBinaryNode::BEqual)
               cursor.invalidate();
            else if (hasTypeMoveField()) {
               if (queryTypeMoveField() <= CMBefore) {
                  if (queryTypeMoveField() == CMBefore) {
                     if (balance == COL::ImplBinaryNode::BRight) {
                        if (!cursor.setToFollowsRighter())
                           cursor.setToPrevious(tree);
                     }
                     else
                        result = COL::ImplBinaryNode::BRight;
                  }
                  else {
                     cursor.setToBinaryPrevious(tree);
                     if (balance == COL::ImplBinaryNode::BRight) {
                        if (!cursor.setToFollowsRighter())
                           cursor.setToPrevious(tree);
                     }
                     else
                        result = COL::ImplBinaryNode::BRight;
                  };
               }
               else if (queryTypeMoveField() <= CMAfter) {
                  if (queryTypeMoveField() == CMAfter) {
                     if (balance == COL::ImplBinaryNode::BLeft) {
                        if (!cursor.setToFollowsLefter())
                           cursor.setToNext(tree);
                     }
                     else
                        result = COL::ImplBinaryNode::BLeft;
                  }
                  else {
                     cursor.setToBinaryNext(tree);
                     if (balance == COL::ImplBinaryNode::BLeft) {
                        if (!cursor.setToFollowsLefter())
                           cursor.setToNext(tree);
                     }
                     else
                        result = COL::ImplBinaryNode::BLeft;
                  };
               }
               else
                  cursor.invalidate();
            }
            else
               result = COL::ImplBinaryNode::BAll;
            return result;
         }

      void setSwapBefore()
         {  AssumeCondition(!hasTypeMoveField())
            mergeTypeMoveField(CMBefore);
         }
      void setSwapAfter()
         {  AssumeCondition(!hasTypeMoveField())
            mergeTypeMoveField(CMAfter);
         }
      void setBefore()
         {  if (!hasTypeMoveField())
               mergeTypeMoveField(CMBefore);
            else if (queryTypeMoveField() == CMBefore)
               setTypeMoveField(CMDblBefore);
            else if (queryTypeMoveField() != CMAfter)
               AssumeUncalled
            mergeCursorKindField(TCNode);
         }
      void setAfter()
         {  if (!hasTypeMoveField())
               mergeTypeMoveField(CMAfter);
            else if (queryTypeMoveField() == CMAfter)
               setTypeMoveField(CMDblAfter);
            else if (queryTypeMoveField() != CMBefore)
               AssumeUncalled
            mergeCursorKindField(TCNode);
         }

      // The swap is only possible with a son node
      void setReplace(COL::ImplBinaryNode::Balance balance)
         {  AssumeCondition(!hasTypeMoveField())
            mergeTypeMoveField((balance == COL::ImplBinaryNode::BLeft) ? CMBefore
               : ((balance == COL::ImplBinaryNode::BRight) ? CMAfter : CMUndefined));
            if (hasTypeMoveField())
               mergeCursorKindField(TCNode);
         }
      void invalidate() { AssumeCondition(!hasTypeMoveField()) mergeTypeMoveField(CMInvalid); }
   };

   template <class TypeTree, class Action>
   static bool foreachNodeDo(const TypeTree& ternary, Action& action);
   template <class TypeTree, class Action>
   static bool foreachNodeReverseDo(const TypeTree& ternary, Action& action);

   template <class TypeCursor, class TypeSubString>
   static void setToInnerLeft(TypeCursor& cursor, TypeSubString* subString);
   template <class TypeCursor, class TypeSubString>
   static void setToInnerRight(TypeCursor& cursor, TypeSubString* subString);
   template <class TypeCursor, class TypeFollowingCell, class TypeSubString>
   static bool setToFirst(TypeCursor& cursor, TypeFollowingCell* root, TypeSubString* subString);
   template <class TypeCursor, class TypeFollowingCell, class TypeSubString>
   static bool setToLast(TypeCursor& cursor, TypeFollowingCell* root, TypeSubString* subString);
   template <class TypeCursor, class TypeFollowingCell, class TypeSubString>
   static bool setToNext(TypeCursor& cursor, TypeFollowingCell* root, TypeSubString* subString);
   template <class TypeCursor, class TypeFollowingCell, class TypeSubString>
   static bool setToPrevious(TypeCursor& cursor, TypeFollowingCell* root, TypeSubString* subString);
   template <class TypeCursor, class TypeFollowingCell, class TypeSubString>
   static bool setToInnerDepth(TypeCursor& cursor, TypeFollowingCell* root, TypeSubString* subString);
   template <class TypeCursor, class TypeSubString>
   static bool setToLefter(TypeCursor& cursor, TypeSubString* subString);
   template <class TypeCursor, class TypeSubString>
   static bool setToRighter(TypeCursor& cursor, TypeSubString* subString);
   template <class TypeCursor, class TypeFollowingCell, class TypeSubString>
   static bool setToSon(TypeCursor& cursor, typename TypeSubString::Char achar, TypeFollowingCell* root, TypeSubString* subString);

   template <class TypeCursor, class TypeFollowingCell>
   static bool setToBinaryNext(TypeCursor& cursor, TypeFollowingCell* root);
   template <class TypeCursor, class TypeFollowingCell>
   static bool setToBinaryPrevious(TypeCursor& cursor, TypeFollowingCell* root);

   template <class TypeCursor>
   static bool hasLeaf(const TypeCursor& cursor);

   template <class TypeTree> class RouteFreeAction;
   template <class TypeTree> class RouteRemoveAction;
   template <class TypeTree> class RouteCopyAction;
   template <class TypeTree> class RouteValidityAction;
   template <class TypeTree> class RouteHeightValidityAction;

   class Copy {};

   template <class TypeTree, class TypeSubString> static LocationResult
   tlocate(const TypeTree& thisTree, TypeSubString& subString, typename TypeTree::Cursor& cursor);
};

/****************************************/
/* Definition of the class SuffixObject */
/****************************************/
// Terminal object for the ternary trees

class BaseSuffixMethods {
  public:
   template <class TypeSubString, class TypeSuffixObject, class TypeFollowingCell>
   static TypeFollowingCell* extractPrefix(TypeSuffixObject& thisSuffix, TypeSubString& subString, TypeFollowingCell*& lastCell);
};

template <class TypeSubString>
class BaseSuffixObject : protected BaseSuffixMethods {
  private:
   typedef BaseSuffixMethods inherited;
   typedef BaseSuffixObject<TypeSubString> thisType;

   typename TypeSubString::Char* szSuffix;

  protected:
   BaseSuffixObject() : szSuffix(nullptr) {}
   BaseSuffixObject(const TypeSubString& suffix) : szSuffix(suffix.createString()) {}
   BaseSuffixObject(const BaseSuffixObject& source);
   ~BaseSuffixObject() { clear(); }
   BaseSuffixObject& operator=(const BaseSuffixObject& source);

   void clear() { if (szSuffix) delete [] szSuffix; szSuffix = nullptr; }
   void setBasicSuffix(typename TypeSubString::Char* newSuffix)
      {  if (szSuffix) delete [] szSuffix; 
         szSuffix = newSuffix;
      }
   void setSuffix(TypeSubString& suffix)
      {  clear();
         szSuffix = suffix.createString();
      }
   void swapSuffix(BaseSuffixObject& source)
      {  typename TypeSubString::Char* temp = szSuffix;
         szSuffix = source.szSuffix;
         source.szSuffix = temp;
      }
   bool hasSuffix() const { return szSuffix != nullptr; }
   friend class BaseSuffixMethods;

  public:
   typename TypeSubString::Char* getString() const { return szSuffix; }
   template <class TypeFollowingCell>
   TypeFollowingCell* extractPrefix(TypeSubString& subString, TypeFollowingCell*& lastCell)
      { return inherited::extractPrefix(*this, subString, lastCell); }
   // static BaseSuffixObject* createSuffixObject(TypeSubString& subString, EnhancedObject* object);
   // EnhancedObject* getObject() const;
   // BaseSuffixObject* clone(bool duplicateObject, const VirtualCast* retrieveRegistrationFromCopy) const;
};

template <class TypeSubString>
class SuffixObject : public BaseSuffixObject<TypeSubString> {
  private:
   typedef BaseSuffixObject<TypeSubString> inherited;
   typedef SuffixObject<TypeSubString> thisType;
   EnhancedObject* peoObject;

  protected:
   SuffixObject(TypeSubString& suffix, EnhancedObject* object)
      :  inherited(suffix), peoObject(object) {}

  public:
   SuffixObject(const thisType& source) : inherited(source), peoObject(source.peoObject) {}
   SuffixObject(const thisType& source, bool duplicateObject,
         const VirtualCast* retrieveRegistrationFromCopy=nullptr)
      :  inherited(source),
         peoObject((duplicateObject && source.peoObject) ? (retrieveRegistrationFromCopy
            ? retrieveRegistrationFromCopy->castFrom(source.peoObject->createCopy())
            : source.peoObject->createCopy()) : source.peoObject) {}

   thisType& operator=(const thisType& source)
      {  inherited::operator=(source);
         peoObject = source.peoObject;
         return *this;
      }

   void clear() { inherited::clear(); peoObject = nullptr; }
   void free()  { inherited::clear(); if (peoObject) delete peoObject; peoObject = nullptr; }
   void duplicate(const SuffixObject& source,
         const VirtualCast* retrieveRegistrationFromCopy=nullptr);
   thisType* clone(bool duplicateObject, const VirtualCast* retrieveRegistrationFromCopy) const
      {  return new thisType(*this, duplicateObject, retrieveRegistrationFromCopy); }

   static thisType* createSuffixObject(TypeSubString& subString, EnhancedObject* object)
      {  return new thisType(subString, object); }
   const PEnhancedObject& getObject() const { return peoObject; }
   void replaceObject(EnhancedObject* object, bool doesFree)
      {  if (doesFree && peoObject)
            delete peoObject;
         peoObject = object;
      }
};

/*******************************************************/
/* Definition of the template class TBaseFollowingCell */
/*******************************************************/
// Non terminal object for the ternary trees

template <class TypeBinaryNode, typename TypeChar>
class TExtendNode : public TypeBinaryNode {
  private:
   typedef TypeBinaryNode inherited;
   typedef TExtendNode<TypeBinaryNode, TypeChar> thisType;
   TypeChar cChar;

  public:
   TExtendNode(TypeChar charSource) : cChar(charSource) {}
   TExtendNode(const thisType& source) = default;
   thisType& operator=(const thisType& source) = default;
   TypeChar getChar() const { return cChar; }
   void setChar(TypeChar charSource) { cChar = charSource; }
   bool hasChar() const { return cChar != 0; }
};

template <class TypeBinaryNode, typename TypeChar, int USizeChar>
class TExtendNodeForChar : public TypeBinaryNode {
  private:
   typedef TypeBinaryNode inherited;
   typedef TExtendNodeForChar<TypeBinaryNode, TypeChar, USizeChar> thisType;

  protected:
   TemplateDefineExtendedParameters(USizeChar, TypeBinaryNode)

  public:
   TExtendNodeForChar(TypeChar achar) { mergeOwnField(achar); }
   TExtendNodeForChar(const thisType& source) : inherited(source) {}
   thisType& operator=(const thisType& source)
      {  setOwnField(source.queryOwnField()); return (thisType&) inherited::operator=(source); }
   TypeChar getChar() const { return queryOwnField(); }
   void setChar(TypeChar achar) { setOwnField(achar); }
   bool hasChar() const { return hasOwnField(); }
};

template <class TypeSubString, class TypeBinaryNode, class TypeSuffixObject>
class TLocateContinuation;

template <class TypeSubString, class TypeBinaryNode, class TypeSuffixObject>
class TBaseFollowingCell : public TypeBinaryNode {
  public:
   typedef BaseTernaryTree::TypeSuffix TypeSuffix;
   typedef BaseTernaryTree::ExtendedComparisonResult ExtendedComparisonResult;
   static const TypeSuffix TSNone = BaseTernaryTree::TSNone;
   static const TypeSuffix TSSuffix = BaseTernaryTree::TSSuffix;
   static const TypeSuffix TSFollowing = BaseTernaryTree::TSFollowing;

  private:
   void* pvFollows;
   typedef TBaseFollowingCell<TypeSubString, TypeBinaryNode, TypeSuffixObject> thisType;
   typedef TypeBinaryNode inherited;

  protected:
   TemplateDefineExtendedParameters(2, TypeBinaryNode)
   DefineSubExtendedParameters(Type, 2, INHERITED)

   TypeSuffix getTypeSuffix() const { return (TypeSuffix) queryTypeField(); }
   typename TypeSubString::Char getChar() const  { return inherited::getChar(); }

   TypeSuffixObject& suffix() const;
   void setSuffix(TypeSubString& subString, EnhancedObject* object);
   void absorbSuffix(TypeSuffixObject* suffix);
   void setFollowing(thisType* decision);
   void resetFollowing(thisType* decision);
   thisType& following() const;

   void clearFollows() { pvFollows = nullptr; clearTypeField(); }
   bool isSuffix() const { return queryTypeField() == TSSuffix; }
   bool hasSuffix() const { return (queryTypeField() == TSSuffix) && pvFollows; }
   bool isFollowing() const { return queryTypeField() == TSFollowing; }
   bool hasFollowing() const { return (queryTypeField() == TSFollowing) && pvFollows; }
   bool hasFollows() const { return hasTypeField() && pvFollows; }
   bool isUndefined() const { return queryTypeField() == TSNone; }
   void changeSon(thisType* node, thisType* newNode);
   void changeFollows(thisType* newNode);

   thisType* left() const { return (thisType*) TypeBinaryNode::left(); }
   thisType* right() const { return (thisType*) TypeBinaryNode::right(); }
   thisType* sfollows() const;

   ExtendedComparisonResult compareSons(thisType* fstSon, thisType* sndSon) const;

  public:
   TBaseFollowingCell(typename TypeSubString::Char achar) : inherited(achar), pvFollows(nullptr) {}
   TBaseFollowingCell(TypeSubString& suffix, EnhancedObject* object);
   TBaseFollowingCell(const thisType& source) : inherited(source), pvFollows(nullptr)
      {  setTypeField(TSNone); }
   TBaseFollowingCell(const thisType& source, bool duplicateObject,
         const VirtualCast* retrieveRegistrationFromCopy=nullptr);
   thisType& operator=(const thisType& source) { return (thisType&) inherited::operator=(source); }

   bool invariant() const
      {  return ((getChar() != '\0') || (queryTypeField() != TSFollowing))
            && (pvFollows ? (queryTypeField() != TSNone) : (queryTypeField() == TSNone));
      }
   bool isValid() const
      {  return inherited::isValid() && pvFollows && hasTypeField()
            && ((getChar() != '\0') || (queryTypeField() == TSSuffix));
      }
   bool locate(TypeSubString& subString,
         TLocateContinuation<TypeSubString, TypeBinaryNode, TypeSuffixObject>& env) const;
};

/***********************************************************/
/* Inline methods of the template class TBaseFollowingCell */
/***********************************************************/

template <class TypeSubString, class TypeBinaryNode, class TypeSuffixObject>
inline
TBaseFollowingCell<TypeSubString, TypeBinaryNode, TypeSuffixObject>::TBaseFollowingCell(const thisType& source,
      bool duplicateObject, const VirtualCast* retrieveRegistrationFromCopy)
   : inherited(source), pvFollows(nullptr) {
   setTypeField(TSNone);
   if (source.isSuffix()) {
      mergeTypeField(TSSuffix);
      pvFollows = ((const TypeSuffixObject*) source.pvFollows)->clone(duplicateObject, retrieveRegistrationFromCopy);
   };
}

template <class TypeSubString, class TypeBinaryNode, class TypeSuffixObject>
inline TypeSuffixObject&
TBaseFollowingCell<TypeSubString, TypeBinaryNode, TypeSuffixObject>::suffix() const {
   AssumeCondition((queryTypeField() == TSSuffix) && pvFollows)
   return *((TypeSuffixObject*) pvFollows);
}

template <class TypeSubString, class TypeBinaryNode, class TypeSuffixObject>
inline void
TBaseFollowingCell<TypeSubString, TypeBinaryNode, TypeSuffixObject>::setSuffix(
      TypeSubString& subString, EnhancedObject* object) {
   AssumeCondition(!hasTypeField())
   mergeTypeField(TSSuffix);
   pvFollows = TypeSuffixObject::createSuffixObject(subString, object);
}

template <class TypeSubString, class TypeBinaryNode, class TypeSuffixObject>
inline
TBaseFollowingCell<TypeSubString, TypeBinaryNode, TypeSuffixObject>::TBaseFollowingCell(
      TypeSubString& suffix, EnhancedObject* object)
   :  inherited((typename TypeSubString::Char) suffix), pvFollows(nullptr) {
   suffix.advance();
   setSuffix(suffix, object);
}

template <class TypeSubString, class TypeBinaryNode, class TypeSuffixObject>
inline void
TBaseFollowingCell<TypeSubString, TypeBinaryNode, TypeSuffixObject>::absorbSuffix(
      TypeSuffixObject* suffix) {
   AssumeCondition(!hasTypeField())
   mergeTypeField(TSSuffix);
   pvFollows = suffix;
}

template <class TypeSubString, class TypeBinaryNode, class TypeSuffixObject>
inline void
TBaseFollowingCell<TypeSubString, TypeBinaryNode, TypeSuffixObject>::setFollowing(
      thisType* decision) {
   AssumeCondition(!hasTypeField() && inherited::hasChar())
   mergeTypeField(TSFollowing);
   pvFollows = decision;
}

template <class TypeSubString, class TypeBinaryNode, class TypeSuffixObject>
inline void
TBaseFollowingCell<TypeSubString, TypeBinaryNode, TypeSuffixObject>::resetFollowing(
      thisType* decision) {
   AssumeCondition(inherited::hasChar())
   setTypeField(TSFollowing);
   pvFollows = decision;
}

template <class TypeSubString, class TypeBinaryNode, class TypeSuffixObject>
inline TBaseFollowingCell<TypeSubString, TypeBinaryNode, TypeSuffixObject>*
TBaseFollowingCell<TypeSubString, TypeBinaryNode, TypeSuffixObject>::sfollows() const
   {  return ((queryTypeField() == TSFollowing) && pvFollows) ? ((thisType*) pvFollows) : nullptr; }

template <class TypeSubString, class TypeBinaryNode, class TypeSuffixObject>
inline void
TBaseFollowingCell<TypeSubString, TypeBinaryNode, TypeSuffixObject>::changeSon(
      thisType* node, thisType* newNode) {
   if (sfollows() == node)
      pvFollows = newNode;
   else
      inherited::changeSon(node, newNode);
}

template <class TypeSubString, class TypeBinaryNode, class TypeSuffixObject>
inline void
TBaseFollowingCell<TypeSubString, TypeBinaryNode, TypeSuffixObject>::changeFollows(
      thisType* newNode)
   {  pvFollows = newNode; }

template <class TypeSubString, class TypeBinaryNode, class TypeSuffixObject>
inline TBaseFollowingCell<TypeSubString, TypeBinaryNode, TypeSuffixObject>&
TBaseFollowingCell<TypeSubString, TypeBinaryNode, TypeSuffixObject>::following() const {
   AssumeCondition((queryTypeField() == TSFollowing) && pvFollows)
   return *((thisType*) pvFollows);
}

template <class TypeSubString, class TypeBinaryNode, class TypeSuffixObject>
inline BaseTernaryTree::ExtendedComparisonResult
TBaseFollowingCell<TypeSubString, TypeBinaryNode, TypeSuffixObject>::compareSons(
      thisType* fstSon, thisType* sndSon) const {
   AssumeCondition(fstSon && sndSon
      && ((fstSon == left()) || (fstSon == right()) || (fstSon == sfollows()))
      && ((sndSon == left()) || (sndSon == right()) || (sndSon == sfollows())))
   ExtendedComparisonResult result;
   if (fstSon == sndSon)
      result.setEqual();
   else if ((fstSon && (fstSon == left())) || (sndSon && (sndSon == right()))) {
      if (!sndSon || !fstSon)
         result.setLessSuffix();
      else
         result.setLess();
   }
   else if ((fstSon && (fstSon == right())) || (sndSon && (sndSon == left()))) {
      if (!sndSon || !fstSon)
         result.setGreaterSuffix();
      else
         result.setGreater();
   }
   else {
      if ((sndSon == sfollows()) && !fstSon)
         result.setLessFollowSuffix();
      else {
         AssumeCondition((fstSon == sfollows()) && !sndSon)
         result.setGreaterFollowSuffix();
      };
   }
   return result;
}

/********************************************************/
/* Definition of the template class TLocateContinuation */
/********************************************************/

template <class TypeSubString, class TypeBinaryNode, class TypeSuffixObject>
class TLocateContinuation : public EnhancedObject {
  public:
   typedef BaseTernaryTree::LocationResult LocationResult;
   typedef TBaseFollowingCell<TypeSubString, TypeBinaryNode, TypeSuffixObject> TypeFollowingCell;
   static const LocationResult LRUndefined = BaseTernaryTree::LRUndefined;
   static const LocationResult LRExact = BaseTernaryTree::LRExact;

  private:
   LocationResult lrResult;
   TypeFollowingCell* pfcNode;
   typedef TLocateContinuation<TypeSubString, TypeBinaryNode, TypeSuffixObject> thisType;
   typedef EnhancedObject inherited;

   friend bool TBaseFollowingCell<TypeSubString, TypeBinaryNode, TypeSuffixObject>::locate(TypeSubString&,
         TLocateContinuation<TypeSubString, TypeBinaryNode, TypeSuffixObject>&) const;

   void setResult(LocationResult result)
      {  AssumeCondition((lrResult == LRUndefined) && (result != LRUndefined))
         lrResult = result;
      }
   bool setContinuation(TypeFollowingCell* node, LocationResult chessResult)
      {  if (node)
            pfcNode = node;
         else
            lrResult = chessResult;
         return !node;
      }
  public:
   TLocateContinuation(TypeFollowingCell& node) : lrResult(LRUndefined), pfcNode(&node) {}
   TLocateContinuation(const thisType& source) = default;
   thisType& operator=(const thisType& source) = default;
   Template3DefineCopy(TLocateContinuation, TypeSubString, TypeBinaryNode, TypeSuffixObject)
   DTemplate3DefineAssign(TLocateContinuation, TypeSubString, TypeBinaryNode, TypeSuffixObject)

   bool isUndefined() const { return lrResult == LRUndefined; }
   bool isPresent() const { AssumeCondition(lrResult != LRUndefined) return (lrResult == LRExact); }
   TypeFollowingCell* node() const { return pfcNode; }
   const LocationResult& getResult() const { return lrResult; }
};

template <class TypeSubString, class TypeBinaryNode, class TypeSuffixObject>
inline bool
TBaseFollowingCell<TypeSubString, TypeBinaryNode, TypeSuffixObject>::locate(TypeSubString& subString,
      TLocateContinuation<TypeSubString, TypeBinaryNode, TypeSuffixObject>& env) const {
   typename TypeSubString::Char achar = (typename TypeSubString::Char) subString,
                                decision = getChar();
   bool result = true;
   if (achar != decision)
      result = (achar < decision)
         ? env.setContinuation(left(), BaseTernaryTree::LRLess)
         : env.setContinuation(right(), BaseTernaryTree::LRGreater);
   else {
      if (achar == '\0')
         env.setResult(BaseTernaryTree::LRExact);
      else if (isFollowing()) {
         subString.advance();
         result = env.setContinuation(&following(), BaseTernaryTree::LRExact);
      }
      else if (isSuffix()) {
         ComparisonResult compareResult = subString.advance().compare(suffix().getString());
         env.setResult((compareResult == CRLess) ? BaseTernaryTree::LRLessSuffix
                      : ((compareResult == CRGreater) ? BaseTernaryTree::LRGreaterSuffix : BaseTernaryTree::LRExact));
      }
      else
         env.setResult(BaseTernaryTree::LRNewSuffix);
   };
   return result;
}

/*****************************************/
/* Definition of the class FollowingCell */
/*****************************************/

template <typename TypeChar>
class TExtendBalancedNode : public TExtendNode<COL::ImplBalancedNode, TypeChar> {
  private:
   typedef TExtendNode<COL::ImplBalancedNode, TypeChar> inherited;
   typedef TExtendBalancedNode<TypeChar> thisType;

  public:
   TExtendBalancedNode(TypeChar achar) : inherited(achar) {}
   TExtendBalancedNode(const thisType& source) : inherited(source) {}
};

template <>
class TExtendBalancedNode<char> : public TExtendNodeForChar<COL::ImplBalancedNode, char, sizeof(char)*8> {
  private:
   typedef TExtendNodeForChar<COL::ImplBalancedNode, char, sizeof(char)*8> inherited;
   typedef TExtendBalancedNode<char> thisType;

  public:
   TExtendBalancedNode(char achar) : inherited(achar) {}
   TExtendBalancedNode(const thisType& source) : inherited(source) {}
};

template <>
#ifdef __GNUC__
class TExtendBalancedNode<wchar_t> : public TExtendNode<COL::ImplBalancedNode, wchar_t> // char outsize the ExtendedParameters
#else
class TExtendBalancedNode<wchar_t> : public TExtendNodeForChar<COL::ImplBalancedNode, wchar_t, sizeof(wchar_t)*8> // char inside the ExtendedParameters
#endif
{ private:
#ifdef __GNUC__
   typedef TExtendNode<COL::ImplBalancedNode, wchar_t> inherited;
#else
   typedef TExtendNodeForChar<COL::ImplBalancedNode, wchar_t, sizeof(wchar_t)*8> inherited;
#endif
   typedef TExtendBalancedNode<wchar_t> thisType;

  public:
   TExtendBalancedNode(wchar_t achar) : inherited(achar) {}
   TExtendBalancedNode(const thisType& source) : inherited(source) {}
};

template <class TypeSubString>
class TFollowingCell
   : public TBaseFollowingCell<TypeSubString, TExtendBalancedNode<typename TypeSubString::Char>,
         SuffixObject<TypeSubString> > {
  private:
   typedef TFollowingCell<TypeSubString> thisType;
   typedef TBaseFollowingCell<TypeSubString, TExtendBalancedNode<typename TypeSubString::Char>,
         SuffixObject<TypeSubString> > inherited;

  protected:
   friend class STG::TImplTernaryTree<TypeSubString>;
   friend class BaseTernaryTree;

   friend class SuffixObject<TypeSubString>;
   friend class BaseSuffixObject<TypeSubString>;
   friend class BaseSuffixMethods;

   thisType& following() const { return (thisType&) inherited::following(); }
   void setFollowing(thisType* decision) { inherited::setFollowing(decision); }
   void resetFollowing(thisType* decision) { inherited::resetFollowing(decision); }
   typename TExtendBalancedNode<typename TypeSubString::Char>::Balance disconnectFollowing();
   void clear();
   void lightClear();
   void free();
   void freeFollowing();
   int splitSuffix(TypeSubString& string);
   static void swap(thisType* fst, thisType* snd, thisType*& parentFst,
         thisType*& parentSnd, thisType*& root)
      {  COL::ImplBinaryNode *aparentFst = parentFst, *aparentSnd = parentSnd, *aroot = root;
         bool hasChangedSnd = parentFst && (parentFst->sfollows() == fst),
              hasChangedFst = parentSnd && (parentSnd->sfollows() == snd);
         inherited::swap(fst, snd, aparentFst, aparentSnd, aroot);
         parentFst = (thisType*) aparentFst;
         parentSnd = (thisType*) aparentSnd;
         if (hasChangedSnd)
            parentSnd->changeFollows(snd);
         if (hasChangedFst)
            parentFst->changeFollows(fst);
         root = (thisType*) aroot;
      }
   static void swapNew(thisType* fst, thisType* snd, thisType* parentFst,
         thisType*& root)
      {  COL::ImplBinaryNode *aroot = root;
         bool hasChangedFst = parentFst && (parentFst->sfollows() == fst);
         inherited::swapNew(fst, snd, parentFst, aroot);
         if (hasChangedFst)
            parentFst->changeFollows(snd);
         root = (thisType*) aroot;
      }

   thisType* left() const { return (thisType*) inherited::left(); }
   thisType* right() const { return (thisType*) inherited::right(); }
   thisType* sfollows() const { return (thisType*) inherited::sfollows(); }

   static thisType* rotateRight(thisType* node, bool& doesPropagate)
      {  return (thisType*) inherited::rotateRight(node, doesPropagate); }
   static thisType* rotateLeft(thisType* node, bool& doesPropagate)
      {  return (thisType*) inherited::rotateLeft(node, doesPropagate); }
   static thisType* rotateLeftRight(thisType* node)
      {  return (thisType*) inherited::rotateLeftRight(node); }
   static thisType* rotateRightLeft(thisType* node)
      {  return (thisType*) inherited::rotateRightLeft(node); }

  public:
   TFollowingCell(typename TypeSubString::Char achar) : inherited(achar) {}
   TFollowingCell(const thisType& source) = default;
   TFollowingCell(TypeSubString& suffix, EnhancedObject* object) : inherited(suffix, object) {}
   TFollowingCell(const thisType& source, bool duplicateObject, const VirtualCast* retrieveRegistrationFromCopy=nullptr)
      : inherited(source, duplicateObject, retrieveRegistrationFromCopy) {}
   ~TFollowingCell();
   thisType& operator=(const thisType& source) = default;
   
   typedef typename COL::ImplBinaryNode::DescentTrace::TraceProperty<thisType> DescentTraceProperty;
#ifdef __GNUC__
   typedef typename inherited::DescentTrace InheritedDescentTrace;
   class DescentTrace : public InheritedDescentTrace {
     private:
      typedef InheritedDescentTrace inherited;
     public:
      DescentTrace(TFollowingCell<TypeSubString>* node=nullptr, int defaultHeight=5) : inherited(node, defaultHeight) {}
      DescentTrace(const DescentTrace& source) = default;
      DescentTrace& operator=(const DescentTrace& source) = default;
   };
#endif
};

template <class TypeSubString>
inline typename TExtendBalancedNode<typename TypeSubString::Char>::Balance
TFollowingCell<TypeSubString>::disconnectFollowing() {
   AssumeCondition(!(following().left() && following().right()))
   thisType* disconnect = (thisType*) &following();
   typename inherited::Balance result = inherited::BEqual;
   if (disconnect->left()) {
      result = inherited::BLeft;
      resetFollowing(disconnect->left());
   }
   else if (disconnect->right()) {
      result = inherited::BRight;
      resetFollowing(disconnect->right());
   }
   else
      inherited::clearFollows();
   disconnect->disconnect();
   return result;
}

template <class TypeSubString>
inline void
TFollowingCell<TypeSubString>::clear() {
   if (inherited::queryTypeField() != inherited::TSNone) {
      if (inherited::hasSuffix())
         delete &inherited::suffix();
      else if (inherited::hasFollowing()) {
         thisType* follows = (thisType*) &following();
         AssumeCondition(follows->isSingleton() && !follows->sfollows())
         delete follows;
      };
      inherited::clearFollows();
   };
}

template <class TypeSubString>
inline void
TFollowingCell<TypeSubString>::lightClear() {
   if (inherited::queryTypeField() != inherited::TSNone) {
      if (inherited::hasSuffix())
         delete &inherited::suffix();
      inherited::clearFollows();
   };
}

template <class TypeSubString>
inline void
TFollowingCell<TypeSubString>::free() {
   if (inherited::queryTypeField() != inherited::TSNone) {
      if (inherited::hasSuffix()) {
         SuffixObject<TypeSubString>* suffix = &inherited::suffix();
         suffix->free();
         delete suffix;
      }
      else if (inherited::hasFollowing()) {
         thisType* follows = (thisType*) &following();
         AssumeCondition(follows->isSingleton() && !follows->sfollows())
         delete follows;
      };
      inherited::clearFollows();
   };
}

template <class TypeSubString>
inline
TFollowingCell<TypeSubString>::~TFollowingCell()
   { lightClear(); }

template <class TypeSubString>
class LocateContinuation : public TLocateContinuation<TypeSubString,
      TExtendBalancedNode<typename TypeSubString::Char>, SuffixObject<TypeSubString> > {
  private:
   typedef LocateContinuation<TypeSubString> thisType;
   typedef TLocateContinuation<TypeSubString,
      TExtendBalancedNode<typename TypeSubString::Char>, SuffixObject<TypeSubString> > inherited;
  public:
   LocateContinuation(typename inherited::TypeFollowingCell& node) : inherited(node) {}
   LocateContinuation(const thisType& source) = default;
};

}} // end of namespace STG::DTernaryTree

#endif // STG_TernaryTreeBaseH

