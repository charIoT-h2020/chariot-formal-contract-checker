#pragma once

#include "dll.h"
#include "decsec_callback.h"
#include "DomainValue.h"
#include "Contract.h"

class MemoryInterpretParameters {

};

class Processor;
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

   void setFromContract(const Contract& contract);
   MemoryModelFunctions* getFunctions() const { return &functions; }
   void write(std::ostream& out) const { out << "end of memory description\n"; }
   const struct _DomainElementFunctions* getDomainFunctions() const { return domainFunctions; }
   // [TODO] to implement
   bool contain(const MemoryState& source, const MemoryInterpretParameters& parameters) { return true; } 
};

