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
// File      : StringRep.h
// Description :
//   Definition of a storage class that defines the support for the sub-strings.
//

#ifndef STG_TStringRepH
#define STG_TStringRepH

#include "StandardClasses/Persistence.h"
#include "Pointer/ExtPointer.hpp"
#include "Collection/ConcreteCollection/List.h"
#include "Collection/ConcreteCollection/Array.h"
#ifdef __GNUC__
#include <memory.h>
#endif

namespace STG {

namespace DStringRep {
   
template <typename TypeChar>
class TBasicRepositoryTraits {
  public:
   static void memmove(TypeChar* destination, const TypeChar* source, int len)
      {  ::memmove(destination, source, len*sizeof(TypeChar)); }
   static void memcpy(TypeChar* destination, const TypeChar* source, int len)
      {  ::memcpy(destination, source, len*sizeof(TypeChar)); }
   static void memset(TypeChar* source, TypeChar achar, int len)
      {  if (len > 0) {
            *source = achar;
            while (--len)
               *(++source) = achar;
         };
      }
};

} // end of namespace DStringRep

template <typename TypeChar>
class TBasicRepositoryTraits : public DStringRep::TBasicRepositoryTraits<TypeChar> {};

template <>
class TBasicRepositoryTraits<char> : public DStringRep::TBasicRepositoryTraits<char> {
  public:
   static void memset(char* source, char achar, int len)
      {  ::memset(source, achar, len); }
};

namespace DStringRep {

template <typename TypeChar>
class TBasicRepository {
  private:
   typedef TBasicRepository<TypeChar> thisType;
   int   uAllocatedSize;
   int   uLength;
   TypeChar* szString;

  protected:
   void clear()
      {  if (szString)
            delete [] szString;
         szString = nullptr;
         uAllocatedSize = uLength = 0;
      }
   bool hasAllocation() const { return (szString != nullptr) && (uAllocatedSize > 0); }

  public:
   TBasicRepository() : uAllocatedSize(0), uLength(0), szString(nullptr) {}
   TBasicRepository(int size) : uAllocatedSize(size), uLength(0), szString(new TypeChar[size+1])
      {  AssumeAllocation(szString) szString[0] = '\0'; }
   TBasicRepository(const thisType& source)
      : uAllocatedSize(source.uAllocatedSize), uLength(source.uLength),
        szString(new TypeChar[source.uAllocatedSize+1])
      {  AssumeCondition(source.isValid());
         TBasicRepositoryTraits<TypeChar>::memcpy(szString, source.szString, source.uLength+1);
      }
   virtual ~TBasicRepository() { if (szString) delete [] szString; }
   thisType& operator=(const thisType& source);

   // get and set methods
   int getAllocatedSize() const { return uAllocatedSize; }
   int getLength() const { return uLength; }
   TypeChar* getString() const { return szString; }
   int& length() { return uLength; }
   const int& length() const { return uLength; }
   void setLength(int newLength)
      {  AssumeCondition(newLength <= uAllocatedSize)
         uLength = newLength;
         if (szString)
            szString[uLength] = '\0';
      }

   void realloc(int newAllocated);
   bool bookPlace(int places=1)
      {  return (uLength+places > uAllocatedSize) ? _realloc(uLength+places) : true; }
   virtual bool _realloc(int newSize) { realloc(newSize*3/2); return true; }
   int getPlace() const { return uAllocatedSize-uLength; }

   /* read and write methods on stream. */
   void read(std::basic_istream<TypeChar>& in);
   void write(std::basic_ostream<TypeChar>& out) const;
   bool isValid() const
      {  return  (uAllocatedSize >= uLength) && (uLength >= 0)
            && ((!uAllocatedSize && !szString) || (szString && szString[uAllocatedSize+1]=='\0'));
      }
};
   
template <class TypePosition, class TypeSubString>
class ReadPosition {
  private:
   typedef ReadPosition<TypePosition, TypeSubString> thisType;
   TypePosition pPosition;
   const TypeSubString* pssReference;

  public:
   ReadPosition(const TypeSubString& reference) : pssReference(&reference) {}
   ReadPosition(const TypeSubString& reference, const TypePosition& position)
      :  pPosition(position), pssReference(&reference) {}
   ReadPosition(const thisType& source) = default;
   thisType& operator=(const thisType& rpSource) = default;

   ComparisonResult compare(const thisType& source) const
      {  return pPosition.compare(source.pPosition, pssReference->getElement()); }

   void retrievePosition(TypePosition& position) const { position = pPosition; }
   bool isValid() const { return pPosition.isValid(pssReference->getElement()); }
   int minus(const thisType& source) const
      {  return pPosition.minus(source.pPosition, pssReference->getElement()); }
   int getPosition() const { return pPosition.getPosition(pssReference->getElement()); }
   
   const TypePosition& getImplementationPosition() const { return pPosition; }
   const TypeSubString& getReference() const { return *pssReference; }
};

template <class TypePosition, class TypeSubString>
class WritePosition {
  private:
   typedef WritePosition<TypePosition, TypeSubString> thisType;
   TypePosition* ppPosition;
   const TypeSubString* pssReference;

  public:
   WritePosition(const TypeSubString& reference) : ppPosition(nullptr), pssReference(&reference) {}
   WritePosition(const TypeSubString& reference, TypePosition& position)
      :  ppPosition(&position), pssReference(&reference) {}
   WritePosition(const WritePosition& source) = default;
   thisType& operator=(const thisType& source) = default;

   void retrievePosition(TypePosition& position) const { AssumeCondition(ppPosition) position = *ppPosition; }
   void clearToPosition(TypePosition& position) { ppPosition = &position; }
   void plusAssign(int shift) { AssumeCondition(ppPosition) ppPosition->plusAssign(shift, pssReference->getElement()); }
   void minusAssign(int shift) { AssumeCondition(ppPosition) ppPosition->minusAssign(shift, pssReference->getElement()); }
   void inc() { AssumeCondition(ppPosition) ppPosition->inc(pssReference->getElement()); }
   void dec() { AssumeCondition(ppPosition) ppPosition->dec(pssReference->getElement()); }
   ComparisonResult compare(const thisType& source) const
      {  AssumeCondition(ppPosition && source.ppPosition)
         return ppPosition->compare(*source.ppPosition, pssReference->getElement());
      }

   int minus(const ReadPosition<TypePosition, TypeSubString>& source) const
      {  AssumeCondition(ppPosition) return ppPosition->minus(source.getImplementationPosition(), pssReference->getElement()); }
   int minus(const thisType& source) const
      {  AssumeCondition(ppPosition) return ppPosition->minus(*source.ppPosition, pssReference->getElement()); }
   void setToBegin() { AssumeCondition(ppPosition) ppPosition->setToBegin(pssReference->getElement()); }
   void setToEnd() { AssumeCondition(ppPosition) ppPosition->setToEnd(pssReference->getElement()); }
   const int& getPosition() const { AssumeCondition(ppPosition) return ppPosition->getPosition(pssReference->getElement()); }
   
