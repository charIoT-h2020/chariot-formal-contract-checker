/////////////////////////////////
//
// Library   : Static Analysis
// Unit      : armsec
// File      : armsec_decoder.h
// Copyright : CEA LIST - 2020
//
// Description :
//   Definition of a generic C Interface to manipulate contracts
//

#pragma once

#if defined(_WIN32)
#define DLL_CDECL __cdecl
#else
#define DLL_CDECL 
#endif

#if defined(_WIN32) && defined(_USRDLL)
#ifdef DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif // DLL_EXPORTS
#else
#define DLL_API
#endif // _USRDLL

#ifdef _MSC_VER
#pragma pack(push,4)
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#if defined(_MSC_VER) && !defined(__cplusplus)
#define inline __inline
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _SAL_VERSION

#define _In_ 
#define _Out_ 
#define _Inout_ 
#define _In_opt_ 

#endif

#include "target_address_decoder.h"

struct _PProcessor;
struct _PProcessor* create_processor(const char* architectureLibrary, const char* domainLibrary);
void free_processor(struct _PProcessor*);
void processor_set_verbose(struct _PProcessor*);
bool processor_load_code(struct _PProcessor* processor, const char* filename);
bool processor_get_targets(struct _PProcessor* processor, uint64_t address,
      struct _ContractContent* contract, TargetAddresses* target_addresses);
struct _ContractContent;
struct _ContractCoverageContent;
struct _WarningsContent;
bool processor_check_block(struct _PProcessor* processor, uint64_t address,
      uint64_t target, struct _ContractContent* firstContract,
      struct _ContractContent* lastContract, struct _ContractCoverageContent* coverage,
      struct _WarningsContent* warnings);

struct _ContractGraphContent;

struct _ContractGraphContent* load_contracts(const char* inputFilename,
      struct _PProcessor* processor, struct _WarningsContent* awarnings);
enum ContractConditionLocalization { CCLPreCondition, CCLPostCondition };
void free_contracts(struct _ContractGraphContent* contracts);
void contract_fill_stop_addresses(struct _ContractContent*, TargetAddresses* stop_addresses);

struct _ContractCursorContent;
struct _ContractCursorContent* contract_cursor_new(struct _ContractGraphContent*);
bool contract_cursor_set_to_next(struct _ContractCursorContent*);
bool contract_cursor_is_initial(struct _ContractCursorContent*);
bool contract_cursor_is_final(struct _ContractCursorContent*);
bool contract_cursor_set_address(struct _ContractCursorContent*, uint64_t address,
      enum ContractConditionLocalization localization);
uint64_t contract_cursor_get_address(struct _ContractCursorContent*);
struct _ContractCursorContent* contract_cursor_clone(struct _ContractCursorContent*);
void contract_cursor_free(struct _ContractCursorContent*);
struct _ContractContent* contract_cursor_get_contract(struct _ContractCursorContent*);

struct _ContractContent* create_contract(const char* filename); /* content is possible */
void free_contract(struct _ContractContent* contract);
uint64_t contract_get_address(struct _ContractContent* contract);

struct _ContractCoverageContent;
struct _ContractCoverageContent* create_empty_coverage(struct _ContractGraphContent* agraph);
void free_coverage(struct _ContractCoverageContent* coverage);

struct _Warning {
   const char* filepos;
   int linepos, columnpos;
   const char* message;
};

struct _WarningsContent* create_warnings();
void free_warnings(struct _WarningsContent* warnings);
struct _WarningCursorContent;
struct _WarningCursorContent* warning_create_cursor(struct _WarningsContent* warnings);
void warning_free_cursor(struct _WarningCursorContent* warning_cursor);
bool warning_set_to_next(struct _WarningCursorContent* warning_cursor);
void warning_retrieve_message(struct _WarningCursorContent* warning_cursor, struct _Warning* warning);

bool is_coverage_complete(struct _ContractCoverageContent* coverage,
      struct _ContractContent* first, struct _ContractContent* last);

TargetAddresses create_address_vector();
void free_address_vector(TargetAddresses*);

#ifdef __cplusplus
}
#endif

#ifdef _MSC_VER
#pragma pack(pop)
#endif


