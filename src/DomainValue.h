#pragma once

#include "dll.h"
#include "domsec_callback.h"
#include "StandardClasses/Persistence.h"

class MemoryState;
class DomainValue : public STG::IOObject {
  private:
   DomainElement deValue;
   struct _DomainElementFunctions* pfFunctions;

  protected:
   friend class MemoryState;

   static char* increase_buffer_size(char* buffer, int old_length, int new_length, void* awriter)
      {  STG::SString* writer = reinterpret_cast<STG::SString*>(awriter);
         AssumeCondition(writer->length() == old_length)
         writer->cat('\0', new_length-old_length);
         return writer->getChunk().string;
      }
   virtual void _write(STG::IOObject::OSBase& out, const STG::IOObject::FormatParameters& params) const override
      {  if (deValue.content && pfFunctions) {
            STG::SString buffer;
            buffer.cat('\0', 40);
            int buffer_size = 40;
            int length = 0;
            (*pfFunctions->write)(deValue, buffer.getChunk().string, buffer_size,
                  &length, &buffer, &increase_buffer_size);
            out.writeall(STG::SubString(buffer, 0, length).asPersistent());
         }
         else
            out << "...";
      }
   virtual ComparisonResult _compare(const EnhancedObject& asource) const override
      {  ComparisonResult result = STG::IOObject::_compare(asource);
         if (result == CREqual) {
            const auto& source = static_cast<const DomainValue&>(asource);
            if (!pfFunctions)
               return !source.pfFunctions ? CREqual : CRLess;
            if (!source.pfFunctions)
               return CRGreater;
            int res = (*pfFunctions->compare)(deValue, source.deValue);
            AssumeCondition(-1 <= res && res <= 2)
            result = (ComparisonResult) res;
         }
         return result;
      }

  public:
   DomainElement& svalue() { return deValue; }
   const DomainElement& value() const { return deValue; }
   bool hasFunctionTable() const { return pfFunctions; }
   struct _DomainElementFunctions& functionTable() const { AssumeCondition(pfFunctions) return *pfFunctions; }

  public:
   DomainValue(struct _DomainElementFunctions* functions) : deValue{}, pfFunctions(functions) {}
   DomainValue(DomainElement&& value, struct _DomainElementFunctions* functions)
      :  deValue(std::move(value)), pfFunctions(functions) {}
   DomainValue(DomainValue&& source)
      :  deValue(source.deValue), pfFunctions(source.pfFunctions)
      {  source.deValue.content = nullptr; }
   DomainValue(const DomainValue& source)
      :  deValue{ nullptr }, pfFunctions(source.pfFunctions)
      {  if (source.deValue.content) {
            AssumeCondition(pfFunctions)
            deValue = (*pfFunctions->clone)(source.deValue);
         }
      }
   DomainValue& operator=(DomainValue&& source)
      {  if (this == &source)
            return *this;
         if (deValue.content)
         {
            AssumeCondition(pfFunctions)
            (*pfFunctions->free)(&deValue);
         }
         pfFunctions = source.pfFunctions;
         deValue = source.deValue;
         source.deValue.content = nullptr;
         return *this;
      }
   DomainValue& operator=(const DomainValue& source)
      {  if (this == &source)
            return *this;
         if (deValue.content)
            (*pfFunctions->free)(&deValue);
         pfFunctions = source.pfFunctions;
         if (source.deValue.content)
         {
            AssumeCondition(pfFunctions)
            deValue = (*pfFunctions->clone)(source.deValue);
         }
         return *this;
      }
   ~DomainValue()
      {  if (deValue.content && pfFunctions) (*pfFunctions->free)(&deValue); }
   DefineCopy(DomainValue)
   DDefineAssign(DomainValue)

   void applyAssign(DomainBitUnaryOperation operation, DomainEvaluationEnvironment& env)
      {  bool result = (*pfFunctions->bit_unary_apply_assign)(&deValue, operation, &env);
         AssumeCondition(result)
      }
   void applyAssign(DomainBitBinaryOperation operation, const DomainValue& source,
         DomainEvaluationEnvironment& env)
      {  bool result = (*pfFunctions->bit_binary_apply_assign)(&deValue, operation, source.deValue, &env);
         AssumeCondition(result)
      }
   DomainValue applyCompare(DomainBitCompareOperation operation, const DomainValue& source,
         DomainEvaluationEnvironment& env) const
      {  return DomainValue((*pfFunctions->bit_binary_compare_domain)
            (deValue, operation, source.deValue, &env), pfFunctions);
      }
   DomainValue applyBitToInteger(int sizeInBits, DomainEvaluationEnvironment& env) const
      {  return DomainValue((*pfFunctions->bit_create_cast_multibit)
            (deValue, sizeInBits, &env), pfFunctions);
      }