   void increaseToPosition(const TypePosition& position) { AssumeCondition(ppPosition) *ppPosition = position; }
   void decreaseToPosition(const TypePosition& position) { AssumeCondition(ppPosition) *ppPosition = position; }
};

} // end of namespace DStringRep

/************************************************/
/* Definition of the template class TRepository */
/************************************************/

template <typename TypeChar>
class TRepository : public PNT::SharedElement, public DStringRep::TBasicRepository<TypeChar> {
  private:
   typedef PNT::SharedElement inherited;
   typedef TRepository<TypeChar> thisType;
   typedef DStringRep::TBasicRepository<TypeChar> inheritedImplementation;

  public:
   TRepository() {}
   TRepository(int size) : inheritedImplementation(size) {}
   TRepository(const thisType& source) = default;
   thisType& operator=(const thisType& source) = default;
   TemplateDefineCopy(TRepository, TypeChar)
   DTemplateDefineAssign(TRepository, TypeChar)

   TypeChar* getString() const { return inheritedImplementation::getString(); }
   const int& length() const { return inheritedImplementation::length(); }
   virtual bool isValid() const override
      {  return inherited::isValid() && inheritedImplementation::isValid() && isReferenced(); }

   int getAllocatedSize() const { return inheritedImplementation::getAllocatedSize(); }
   
   class MoveNotification;

   class Position {
     private:
      int uPosition;

      Position(int position) : uPosition(position) {}
      
     public:
      Position() : uPosition(0) {}
      Position(const Position& source) = default;
      Position& operator=(const Position& source) = default;

      bool inc(const TRepository<TypeChar>& repository)
         {  AssumeCondition(isValid(repository))
            bool result = (uPosition == repository.length());
            if (!result)
               ++uPosition;
            return result;
         }
      bool dec(const TRepository<TypeChar>& repository)
         {  AssumeCondition(isValid(repository))
            bool result = (uPosition == 0);
            if (!result)
               --uPosition;
            return result;
         }
      void advance(const TRepository<TypeChar>& repository, bool isBackward)
         {  if (isBackward)
               --uPosition;
            else
               ++uPosition;
            AssumeCondition(isValid(repository))
         }
      void advance(int shift, const TRepository<TypeChar>& repository, bool isBackward)
         {  if (isBackward)
               uPosition -= shift;
            else
               uPosition += shift;
            AssumeCondition(isValid(repository))
         }
      bool plusAssign(int shift, const TRepository<TypeChar>& repository)
         {  uPosition += shift;
            return normalize(repository.length());
         }
      bool minusAssign(int shift, const TRepository<TypeChar>& repository)
         {  uPosition -= shift;
            return normalize(repository.length());
         }
      ComparisonResult compare(const Position& source, const TRepository<TypeChar>&) const
         {  return fcompare(uPosition, source.uPosition); }

      int minus(const Position& source, const TRepository<TypeChar>&) const
         { return uPosition - source.uPosition; }
      bool isValid() const { return (uPosition >= 0); }
      bool isValid(const TRepository<TypeChar>& repository) const
         {  return (uPosition >= 0) && (uPosition <= repository.length()); }
      bool normalize(int length)
         {  bool result = (uPosition < 0) || (uPosition > length);
            if (uPosition < 0)
               uPosition = 0;
            else if (uPosition > length)
               uPosition = length;
            return result;
         }
      void setToBegin(const TRepository<TypeChar>&) { uPosition = 0; }
      void setToEnd(const TRepository<TypeChar>& repository) { uPosition = repository.length(); }
      bool isAtBegin(const TRepository<TypeChar>&) const { return uPosition == 0; }
      bool isAtEnd(const TRepository<TypeChar>& repository) const { return uPosition == repository.length(); }
      const int& getPosition(const TRepository<TypeChar>&) const { return uPosition; }
      friend class TRepository<TypeChar>;
      friend class MoveNotification;
   };

  public:
   class BaseSubString;
   class MoveNotification : public PNT::SharedPointer::Notification {
     private:
      typedef PNT::SharedPointer::Notification inherited;
      Position pSource;
      Position pNewSource;
      int uShift;

     protected:
      const Position& getNewSource() const { return pNewSource; }

     public:
      MoveNotification(const TRepository<TypeChar>&) : uShift(0) {}
      MoveNotification(const PNT::SharedPointer& call, const TRepository<TypeChar>&)
         :  inherited(call), uShift(0) {}
      MoveNotification(const MoveNotification& source) = default;

      DefineCopy(MoveNotification)
      DDefineAssign(MoveNotification)

      void update(const TRepository<TypeChar>& repository, Position* first=nullptr)
         {  repository.notifyUpdate(*this); }
      MoveNotification& setSource(const Position& source)
         {  pSource = source; pNewSource = source; return *this; }
      MoveNotification& setShift(int shift)
         {  AssumeCondition(uShift == 0) uShift = shift;
            if (shift > 0)
               pNewSource.uPosition += shift;
            return *this;
         }
      bool isInsertion() const { return uShift > 0; }
      bool isSuppression() const { return uShift < 0; }
      int getNewChars() const { return uShift; }
      void update(Position& position, int* length=nullptr, bool doesInclude=false);
      void update(BaseSubString& source, int& length, bool doesInclude=false);
      Position& source() { return pSource; }
      const Position& source() const { return pSource; }
      friend class TRepository<TypeChar>;
   };
   friend class MoveNotification;

   void setLength(const Position&, int newLength)
      {  inheritedImplementation::setLength(newLength); }
   int getLength(const Position&) const { return inheritedImplementation::length(); }
   TypeChar& operator[](const Position& position) { return inheritedImplementation::getString()[position.uPosition]; }
   const TypeChar& operator[](const Position& position) const { return getString()[position.uPosition]; }
   void retrieveChunk(const Position& position, TChunk<TypeChar>& chunk, bool isBackward=false) const
      {  if (!isBackward ? (position.uPosition < length()) : (position.uPosition > 0)) {
            chunk.length = !isBackward ? (length() - position.uPosition) : position.uPosition;
            chunk.string = getString() + position.uPosition;
            if (isBackward)
               --chunk.string;
         }
         else {
            chunk.length = 0;
            chunk.string = nullptr;
         };
      }
   bool setToNextChunk(Position& position, TChunk<TypeChar>& chunk, bool isBackward=false) const
      {  position = 0;
         chunk.setLocalLength(0).setString(nullptr);
         return false;
      }
   bool move(MoveNotification& notification);
   void setMove(MoveNotification& notification) {}
   void copy(const Position& destinationPosition, const thisType& source,
         const Position& sourcePosition, int len)
      {  AssumeCondition((len >= 0) && (sourcePosition.uPosition >= 0) && (sourcePosition.uPosition+len <= source.length())
            && (destinationPosition.uPosition >= 0) && (destinationPosition.uPosition+len <= length()))
         TBasicRepositoryTraits<TypeChar>::memcpy(getString() + destinationPosition.uPosition,
               source.getString() + sourcePosition.uPosition, len);
      }
   void copy(const Position& destination, const TypeChar* source, int len)
      {  AssumeCondition((len >= 0) && (destination.uPosition >= 0) && (destination.uPosition+len <= length()))
         TBasicRepositoryTraits<TypeChar>::memcpy(getString() + destination.uPosition,
               source, len);
      }
   void set(const Position& destination, const TypeChar& achar, int number)
      {  AssumeCondition((number >= 0) && (destination.uPosition >= 0) && (destination.uPosition+number <= length()))
         TBasicRepositoryTraits<TypeChar>::memset(getString() + destination.uPosition, achar, number);
      }
   bool bookPlace(Position&, int places=1)
      {  if (!inheritedImplementation::bookPlace(places))
            throw EMemoryExhausted();
         return true;
      }
   void normalize(Position&) {}
   int getPlace(const Position&) const { return inheritedImplementation::getPlace(); }
   
