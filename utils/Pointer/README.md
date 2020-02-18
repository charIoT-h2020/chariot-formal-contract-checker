The Pointer Classes
===================

This library collects all the classes of simple pointers. It contains
three units: base pointers, intelligent pointers and decision tree.
The base pointers only care about allocation whereas more intelligent
pointers care about reactive objects and/or concepts like “copy on
modification”. These sets of pointers enable to define tree-like
structures. For graph-like structure, additional concepts are
required since no graph normalization is provided. The last unit
decision tree is useful to define evolved objects, like agents able
to progressively decode a request on them and to handle it with the
appropriate answer.

The base pointers are very simple. They only care about the
allocation and the automatic destruction of objects. This simple
mechanism enables us to forget the try…finally construction, since
destructors are called even if exceptions are thrown. The class
`AutoPointer` and `PassPointer` are such basic pointers, with copy
semantics for `AutoPointer` and move semantics for `PassPointer`. They
both rely on the virtual methods `EnhancedObject::~EnhancedObject` and
`EnhancedObject::clone`. `PassPointer` is similar in its use to
`std::unique_ptr`.

The class `MngPointer` embeds a reference counter and it is similar to
the class `std::shared_ptr`. In fact, it is not so often used and be
careful to the potential recursive destruction. The next evolution of
`MngPointer` is the class `SharedPointer`. It explicitly knows the other
pointers that point onto the same object. This class enables
notifications to the other pointers each time an object is modified
through one SharedPointer. As they use collections, they are in the
unit Intelligent Pointers.

The Intelligent Pointers require basic collection implementation for
its internal use. The collection implementation defines data
structures and basic algorithms to provide services for collections.
Such implementation will then be used as implementation to realize
the whole hierarchic construction deriving from `COL::VirtualCollection`.
This implementation unit provides double linked lists, arrays with
sort algorithms, AVL, trees. Such implementations have no cursors but
cursors can be implemented on them. This unit is in the `COL` namespace.
