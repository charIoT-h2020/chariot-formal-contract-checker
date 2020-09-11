import ctypes

class _PProcessor(ctypes.Structure): pass
class _DecisionVectorContent(ctypes.Structure): pass
class _ContractContent(ctypes.Structure): pass
class _ContractCoverageContent(ctypes.Structure): pass
class _WarningsContent(ctypes.Structure): pass
class _WarningCursorContent(ctypes.Structure): pass
class _ContractGraphContent(ctypes.Structure): pass
class _ContractCursorContent(ctypes.Structure): pass
class _ContractCoverageContent(ctypes.Structure): pass

class _TargetAddresses(ctypes.Structure):
    _fields_ = [("addresses", ctypes.POINTER(ctypes.c_uint64)),
                ("addresses_array_size", ctypes.c_int),
                ("addresses_array_length", ctypes.c_int),
                ("realloc_addresses", ctypes.CFUNCTYPE(ctypes.POINTER(ctypes.c_uint64),
                    ctypes.POINTER(ctypes.c_uint64), ctypes.c_int,
                    ctypes.POINTER(ctypes.c_int), ctypes.c_void_p)),
                ("address_container", ctypes.c_void_p)]

class _Warning(ctypes.Structure):
    _fields_ = [("filepos", ctypes.c_char_p),
                ("linepos", ctypes.c_int),
                ("columnpos", ctypes.c_int),
                ("message", ctypes.c_char_p)]

class ContractReference(object):
    def __init__(self):
        self.funs = None
        self.content = None

    def get_content(self):
        return self.content
    def is_valid(self):
        return not (self.content is None)

class DecisionVector(object):
    def __init__(self):
        self.funs = None
        self.content = None

    def set_from(self, processor):
        self.funs = processor.funs
        self.content = self.funs.processor_create_decision_vector(processor.content)
    def set_from_move(self, source):
        self.funs = source.funs
        self.content = source.content
        source.funs = None
        source.content = None
    def __del__(self):
        self.clear()
    def clear(self):
        if self.content:
            self.funs.processor_free_decision_vector(self.content)
            self.content = None
    def clone(self):
        result = DecisionVector()
        result.funs = self.funs
        result.content = self.funs.processor_clone_decision_vector(self.content)
        return result;

