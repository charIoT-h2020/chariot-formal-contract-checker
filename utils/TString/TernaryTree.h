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
// File      : TernaryTree.h
// Description :
//   Definition of a template class of TernaryTree. The objective
//   is an efficient data structure to implement dictionaries based
//   on a lexicographical ordering.
//

#ifndef STG_TernaryTreeH
#define STG_TernaryTreeH

#include "TString/TernaryTreeBase.h"

namespace STG {

/*****************************************************/
/* Definition of the template class TImplTernaryTree */
/*****************************************************/

template <class TypeSubString>
class TImplTernaryTree : public DTernaryTree::BaseTernaryTree {
  public:
   class Cursor;
   typedef EnhancedObject Element;

#ifndef _MSC_VER
   friend class DTernaryTree::BaseTernaryTree;
  protected:
#else
  public:
#endif

   typedef DTernaryTree::SuffixObject<TypeSubString> SuffixObject;
   typedef DTernaryTree::TFollowingCell<TypeSubString> FollowingCell;
   typedef typename DTernaryTree::TFollowingCell<TypeSubString>::DescentTrace CellDescentTrace;
   typedef DTernaryTree::LocateContinuation<TypeSubString> LocateContinuation;

   FollowingCell* pfcRoot;

  private:
   typedef DTernaryTree::BaseTernaryTree inherited;
   typedef TImplTernaryTree<TypeSubString> thisType;
   int uCount, uHeight;

   void copyCount(const thisType& source)
      {  uCount = source.uCount;
         uHeight = source.uHeight;
      }

  protected:
   friend class RouteFreeAction<TImplTernaryTree<TypeSubString> >;
   friend class RouteRemoveAction<TImplTernaryTree<TypeSubString> >;
   friend class RouteCopyAction<TImplTernaryTree<TypeSubString> >;
   friend class RouteValidityAction<TImplTernaryTree<TypeSubString> >;
   friend class RouteHeightValidityAction<TImplTernaryTree<TypeSubString> >;

  public:
   class Cursor : public EnhancedObject {
     public:
      typedef TImplTernaryTree<TypeSubString> Support;
     protected:
      typedef typename Support::FollowingCell FollowingCell;

     private:
      class DescentTrace;
      class FollowingCellAccess : public FollowingCell {
        private:
         typedef FollowingCell inherited;
         FollowingCellAccess(typename TypeSubString::Char achar) : FollowingCell(achar) {}
         friend class Cursor;
#if !defined(_MSC_VER)
         friend class Cursor::DescentTrace;
#endif
         friend class RouteFreeAction<TImplTernaryTree<TypeSubString> >;
         friend class RouteRemoveAction<TImplTernaryTree<TypeSubString> >;
         friend class RouteCopyAction<TImplTernaryTree<TypeSubString> >;
         friend class RouteValidityAction<TImplTernaryTree<TypeSubString> >;
         friend class RouteHeightValidityAction<TImplTernaryTree<TypeSubString> >;
         friend class TImplTernaryTree<TypeSubString>;

         friend class DTernaryTree::BaseTernaryTree;
#if defined(_MSC_VER)
        public:
         ExtendedComparisonResult compareSons(FollowingCell* fstSon, FollowingCell* sndSon) const
            {  return inherited::compareSons(fstSon, sndSon); }
#endif
         FollowingCellAccess* left() const { return (FollowingCellAccess*) inherited::left(); }
         FollowingCellAccess* right() const { return (FollowingCellAccess*) inherited::right(); }
         FollowingCellAccess* sfollows() const { return (FollowingCellAccess*) inherited::sfollows(); }
      };

      class DescentTrace : public CellDescentTrace {
        private:
         typedef CellDescentTrace inherited;

        protected:
         virtual ComparisonResult _compare(const EnhancedObject& asource) const override
            {  ComparisonResult result = inherited::_compare(asource);
               return (result == CREqual) ? comparePlus((const DescentTrace&) asource) : result;
            }

