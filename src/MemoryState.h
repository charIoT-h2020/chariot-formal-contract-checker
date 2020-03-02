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

   virtual Type getType() const override { return TLocal; }
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
   class DomainValueZone : public DomainValue {
     private:
      PNT::TSharedPointer<MemoryZone> spmzZone;

     protected:
      virtual ComparisonResult _compare(const EnhancedObject& asource) const
         {  ComparisonResult result = CREqual;
            const DomainValueZone& source = static_cast<const DomainValueZone&>(castFromCopyHandler(asource));
            if (!spmzZone.isValid())
               result = !source.spmzZone.isValid() ? CREqual : CRLess;
            else if (!source.spmzZone.isValid())
               result = CRGreater;
            else if (spmzZone.key() == source.spmzZone.key())
               result = DomainValue::_compare(source);
            else
               result = fcompare(spmzZone->getId(), source.spmzZone->getId());
            return result;
         }

     public:
      DomainValueZone(DomainValue&& value, const PNT::TSharedPointer<MemoryZone>& zone)
         :  DomainValue(value), spmzZone(zone) {}
      DomainValueZone(DomainValueZone&& source) = default;
      DomainValueZone(const DomainValueZone& source) = default;
      DomainValueZone& operator=(DomainValueZone&& source) = default;
      DomainValueZone& operator=(const DomainValueZone& source) = default;

      void mergeWith(DomainValueZone& source)
         {  if (spmzZone.key() != source.spmzZone.key())
               spmzZone = PNT::TSharedPointer<MemoryZone>();
            DomainEvaluationEnvironment env{};
            env.defaultDomainType = DISFormal;
            DomainValue::mergeWith(source, env);
         }
      bool contain(const DomainValueZone& source) const
         {  // if (spmzZone.isValid() && spmzZone.key() != source.spmzZone.key())
            //    return false;
            DomainEvaluationEnvironment env{};
            env.defaultDomainType = DISFormal;
            return DomainValue::contain(source, env);
         }
      const PNT::TSharedPointer<MemoryZone>& getZone() const { return spmzZone; }
      void setFrom(DomainValue&& value, const PNT::TSharedPointer<MemoryZone>& zone)
         {  DomainValue::operator=(std::move(value)); spmzZone = zone; }
   };
   class RegisterValue : public COL::GenericAVL::Node {
     private:
      typedef COL::GenericAVL::Node inherited;
      int uRegister;
      DomainValueZone dvzValue;

     public:
      RegisterValue(int aregister, DomainValue&& value,
            const PNT::TSharedPointer<MemoryZone>& zone)
         :  uRegister(aregister), dvzValue(std::move(value), zone) {}
      RegisterValue(int aregister, DomainValueZone&& value)
         :  uRegister(aregister), dvzValue(std::move(value)) {}
      RegisterValue(const RegisterValue& source) = default;
      DefineCopy(RegisterValue)

      const int& getRegister() const { return uRegister; }
      void setValue(DomainValue&& value, const PNT::TSharedPointer<MemoryZone>& zone)
         {  dvzValue.setFrom(std::move(value), zone); }
      void setValue(DomainValueZone&& value)
         {  dvzValue = std::move(value); }
      const DomainValue& getValue() const { return dvzValue; }
      void mergeWith(RegisterValue& source)
         {  AssumeCondition(uRegister == source.uRegister)
            dvzValue.mergeWith(source.dvzValue);
         }
      bool contain(const RegisterValue& source) const
         {  AssumeCondition(uRegister == source.uRegister)
            return dvzValue.contain(source.dvzValue);
         }
      bool isTop() const { return dvzValue.isTop(); }
      class Key {
        public:
         Key(const COL::VirtualCollection&) {}
         typedef const int& KeyType;
         typedef int ControlKeyType;
         static KeyType key(const RegisterValue& element) { return element.getRegister(); }
         static ComparisonResult compare(KeyType fst, KeyType snd) { return fcompare(fst, snd); }
      };
   };
   class MemoryValue : public COL::GenericAVL::Node {
     private:
      typedef COL::GenericAVL::Node inherited;
      DomainValueZone dvzAddress;
      DomainValueZone dvzValue;

     public:
      MemoryValue(DomainValue&& address, const PNT::TSharedPointer<MemoryZone>& zoneAddress,
            DomainValue&& value, const PNT::TSharedPointer<MemoryZone>& zoneValue)
         :  dvzAddress(std::move(address), zoneAddress), dvzValue(std::move(value), zoneValue) {}
      MemoryValue(DomainValueZone&& address, DomainValueZone&& value)
         :  dvzAddress(std::move(address)), dvzValue(std::move(value)) {}
      MemoryValue(const MemoryValue& source) = default;
      DefineCopy(MemoryValue)

      const DomainValueZone& getAddress() const { return dvzAddress; }
      void setValue(DomainValue&& value, const PNT::TSharedPointer<MemoryZone>& zone)
         {  dvzValue.setFrom(std::move(value), zone); }
      void setValue(DomainValueZone&& value)
         {  dvzValue = std::move(value); }
      const DomainValueZone& getValue() const { return dvzValue; }
      void mergeWith(MemoryValue& source)
         {  dvzValue.mergeWith(source.dvzValue);
         }
      bool contain(const MemoryValue& source) const
         {  return dvzValue.contain(source.dvzValue);
         }
      bool isTop() const { return dvzValue.isTop(); }
      class Key {
        public:
         Key(const COL::VirtualCollection&) {}
         typedef const DomainValue& KeyType;
         typedef DomainValue ControlKeyType;
         static KeyType key(const MemoryValue& element) { return element.getAddress(); }
         static ComparisonResult compare(KeyType fst, KeyType snd) { return fst.compare(snd); }
      };
   };

   int uRegisterNumber = 0;
   typedef COL::TCopyCollection<COL::TSortedAVL<RegisterValue, RegisterValue::Key> > RegisterContent;
   typedef COL::TCopyCollection<COL::TSortedAVL<MemoryValue, MemoryValue::Key> > MemoryContent;
   RegisterContent rcRegisters;
   MemoryContent mcMemory;
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
   static MemoryModel* clone(MemoryModel* amemory)
      {  MemoryState* memory = reinterpret_cast<MemoryState*>(amemory);
         return reinterpret_cast<MemoryModel*>(new MemoryState(*memory));
      }
   static void assign(MemoryModel* afirst, MemoryModel* asecond)
      {  MemoryState* first = reinterpret_cast<MemoryState*>(afirst);
         MemoryState* second = reinterpret_cast<MemoryState*>(asecond);
         *first = *second;
      }
   static void swap(MemoryModel* afirst, MemoryModel* asecond)
      {  MemoryState* first = reinterpret_cast<MemoryState*>(afirst);
         MemoryState* second = reinterpret_cast<MemoryState*>(asecond);
         first->swap(*second);
      }
   static void free(MemoryModel* amemory)
      {  MemoryState* memory = reinterpret_cast<MemoryState*>(amemory);
         delete memory;
      }
   static void merge(MemoryModel* afirst, MemoryModel* asecond)
      {  MemoryState* first = reinterpret_cast<MemoryState*>(afirst);
         MemoryState* second = reinterpret_cast<MemoryState*>(asecond);
         first->mergeWith(*second);
      }
   // [TODO] augment DomainElement with a zone
   static void set_register_value(MemoryModel* amemory, int registerIndex,
         DomainElement* avalue, InterpretParameters* parameters,
         unsigned* error /* set of MemoryEvaluationErrorFlags */)
      {  MemoryState* memory = reinterpret_cast<MemoryState*>(amemory);
         RegisterContent::Cursor cursor(memory->rcRegisters);
         auto locationResult = memory->rcRegisters.locateKey(registerIndex, cursor);
         DomainValueZone value(DomainValue(std::move(*avalue), memory->domainFunctions), PNT::TSharedPointer<MemoryZone>());
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
         MemoryContent::Cursor cursor(memory->mcMemory);
         DomainValue result(memory->domainFunctions);
         if (memory->mcMemory.locateKey(address, cursor, COL::VirtualCollection::RPExact))
            result = cursor.elementAt().getValue();
         else
            result = DomainValue((*memory->domainFunctions->multibit_create_top)(size, true /* isSymbolic */), memory->domainFunctions);
         if (elementFunctions)
            *elementFunctions = memory->domainFunctions;
         return result.extractElement();
      }
   static DomainElement load_multibit_disjunctive_value(MemoryModel* amemory,
         DomainElement indirect_address, size_t size, InterpretParameters* parameters,
         unsigned* error, DomainElementFunctions** elementFunctions)
      {  MemoryState* memory = reinterpret_cast<MemoryState*>(amemory);
         DomainValue address((*memory->domainFunctions->clone)(indirect_address), memory->domainFunctions);
         MemoryContent::Cursor cursor(memory->mcMemory);
         DomainValue result(memory->domainFunctions);
         if (memory->mcMemory.locateKey(address, cursor, COL::VirtualCollection::RPExact))
            result = cursor.elementAt().getValue();
         else
            result = DomainValue((*memory->domainFunctions->multibit_create_top)(size, true /* isSymbolic */), memory->domainFunctions);
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
         DomainValueZone value(DomainValue((*memory->domainFunctions->clone)(avalue), memory->domainFunctions), PNT::TSharedPointer<MemoryZone>());
         DomainValueZone address(DomainValue((*memory->domainFunctions->clone)(indirect_address), memory->domainFunctions), PNT::TSharedPointer<MemoryZone>());
         MemoryContent::Cursor cursor(memory->mcMemory);
         auto localize = memory->mcMemory.locateKey(address, cursor);
         if (localize)
            cursor.elementSAt().setValue(std::move(value));
         else
            memory->mcMemory.add(new MemoryValue(std::move(address), std::move(value)),
                  localize.queryInsertionParameters().setFreeOnError(), &cursor);
      }

  public:
   MemoryState(int registerNumber, struct _DomainElementFunctions* adomainFunctions)
      :  uRegisterNumber(registerNumber), domainFunctions(adomainFunctions) {}
   MemoryState(MemoryState&&) = default;
   MemoryState(const MemoryState&) = default;
   MemoryState& operator=(MemoryState&&) = default;
   MemoryState& operator=(const MemoryState&) = default;

   void swap(MemoryState& source)
      {  AssumeCondition(uRegisterNumber == source.uRegisterNumber && domainFunctions == source.domainFunctions
               && sphImplicitHypotheses.isValid() == source.sphImplicitHypotheses.isValid()
               && (!sphImplicitHypotheses.isValid() || sphImplicitHypotheses.key() == source.sphImplicitHypotheses.key()))
         rcRegisters.swap(source.rcRegisters);
         mcMemory.swap(source.mcMemory);
         mzMemoryZones.swap(source.mzMemoryZones);
      }
   void mergeWith(MemoryState& source)
      {  AssumeCondition(uRegisterNumber == source.uRegisterNumber && domainFunctions == source.domainFunctions
               && sphImplicitHypotheses.isValid() == source.sphImplicitHypotheses.isValid()
               && (!sphImplicitHypotheses.isValid() || sphImplicitHypotheses.key() == source.sphImplicitHypotheses.key()))
         mzMemoryZones.mergeWith(source.mzMemoryZones);
         {  RegisterContent::Cursor thisCursor(rcRegisters), sourceCursor(source.rcRegisters);
            sourceCursor.setToFirst();
            while (thisCursor.setToNext()) {
               if (!sourceCursor.isValid()) {
                  auto copyCursor = thisCursor;
                  thisCursor.setToPrevious();
                  rcRegisters.freeAt(copyCursor);
               }
               else {
                  ComparisonResult compare = RegisterValue::Key::compare(
                        RegisterValue::Key::key(thisCursor.elementAt()),
                        RegisterValue::Key::key(sourceCursor.elementAt()));
                  if (compare == CRLess) {
                     auto copyCursor = thisCursor;
                     thisCursor.setToPrevious();
                     rcRegisters.freeAt(copyCursor);
                  }
                  else if (compare == CRGreater) {
                     thisCursor.setToPrevious();
                     sourceCursor.setToNext();
                  }
                  else {
                     thisCursor.elementSAt().mergeWith(sourceCursor.elementSAt());
                     sourceCursor.setToNext();
                  }
               };
            };
         }
         {  MemoryContent::Cursor thisCursor(mcMemory), sourceCursor(source.mcMemory);
            sourceCursor.setToFirst();
            while (thisCursor.setToNext()) {
               if (!sourceCursor.isValid()) {
                  auto copyCursor = thisCursor;
                  thisCursor.setToPrevious();
                  mcMemory.freeAt(copyCursor);
               }
               else {
                  ComparisonResult compare = MemoryValue::Key::compare(
                        MemoryValue::Key::key(thisCursor.elementAt()),
                        MemoryValue::Key::key(sourceCursor.elementAt()));
                  if (compare == CRLess) {
                     auto copyCursor = thisCursor;
                     thisCursor.setToPrevious();
                     mcMemory.freeAt(copyCursor);
                  }
                  else if (compare == CRGreater) {
                     thisCursor.setToPrevious();
                     sourceCursor.setToNext();
                  }
                  else {
                     thisCursor.elementSAt().mergeWith(sourceCursor.elementSAt());
                     sourceCursor.setToNext();
                  }
               };
            };
         }
      }

   DomainValue evaluate(const VirtualExpressionNode& aexpression, struct _Processor* processor,
         struct _ProcessorFunctions* processorFunctions)
      {  switch (aexpression.getTypeExpression()) {
            case VirtualExpressionNode::TERegisterAccess:
               {  const auto& expression = static_cast<const RegisterAccessNode&>(aexpression);
                  int registerIndex = (*processorFunctions->get_register_index)
                     (processor, expression.getName().getChunk().string);
                  RegisterContent::Cursor cursor(rcRegisters);
                  if (rcRegisters.locateKey(registerIndex, cursor, COL::VirtualCollection::RPExact))
                     return cursor.elementAt().getValue();
                  return DomainValue(domainFunctions);
               }
            case VirtualExpressionNode::TEIndirection:
               {  const auto& expression = static_cast<const IndirectionNode&>(aexpression);
                  // [TODO] do it symbolically
                  DomainValue address = evaluate(expression.getAddress(), processor, processorFunctions);
                  MemoryContent::Cursor cursor(mcMemory);
                  if (mcMemory.locateKey(address, cursor, COL::VirtualCollection::RPExact))
                     return cursor.elementAt().getValue();
                  return DomainValue(domainFunctions);
               }
            case VirtualExpressionNode::TEDomain:
               {  const auto& expression = static_cast<const DomainNode&>(aexpression);
                  return expression.getValue();
               }
            case VirtualExpressionNode::TEOperation:
               {  const auto& expression = static_cast<const OperationNode&>(aexpression);
                  DomainValue first = evaluate(expression.getFirst(), processor, processorFunctions);
                  DomainValue second = DomainValue(domainFunctions);
                  if (expression.isBinary())
                     second = evaluate(expression.getSecond(), processor, processorFunctions);
                  expression.applyOperation(first, second);
                  return first;
               }
            default:
               break;
         };
         return DomainValue(DomainElement{}, domainFunctions);
      }
   MemoryZones& memoryZones() { return mzMemoryZones; }
   // void intersectWith(const VirtualAddressConstraint& contract);
   MemoryModelFunctions* getFunctions() const { return &functions; }
   void write(std::ostream& out) const { out << "end of memory description\n"; }
   const struct _DomainElementFunctions* getDomainFunctions() const { return domainFunctions; }
   bool contain(const MemoryState& source, const MemoryInterpretParameters& parameters)
      {  AssumeCondition(uRegisterNumber == source.uRegisterNumber && domainFunctions == source.domainFunctions
               && sphImplicitHypotheses.isValid() == source.sphImplicitHypotheses.isValid()
               && (!sphImplicitHypotheses.isValid() || sphImplicitHypotheses.key() == source.sphImplicitHypotheses.key()))
         {  RegisterContent::Cursor thisCursor(rcRegisters), sourceCursor(source.rcRegisters);
            sourceCursor.setToFirst();
            while (thisCursor.setToNext()) {
               if (!sourceCursor.isValid()) {
                  if (!thisCursor.elementAt().isTop())
                     return false;
               }
               else {
                  ComparisonResult compare = RegisterValue::Key::compare(
                        RegisterValue::Key::key(thisCursor.elementAt()),
                        RegisterValue::Key::key(sourceCursor.elementAt()));
                  if (compare == CRLess) {
                     if (!thisCursor.elementAt().isTop())
                        return false;
                  }
                  else if (compare == CRGreater) {
                     thisCursor.setToPrevious();
                     sourceCursor.setToNext();
                  }
                  else {
                     if (!thisCursor.elementSAt().contain(sourceCursor.elementSAt()))
                        return false;
                     sourceCursor.setToNext();
                  }
               };
            };
         }
         {  MemoryContent::Cursor thisCursor(mcMemory), sourceCursor(source.mcMemory);
            sourceCursor.setToFirst();
            while (thisCursor.setToNext()) {
               if (!sourceCursor.isValid()) {
                  if (!thisCursor.elementAt().isTop())
                     return false;
               }
               else {
                  ComparisonResult compare = MemoryValue::Key::compare(
                        MemoryValue::Key::key(thisCursor.elementAt()),
                        MemoryValue::Key::key(sourceCursor.elementAt()));
                  if (compare == CRLess) {
                     if (!thisCursor.elementAt().isTop())
                        return false;
                  }
                  else if (compare == CRGreater) {
                     thisCursor.setToPrevious();
                     sourceCursor.setToNext();
                  }
                  else {
                     if (!thisCursor.elementSAt().contain(sourceCursor.elementSAt()))
                        return false;
                     sourceCursor.setToNext();
                  }
               };
            };
         }
         return true;
      }
   void intersectRegister(int registerIndex, DomainValue&& avalue)
      {  RegisterContent::Cursor cursor(rcRegisters);
         auto locationResult = rcRegisters.locateKey(registerIndex, cursor);
         DomainValueZone value(std::move(avalue), PNT::TSharedPointer<MemoryZone>());
         if (locationResult)
            cursor.elementSAt().setValue(std::move(value));
         else
            rcRegisters.add(new RegisterValue(registerIndex, std::move(value)),
                  locationResult.queryInsertionParameters().setFreeOnError(), &cursor);
      }
   void intersectMemory(DomainValue&& aaddress, DomainValue&& avalue)
      {  DomainValueZone value(std::move(avalue), PNT::TSharedPointer<MemoryZone>());
         DomainValueZone address(std::move(aaddress), PNT::TSharedPointer<MemoryZone>());
         MemoryContent::Cursor cursor(mcMemory);
         auto localize = mcMemory.locateKey(address, cursor);
         if (localize)
            cursor.elementSAt().setValue(std::move(value));
         else
            mcMemory.add(new MemoryValue(std::move(address), std::move(value)),
                  localize.queryInsertionParameters().setFreeOnError(), &cursor);
      }
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

   const Expression& getConstraint() const { return eConstraint; }
   DomainValue evaluateInMemory(MemoryState& memoryState, const Expression& expression,
         struct _Processor* processor, struct _ProcessorFunctions* processorFunctions)
      {  return memoryState.evaluate(expression.getContent(), processor, processorFunctions); }

  public:
   VirtualAddressConstraint() = default;
   VirtualAddressConstraint(const VirtualAddressConstraint&) = default;
   DefineCopy(VirtualAddressConstraint)
   StaticInheritConversions(VirtualAddressConstraint, inherited)

   virtual bool apply(MemoryState& memoryState, uint64_t startAddress,
         struct _Processor* processor, struct _ProcessorFunctions* processorFunctions) { return true; }
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

   virtual bool apply(MemoryState& memoryState, uint64_t startAddress,
         struct _Processor* processor, struct _ProcessorFunctions* processorFunctions) override
      {  memoryState.intersectRegister(uRegisterIndex,
            evaluateInMemory(memoryState, getConstraint(), processor, processorFunctions));
         return true;
      }
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

   virtual bool apply(MemoryState& memoryState, uint64_t startAddress,
         struct _Processor* processor, struct _ProcessorFunctions* processorFunctions) override
      {  memoryState.intersectMemory(
            evaluateInMemory(memoryState, eAddress, processor, processorFunctions),
            evaluateInMemory(memoryState, getConstraint(), processor, processorFunctions));
         return true;
      }
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
      ReadRuleResult& operator=(const ReadRuleResult&) = default;
   };

   struct WriteRuleResult : public VirtualAddressConstraint::WriteRuleResult {
      PNT::PassPointer<COL::TArray<VirtualAddressConstraint>::Cursor> writeCursor;
      WriteRuleResult() = default;
      WriteRuleResult(struct _Processor* aprocessor, struct _ProcessorFunctions* aprocessorFunctions)
         :  VirtualAddressConstraint::WriteRuleResult(aprocessor, aprocessorFunctions) {}
      WriteRuleResult(const WriteRuleResult&) = default;
      WriteRuleResult& operator=(const WriteRuleResult&) = default;
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
               break;
            case TCIndirect:
               result.absorbElement(new IndirectAddressConstraint());
               break;
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

   void apply(MemoryState& memoryState, uint64_t startAddress, struct _Processor* processor,
         struct _ProcessorFunctions* processorFunctions)
      {  Cursor cursor(*this);
         while (cursor.setToNext())
            cursor.elementSAt().apply(memoryState, startAddress, processor, processorFunctions);
      }
   ReadResult readJSon(STG::JSon::CommonParser::State& state, STG::JSon::CommonParser::Arguments& arguments);
   WriteResult writeJSon(STG::JSon::CommonWriter::State& state, STG::JSon::CommonWriter::Arguments& arguments) const;
};

