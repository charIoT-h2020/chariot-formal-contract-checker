#include "Processor.h"
#include <unistd.h>
#include <iostream>

void
Processor::setFromFile(const char* filename) {
   AssumeCondition(!pvContent)
   dlProcessorLibrary.setFromFile(filename);
   if (!(bool) dlProcessorLibrary) {
      char cwd[1024];
      char* szcwd = getcwd(cwd, sizeof(cwd));
      std::cerr << "unable to find dynamic processor library " << filename
         << " in working directory " << (szcwd ? szcwd : "") << std::endl; 
      AssumeUncalled
   }
   uint64_t (*init_processor_functions)( struct _ProcessorFunctions* );
   dlProcessorLibrary.loadSymbol("init_processor_functions", &init_processor_functions );
   uint64_t chk = (*init_processor_functions)( &architectureFunctions );
   
   if (chk != 1) { struct Incompatible{}; throw Incompatible(); }
   pvContent = (*architectureFunctions.create_processor)();
}

void
Processor::setDomainFunctionsFromFile(const char* domainFilename) {
   AssumeCondition(pvContent)
   struct _DomainElementFunctions domainFunctions;
   dlDomainLibrary.setFromFile(domainFilename);
   if (!(bool) dlDomainLibrary) {
      char cwd[1024];
      char* szcwd = getcwd(cwd, sizeof(cwd));
      std::cerr << "unable to find dynamic domain library " << domainFilename
         << " in working directory " << (szcwd ? szcwd : "") << std::endl; 
      AssumeUncalled
   }
   dlDomainLibrary.loadSymbol("domain_get_type", &domainFunctions.get_type);
   dlDomainLibrary.loadSymbol("domain_query_zero_result", &domainFunctions.query_zero_result);
   dlDomainLibrary.loadSymbol("domain_get_size_in_bits", &domainFunctions.get_size_in_bits);
   dlDomainLibrary.loadSymbol("domain_is_top", &domainFunctions.is_top);
   dlDomainLibrary.loadSymbol("domain_free", &domainFunctions.free);
   dlDomainLibrary.loadSymbol("domain_clone", &domainFunctions.clone);

   dlDomainLibrary.loadSymbol("domain_bit_create_constant", &domainFunctions.bit_create_constant);
   dlDomainLibrary.loadSymbol("domain_bit_create_top", &domainFunctions.bit_create_top);
   dlDomainLibrary.loadSymbol("domain_bit_create_cast_multibit", &domainFunctions.bit_create_cast_multibit);
   dlDomainLibrary.loadSymbol("domain_bit_unary_apply_assign", &domainFunctions.bit_unary_apply_assign);
   dlDomainLibrary.loadSymbol("domain_bit_create_unary_apply", &domainFunctions.bit_create_unary_apply);
   dlDomainLibrary.loadSymbol("domain_bit_binary_apply_assign", &domainFunctions.bit_binary_apply_assign);
   dlDomainLibrary.loadSymbol("domain_bit_create_binary_apply", &domainFunctions.bit_create_binary_apply);
   dlDomainLibrary.loadSymbol("domain_bit_binary_compare", &domainFunctions.bit_binary_compare);
   dlDomainLibrary.loadSymbol("domain_bit_binary_compare_domain", &domainFunctions.bit_binary_compare_domain);
   dlDomainLibrary.loadSymbol("domain_bit_guard_assign", &domainFunctions.bit_guard_assign);
   dlDomainLibrary.loadSymbol("domain_bit_query_boolean", &domainFunctions.bit_query_boolean);
   dlDomainLibrary.loadSymbol("domain_bit_cast_multibit_constraint", &domainFunctions.bit_cast_multibit_constraint);
   dlDomainLibrary.loadSymbol("domain_bit_unary_constraint", &domainFunctions.bit_unary_constraint);
   dlDomainLibrary.loadSymbol("domain_bit_binary_constraint", &domainFunctions.bit_binary_constraint);
   dlDomainLibrary.loadSymbol("domain_bit_compare_constraint", &domainFunctions.bit_compare_constraint);
   dlDomainLibrary.loadSymbol("domain_bit_is_constant_value", &domainFunctions.bit_is_constant_value);

   dlDomainLibrary.loadSymbol("domain_multibit_create_constant", &domainFunctions.multibit_create_constant);
   dlDomainLibrary.loadSymbol("domain_multibit_create_top", &domainFunctions.multibit_create_top);
   dlDomainLibrary.loadSymbol("domain_multibit_create_interval_and_absorb", &domainFunctions.multibit_create_interval_and_absorb);
   dlDomainLibrary.loadSymbol("domain_multibit_create_cast_bit", &domainFunctions.multibit_create_cast_bit);
   dlDomainLibrary.loadSymbol("domain_multibit_create_cast_shift_bit", &domainFunctions.multibit_create_cast_shift_bit);
   dlDomainLibrary.loadSymbol("domain_multibit_create_cast_multibit", &domainFunctions.multibit_create_cast_multibit);
   dlDomainLibrary.loadSymbol("domain_multibit_create_cast_multifloat", &domainFunctions.multibit_create_cast_multifloat);
   dlDomainLibrary.loadSymbol("domain_multibit_create_cast_multifloat_ptr", &domainFunctions.multibit_create_cast_multifloat_ptr);
   dlDomainLibrary.loadSymbol("domain_multibit_unary_apply_assign", &domainFunctions.multibit_unary_apply_assign);
   dlDomainLibrary.loadSymbol("domain_multibit_create_unary_apply", &domainFunctions.multibit_create_unary_apply);
   dlDomainLibrary.loadSymbol("domain_multibit_extend_apply_assign", &domainFunctions.multibit_extend_apply_assign);
   dlDomainLibrary.loadSymbol("domain_multibit_create_extend_apply", &domainFunctions.multibit_create_extend_apply);
   dlDomainLibrary.loadSymbol("domain_multibit_reduce_apply_assign", &domainFunctions.multibit_reduce_apply_assign);
   dlDomainLibrary.loadSymbol("domain_multibit_create_reduce_apply", &domainFunctions.multibit_create_reduce_apply);
   dlDomainLibrary.loadSymbol("domain_multibit_bitset_apply_assign", &domainFunctions.multibit_bitset_apply_assign);
   dlDomainLibrary.loadSymbol("domain_multibit_create_bitset_apply", &domainFunctions.multibit_create_bitset_apply);
   dlDomainLibrary.loadSymbol("domain_multibit_binary_apply_assign", &domainFunctions.multibit_binary_apply_assign);
   dlDomainLibrary.loadSymbol("domain_multibit_create_binary_apply", &domainFunctions.multibit_create_binary_apply);
   dlDomainLibrary.loadSymbol("domain_multibit_binary_compare", &domainFunctions.multibit_binary_compare);
   dlDomainLibrary.loadSymbol("domain_multibit_binary_compare_domain", &domainFunctions.multibit_binary_compare_domain);
   dlDomainLibrary.loadSymbol("domain_multibit_guard_assign", &domainFunctions.multibit_guard_assign);
   dlDomainLibrary.loadSymbol("domain_multibit_query_boolean", &domainFunctions.multibit_query_boolean);
   dlDomainLibrary.loadSymbol("domain_multibit_cast_bit_constraint", &domainFunctions.multibit_cast_bit_constraint);
   dlDomainLibrary.loadSymbol("domain_multibit_cast_shift_bit_constraint", &domainFunctions.multibit_cast_shift_bit_constraint);
   dlDomainLibrary.loadSymbol("domain_multibit_cast_multifloat_constraint", &domainFunctions.multibit_cast_multifloat_constraint);
   dlDomainLibrary.loadSymbol("domain_multibit_cast_multifloat_ptr_constraint", &domainFunctions.multibit_cast_multifloat_ptr_constraint);
   dlDomainLibrary.loadSymbol("domain_multibit_unary_constraint", &domainFunctions.multibit_unary_constraint);
   dlDomainLibrary.loadSymbol("domain_multibit_extend_constraint", &domainFunctions.multibit_extend_constraint);
   dlDomainLibrary.loadSymbol("domain_multibit_reduce_constraint", &domainFunctions.multibit_reduce_constraint);
   dlDomainLibrary.loadSymbol("domain_multibit_bitset_constraint", &domainFunctions.multibit_bitset_constraint);
   dlDomainLibrary.loadSymbol("domain_multibit_binary_constraint", &domainFunctions.multibit_binary_constraint);
   dlDomainLibrary.loadSymbol("domain_multibit_compare_constraint", &domainFunctions.multibit_compare_constraint);
   dlDomainLibrary.loadSymbol("domain_multibit_is_constant_value", &domainFunctions.multibit_is_constant_value);
   dlDomainLibrary.loadSymbol("domain_multibit_is_constant_disjunction", &domainFunctions.multibit_is_constant_disjunction);
   dlDomainLibrary.loadSymbol("domain_multibit_retrieve_constant_values", &domainFunctions.multibit_retrieve_constant_values);

   dlDomainLibrary.loadSymbol("domain_multifloat_create_constant", &domainFunctions.multifloat_create_constant);
   dlDomainLibrary.loadSymbol("domain_multifloat_create_top", &domainFunctions.multifloat_create_top);
   dlDomainLibrary.loadSymbol("domain_multifloat_create_interval_and_absorb", &domainFunctions.multifloat_create_interval_and_absorb);
   dlDomainLibrary.loadSymbol("domain_multifloat_create_cast_multibit", &domainFunctions.multifloat_create_cast_multibit);
   dlDomainLibrary.loadSymbol("domain_multifloat_query_to_multibit", &domainFunctions.multifloat_query_to_multibit);
   dlDomainLibrary.loadSymbol("domain_multifloat_cast_multifloat_assign", &domainFunctions.multifloat_cast_multifloat_assign);
   dlDomainLibrary.loadSymbol("domain_multifloat_cast_multifloat", &domainFunctions.multifloat_cast_multifloat);
   dlDomainLibrary.loadSymbol("domain_multifloat_unary_apply_assign", &domainFunctions.multifloat_unary_apply_assign);
   dlDomainLibrary.loadSymbol("domain_multifloat_create_unary_apply", &domainFunctions.multifloat_create_unary_apply);
   dlDomainLibrary.loadSymbol("domain_multifloat_flush_to_zero", &domainFunctions.multifloat_flush_to_zero);
   dlDomainLibrary.loadSymbol("domain_multifloat_binary_apply_assign", &domainFunctions.multifloat_binary_apply_assign);
   dlDomainLibrary.loadSymbol("domain_multifloat_create_binary_apply", &domainFunctions.multifloat_create_binary_apply);
   dlDomainLibrary.loadSymbol("domain_multifloat_binary_compare", &domainFunctions.multifloat_binary_compare);
   dlDomainLibrary.loadSymbol("domain_multifloat_binary_compare_domain", &domainFunctions.multifloat_binary_compare_domain);
   dlDomainLibrary.loadSymbol("domain_multifloat_binary_full_compare_domain", &domainFunctions.multifloat_binary_full_compare_domain);
   dlDomainLibrary.loadSymbol("domain_multifloat_guard_assign", &domainFunctions.multifloat_guard_assign);
   dlDomainLibrary.loadSymbol("domain_multifloat_ternary_apply_assign", &domainFunctions.multifloat_ternary_apply_assign);
   dlDomainLibrary.loadSymbol("domain_multifloat_ternary_query", &domainFunctions.multifloat_ternary_query);
   dlDomainLibrary.loadSymbol("domain_multifloat_create_ternary_apply", &domainFunctions.multifloat_create_ternary_apply);
   dlDomainLibrary.loadSymbol("domain_multifloat_cast_multibit_constraint", &domainFunctions.multifloat_cast_multibit_constraint);
   dlDomainLibrary.loadSymbol("domain_multifloat_query_to_multibit_constraint", &domainFunctions.multifloat_query_to_multibit_constraint);
   dlDomainLibrary.loadSymbol("domain_multifloat_cast_multifloat_constraint", &domainFunctions.multifloat_cast_multifloat_constraint);
   dlDomainLibrary.loadSymbol("domain_multifloat_unary_constraint", &domainFunctions.multifloat_unary_constraint);
   dlDomainLibrary.loadSymbol("domain_multifloat_binary_constraint", &domainFunctions.multifloat_binary_constraint);
   dlDomainLibrary.loadSymbol("domain_multifloat_compare_constraint", &domainFunctions.multifloat_compare_constraint);
   dlDomainLibrary.loadSymbol("domain_multifloat_ternary_constraint", &domainFunctions.multifloat_ternary_constraint);
   dlDomainLibrary.loadSymbol("domain_multifloat_is_constant_value", &domainFunctions.multifloat_is_constant_value);
   dlDomainLibrary.loadSymbol("domain_write", &domainFunctions.write);
   dlDomainLibrary.loadSymbol("domain_merge", &domainFunctions.merge);
   dlDomainLibrary.loadSymbol("domain_intersect", &domainFunctions.intersect);
   dlDomainLibrary.loadSymbol("domain_contain", &domainFunctions.contain);
   dlDomainLibrary.loadSymbol("domain_compare", &domainFunctions.compare);
   dlDomainLibrary.loadSymbol("domain_create_disjunction_and_absorb", &domainFunctions.create_disjunction_and_absorb);
   dlDomainLibrary.loadSymbol("domain_disjunction_absorb", &domainFunctions.disjunction_absorb);
   dlDomainLibrary.loadSymbol("domain_specialize", &domainFunctions.specialize);
   (*architectureFunctions.set_domain_functions)(pvContent, &domainFunctions);
}