        public:
         DescentTrace(FollowingCell* node=nullptr, int defaultHeight=5) : inherited(node, defaultHeight) {}
         DescentTrace(const DescentTrace& source) = default;
         DescentTrace& operator=(const DescentTrace& source) = default;
         DefineCopy(DescentTrace)
         typedef typename TImplTernaryTree<TypeSubString>::ExtendedComparisonResult ExtendedComparisonResult;
         ExtendedComparisonResult comparePlus(const DescentTrace& source) const;
         const int& count() const { return CellDescentTrace::count(); }

         virtual bool isValid() const override;
         FollowingCellAccess* operator[](int index) const
            {  return (FollowingCellAccess*) inherited::operator[](index); }
      };
      friend class DescentTrace;
      DescentTrace dtTrace;

      friend class DTernaryTree::BaseTernaryTree;
      friend class RouteFreeAction<Support>;
      friend class RouteRemoveAction<Support>;
      friend class RouteCopyAction<Support>;
      friend class RouteValidityAction<Support>;
      friend class RouteHeightValidityAction<Support>;

     protected:
      virtual ComparisonResult _compare(const EnhancedObject& asource) const override
         {  ComparisonResult result = EnhancedObject::_compare(asource);
            return (result == CREqual)
               ? dtTrace.compare(((const Cursor&) asource).dtTrace) : result;
         }
      typedef typename Support::ExtendedComparisonResult ExtendedComparisonResult;
      ExtendedComparisonResult comparePlus(const Cursor& source) const
         {  return dtTrace.comparePlus(source.dtTrace); }

#ifdef __GNUC__
     public:
#endif
      bool setToLeft();
      bool setToRight();
      bool setToFollows();
      bool setToSon(FollowingCell* son);
      bool setToSon(COL::ImplBinaryNode::Balance balance);
      bool setToParent() { dtTrace.pop(); return dtTrace.count() > 0; }
      bool setToRoot(FollowingCell* root);
      bool setToRoot(const Support& tree);

      FollowingCellAccess* getParent() const
         {  AssumeCondition(dtTrace.count() > 0)
            return (dtTrace.count() > 1) ? (FollowingCellAccess*) dtTrace[dtTrace.count()-2] : nullptr;
         }
      FollowingCellAccess* getReference() const { return (FollowingCellAccess*) dtTrace[dtTrace.count()-1]; }

      FollowingCell* operator[](int index) const { return dtTrace[index]; }
      typename FollowingCell::DescentTraceProperty operator[](int index)
         {  return typename FollowingCell::DescentTraceProperty(dtTrace, index); }
      void removeAt(int index) { dtTrace.removeAt(index); }
      void insertAt(int index, FollowingCell* node) { dtTrace.insertAt(index, node); }
      void cut(int depth) { dtTrace.removeAllAfter(depth); }

      friend class DTernaryTree::BaseTernaryTree::CursorMove;
#ifdef __GNUC__
     public:
#endif
      bool setToBinaryNext(const Support& tree)
         {  return Support::setToBinaryNext(*this, tree.pfcRoot); }
      bool setToBinaryPrevious(const Support& tree)
         {  return Support::setToBinaryPrevious(*this, tree.pfcRoot); }
      bool setToInnerLeft(TypeSubString* subString=nullptr)
         {  Support::setToInnerLeft(*this, subString);
            return getReference()->hasSuffix();
         }
      bool setToInnerRight(TypeSubString* subString=nullptr)
         {  Support::setToInnerRight(*this, subString);
            return getReference()->hasSuffix();
         }

#ifdef __GNUC__
     protected:
#endif
      Cursor(const Support& tree, Copy) : dtTrace(nullptr, tree.uHeight) {}
      friend class TImplTernaryTree<TypeSubString>;

     public:
      Cursor(const Support& tree)
         :  dtTrace(nullptr, tree.uHeight+1) {}
      Cursor(const Cursor& source) = default;
      Cursor& operator=(const Cursor& source) = default;
      DefineCopy(Cursor)
      DDefineAssign(Cursor)

