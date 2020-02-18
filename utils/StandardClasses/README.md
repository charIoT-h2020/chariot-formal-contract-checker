The Standard Classes
====================

It is a shared piece of code used as a submodule of many projects.
It should be placed in a repository `utils/StandardClasses`.

The main class of this library is the class `EnhancedObject`. It is
likely to be inherited by any object whose derived classes are not
all known.

It defines copy, assignment and comparison methods. Some of these
methods may not be useful, but the macro `DefineCopy`,
`DDefineAssign`, `DCompare` enables their automatic definition. This
macro should be automatically used in every derived class of
`EnhancedObject`.

Static inheritance and virtual inheritance is supported thanks to the
static methods EnhancedObject::castToCopyHandler. The macro
StaticInheritConversions automatically identifies the copy and
assignment handlers. Some inspiration for this construction comes
from "Taming C++", Jiri Soukup.

The inherited class `STG::IOObject` defines the persistence methods.
From the mathematical point of view, we could add a merge method, but
multiple inheritance is often annoyed by the multiplicity of such
virtual methods.

The class `ESPreconditionError` introduces software errors that are
contract violations. Besides this class this unit introduces other error
classes, like the class `STG::EReadError` for the parsing errors.