   class BaseSubString : public PNT::TSharedPointer<TRepository<TypeChar> > {
     private:
      typedef PNT::TSharedPointer<TRepository<TypeChar> > inherited;
      Position pStart;

     protected:
      void _lightAssign(const EnhancedObject& asource) { inherited::_lightAssign(asource); }
      virtual void _assign(const EnhancedObject& asource)
         {  const BaseSubString& source = (const BaseSubString&) inherited::castFromCopyHandler(asource);
            if (this != &source) {
               inherited::fullAssign(source);
               pStart = source.pStart;
            };
         }

     public:
      BaseSubString() {}
      BaseSubString(TRepository<TypeChar>* element, PNT::Pointer::Init init)
         :  inherited(element, init) {}
      BaseSubString(const BaseSubString& source) = default;
      BaseSubString& operator=(const BaseSubString& source)
         {  inherited::fullAssign(source);
            pStart = source.pStart;
            return *this;
         }
      virtual ~BaseSubString() { inherited::release(); }
      typedef PNT::SharedPointer BaseNotification;
      typedef HandlerCast<BaseSubString, PNT::SharedPointer> BaseNotificationCast;

      typedef DStringRep::WritePosition<Position, BaseSubString> WriteStartPosition;
      typedef DStringRep::ReadPosition<Position, BaseSubString> ReadStartPosition;
      WriteStartPosition start() { return WriteStartPosition(*this, pStart); }
      ReadStartPosition start() const { return ReadStartPosition(*this, pStart); }

      bool isAffectedBy(MoveNotification& notification) const { return false; }
      bool doesSupportNotificationLength(int notificationLength) const
         {  return notificationLength >= 0; }

      class WriteEndPosition {
        public:
         WriteEndPosition() {}
         WriteEndPosition(const WriteEndPosition& source) {}
         WriteEndPosition& operator=(const WriteEndPosition& source) { return *this; }

         void setToStart() {}
         void setToEnd() {}
         void increaseToPosition(const Position&) {}
         void decreaseToPosition(const Position&) {}
         void plusAssign(int add) {}
         void minusAssign(int sub) {}
         void inc() {}
         void dec() {}
      };
      WriteEndPosition end() { return WriteEndPosition(); }
      friend class MoveNotification;
   };
};

template <typename TypeChar>
inline void
TRepository<TypeChar>::MoveNotification::update(Position& position, int* length, bool doesInclude) {
   if (uShift <= 0) { // suppression
      if (position.uPosition >= pSource.uPosition) {
         if (position.uPosition >= pSource.uPosition - uShift)
            position.uPosition += uShift;
         else { // pSource.uPosition <= position.uPosition - uShift < pSource.uPosition
            if (length) {
               if (position.uPosition + *length >= pSource.uPosition - uShift)
                  *length += position.uPosition - pSource.uPosition + uShift;
               else
                  *length = 0;
            };
            position.uPosition = pSource.uPosition;
         };
      }
      else if (length && (position.uPosition + *length > pSource.uPosition)) {
         // position.uPosition - uShift < pSource.uPosition <= pSource.uPosition
         if (position.uPosition + *length >= pSource.uPosition - uShift)
            *length += uShift;
         else
            *length = pSource.uPosition - position.uPosition;
      };
   }
   else { // 0 < uShift = insertion
      if (!doesInclude ? (position.uPosition >= pSource.uPosition) : (position.uPosition > pSource.uPosition))
         position.uPosition += uShift;
      else { // position.uPosition < pSource.uPosition < pSource.uPosition + uShift
         if (length && (position.uPosition + *length > pSource.uPosition))
            *length += uShift;
      };
   };
}

template <typename TypeChar>
inline void
TRepository<TypeChar>::MoveNotification::update(BaseSubString& source, int& length, bool doesInclude)
   {  update(source.pStart, &length, doesInclude); }

template <typename TypeChar>
inline bool
TRepository<TypeChar>::move(MoveNotification& notification) {
   const Position& source = notification.pSource;
   int shift = notification.uShift;
   AssumeCondition(source.uPosition >= 0 && source.uPosition <= length())
   if (shift != 0) {
      if (shift > 0) {
         if (!inheritedImplementation::bookPlace(shift))
            throw EMemoryExhausted();
         TBasicRepositoryTraits<TypeChar>::memmove(getString() + source.uPosition + shift,
               getString() + source.uPosition, length() - source.uPosition + 1);
      }
      else
         TBasicRepositoryTraits<TypeChar>::memmove(getString() + source.uPosition,
               getString() + source.uPosition - shift, length() - source.uPosition + shift + 1);
      inheritedImplementation::length() += shift;
   };
   return true;
}

/****************************************************/
/* Definition of the template class TListRepository */
/****************************************************/

namespace DListRepository {

class SharedRegistration : public PNT::SharedElement {
  private:
   typedef PNT::SharedElement inherited;

  public:
   SharedRegistration() {}
   SharedRegistration(const SharedRegistration& source) : inherited(source) {}
   void notifyUpdate(PNT::SharedPointer::Notification& notification) const
      {  inherited::notifyUpdate(notification); }
};

template <typename TypeChar>
class TAtomicRepository : public COL::List::Node, public DStringRep::TBasicRepository<TypeChar> {
  private:
   typedef COL::List::Node inherited;
   typedef DStringRep::TBasicRepository<TypeChar> inheritedImplementation;

  protected:
   virtual bool _realloc(int newSize) override { return false; }
   virtual ComparisonResult _compare(const EnhancedObject& asource) const override
      {  return inherited::_compare(asource); }
   
  public:
   TAtomicRepository(int size) : inherited(), inheritedImplementation(size) {}
   TAtomicRepository(const TAtomicRepository<TypeChar>& source) = default;
   TemplateDefineCopy(TAtomicRepository, TypeChar)
   DTemplateDefineAssign(TAtomicRepository, TypeChar)

   int getAllocatedSize() const { return inheritedImplementation::getAllocatedSize(); }
   virtual bool isValid() const override { return inherited::isValid(); }
   void suppress()
      {  AssumeCondition(inherited::isAtomic() && (inheritedImplementation::length() > 0));
         int uLength = inheritedImplementation::length();
         inheritedImplementation::clear();
         inheritedImplementation::length() = uLength;
      }
   bool isSuppressed() const
      { return !inheritedImplementation::hasAllocation() && (inheritedImplementation::length() > 0); }
};
   
template <typename TypeChar, class RepositoryPolicy>
class TPosition;

template <typename TypeChar, class RepositoryPolicy>
class TMoveNotification;

template <typename TypeChar>
class GlobalRepositoryPolicy {
  public:
   typedef TAtomicRepository<TypeChar> AtomicRepository;
   typedef COL::TCopyCollection<COL::TList<AtomicRepository> > ListStrings;
   
   class MoveNotification;
   class BaseSubString : public PNT::TSharedPointer<SharedRegistration> {
     private:
      typedef PNT::TSharedPointer<SharedRegistration> inherited;
      int uStartPosition;
      AtomicRepository* parStart;