      virtual bool isValid() const override { return (dtTrace.count() > 0); }
      const int& depth() const { return dtTrace.count(); }
      bool invariant() const { return EnhancedObject::isValid() && dtTrace.isValid(); }
      TypeSubString stringAt() const;
      TypeSubString prefixAt() const;
      EnhancedObject* elementAt() const;
      bool isEqual(const Cursor& source) const
         {  int traceLength = dtTrace.count();
            return (traceLength == source.dtTrace.count()) &&
               ((traceLength == 0) || dtTrace[traceLength-1] == source.dtTrace[traceLength-1]);
         }
      bool isPrefixOf(const Cursor& source) const
         {  int traceLength = dtTrace.count();
            return (traceLength <= source.dtTrace.count()) &&
               ((traceLength == 0) || dtTrace[traceLength-1] == source.dtTrace[traceLength-1]);
         }

      void invalidate() { dtTrace.invalidate();  }
      void swap(Cursor& source) { dtTrace.swap(source.dtTrace); }
      bool setToLeftRighter(TypeSubString* subString=nullptr);
      bool setToRightLefter(TypeSubString* subString=nullptr);
      bool setToFollowsRighter(TypeSubString* subString=nullptr);
      bool setToFollowsLefter(TypeSubString* subString=nullptr);
      
      bool setToFirst(const Support& tree, TypeSubString* subString=nullptr)
         {  return Support::setToFirst(*this, tree.pfcRoot, subString); }
      bool setToLast(const Support& tree, TypeSubString* subString=nullptr)
         {  return Support::setToLast(*this, tree.pfcRoot, subString); }
      bool setToNext(const Support& tree, TypeSubString* subString=nullptr)
         {  return Support::setToNext(*this, tree.pfcRoot, subString); }
      bool setToPrevious(const Support& tree, TypeSubString* subString=nullptr)
         {  return Support::setToPrevious(*this, tree.pfcRoot, subString); }
      bool setToInnerDepth(const Support& tree, TypeSubString* subString=nullptr)
         {  return Support::setToInnerDepth(*this, tree.pfcRoot, subString); }
      bool setToLefter(TypeSubString* subString=nullptr)
         {  return Support::setToLefter(*this, subString); }
      bool setToRighter(TypeSubString* subString=nullptr)
         {  return Support::setToRighter(*this, subString); }
      bool setToSon(typename TypeSubString::Char achar, const Support& tree)
         {  return Support::setToSon(*this, achar, tree.pfcRoot, (TypeSubString*) nullptr); }
      const SuffixObject& suffix() const { return getReference()->suffix(); }
      bool isFinalValid() const;
      bool isUpRoot() const { return dtTrace.count() == 0; }

      bool hasLeaf() const { return Support::hasLeaf(*this); }
      bool isJustAfter(const Cursor& source) const;
      bool isJustBefore(const Cursor& source) const;
      bool isJustAfterKey(const TypeSubString& key) const;
      bool isJustBeforeKey(const TypeSubString& key) const;
      bool isInnerLeft(const Cursor* source=nullptr) const;
      bool isInnerRight(const Cursor* source=nullptr) const;
   };
   friend class Cursor;
   friend class Cursor::DescentTrace;

   typedef FollowingCell Node;

  public:
   // Notification methods
   static typename FollowingCell::DescentTrace& trace(Cursor& cursor) { return cursor.dtTrace; }
   class InsertionNotification : public COL::BinaryInsertionNotification {
     public:
      InsertionNotification() {}
      InsertionNotification(const InsertionNotification& source) = default;

      void apply(Cursor& cursor) const { COL::BinaryInsertionNotification::apply(trace(cursor)); }
   };
   class SuppressionNotification : public COL::BinarySuppressionNotification {
     private:
      FollowingCell* pfcFollowRemoved;

     public:
      SuppressionNotification() : pfcFollowRemoved(nullptr) {}
      SuppressionNotification(const SuppressionNotification& source) = default;

      void setFollowRemoved(FollowingCell* followRemoved)
         {  pfcFollowRemoved = followRemoved; }
      void apply(Cursor& cursor) const
         {  if (pfcFollowRemoved && isAfterRemoved(trace(cursor), pfcFollowRemoved))
               cursor.invalidate();
            else
               COL::BinarySuppressionNotification::apply(trace(cursor));
         }
   };
   class SplitSuffixNotification {
     private:
      FollowingCell* pfOldReference;