class Processor(object):
    def __init__(self, memsec_library: str, arch_library: str, dom_library: str):
        self.funs = ctypes.cdll.LoadLibrary(memsec_library)
        self.funs.create_processor.argtypes = [ ctypes.c_char_p, ctypes.c_char_p ]
        self.funs.create_processor.restype = ctypes.POINTER(_PProcessor)
        self.funs.free_processor.argtypes = [ ctypes.POINTER(_PProcessor) ]
        self.funs.processor_set_verbose.argtypes = [ ctypes.POINTER(_PProcessor) ]
        self.funs.processor_load_code.argtypes = [ ctypes.POINTER(_PProcessor), ctypes.c_char_p ]
        self.funs.processor_load_code.restype = ctypes.c_bool
        self.funs.processor_create_decision_vector.argtypes = [ ctypes.POINTER(_PProcessor) ]
        self.funs.processor_create_decision_vector.restype = ctypes.POINTER(_DecisionVectorContent)
        self.funs.processor_clone_decision_vector.argtypes = [
                ctypes.POINTER(_DecisionVectorContent) ]
        self.funs.processor_clone_decision_vector.restype = ctypes.POINTER(_DecisionVectorContent)
        self.funs.processor_free_decision_vector.argtypes = [
                ctypes.POINTER(_DecisionVectorContent) ]
        self.funs.processor_filter_decision_vector.argtypes = [
                ctypes.POINTER(_DecisionVectorContent), ctypes.c_uint64 ]
        self.funs.processor_get_targets.argtypes = [ ctypes.POINTER(_PProcessor),
            ctypes.c_uint64, ctypes.POINTER(_ContractContent),
            ctypes.POINTER(_DecisionVectorContent), ctypes.POINTER(_TargetAddresses) ]
        self.funs.processor_get_targets.restype = ctypes.c_bool
        self.funs.processor_set_loader_alloc_shift.argtypes = [ ctypes.POINTER(_PProcessor), ctypes.c_uint64 ]
        self.funs.processor_check_block.argtypes = [ ctypes.POINTER(_PProcessor),
            ctypes.c_uint64, ctypes.c_uint64, ctypes.POINTER(_ContractContent),
            ctypes.POINTER(_ContractContent), ctypes.POINTER(_DecisionVectorContent),
            ctypes.POINTER(_ContractCoverageContent), ctypes.POINTER(_WarningsContent) ]
        self.funs.processor_check_block.restype = ctypes.c_bool
        self.funs.load_contracts.argtypes = [ ctypes.c_char_p, ctypes.POINTER(_PProcessor),
                ctypes.POINTER(_WarningsContent) ]
        self.funs.load_contracts.restype = ctypes.POINTER(_ContractGraphContent)
        self.funs.contracts_has_alloc_shift.argtypes = [ ctypes.POINTER(_ContractGraphContent) ]
        self.funs.contracts_has_alloc_shift.restype = ctypes.c_bool
        self.funs.contracts_get_alloc_shift.argtypes = [ ctypes.POINTER(_ContractGraphContent) ]
        self.funs.contracts_get_alloc_shift.restype = ctypes.c_uint64
        self.funs.free_contracts.argtypes = [ ctypes.POINTER(_ContractGraphContent) ]
        self.funs.contract_fill_stop_addresses.argtypes = [ ctypes.POINTER(_ContractContent),
                ctypes.POINTER(_TargetAddresses) ]
        self.funs.contract_cursor_new.argtypes = [ ctypes.POINTER(_ContractGraphContent) ]
        self.funs.contract_cursor_new.restype = ctypes.POINTER(_ContractCursorContent)
        self.funs.contract_cursor_set_to_next.argtypes = [ ctypes.POINTER(_ContractCursorContent) ]
        self.funs.contract_cursor_set_to_next.restype = ctypes.c_bool
        self.funs.contract_cursor_is_initial.argtypes = [ ctypes.POINTER(_ContractCursorContent) ]
        self.funs.contract_cursor_is_initial.restype = ctypes.c_bool
        self.funs.contract_cursor_is_final.argtypes = [ ctypes.POINTER(_ContractCursorContent) ]
        self.funs.contract_cursor_is_final.restype = ctypes.c_bool
        self.funs.contract_cursor_set_address.argtypes = [ ctypes.POINTER(_ContractCursorContent),
            ctypes.c_uint64, ctypes.c_int ]
        self.funs.contract_cursor_set_address.restype = ctypes.c_bool
        self.funs.contract_cursor_get_address.argtypes = [ ctypes.POINTER(_ContractCursorContent) ]
        self.funs.contract_cursor_get_address.restype = ctypes.c_uint64
        self.funs.contract_cursor_clone.argtypes = [ ctypes.POINTER(_ContractCursorContent) ]
        self.funs.contract_cursor_clone.restype = ctypes.POINTER(_ContractCursorContent)
        self.funs.contract_cursor_free.argtypes = [ ctypes.POINTER(_ContractCursorContent) ]
        self.funs.contract_cursor_get_contract.argtypes = [ ctypes.POINTER(_ContractCursorContent) ]
        self.funs.contract_cursor_get_contract.restype = ctypes.POINTER(_ContractContent)
        self.funs.create_contract.argtypes = [ ctypes.c_char_p ]
        self.funs.create_contract.restype = ctypes.POINTER(_ContractContent)
        self.funs.free_contract.argtypes = [ ctypes.POINTER(_ContractContent) ]
        self.funs.contract_get_address.argtypes = [ ctypes.POINTER(_ContractContent) ]
        self.funs.contract_get_address.restype = ctypes.c_uint64
        self.funs.create_empty_coverage.argtypes = [ ctypes.POINTER(_ContractGraphContent) ]
        self.funs.create_empty_coverage.restype = ctypes.POINTER(_ContractCoverageContent)
        self.funs.free_coverage.argtypes = [ ctypes.POINTER(_ContractCoverageContent) ]
        self.funs.create_warnings.argtypes = [ ]
        self.funs.create_warnings.restype = ctypes.POINTER(_WarningsContent)
        self.funs.free_warnings.argtypes = [ ctypes.POINTER(_WarningsContent) ]
        self.funs.warning_create_cursor.argtypes = [ ctypes.POINTER(_WarningsContent) ]
        self.funs.warning_create_cursor.restype = ctypes.POINTER(_WarningCursorContent)
        self.funs.warning_free_cursor.argtypes = [ ctypes.POINTER(_WarningCursorContent) ]
        self.funs.warning_set_to_next.argtypes = [ ctypes.POINTER(_WarningCursorContent) ]
        self.funs.warning_set_to_next.restype = ctypes.c_bool
        self.funs.warning_retrieve_message.argtypes = [ ctypes.POINTER(_WarningCursorContent),
                ctypes.POINTER(_Warning)]
        self.funs.is_coverage_complete.argtypes = [ ctypes.POINTER(_ContractCoverageContent),
                ctypes.POINTER(_ContractContent), ctypes.POINTER(_ContractContent) ]
        self.funs.is_coverage_complete.restype = ctypes.c_bool
        self.funs.create_address_vector.argtypes = [ ]
        self.funs.create_address_vector.restype = _TargetAddresses
        self.funs.free_address_vector.argtypes = [ ctypes.POINTER(_TargetAddresses) ]
        self.funs.flush_cpp_stdout.argtypes = []
        self.content = self.funs.create_processor(arch_library.encode(), dom_library.encode())

    def __del__(self):
        self.clear()
    def clear(self):
        if self.content:
            self.funs.free_processor(self.content)
            self.content = None
    def set_verbose(self):
        if self.content:
            self.funs.processor_set_verbose(self.content)
    def flush_cpp_out(self):
        self.funs.flush_cpp_stdout()
    def load_code(self, filename : str) -> bool:
        return self.funs.processor_load_code(self.content, filename.encode())
    def retrieve_targets(self, address : ctypes.c_uint64, contract : ContractReference,
            decisions: DecisionVector, result: ctypes.POINTER(_TargetAddresses)):
        is_valid = self.funs.processor_get_targets(self.content, address, contract.content,
                decisions.content, result)
        if not is_valid:
            return [ ]
        assert is_valid
        index = 0
        res = [ ]
        while index < result.contents.addresses_array_length:
            res.append(result.contents.addresses[index])
            index = index+1
        return res
    def set_loader_alloc_shift(self, shift : ctypes.c_uint64):
        self.funs.processor_set_loader_alloc_shift(self.content, shift)

    # the main function to check contracts
    # it will use memsec functions
    def check_block(self, address : ctypes.c_uint64, target : ctypes.c_uint64,
            first_contract : ctypes.POINTER(_ContractContent),
            last_contract : ctypes.POINTER(_ContractContent),
            decisions: ctypes.POINTER(_DecisionVectorContent),
            coverage : ctypes.POINTER(_ContractCoverageContent),
            warnings : ctypes.POINTER(_WarningsContent)) -> bool:
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
        return self.funs.processor_check_block(self.content, address, target, first_contract,
                last_contract, decisions, coverage, warnings)

