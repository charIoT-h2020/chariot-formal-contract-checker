#pragma once

#include "dll.h"
#include "decsec_callback.h"
#include "DomainValue.h"
#include "MemoryZone.h"

class MemoryInterpretParameters {

};

// to state hypotheses on BaseLocation

class BaseLocation : public PNT::SharedCollection::Element {
  public:
   enum Type { TUndefined, TRegister, TMemory, TLocal };

  private:
   typedef PNT::SharedCollection::Element inherited;

  protected:
   virtual ComparisonResult _compare(const EnhancedObject& asource) const override
      {  ComparisonResult result = inherited::_compare(asource);
         if (result == CREqual) {
            const auto& source = static_cast<const BaseLocation&>(asource);
            result = fcompare(getType(), source.getType());
         }
         return result;
      }

  public:
   BaseLocation() = default;
   BaseLocation(const BaseLocation&) = default;
   DefineCopy(BaseLocation)

   virtual Type getType() const { return TUndefined; }

   class Key {
     public:
      typedef BaseLocation TypeOfKey;
      typedef const TypeOfKey& KeyType;
      typedef const TypeOfKey& ControlKeyType;
      static const TypeOfKey& key(const BaseLocation& element) { return element; }
   };
};

class BaseLocations : public COL::TCopyCollection<COL::SortedArray<BaseLocation, BaseLocation::Key> > {
  public:
   BaseLocations() = default;
   BaseLocations(const BaseLocations&) = default;
   DefineCopy(BaseLocations)


};

// implicit hypotheses

class Predicate
   :  public PNT::TSharedCollection<BaseLocation, HandlerCast<BaseLocation, PNT::SharedCollection::Element> >,
      public STG::IOObject, public STG::Lexer::Base {
  private:
   typedef PNT::TSharedCollection<BaseLocation, HandlerCast<BaseLocation, PNT::SharedCollection::Element> > inherited;
   Expression ePredicate;

  public:
   Predicate() = default;
   Predicate(const Predicate&) = default;
   DefineCopy(Predicate)
   StaticInheritConversions(Predicate, inherited)
};

class LetVariable : public BaseLocation, public STG::IOObject, public STG::Lexer::Base {
  private:
   typedef BaseLocation inherited;
   Expression eDefinition;
   STG::SubString ssName = STG::SString();

  protected:
   virtual ComparisonResult _compare(const EnhancedObject& asource) const override
      {  ComparisonResult result = inherited::_compare(asource);
         if (result == CREqual) {
            const auto& source = static_cast<const LetVariable&>(castFromCopyHandler(asource));
            result = ssName.compare(source.ssName);
         }
         return result;
      }

  public:
   LetVariable(const STG::SubString& name) : ssName(name) {}
   LetVariable(const LetVariable&) = default;
   DefineCopy(LetVariable)
   StaticInheritConversions(LetVariable, BaseLocation)

   virtual Type getType() const { return TLocal; }
   class Key {
     public:
      typedef STG::SubString TypeOfKey;	// TypeOfKey inherits from EnhancedObject
      typedef const TypeOfKey& KeyType;
      typedef const TypeOfKey& ControlKeyType;
      static const TypeOfKey& key(const LetVariable& element) { return element.ssName; }
   };
   friend class Key;
};

class RegisterLocation : public BaseLocation {
  private:
   typedef BaseLocation inherited;
   int uRegisterIndex;

  protected:
   virtual ComparisonResult _compare(const EnhancedObject& asource) const override
      {  ComparisonResult result = inherited::_compare(asource);
         if (result == CREqual) {
            const auto& source = static_cast<const RegisterLocation&>(asource);
            result = fcompare(uRegisterIndex, source.uRegisterIndex);
         }
         return result;
      }

  public:
   RegisterLocation(int registerIndex) : uRegisterIndex(registerIndex) {}
   RegisterLocation(const RegisterLocation&) = default;
   DefineCopy(RegisterLocation)

   virtual Type getType() const override { return TRegister; }
};

class MemoryLocation : public BaseLocation {
  private:
   typedef BaseLocation inherited;
   Expression eLocation;

  protected:
   virtual ComparisonResult _compare(const EnhancedObject& asource) const override
      {  ComparisonResult result = inherited::_compare(asource);
         if (result == CREqual) {
            const auto& source = static_cast<const MemoryLocation&>(asource);
            result = eLocation.compare(source.eLocation);
         }
         return result;
      }

