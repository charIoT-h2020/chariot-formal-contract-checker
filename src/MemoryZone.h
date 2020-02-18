#pragma once

#include "Expression.h"

class MemoryZone;
class MemoryZonePool : public PNT::SharedElement {
  private:
   int uZoneCounter = 0;
   friend class MemoryZone;

   int generateNewId() { return ++uZoneCounter; }

  public:
   MemoryZonePool() = default;

};

class MemoryZone : public PNT::SharedElement, public STG::IOObject {
  private:
   int uZoneId = 0;
   PNT::TSharedPointer<MemoryZonePool> spPool;
   uint64_t uStartAddressInCode = 0;
   Expression eStartAddress;
   Expression eLength;
   STG::SString sName;

  public:
   MemoryZone(MemoryZonePool& pool) : spPool(&pool, PNT::Pointer::Init()) {}
   MemoryZone(MemoryZonePool& pool, uint64_t startAddressInCode,
         const STG::SubString& name, Expression&& start, Expression&& length)
      :  uZoneId(pool.generateNewId()), spPool(&pool, PNT::Pointer::Init()),
         uStartAddressInCode(startAddressInCode), eStartAddress(std::move(start)),
         eLength(std::move(length)), sName(name) {}
   MemoryZone(const MemoryZone& source) = default;
   StaticInheritConversions(MemoryZone, PNT::SharedElement)

   virtual bool isValid() const override { return uZoneId > 0; }
   void initialize(uint64_t startAddressInCode,
         const STG::SubString& name, Expression&& start, Expression&& length)
      {  AssumeCondition(uZoneId == 0);
         uZoneId = spPool->generateNewId();
         uStartAddressInCode = startAddressInCode;
         eStartAddress = std::move(start);
         eLength = std::move(length);
         sName = name;
      }
   void clear()
      {  uZoneId = 0;
         uStartAddressInCode = 0;
         eStartAddress.clear();
         eLength.clear();
         sName.clear();
      }
   void rename(const STG::SubString& newName) { sName = newName; }
   PNT::PassPointer<MemoryZone> newZoneFromSplit(uint64_t startAddressInCode,
         const STG::SubString& name, Expression&& start)
      {  Expression newLength(eLength);
         eLength = start;
         eLength -= eStartAddress;
         newLength -= eLength;

         return PNT::PassPointer<MemoryZone>(new MemoryZone(*spPool, startAddressInCode,
               name, std::move(start), std::move(newLength)), PNT::Pointer::Init());
      }
   void merge(MemoryZone&& source)
      {  eLength = std::move(source.eStartAddress);
         eLength -= eStartAddress;
         eLength += source.eLength;
         source.clear();
      }
   int getId() const { return uZoneId; }
};

class MemoryZoneAction : public STG::IOObject {
  public:
   enum TypeAction { TAUndefined, TACreate, TARename, TASplit, TAMerge };
 
  public:
   virtual TypeAction getType() const { return TAUndefined; }
};

class MemoryZoneModifier : public STG::IOObject {
  private:
   COL::TCopyCollection<COL::TArray<MemoryZoneAction> > azaActions;

  public:

};
