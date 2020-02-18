The Collection Classes
======================

The collection library aims to provide many collections through one
interface dedicated on the functionalities of the collection. Despite
the containers of STL, all collections are defined within a hierarchy
whose common ancestor is VirtualCollection. This enables cast between
collections with different types of elements. It is also possible to
change one implementation by another one with a minimum of changes. A
further optimization consists in suppressing indirections like in
replacing some collections by their implementation.

The collections naturally support the concept of cursor.
`AbstractCollection` introduces them as `AbstractCollectionCursor`.
This documentation makes distinction between the notion of pointer,
of cursor and of iterator.

* The pointers are at the lower level. The simplest implementation
  consists in defining them as pointers on the elements (see
  `PNT::Pointer`). Some collections are able to translate pointers
  into cursors. It is the case of collections whose cursors provide
  the `gotoReference` method. By default, this method is not present.
* The cursors define a position in the collection. They can move
  forward or move backward. If an insertion occurs on the collection,
  the previously defined cursors remain valid and they still point
  onto the same position. The suppressions have the same behavior on
  cursors, except for the cursors that pointed onto the suppressed
  element. In that case, those cursors become invalid, that means
  that their position is then before the first element (or after the
  last one). In the general case, there are no more than two cursors
  on an existing collection. To be updated on modifications of the
  collection, they are registered in a double linked list.
* The iterators define a way to go along the collection. With C++-11,
  C++-14, they are mainly implemented through `foreachDo` methods.
  The method `VirtualCollection::foreachDo` uses
  `VirtualCollection::Cursor` to provide iterations. But in the
  general cases, iterators are not registered and the collections
  should not be modified while iterators are doing their job.

The collection unit `COL` defines 3 kinds of collections deriving
from `VirtualCollection`: the native ones, the wrappers and
collections supporting multi-threading. The native collections are
the most efficient ones and they simply inherit from
`VirtualCollection`. They use specific implementations which may be
even more efficient if the user directly calls the methods of the
implementation. The wrapper collections are useful to provide a typed
interface for `VirtualCollection` with different implementations
provided. The threaded collections are available for different
threads that have to gain a read and/or a write access to parts of
the collection.

Most of the collections defined in this library are optimized thanks
to a high knowledge of the elements in the collection. In practice,
each element carries some kind of linkage to quickly find its
neighbors. As example, we can cite the collections `TList`,
`TSortedAVL`, `TSortedParentAVL`, `Tree`, `TreeParent`. `TList`,
`TSortedParentAVL`, `TreeParent` provides localization function from
pointers at constant cost. This is different from the STL where each
container has no idea of the elements that will be stored inside. The
elaboration of such collection is a bit more complex since it imposes
some kind of inheritance for elements: As example, using the lists
requires first to define the elements `MyListElement` as a derived
class of `List::Node` and second to create the template instance
`TList<MyListElement>`. This way and the multiple inheritance support
enable to create spare matrices just with lists. Note that the
collections like `TArray`, `SortedArray`, `TSortedArray`, `TBasic...`
do not require any inheritance relations for their elements.

Here are some distinctive features of this library with respect to
the containers of the STL:

* All our collections derive from `VirtualCollection`.
  `VirtualCollection` defines many generic methods that enable to
  hide the implementation.
* It is easy to redefine new collections, because the user only to
  connect ten virtual methods to an actual implementation. Such
  methods are identified by a `_` as first character.
* The modifications on the collections automatically update the
  cursors.
* Our collections do not have differentiation and intersection
  algorithms.
* Our collections are likely to require some kind of inheritance on
  the elements. `List`, sorted collections, trees use this mechanism.
  It is powerful enough to make `TList` the most used collection with
  insertions, registrations and suppressions done in constant time.
* The STL let the user deciding of the semantic whereas our
  collections are either in ownership semantic or in share semantic
  or in reference semantic.
* The code is shared between the collections (AVL and ternary trees
  have the same bases). Typing is done by a template layer at the
  last level.
