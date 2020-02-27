#include "contract_checker.h"
#include <iostream>
#include <cstring>
#include <cassert>
#include <vector>

class ProcessArgument {
  private:
   const char* szInputFile = nullptr;
   const char* szMemoryFile = nullptr;
   const char* szArch = nullptr;
   const char* szDomain = nullptr;
   const char* szProperty = nullptr;
   bool fVerbose = false;
   bool fHasEchoedMessage = false;

  public:
   ProcessArgument() = default;
   ProcessArgument(int argc, char** argv)
      {  int arg = argc-2;
         while (arg >= 0)
            fHasEchoedMessage = !process(argv + (argc - arg - 1), arg) && fHasEchoedMessage;
      }

   bool process(char** argument, int& currentArgument);
   void printUsage(std::ostream& out) const
      {  if (fHasEchoedMessage)
            return;
         out << "usage of check_contract:\n"
             << "\tcheck_contract [options] binary_file first_address last_address contracts.json\n"
             << "\n"
             << "where option can be:\n"
             << "\t-arch armsec.so \tto provide the instruction set\n"
             << "\t-dom domsec.so \tto provide the domain library\n"
             << "\t-prop property.json \tuser contract to check\n"
             << "\t-v \t\tfor verbose mode\n";
         out.flush();
      }

   bool isValid() const { return szInputFile && szMemoryFile; }
   bool hasEchoedMessage() const { return fHasEchoedMessage; }
   bool isVerbose() const { return fVerbose; }

   const char* getInputFile() const { return szInputFile; }
   const char* getMemoryFile() const { return szMemoryFile; }
   bool hasArch() const { return szArch; }
   bool hasDomain() const { return szDomain; }
   bool hasProperty() const { return szProperty; }
   const char* getArch() const { return szArch; }
   const char* getDomain() const { return szDomain; }
   const char* getProperty() const { return szProperty; }
};

bool
ProcessArgument::process(char** argument, int& currentArgument) {
   if (argument[0][0] == '-') {
      switch (argument[0][1]) {
         case 'a':
            if (strcmp(&argument[0][1], "arch") == 0) {
               if (currentArgument == 0 || szArch) {
                  printUsage(std::cout);
                  return false;
               };
               currentArgument -= 2;
               szArch = argument[1];
            }
            else {
               printUsage(std::cout);
               fHasEchoedMessage = true;
               --currentArgument;
            }
            return true;
         case 'd':
            if (strcmp(&argument[0][1], "dom") == 0) {
               if (currentArgument == 0 || szDomain) {
                  printUsage(std::cout);
                  return false;
               };
               currentArgument -= 2;
               szDomain = argument[1];
            }
            else {
               printUsage(std::cout);
               fHasEchoedMessage = true;
               --currentArgument;
            }
            return true;
         case 'p':
            if (strcmp(&argument[0][1], "prop") == 0) {
               if (currentArgument == 0 || szProperty) {
                  printUsage(std::cout);
                  return false;
               };
               currentArgument -= 2;
               szProperty = argument[1];
            }
            else {
               printUsage(std::cout);
               fHasEchoedMessage = true;
               --currentArgument;
            }
            return true;
         case 'v':
            if ((strcmp(&argument[0][1], "v") == 0) ||
                  (strcmp(&argument[0][1], "verbose") == 0)) {
               --currentArgument;
               fVerbose = true;
            }
            else {
               printUsage(std::cout);
               fHasEchoedMessage = true;
               --currentArgument;
            }
            return true;
         default:
            printUsage(std::cout);
            --currentArgument;
            return false;
      };
   }
   else {
      if (szInputFile) {
         if (szMemoryFile) {
            printUsage(std::cout);
            --currentArgument;
            return false;
         }
         szMemoryFile = argument[0];
         --currentArgument;
      }
      else {
         szInputFile = argument[0];
         --currentArgument;
      }
   }
   return true;
}

class AContractCoverage;
class AWarnings;

class AContractReference {
  private:
   struct _ContractContent* pvContent;

