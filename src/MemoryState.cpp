#include "MemoryState.h"
#include "Collection/Collection.template"

MemoryModelFunctions MemoryState::functions={
   &MemoryState::set_number_of_registers,
   &MemoryState::set_register_value,
   &MemoryState::get_register_value,
   &MemoryState::load_multibit_value,
   &MemoryState::load_multibit_disjunctive_value,
   &MemoryState::load_multifloat_value,
   &MemoryState::store_value,
   nullptr /* &MemoryState::constraint_store_value */,
   nullptr /* &MemoryState::constraint_address */
};

// [TODO] to implement
void
MemoryState::setFromContract(const Contract& contract)
{
}

template class COL::TSortedAVL<MemoryState::RegisterValue, MemoryState::RegisterValue::Key>;