     public:
      void retrieveStartPosition(AtomicRepository*& newStart, int& newStartPosition) const
         {  newStartPosition = uStartPosition;
            newStart = parStart;
         }
      void setStartPosition(AtomicRepository* start, int startPosition)
         {  parStart = start;
            uStartPosition = startPosition;
         }
      bool increaseStartPosition(int add);
      bool decreaseStartPosition(int sub);
      void increaseStartTo(AtomicRepository* start, int localPos)
         {  parStart = start; uStartPosition = localPos; }
      void decreaseStartTo(AtomicRepository* start, int localPos)
         {  parStart = start; uStartPosition = localPos; }
      void setEndToStartPosition() {}

      AtomicRepository* getEndPositionEstimate() const { AssumeUncalled return nullptr; }
      void increaseEndPosition(int add) {}
      void decreaseEndPosition(int sub) {}
      void increaseEndTo(AtomicRepository* newEnd, int localPos) {}
      void decreaseEndTo(AtomicRepository* newEnd, int localPos) {}
      void setEndToLastPosition() {}
      bool doesSupportNotificationLength(int notificationLength) const
         {  return notificationLength >= 0; }

     protected:
      void _lightAssign(const EnhancedObject& asource)
         {  EnhancedObject::_assign(asource); }
      virtual void _assign(const EnhancedObject& asource)
         {  const BaseSubString& source = (const BaseSubString&) inherited::castFromCopyHandler(asource);
            if (this != &source) {
               inherited::fullAssign(source);
               uStartPosition = source.uStartPosition;
               parStart = source.parStart;
            };
         }

     public:
      BaseSubString() : uStartPosition(0), parStart(nullptr) {}
      BaseSubString(SharedRegistration* element, PNT::Pointer::Init init)
         : inherited(element, init), uStartPosition(0), parStart(nullptr) {}
      BaseSubString(const BaseSubString& source) = default;
      BaseSubString& operator=(const BaseSubString& source) = default;
      virtual ~BaseSubString() { inherited::release(); }
      typedef PNT::SharedPointer BaseNotification;
      typedef HandlerCast<BaseSubString, PNT::SharedPointer> BaseNotificationCast;
      
      bool isAffectedBy(MoveNotification& notification) const { return false; }
   };

   class MoveNotification : public PNT::SharedPointer::Notification {
     private:
      typedef PNT::SharedPointer::Notification inherited;

     public:
      MoveNotification(const SharedRegistration& repository)  {}
      MoveNotification(const BaseSubString& call, const SharedRegistration& repository)
         :  inherited(call) {}
      MoveNotification(const MoveNotification& source) = default;
      DefineCopy(MoveNotification)
      void setOrigin(const BaseSubString& origin) { inherited::setOrigin(origin); }
      void update(const SharedRegistration& repository, void* first=nullptr) { repository.notifyUpdate(*this); }
   };
};

template <typename TypeChar>
class LocalRepositoryPolicy {
  public:
   class AtomicRepository : public TAtomicRepository<TypeChar>, public PNT::SharedCollection::Element {
     private:
      typedef TAtomicRepository<TypeChar> inherited;
      typedef PNT::SharedCollection::Element NotificationPart;
      
     protected:
      virtual ComparisonResult _compare(const EnhancedObject& asource) const override
         {  return inherited::_compare(asource); }

     public:
      AtomicRepository(int places) : inherited(places) {}
      AtomicRepository(const AtomicRepository& source) = default;
      AtomicRepository& operator=(const AtomicRepository& source) = default;
      DefineCopy(AtomicRepository)
      DDefineAssign(AtomicRepository)
      StaticInheritConversions(AtomicRepository, inherited)

      virtual bool isValid() const override { return inherited::isValid() && NotificationPart::isValid(); }
      int getAllocatedSize() const { return inherited::getAllocatedSize(); }
      typedef HandlerCast<AtomicRepository, NotificationPart> NotificationCast;
   };
   typedef COL::TCopyCollection<COL::TList<AtomicRepository> > ListStrings;
   
   class MoveNotification;
   class BaseSubString : public PNT::TSharedPointer<SharedRegistration>, public PNT::TSharedCollection<AtomicRepository, typename AtomicRepository::NotificationCast> {
     private:
      typedef PNT::TSharedPointer<SharedRegistration> inherited;
      typedef PNT::TSharedCollection<AtomicRepository, typename AtomicRepository::NotificationCast> inheritedImplementation;
      int uLocalStart;
      int uLocalEnd;

     protected:
      void _lightAssign(const EnhancedObject& asource) { inherited::_lightAssign(asource); }
      virtual ComparisonResult _compare(const EnhancedObject& asource) const override
         {  return inherited::_compare(asource); }

     public:
      void retrieveStartPosition(AtomicRepository*& newStart, int& newStartPosition) const
         {  newStartPosition = uLocalStart;
            newStart = inheritedImplementation::getPFirst();
         }
      void setStartPosition(AtomicRepository* start, int startPosition)
         {  uLocalEnd = uLocalStart = startPosition;
            inheritedImplementation::removeAll();
            if (start)
               inheritedImplementation::addElement(*start);
         }
      bool increaseStartPosition(int add);
      bool decreaseStartPosition(int sub);
      void increaseStartTo(AtomicRepository* start, int localPos);
      void decreaseStartTo(AtomicRepository* start, int localPos);
      void setEndToStartPosition()
         {  uLocalEnd = uLocalStart;
            if (!inheritedImplementation::isSingleton()) {
               AtomicRepository* first = inheritedImplementation::getPFirst();
               inheritedImplementation::removeAll();
               if (first)
                  inheritedImplementation::addElement(*first);
            };
         }

      AtomicRepository* getEndPositionEstimate() const { return inheritedImplementation::getPLast(); }
      void increaseEndPosition(int add);
      void decreaseEndPosition(int sub);
      void increaseEndTo(AtomicRepository* newEnd, int localPos);
      void decreaseEndTo(AtomicRepository* newEnd, int localPos);
      void setEndToLastPosition();

     public:
      BaseSubString() : uLocalStart(0), uLocalEnd(0) {}
      BaseSubString(SharedRegistration* element, PNT::Pointer::Init init)
         : inherited(element, init), uLocalStart(0), uLocalEnd(0) {}
      BaseSubString(const BaseSubString& source) = default;
      virtual ~BaseSubString()
         {  inheritedImplementation::disconnect();
            inherited::release();
         }
      BaseSubString& operator=(const BaseSubString& source)
         {  if (this!= &source) {
               inheritedImplementation::disconnect();
               inherited::fullAssign(source);
               inheritedImplementation::operator=(source);
               uLocalStart = source.uLocalStart;
               uLocalEnd = source.uLocalEnd;
            };
            return *this;
         }

      DefineCopy(BaseSubString)
      DDefineAssign(BaseSubString)
      StaticInheritConversions(BaseSubString, inherited)

      void notifyUpdate(typename inheritedImplementation::Notification& notification)
         {  inheritedImplementation::notifyUpdate(notification); }
      bool isAffectedBy(MoveNotification& notification) const { return true; }
         