  public:
   AContractReference() : pvContent(nullptr) {}
   AContractReference(struct _ContractContent* content) : pvContent(content) {}

   struct _ContractContent* getContent() const { return pvContent; }
   bool isValid() const { return pvContent; }
};

class AProcessor {
  private:
   struct _PProcessor* pvContent;

   static uint64_t* reallocAddresses(uint64_t* old_addresses, int old_size,
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

  public:
   AProcessor(const char* architectureLibrary, const char* domainLibrary)
      :  pvContent(create_processor(architectureLibrary, domainLibrary)) {}
   ~AProcessor() { if (pvContent) { free_processor(pvContent); pvContent = nullptr; } }

   struct _PProcessor* getContent() const { return pvContent; }
   void setVerbose()
      {  if (pvContent) { processor_set_verbose(pvContent); } }
   bool loadCode(const char* filename)
      {  return processor_load_code(pvContent, filename); }
   std::vector<uint64_t> getTargets(uint64_t address, const AContractReference& contract) const
      {  std::vector<uint64_t> result;
         result.push_back(0);
         result.push_back(0);
         TargetAddresses argument;
         argument.addresses = &result[0];
         argument.addresses_array_size = 2;
         argument.addresses_length = 0;
         argument.realloc_addresses = &reallocAddresses;
         argument.address_container = &result;
         contract_fill_stop_addresses(contract.getContent(), &argument);
         bool isValid = processor_get_targets(pvContent, address, contract.getContent(), &argument);
         assert(isValid);
         result.resize(argument.addresses_length);
         return result;
      }

   bool checkBlock(uint64_t address, uint64_t target, struct _ContractContent* firstContract,
         struct _ContractContent* lastContract, const AContractCoverage* coverage, AWarnings& warnings);
};

class AWarnings {
  private:
   struct _WarningsContent* pvContent;

  public:
   AWarnings() : pvContent(create_warnings()) {}
   ~AWarnings() { if (pvContent) { free_warnings(pvContent); pvContent = nullptr; } }

   struct _WarningsContent* getContent() const { return pvContent; }

   class Cursor {
     private:
      struct _WarningCursorContent* pvContent;

     public:
      Cursor(const AWarnings& support) : pvContent(warning_create_cursor(support.getContent())) {}
      ~Cursor() { warning_free_cursor(pvContent); }

      bool setToNext()
         {  return warning_set_to_next(pvContent); }
      struct _Warning elementAt() const
         {  struct _Warning result{};
            warning_retrieve_message(pvContent, &result);
            return result;
         }
   };
};

class AContracts {
  private:
   struct _ContractGraphContent* pvContent = nullptr;

  public:
   AContracts() = default;
   ~AContracts() { if (pvContent) { free_contracts(pvContent); pvContent = nullptr; } }
   bool loadFromFile(const char* filename, const AProcessor& processor, AWarnings& errors)
      {  assert(!pvContent);
         pvContent = load_contracts(filename, processor.getContent(), errors.getContent());
         return pvContent;
      }
   struct _ContractGraphContent* getContent() const { return pvContent; }

   class Cursor {
     private:
      struct _ContractCursorContent* pvContent;

     public:
      Cursor(AContracts& reference) : pvContent(contract_cursor_new(reference.pvContent)) {}
      Cursor(const Cursor& source) : pvContent(contract_cursor_clone(source.pvContent)) {}
      ~Cursor() { if (pvContent) { contract_cursor_free(pvContent); pvContent = nullptr; } }

      bool setToNext() { return contract_cursor_set_to_next(pvContent); }
      bool isInitial() const { return contract_cursor_is_initial(pvContent); }
      bool isFinal() const { return contract_cursor_is_final(pvContent); }
      bool setBeforeAddress(uint64_t address)
         {  return contract_cursor_set_address(pvContent, address, CCLPreCondition); }
      bool setAfterAddress(uint64_t address)
         {  return contract_cursor_set_address(pvContent, address, CCLPostCondition); }
      uint64_t getAddress() const { return contract_cursor_get_address(pvContent); }
      AContractReference getContract() const
         {  return AContractReference(contract_cursor_get_contract(pvContent)); }
   };
};

class AContract {
  private:
   struct _ContractContent* pvContent = nullptr;

