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
// File      : TernaryTreeParent.template
// Description :
//   Definition of a template class of TernaryTreeParent whose nodes
//   have a back reference to their parent node.
//

#ifndef STG_TernaryTreeParentH
#define STG_TernaryTreeParentH

#include "Collection/Implementation/BinaryParent.h"
#include "TString/TernaryTreeBase.h"

namespace STG {

template <class TypeSubString>
class TImplParentTernaryTree;

namespace DTernaryTree {

template <class TypeSubString>
class TParentFollowingCell;

template <class TypeSubString>
class ParentSuffixObject : public EnhancedObject, public BaseSuffixObject<TypeSubString> {
  private:
   typedef EnhancedObject inherited;
   typedef ParentSuffixObject<TypeSubString> thisType;
   
   TParentFollowingCell<TypeSubString>* pfcParent;

  protected:
   ParentSuffixObject(TypeSubString& suffix) : BaseSuffixObject<TypeSubString>(suffix), pfcParent(nullptr) {}
   TParentFollowingCell<TypeSubString>*& parent() { return pfcParent; }
   TParentFollowingCell<TypeSubString>* parent() const { return pfcParent; }
   friend class TParentFollowingCell<TypeSubString>;
   friend class STG::TImplParentTernaryTree<TypeSubString>;

   thisType& assignSuffix(const thisType& source)
      {  return (ParentSuffixObject<TypeSubString>&) BaseSuffixObject<TypeSubString>::operator=(source); }

  public:
   ParentSuffixObject() : pfcParent(nullptr) {}
   ParentSuffixObject(const thisType& source, bool duplicateObject)
      : inherited(source), BaseSuffixObject<TypeSubString>(source), pfcParent(nullptr) {}
   virtual ~ParentSuffixObject() {}
   ParentSuffixObject& operator=(const ParentSuffixObject& source)
      {  inherited::operator=(source);
         BaseSuffixObject<TypeSubString>::operator=(source);
         return *this;
      }
   TemplateDefineCopy(ParentSuffixObject, TypeSubString)
   DTemplateDefineAssign(ParentSuffixObject, TypeSubString)

   static thisType* createSuffixObject(TypeSubString& subString, EnhancedObject* object)
      {  ((thisType*) object)->BaseSuffixObject<TypeSubString>::setSuffix(subString);
         return (thisType*) object;
      }
   EnhancedObject* getObject() const { return const_cast<thisType*>(this); }
   thisType* clone(bool duplicateObject, const VirtualCast* retrieveRegistrationFromCopy) const
      {  AssumeCondition(duplicateObject)
         return retrieveRegistrationFromCopy
            ? (thisType*) retrieveRegistrationFromCopy->castFrom(createCopy())
            : createSCopy();
      }
};

template <typename TypeChar>
class TExtendParentBalancedNode : public TExtendNode<COL::ImplParentBalancedNode, TypeChar> {
  private:
   typedef TExtendNode<COL::ImplParentBalancedNode, TypeChar> inherited;
   typedef TExtendParentBalancedNode<TypeChar> thisType;

  public:
   TExtendParentBalancedNode(TypeChar achar) : inherited(achar) {}
   TExtendParentBalancedNode(const thisType& source) : inherited(source) {}
};

template <>
class TExtendParentBalancedNode<char> : public TExtendNodeForChar<COL::ImplParentBalancedNode, char, sizeof(char)*8> {
  private:
   typedef TExtendNodeForChar<COL::ImplParentBalancedNode, char, sizeof(char)*8> inherited;
   typedef TExtendParentBalancedNode<char> thisType;

