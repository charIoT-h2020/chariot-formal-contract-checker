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
#include <iostream>

extern "C" {

struct _PProcessor*
create_processor(const char* architectureLibrary, const char* domainLibrary)
{  try {
   std::unique_ptr<Processor> result(new Processor());
   result->setFromFile(architectureLibrary);
   result->setDomainFunctionsFromFile(domainLibrary);
   return reinterpret_cast<struct _PProcessor*>(result.release());
   }
   catch (ESPreconditionError& error) {
     std::cerr << "unable to initialize the processor!\n";
     error.print(std::cerr);
     std::cerr.flush();
     return nullptr;
   }
   catch (...) {
     std::cerr << "unable to initialize the processor!" << std::endl;
     return nullptr;
   }
}

void
free_processor(struct _PProcessor* processor)
{  try {
   delete reinterpret_cast<Processor*>(processor);
   }
   catch (ESPreconditionError& error) {
     std::cerr << "unable to free the processor!\n";
     error.print(std::cerr);
     std::cerr.flush();
   }
   catch (...) {
     std::cerr << "unable to free the processor!" << std::endl;
   }
}

void
processor_set_verbose(struct _PProcessor* processor)
{  try {
   reinterpret_cast<Processor*>(processor)->setVerbose();
   }
   catch (ESPreconditionError& error) {
     std::cerr << "unable to set the verbose mode!\n";
     error.print(std::cerr);
     std::cerr.flush();
   }
   catch (...) {
     std::cerr << "unable to set the verbose mode!" << std::endl;
   }
}

bool
processor_load_code(struct _PProcessor* aprocessor, const char* filename)
{  try {
   Processor* processor = reinterpret_cast<Processor*>(aprocessor);
   processor->binaryFile().open(filename, std::ifstream::binary);
   if (!processor->binaryFile().good())
      return false;
   return true;
   }
   catch (ESPreconditionError& error) {
     std::cerr << "unable to load the code!\n";
     error.print(std::cerr);
     std::cerr.flush();
     return false;
   }
   catch (...) {
     std::cerr << "unable to load the code!" << std::endl;
     return false;
   }
}

struct _PDecisionVector*
processor_create_decision_vector(struct _PProcessor* aprocessor)
{  try {
   Processor* processor = reinterpret_cast<Processor*>(aprocessor);
   return reinterpret_cast<struct _PDecisionVector*>(new DecisionVector(processor->createDecisionVector()));
   }
   catch (ESPreconditionError& error) {
     std::cerr << "unable to create a decision vector!\n";
     error.print(std::cerr);
     std::cerr.flush();
     return nullptr;
   }
   catch (...) {
     std::cerr << "unable to create a decision vector!" << std::endl;
     return nullptr;
   }
}

struct _PDecisionVector*
processor_clone_decision_vector(struct _PDecisionVector* decision_vector)
{  try {
   DecisionVector* decision = reinterpret_cast<DecisionVector*>(decision_vector);
   return reinterpret_cast<struct _PDecisionVector*>(new DecisionVector(*decision));
   }
   catch (ESPreconditionError& error) {
     std::cerr << "unable to clone a decision vector!\n";
     error.print(std::cerr);
     std::cerr.flush();
     return nullptr;
   }
   catch (...) {
     std::cerr << "unable to clone a decision vector!" << std::endl;
     return nullptr;
   }
}

void processor_free_decision_vector(struct _PDecisionVector* decision_vector)
{  try {
   DecisionVector* decision = reinterpret_cast<DecisionVector*>(decision_vector);
   delete decision;
   }
   catch (ESPreconditionError& error) {
     std::cerr << "unable to free a decision vector!\n";
     error.print(std::cerr);
     std::cerr.flush();
   }
   catch (...) {
     std::cerr << "unable to free a decision vector!" << std::endl;
   }
}

void processor_filter_decision_vector(struct _PDecisionVector* decision_vector, uint64_t address)
{  try {
   DecisionVector* decision = reinterpret_cast<DecisionVector*>(decision_vector);
   decision->filter(address);
   }
   catch (ESPreconditionError& error) {
     std::cerr << "unable to filter a decision vector!\n";
     error.print(std::cerr);
     std::cerr.flush();
   }
   catch (...) {
     std::cerr << "unable to filter a decision vector!" << std::endl;
   }
}

bool
processor_get_targets(struct _PProcessor* aprocessor, uint64_t address,
      struct _ContractContent* contract, struct _PDecisionVector* adecision,
      TargetAddresses* target_addresses)
{  try {
   Processor& processor = *reinterpret_cast<Processor*>(aprocessor);
   Contract& startContract = *reinterpret_cast<Contract*>(contract);
   DecisionVector& decision = *reinterpret_cast<DecisionVector*>(adecision);
   MemoryState memoryState(processor.getRegistersNumber(), processor.getDomainFunctions());
   MemoryInterpretParameters parameters;
   if (startContract.isInitial())
      processor.initializeMemory(memoryState, parameters);
   startContract.applyTo(memoryState, processor.getContent(), &processor.getArchitectureFunctions());
   return processor.retrieveNextTargets(address, memoryState, *target_addresses, decision, parameters);
   }
   catch (ESPreconditionError& error) {
     std::cerr << "unable to get targets!\n";
     error.print(std::cerr);
     std::cerr.flush();
     return false;
   }
   catch (...) {
     std::cerr << "unable to get targets!" << std::endl;
     return false;
   }
}

bool processor_check_block(struct _PProcessor* aprocessor, uint64_t address,
      uint64_t target, struct _ContractContent* afirstContract,
      struct _ContractContent* alastContract, struct _PDecisionVector* adecision,
      struct _ContractCoverageContent* acoverage, struct _WarningsContent* awarnings)
{  try {
   Processor& processor = *reinterpret_cast<Processor*>(aprocessor);
   Contract& firstContract = *reinterpret_cast<Contract*>(afirstContract);
   Contract& lastContract = *reinterpret_cast<Contract*>(alastContract);
   DecisionVector& decision = *reinterpret_cast<DecisionVector*>(adecision);
   Warnings& warnings = *reinterpret_cast<Warnings*>(awarnings);
   MemoryState memoryState(processor.getRegistersNumber(), processor.getDomainFunctions());
   MemoryInterpretParameters parameters;
   if (firstContract.isInitial())
      processor.initializeMemory(memoryState, parameters);
   firstContract.applyTo(memoryState, processor.getContent(), &processor.getArchitectureFunctions());
   processor.interpret(address, memoryState, target, decision, warnings, parameters);
   MemoryState lastMemoryState(processor.getRegistersNumber(), processor.getDomainFunctions());
   // processor.initializeMemory(lastMemoryState, parameters);
   lastContract.applyTo(lastMemoryState, processor.getContent(), &processor.getArchitectureFunctions());
   ContractCoverage& coverage = *reinterpret_cast<ContractCoverage*>(acoverage);
   coverage.add(firstContract, lastContract);
   return lastMemoryState.contain(memoryState, parameters);
   }
   catch (ESPreconditionError& error) {
     std::cerr << "unable to check block!\n";
     error.print(std::cerr);
     std::cerr.flush();
     return false;
   }
   catch (...) {
     std::cerr << "unable to check block!" << std::endl;
     return false;
   }
}

struct _ContractGraphContent* load_contracts(const char* inputFilename,
      struct _PProcessor* aprocessor, struct _WarningsContent* awarnings)
{  try {
   Processor& processor = *reinterpret_cast<Processor*>(aprocessor);
   std::unique_ptr<ContractGraph> result(new ContractGraph());
   Warnings& warnings = *reinterpret_cast<Warnings*>(awarnings);
   result->loadFromFile(inputFilename, processor.getDomainFunctions(),
         processor.getContent(), &processor.getArchitectureFunctions(), warnings);
   return reinterpret_cast<struct _ContractGraphContent*>(result.release());
   }
   catch (ESPreconditionError& error) {
     std::cerr << "unable to load contracts!\n";
     error.print(std::cerr);
     std::cerr.flush();
     return nullptr;
   }
   catch (...) {
     std::cerr << "unable to load contracts!" << std::endl;
     return nullptr;
   }
}

void free_contracts(struct _ContractGraphContent* acontracts)
{  try {
   delete reinterpret_cast<ContractGraph*>(acontracts);
   }
   catch (ESPreconditionError& error) {
     std::cerr << "unable to free contracts!\n";
     error.print(std::cerr);
     std::cerr.flush();
   }
   catch (...) {
     std::cerr << "unable to free contracts!" << std::endl;
   }
}

void contract_fill_stop_addresses(struct _ContractContent* acontract, TargetAddresses* stop_addresses)
{  try {
   Contract& contract = *reinterpret_cast<Contract*>(acontract);
   contract.retrieveNextAddresses(*stop_addresses);
   }
   catch (ESPreconditionError& error) {
     std::cerr << "unable to fill stop addresses for contract checking!\n";
     error.print(std::cerr);
     std::cerr.flush();
   }
   catch (...) {
     std::cerr << "unable to fill stop addresses for contract checking!" << std::endl;
   }
}

struct _ContractCursorContent*
contract_cursor_new(struct _ContractGraphContent* agraph)
{  try {
   ContractGraph& contractGraph = *reinterpret_cast<ContractGraph*>(agraph);
   return reinterpret_cast<struct _ContractCursorContent*>(new ContractGraph::Cursor(contractGraph));
   }
   catch (ESPreconditionError& error) {
     std::cerr << "unable to create a new contract cursor!\n";
     error.print(std::cerr);
     std::cerr.flush();
     return nullptr;
   }
   catch (...) {
     std::cerr << "unable to create a new contract cursor!" << std::endl;
     return nullptr;
   }
}

bool
contract_cursor_set_to_next(struct _ContractCursorContent* agraphCursor)
{  try {
   ContractGraph::Cursor& graphCursor = *reinterpret_cast<ContractGraph::Cursor*>(agraphCursor);
   return graphCursor.setToNext();
   }
   catch (ESPreconditionError& error) {
     std::cerr << "unable to go to the next contract!\n";
     error.print(std::cerr);
     std::cerr.flush();
     return false;
   }
   catch (...) {
     std::cerr << "unable to go to the next contract!" << std::endl;
     return false;
   }
}

bool
contract_cursor_is_initial(struct _ContractCursorContent* agraphCursor)
{  try {
   ContractGraph::Cursor& graphCursor = *reinterpret_cast<ContractGraph::Cursor*>(agraphCursor);
   return graphCursor.elementAt()->isInitial();
   }
   catch (ESPreconditionError& error) {
     std::cerr << "unable to see if the contract is initial!\n";
     error.print(std::cerr);
     std::cerr.flush();
     return false;
   }
   catch (...) {
     std::cerr << "unable to see if the contract is initial!" << std::endl;
     return false;
   }
}

bool
contract_cursor_is_final(struct _ContractCursorContent* agraphCursor)
{  try {
   ContractGraph::Cursor& graphCursor = *reinterpret_cast<ContractGraph::Cursor*>(agraphCursor);
   return graphCursor.elementAt()->isFinal();
   }
   catch (ESPreconditionError& error) {
     std::cerr << "unable to to see if the contract is final!\n";
     error.print(std::cerr);
     std::cerr.flush();
     return false;
   }
   catch (...) {
     std::cerr << "unable to to see if the contract is final!" << std::endl;
     return false;
   }
}

bool
contract_cursor_set_address(struct _ContractCursorContent* agraphCursor, uint64_t address,
      enum ContractConditionLocalization localization)
{  try {
   ContractGraph::Cursor& graphCursor = *reinterpret_cast<ContractGraph::Cursor*>(agraphCursor);
   if (graphCursor.setToNext() && graphCursor.elementAt()->getAddress() == address)
      return true;

   const auto& support = (const ContractGraph&) graphCursor.getSupport();
   support.locateKey(address, graphCursor, (localization == CCLPreCondition)
         ? COL::VirtualCollection::RPBefore : ((localization == CCLPostCondition)
         ? COL::VirtualCollection::RPAfter : COL::VirtualCollection::RPUndefined));
   return graphCursor.isValid();
   }
   catch (ESPreconditionError& error) {
     std::cerr << "unable to set the contract address!\n";
     error.print(std::cerr);
     std::cerr.flush();
     return false;
   }
   catch (...) {
     std::cerr << "unable to set the contract address!" << std::endl;
     return false;
   }
}

uint64_t
contract_cursor_get_address(struct _ContractCursorContent* agraphCursor)
{  try {
   ContractGraph::Cursor& graphCursor = *reinterpret_cast<ContractGraph::Cursor*>(agraphCursor);
   return graphCursor.elementAt()->getAddress();
   }
   catch (ESPreconditionError& error) {
     std::cerr << "unable to get the contract address!\n";
     error.print(std::cerr);
     std::cerr.flush();
     return 0;
   }
   catch (...) {
     std::cerr << "unable to get the contract address!" << std::endl;
     return 0;
   }
}

struct _ContractCursorContent*
contract_cursor_clone(struct _ContractCursorContent* agraphCursor)
{  try {
   ContractGraph::Cursor& graphCursor = *reinterpret_cast<ContractGraph::Cursor*>(agraphCursor);
   return reinterpret_cast<struct _ContractCursorContent*>(new ContractGraph::Cursor(graphCursor));
   }
   catch (ESPreconditionError& error) {
     std::cerr << "unable to clone a contract cursor!\n";
     error.print(std::cerr);
     std::cerr.flush();
     return nullptr;
   }
   catch (...) {
     std::cerr << "unable to clone a contract cursor!" << std::endl;
     return nullptr;
   }
}

void
contract_cursor_free(struct _ContractCursorContent* agraphCursor)
{  try {
   ContractGraph::Cursor* graphCursor = reinterpret_cast<ContractGraph::Cursor*>(agraphCursor);
   delete graphCursor;
   }
   catch (ESPreconditionError& error) {
     std::cerr << "unable to free a contract cursor!\n";
     error.print(std::cerr);
     std::cerr.flush();
   }
   catch (...) {
     std::cerr << "unable to free a contract cursor!" << std::endl;
   }
}

struct _ContractContent*
contract_cursor_get_contract(struct _ContractCursorContent* agraphCursor)
{  try {
   ContractGraph::Cursor& graphCursor = *reinterpret_cast<ContractGraph::Cursor*>(agraphCursor);
   return reinterpret_cast<struct _ContractContent*>(&*graphCursor.elementSAt());
   }
   catch (ESPreconditionError& error) {
     std::cerr << "unable to get the contract!\n";
     error.print(std::cerr);
     std::cerr.flush();
     return nullptr;
   }
   catch (...) {
     std::cerr << "unable to get the contract!" << std::endl;
     return nullptr;
   }
}

struct _ContractContent*
create_contract(const char* filename) /* content is possible */
{  try {
   std::unique_ptr<Contract> result(new Contract());
   STG::DIOObject::IFStream in(filename);
   result->read(in, Contract::FormatParameters());
   return reinterpret_cast<struct _ContractContent*>(result.release());
   }
   catch (ESPreconditionError& error) {
     std::cerr << "unable to create a contract!\n";
     error.print(std::cerr);
     std::cerr.flush();
     return nullptr;
   }
   catch (...) {
     std::cerr << "unable to create a contract!" << std::endl;
     return nullptr;
   }
}

void
free_contract(struct _ContractContent* acontract)
{  try {
   Contract* contract = reinterpret_cast<Contract*>(acontract);
   delete contract;
   }
   catch (ESPreconditionError& error) {
     std::cerr << "unable to free a contract!\n";
     error.print(std::cerr);
     std::cerr.flush();
   }
   catch (...) {
     std::cerr << "unable to free a contract!" << std::endl;
   }
}

uint64_t
contract_get_address(struct _ContractContent* acontract)
{  try {
   Contract& contract = *reinterpret_cast<Contract*>(acontract);
   return contract.getAddress();
   }
   catch (ESPreconditionError& error) {
     std::cerr << "unable to get the contract address!\n";
     error.print(std::cerr);
     std::cerr.flush();
     return 0;
   }
   catch (...) {
     std::cerr << "unable to get the contract address!" << std::endl;
     return 0;
   }
}

struct _ContractCoverageContent*
create_empty_coverage(struct _ContractGraphContent* agraph)
{  try {
   ContractGraph& contractGraph = *reinterpret_cast<ContractGraph*>(agraph);
   return reinterpret_cast<struct _ContractCoverageContent*>(new ContractCoverage(contractGraph));
   }
   catch (ESPreconditionError& error) {
     std::cerr << "unable to create a contract coverage!\n";
     error.print(std::cerr);
     std::cerr.flush();
     return nullptr;
   }
   catch (...) {
     std::cerr << "unable to create a contract coverage!" << std::endl;
     return nullptr;
   }
}

void
free_coverage(struct _ContractCoverageContent* acoverage)
{  try {
   ContractCoverage* coverage = reinterpret_cast<ContractCoverage*>(acoverage);
   delete coverage;
   }
   catch (ESPreconditionError& error) {
     std::cerr << "unable to free a contract coverage!\n";
     error.print(std::cerr);
     std::cerr.flush();
   }
   catch (...) {
     std::cerr << "unable to free a contract coverage!" << std::endl;
   }
}

struct _WarningsContent*
create_warnings()
{  try {
   return reinterpret_cast<struct _WarningsContent*>(new Warnings());
   }
   catch (ESPreconditionError& error) {
     std::cerr << "unable to create warnings!\n";
     error.print(std::cerr);
     std::cerr.flush();
     return nullptr;
   }
   catch (...) {
     std::cerr << "unable to create warnings!" << std::endl;
     return nullptr;
   }
}

void
free_warnings(struct _WarningsContent* warnings)
{  delete reinterpret_cast<Warnings*>(warnings); }

struct _WarningCursorContent*
warning_create_cursor(struct _WarningsContent* awarnings)
{  try {
   auto& warnings = *reinterpret_cast<Warnings*>(awarnings);
   return reinterpret_cast<struct _WarningCursorContent*>(new Warnings::Cursor(warnings));
   }
   catch (ESPreconditionError& error) {
     std::cerr << "unable to iterate over warnings!\n";
     error.print(std::cerr);
     std::cerr.flush();
     return nullptr;
   }
   catch (...) {
     std::cerr << "unable to iterate over warnings!" << std::endl;
     return nullptr;
   }
}

void
warning_free_cursor(struct _WarningCursorContent* warning_cursor)
{  try {
   delete reinterpret_cast<Warnings::Cursor*>(warning_cursor);
   }
   catch (ESPreconditionError& error) {
     std::cerr << "unable to free the warning iterator!\n";
     error.print(std::cerr);
     std::cerr.flush();
   }
   catch (...) {
     std::cerr << "unable to free the warning iterator!" << std::endl;
   }
}

bool
warning_set_to_next(struct _WarningCursorContent* warning_cursor)
{  try {
   return reinterpret_cast<Warnings::Cursor*>(warning_cursor)->setToNext();
   }
   catch (ESPreconditionError& error) {
     std::cerr << "unable to iterate over warnings!\n";
     error.print(std::cerr);
     std::cerr.flush();
     return false;
   }
   catch (...) {
     std::cerr << "unable to iterate over warnings!" << std::endl;
     return false;
   }
}

void
warning_retrieve_message(struct _WarningCursorContent* warning_cursor, struct _Warning* warning)
{  try {
   const auto& error = reinterpret_cast<Warnings::Cursor*>(warning_cursor)->elementAt();
   warning->filepos = error.filepos().getChunk().string;
   warning->linepos = error.linepos();
   warning->columnpos = error.columnpos();
   warning->message = error.getMessage().getChunk().string;
   }
   catch (ESPreconditionError& error) {
     std::cerr << "unable to retrieve the warning message!\n";
     error.print(std::cerr);
     std::cerr.flush();
   }
   catch (...) {
     std::cerr << "unable to retrieve the warning message!" << std::endl;
   }
}

bool
is_coverage_complete(struct _ContractCoverageContent* acoverage,
      struct _ContractContent* first, struct _ContractContent* last)
{  try {
   ContractCoverage& coverage = *reinterpret_cast<ContractCoverage*>(acoverage);
   return coverage.isComplete();
   }
   catch (ESPreconditionError& error) {
     std::cerr << "unable to see if the contract coverage is complete!\n";
     error.print(std::cerr);
     std::cerr.flush();
     return false;
   }
   catch (...) {
     std::cerr << "unable to see if the contract coverage is complete!" << std::endl;
     return false;
   }
}

uint64_t* address_vector_realloc_addresses(uint64_t* old_addresses, int old_size,
      int* new_size, void* address_container)
{  try {
   auto* container = reinterpret_cast<std::vector<uint64_t>*>(address_container);
   assert(container->size() == (size_t) old_size
         && old_addresses == &(*container)[0]);
   if (old_size < 6)
      *new_size = 6;
   else
      *new_size = old_size*3/2;
   container->insert(container->end(), (*new_size-old_size), 0);
   return &(*container)[0];
   }
   catch (ESPreconditionError& error) {
     std::cerr << "unable to realloc targets when a new target is detected!\n";
     error.print(std::cerr);
     std::cerr.flush();
     return nullptr;
   }
   catch (...) {
     std::cerr << "unable to realloc targets when a new target is detected!" << std::endl;
     return nullptr;
   }
}

TargetAddresses
create_address_vector()
{  try {
 
   auto* result = new std::vector<uint64_t>;
   result->push_back(0);
   result->push_back(0);
   return TargetAddresses { &(*result)[0], 2, 0, &address_vector_realloc_addresses, result };
   }
   catch (ESPreconditionError& error) {
     std::cerr << "unable to create a set of targets!\n";
     error.print(std::cerr);
     std::cerr.flush();
     return TargetAddresses{};
   }
   catch (...) {
     std::cerr << "unable to create a set of targets!" << std::endl;
     return TargetAddresses{};
   }
}

void
free_address_vector(TargetAddresses* addresses) {
   auto* container = reinterpret_cast<std::vector<uint64_t>*>(addresses->address_container);
   delete container;
   *addresses = { nullptr, 0, 0, nullptr, nullptr };
}

void
flush_cpp_stdout() { std::cout.flush(); }

} // end of extern "C"
