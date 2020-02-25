#pragma once

#include "Contract.h"
#include "dll.h"
#include <vector>
#include "decsec_callback.h"

class Processor {
  private:
   DLL::Library dlProcessorLibrary;
   DLL::Library dlDomainLibrary;
   struct _Processor* pvContent;
   struct _ProcessorFunctions architectureFunctions;
   // struct _DomainElementFunctions domainFunctions;
   std::ifstream fBinaryFile;

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
      :  pvContent(nullptr), architectureFunctions{} {}
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
   void initializeMemory(MemoryState& memory, MemoryInterpretParameters& parameters)
      {  AssumeCondition(pvContent)
         (*architectureFunctions.initialize_memory)(pvContent, reinterpret_cast<MemoryModel*>(&memory),
            &MemoryState::functions, reinterpret_cast<InterpretParameters*>(&parameters));
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
         TargetAddresses& targetAddresses, MemoryInterpretParameters& parameters);
   void interpret(uint64_t address, MemoryState& memoryState,
         uint64_t targetAddress, Warnings& warnings, MemoryInterpretParameters& parameters);
};