      void swap(BaseSubString& source)
         {  inherited::swap(source);
            inheritedImplementation::swap(source);
            int temp = uLocalStart;
            uLocalStart = source.uLocalStart;
            source.uLocalStart = temp;
            temp = uLocalEnd;
            uLocalEnd = source.uLocalEnd;
            source.uLocalStart = temp;
         }
      void release()
         {  inheritedImplementation::disconnect();
            inherited::release();
            uLocalStart = uLocalEnd = 0;
         }
      BaseSubString& assign(SharedRegistration* element, PNT::Pointer::Init init)
         {  inheritedImplementation::disconnect();
            inherited::release();
            inherited::assign(element, init);
            uLocalStart = uLocalEnd = 0;
            return *this;
         }
      virtual bool isValid() const override { return inherited::isValid(); }
      bool doesSupportNotificationLength(int notificationLength) const
         {  if (inheritedImplementation::isSingleton())
               return uLocalEnd - uLocalStart == notificationLength;
            notificationLength -= inheritedImplementation::getFirst().length();
            typename inheritedImplementation::Cursor notificationCursor((const inheritedImplementation&) *this);
            if (notificationCursor.setToFirst())
               while (notificationCursor.setToNext())
                  notificationLength -= notificationCursor.elementAt().length();
            return -notificationLength == inheritedImplementation::getLast().length() - uLocalStart;
         }

      typedef PNT::SharedCollection BaseNotification;
      typedef HandlerCast<BaseSubString, PNT::SharedCollection> BaseNotificationCast;
   };
   class MoveNotification : public PNT::SharedCollection::Notification {
     private:
      typedef PNT::SharedCollection::Notification inherited;

     public:
      MoveNotification(const SharedRegistration& repository) {}
      MoveNotification(const BaseSubString& call, const SharedRegistration& repository)
         {  setOrigin(call); }
      MoveNotification(const MoveNotification& source) = default;
      DefineCopy(MoveNotification)
      void setOrigin(const BaseSubString& origin)
         {  inherited::setOrigin(origin); }
      void update(const SharedRegistration& repository, void* first=nullptr);
   };
};

template <typename TypeChar>
class LightLocalRepositoryPolicy : public LocalRepositoryPolicy<TypeChar> {
  private:
   typedef LocalRepositoryPolicy<TypeChar> inherited;
   
  public:
   typedef typename inherited::MoveNotification InheritedMoveNotification; 
   typedef typename inherited::BaseSubString BaseSubString;

   class MoveNotification : public InheritedMoveNotification {
     private:
      typedef InheritedMoveNotification inherited;

     public:
      MoveNotification(const SharedRegistration& repository) : inherited(repository) {}
      MoveNotification(const BaseSubString& call, const SharedRegistration& repository)
         :  inherited(call, repository) {}
      MoveNotification(const MoveNotification& source) = default;
      DefineCopy(MoveNotification)
            
      void update(const SharedRegistration& repository, void* first=nullptr);
   };
};

template <typename TypeChar, class RepositoryPolicy>
class TMoveNotification;

template <typename TypeChar, class RepositoryPolicy>
class TBasicRepository {
  public:
   typedef typename RepositoryPolicy::AtomicRepository AtomicRepository;
   typedef typename RepositoryPolicy::ListStrings ListStrings;

  public:
   typedef TMoveNotification<TypeChar, RepositoryPolicy> MoveNotification;
   typedef TPosition<TypeChar, RepositoryPolicy> Position;

  private:
   typedef TBasicRepository<TypeChar, RepositoryPolicy> thisType;
   ListStrings larStrings;
   int uTotalLength;

   int insertSupport(MoveNotification& notification, AtomicRepository& support, int& shift);
   int suppressSupport(MoveNotification& notification, AtomicRepository& support, int& shift);
   int insertPreviousSupport(MoveNotification& notification, AtomicRepository& previousSupport,
         int& shift, AtomicRepository& support, int& firstPosition, int sourcePosition);
   int insertNextSupport(MoveNotification& notification, AtomicRepository& nextSupport,
         int& shift, AtomicRepository& support, int sourcePosition);
   int suppressNextSupport(MoveNotification& notification, AtomicRepository& support,
         AtomicRepository& nextSupport, int shift);
   void mergeSupport(MoveNotification& notification, AtomicRepository& mergeSupport,
         AtomicRepository& deleteSupport, int leftMoveIndex);
      
   void insert(MoveNotification& notification, int shift);
   void suppress(MoveNotification& notification, int shift);
   
  protected:
   virtual AtomicRepository* createAtomic(int expected) const { return new AtomicRepository(5000); }

  public:
   ListStrings& strings() { return larStrings; }
   const ListStrings& strings() const { return larStrings; }

  public:
   TBasicRepository() : uTotalLength(0) {}
   TBasicRepository(int size) : uTotalLength(0) {}
   TBasicRepository(const thisType& source);
#ifdef __GNUC__
   virtual ~TBasicRepository() {}
#endif
   thisType& operator=(const thisType& source) = default;

   // get and set methods
   int length() const { return uTotalLength; }
   void read(std::basic_istream<TypeChar>& in);
   void write(std::basic_ostream<TypeChar>& out) const;
   bool invariant() const;
   bool isValid() const
      {  return (uTotalLength >= 0) && ((uTotalLength == 0) == larStrings.isEmpty()); }

   TypeChar& operator[](const Position& position);
   const TypeChar& operator[](const Position& position) const;
   void retrieveChunk(const Position& position, TChunk<TypeChar>& chunk, bool isBackward=false) const;
   bool setToNextChunk(Position& position, TChunk<TypeChar>& chunk, bool isBackward=false) const;
   bool move(MoveNotification& notification);
   void setMove(MoveNotification& notification);
   void copy(const Position& destination, const thisType& source, const Position& sourcePosition, int len);
   void copy(const Position& destination, const TypeChar* source, int len);
   void set(const Position& destination, const TypeChar& achar, int number);
   bool bookPlace(Position&, int places=1);
   void normalize(Position& position);
   int getPlace(const Position&) const;
   void setLength(const Position& position, int newLength);
   int getLength(const Position& position) const;

   friend class TPosition<TypeChar, RepositoryPolicy>;
   friend class TMoveNotification<TypeChar, RepositoryPolicy>;
};

template <typename TypeChar, class RepositoryPolicy>
class TPosition {
  private:
   typedef TBasicRepository<TypeChar, RepositoryPolicy> Repository;
   typedef typename RepositoryPolicy::AtomicRepository AtomicRepository;
   typedef TPosition<TypeChar, RepositoryPolicy> thisType;
   typedef typename RepositoryPolicy::ListStrings ListStrings;

  protected:
   AtomicRepository* parSupport;
   int uPosition;
   
   ComparisonResult _compare(const thisType& source, const Repository& repository) const;
   int _minus(const thisType& source, const Repository& repository) const;
   bool normalize(const Repository& repository);

  public:
   TPosition() : parSupport(nullptr), uPosition(0) {}
   TPosition(const thisType& source) = default;