bool
Processor::retrieveNextTargets(uint64_t address, MemoryState& memoryState,
      TargetAddresses& targetAddresses, DecisionVector& decisionVector,
      MemoryInterpretParameters& parameters) {
   if ((uint64_t) fBinaryFile.tellg() != address-uLoaderAllocShift) {
      fBinaryFile.seekg(address-uLoaderAllocShift);
      if (!fBinaryFile.good())
         return false;
   }
   static const int BufferSize = 1000;
   char instructionBuffer[BufferSize];
   int length = fBinaryFile.readsome(instructionBuffer, BufferSize);
   if (length <= 0)
      return false;

   char* instruction = instructionBuffer;
   char* nextInstruction = instructionBuffer;

   std::vector<uint64_t> stopAddresses;
   stopAddresses.reserve(targetAddresses.addresses_length);
   if (targetAddresses.addresses_length >= 1) {
      for (int index = 0; index < targetAddresses.addresses_length; ++index) {
         stopAddresses.push_back(targetAddresses.addresses[index]);
         targetAddresses.addresses[index] = 0;
      }
      targetAddresses.addresses_length = 0;
   }
   while (length > 0) {
      bool isValid = (*architectureFunctions.processor_next_targets)(pvContent,
            nextInstruction, length, address, &targetAddresses,
            reinterpret_cast<MemoryModel*>(&memoryState), memoryState.getFunctions(),
            decisionVector.getContent(), reinterpret_cast<InterpretParameters*>(&parameters));
      AssumeCondition(isValid)
      if (targetAddresses.addresses_length == 1) {
         for (const auto& stopAddress : stopAddresses)
            if (targetAddresses.addresses[0] == stopAddress)
               return true;
         if (nextInstruction == instruction)
            nextInstruction += targetAddresses.addresses[0]-address;
         else
            {  AssumeCondition((int64_t) (nextInstruction-instruction)
                  == ((int64_t) (targetAddresses.addresses[0]-address)))
            }
         length -= (nextInstruction-instruction);
         address += (nextInstruction-instruction);
         if (length <= 20 || length > BufferSize) {
            if ((uint64_t) fBinaryFile.tellg() != address-uLoaderAllocShift) {
               fBinaryFile.seekg(address-uLoaderAllocShift);
               if (!fBinaryFile.good())
                  return false;
            }
            length = fBinaryFile.readsome(instructionBuffer, BufferSize);
            if (length <= 0)
               return false;
            instruction = nextInstruction = instructionBuffer;
         }
         else
            instruction = nextInstruction;
         targetAddresses.addresses_length = 0;
         targetAddresses.addresses[0] = 0;
      }
      else
         return true;
      // stop on calls, ...
   }
   return false;
}

