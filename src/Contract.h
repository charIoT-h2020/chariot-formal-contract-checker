#pragma once

#include "Pointer/ExtPointer.hpp"
#include "TString/String.hpp"
#include "dll.h"
#include "MemoryZone.h"
#include <vector>

enum ContractLocalization
   {  CLBeforeInstruction, CLAfterInstruction, CLBetweenInstruction };

class ContractGraph;
class Contract : public PNT::SharedElement, public STG::IOObject {
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

  private:
   class ListRegistration : public COL::List::Node {};
   class EdgeContract : public ContractPointer, public ListRegistration {
     public:
      virtual bool isEdge() const { return true; }
   };

   typedef COL::TCopyCollection<COL::TList<EdgeContract,
           HandlerIntermediateCast<EdgeContract, ListRegistration, COL::List::Node> > >
      ListEdgeContract;
   ListEdgeContract lecNexts;
   ListEdgeContract lecPreviouses;
   ContractPointer ccDominator;
   uint64_t uAddress = 0;
   // uint64_t uAdditionalAddress;
   ContractLocalization clLocalization;
   MemoryZoneModifier zmZoneModifier;
   COL::TCopyCollection<COL::TArray<Expression> > aeProperties; // should be true
   ContractGraph* pcgParent = nullptr;

  public:
   Contract() = default;
   Contract(const Contract&) = default;
   DefineCopy(Contract)
   StaticInheritConversions(Contract, PNT::SharedElement)

   bool isInitial() const;
   bool isFinal() const;

   const uint64_t& getAddress() const { return uAddress; }
};

class Warning : public STG::IOObject {
  private:
   STG::SubString ssErrorMessage = STG::SString();

  public:

};

typedef COL::TCopyCollection<COL::TArray<Warning> > Warnings;

class ContractGraph : public COL::TCopyCollection<COL::TSortedArray<Contract::ContractPointer, Contract::ContractPointer::Key> >, public STG::IOObject {
  public:
   typedef Contract::ContractPointer ContractPointer;
   typedef COL::TCopyCollection<COL::TSortedArray<ContractPointer, ContractPointer::Key> > inherited;

  private:
   ContractPointer cpInitial, cpFinal;

  public:
   ContractGraph() {}
   // [TODO] to implement
   void loadFromFile(const char* filename, struct _Processor* processor) {}
   const ContractPointer& getInitial() const { return cpInitial; }
   const ContractPointer& getFinal() const { return cpFinal; }
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
