project(utils)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(../utils)

set(SOURCES
   ../utils/StandardClasses/StandardClasses.cpp
   ../utils/StandardClasses/Persistence.cpp
   ../utils/Pointer/ImplList.cpp
   ../utils/Pointer/ImplArray.cpp
   ../utils/Pointer/Pointer.cpp
   ../utils/Pointer/MngPointer.cpp
   ../utils/Pointer/SharedPointer.cpp
   ../utils/Pointer/Binary.cpp
   ../utils/Pointer/SharedCollection.cpp
   ../utils/Collection/Implementation/BinaryParent.cpp
   ../utils/Collection/VirtualCollection/VirtualCollection.cpp
   ../utils/Collection/VirtualCollection/VirtualTree.cpp
   ../utils/Collection/ConcreteCollection/Array.cpp
   ../utils/Collection/ConcreteCollection/List.cpp
   ../utils/Collection/ConcreteCollection/BasicList.cpp
   ../utils/Collection/Implementation/ImplTree.cpp
   ../utils/Collection/ConcreteCollection/Special.cpp
   ../utils/Collection/ConcreteCollection/SortedArray.cpp
   ../utils/Collection/ConcreteCollection/SortedAVL.cpp
   ../utils/Collection/ConcreteCollection/BasicSortedAVL.cpp
   ../utils/Collection/ConcreteCollection/MultiArray.cpp
   ../utils/Collection/ConcreteCollection/Tree.cpp
   ../utils/TString/StringRep.cpp
   ../utils/TString/SetOfChars.cpp
   ../utils/TString/SubString.cpp
   ../utils/TString/SString.cpp
   ../utils/TString/TernaryTree.cpp
   ../utils/TString/TernaryTreeParent.cpp
   ../utils/TString/TernaryTreeInterfaceBase.cpp
   ../utils/TString/TernaryTreeInterface.cpp
   ../utils/TString/TernaryTreeParentInterface.cpp
   ../utils/JSON/JSonLexer.cpp
   ../utils/JSON/JSonParser.cpp
   )

add_library(utils ${SOURCES})
set_property(TARGET utils PROPERTY POSITION_INDEPENDENT_CODE ON)