void
Processor::interpret(uint64_t address, MemoryState& memoryState,
      uint64_t targetAddress, DecisionVector& decisionVector, Warnings& warnings,
      MemoryInterpretParameters& parameters) {
   if ((uint64_t) fBinaryFile.tellg() != address-uLoaderAllocShift) {
      fBinaryFile.seekg(address-uLoaderAllocShift);
      if (!fBinaryFile.good())
         return;
   }
   static const int BufferSize = 1000;
   char instructionBuffer[BufferSize];
   int length = fBinaryFile.readsome(instructionBuffer, BufferSize);
   if (length <= 0)
      return;

   char* instruction = instructionBuffer;
   decisionVector.filter(targetAddress);
   while (length > 0) {
      uint64_t old_address = address;
      bool hasFound = (*architectureFunctions.processor_interpret)(pvContent,
            instruction, length, &address, targetAddress,
            reinterpret_cast<MemoryModel*>(&memoryState), memoryState.getFunctions(),
            decisionVector.getContent(), reinterpret_cast<InterpretParameters*>(&parameters));
      if (hasFound)
         return;
      instruction += (address-old_address);
      AssumeCondition(instruction >= instructionBuffer && instruction < instructionBuffer+1000)
      length -= (address-old_address);
      if (length <= 20 || length > BufferSize) {
         if ((uint64_t) fBinaryFile.tellg() != address-uLoaderAllocShift) {
            fBinaryFile.seekg(address-uLoaderAllocShift);
            if (!fBinaryFile.good())
               return;
         }
         length = fBinaryFile.readsome(instructionBuffer, BufferSize);
         if (length <= 0)
            return;
         instruction = instructionBuffer;
      }
   }
}