  public:
   MemoryLocation(const Expression& location) : eLocation(location) {}
   MemoryLocation(const MemoryLocation&) = default;
   DefineCopy(MemoryLocation)

   virtual Type getType() const override { return TMemory; }
};

class ImplicitHypotheses : public PNT::SharedElement {
  private:
   COL::TCopyCollection<COL::TArray<Predicate> > apConstraints;
   COL::TCopyCollection<COL::SortedArray<LetVariable, LetVariable::Key> > alDefinitions;

  public:
   ImplicitHypotheses() = default;
   ImplicitHypotheses(const ImplicitHypotheses&) = default;


};

// memory states

class Processor;
class VirtualAddressConstraint;
class MemoryState : public STG::IOObject {
  private:
   class RegisterValue : public COL::GenericAVL::Node {
     private:
      typedef COL::GenericAVL::Node inherited;
      int uRegister;
      DomainValue dvValue;

     public:
      RegisterValue(int aregister, DomainValue&& value)
         :  uRegister(aregister), dvValue(std::move(value)) {}
      RegisterValue(const RegisterValue& source) = default;
      DefineCopy(RegisterValue)

      const int& getRegister() const { return uRegister; }
      void setValue(DomainValue&& value) { dvValue = std::move(value); }
      const DomainValue& getValue() const { return dvValue; }
      class Key {
        public:
         Key(const COL::VirtualCollection&) {}
         typedef const int& KeyType;
         typedef int ControlKeyType;
         static KeyType key(const RegisterValue& element) { return element.getRegister(); }
         static ComparisonResult compare(KeyType fst, KeyType snd) { return fcompare(fst, snd); }
      };
   };

   int uRegisterNumber = 0;
   typedef COL::TCopyCollection<COL::TSortedAVL<RegisterValue, RegisterValue::Key> > RegisterContent;
   RegisterContent rcRegisters;
   struct _DomainElementFunctions* domainFunctions;
   MemoryZones mzMemoryZones;
   PNT::TSharedPointer<ImplicitHypotheses> sphImplicitHypotheses;

  public:
   static MemoryModelFunctions functions;

  private:
   /* call-back functions */
   static void set_number_of_registers(MemoryModel* amemory, int numbers)
      {  MemoryState* memory = reinterpret_cast<MemoryState*>(amemory);
         memory->uRegisterNumber = numbers;
      }
   static void set_register_value(MemoryModel* amemory, int registerIndex,
         DomainElement* avalue, InterpretParameters* parameters,
         unsigned* error /* set of MemoryEvaluationErrorFlags */)
      {  MemoryState* memory = reinterpret_cast<MemoryState*>(amemory);
         DomainValue value(std::move(*avalue), memory->domainFunctions);
         RegisterContent::Cursor cursor(memory->rcRegisters);
         auto locationResult = memory->rcRegisters.locateKey(registerIndex, cursor);
         if (locationResult)
            cursor.elementSAt().setValue(std::move(value));
         else
            memory->rcRegisters.add(new RegisterValue(registerIndex, std::move(value)),
                  locationResult.queryInsertionParameters().setFreeOnError(), &cursor);
      }
   static DomainElement get_register_value(MemoryModel* amemory,
         int registerIndex, InterpretParameters* parameters,
         unsigned* error /* set of MemoryEvaluationErrorFlags */,
         DomainElementFunctions** elementFunctions)
      {  MemoryState* memory = reinterpret_cast<MemoryState*>(amemory);
         RegisterContent::Cursor cursor(memory->rcRegisters);
         DomainValue result(memory->domainFunctions);
         if (memory->rcRegisters.locateKey(registerIndex, cursor, COL::VirtualCollection::RPExact))
            result = cursor.elementAt().getValue();
         else // [TODO] asks the architecture for the size of the register or return invalid result
            result = DomainValue((*memory->domainFunctions->multibit_create_top)(32, true /* isSymbolic */), memory->domainFunctions);
         if (elementFunctions)
            *elementFunctions = memory->domainFunctions;
         return result.extractElement();
      }

