#!/usr/bin/python3
import sys
import argparse
from contract_checker import *

parser = argparse.ArgumentParser(description='check contract over binary firmware')
parser.add_argument('binary_file', help='the firmware to analyze')
parser.add_argument('contracts', help='the contracts to check in json format')
parser.add_argument('-arch', nargs=1,
                   help='the instruction set as a dynamic library')
parser.add_argument('-dom', nargs=1,
                   help='the library of domains as a dynamic library')
parser.add_argument('-v', '--verbose', help='set verbose mode',
                    action='store_true')
parser.add_argument('-prop', nargs=1,
                   help='the additional properties to check')
args = parser.parse_args()

if args.arch is None:
    args.arch = "./armsec_decoder.so"
else:
    args.arch = args.arch[0]
if args.dom is None:
    args.dom = "./libScalarInterface.so"
else:
    args.dom = args.dom[0]

processor = Processor("./libcontract_checker.so", args.arch, args.dom)
if args.verbose:
    processor.set_verbose()

contracts = Contracts(processor)
warnings = Warnings(processor)
if not contracts.load_from_file(args.contracts, processor, warnings):
    warning_cursor = _WarningCursor(warnings)
    print ("unable to load contracts from file " + args.contracts)
    while warning_cursor.set_to_next():
        warning = warning_cursor.element_at()
        print ("at " + warning.filepos + ":" + warning.linepos
                + " error at column " + error.columnpos + ", " + error.message)
    sys.exit(0)
if args.verbose:
    print ("contracts in " + args.contracts + " successfully loaded", flush=True)

contract_cursor = ContractCursor()
contract_cursor.init(contracts)
coverage = ContractCoverage(contracts) # Directed Acyclic Graph with domination & marked segments
if not processor.load_code(args.binary_file):
    print ("unable to load code from file " + args.binary_file)
    sys.exit(0)
if args.verbose:
    print ("code in " + args.binary_file + " successfully loaded", flush=True)

first_contract = ContractReference()
last_contract = ContractReference()
# the contracts covers the firmware execution from the address of first_contract
#   to the address of last_contract

# check all the contracts defined in the meta-data (for every function)
all_valid = True
while contract_cursor.set_to_next():
    # do security engine job

    # a linear block finishes with a jump, a branch, a call instruction
    #   (or a standard instruction if the next instruction is the target of jump, branches).
    # hence a linear block may have multiple targets
    address = contract_cursor.get_address()
    if contract_cursor.is_initial():
        first_contract = contract_cursor.get_contract()
    if contract_cursor.is_final():
        last_contract = contract_cursor.get_contract()
        continue
    targets = contract_cursor.funs.create_address_vector()
    contract_cursor.fill_stop_addresses(ctypes.pointer(targets))
    if args.verbose:
        print ("look for targets starting at 0x" + hex(address) + " by instruction interpretation", flush=True)
    decisions = DecisionVector()
    decisions.set_from(processor)
    processor.retrieve_targets(address, contract_cursor.get_contract(),
            decisions, ctypes.pointer(targets))
    processor.flush_cpp_out()
    target_index = 0
    while target_index < targets.addresses_array_length:
        target = targets.addresses[target_index]
        post_contract_cursor = contract_cursor.clone()
        post_contract_cursor.set_after_address(target)
        assert (post_contract_cursor.get_address() == target)
        warnings = Warnings(processor)

        if args.verbose:
            print ("check block starting at 0x" + hex(address) + " by instruction interpretation", flush=True)
        decisions_target = decisions.clone()
        if not processor.check_block(address, target, contract_cursor.get_contract().content,
                post_contract_cursor.get_contract().content,
                decisions_target.content, coverage.content, warnings.content):
            processor.flush_cpp_out()
            print ("unable to check block starting at 0x" + hex(address))
            warning_cursor = _WarningCursor(warnings)
            while warning_cursor.set_to_next():
                warning = warning_cursor.element_at()
                print ("at " + warning.filepos + ":" + warning.linepos
                        + " error at column " + error.columnpos + ", " + error.message);
            all_valid = False
        else:
            processor.flush_cpp_out()
        decisions_target.clear()
        target_index = target_index + 1
    targets = contract_cursor.funs.free_address_vector(targets)
    decisions.clear()

    if not first_contract.is_valid:
        print ("no initial contract found")

    if not last_contract.is_valid:
        print ("no final contract found")

    # check for if the DAG of contracts has been covered
    if coverage.is_complete(first_contract, last_contract):
        print ("contract coverage fully verified", flush=True)
    else:
        print ("incomplete contract verification", flush=True)

    # check for a property generated by the Security Engine
    if args.prop:
        contract = Contract(processor)
        contract.load_from_file(args.prop)
        contract_cursor.set_before_address(contract.get_address())

        warnings = Warnings(processor)
        decisions = DecisionVector()
        decisions.set_from(processor)
        targets = contract_cursor.funs.create_address_vector()
        processor.retrieve_targets(contract_cursor.get_address(), contract_cursor.get_contract(),
                decisions, ctypes.pointer(targets))
        processor.flush_cpp_out()
        targets = contract_cursor.funs.free_address_vector(targets)
        if not processor.check_block(contract_cursor.get_address(), contract.get_address(),
                contract_cursor.get_contract().content, contract.content, decisions.content,
                None, warnings.content):
            processor.flush_cpp_out()
            for warning in warnings:
                print (warning)
            print ("property is not proved", flush=True)
        else:
            processor.flush_cpp_out()
            print ("property is proved", flush=True)
        decisions.clear()
if all_valid:
    print ("all contracts have been verified!")
else:
    print ("some contracts have failed!")