   friend class TBasicRepository<TypeChar, RepositoryPolicy>;
   friend class TMoveNotification<TypeChar, RepositoryPolicy>;
   bool inc(const Repository& repository)
      {  ++uPosition;
         return (!parSupport || (uPosition >= parSupport->length()))
            ? normalize(repository) : false;
      }
   bool dec(const Repository& repository)
      {  --uPosition;
         return ((uPosition < 0) || !parSupport)
            ? normalize(repository) : false;
      }
   void advance(const Repository& repository, bool isBackward)
      {  if (isBackward)
            --uPosition;
         else
            ++uPosition;
         bool isInvalid = ((uPosition < 0) || !parSupport || (uPosition >= parSupport->length()))
            ? normalize(repository) : false;
         AssumeCondition(!isInvalid)
      }
   void advance(int shift, const Repository& repository, bool isBackward)
      {  if (isBackward)
            uPosition -= shift;
         else
            uPosition += shift;
         bool isInvalid = ((uPosition < 0) || !parSupport || (uPosition >= parSupport->length()))
            ? normalize(repository) : false;
         AssumeCondition(!isInvalid)
      }
   bool plusAssign(int shift, const Repository& repository)
      {  uPosition += shift;
         return (uPosition < 0) || !parSupport || (uPosition >= parSupport->length())
            ? normalize(repository) : false;
      }
   bool minusAssign(int shift, const Repository& repository)
      {  uPosition -= shift;
         return (uPosition < 0) || !parSupport || (uPosition >= parSupport->length())
            ? normalize(repository) : false;
      }
   ComparisonResult compare(const thisType& source, const Repository& repository) const
      {  return (parSupport == source.parSupport)
            ? fcompare(uPosition, source.uPosition)
            : _compare(source, repository);
      }
   int minus(const thisType& source, const Repository& repository) const
      {  return (parSupport == source.parSupport)
            ? uPosition - source.uPosition
            : _minus(source, repository);
      }
   bool isValid() const { return (uPosition >= 0) && parSupport && (uPosition <= parSupport->length()); }
   bool isValid(const Repository& repository) const
      {  return (repository.larStrings.isEmpty() && (uPosition == 0) && !parSupport)
            || (!repository.larStrings.isEmpty() && (uPosition >= 0) && parSupport
               && ((uPosition < parSupport->length())
                  || ((parSupport == &repository.larStrings.getLast()) && (uPosition == parSupport->length()))));
      }
   void setToBegin(const Repository& repository)
      {  parSupport = !repository.larStrings.isEmpty() ? &repository.larStrings.getSFirst() : nullptr;
         uPosition = 0;
      }
   void setToEnd(const Repository& repository)
      {  parSupport = !repository.larStrings.isEmpty() ? &repository.larStrings.getSLast() : nullptr;
         uPosition = parSupport ? parSupport->length() : 0;
      }
   bool isAtBegin(const Repository& repository) const
      {  return (uPosition == 0) && (repository.larStrings.isEmpty()
            || parSupport == &repository.larStrings.getFirst());
      }
   bool isAtEnd(const Repository& repository) const
      {  return repository.larStrings.isEmpty()
            || ((parSupport == &repository.larStrings.getLast())
                  && (uPosition == parSupport->length()));
      }
   int getPosition(const Repository& repository) const;
};

template <typename TypeChar, class RepositoryPolicy>
inline TypeChar&
TBasicRepository<TypeChar, RepositoryPolicy>::operator[](const Position& position) {
   AssumeCondition(position.parSupport)
   return position.parSupport->getString()[position.uPosition];
}

template <typename TypeChar, class RepositoryPolicy>
inline const TypeChar&
TBasicRepository<TypeChar, RepositoryPolicy>::operator[](const Position& position) const {
   AssumeCondition(position.parSupport)
   return position.parSupport->getString()[position.uPosition];
}

template <typename TypeChar, class RepositoryPolicy>
inline void
TBasicRepository<TypeChar, RepositoryPolicy>::setLength(const Position& position, int newLength) {
   position.parSupport->length() = newLength;
   if (position.parSupport->getString())
      position.parSupport->getString()[newLength] = '\0';
}

template <typename TypeChar, class RepositoryPolicy>
inline int
TBasicRepository<TypeChar, RepositoryPolicy>::getLength(const Position& position) const {
   return position.parSupport->length();
}

template <typename TypeChar, class RepositoryPolicy>
inline int
TBasicRepository<TypeChar, RepositoryPolicy>::getPlace(const Position& position) const {
   if (larStrings.isEmpty())
      return 0;
   AssumeCondition(position.parSupport)
   return position.parSupport->getPlace();
}

template <typename TypeChar, class RepositoryPolicy>
inline void
TBasicRepository<TypeChar, RepositoryPolicy>::normalize(Position& position) {
   if (position.parSupport && (position.parSupport->length() == 0)) {
      typename ListStrings::Cursor cursor(larStrings);
      cursor.gotoReference(*position.parSupport);
      cursor.setToNext();
      larStrings.freePrevious(cursor);
      if (cursor.isValid()) {
         position.parSupport = &cursor.elementSAt();
         position.uPosition = 0;
      }
      else if (cursor.setToPrevious()) {
         position.parSupport = &cursor.elementSAt();
         position.uPosition = position.parSupport->length();
      }
      else {
         position.parSupport = nullptr;
         position.uPosition = 0;
      };
   };
}

template <typename TypeChar, class RepositoryPolicy>
class TMoveNotification : public RepositoryPolicy::MoveNotification {
  public:
   typedef TBasicRepository<TypeChar, RepositoryPolicy> Repository;

  private:
   typedef TMoveNotification<TypeChar, RepositoryPolicy> thisType;
   typedef typename RepositoryPolicy::MoveNotification inherited;

  protected:
   typedef typename RepositoryPolicy::AtomicRepository AtomicRepository;
   typedef TPosition<TypeChar, RepositoryPolicy> Position;

   class LengthRemember {
     private:
      AtomicRepository* parSupport;
      int uLength;

     public:
      LengthRemember() : parSupport(nullptr), uLength(0) {}
      LengthRemember(const LengthRemember& source) = default;
      
      bool isValid() const { return parSupport != nullptr; }
      AtomicRepository* ssupport() const { return parSupport; }
      friend class TBasicRepository<TypeChar, RepositoryPolicy>;
      friend class TMoveNotification<TypeChar, RepositoryPolicy>;
   };

   class RightMove {
     private:
      AtomicRepository* parSupport;
      int uPosition;
      Position pNewPosition;

     public:
      RightMove() : parSupport(nullptr), uPosition(0) {}
      RightMove(const RightMove& source) = default;
      bool isValid() const { return parSupport != nullptr; }
      AtomicRepository* ssupport() const { return parSupport; }
      friend class TBasicRepository<TypeChar, RepositoryPolicy>;
      friend class TMoveNotification<TypeChar, RepositoryPolicy>;
   };

  protected:
   Position pSource;
   int uShift;
   typedef COL::TCopyCollection<COL::TList<AtomicRepository> > SuppressedRepositories;
   typedef COL::TArray<AtomicRepository, HandlerCopyCast<AtomicRepository> > InsertedRepositories;
   const Repository& rRepository;
   SuppressedRepositories larSuppressedRep;
   InsertedRepositories aarNewRep;
   Position pNewPosition;
   LengthRemember lcCurrent, lcLeft;
   RightMove rmRightMove;

   static AtomicRepository* getSupport(const Position& pPosition) { return pPosition.parSupport; }
   static int getPosition(const Position& pPosition) { return pPosition.uPosition; }
   static void setSupport(Position& pPosition, AtomicRepository* parSupport) { pPosition.parSupport = parSupport; }
   static void setPosition(Position& pPosition, int uPosition) { pPosition.uPosition = uPosition; }