   static DomainElement load_multibit_value(MemoryModel* amemory,
         DomainElement indirect_address, size_t size, InterpretParameters* parameters,
         unsigned* error, DomainElementFunctions** elementFunctions)
      {  MemoryState* memory = reinterpret_cast<MemoryState*>(amemory);
         DomainValue address((*memory->domainFunctions->clone)(indirect_address), memory->domainFunctions);
         DomainValue result((*memory->domainFunctions->multibit_create_top)(size, true /* isSymbolic */), memory->domainFunctions);
         if (elementFunctions)
            *elementFunctions = memory->domainFunctions;
         return result.extractElement();
      }
   static DomainElement load_multibit_disjunctive_value(MemoryModel* amemory,
         DomainElement indirect_address, size_t size, InterpretParameters* parameters,
         unsigned* error, DomainElementFunctions** elementFunctions)
      {  MemoryState* memory = reinterpret_cast<MemoryState*>(amemory);
         DomainValue address((*memory->domainFunctions->clone)(indirect_address), memory->domainFunctions);
         DomainValue result((*memory->domainFunctions->multibit_create_top)(size, true /* isSymbolic */), memory->domainFunctions);
         if (elementFunctions)
            *elementFunctions = memory->domainFunctions;
         return result.extractElement();
      }
   static DomainElement load_multifloat_value(MemoryModel* amemory,
         DomainElement indirect_address, size_t size, InterpretParameters* parameters,
         unsigned* error, DomainElementFunctions** elementFunctions)
      {  MemoryState* memory = reinterpret_cast<MemoryState*>(amemory);
         DomainValue address((*memory->domainFunctions->clone)(indirect_address), memory->domainFunctions);
         DomainValue result((*memory->domainFunctions->multifloat_create_top)(size, true /* isSymbolic */), memory->domainFunctions);
         if (elementFunctions)
            *elementFunctions = memory->domainFunctions;
         return result.extractElement();
      }
   static void store_value(MemoryModel* amemory, DomainElement indirect_address,
         DomainElement avalue, InterpretParameters* parameters, unsigned* error)
      {  MemoryState* memory = reinterpret_cast<MemoryState*>(amemory);
         DomainValue value((*memory->domainFunctions->clone)(avalue), memory->domainFunctions);
         DomainValue address((*memory->domainFunctions->clone)(indirect_address), memory->domainFunctions);
      }

  public:
   MemoryState(struct _DomainElementFunctions* adomainFunctions)
      :  domainFunctions(adomainFunctions) {}
   MemoryState(const MemoryState&) = default;

   void intersectWith(const VirtualAddressConstraint& contract);
   MemoryModelFunctions* getFunctions() const { return &functions; }
   void write(std::ostream& out) const { out << "end of memory description\n"; }
   const struct _DomainElementFunctions* getDomainFunctions() const { return domainFunctions; }
   // [TODO] to implement
   bool contain(const MemoryState& source, const MemoryInterpretParameters& parameters) { return true; } 
};

class VirtualAddressConstraint
   :  public PNT::TSharedCollection<BaseLocation, HandlerCast<BaseLocation, PNT::SharedCollection::Element> >,
      public STG::IOObject, public STG::Lexer::Base {
  public:
   struct ReadRuleResult {
      struct _DomainElementFunctions* elementFunctions = nullptr;
      struct _Processor* processor = nullptr;
      struct _ProcessorFunctions* processorFunctions = nullptr;

      ReadRuleResult() = default;
      ReadRuleResult(struct _DomainElementFunctions* aelementFunctions,
            struct _Processor* aprocessor, struct _ProcessorFunctions* aprocessorFunctions)
         :  elementFunctions(aelementFunctions), processor(aprocessor), processorFunctions(aprocessorFunctions) {}
   };

   struct WriteRuleResult {
      struct _Processor* processor = nullptr;
      struct _ProcessorFunctions* processorFunctions = nullptr;

      WriteRuleResult() = default;
      WriteRuleResult(struct _Processor* aprocessor, struct _ProcessorFunctions* aprocessorFunctions)
         :  processor(aprocessor), processorFunctions(aprocessorFunctions) {}
   };

  private:
   typedef PNT::TSharedCollection<BaseLocation, HandlerCast<BaseLocation, PNT::SharedCollection::Element> > inherited;
   Expression eConstraint; 
   STG::SubString ssZoneName = STG::SString();

  protected:
   virtual bool readFromKey(const STG::SubString& key, STG::JSon::CommonParser::State& state,
         STG::JSon::CommonParser::Arguments& arguments, ReadResult& result) { return false; }
   virtual bool writeToKey(STG::JSon::CommonWriter::State& state,
         STG::JSon::CommonWriter::Arguments& arguments, WriteResult& result) const { return false; }

  public:
   VirtualAddressConstraint() = default;
   VirtualAddressConstraint(const VirtualAddressConstraint&) = default;
   DefineCopy(VirtualAddressConstraint)
   StaticInheritConversions(VirtualAddressConstraint, inherited)

   virtual bool isRegister() const { return false; }
   virtual bool isIndirect() const { return false; }
   ReadResult readJSon(STG::JSon::CommonParser::State& state, STG::JSon::CommonParser::Arguments& arguments);
   WriteResult writeJSon(STG::JSon::CommonWriter::State& state, STG::JSon::CommonWriter::Arguments& arguments) const;
};