  public:
   AContract() = default;
   ~AContract() { if (pvContent) { free_contract(pvContent); pvContent = nullptr; } }
   bool loadFromFile(const char* filename)
      {  assert(!pvContent);
         pvContent = create_contract(filename);
         return pvContent;
      }
   uint64_t getAddress() const { return contract_get_address(pvContent); }
   struct _ContractContent* getContent() const { return pvContent; }
};

class AContractCoverage {
  private:
   struct _ContractCoverageContent* pvContent;

  public:
   AContractCoverage(const AContracts& reference)
      :  pvContent(create_empty_coverage(reference.getContent())) {}
   ~AContractCoverage() { if (pvContent) { free_coverage(pvContent); pvContent = nullptr; } }

   bool isComplete(struct _ContractContent* firstContract, struct _ContractContent* lastContract) const
      {  return is_coverage_complete(pvContent, firstContract, lastContract); }
   struct _ContractCoverageContent* getContent() const { return pvContent; }
};

inline bool
AProcessor::checkBlock(uint64_t address, uint64_t target, struct _ContractContent* firstContract,
      struct _ContractContent* lastContract, const AContractCoverage* coverage, AWarnings& warnings)
{  return processor_check_block(pvContent, address, target, firstContract, lastContract,
      coverage ? coverage->getContent() : nullptr, warnings.getContent());
}

int main(int argc, char** argv) {
   ProcessArgument processArgument(argc, argv);
   if (argc == 1) {
      processArgument.printUsage(std::cout);
      return 0;
   };
   if (!processArgument.isValid()) {
      if (!processArgument.hasEchoedMessage())
         processArgument.printUsage(std::cout);
      return 0;
   };

   AProcessor processor(processArgument.hasArch() ? processArgument.getArch() : "armsec_decoder.so",
         processArgument.hasDomain() ? processArgument.getDomain() : "libScalarInterface.so");

   AContracts contracts;
   {  AWarnings warnings;
      if (!contracts.loadFromFile(processArgument.getMemoryFile(), processor, warnings)) {
         std::cerr << "unable to load contracts from file " << processArgument.getMemoryFile() << std::endl;
         AWarnings::Cursor cursor(warnings);
         while (cursor.setToNext()) {
            auto error = cursor.elementAt();
            std::cerr << error.filepos << ':' << error.linepos << " error at column "
               << error.columnpos << ", " << error.message << '\n';
         };
         std::cerr.flush();
         return 0;
      }
      if (processArgument.isVerbose())
         std::cout << "contracts in " << processArgument.getMemoryFile() << " successfully loaded\n";
   }
   AContracts::Cursor cursor(contracts);
   AContractCoverage coverage(contracts);
   if (!processor.loadCode(processArgument.getInputFile())) {
      std::cerr << "unable to load code from file " << processArgument.getInputFile() << std::endl;
      return 0;
   }
   if (processArgument.isVerbose())
      std::cout << "code in " << processArgument.getInputFile() << " successfully loaded\n";

   AContractReference firstContract, lastContract;
   if (processArgument.isVerbose())
      processor.setVerbose();

   while (cursor.setToNext()) {
      auto address = cursor.getAddress();
      if (cursor.isInitial())
         firstContract = cursor.getContract();
      if (cursor.isFinal()) {
         lastContract = cursor.getContract();
         continue;
      }
      if (processArgument.isVerbose())
         std::cout << "look for targets starting at " << address << "by instruction interpretation\n";
      auto targets = processor.getTargets(address, cursor.getContract());
      for (const auto& target : targets) {
         AContracts::Cursor lastCursor(cursor);
         lastCursor.setAfterAddress(target);
         assert(lastCursor.getAddress() == target);
         AWarnings warnings;
         break;
         if (!processor.checkBlock(address, target, cursor.getContract().getContent(),
                  lastCursor.getContract().getContent(), &coverage, warnings)) {
            // for (const auto& warning : warnings)
            //    std::cout << warning << '\n';
            // warnings.clear();
         }
      };
   }
   if (!firstContract.isValid()) {
      std::cerr << "no initial contract found\n";
      return 0;
   }
   if (!lastContract.isValid()) {
      std::cerr << "no final contract found\n";
      return 0;
   }
   if (coverage.isComplete(firstContract.getContent(), lastContract.getContent()))
      std::cout << "contract coverage fully verified!";
   else
      std::cout << "incomplete contract verification";
   std::cout << std::endl;

   if (processArgument.hasProperty()) {
      AContract contract;
      if (!contract.loadFromFile(processArgument.getProperty())) {
         std::cerr << "unable to load the property from file " << processArgument.getProperty() << std::endl;
         return 0;
      }
      if (coverage.isComplete(firstContract.getContent(), contract.getContent()))
         std::cout << "contract coverage fully verified!";
      else
         std::cout << "incomplete contract verification";
      cursor.setBeforeAddress(contract.getAddress());
      AWarnings warnings;
      if (!processor.checkBlock(cursor.getAddress(), contract.getAddress(),
               cursor.getContract().getContent(), contract.getContent(), nullptr, warnings)) {
         // for (const auto& warning : warnings)
         //    std::cout << warning << '\n';
         // warnings.clear();
      }
   }
   return 0;
}

/*
int main(int argc, char** argv) {
   ProcessArgument processArgument(argc, argv);
   if (argc == 1) {
      processArgument.printUsage(std::cout);
      return 0;
   };
   if (!processArgument.isValid()) {
      if (!processArgument.hasEchoedMessage())
         processArgument.printUsage(std::cout);
      return 0;
   };

   Processor processor(processArgument.hasArch() ? processArgument.getArch() : "armsec_decoder.so",
         processArgument.hasDomain() ? processArgument.getDomain() : "libScalarInterface.so");

   Contracts contracts;
   contracts.loadFromFile(processArgument.getMemoryFile());
   Contracts::Cursor cursor(contracts);
   ContractCoverage coverage(contracts);
   processor.loadCode(processArgument.getInputFile());
   ContractReference firstContract, lastContract;

   for (const auto& block : ...) {
      auto address = block.getAddress();
      cursor.setBeforeAddress(address);
      if (cursor.isInitial())
         firstContract = cursor.getContract();
      for (const auto& target : block.getTargets(cursor.getContract())) {
         Contracts::Cursor lastCursor(cursor);
         lastCursor.setAfterAddress(target.address);
         if (lastCursor.isFinal())
            lastContract = cursor.getContract();
         assert(lastCursor.getAddress() == target.address);
         Warnings warnings;
         if (!processor.checkBlock(address, target.address, target.length,
                  cursor.getContract(), lastCursor.getContract(), &coverage, warnings)) {
            for (const auto& warning : warnings)
               std::cout << warning << '\n';
            warnings.clear();
         }
      };
   }
   if (!firstContract.isValid()) {
      std::cout << "no initial contract found\n";
      return 0;
   }
   if (!lastContract.isValid()) {
      std::cout << "no final contract found\n";
      return 0;
   }
   if (coverage.isComplete(firstContract, lastContract))
      std::cout << "contract coverage fully verified!";
   else
      std::cout << "incomplete contract verification";
   std::cout << std::endl;

   if (processArgument.hasProperty()) {
      Contract contract;
      contract.loadFromFile(processArgument.getProperty());
      if (coverage.isComplete(firstContract, contract.getAddress()))
         std::cout << "contract coverage fully verified!";
      else
         std::cout << "incomplete contract verification";
      cursor.setBeforeAddress(contract.getAddress());
      Warnings warnings;
      if (!processor.checkBlock(cursor.getAddress(), contract.getAddress(),
               contract.getAddress()-cursor.getAddress(),
               cursor.getContract(), contract, nullptr, warnings)) {
         for (const auto& warning : warnings)
            std::cout << warning << '\n';
         warnings.clear();
      }
   }
   return 0;
}
*/