   void applyAssign(DomainMultiBitUnaryOperation operation, DomainEvaluationEnvironment& env)
      {  bool result = (*pfFunctions->multibit_unary_apply_assign)(&deValue, operation, &env);
         AssumeCondition(result)
      }
   void applyAssign(DomainMultiBitExtendOperation operation, DomainEvaluationEnvironment& env)
      {  bool result = (*pfFunctions->multibit_extend_apply_assign)(&deValue, operation, &env);
         AssumeCondition(result)
      }
   void applyAssign(DomainMultiBitReduceOperation operation, DomainEvaluationEnvironment& env)
      {  bool result = (*pfFunctions->multibit_reduce_apply_assign)(&deValue, operation, &env);
         AssumeCondition(result)
      }
   void applyAssign(DomainMultiBitBinaryOperation operation, const DomainValue& source,
         DomainEvaluationEnvironment& env)
      {  bool result = (*pfFunctions->multibit_binary_apply_assign)(&deValue, operation, source.deValue, &env);
         AssumeCondition(result)
      }
   void applyAssign(DomainMultiBitSetOperation operation, const DomainValue& source,
         DomainEvaluationEnvironment& env)
      {  bool result = (*pfFunctions->multibit_bitset_apply_assign)(&deValue, operation, source.deValue, &env);
         AssumeCondition(result)
      }
   DomainValue applyCompare(DomainMultiBitCompareOperation operation, const DomainValue& source,
         DomainEvaluationEnvironment& env) const
      {  return DomainValue((*pfFunctions->multibit_binary_compare_domain)
            (deValue, operation, source.deValue, &env), pfFunctions);
      }
   DomainValue applyIntegerToBit(DomainEvaluationEnvironment& env) const
      {  return DomainValue((*pfFunctions->multibit_create_cast_bit)
            (deValue, &env), pfFunctions);
      }
   DomainValue applyIntegerToShiftBit(int shift, DomainEvaluationEnvironment& env) const
      {  return DomainValue((*pfFunctions->multibit_create_cast_shift_bit)
            (deValue, shift, &env), pfFunctions);
      }
   DomainValue applyIntegerToInteger(int sizeInBits, bool isSigned,
         DomainEvaluationEnvironment& env) const
      {  return DomainValue((*pfFunctions->multibit_create_cast_multibit)
            (deValue, sizeInBits, isSigned, &env), pfFunctions);
      }
   DomainValue applyIntegerToFloating(int sizeInBits, bool isSigned,
         DomainEvaluationEnvironment& env) const
      {  return DomainValue((*pfFunctions->multibit_create_cast_multifloat)
            (deValue, sizeInBits, isSigned, &env), pfFunctions);
      }
   DomainValue applyIntegerToFloatingPtr(int sizeInBits, bool isSigned,
         DomainEvaluationEnvironment& env) const
      {  return DomainValue((*pfFunctions->multibit_create_cast_multifloat_ptr)
            (deValue, sizeInBits, isSigned, &env), pfFunctions);
      }

   void applyAssign(DomainMultiFloatUnaryOperation operation, DomainEvaluationEnvironment& env)
      {  bool result = (*pfFunctions->multifloat_unary_apply_assign)(&deValue, operation, &env);
         AssumeCondition(result)
      }
   void applyAssign(DomainMultiFloatBinaryOperation operation, const DomainValue& source,
         DomainEvaluationEnvironment& env)
      {  bool result = (*pfFunctions->multifloat_binary_apply_assign)(&deValue, operation, source.deValue, &env);
         AssumeCondition(result)
      }
   DomainValue applyCompare(DomainMultiFloatCompareOperation operation, const DomainValue& source,
         DomainEvaluationEnvironment& env) const
      {  return DomainValue((*pfFunctions->multifloat_binary_compare_domain)
            (deValue, operation, source.deValue, &env), pfFunctions);
      }
   DomainValue applyFloatingToInteger(int sizeInBits, DomainEvaluationEnvironment& env) const
      {  return DomainValue((*pfFunctions->multifloat_create_cast_multibit)
            (deValue, sizeInBits, &env), pfFunctions);
      }
   void applyFloatingToFloating(int sizeInBits, DomainEvaluationEnvironment& env)
      {  bool result = (*pfFunctions->multifloat_cast_multifloat_assign)
               (&deValue, sizeInBits, &env);
         AssumeCondition(result)
      }

   void mergeWith(const DomainValue& source, DomainEvaluationEnvironment& env)
      {  bool result = (*pfFunctions->merge)(&deValue, source.deValue, &env);
         AssumeCondition(result)
      }
   void intersectWith(const DomainValue& source, DomainEvaluationEnvironment& env)
      {  bool result = (*pfFunctions->intersect)(&deValue, source.deValue, &env);
         AssumeCondition(result)
      }

   void clear()
      {  if (deValue.content)
         {  AssumeCondition(pfFunctions)
            (*pfFunctions->free)(&deValue);
         }
      }
   virtual bool isValid() const override { return deValue.content; }
   DomainType getType() const
      {  AssumeCondition(pfFunctions)
         return (*pfFunctions->get_type)(deValue);
      } 
   ZeroResult queryZeroResult() const
      {  AssumeCondition(pfFunctions)
         return (*pfFunctions->query_zero_result)(deValue);
      } 
   int getSizeInBits() const
      {  AssumeCondition(pfFunctions)
         return (*pfFunctions->get_size_in_bits)(deValue);
      } 
   DomainElement extractElement()
      {  auto res = deValue;
         deValue = DomainElement{};
         return res;
      }
};

