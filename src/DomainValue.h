#pragma once

#include "dll.h"
#include "domsec_callback.h"

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
            out.writeall(STG::SubString(buffer, length).asPersistent());
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