  protected:
   const LengthRemember& current() const { return lcCurrent; }
   const LengthRemember& left() const { return lcLeft; }
   const RightMove& right() const { return rmRightMove; }

   const Position& getNewSource() const { return pNewPosition; }
   
  public:
   class Access;

  public:
   TMoveNotification(const SharedRegistration& repository, const Repository& repositorySource)
      :  inherited(repository), uShift(0), rRepository(repositorySource) {}
   TMoveNotification(const typename RepositoryPolicy::BaseSubString& call, const SharedRegistration& repository, const Repository& repositorySource)
      :  inherited(call, repository), uShift(0), rRepository(repositorySource) {}
   TMoveNotification(const thisType& source)
      :  inherited(source), pSource(source.pSource), uShift(source.uShift),
         rRepository(source.rRepository) {}
   thisType& operator=(const thisType& source)
      {  AssumeCondition(&rRepository == &source.rRepository)
         pSource = source.pSource;
         uShift = source.uShift;
         return *this;
      }

   bool isInsertion() const { return uShift > 0; }
   bool isSuppression() const { return uShift < 0; }
   int getNewChars() const { return uShift; }
   void update(Position& position, int* length=nullptr, bool doesInclude=false);
   void update(const SharedRegistration& repository, const TPosition<TypeChar, RepositoryPolicy>* first = nullptr)
      {  inherited::update(repository, (void*) const_cast<TPosition<TypeChar, RepositoryPolicy>*>(first)); }
   void update(typename RepositoryPolicy::BaseSubString& subString, int& length, bool doesInclude=false);
   Position& source() { return pSource; }
   const Position& source() const { return pSource; }
   void setShift(int shift) { uShift = shift; }
   
   friend class TBasicRepository<TypeChar, RepositoryPolicy>;
   // friend class RepositoryPolicy::MoveNotification;
};

template <typename TypeChar, class RepositoryPolicy>
class TMoveNotification<TypeChar, RepositoryPolicy>::Access : public TMoveNotification<TypeChar, RepositoryPolicy> {
  private:
   typedef TMoveNotification<TypeChar, RepositoryPolicy> inherited;

  public:
#ifdef __GNUC__
   Access(const SharedRegistration& repository, const Repository& repositorySource)
      : inherited(repository, repositorySource) {}
#endif
   typedef typename inherited::AtomicRepository AtomicRepository;

   Position& source() { return inherited::pSource; }
   const Position& source() const { return inherited::pSource; }
   int& shift() { return inherited::uShift; }
   const int& shift() const { return inherited::uShift; }
   const Repository& repository() { return inherited::rRepository; }
   SuppressedRepositories& suppressedRep() { return inherited::larSuppressedRep; }
   const SuppressedRepositories& suppressedRep() const { return inherited::larSuppressedRep; }
   InsertedRepositories& newRep() { return inherited::aarNewRep; }
   const InsertedRepositories& newRep() const { return inherited::aarNewRep; }
   Position& newPosition() { return inherited::pNewPosition; }
   const Position& newPosition() const { return inherited::pNewPosition; }
   LengthRemember& current() { return inherited::lcCurrent; }
   const LengthRemember& current() const { return inherited::lcCurrent; }
   LengthRemember& left() { return inherited::lcLeft; }
   const LengthRemember& left() const { return inherited::lcLeft; }
   RightMove& rightMove() { return inherited::rmRightMove; }
   const RightMove& rightMove() const { return inherited::rmRightMove; }
   
