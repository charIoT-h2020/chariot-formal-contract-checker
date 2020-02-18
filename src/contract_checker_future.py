#!/usr/bin/python3
from enum import Enum

# This api is complex because the Chariot Security Engine needs to have
#   the capability to check any property at any address in the binary code
# Usage to only check the contracts:
#   set_architecture(".../libarmv7.so")
#   set_arithmetic(".../libScalarInterface.so")
#   contracts = load_contracts(meta_data_file)
#   
#   contracts_cursor = contract.get_init()
#   iterate over linear blocks
#       # this provides block, length_block, start_address
#       messages = []
#       if not check_block(block, length_block, start_address,
#               contracts, contracts_cursor, messages):
#           for msg in messages:
#              print (msg)
#
# Since Security Engine is responsible to provide the required guarantees
#   it needs to check:
#       all the linear blocs of instructions between the initial contract
#           contract.get_init() and the final contracts should be scanned
#           (see contract invariants)
#   then all the properties defined in the contracts should be true for
#       any address covered by the paths contract.get_init() -> final contract
#
# if the Security Engine wants to know that the content of the content
#   of a register r1 is not zero at a given address, then it should
#       check that the address is covered by the contracts
#       create a contract at this address with (*r1)_32 != 0_32
#       add it in contracts
#       localize the linear block (block, length, start)
#           such that start <= address < start+length
#       call check_block(block, address-start, start, contracts,
#           contracts_cursor, messages)
#       if the result is ok then the property is true

# an expressions defines :
#   a symbolic address
#   a value or an interval of value (or more - see domsec)
#       note for developer (properly define existential or universal quantification)
#           in precondition default interpretation is universal quantification
#           in post-condition default interpretation is existential quantification
#   a constraint on the memory (thanks to the comparison operators)
#       like (*(r1+3))_32 == [1_32, 128_32]S
class Expression(object):
    def __init__(self, content: str):
        self.content = content

# We need to define memory zones to limit the size of the contracts
#   For example we need to express the content of a zone is left unchanged
#       or we do not care about the content of a zone.
#   Zones are in a finite set and the do not overlap by nature (partition
#       of the memory); they are symbolically defined: ex zone above the
#       stack pointer even if the value of the stack pointer depends of the
#       context.
#
#   Since the number of MemoryZones is finite, their management is like
#       register allocation in compilation. The rename function implements
#       phi-function that is used in SSA (Single Static Assignment). The
#       split and merge functions are 
class MemoryZone(object):
    def __init__(self, name: str, start: Expression, length: Expression):
        self.start = start
        self.length = length

    def rename(new_name):
        self.name = new_name

    def split(new_start, new_name): # -> MemoryZone
        return None

    def merge(source): # : MemoryZone
        return None

    def is_equal(source) -> bool: # : MemoryZone
        return False

# A memory model is required to interact with the check_block
# It is currently hidden behind the check_block implementation
#   since the contracts are provided.
# It could be exposed in the future.

# Contracts are always before or after an instruction. They are
#   properties over the memory at that point.

class ContractLocalization(Enum):
    BeforeInstruction : 0
    AfterInstruction  : 1
    BetweenInstruction: 2

# Contracts are organized as a (dominance) tree to avoid huge contract
# If the linear block iterator iterates by respecting the dominance tree,
#       the cursor should point to the inherited contract or to one of its
#       successors, which enables to find quickly the memory of the
#       inherited contract.

class Contract(object):
    def __init__(self):
        self.nexts = []         # next contracts - a final contract has no next
        self.previouses = []    # previous contracts - the initial contract has no previous
        self.dominator = None   # inherited contract = dominator from previous contract
        self.address = 0
        self.additional_address = 0
        self.localization = ContractLocalization.BeforeInstruction
        self.zone_modifier = [] # actions rename, split, merge
        self.properties = []    # list of expressions = constraints over the memory
        # implicit quantification is universal when interpreted as pre-condition
        # implicit quantification is existential when interpreted as post-condition

    def add_condition(self, condition):
        return None

    # see load_contracts to build a new contract

class Warning(object):
    def __init__(self, text: str):
        self.text = text

# The architecture is the API of armsec
class Architecture(object):
    def __init__(self, text: str):
        self.content = None
        self.method_table = []
    # expose the method interpret which is called by check_block

# The arithmetic is the API of domsec
class Arithmetic(object):
    def __init__(self, text: str):
        self.content = None
        self.method_table = []
    # internally used - the method_table of arithmetic is sent as call-back
    #   functions for the architecture methods

# The main API starts here
def set_architecture(dllname: str) -> Architecture :
    return None

def set_arithmetic(dllname: str) -> Arithmetic :
    return None

# parser of contracts
# It build the graph of contracts (and so its coverage over the code)
# post-condition: the graph is connex, has only a start contract and it has
#   final contracts. Every node in the graph should be correctly connected
#   (consistency of the fields nexts, previouses and dominator)
def load_contracts(meta_data_file, architecture, arithmetic
        ): # dictionary of Contract indexed by uint64 addresses
    return []

# the main function to check contracts
# it will use memsec functions
def check_block(block, length_block, address_block,
            contracts, contracts_cursor, warnings,
            architecture, arithmetic
        ) -> bool:
    # for each target at the end of the block (branch, dynamic calls,
    #       dynamic jumps should be at last position in the block
    #   look for the pre-condition/post-condition contract in contracts
    #       we may need the function call tree & the control flow graph
    #       [TODO] store such partial information in the Contract class
    #   create a MemoryModel named mem for the pre-condition contract and the
    #       "previous" memory model attached to the inherited contract
    #   for each instruction in the block
    #       call mem = architecture.interpret(mem)
    #   create a MemoryModel named post_mem for the post-condition contract
    #       and the memory model attached to its inherited contract
    #   return mem.is_included_in(post_mem)
    return False

