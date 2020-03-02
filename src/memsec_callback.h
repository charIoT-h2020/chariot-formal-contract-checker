/////////////////////////////////
//
// Library   : Static Analysis
// Unit      : armsec
// File      : armsec_memory_callback.h
// Copyright : CEA LIST - 2020
//
// Description :
//   Definition of a generic C Interface to manipulate abstract domains
//   = interval, disjunctions, ...
//

#pragma once

#include "domsec_callback.h"

#include <stddef.h>

#ifdef _MSC_VER
#pragma pack(push,4)
#endif

#ifdef __cplusplus
extern "C" {
#endif

enum MemoryEvaluationErrorFlags
{  MEEFNoError=0, MEEFImpreciseAccess=1<<1, MEEFUndefinedAccess=1<<2, MEEFNullAccess=1<<3,
   MEEFUnallocatedAccess=1<<4, MEEFMemoryExhausted=1<<5, MEEFOSDependantAcess=1<<6
};

typedef struct _InterpretParameters InterpretParameters;
typedef struct _MemoryModel MemoryModel;
typedef struct _MemoryModelFunctions {
  void (*set_number_of_registers)(MemoryModel* memory, int numbers);
  MemoryModel* (*clone)(MemoryModel* memory);
  void (*assign)(MemoryModel* first, MemoryModel* second);
  void (*swap)(MemoryModel* first, MemoryModel* second);
  void (*free)(MemoryModel* memory);
  void (*merge)(MemoryModel* first, MemoryModel* second);

  void (*set_register_value)(MemoryModel* memory, int registerIndex,
      DomainElement* value, InterpretParameters* parameters,
      unsigned* error /* set of MemoryEvaluationErrorFlags */);
  DomainElement (*get_register_value)(MemoryModel* memory,
      int registerIndex, InterpretParameters* parameters,
      unsigned* error /* set of MemoryEvaluationErrorFlags */,
      DomainElementFunctions** elementFunctions);

  DomainElement (*load_multibit_value)(MemoryModel* memory,
      DomainElement indirect_address, size_t size, InterpretParameters* parameters,
      unsigned* error, DomainElementFunctions** elementFunctions);
  DomainElement (*load_multibit_disjunctive_value)(MemoryModel* memory,
      DomainElement indirect_address, size_t size, InterpretParameters* parameters,
      unsigned* error, DomainElementFunctions** elementFunctions);
  DomainElement (*load_multifloat_value)(MemoryModel* memory,
      DomainElement indirect_address, size_t size, InterpretParameters* parameters,
      unsigned* error, DomainElementFunctions** elementFunctions);

  void (*store_value)(MemoryModel* memory, DomainElement indirect_address,
      DomainElement value, InterpretParameters* parameters, unsigned* error);
  void (*constraint_store_value)(MemoryModel* memory, DomainElement indirect_address,
      DomainElement value, unsigned indirect_register, InterpretParameters* parameters, unsigned* error);
  void (*constraint_address)(MemoryModel* memory, DomainElement indirect_address,
      DomainElement value, InterpretParameters* parameters, unsigned* error);
} MemoryModelFunctions;

#ifdef __cplusplus
}
#endif

#ifdef _MSC_VER
#pragma pack(pop)
#endif