     public:
      SplitSuffixNotification(FollowingCell& reference) : pfOldReference(&reference) {}
      SplitSuffixNotification(const SplitSuffixNotification& source) = default;
      void applyFinal(Cursor& cursor)
         {  if (cursor.isValid() && cursor.getReference() == pfOldReference) {
               while (cursor.setToFollows());
            };
         }
      bool isValid() const { return pfOldReference; }
   };
   friend class InsertionNotification;
   friend class SuppressionNotification;
   friend class SplitSuffixNotification;

  private:
   typedef typename Cursor::FollowingCellAccess FollowingCellAccess;

   void clearRoot() { pfcRoot = nullptr; uCount = uHeight = 0; }

   void setElement(Cursor& cursor, int index, FollowingCell* node);
   void insertElementAt(Cursor& cursor, int index, FollowingCell* node);

   typedef COL::ImplBinaryTreeCursorNotification CursorsNotification;
   bool balanceAfterInsertion(Cursor& cursor, CursorsNotification* notifications);
   bool balanceAfterSuppression(Cursor& cursor, typename Node::Balance balance, CursorsNotification* notifications);

  public:
   TImplTernaryTree() : pfcRoot(nullptr), uCount(0), uHeight(0) {}
   TImplTernaryTree(thisType&& source) { swap(source); }
   TImplTernaryTree(const thisType& source, bool duplicateObject=false,
         const VirtualCast* retrieveRegistrationFromCopy=nullptr)
      : inherited(source)
      { if (duplicateObject)
           fullAssign(source, retrieveRegistrationFromCopy);
        else
           operator=(source);
      }
   thisType& operator=(const thisType& source);
   TemplateDefineCopy(TImplTernaryTree, TypeSubString)
   DTemplateDefineAssign(TImplTernaryTree, TypeSubString)

   thisType& fullAssign(const thisType& source, const VirtualCast* retrieveRegistrationFromCopy=nullptr);
   const int& count() const { return uCount; }
   bool isEmpty() const { return !pfcRoot; }
   void swap(thisType& source);

   LocationResult locate(TypeSubString& subString, Cursor& cursor) const
      {  return inherited::tlocate(*this, subString, cursor); }
   void addAsSon(TypeSubString& subString, EnhancedObject* object, Cursor& cursor,
         LocationResult result, InsertionNotification* notification=nullptr);
   DTernaryTree::BaseTernaryTree::CursorMove removeAt(Cursor& cursor, bool doesFree,
         SuppressionNotification* notification=nullptr);
   COL::ImplBinaryNode::Balance adjustRemove(Cursor& cursor,
         COL::VirtualCollection::RelativePosition pos,
         DTernaryTree::BaseTernaryTree::CursorMove move) const
      {  return move.applyMove(cursor, *this, (pos == COL::VirtualCollection::RPAfter)
            ? COL::ImplBinaryNode::BRight : ((pos == COL::VirtualCollection::RPBefore)
            ? COL::ImplBinaryNode::BLeft : COL::ImplBinaryNode::BEqual));
      }

   void replaceAt(Cursor& cursor, EnhancedObject* object, bool doesFree);
   void freeAll();
   void removeAll();

