#pragma once

#include "Contract.h"
#include "Dll/dll.h"
#include <vector>
#include "decsec_callback.h"

class DecisionVector {
  private:
   struct _DecisionVector* pvContent;
   struct _ProcessorFunctions* architectureFunctions;

  public:
   DecisionVector(struct _DecisionVector* content, struct _ProcessorFunctions* functions)
      : pvContent(content), architectureFunctions(functions) {}
   DecisionVector(DecisionVector&& source)
      : pvContent(source.pvContent), architectureFunctions(source.architectureFunctions)
      {  source.pvContent = nullptr; }
   DecisionVector(const DecisionVector& source)
      :  pvContent(nullptr), architectureFunctions(source.architectureFunctions)
      {  if (source.pvContent)
            pvContent = (*architectureFunctions->clone_decision_vector)(source.pvContent);
      }
   ~DecisionVector()
      {  if (pvContent) {
            (*architectureFunctions->free_decision_vector)(pvContent);
            pvContent = nullptr;
         }
      }
   void filter(uint64_t address)
      {  (*architectureFunctions->filter_decision_vector)(pvContent, address); }
   struct _DecisionVector* getContent() const { return pvContent; }
};

class Processor {
  private:
   DLL::Library dlProcessorLibrary;
   DLL::Library dlDomainLibrary;
   struct _Processor* pvContent;
   struct _ProcessorFunctions architectureFunctions;
   // struct _DomainElementFunctions domainFunctions;
   std::ifstream fBinaryFile;
   uint64_t uLoaderAllocShift = 0;

   static uint64_t* reallocAddresses(uint64_t* old_addresses, int old_size,
         int* new_size, void* address_container)
      {  auto* container = reinterpret_cast<std::vector<uint64_t>*>(address_container);
         AssumeCondition(container->size() == (size_t) old_size
               && old_addresses == &(*container)[0])
         if (old_size < 6)
            *new_size = 6;
         else
            *new_size = old_size*3/2;
         container->insert(container->end(), (*new_size-old_size), 0);
         return &(*container)[0];
      }

  public:
   Processor()
      :  pvContent(nullptr), architectureFunctions{} { debugPrint((STG::IOObject*) nullptr); }
   Processor(Processor&& source)
      :  dlProcessorLibrary(std::move(source.dlProcessorLibrary)),
         dlDomainLibrary(std::move(source.dlDomainLibrary)),
         pvContent(source.pvContent),
         architectureFunctions(source.architectureFunctions)/* ,
         domainFunctions(source.domainFunctions) */
      {  source.pvContent = nullptr;
         source.architectureFunctions = _ProcessorFunctions{};
         // source.domainFunctions = _DomainElementFunctions{};
      }
   ~Processor() { if (pvContent) { (*architectureFunctions.free_processor)(pvContent); pvContent = nullptr; } }

   struct _Processor* getContent() const { return pvContent; }
   struct _ProcessorFunctions& getArchitectureFunctions() { return architectureFunctions; }
   void setFromFile(const char* filename);
   void setDomainFunctionsFromFile(const char* domainFilename);
   std::ifstream& binaryFile() { return fBinaryFile; }
   void setLoaderAllocShift(uint64_t shift) { uLoaderAllocShift = shift; }
   void setVerbose() { (*architectureFunctions.set_verbose)(pvContent); }
   int getRegistersNumber() const
      {  AssumeCondition(pvContent)
         return (*architectureFunctions.get_registers_number)(pvContent);
      }
   void initializeMemory(MemoryState& memory, MemoryInterpretParameters& parameters)
      {  AssumeCondition(pvContent)
         (*architectureFunctions.initialize_memory)(pvContent, reinterpret_cast<MemoryModel*>(&memory),
            &MemoryState::functions, reinterpret_cast<InterpretParameters*>(&parameters));
      }
   DecisionVector createDecisionVector() const
      {  return DecisionVector((*architectureFunctions.create_decision_vector)(pvContent),
            &const_cast<struct _ProcessorFunctions&>(architectureFunctions));
      }

   // void setDomainFunctions(struct _DomainElementFunctions* functions)
   //    {  AssumeCondition(pvContent)
   //       (*architectureFunctions.set_domain_functions)(pvContent, functions);
   //    }
   struct _DomainElementFunctions* getDomainFunctions() const
      {  AssumeCondition(pvContent)
         return (*architectureFunctions.get_domain_functions)(pvContent);
      }
   //   {  return const_cast<struct _DomainElementFunctions*>(&domainFunctions); }
   bool retrieveNextTargets(uint64_t address, MemoryState& memoryState,
         TargetAddresses& targetAddresses, DecisionVector& decisionVector,
         MemoryInterpretParameters& parameters);
   void interpret(uint64_t address, MemoryState& memoryState,
         uint64_t targetAddress, DecisionVector& decisionVector, Warnings& warnings,
         MemoryInterpretParameters& parameters);
};

