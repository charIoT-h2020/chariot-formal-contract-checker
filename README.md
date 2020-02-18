CharIoT formal contract checker
===============================

The formal contract checker defines an API to check formal contracts written
in JSON on binary file.

The API is `contract_checker.py`.

Its implementation is in the directory `src`. At time being, it is a C++
implementation, but a pure python implementation is expected for
maintenance issues.

A demonstrator is available in the [test](test) directory.

# build contract\_checker.so

You first need to go in the `chariot-formal-contract-checker` directory.

Then the standard `cmake` commands should build the executable `domsec`.

```sh
mkdir build
cd build
cmake ..
make
# make -j 8
```

# demo of contract\_checker

In the build directory, the command

```sh
make test ARGS="-V"
```

runs the unit-test `contract_check` on the file `tests/PreContract.json`
and `tests/PostContract.json`.
The result `ok` is then printed on the standard output.
Any unsound modification in one of the contracts should issue the result
`ko` on the standard output.