   virtual bool isValid() const override;
};

/* Definition of the inline methods of the class TImplTernaryTree::Cursor */

template <class TypeSubString>
inline bool
TImplTernaryTree<TypeSubString>::Cursor::setToLeft() {
   FollowingCell* son = getReference()->left();
   dtTrace.push(son);
   return son;
}

template <class TypeSubString>
inline bool
TImplTernaryTree<TypeSubString>::Cursor::setToRight() {
   FollowingCell* son = getReference()->right();
   dtTrace.push(son);
   return son;
}

template <class TypeSubString>
inline bool
TImplTernaryTree<TypeSubString>::Cursor::setToFollows() {
   FollowingCell* son = getReference()->sfollows();
   dtTrace.push(son);
   return son;
}

template <class TypeSubString>
inline bool
TImplTernaryTree<TypeSubString>::Cursor::setToSon(FollowingCell* son) {
   AssumeCondition(son
         && ((getReference()->left() == son) || (getReference()->right() == son)
            || (getReference()->sfollows() == son)))
   dtTrace.push(son);
   return true;
}

template <class TypeSubString>
inline bool
TImplTernaryTree<TypeSubString>::Cursor::setToSon(COL::ImplBinaryNode::Balance balance) {
   return dtTrace.push((balance == COL::ImplBinaryNode::BLeft) ? getReference()->left()
         : (balance == COL::ImplBinaryNode::BRight) ? getReference()->right()
         : getReference()->sfollows());
}

template <class TypeSubString>
inline bool
TImplTernaryTree<TypeSubString>::Cursor::setToLeftRighter(TypeSubString* subString)
   {  return setToLeft() && setToInnerRight(subString); }

template <class TypeSubString>
inline bool
TImplTernaryTree<TypeSubString>::Cursor::setToRightLefter(TypeSubString* subString)
   {  return setToRight() && setToInnerLeft(subString); }

template <class TypeSubString>
inline bool
TImplTernaryTree<TypeSubString>::Cursor::setToFollowsLefter(TypeSubString* subString)
   {  if (subString)
         subString->cat(getReference()->getChar());
      return setToFollows() && setToInnerLeft(subString);
   }

template <class TypeSubString>
inline bool
TImplTernaryTree<TypeSubString>::Cursor::setToFollowsRighter(TypeSubString* subString)
   {  if (subString)
         subString->cat(getReference()->getChar());
      return setToFollows() && setToInnerRight(subString);
   }

template <class TypeSubString>
inline bool
TImplTernaryTree<TypeSubString>::Cursor::setToRoot(FollowingCell* root) {
   invalidate();
   dtTrace.push(root);
   return root;
}

template <class TypeSubString>
inline bool
TImplTernaryTree<TypeSubString>::Cursor::setToRoot(const Support& tree)
   { return setToRoot(tree.pfcRoot); }

template <class TypeSubString>
inline EnhancedObject*
TImplTernaryTree<TypeSubString>::Cursor::elementAt() const {
   FollowingCellAccess* last = getReference();
   AssumeCondition(last->isSuffix())
   return last->suffix().getObject();
}

template <class TypeSubString>
inline bool
TImplTernaryTree<TypeSubString>::Cursor::isFinalValid() const
   { return Cursor::isValid() && getReference()->hasSuffix(); }

/* Definition of the inline methods of the template class TImplTernaryTree */

template <class TypeSubString>
inline void
TImplTernaryTree<TypeSubString>::swap(thisType& source) {
   FollowingCell* rootTemp = pfcRoot;
   int countTemp = uCount, heightTemp = uHeight;
   pfcRoot = source.pfcRoot;
   uCount = source.uCount;
   uHeight = source.uHeight;
   source.pfcRoot = rootTemp;
   source.uCount = countTemp;
   source.uHeight = heightTemp;
}

template <class TypeSubString>
inline void
TImplTernaryTree<TypeSubString>::setElement(Cursor& cursor, int index, FollowingCell* node) {
   if (index > 0)
      cursor[index-1]->changeSon(cursor[index], node);
   else
      pfcRoot = node;
   cursor[index] = node;
}

template <class TypeSubString>
inline void
TImplTernaryTree<TypeSubString>::insertElementAt(Cursor& cursor, int index, FollowingCell* node) {
   if (index > 0)
      cursor[index-1]->changeSon(cursor[index], node);
   else
      pfcRoot = node;
   cursor.insertAt(index, node);
}

template <class TypeSubString>
   inline void
TImplTernaryTree<TypeSubString>::replaceAt(Cursor& cursor, EnhancedObject* object, bool doesFree)
   { cursor.getReference()->suffix().replaceObject(object, doesFree); }

} // end of namespace STG

#endif // STG_TernaryTreeH

