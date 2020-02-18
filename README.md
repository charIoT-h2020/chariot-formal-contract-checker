CharIoT formal contract checker
===============================

The formal contract checker defines an API to check formal contracts written
in JSON on binary file.

The API is [src/contract\_checker.py](src/contract_checker.py).

Its implementation is in the directory `src`. At time being, it is a C++
implementation, but a pure python implementation is expected for
maintenance issues (see [src/contract\_checker\_future.py](contract_checker_future.py).

At 18/02/2020, the following connections are still under implementations

- [x] implementation of domains `libScalarInterface.so`
    * [ ] customization coming from contracts should be provided
- [x] implementation of arm7 decoder (thumb) `armsec_decoder.so`
    * [x] memory state inference
    * [ ] [`Required`] correct computation of targets should be provided
    * [ ] JIT/optimization to be provided to avoid flag computations
- [ ] [`Required`] parsing formal contracts in json
    * [x] architecture - expressions, memory zones, memory states, contracts
    * [x] expression parsing
    * [ ] [`Required`] memory zone manipulation
    * [ ] [`Required`] memory zone parsing
    * [ ] [`Required`] memory state parsing
    * [ ] [`Required`] Directed Acyclic Graph of contracts - inheritance through dominance
    * [ ] [`Required`] contract parsing
    * [ ] [`Required`] contract graph parsing
- [ ] [`Required`] Full connection between the components
- [ ] [`Required`] Testing, Debugging
- [ ] [`Required`] Convincing demo
- [ ] Translation of `src` into python code
- [ ] Integration in the Security Engine

# Build the dynamic libraries required by the API

The 3 projects [chariot-h2020](https://gitlab.com/chariot-h2020) projects
[chariot-formal-decoder-armv7](https://gitlab.com/chariot-h2020/chariot-formal-decoder-armv7),
[chariot-semantic-domains](https://gitlab.com/chariot-h2020/chariot-semantic-domains),
[chariot-formal-contract-checker](https://gitlab.com/chariot-h2020/chariot-formal-contract-checker)
need to be cloned in the same directory.

```sh
mkdir chariot-h2020
cd chariot-h2020
git clone git@gitlab.com:chariot-h2020/chariot-formal-decoder-armv7.git
git clone git@gitlab.com:chariot-h2020/chariot-semantic-domains.git
git clone git@gitlab.com:chariot-h2020/chariot-formal-contract-checker.git
```

For the first installation, you can type

```sh
cd chariot-formal-contract-checker
./install.sh
```

Do not hesitate to modify this script for your our own needs.
It just creates a `build` directory in every project, then it calls
`cmake` to create `Makefiles`, then it calls `make` to build every
dynamic library required by the python API.

To update the current libraries with the last version, you can type

```sh
cd chariot-formal-contract-checker
./update.sh
```

It calls `git pull --ff-only` and then it calls `make` to rebuild every
dynamic library required by the python API.

# Demo of contract\_checker

In the build directory, the command

```sh
cd build
make test ARGS="-V"
```

runs the test [src/check\_contract.py]([src/check\_contract.py) on the file
`tests/modbus.nobug.bin` (you need to copy it in the directory tests)
with the contracts `tests/contracts.json`.
The result `ok` is then printed on the standard output.

It calls the command
```sh
../src/check_contract.py -arch ../../chariot-formal-decoder-armv7/src/armsec_decoder.so -dom ../../chariot-semantic-domains/build/libapplications/libScalarInterface.so ../tests/modbus.nobug.bin ../tests/contracts.json
```

Note that `check_contract.py` looks for `libcontract_checker.so` in the
current directory. This should be changed to be tested outside the build
directory.

# Integration in the Security Engine

The Security Engine has its own definition of linear blocs of instructions.
It should be more or less the same than the one of `formal-contract-checker`.

A linear bloc can start with any instruction. It is preferably

* a *label* instruction that is the target of a *jump* instruction,
* or the first function instruction
* or the instruction after a function call

A linear bloc can end with any instruction. It is preferably

* a *branch* instruction
* a *call* instruction
* a *computed-jump* instruction

Every instruction between the start instruction and the end instruction
(excluding the end one) has only one target, preferably its next instruction.

From these hypotheses, the integration in the Security Engine can
look at the file [src/check\_contract.py](src/check_contract.py)
or at the template file [tests/integration.py](tests/integration.py).

In [src/check\_contract.py](src/check_contract.py), the loop that iterates
over the contracts should be changed to iterate over the linear blocs.

