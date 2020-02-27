/////////////////////////////////
//
// Library   : Static Analysis
// Unit      : armsec
// File      : armsec_decoder.cpp
// Copyright : CEA LIST - 2020
//
// Description :
//   Implementation of a generic C Interface to manipulate contracts
//

#include "contract_checker.h"
#include "Processor.h"
#include <memory>
#include <cassert>
#include <vector>

extern "C" {

struct _PProcessor*
create_processor(const char* architectureLibrary, const char* domainLibrary)
{  std::unique_ptr<Processor> result(new Processor());
   result->setFromFile(architectureLibrary);
   result->setDomainFunctionsFromFile(domainLibrary);
   return reinterpret_cast<struct _PProcessor*>(result.release());
}

void
free_processor(struct _PProcessor* processor)
{  delete reinterpret_cast<Processor*>(processor);
}

void
processor_set_verbose(struct _PProcessor* processor)
{  reinterpret_cast<Processor*>(processor)->setVerbose();
}

bool
processor_load_code(struct _PProcessor* aprocessor, const char* filename)
{  Processor* processor = reinterpret_cast<Processor*>(aprocessor);
   processor->binaryFile().open(filename, std::ifstream::binary);
   if (!processor->binaryFile().good())
      return false;
   return true;
}

bool
processor_get_targets(struct _PProcessor* aprocessor, uint64_t address,
      struct _ContractContent* contract, TargetAddresses* target_addresses)
{  Processor& processor = *reinterpret_cast<Processor*>(aprocessor);
   Contract& startContract = *reinterpret_cast<Contract*>(contract);
   MemoryState memoryState(processor.getDomainFunctions());
   MemoryInterpretParameters parameters;
   // [TODO] remove it
   processor.initializeMemory(memoryState, parameters);
   startContract.applyTo(memoryState, processor.getContent(), &processor.getArchitectureFunctions());
   return processor.retrieveNextTargets(address, memoryState, *target_addresses, parameters);
}

bool processor_check_block(struct _PProcessor* aprocessor, uint64_t address,
      uint64_t target, struct _ContractContent* afirstContract,
      struct _ContractContent* alastContract, struct _ContractCoverageContent* acoverage,
      struct _WarningsContent* awarnings)
{  Processor& processor = *reinterpret_cast<Processor*>(aprocessor);
   Contract& firstContract = *reinterpret_cast<Contract*>(afirstContract);
   Contract& lastContract = *reinterpret_cast<Contract*>(alastContract);
   Warnings& warnings = *reinterpret_cast<Warnings*>(awarnings);
   MemoryState memoryState(processor.getDomainFunctions());
   MemoryInterpretParameters parameters;
   // [TODO] remove it
   processor.initializeMemory(memoryState, parameters);
   firstContract.applyTo(memoryState, processor.getContent(), &processor.getArchitectureFunctions());
   processor.interpret(address, memoryState, target, warnings, parameters);
   MemoryState lastMemoryState(processor.getDomainFunctions());
   processor.initializeMemory(lastMemoryState, parameters);
   lastContract.applyTo(lastMemoryState, processor.getContent(), &processor.getArchitectureFunctions());
   ContractCoverage& coverage = *reinterpret_cast<ContractCoverage*>(acoverage);
   coverage.add(firstContract, lastContract);
   return lastMemoryState.contain(memoryState, parameters);
}

struct _ContractGraphContent* load_contracts(const char* inputFilename,
      struct _PProcessor* aprocessor, struct _WarningsContent* awarnings)
{  Processor& processor = *reinterpret_cast<Processor*>(aprocessor);
   std::unique_ptr<ContractGraph> result(new ContractGraph());
   Warnings& warnings = *reinterpret_cast<Warnings*>(awarnings);
   result->loadFromFile(inputFilename, processor.getDomainFunctions(),
         processor.getContent(), &processor.getArchitectureFunctions(), warnings);
   return reinterpret_cast<struct _ContractGraphContent*>(result.release());
}

void free_contracts(struct _ContractGraphContent* acontracts)
{  delete reinterpret_cast<ContractGraph*>(acontracts); }

void contract_fill_stop_addresses(struct _ContractContent* acontract, TargetAddresses* stop_addresses)
{  Contract& contract = *reinterpret_cast<Contract*>(acontract);
   contract.retrieveNextAddresses(*stop_addresses);
}

struct _ContractCursorContent*
contract_cursor_new(struct _ContractGraphContent* agraph)
{  ContractGraph& contractGraph = *reinterpret_cast<ContractGraph*>(agraph);
   return reinterpret_cast<struct _ContractCursorContent*>(new ContractGraph::Cursor(contractGraph));
}

bool
contract_cursor_set_to_next(struct _ContractCursorContent* agraphCursor)
{  ContractGraph::Cursor& graphCursor = *reinterpret_cast<ContractGraph::Cursor*>(agraphCursor);
   return graphCursor.setToNext();
}

bool
contract_cursor_is_initial(struct _ContractCursorContent* agraphCursor)
{  ContractGraph::Cursor& graphCursor = *reinterpret_cast<ContractGraph::Cursor*>(agraphCursor);
   return graphCursor.elementAt()->isInitial();
}

bool
contract_cursor_is_final(struct _ContractCursorContent* agraphCursor)
{  ContractGraph::Cursor& graphCursor = *reinterpret_cast<ContractGraph::Cursor*>(agraphCursor);
   return graphCursor.elementAt()->isFinal();
}

bool
contract_cursor_set_address(struct _ContractCursorContent* agraphCursor, uint64_t address,
      enum ContractConditionLocalization localization)
{  ContractGraph::Cursor& graphCursor = *reinterpret_cast<ContractGraph::Cursor*>(agraphCursor);
   if (graphCursor.setToNext() && graphCursor.elementAt()->getAddress() == address)
      return true;

   const auto& support = (const ContractGraph&) graphCursor.getSupport();
   support.locateKey(address, graphCursor, (localization == CCLPreCondition)
         ? COL::VirtualCollection::RPBefore : ((localization == CCLPostCondition)
         ? COL::VirtualCollection::RPAfter : COL::VirtualCollection::RPUndefined));
   return graphCursor.isValid();
}

uint64_t
contract_cursor_get_address(struct _ContractCursorContent* agraphCursor)
{  ContractGraph::Cursor& graphCursor = *reinterpret_cast<ContractGraph::Cursor*>(agraphCursor);
   return graphCursor.elementAt()->getAddress();
}

struct _ContractCursorContent*
contract_cursor_clone(struct _ContractCursorContent* agraphCursor)
{  ContractGraph::Cursor& graphCursor = *reinterpret_cast<ContractGraph::Cursor*>(agraphCursor);
   return reinterpret_cast<struct _ContractCursorContent*>(new ContractGraph::Cursor(graphCursor));
}

void
contract_cursor_free(struct _ContractCursorContent* agraphCursor)
{  ContractGraph::Cursor* graphCursor = reinterpret_cast<ContractGraph::Cursor*>(agraphCursor);
   delete graphCursor;
}

struct _ContractContent*
contract_cursor_get_contract(struct _ContractCursorContent* agraphCursor)
{  ContractGraph::Cursor& graphCursor = *reinterpret_cast<ContractGraph::Cursor*>(agraphCursor);
   return reinterpret_cast<struct _ContractContent*>(&*graphCursor.elementSAt());
}

struct _ContractContent*
create_contract(const char* filename) /* content is possible */
{  std::unique_ptr<Contract> result(new Contract());
   STG::DIOObject::IFStream in(filename);
   result->read(in, Contract::FormatParameters());
   return reinterpret_cast<struct _ContractContent*>(result.release());
}

void
free_contract(struct _ContractContent* acontract)
{  Contract* contract = reinterpret_cast<Contract*>(acontract);
   delete contract;
}

uint64_t
contract_get_address(struct _ContractContent* acontract)
{  Contract& contract = *reinterpret_cast<Contract*>(acontract);
   return contract.getAddress();
}

struct _ContractCoverageContent*
create_empty_coverage(struct _ContractGraphContent* agraph)
{  ContractGraph& contractGraph = *reinterpret_cast<ContractGraph*>(agraph);
   return reinterpret_cast<struct _ContractCoverageContent*>(new ContractCoverage(contractGraph));
}

void
free_coverage(struct _ContractCoverageContent* acoverage)
{  ContractCoverage* coverage = reinterpret_cast<ContractCoverage*>(acoverage);
   delete coverage;
}

struct _WarningsContent*
create_warnings()
{  return reinterpret_cast<struct _WarningsContent*>(new Warnings()); }

void
free_warnings(struct _WarningsContent* warnings)
{  delete reinterpret_cast<Warnings*>(warnings); }

struct _WarningCursorContent*
warning_create_cursor(struct _WarningsContent* awarnings) {
   auto& warnings = *reinterpret_cast<Warnings*>(awarnings);
   return reinterpret_cast<struct _WarningCursorContent*>(new Warnings::Cursor(warnings));
}

void
warning_free_cursor(struct _WarningCursorContent* warning_cursor)
{  delete reinterpret_cast<Warnings::Cursor*>(warning_cursor); }

bool
warning_set_to_next(struct _WarningCursorContent* warning_cursor)
{  return reinterpret_cast<Warnings::Cursor*>(warning_cursor)->setToNext(); }

void
warning_retrieve_message(struct _WarningCursorContent* warning_cursor, struct _Warning* warning)
{  const auto& error = reinterpret_cast<Warnings::Cursor*>(warning_cursor)->elementAt();
   warning->filepos = error.filepos().getChunk().string;
   warning->linepos = error.linepos();
   warning->columnpos = error.columnpos();
   warning->message = error.getMessage().getChunk().string;
}

bool
is_coverage_complete(struct _ContractCoverageContent* acoverage,
      struct _ContractContent* first, struct _ContractContent* last)
{  ContractCoverage& coverage = *reinterpret_cast<ContractCoverage*>(acoverage);
   return coverage.isComplete();
}

uint64_t* address_vector_realloc_addresses(uint64_t* old_addresses, int old_size,
      int* new_size, void* address_container)
{  auto* container = reinterpret_cast<std::vector<uint64_t>*>(address_container);
   assert(container->size() == (size_t) old_size
         && old_addresses == &(*container)[0]);
   if (old_size < 6)
      *new_size = 6;
   else
      *new_size = old_size*3/2;
   container->insert(container->end(), (*new_size-old_size), 0);
   return &(*container)[0];
}


TargetAddresses
create_address_vector() {
   auto* result = new std::vector<uint64_t>;
   result->push_back(0);
   result->push_back(0);
   return TargetAddresses { &(*result)[0], 2, 0, &address_vector_realloc_addresses, result };
}

void
free_address_vector(TargetAddresses* addresses) {
   auto* container = reinterpret_cast<std::vector<uint64_t>*>(addresses->address_container);
   delete container;
   *addresses = { nullptr, 0, 0, nullptr, nullptr };
}

} // end of extern "C"