class Warnings(object):
    def __init__(self, processor : Processor):
        self.funs = processor.funs
        self.content = self.funs.create_warnings()
    def __del__(self):
        self.clear()
    def clear(self):
        if self.content:
            self.funs.free_warnings(self.content)
            self.content = None

class WarningCursor(object):
    def __init__(self, warnings : Warnings):
        self.funs = warnings.funs
        self.content = self.funs.warning_create_cursor(warnings.content)
    def __del__(self):
        self.clear()
    def clear(self):
        if self.content:
            self.funs.warning_free_cursor(self.content)
            self.content = None
    def set_to_next(self) -> bool:
        assert (self.content)
        return self.funs.warning_set_to_next(self.content)
    def element_at(self) -> _Warning:
        assert (self.content)
        warning = _Warning()
        self.funs.warning_retrieve_message(self.content, ctypes.pointer(warning))
        return warning

class Contracts(object):
    def __init__(self, processor : Processor):
        self.funs = processor.funs
        self.content = None
    def __del__(self):
        self.clear()
    def clear(self):
        if self.content:
            self.funs.free_contracts(self.content)
            self.content = None
    def has_alloc_shift(self) -> bool:
        if self.content:
            return self.funs.contracts_has_alloc_shift(self.content)
        return False
    def get_alloc_shift(self) -> ctypes.c_uint64:
        if self.content:
            return self.funs.contracts_get_alloc_shift(self.content)
        return ctypes.c_uint64(0)
    # parser of contracts
    # It build the graph of contracts (and so its coverage over the code)
    # post-condition: the graph is connex, has only a start contract and it has
    #   final contracts. Every node in the graph should be correctly connected
    #   (consistency of the fields nexts, previouses and dominator)
    def load_from_file(self, filename : str, processor : Processor, warnings : Warnings) -> bool:
        assert (not self.content)
        self.funs = processor.funs
        self.content = self.funs.load_contracts(filename.encode(), processor.content,
                warnings.content)
        return self.content

