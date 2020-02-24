#pragma once

#include "Pointer/ExtPointer.hpp"
#include "TString/String.hpp"
#include "dll.h"
#include "MemoryZone.h"
#include "MemoryState.h"
#include <vector>
#include <map>

enum ContractLocalization
   {  CLBeforeInstruction, CLAfterInstruction, CLBetweenInstruction };

class ContractGraph;
class Contract : public PNT::SharedElement, public STG::IOObject, public STG::Lexer::Base {
  public:
   class ContractPointer : public PNT::TSharedPointer<Contract> {
     public:
      ContractPointer() = default;
      ContractPointer(Contract* element, Init init)
         :  PNT::TSharedPointer<Contract>(element, init) {}
      ContractPointer(const ContractPointer& source) = default;
      DefineCopy(ContractPointer)

      virtual bool isEdge() const { return false; }

      class Key {
        public:
         Key(const COL::VirtualCollection&) {}
         typedef const uint64_t& KeyType;
         typedef uint64_t ControlKeyType;
         static KeyType key(const ContractPointer& element) { return element->getAddress(); }
         static ComparisonResult compare(KeyType fst, KeyType snd) { return tcompare(fst, snd); }
      };
   };

   class ListRegistration : public COL::List::Node {};
   class EdgeContract : public ContractPointer, public ListRegistration {
     public:
      virtual bool isEdge() const override { return true; }
      virtual bool isValid() const override { return ContractPointer::isValid(); }
   };

   typedef COL::TCopyCollection<COL::TList<EdgeContract,
           HandlerIntermediateCast<EdgeContract, ListRegistration, COL::List::Node> > >
      ListEdgeContract;

   struct ReadRuleResult : public MemoryStateConstraint::ReadRuleResult {
      typedef std::map<int, Contract*> IdMap;
      typedef std::map<int, std::vector<PNT::TSharedPointer<Contract>*> > PendingIds;
      IdMap* idMap = nullptr;
      PendingIds* pendingIds = nullptr;

      ReadRuleResult() = default;
      ReadRuleResult(IdMap& aidMap, PendingIds& apendingIds, struct _DomainElementFunctions* aelementFunctions,
            struct _Processor* aprocessor, struct _ProcessorFunctions* aprocessorFunctions)
         :  MemoryStateConstraint::ReadRuleResult(aelementFunctions, aprocessor, aprocessorFunctions),
            idMap(&aidMap), pendingIds(&apendingIds) {}
      ReadRuleResult(IdMap& aidMap, PendingIds& apendingIds, const MemoryStateConstraint::ReadRuleResult& functions)
         :  MemoryStateConstraint::ReadRuleResult(functions),
            idMap(&aidMap), pendingIds(&apendingIds) {}
      ReadRuleResult& operator=(const ReadRuleResult&) = default;

      bool setContractId(Contract& reference)
         {  auto localize = idMap->lower_bound(reference.getId());
            if (localize != idMap->end() && localize->first == reference.getId())
               return false;
            idMap->insert(localize, std::make_pair(reference.getId(), &reference));
            auto found = pendingIds->find(reference.getId());
            if (found != pendingIds->end()) {
               for (PNT::TSharedPointer<Contract>* pendingId : found->second) {
                  AssumeCondition(!pendingId->isValid())
                  *pendingId = PNT::TSharedPointer<Contract>(&reference, PNT::Pointer::Init());
               }
               pendingIds->erase(found);
            }
            return true;
         }
      void lookForContractId(int id, PNT::TSharedPointer<Contract>& result)
         {  auto found = idMap->find(id);
            if (found != idMap->end())
               result = PNT::TSharedPointer<Contract>(found->second, PNT::Pointer::Init());
            else {
               auto localize = pendingIds->lower_bound(id);
               if (localize == pendingIds->end() || localize->first == id)
                  localize = pendingIds->insert(localize, std::make_pair(id, std::vector<PNT::TSharedPointer<Contract>*>()));
               localize->second.push_back(&result);
            }
         }
   };

   struct WriteRuleResult : public MemoryStateConstraint::WriteRuleResult {
      PNT::PassPointer<ListEdgeContract::Cursor> edgeCursor;
      WriteRuleResult() = default;
      WriteRuleResult(struct _Processor* aprocessor, struct _ProcessorFunctions* aprocessorFunctions)
         :  MemoryStateConstraint::WriteRuleResult(aprocessor, aprocessorFunctions) {}
      void setFrom(const MemoryStateConstraint::WriteRuleResult& source)
         {  MemoryStateConstraint::WriteRuleResult::operator=(source); }
   };

  private:
   ListEdgeContract lecNexts;
   ListEdgeContract lecPreviouses;
   ContractPointer cpDominator;
   int uId = 0;
   uint64_t uAddress = 0;
   // uint64_t uAdditionalAddress;
   ContractLocalization clLocalization;
   MemoryZoneModifier zmZoneModifier;
   MemoryStateConstraint scMemoryConstraints; // should be true
   ContractGraph* pcgParent = nullptr;

   static bool setLocalizationFromText(ContractLocalization& localization,
         const STG::SubString& text)
      {  bool result = true;
         if (text == "before")
            localization = CLBeforeInstruction;
         else if (text == "after")
            localization = CLAfterInstruction;
         else if (text == "between")
            localization = CLBetweenInstruction;
         else
            result = false;
         return result;
      }
   static STG::SubString getTextFromLocalization(ContractLocalization localization)
      {  switch (localization) {
            case CLBeforeInstruction: return STG::SString("before");
            case CLAfterInstruction: return STG::SString("after");
            case CLBetweenInstruction: return STG::SString("between");
            default:
               break;
         }
         return STG::SString();
      }
   void applyOneTo(MemoryState& memoryState, struct _Processor* processor,
         struct _ProcessorFunctions* processorFunctions);