   static AtomicRepository* getSupport(const Position& position) { return inherited::getSupport(position); }
   static int getPosition(const Position& position) { return inherited::getPosition(position); }
   static void setSupport(Position& position, AtomicRepository* support) { inherited::setSupport(position, support); }
   static void setPosition(Position& position, int localPos) { inherited::setPosition(position, localPos); }
};


template <typename TypeChar, class RepositoryPolicy>
inline bool
TBasicRepository<TypeChar, RepositoryPolicy>::move(MoveNotification& notification) {
   if (notification.uShift != 0) {
      if (!notification.pSource.parSupport) {
         AssumeCondition(larStrings.isEmpty() && notification.isInsertion())
         larStrings.insertNewAtEnd(notification.pSource.parSupport = createAtomic(notification.uShift));
      };
      notification.pNewPosition = notification.pSource;
      if (notification.uShift < 0)
         insert(notification, -notification.uShift);
      else if (notification.uShift > 0)
         suppress(notification, notification.uShift);
      uTotalLength += notification.uShift;
   }
   else
      notification.pNewPosition = notification.pSource;
      
   return true;
}

template <typename TypeChar, class RepositoryPolicy>
inline void
TBasicRepository<TypeChar, RepositoryPolicy>::setMove(MoveNotification& notification) {
   notification.pNewPosition = notification.pSource;
   if (notification.uShift != 0) {
      notification.lcCurrent.parSupport = notification.pSource.parSupport;
      notification.lcCurrent.uLength = notification.pSource.parSupport
         ? notification.pSource.parSupport->getLength() : 0;
      if (notification.uShift > 0) {
         notification.rmRightMove.pNewPosition = notification.pSource;
         notification.rmRightMove.pNewPosition.plusAssign(notification.uShift, *this);
         notification.rmRightMove.parSupport = notification.pNewPosition.parSupport;
      };
   };
}

} // end of namespace DListRepository

template <typename TypeChar,
   class RepositoryPolicy = DListRepository::GlobalRepositoryPolicy<TypeChar> >
class TListRepository :
   public DListRepository::SharedRegistration,
   public DListRepository::TBasicRepository<TypeChar, RepositoryPolicy> {
  private:
   typedef DListRepository::SharedRegistration inherited;
   typedef DListRepository::TBasicRepository<TypeChar, RepositoryPolicy> inheritedImplementation;
   typedef TListRepository<TypeChar, RepositoryPolicy> thisType;

  public:
   class Position;

   class MoveNotification;
   class BaseSubString : public RepositoryPolicy::BaseSubString {
     private:
      typedef typename RepositoryPolicy::BaseSubString inherited;
      typedef TListRepository<TypeChar, RepositoryPolicy> Shared;
      
     public:
      BaseSubString() {}
      BaseSubString(Shared* element, PNT::Pointer::Init init) : inherited(element, init)
         {  if (!element->strings().isEmpty())
               inherited::setStartPosition(&element->strings().getSFirst(), 0); 
         }
      BaseSubString(const BaseSubString& source) = default;
      BaseSubString& operator=(const BaseSubString& source) = default;

      void setElement(Shared& element) { inherited::setElement(element); }
      BaseSubString& assign(Shared* element, PNT::Pointer::Init init)
         { return (BaseSubString&) inherited::assign(element, init); }

      void swap(BaseSubString& source) { inherited::swap(source); }

      Shared& getElement() const { return (Shared&) inherited::getElement(); }
      const Shared& getCElement() const { return (const Shared&) inherited::getElement(); }
      const Shared* key() const { return &getElement(); }
      Shared* operator->() const { return &getElement(); }
      Shared& operator*() const { return getElement(); }
      virtual bool isValid() const override { return inherited::isValid(); }
      bool isAffectedBy(MoveNotification& notification) const
         {  return inherited::isAffectedBy(notification) ? true
               : (notification.left().isValid()
                     || (notification.current().ssupport() == inherited::getEndPositionEstimate()));
         }
      
      class WriteStartPosition {
        private:
         BaseSubString& bssReference;
         typedef typename RepositoryPolicy::AtomicRepository AtomicRepository;
         
        public:
         WriteStartPosition(BaseSubString& reference) : bssReference(reference) {}

         void retrievePosition(Position& position) const
            {  bssReference.retrieveStartPosition(position.parSupport, position.uPosition); }
         void clearToPosition(const Position& position)
            {  bssReference.setStartPosition(position.parSupport, position.uPosition); }
         ComparisonResult compare(const WriteStartPosition& source) const
            {  Position position, sourcePosition;
               bssReference.retrieveStartPosition(position.parSupport, position.uPosition);
               source.bssReference.retrieveStartPosition(sourcePosition.parSupport, sourcePosition.uPosition);
               return position.compare(sourcePosition, bssReference.getElement());
            }
         ComparisonResult compare(const DStringRep::ReadPosition<Position, BaseSubString>& source) const
            {  AssumeCondition(&bssReference.getElement() == &source.getReference().getElement())
               Position position;
               bssReference.retrieveStartPosition(position.parSupport, position.uPosition);
               return position.compare(source.getImplementationPosition(), bssReference.getElement());
            }
         int getPosition() const
            {  Position position;
               bssReference.retrieveStartPosition(position.parSupport, position.uPosition);
               return position.getPosition(bssReference.getElement());
            }
         void inc() { bssReference.increaseStartPosition(1); }
         void dec() { bssReference.decreaseStartPosition(1); }
         void plusAssign(int shift)
            {  if (shift > 0)  
                  bssReference.increaseStartPosition(shift);
               else if (shift < 0)
                  bssReference.decreaseStartPosition(-shift);
            }
         void minusAssign(int shift)
            {  if (shift > 0) 
                  bssReference.decreaseStartPosition(shift);
               else if (shift < 0)
                  bssReference.increaseStartPosition(-shift);
            }
         void setToBegin()
            {  bssReference.setStartPosition(!bssReference.getElement().strings().isEmpty()
                  ? &bssReference.getElement().strings().getSFirst() : nullptr, 0);
            }
         void setToEnd()
            {  AtomicRepository* last = !bssReference.getElement().strings().isEmpty()
                  ? &bssReference.getElement().strings().getSFirst() : nullptr;
               bssReference.setStartPosition(last, last ? last->length() : 0);
            }
         int minus(const WriteStartPosition& source) const
            {  AssumeCondition(&bssReference.getElement() == &source.bssReference.getElement())
               Position position, sourcePosition;
               bssReference.retrieveStartPosition(position.parSupport, position.uPosition);
               source.bssReference.retrieveStartPosition(sourcePosition.parSupport, sourcePosition.uPosition);
               return position.minus(sourcePosition, bssReference.getElement()); 
            }
         int minus(const DStringRep::ReadPosition<Position, BaseSubString> source) const
            {  AssumeCondition(&bssReference.getElement() == &source.getReference().getElement())
               Position position;
               bssReference.retrieveStartPosition(position.parSupport, position.uPosition);
               return position.minus(source.getImplementationPosition(), bssReference.getElement()); 
            }
         void increaseToPosition(const Position& position)
            {  bssReference.increaseStartTo(position.parSupport, position.uPosition); }
         void decreaseToPosition(const Position& position)
            {  bssReference.decreaseStartTo(position.parSupport, position.uPosition); }
      };
      WriteStartPosition start() { return WriteStartPosition(*this); }
      typedef DStringRep::ReadPosition<Position, BaseSubString> ReadStartPosition;
      ReadStartPosition start() const
         {  Position result;
            inherited::retrieveStartPosition(result.parSupport, result.uPosition);
            return DStringRep::ReadPosition<Position, BaseSubString>(*this, result);
         }

      class WriteEndPosition {
        private:
         BaseSubString* pbssReference;
         
        public:
         WriteEndPosition(BaseSubString& reference) : pbssReference(&reference) {}
         WriteEndPosition(const WriteEndPosition& source) = default;

         void setToStart() { pbssReference->setEndToStartPosition(); }
         void inc() { pbssReference->increaseEndPosition(1); }
         void dec() { pbssReference->decreaseEndPosition(1); }
         void plusAssign(int add)
            {  if (add > 0)
                  pbssReference->increaseEndPosition(add);
               else if (add < 0)
                  pbssReference->decreaseEndPosition(-add);
            }
         void minusAssign(int sub)
            {  if (sub > 0)
                  pbssReference->decreaseEndPosition(sub);
               else if (sub < 0)
                  pbssReference->increaseEndPosition(-sub);
            }
         void setToEnd() { pbssReference->setEndToLastPosition(); }
         void increaseToPosition(const Position& position)
            {  pbssReference->increaseEndTo(position.parSupport, position.uPosition); }
         void decreaseToPosition(const Position& position)
            {  pbssReference->decreaseEndTo(position.parSupport, position.uPosition); }
      };
      WriteEndPosition end() { return WriteEndPosition(*this); }
   };
   friend class BaseSubString;
   friend class BaseSubString::WriteStartPosition;
   typedef typename inheritedImplementation::Position InheritedPosition;
   class Position : public InheritedPosition {
     private:
      typedef typename TListRepository<TypeChar, RepositoryPolicy>::InheritedPosition inherited;

     public:
      Position() {}
      Position(const Position& source) = default;
      Position& operator=(const Position& source) = default;
      friend class BaseSubString;
      friend class BaseSubString::WriteStartPosition;
      friend class BaseSubString::WriteEndPosition;
   };

   class MoveNotification : public DListRepository::TMoveNotification<TypeChar, RepositoryPolicy> {
     private:
      typedef DListRepository::TMoveNotification<TypeChar, RepositoryPolicy> inherited;
      
     public:
      typedef typename TListRepository<TypeChar, RepositoryPolicy>::Position Position;
      MoveNotification(const TListRepository<TypeChar, RepositoryPolicy>& repository)
         :  inherited(repository, repository) {}
      MoveNotification(const BaseSubString& call, const TListRepository<TypeChar, RepositoryPolicy>& repository)
         :  inherited(call, repository, repository) {}
      MoveNotification(const MoveNotification& source) = default;
      DDefineCopy(MoveNotification)

      MoveNotification& setSource(const Position& source) {  inherited::source() = source; return *this; }
      MoveNotification& setShift(int shift) {  inherited::setShift(shift); return *this; }
      friend class BaseSubString;
      friend class TListRepository<TypeChar, RepositoryPolicy>;
   };
   void realloc(int newAllocated) {}
   bool move(MoveNotification& notification)
      {  BaseSubString* subString = const_cast<BaseSubString*>((const BaseSubString*) notification.getOrigin());
         Position start;
         if (subString)
            subString->start().retrievePosition(start);
         inheritedImplementation::move(notification);
         
         if (subString && (notification.left().ssupport() || notification.right().ssupport())
               && subString->isAffectedBy(notification)) {
            notification.update(start);
            subString->start().clearToPosition(start);
            subString->end().setToStart();
            return false;
         };
         return true;
      }
   void setMove(MoveNotification& notification) { inheritedImplementation::move(notification); }
};

} // end of namespace STG

#endif // end of STG_TStringRepH