class ContractCursor(object):
    def __init__(self):
        self.funs = None
        self.content = None

    def init(self, contracts : Contracts):
        self.funs = contracts.funs
        self.content = self.funs.contract_cursor_new(contracts.content)
    def clone(self):
        result = ContractCursor()
        result.funs = self.funs
        result.content = self.funs.contract_cursor_clone(self.content)
        return result
    def __del__(self):
        self.clear()
    def clear(self):
        if self.content:
            self.funs.contract_cursor_free(self.content)
            self.content = None
    def set_to_next(self) -> bool:
        return self.funs.contract_cursor_set_to_next(self.content)
    def is_initial(self) -> bool:
        return self.funs.contract_cursor_is_initial(self.content)
    def is_final(self) -> bool:
        return self.funs.contract_cursor_is_final(self.content)
    def set_before_address(self, address : ctypes.c_uint64) -> bool:
        return self.funs.contract_cursor_set_address(self.content, address, 0)
    def set_after_address(self, address : ctypes.c_uint64) -> bool:
        return self.funs.contract_cursor_set_address(self.content, address, 1)
    def get_address(self) -> ctypes.c_uint64:
        return self.funs.contract_cursor_get_address(self.content)
    def get_contract(self) -> ContractReference:
        result = ContractReference()
        result.content = self.funs.contract_cursor_get_contract(self.content)
        return result
    def fill_stop_addresses(self, result : ctypes.POINTER(_TargetAddresses)):
        self.funs.contract_fill_stop_addresses(
                self.funs.contract_cursor_get_contract(self.content), result)

class Contract(object):
    def __init__(self):
        self.funs = None
        self.content = None
    def __del__(self):
        self.clear()
    def clear(self):
        if self.content:
            self.funs.free_contract(self.content)
            self.content = None
    def load_from_file(self, filename : str, processor : Processor) -> bool:
        self.funs = processor.funs
        self.content = funs.create_contract(filename.encode())
        return self.content
    def get_address(self) -> ctypes.c_uint64:
        return self.funs.contract_get_address(self.content)

class ContractCoverage(object):
    def __init__(self, reference : Contracts):
        self.funs = reference.funs
        self.content = self.funs.create_empty_coverage(reference.content)
    def __del__(self):
        self.clear()
    def clear(self):
        if self.content:
            self.funs.free_coverage(self.content)
            self.content = None
    def is_complete(self, first_contract : ctypes.POINTER(_ContractContent),
            last_contract : ctypes.POINTER(_ContractContent)) -> bool:
        return self.funs.is_coverage_complete(self.content, first_contract.content,
                last_contract.content)

