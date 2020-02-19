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

class MemoryZone : public STG::IOObject {
  private:
   class MemoryZonePoolPointer : public PNT::TSharedPointer<MemoryZonePool> {
     private:
      typedef PNT::TSharedPointer<MemoryZonePool> inherited;

     public:
      MemoryZonePoolPointer() = default;
      MemoryZonePoolPointer(MemoryZonePool* value, PNT::Pointer::Init init)
         :  inherited(value, init) {}
      MemoryZonePoolPointer(const MemoryZonePoolPointer&) = default;
      virtual ~MemoryZonePoolPointer() override { inherited::release(); }
   };

   int uZoneId = 0;
   MemoryZonePoolPointer spPool;
   uint64_t uStartAddressInCode = 0;
   Expression eStartAddress;
   Expression eLength;
   STG::SubString ssName = STG::SString();

  public:
   MemoryZone() : spPool(new MemoryZonePool(), PNT::Pointer::Init())
      {  uZoneId = spPool->generateNewId(); }
   MemoryZone(MemoryZonePool& pool) : spPool(&pool, PNT::Pointer::Init())
      {  uZoneId = spPool->generateNewId(); }
   MemoryZone(MemoryZonePool& pool, uint64_t startAddressInCode,
         const STG::SubString& name, Expression&& start, Expression&& length)
      :  uZoneId(pool.generateNewId()), spPool(&pool, PNT::Pointer::Init()),
         uStartAddressInCode(startAddressInCode), eStartAddress(std::move(start)),
         eLength(std::move(length)), ssName(name) {}
   MemoryZone(const MemoryZone& source) = default;

   virtual bool isValid() const override { return uZoneId > 0; }
   const int& getId() const { return uZoneId; }
   MemoryZonePool& getPool() const { return *spPool; }
   const STG::SubString& getName() const { return ssName; }

   void initialize(uint64_t startAddressInCode,
         const STG::SubString& name, Expression&& start, Expression&& length)
      {  AssumeCondition(uZoneId == 0);
         uZoneId = spPool->generateNewId();
         uStartAddressInCode = startAddressInCode;
         eStartAddress = std::move(start);
         eLength = std::move(length);
         ssName = name;
      }
   void clear()
      {  uZoneId = 0;
         uStartAddressInCode = 0;
         eStartAddress.clear();
         eLength.clear();
         ssName.clear();
      }
   void rename(const STG::SubString& newName) { ssName = newName; }
   PNT::PassPointer<MemoryZone> newZoneFromSplit(uint64_t startAddressInCode,
         const STG::SubString& name, Expression&& start)
      {  Expression newLength(eLength);
         eLength = start;
         eLength -= eStartAddress;
         newLength -= eLength;

         return PNT::PassPointer<MemoryZone>(new MemoryZone(*spPool, startAddressInCode,
               name, std::move(start), std::move(newLength)), PNT::Pointer::Init());
      }
   void mergeWith(MemoryZone&& source)
      {  eLength = std::move(source.eStartAddress);
         eLength -= eStartAddress;
         eLength += source.eLength;
         source.clear();
      }

   class Key {
     public:
      Key(const COL::VirtualCollection&) {}
      typedef const int& KeyType;
      typedef int ControlKeyType;
      static KeyType key(const MemoryZone& element) { return element.getId(); }
      static ComparisonResult compare(KeyType fst, KeyType snd) { return fcompare(fst, snd); }
   };
};

class MemoryZones : public COL::TCopyCollection<COL::TSortedArray<MemoryZone, MemoryZone::Key> > {
  public:
   MemoryZones() = default;
   MemoryZones(const MemoryZones&) = default;

};

class MemoryZoneAction : public STG::IOObject, public STG::Lexer::Base {
  public:
   typedef struct _DomainElementFunctions* RuleResult;

  public:
   enum TypeAction { TAUndefined, TACreate, TARename, TASplit, TAMerge };
 
  public:
   MemoryZoneAction() = default;
   MemoryZoneAction(const MemoryZoneAction&) = default;
   DefineCopy(MemoryZoneAction)

   virtual TypeAction getType() const { return TAUndefined; }
   virtual void apply(MemoryZones& zones, uint64_t startAddress) {}
   virtual ReadResult readJSon(STG::JSon::CommonParser::State& state, STG::JSon::CommonParser::Arguments& arguments) { AssumeUncalled return RRContinue; }
   virtual WriteResult writeJSon(STG::JSon::CommonWriter::State& state, STG::JSon::CommonWriter::Arguments& arguments) const { AssumeUncalled return WRNeedEvent; }
};

class MemoryZoneCreate : public MemoryZoneAction {
  private:
   Expression eStartAddress;
   Expression eLength;
   STG::SubString ssName = STG::SString();

  public:
   MemoryZoneCreate() = default;
   MemoryZoneCreate(const MemoryZoneCreate&) = default;
   DefineCopy(MemoryZoneCreate)

   virtual TypeAction getType() const override { return TACreate; }
   virtual void apply(MemoryZones& zones, uint64_t startAddress) override
      {  MemoryZone* newZone;
         if (zones.isEmpty())
            zones.insertNewAtEnd(newZone = new MemoryZone());
         else
            zones.insertNewAtEnd(newZone = new MemoryZone(zones.getFirst().getPool()));
         newZone->initialize(startAddress, ssName, Expression(eStartAddress), Expression(eLength));
      }
   virtual ReadResult readJSon(STG::JSon::CommonParser::State& state, STG::JSon::CommonParser::Arguments& arguments) override;
   virtual WriteResult writeJSon(STG::JSon::CommonWriter::State& state, STG::JSon::CommonWriter::Arguments& arguments) const override;
};