class RegisterConstraint : public VirtualAddressConstraint {
  private:
   int uRegisterIndex = -1;

  protected:
   virtual bool readFromKey(const STG::SubString& key, STG::JSon::CommonParser::State& state,
         STG::JSon::CommonParser::Arguments& arguments, ReadResult& result) override;
   virtual bool writeToKey(STG::JSon::CommonWriter::State& state,
         STG::JSon::CommonWriter::Arguments& arguments, WriteResult& result) const override;

  public:
   RegisterConstraint() = default;
   RegisterConstraint(const RegisterConstraint&) = default;

   virtual bool isRegister() const override { return true; }
};

class IndirectAddressConstraint : public VirtualAddressConstraint {
  private:
   Expression eAddress;

  protected:
   virtual bool readFromKey(const STG::SubString& key, STG::JSon::CommonParser::State& state,
         STG::JSon::CommonParser::Arguments& arguments, ReadResult& result) override;
   virtual bool writeToKey(STG::JSon::CommonWriter::State& state,
         STG::JSon::CommonWriter::Arguments& arguments, WriteResult& result) const override;

  public:
   IndirectAddressConstraint() = default;
   IndirectAddressConstraint(const IndirectAddressConstraint&) = default;

   virtual bool isIndirect() const override { return true; }
};

class MemoryStateConstraint : public COL::TCopyCollection<COL::TArray<VirtualAddressConstraint> >, public STG::IOObject, public STG::Lexer::Base {
  public:
   enum TypeConstraint { TCUndefined, TCRegister, TCIndirect };
   struct ReadRuleResult : public VirtualAddressConstraint::ReadRuleResult {
      TypeConstraint type = TCUndefined;
      ReadRuleResult() = default;
      ReadRuleResult(struct _DomainElementFunctions* aelementFunctions,
            struct _Processor* aprocessor, struct _ProcessorFunctions* aprocessorFunctions)
         :  VirtualAddressConstraint::ReadRuleResult(aelementFunctions, aprocessor, aprocessorFunctions) {}
      ReadRuleResult(const ReadRuleResult&) = default;
   };

   struct WriteRuleResult : public VirtualAddressConstraint::WriteRuleResult {
      PNT::PassPointer<COL::TArray<VirtualAddressConstraint>::Cursor> writeCursor;
      WriteRuleResult() = default;
      WriteRuleResult(struct _Processor* aprocessor, struct _ProcessorFunctions* aprocessorFunctions)
         :  VirtualAddressConstraint::WriteRuleResult(aprocessor, aprocessorFunctions) {}
      WriteRuleResult(const WriteRuleResult&) = default;
   };

  private:
   static void setTypeConstraintFromText(TypeConstraint& type, const STG::SubString& text)
      {  if (text == "register")
            type = TCRegister;
         else if (text == "indirect")
            type = TCIndirect;
      }
   static PNT::PassPointer<VirtualAddressConstraint> newAddressConstraint(TypeConstraint type)
      {  PNT::PassPointer<VirtualAddressConstraint> result;
         switch (type) {
            case TCRegister:
               result.absorbElement(new RegisterConstraint());
            case TCIndirect:
               result.absorbElement(new IndirectAddressConstraint());
            default:
               break;
         }
         return result;
      }
   static STG::SubString getTextFromConstraint(const VirtualAddressConstraint& constraint)
      {  if (constraint.isRegister())
            return STG::SString("register");
         else if (constraint.isIndirect())
            return STG::SString("indirect");
         return STG::SString();
      }

  public:
   MemoryStateConstraint() = default;
   MemoryStateConstraint(const MemoryStateConstraint&) = default;

   ReadResult readJSon(STG::JSon::CommonParser::State& state, STG::JSon::CommonParser::Arguments& arguments);
   WriteResult writeJSon(STG::JSon::CommonWriter::State& state, STG::JSon::CommonWriter::Arguments& arguments) const;
};

