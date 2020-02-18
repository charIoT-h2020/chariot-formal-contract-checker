The TString library
===================

The library `TString` defines the concept of sub-string that
shares a common support with other sub-string. It contains
high-level methods to manipulate the sub-strings and their
support. It contains several units, like

* the support unit: the classes `TRepository` and
  `TListRepository` define how a sequence of chars is stored
  (a vector or a list of vectors).
* the set-of-chars unit: the classes `SetOfChars` and
  `TAVLBasedCharSet` define different ways to represent set of chars.
  They are use for search methods and to represent transitions
  in the automata of the `Regexp` library.
* the sub-string unit: the classes `TSubString<char>` and
  `TString<char>` define sub-strings and strings. They are widely
  used for persistence.
* the dictionary unit: the classes `ImplTernaryTree`,
  `ImplParentTernaryTree` implement ternary trees for efficient
  queries on the dictionary. `std::unordered_map` is also an
  efficient alternative based on hash maps.

This library is in connection with

* the library `Pointer` and the class `Parser::TStateStack`
  for persistence,
* the libraries `JSON` and `XML`,
* the library `Regexp` with the classes `TDeterministicAutomaton`
  and `TPassAutomaton` for search based on regular expressions.

The main classes of these libraries are `TSubString` and `TString`
defined in the files `SubString.h` and `SString.h`.