  public:
   TExtendParentBalancedNode(char achar) : inherited(achar) {}
   TExtendParentBalancedNode(const thisType& source) : inherited(source) {}
};

template <>
#ifdef __GNUC__
class TExtendParentBalancedNode<wchar_t> : public TExtendNode<COL::ImplParentBalancedNode, wchar_t>
#else
class TExtendParentBalancedNode<wchar_t> : public TExtendNodeForChar<COL::ImplParentBalancedNode, wchar_t, sizeof(wchar_t)*8>
#endif
{ private:
#ifdef __GNUC__
   typedef TExtendNode<COL::ImplParentBalancedNode, wchar_t> inherited;
#else
   typedef TExtendNodeForChar<COL::ImplParentBalancedNode, wchar_t, sizeof(wchar_t)*8> inherited;
#endif
   typedef TExtendParentBalancedNode<wchar_t> thisType;

  public:
   TExtendParentBalancedNode(wchar_t achar) : inherited(achar) {}
   TExtendParentBalancedNode(const thisType& source) : inherited(source) {}
};

template <class TypeSubString>
class TParentFollowingCell : public TBaseFollowingCell<TypeSubString,
      TExtendParentBalancedNode<typename TypeSubString::Char>, ParentSuffixObject<TypeSubString> > {
  private:
   typedef TParentFollowingCell<TypeSubString> thisType;
   typedef TBaseFollowingCell<TypeSubString, TExtendParentBalancedNode<typename TypeSubString::Char>,
      ParentSuffixObject<TypeSubString> > inherited;

  protected:
   friend class STG::TImplParentTernaryTree<TypeSubString>;
   friend class BaseTernaryTree;

   friend class ParentSuffixObject<TypeSubString>;
   friend class BaseSuffixObject<TypeSubString>;
   friend class BaseSuffixMethods;

   void setSuffix(TypeSubString& subString, ParentSuffixObject<TypeSubString>* suffixObject);
   void absorbSuffix(ParentSuffixObject<TypeSubString>* suffixObject);
   thisType& following() const { return (thisType&) inherited::following(); }
   void setFollowing(thisType* decision);
   void resetFollowing(thisType* decision);
   void disconnect();
   typename TExtendParentBalancedNode<typename TypeSubString::Char>::Balance disconnectFollowing();
   void clear();
   void clearSuffix();
   void lightClear();
   void free();
   void freeSuffix();
   void freeFollowing();
   void splitSuffix(TypeSubString& string);
   static void swap(thisType* fst, thisType* snd, thisType*& root);
   static void swapNew(thisType* fst, thisType* snd, thisType*& root);

   thisType* left() const { return (thisType*) inherited::left(); }
   thisType* right() const { return (thisType*) inherited::right(); }
   thisType* sfollows() const { return (thisType*) inherited::sfollows(); }
   thisType* parent() const { return (thisType*) inherited::parent(); }

   static thisType* rotateRight(thisType* node, bool& propagation)
      {  thisType* parent = node->parent();
         thisType* result = (thisType*) inherited::rotateRight(node, propagation);
         if (parent && !result->parent()) {
            AssumeCondition(parent->sfollows() == node)
            result->inherited::parent() = parent;
            parent->changeFollows(result);
         };
         return result;
      }
   static thisType* rotateLeft(thisType* node, bool& propagation)
      {  thisType* parent = node->parent();
         thisType* result = (thisType*) inherited::rotateLeft(node, propagation);
         if (parent && !result->parent()) {
            AssumeCondition(parent->sfollows() == node)
            result->inherited::parent() = parent;
            parent->changeFollows(result);
         };
         return result;
      }
   static thisType* rotateLeftRight(thisType* node)
      {  thisType* parent = node->parent();
         thisType* result = (thisType*) inherited::rotateLeftRight(node);
         if (parent && !result->parent()) {
            AssumeCondition(parent->sfollows() == node)
            result->inherited::parent() = parent;
            parent->changeFollows(result);
         };
         return result;
      }
   static thisType* rotateRightLeft(thisType* node)
      {  thisType* parent = node->parent();
         thisType* result = (thisType*) inherited::rotateRightLeft(node);
         if (parent && !result->parent()) {
            AssumeCondition(parent->sfollows() == node)
            result->inherited::parent() = parent;
            parent->changeFollows(result);
         };
         return result;
      }

  public:
   TParentFollowingCell(typename TypeSubString::Char achar) : inherited(achar) {}
   TParentFollowingCell(const thisType& source) : inherited(source) {}
   TParentFollowingCell(TypeSubString& suffix, EnhancedObject* object)
      : inherited(suffix, object) { inherited::suffix().parent() = this; }
   TParentFollowingCell(const thisType& source, bool duplicateObject,
         const VirtualCast* retrieveRegistrationFromCopy=nullptr)
      : inherited(source, duplicateObject, retrieveRegistrationFromCopy)
      {  if (inherited::hasSuffix())
            inherited::suffix().parent() = this;
      }
   ~TParentFollowingCell() { lightClear(); }
   thisType& operator=(const thisType& source) { return (thisType&) inherited::operator=(source); }
};

template <class TypeSubString>
inline void
TParentFollowingCell<TypeSubString>::setSuffix(TypeSubString& subString, ParentSuffixObject<TypeSubString>* suffixObject) {
   inherited::setSuffix(subString, suffixObject);
   inherited::suffix().parent() = this;
}

template <class TypeSubString>
inline void
TParentFollowingCell<TypeSubString>::absorbSuffix(ParentSuffixObject<TypeSubString>* suffixObject) {
   inherited::absorbSuffix(suffixObject);
   inherited::suffix().parent() = this;
}

template <class TypeSubString>
inline void
TParentFollowingCell<TypeSubString>::setFollowing(thisType* decision) {
   inherited::setFollowing(decision);
   decision->inherited::parent() = this;
}

template <class TypeSubString>
inline void
TParentFollowingCell<TypeSubString>::resetFollowing(thisType* decision) {
   inherited::resetFollowing(decision);
   decision->inherited::parent() = this;
}

template <class TypeSubString>
inline void
TParentFollowingCell<TypeSubString>::disconnect() {
   inherited::disconnect();
   if (inherited::hasSuffix())
      inherited::suffix().parent() = nullptr;
   else if (inherited::hasFollowing())
      following().inherited::parent() = nullptr;
   inherited::clearFollows();
}

template <class TypeSubString>
inline typename TExtendParentBalancedNode<typename TypeSubString::Char>::Balance
TParentFollowingCell<TypeSubString>::disconnectFollowing() {
   AssumeCondition(!(following().left() && following().right()))
   thisType* disconnectCell = &following();
   typename inherited::Balance result = inherited::BEqual;
   if (disconnectCell->left()) {
      thisType* newFollowing = disconnectCell->left();
      result = inherited::BLeft;
      disconnectCell->disconnect();
      resetFollowing(newFollowing);
   }
   else if (disconnectCell->right()) {
      thisType* newFollowing = disconnectCell->right();
      result = inherited::BRight;
      disconnectCell->disconnect();
      resetFollowing(newFollowing);
   }
   else {
      disconnectCell->disconnect();
      inherited::clearFollows();
   };
   return result;
}

template <class TypeSubString>
inline void
TParentFollowingCell<TypeSubString>::clear() {
   if (!inherited::isUndefined()) {
      if (inherited::hasSuffix()) {
         inherited::suffix().clear();
         inherited::suffix().parent() = nullptr;
      }
      else if (inherited::hasFollowing()) {
         thisType* follows = &following();
         AssumeCondition(follows->isSingleton() && !follows->sfollows())
         delete follows;
      };
      inherited::clearFollows();
   };
}

template <class TypeSubString>
inline void
TParentFollowingCell<TypeSubString>::clearSuffix() {
   inherited::suffix().parent() = nullptr;
   inherited::suffix().clear();
   inherited::clearFollows();
}

template <class TypeSubString>
inline void
TParentFollowingCell<TypeSubString>::lightClear() {
   if (!inherited::isUndefined()) {
      if (inherited::hasSuffix())
         inherited::suffix().parent() = nullptr;
      else if (inherited::hasFollowing())
         following().inherited::parent() = nullptr;
      inherited::clearFollows();
   };
}

template <class TypeSubString>
inline void
TParentFollowingCell<TypeSubString>::free() {
   if (!inherited::isUndefined()) {
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
TParentFollowingCell<TypeSubString>::freeSuffix() {
   delete &inherited::suffix();
   inherited::clearFollows();
}

template <class TypeSubString>
inline void
TParentFollowingCell<TypeSubString>::swap(thisType* fst, thisType* snd, thisType*& root) {
   COL::ImplBinaryNode *aroot = root;
   thisType *sndParent = fst->parent(), *fstParent = snd->parent();
   if (sndParent && (sndParent->sfollows() != fst))
      sndParent = nullptr;
   if (fstParent && (fstParent->sfollows() != snd))
      fstParent = nullptr;
   inherited::swap(fst, snd, aroot);
   root = (thisType*) aroot;
   if (fstParent) {
      fst->inherited::parent() = fstParent;
      fstParent->changeFollows(fst);
   };
   if (sndParent) {
      snd->inherited::parent() = sndParent;
      sndParent->changeFollows(snd);
   };
}

template <class TypeSubString>
inline void
TParentFollowingCell<TypeSubString>::swapNew(thisType* fst, thisType* snd, thisType*& root) {
   COL::ImplBinaryNode *aroot = root;
   thisType *sndParent = fst->parent();
   if (sndParent && sndParent->sfollows() != fst)
      sndParent = nullptr;
   inherited::swapNew(fst, snd, aroot);
   root = (thisType*) aroot;
   if (sndParent) {
      snd->inherited::parent() = sndParent;
      sndParent->changeFollows(snd);
   };
}

template <class TypeSubString>
class ParentLocateContinuation : public TLocateContinuation<TypeSubString,
      TExtendParentBalancedNode<typename TypeSubString::Char>, ParentSuffixObject<TypeSubString> > {
  private:
   typedef ParentLocateContinuation<TypeSubString> thisType;
   typedef TLocateContinuation<TypeSubString,
      TExtendParentBalancedNode<typename TypeSubString::Char>, ParentSuffixObject<TypeSubString> > inherited;
  public:
   ParentLocateContinuation(typename inherited::TypeFollowingCell& node) : inherited(node) {}
   ParentLocateContinuation(const thisType& source) : inherited(source) {}
};

} // end of namespace DTernaryTree

/***********************************************/
/* Définition du patron TImplParentTernaryTree */
/***********************************************/

template <class TypeSubString>
class TImplParentTernaryTree : public DTernaryTree::BaseTernaryTree {
#ifndef _MSC_VER
  protected:
   friend class DTernaryTree::BaseTernaryTree;

#else
  public:
#endif

   typedef DTernaryTree::TParentFollowingCell<TypeSubString> FollowingCell;
   typedef DTernaryTree::ParentLocateContinuation<TypeSubString> LocateContinuation;
   typedef DTernaryTree::ParentSuffixObject<TypeSubString> SuffixObject;

   FollowingCell* pfcRoot;

  private:
   typedef DTernaryTree::BaseTernaryTree inherited;
   typedef TImplParentTernaryTree<TypeSubString> thisType;
   int uCount;

   void copyCount(const thisType& source) { uCount = source.uCount; }

  public:
   class Cursor;
   typedef SuffixObject Element;

  protected:
   class SuppressionNotification;

   friend class RouteFreeAction<TImplParentTernaryTree<TypeSubString> >;
   friend class RouteRemoveAction<TImplParentTernaryTree<TypeSubString> >;
   friend class RouteCopyAction<TImplParentTernaryTree<TypeSubString> >;
   friend class RouteValidityAction<TImplParentTernaryTree<TypeSubString> >;

  public:
   class Cursor : public EnhancedObject { // Hériter de ImplParentBinaryTreeCursor
     protected:
      typedef TImplParentTernaryTree<TypeSubString> Support;
      typedef typename TImplParentTernaryTree<TypeSubString>::FollowingCell FollowingCell;

     private:
      class FollowingCellAccess : public FollowingCell {
        private:
         FollowingCellAccess(typename TypeSubString::Char achar) : FollowingCell(achar) {}

         friend class Cursor;
         friend class RouteFreeAction<TImplParentTernaryTree<TypeSubString> >;
         friend class RouteRemoveAction<TImplParentTernaryTree<TypeSubString> >;
         friend class RouteCopyAction<TImplParentTernaryTree<TypeSubString> >;
         friend class RouteValidityAction<TImplParentTernaryTree<TypeSubString> >;
         friend class TImplParentTernaryTree<TypeSubString>;
         friend class SuppressionNotification;
         friend class DTernaryTree::BaseTernaryTree;

         FollowingCellAccess* left() const { return (FollowingCellAccess*) Cursor::FollowingCell::left(); }
         FollowingCellAccess* right() const { return (FollowingCellAccess*) Cursor::FollowingCell::right(); }
         FollowingCellAccess* sfollows() const { return (FollowingCellAccess*) Cursor::FollowingCell::sfollows(); }
         FollowingCellAccess* parent() const { return (FollowingCellAccess*) Cursor::FollowingCell::parent(); }
         bool hasPrefix(TypeSubString& key) const;
      };
      friend class FollowingCellAccess;

      typedef EnhancedObject inherited;
      FollowingCellAccess* pfcElement;

      friend class DTernaryTree::BaseTernaryTree;
      friend class SuppressionNotification;
      friend class RouteFreeAction<TImplParentTernaryTree<TypeSubString> >;
      friend class RouteRemoveAction<TImplParentTernaryTree<TypeSubString> >;
      friend class RouteCopyAction<TImplParentTernaryTree<TypeSubString> >;
      friend class RouteValidityAction<TImplParentTernaryTree<TypeSubString> >;

     protected:
      virtual ComparisonResult _compare(const EnhancedObject& asource) const override
         {  ComparisonResult result = inherited::_compare(asource);
            return (result == CREqual) ? comparePlus((const Cursor&) asource) : result;
         }
      typedef typename Support::ExtendedComparisonResult ExtendedComparisonResult;
      ExtendedComparisonResult comparePlus(const Cursor& source) const;

#ifdef __GNUC__
     public:
#endif
      bool setToLeft()
         {  return pfcElement->left()
               ? (bool) (pfcElement = (FollowingCellAccess*) pfcElement->left()) : false;
         }
      bool setToRight()
         {  return pfcElement->right()
               ? (bool) (pfcElement = (FollowingCellAccess*) pfcElement->right()) : false;
         }
      bool setToFollows()
         {  return pfcElement->hasFollowing()
               ? (bool) (pfcElement = (FollowingCellAccess*) pfcElement->sfollows()) : false;
         }
      bool setToSon(FollowingCell* son)
         {  AssumeCondition(son && ((pfcElement->left() == son)
                  || (pfcElement->right() == son) || (pfcElement->sfollows() == son)))
            pfcElement = (FollowingCellAccess*) son;
            return true;
         }
      bool setToSon(COL::ImplBinaryNode::Balance balance)
         {  AssumeCondition(pfcElement)
            FollowingCell* newReference =
               (balance == COL::ImplBinaryNode::BLeft) ? pfcElement->left()
               : ((balance == COL::ImplBinaryNode::BRight) ? pfcElement->right()
               : pfcElement->sfollows());
            return (newReference) ? ((pfcElement = (FollowingCellAccess*) newReference) != nullptr) : false;
         }
      bool setToParent()
         {  if (pfcElement)
               pfcElement = pfcElement->parent();
            return pfcElement;
         }
      void setToReference(FollowingCell* element)
         {  AssumeCondition(element) pfcElement = (FollowingCellAccess*) element; }
      bool setToRoot(FollowingCell* root)
         {  pfcElement = (FollowingCellAccess*) root;
            return pfcElement;
         }
      bool setToRoot(const Support& tree)
         {  pfcElement = (FollowingCellAccess*) tree.pfcRoot;
            return pfcElement;
         }

      friend class DTernaryTree::BaseTernaryTree::CursorMove;
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

      FollowingCellAccess* getParent() const
         { AssumeCondition(pfcElement) return pfcElement->parent(); }
      FollowingCellAccess* getReference() const
         { AssumeCondition(pfcElement) return (FollowingCellAccess*) pfcElement; }

      Cursor() : pfcElement(nullptr) {}
      friend class TImplParentTernaryTree<TypeSubString>;
      Cursor(const Support& ittTree, Copy) : pfcElement(nullptr) {}

     public:
      Cursor(const Support& tree) : pfcElement(nullptr) {}
      Cursor(const Cursor& source) : inherited(source), pfcElement(source.pfcElement) {}
      Cursor& operator=(const Cursor& source)
         {  inherited::operator=(source);
            pfcElement = source.pfcElement;
            return *this;
         }
      DefineCopy(Cursor)
      DDefineAssign(Cursor)

      virtual bool isValid() const override { return pfcElement != nullptr; }
      TypeSubString stringAt() const;
      TypeSubString prefixAt() const;
      EnhancedObject* elementAt() const
         {  AssumeCondition(pfcElement && pfcElement->hasSuffix())
            return pfcElement->suffix().getObject();
         }
      bool isEqual(const Cursor& cursor) const { return pfcElement == cursor.pfcElement; }
      bool isPrefixOf(const Cursor& cursor) const
         {  bool result = (pfcElement == nullptr);
            if (!result) {
               FollowingCellAccess* cursorCell = cursor.pfcElement;
               while (cursorCell && cursorCell != pfcElement)
                  cursorCell = cursorCell->parent();
               result = (cursorCell == pfcElement);
            };
            return result;
         }

      void invalidate() { pfcElement = nullptr;  }
      void swap(Cursor& source)
         {  FollowingCellAccess* temp = source.pfcElement; source.pfcElement = pfcElement; pfcElement = temp; }
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
      bool setToLeftRighter(TypeSubString* subString=nullptr)
         {  return setToLeft() && setToInnerRight(subString); }
      bool setToRightLefter(TypeSubString* subString=nullptr)
         {  return setToRight() && setToInnerLeft(subString); }
      bool setToFollowsRighter(TypeSubString* subString=nullptr)
         {  if (subString)
               subString->cat(getReference()->getChar());
            return setToFollows() && setToInnerRight(subString); }
      bool setToFollowsLefter(TypeSubString* subString=nullptr)
         {  if (subString)
               subString->cat(getReference()->getChar());
            return setToFollows() && setToInnerLeft();
         }
      bool setToSon(typename TypeSubString::Char achar, const Support& tree)
         {  return Support::setToSon(*this, achar, tree.pfcRoot, (TypeSubString*) nullptr); }
      const SuffixObject& suffix() const { return getReference()->suffix(); }
      bool isFinalValid() const
         { return (pfcElement != nullptr) && pfcElement->hasSuffix(); }
      bool isUpRoot() const { return pfcElement == nullptr; }

      bool hasLeaf() const { return Support::hasLeaf(*this); }
      bool isJustAfter(const Cursor& source) const;
      bool isJustBefore(const Cursor& source) const;
      bool isJustAfterKey(const TypeSubString& key) const;
      bool isJustBeforeKey(const TypeSubString& key) const;
      bool isInnerLeft(const Cursor* source=nullptr) const;
      bool isInnerRight(const Cursor* source=nullptr) const;
   };
   friend class Cursor;
   typedef FollowingCell Node;

  protected:
   // Méthodes de notification
   class SuppressionNotification {
     private:
      FollowingCell* pfcSuppressNode;
      FollowingCell* pfcSuppressSuffixNode;

      void suppressBetween(FollowingCell* reference, Cursor& cursor) const
         {  typename Cursor::FollowingCellAccess* index
               = (typename Cursor::FollowingCellAccess*) pfcSuppressSuffixNode;
            while (index && (index != reference) && (index != pfcSuppressNode))
               index = index->parent();
            if (index == reference)
               cursor.invalidate();
         }

     public:
      SuppressionNotification() : pfcSuppressNode(nullptr), pfcSuppressSuffixNode(nullptr) {}
      SuppressionNotification(const SuppressionNotification& source) = default;
      SuppressionNotification& operator=(const SuppressionNotification& source) = default;

      void setFollowRemoved(FollowingCell* followRemoved)
         {  if (pfcSuppressSuffixNode == nullptr)
               pfcSuppressSuffixNode = followRemoved;
         }

      void setStart(FollowingCell* suppressNode) { pfcSuppressNode = suppressNode; }
      void applySuffix(Cursor& cursor) const
         {  FollowingCell* reference = cursor.getReference();
            if ((pfcSuppressNode == reference) || (pfcSuppressSuffixNode == reference))
               cursor.invalidate();
         }
      void apply(Cursor& cursor) const
         {  FollowingCell* reference = cursor.getReference();
            if (pfcSuppressSuffixNode == nullptr) {
               if (pfcSuppressNode == reference)
                  cursor.invalidate();
            }
            else
               suppressBetween(reference, cursor);
         }
      void applyFinal(Cursor& cursor) const
         {  if (pfcSuppressNode == cursor.getReference())
               cursor.invalidate();
         }
     
      bool isValid() const { return pfcSuppressNode != nullptr; }
   };
   friend class SuppressionNotification;

   class SplitSuffixNotification {
     private:
      FollowingCell* pfOldReference;

     public:
      SplitSuffixNotification(FollowingCell& oldReference) : pfOldReference(&oldReference) {}
      SplitSuffixNotification(const SplitSuffixNotification& source) = default;
      void applyFinal(Cursor& cursor)
         {  if (cursor.isValid() && cursor.getReference() == pfOldReference) {
               while (cursor.setToFollows());
            };
         }
      bool isValid() const { return pfOldReference; }
   };
   friend class SplitSuffixNotification;

  private:
   typedef typename Cursor::FollowingCellAccess FollowingCellAccess;

   void clearRoot() { pfcRoot = nullptr; uCount = 0; }
   void setElement(FollowingCell* node)
      {  if (node->parent() == nullptr)
            pfcRoot = node;
      }
   void insertElementAt(FollowingCell* node)
      {  if (node->parent() == nullptr)
            pfcRoot = node;
      }

   bool balanceAfterInsertion(Cursor& cursor);
   bool balanceAfterSuppression(Cursor& cursor, typename Node::Balance balance);

  public:
   TImplParentTernaryTree() : pfcRoot(nullptr), uCount(0) {}
   TImplParentTernaryTree(const thisType& source, bool duplicateObject=false,
         const VirtualCast* retrieveRegistrationFromCopy=nullptr)
      :  inherited(source)
      {  if (duplicateObject)
            fullAssign(source, retrieveRegistrationFromCopy);
         else
            _assign(source);
      }
   thisType& operator=(const thisType& source) { return (thisType&) inherited::operator=(source); }
   TemplateDefineCopy(TImplParentTernaryTree, TypeSubString)
   DTemplateDefineAssign(TImplParentTernaryTree, TypeSubString)

   thisType& fullAssign(const thisType& source, const VirtualCast* retrieveRegistrationFromCopy=nullptr);
   const int& count() const { return uCount; }
   bool isEmpty() const { return !pfcRoot; }
   void swap(thisType& source)
      {  FollowingCell* temp = pfcRoot;
         pfcRoot = source.pfcRoot;
         source.pfcRoot = temp;
         int tempCount = uCount;
         uCount = source.uCount;
         source.uCount = tempCount;
      }

   LocationResult locate(TypeSubString& subString, Cursor& cursor) const
      {  return inherited::tlocate(*this, subString, cursor); }
   void addAsSon(TypeSubString& subString,
         DTernaryTree::ParentSuffixObject<TypeSubString>* object,
         Cursor& cursor, LocationResult result);
   DTernaryTree::BaseTernaryTree::CursorMove removeAt(Cursor& cursor, bool doesFree,
         SuppressionNotification* notification=nullptr);
   COL::ImplBinaryNode::Balance adjustRemove(Cursor& cursor,
         COL::VirtualCollection::RelativePosition pos,
         DTernaryTree::BaseTernaryTree::CursorMove move) const
      {  return move.applyMove(cursor, *this, (pos == COL::VirtualCollection::RPAfter)
            ? COL::ImplBinaryNode::BRight : ((pos == COL::VirtualCollection::RPBefore)
            ? COL::ImplBinaryNode::BLeft : COL::ImplBinaryNode::BEqual));
      }
   void replaceAt(Cursor& cursor,
         DTernaryTree::ParentSuffixObject<TypeSubString>* object, bool doesFree)
      {  FollowingCell& cell = *cursor.getReference();
         if (object != &cell.suffix()) {
            AssumeCondition(!object->hasSuffix())
            object->swapSuffix(cell.suffix());
            if (doesFree)
               cell.freeSuffix();
            else
               cell.clearSuffix();
            cell.absorbSuffix(object);
         };
      }
   void freeAll();
   void removeAll();

   virtual bool isValid() const override;
};

} // end of namespace STG

#endif // STG_TernaryTreeParentH

