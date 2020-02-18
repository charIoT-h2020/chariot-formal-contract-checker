processor = Processor(".../libcontract_checker.so", "...//armsec_decoder.so", ".../libScalarInterface.so")

contracts = Contracts(processor)
contracts.load_from_file(args.contracts, processor)

contract_cursor = ContractCursor()
contract_cursor.init(contracts)
coverage = ContractCoverage(contracts) # Directed Acyclic Graph with domination & marked segments
processor.load_code(firmware_file)

first_contract = ContractReference()
last_contract = ContractReference()
# the contracts covers the firmware execution from the address of first_contract
#   to the address of last_contract

# check all the contracts defined in the meta-data (for every function)
contract_cursor = ContractCursor(contracts)
for block in "set of linear blocks":
    # do security engine job

    # a linear block finishes with a jump, a branch, a call instruction
    #   (or a standard instruction if the next instruction is the target of jump, branches).
    # hence a linear block may have multiple targets
    start_address = block.get_address()
    contract_cursor.set_before_address(start_address)
    if not contract_cursor.is_valid():
        continue # no contract

    if contract_cursor.is_initial():
        first_contract = contract_cursor.get_contract()
    # block.get_targets() should be equal to processor.get_targets(start_address, contract_cursor.get_contract())
    #   or before (on a standard instruction whose next instruction is the target of a jump, branch).
    for target in block.get_targets():
        post_contract_cursor = contract_cursor
        post_contract_cursor.set_after_address(target.address)

        if post_contract_cursor.is_final():
           last_contract = post_contract_cursor.get_contract();
        assert post_contract_cursor.get_address() == target.address # for the time being

        warnings = Warnings(processor)
        if not processor.check_block(start_address, target.address,
                contract_cursor.get_contract(), post_contract_cursor.get_contract(),
                coverage, warnings):
            for warning in warnings.as_list():
                print (warning)

if not first_contract.is_valid:
    print ("no initial contract found")
    return

if not last_contract.is_valid:
    print ("no final contract found")
    return

# check for if the DAG of contracts has been covered
if coverage.is_complete(first_contract, last_contract):
    print ("contract coverage fully verified")
else:
    print ("incomplete contract verification")

# check for a property generated by the Security Engine
if ...has_property:
    contract = Contract(...get_property())
    contract_cursor.set_before_address(contract.get_address())
    warnings = Warnings(processor)
    if not processor.check_block(cursor.get_address(), contract.get_address(),
            contract_cursor.get_contract(), contract, None, warnings):
        for warning in warnings.as_list():
            print (warning)
        # property is not proved
    # else: property is proved