class MemoryZoneRename : public MemoryZoneAction {
  private:
   STG::SubString ssOldName = STG::SString();
   STG::SubString ssNewName = STG::SString();

  public:
   MemoryZoneRename() = default;
   MemoryZoneRename(const MemoryZoneRename&) = default;
   DefineCopy(MemoryZoneRename)

   virtual TypeAction getType() const override { return TARename; }
   virtual void apply(MemoryZones& zones, uint64_t startAddress) override
      {  for (auto& zone : zones)
            if (zone.getName() == ssOldName) {
               zone.rename(ssNewName);
               break;
            }
      }
   virtual ReadResult readJSon(STG::JSon::CommonParser::State& state, STG::JSon::CommonParser::Arguments& arguments) override;
   virtual WriteResult writeJSon(STG::JSon::CommonWriter::State& state, STG::JSon::CommonWriter::Arguments& arguments) const override;
};

class MemoryZoneSplit : public MemoryZoneAction {
  private:
   Expression eNewStartAddress;
   STG::SubString ssOldName = STG::SString();
   STG::SubString ssNewName = STG::SString();

  public:
   MemoryZoneSplit() = default;
   MemoryZoneSplit(const MemoryZoneSplit&) = default;
   DefineCopy(MemoryZoneSplit)

   virtual TypeAction getType() const override { return TASplit; }
   virtual void apply(MemoryZones& zones, uint64_t startAddress) override
      {  for (auto& zone : zones)
            if (zone.getName() == ssOldName) {
               auto newZone = zone.newZoneFromSplit(startAddress, ssNewName, Expression(eNewStartAddress));
               zones.insertNewAtEnd(newZone.extractElement());
               break;
            }
      }
   virtual ReadResult readJSon(STG::JSon::CommonParser::State& state, STG::JSon::CommonParser::Arguments& arguments) override;
   virtual WriteResult writeJSon(STG::JSon::CommonWriter::State& state, STG::JSon::CommonWriter::Arguments& arguments) const override;
};

class MemoryZoneMerge : public MemoryZoneAction {
  private:
   STG::SubString ssFirstName = STG::SString();
   STG::SubString ssSecondName = STG::SString();

  public:
   MemoryZoneMerge() = default;
   MemoryZoneMerge(const MemoryZoneMerge&) = default;
   DefineCopy(MemoryZoneMerge)

   virtual TypeAction getType() const override { return TAMerge; }
   virtual void apply(MemoryZones& zones, uint64_t startAddress) override;

   virtual ReadResult readJSon(STG::JSon::CommonParser::State& state, STG::JSon::CommonParser::Arguments& arguments) override;
   virtual WriteResult writeJSon(STG::JSon::CommonWriter::State& state, STG::JSon::CommonWriter::Arguments& arguments) const override;
};

class MemoryZoneModifier : public STG::IOObject, public STG::Lexer::Base {
  public:
   typedef COL::TCopyCollection<COL::TArray<MemoryZoneAction> > MemoryZoneActions;
   struct ReadRuleResult {
      struct _DomainElementFunctions* functions;
      MemoryZoneAction::TypeAction type = MemoryZoneAction::TAUndefined;

      ReadRuleResult(struct _DomainElementFunctions* afunctions) : functions(afunctions) {}
   };

   typedef PNT::PassPointer<MemoryZoneActions::Cursor> WriteRuleResult;

  private:
   MemoryZoneActions azaActions;

   static void setTypeActionFromText(MemoryZoneAction::TypeAction& type, const STG::SubString& text)
      {  if (text == "create")
            type = MemoryZoneAction::TACreate;
         else if (text == "rename")
            type = MemoryZoneAction::TARename;
         else if (text == "split")
            type = MemoryZoneAction::TASplit;
         else if (text == "merge")
            type = MemoryZoneAction::TAMerge;
      }
   static PNT::PassPointer<MemoryZoneAction> newZoneAction(MemoryZoneAction::TypeAction type)
      {  PNT::PassPointer<MemoryZoneAction> result;
         switch (type) {
            case MemoryZoneAction::TACreate:
               result.absorbElement(new MemoryZoneCreate());
            case MemoryZoneAction::TARename:
               result.absorbElement(new MemoryZoneRename());
            case MemoryZoneAction::TASplit:
               result.absorbElement(new MemoryZoneSplit());
            case MemoryZoneAction::TAMerge:
               result.absorbElement(new MemoryZoneMerge());
               break;
            default:
               break;
         }
         return result;
      }
   static STG::SubString getTextFromTypeAction(MemoryZoneAction::TypeAction type)
      {  switch (type) {
            case MemoryZoneAction::TACreate: return STG::SString("create");
            case MemoryZoneAction::TARename: return STG::SString("rename");
            case MemoryZoneAction::TASplit: return STG::SString("split");
            case MemoryZoneAction::TAMerge: return STG::SString("merge");
            default:
               break;
         }
         return STG::SString();
      }

  public:
   MemoryZoneModifier() = default;
   MemoryZoneModifier(const MemoryZoneModifier&) = default;

   void apply(MemoryZones& zones, uint64_t startAddress)
      {  for (auto& action : azaActions)
            action.apply(zones, startAddress);
      }
   ReadResult readJSon(STG::JSon::CommonParser::State& state, STG::JSon::CommonParser::Arguments& arguments);
   WriteResult writeJSon(STG::JSon::CommonWriter::State& state, STG::JSon::CommonWriter::Arguments& arguments) const;
};