  public:
   Contract() = default;
   Contract(const Contract&) = default;
   DefineCopy(Contract)
   StaticInheritConversions(Contract, PNT::SharedElement)

   virtual bool isValid() const override
      {  return PNT::SharedElement::isValid() && (uId > 0) && (uAddress != 0); }

   int getId() const { return uId; }
   ReadResult readJSon(STG::JSon::CommonParser::State& state, STG::JSon::CommonParser::Arguments& arguments);
   WriteResult writeJSon(STG::JSon::CommonWriter::State& state, STG::JSon::CommonWriter::Arguments& arguments) const;

   bool isInitial() const;
   bool isFinal() const;
   void applyTo(MemoryState& memoryState, struct _Processor* processor,
         struct _ProcessorFunctions* processorFunctions);
   const uint64_t& getAddress() const { return uAddress; }
};

class Warning : public STG::IOObject {
  private:
   STG::SubString ssErrorMessage = STG::SString();

  public:

};

typedef COL::TCopyCollection<COL::TArray<Warning> > Warnings;

class ContractGraph : public COL::TCopyCollection<COL::TSortedArray<Contract::ContractPointer, Contract::ContractPointer::Key> >, public STG::IOObject, public STG::Lexer::Base {
  public:
   typedef Contract::ContractPointer ContractPointer;
   typedef COL::TCopyCollection<COL::TSortedArray<ContractPointer, ContractPointer::Key> > inherited;

  private:
   ContractPointer cpInitial, cpFinal;
   
   struct ReadRuleResult : public MemoryStateConstraint::ReadRuleResult {
      typedef std::map<int, Contract*> IdMap;
      typedef std::map<int, std::vector<PNT::TSharedPointer<Contract>*> > PendingIds;
      IdMap idMap;
      PendingIds pendingIds;

      ReadRuleResult() = default;
      ReadRuleResult(struct _DomainElementFunctions* aelementFunctions,
            struct _Processor* aprocessor, struct _ProcessorFunctions* aprocessorFunctions)
         :  MemoryStateConstraint::ReadRuleResult(aelementFunctions, aprocessor, aprocessorFunctions) {}
      ReadRuleResult(const ReadRuleResult&) = default;
      ReadRuleResult& operator=(const ReadRuleResult&) = default;
   };
   struct WriteRuleResult : public MemoryStateConstraint::WriteRuleResult {
      ContractGraph::PPCursor cursor;
      WriteRuleResult() = default;
      WriteRuleResult(const MemoryStateConstraint::WriteRuleResult& source)
         :  MemoryStateConstraint::WriteRuleResult(source) {}
      WriteRuleResult(struct _Processor* aprocessor, struct _ProcessorFunctions* aprocessorFunctions)
         :  MemoryStateConstraint::WriteRuleResult(aprocessor, aprocessorFunctions) {}
      WriteRuleResult(const WriteRuleResult&) = default;
      WriteRuleResult& operator=(const WriteRuleResult&) = default;
   };

  public:
   ContractGraph() {}
   virtual ~ContractGraph() override
      {  inherited::foreachDo([](const ContractPointer& pointer)
            {  if (pointer.isValid())
                  delete &*pointer;
               return true;
            });
      }
   bool loadFromFile(const char* filename, struct _DomainElementFunctions* domainFunctions,
         struct _Processor* processor, struct _ProcessorFunctions* processorFunctions)
      {  STG::DIOObject::IFStream inputFile(filename);
         if (!inputFile.good())
            return false;
         STG::JSon::CommonParser parser(*this, (ReadRuleResult*) nullptr, STG::JSon::CommonParser::Parse());
         parser.state().getSResult((ReadRuleResult*) nullptr) = ReadRuleResult(domainFunctions, processor, processorFunctions);
         parser.setPartialToken();
         parser.parse(inputFile);
         return true;
      }
   bool saveFromFile(const char* filename)
      {  STG::DIOObject::OFStream outputFile(filename);
         STG::JSon::CommonWriter writer(*this, (WriteRuleResult*) nullptr, STG::JSon::CommonWriter::Write());
         writer.write(outputFile);
         return true;
      }
   const ContractPointer& getInitial() const { return cpInitial; }
   const ContractPointer& getFinal() const { return cpFinal; }

   ReadResult readJSon(STG::JSon::CommonParser::State& state, STG::JSon::CommonParser::Arguments& arguments);
   WriteResult writeJSon(STG::JSon::CommonWriter::State& state, STG::JSon::CommonWriter::Arguments& arguments) const;
};

inline bool
Contract::isInitial() const
   {  return pcgParent && pcgParent->getInitial().key() == this; }
   
inline bool
Contract::isFinal() const
   {  return pcgParent && pcgParent->getFinal().key() == this; }

class ContractCoverage : public STG::IOObject {
  public:
   typedef Contract::ContractPointer ContractPointer;

  private:
   class ContractConnection : public EnhancedObject {
     private:
      ContractPointer ccOrigin, ccTarget;

     public:
      ContractConnection(Contract& origin, Contract& target)
         :  ccOrigin(&origin, PNT::Pointer::Init()), ccTarget(&target, PNT::Pointer::Init()) {}
   };
   ContractGraph& cgReference;
   COL::TCopyCollection<COL::TArray<ContractConnection> > aContent;

  public:
   ContractCoverage(ContractGraph& reference) : cgReference(reference) {}
   ContractCoverage(const ContractCoverage&) = default;

   void add(Contract& origin, Contract& target)
      {  aContent.insertNewAtEnd(new ContractConnection(origin, target)); }
   bool isComplete() const { return true; }

};
