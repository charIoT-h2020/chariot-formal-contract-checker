/**************************************************************************/
/*                                                                        */
/*  Copyright (C) 2013-2019                                               */
/*    CEA (Commissariat a l'Energie Atomique et aux Energies              */
/*         Alternatives)                                                  */
/*                                                                        */
/*  you can redistribute it and/or modify it under the terms of the GNU   */
/*  Lesser General Public License as published by the Free Software       */
/*  Foundation, version 2.1.                                              */
/*                                                                        */
/*  It is distributed in the hope that it will be useful,                 */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of        */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         */
/*  GNU Lesser General Public License for more details.                   */
/*                                                                        */
/*  See the GNU Lesser General Public License version 2.1                 */
/*  for more details (enclosed in the file LICENSE).                      */
/*                                                                        */
/**************************************************************************/

/////////////////////////////////
//
// Library   : TString
// Unit      : sub-string
// File      : SetOfChars.h
// Description :
//   Definition of the interface for the set of chars.
//

#ifndef STG_SetOfCharsH
#define STG_SetOfCharsH

#include "StandardClasses/Persistence.h"
#include "Pointer/LinearCollection.hpp"

namespace STG {

class ComparisonMode : protected ExtendedParameters {
  protected:
   DefineExtendedParameters(1, ExtendedParameters)

  public:
   ComparisonMode() {}
   ComparisonMode(const ComparisonMode& source) = default;

   ComparisonMode& setCaseSensitive() { clearOwnField(); return *this; }
   ComparisonMode& setCaseInsensitive() { mergeOwnField(1); return *this; }
   ComparisonMode& setComparisonMode(const ComparisonMode& source)
      { AssumeCondition(!hasOwnField()) mergeOwnField(source.queryOwnField()); return *this; }
   bool isCaseSensitive() const { return !hasOwnField(); }
   bool isCaseInsensitive() const { return hasOwnField(); }
};

class Direction : protected ExtendedParameters {
  protected:
   DefineExtendedParameters(1, ExtendedParameters)

  public:
   Direction() {}
   Direction(const Direction& source) = default;

   bool isForward() const { return !hasOwnField(); }
   bool isBackward() const { return hasOwnField(); }
   
   Direction& setForward() { clearOwnField(); return *this; }
   Direction& setBackward() { mergeOwnField(1); return *this; }
   Direction& setInverse() { setOwnField(1-queryOwnField()); return *this; }
};

namespace DCharsSet {

class Access : public STG::Lexer::Base {
  public:
   template <typename TypeChar, class TypeSubString>
   class TCharParser {
     private:
      TypeChar cReadChar;
      
     public:
      TCharParser() : cReadChar() {}
      ReadResult readChar(TypeSubString& subString);
      enum Special {
         SNormal, SAny, SNot, SNewLine, SMinus, SOpenBracket, SCloseBracket, SOpenBrace,
         SCloseBrace, SOr, SEnd
      };
      bool isSpecial() const           { return false; }
      bool isNegation() const          { AssumeUncalled return false; }
      bool isOpenBracket() const       { AssumeUncalled return false; }
      bool isBeginOfLine() const       { AssumeUncalled return false; }
      bool isEndOfLine() const         { AssumeUncalled return false; }
      bool isAll() const               { AssumeUncalled return false; }
      bool isIntervalDelimiter() const { AssumeUncalled return false; }
      bool isCloseBracket() const      { AssumeUncalled return false; }
      bool isDisjunction() const       { AssumeUncalled return false; }

      const TypeChar& getReadChar() const { return cReadChar; }
   };

   template <class TypeSubString>
   class TSpecialCharParser {
     private:
      char cReadChar;
      bool fSpecial;

     public:
      TSpecialCharParser() : cReadChar('\0'), fSpecial(false) {}
      ReadResult readChar(TypeSubString& subString);
      bool isSpecial() const           { return fSpecial; }
      bool isNegation() const          { return cReadChar == '!'; }
      bool isOpenBracket() const       { return cReadChar == '['; }
      bool isBeginOfLine() const       { return cReadChar == '@'; }
      bool isEndOfLine() const         { return cReadChar == '^'; }
      bool isAll() const               { return cReadChar == '?'; }
      bool isIntervalDelimiter() const { return cReadChar == '-'; }
      bool isCloseBracket() const      { return cReadChar == ']'; }
      bool isDisjunction() const       { return cReadChar == '|'; }

      const char& getReadChar() const { return cReadChar; }
   };

   template <class TypeSet, typename TypeChar, class TypeSubString,
      class TypeCharParser=Access::TCharParser<TypeChar, TypeSubString> >
   class TParser : protected ExtendedParameters {
     private:
      typedef ExtendedParameters inherited;
      enum State
         {  SInitial, SSimple, SOpenBracket, SAfterOpenBracketAtom, SAtomDisjunction,
            SDisjunction
         };
      enum SimpleState
         {  SSInitial, SSAfterInitial, SSOpenBracket, SSInterval, SSMaxInterval, SSCloseBracket };

      TypeCharParser tcpCharParser;
      TypeChar tcMin, tcMax;

      ReadResult addSimp(TypeSet& set, TypeSubString& subString, const ComparisonMode& mode);
      
     protected:
      DefineExtendedParameters(6, ExtendedParameters)
      DefineSubExtendedParameters(State, 2, INHERITED)
      DefineSubExtendedParameters(SimpleState, 3, State)
      DefineSubExtendedParameters(Exclude, 1, SimpleState)

     public:
      TParser() : tcMin((TypeChar) 0), tcMax((TypeChar) 0) {}
      ReadResult parse(TypeSet& set, TypeSubString& subString, const ComparisonMode& mode);
   };
};

template <typename TypeChar, class TypeSubString>
inline Access::ReadResult
Access::TCharParser<TypeChar, TypeSubString>::readChar(TypeSubString& subString) {
   ReadResult result = RRNeedChars;
   if (sizeof(typename TypeSubString::Char)*subString.length() >= sizeof(TypeChar)) {
      result = RRHasToken;
      TChunk<TypeChar> chunk = subString.getChunk();
      if (sizeof(typename TypeSubString::Char)*chunk.length >= sizeof(TypeChar))
         memcpy(&cReadChar, chunk.string, sizeof(TypeChar));
      else {
         int minChars = (sizeof(TypeChar)-1) / sizeof(typename TypeSubString::Char) + 1;
         typename TypeSubString::Char* temp = new typename TypeSubString::Char[minChars];
         AssumeAllocation(temp)
         subString.copyTo(temp, minChars);
         memcpy(&cReadChar, temp, sizeof(TypeChar));
         delete [] temp;
      };
   };
   return result;
}

} // end of namespace DCharsSet

/**************************************/
/* Definition of the class SetOfChars */
/**************************************/

class SetOfChars : public DCharsSet::Access {
  private:
   static const int ByteSize = 256 / 8;
   unsigned int acTable[(ByteSize-1)/sizeof(unsigned int)+1];
   class Special {};
   SetOfChars(Special);

  protected:
   void _addInterval(char lower, char upper);
   void _retrieveIntervalAround(char achar, char& min, char& max, bool isNegative) const; 
  
  public:
   static SetOfChars special;

   SetOfChars();
   SetOfChars(const SetOfChars& source) { memcpy(acTable, source.acTable, ByteSize); }
   SetOfChars& operator=(const SetOfChars& source)
      {  if (&source != this) memcpy(acTable, source.acTable, ByteSize); return *this; }

   SetOfChars& add(const char& chadd)
      {  acTable[((unsigned char) chadd) / (8*sizeof(unsigned int))]
            |= (unsigned int) (1 << (((unsigned char) chadd) % (8*sizeof(unsigned int))));
         return *this;
      }
   SetOfChars& add(const SetOfChars& source);
   SetOfChars& addNegation(const SetOfChars& source);
   SetOfChars& addInterval(char lower, char upper)
      {  if ((lower < 0) && (upper >= 0)) {
            _addInterval(lower, (char) 0xff);
            _addInterval(0x00, upper);
         }
         else
            _addInterval(lower, upper);
         return *this;
      }
   SetOfChars& addAll();
   SetOfChars& addBLine() { add(10); return *this; }
   SetOfChars& addELine() { add(13); return *this; }
   SetOfChars& addNot(const char& achar)
      {  if (accept(achar))
            addAll();
         else {
            addAll();
            remove(achar);
         };
         return *this;
      }

   SetOfChars& add(char chadd, const ComparisonMode& mode);
   SetOfChars& addInterval(char lower, char upper, const ComparisonMode& mode);
   SetOfChars& addNot(char achar, const ComparisonMode& mode);

   SetOfChars& remove(const char& achar)
      {  acTable[((unsigned char) achar) / (8*sizeof(unsigned int))]
            &= ~(1U << (((unsigned char) achar) % (8*sizeof(unsigned int))));
         return *this;
      }
   SetOfChars& clear();
   SetOfChars& remove(char achar, const ComparisonMode& mode);

   SetOfChars& negate();
   SetOfChars& remove(const SetOfChars& source);
   SetOfChars& mergeWith(const SetOfChars& source) { return add(source); }
   SetOfChars& intersectWith(const SetOfChars& source);
   const SetOfChars& separateWith(const SetOfChars& source,
      SetOfChars& diff1, SetOfChars& intersect, SetOfChars& diff2) const;

   SetOfChars& insensitive();

   /* get methods */
   bool accept(const char& achar) const
      {  return acTable[((unsigned char) achar) / (8*sizeof(unsigned int))]
            & (1U << (((unsigned char) achar) % (8*sizeof(unsigned int))));
      }
   bool isAny() const
      {  int index = ByteSize / sizeof(unsigned int);
         while ((index != 0) && (acTable[--index] == ~0U));
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
#endif
         return (index == 0) && ((ByteSize % sizeof(unsigned int) == 0)
            || (acTable[ByteSize / sizeof(unsigned int)] == ~(~0U << (ByteSize % sizeof(unsigned int)))));
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
      }
   bool isEmpty() const
      {  int index = ByteSize / sizeof(unsigned int);
         while ((index != 0) && (acTable[--index] == 0U));
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
#endif
         return (index == 0) && ((ByteSize % sizeof(unsigned int) == 0) || (acTable[ByteSize / sizeof(unsigned int)] == 0));
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
      }
   bool contain(const SetOfChars& source) const;
   bool isIntersectionEmpty(const SetOfChars& source) const;
   int queryCount() const;
   void retrieveIntervalAround(char achar, char& min, char& max, bool isNegative=false) const;
   void retrieveNegativeIntervalAround(char achar, char& min, char& max) const
      {  retrieveIntervalAround(achar, min, max, true); }

   /* persistence methods */
   class SubString;
   void read(SubString& buffer, const ComparisonMode& mode);
   void prettyPrint(SubString& buffer);
   void write(std::ostream& out) const;
   void read(std::istream& in);
};

inline void
SetOfChars::retrieveIntervalAround(char achar, char& min, char& max, bool isNegative) const {
   _retrieveIntervalAround(achar, min, max, isNegative);
   if (((char) 0xff) < 0) {
      if (achar >= 0) {
         if (min == (char) 0x00) {
            bool doesAccept = accept((char) 0xff);
            if (isNegative ? !doesAccept : doesAccept) {
               char tempMax;
               _retrieveIntervalAround((char) 0xff, min, tempMax, isNegative);
               if (min >= 0)
                  min = (char) 0x80;
            };
         };
         if (max < 0)
            max = (char) 0x7f;
      }
      else { // achar < 0
         if (max == (char) 0xff) {
            bool doesAccept = accept((char) 0x00);
            if (isNegative ? !doesAccept : doesAccept) {
               char tempMin;
               _retrieveIntervalAround((char) 0x00, tempMin, max, isNegative);
               if (max < 0)
                  max = (char) 0x7f;
            }
         };
         if (min >= 0)
            min = (char) 0x80;
      };
   };
}

std::ostream&
operator <<(std::ostream& out, const SetOfChars& source);

class ESetOfChars : public EnhancedObject, public SetOfChars {
  public:
   ESetOfChars() {}
   ESetOfChars(const SetOfChars& source) : SetOfChars(source) {}
   ESetOfChars(const ESetOfChars& source) = default;
   DefineCopy(ESetOfChars)
   DDefineAssign(ESetOfChars)
};

/*****************************************************/
/* Definition of the template class TAVLBasedCharSet */
/*****************************************************/

template <class TypeChar>
class TCharArithmetic {
  public:
   static bool isJustBefore(const TypeChar& fst, const TypeChar& snd) { return fst == snd-1; }
   static bool isJustAfter(const TypeChar& fst, const TypeChar& snd) { return fst == snd+1; }
   static TypeChar getBefore(const TypeChar& achar) { return (TypeChar) (achar-1); }
   static TypeChar getAfter(const TypeChar& achar) { return (TypeChar) (achar+1); }
   static TypeChar getLowerBound() { return (TypeChar) 0; }
   static TypeChar getUpperBound() { return (TypeChar) 0xffffffff; }
   static bool isLowerBound(const TypeChar& achar) { return achar == 0; }
   static bool isUpperBound(const TypeChar& achar) { return achar == (TypeChar) 0xffffffff; }
   static void setToNext(TypeChar& achar) { ++achar; }
   static void setToPrevious(TypeChar& achar) { --achar; }
   static bool isLower(const TypeChar& achar) { return false; }
   static bool isUpper(const TypeChar& achar) { return false; }
   static TypeChar lower(const TypeChar& achar) { return achar; }
   static TypeChar upper(const TypeChar& achar) { return achar; }
   static TypeChar lowerUpperBound() { return (TypeChar) 0; }
   static TypeChar lowerLowerBound() { return (TypeChar) 0; }
   static TypeChar upperUpperBound() { return (TypeChar) 0; }
   static TypeChar upperLowerBound() { return (TypeChar) 0; }
};

template <>
class TCharArithmetic<char> {
  public:
   static bool isJustBefore(const char& fst, const char& snd) { return fst == snd-1; }
   static bool isJustAfter(const char& fst, const char& snd) { return fst == snd+1; }
   static char getBefore(const char& achar) { return (char) (achar-1); }
   static char getAfter(const char& achar) { return (char) (achar+1); }
   static char getLowerBound() { return (((char) 0xff) > 0) ? (char) 0x00 : (char) 0x80; }
   static char getUpperBound() { return (((char) 0xff) > 0) ? (char) 0xff : (char) 0x7f; }
   static bool isLowerBound(const char& achar) { return achar == ((((char) 0xff) > 0) ? (char) 0x00 : (char) 0x80); }
   static bool isUpperBound(const char& achar) { return achar == ((((char) 0xff) > 0) ? (char) 0xff : (char) 0x7f); }
   static void setToNext(char& achar) { ++achar; AssumeCondition(achar) }
   static void setToPrevious(char& achar) { AssumeCondition(achar) --achar; }
   static bool isLower(const char& achar) { return achar >= 'a' && achar <= 'z'; }
   static bool isUpper(const char& achar) { return achar >= 'A' && achar <= 'Z'; }
   static char lower(const char& achar)
      {  AssumeCondition(achar >= 'A' && achar <= 'Z') return achar-'A'+'a'; }
   static char upper(const char& achar)
      {  AssumeCondition(achar >= 'a' && achar <= 'z') return achar-'a'+'A'; }
   static char lowerUpperBound() { return 'z'; }
   static char lowerLowerBound() { return 'a'; }
   static char upperUpperBound() { return 'Z'; }
   static char upperLowerBound() { return 'z'; }
};

template <typename TypeChar, class CharTraits=STG::TCharArithmetic<TypeChar> >
class TAVLBasedCharSet;

} // end of namespace STG

namespace COL { namespace DSTG { namespace DTAVLBasedCharSet {

template <typename TypeChar, class CharTraits>
class IntervalsSet;

template <typename TypeChar, class CharTraits>
class Interval : public COL::ImplBalancedNode {
  private:
   typedef COL::ImplBalancedNode inherited;
   typedef Interval<TypeChar, CharTraits> thisType;
   TypeChar cMin;
   TypeChar cMax;

   static void swap(thisType* fst, thisType* snd, thisType*& parentFst, thisType*& parentSnd,
         PImplBinaryNode& root)
      {  ImplBinaryNode *aparentFst = parentFst, *aparentSnd = parentSnd;
         inherited::swap(fst, snd, aparentFst, aparentSnd, root);
         parentFst = (thisType*) aparentFst;
         parentSnd = (thisType*) aparentSnd;
      }
  public:
   Interval(const TypeChar& min, const TypeChar& max) : cMin(min), cMax(max) {}
   Interval(const thisType& source) = default;
   thisType& operator=(const thisType& source) = default;

   ComparisonResult compare(const TypeChar& achar) const
      {  return (achar < cMin) ? CRLess : ((achar > cMax) ? CRGreater : CREqual); }
   ComparisonResult compare(const thisType& source) const
      {  return (cMax < source.cMin) ? CRLess : ((source.cMax < cMin) ? CRGreater : CREqual); }
   bool operator<(const thisType& source) const { return compare(source) == CRLess; }
   bool operator>(const thisType& source) const { return compare(source) == CRGreater; }
   bool isPrevMin(const TypeChar& achar) const { return CharTraits::isJustBefore(achar, cMin); }
   bool isNextMax(const TypeChar& achar) const { return CharTraits::isJustAfter(achar, cMax); }

   bool accept(TypeChar achar) const { return (cMin <= achar) && (achar <= cMax); }
   const TypeChar& min() const { return cMin; }
   TypeChar& min() { return cMin; }
   const TypeChar& max() const { return cMax; }
   TypeChar& max() { return cMax; }
   void setMin(const TypeChar& min) { cMin = min; }
   void setMax(const TypeChar& max) { cMax = max; }
   bool isValid() const { return inherited::isValid() && (cMin <= cMax); }
   typedef inherited::Balance Balance;

   friend class IntervalsSet<TypeChar, CharTraits>;
   friend class ::STG::TAVLBasedCharSet<TypeChar, CharTraits>;
};

#define DefineTemplate template <typename TypeChar, class CharTraits>
#define DefineTypeNode Interval<TypeChar, CharTraits>
#define DefineTypeBinaryTree IntervalsSet<TypeChar, CharTraits>
#define DefineDeclareTypeBinaryTree IntervalsSet
#define DefineUninheritsEnhancedObject
#define DefineTypeCast HandlerCast<Interval<TypeChar, CharTraits>, COL::ImplBalancedNode>
#include "Pointer/Binary.inch"
#undef DefineTypeCast
#undef DefineUninheritsEnhancedObject
#undef DefineTypeNode
#undef DefineTypeBinaryTree
#undef DefineDeclareTypeBinaryTree
#undef DefineTemplate

}}} // end of namespace COL::DSTG::DTAVLBasedCharSet

namespace STG {

template <typename TypeChar, class CharTraits>
class TAVLBasedCharSet
      :  public COL::DSTG::DTAVLBasedCharSet::IntervalsSet<TypeChar, CharTraits>,
         public DCharsSet::Access {
  private:
   typedef TAVLBasedCharSet<TypeChar, CharTraits> thisType;
   typedef COL::DSTG::DTAVLBasedCharSet::IntervalsSet<TypeChar, CharTraits> inherited;
   typedef COL::DSTG::DTAVLBasedCharSet::IntervalsSet<TypeChar, CharTraits> inheritedImplementation;
   typedef COL::DSTG::DTAVLBasedCharSet::Interval<TypeChar, CharTraits> Interval;

   class CharLocate {
     public:
      typedef const TypeChar& KeyType;
      typedef Interval ElementType;
      typedef thisType TabType;
      typedef inheritedImplementation TabImplementation;
      typedef HandlerCast<Interval, Interval> Cast;
      typedef Interval TypeNode;
      typedef typename inheritedImplementation::Cursor TypeCursor;
      
      TypeChar key;
      TypeCursor& cursor;

      CharLocate(const TypeChar& node, TypeCursor& cursorSource)
         :  key(node), cursor(cursorSource) {}

      static ComparisonResult compare(const TypeChar& fst, const Interval& snd)
         {  ComparisonResult result = snd.compare(fst);
            return result != CRNonComparable ? (ComparisonResult) -result : CRNonComparable;
         }
   };
   friend class CharLocate;
   static const Interval& keyFromCell(const Interval& interval) { return interval; }
   static const thisType& castFromImplementation(const COL::CustomImplBinaryTree& tree)
      {  return (const thisType&) tree; }
   friend class COL::CustomImplBinaryTree;

   static TypeChar sminLower()
      {  return (CharTraits::lowerLowerBound() < CharTraits::lowerUpperBound())
            ? CharTraits::lowerLowerBound() : CharTraits::lowerUpperBound();
      }
   static TypeChar smaxLower()
      {  return (CharTraits::upperLowerBound() < CharTraits::upperUpperBound())
            ? CharTraits::upperLowerBound() : CharTraits::upperUpperBound();
      }
   static TypeChar sminUpper()
      {  return (CharTraits::lowerLowerBound() > CharTraits::lowerUpperBound())
            ? CharTraits::lowerLowerBound() : CharTraits::lowerUpperBound();
      }
   static TypeChar smaxUpper()
      {  return (CharTraits::upperLowerBound() > CharTraits::upperUpperBound())
            ? CharTraits::upperLowerBound() : CharTraits::upperUpperBound();
      }
   static TypeChar slower(const TypeChar& tcChar)
      {  return (CharTraits::lowerLowerBound() < CharTraits::lowerUpperBound())
            ? CharTraits::lower(tcChar) : CharTraits::upper(tcChar);
      }
   static TypeChar supper(const TypeChar& tcChar)
      {  return (CharTraits::lowerLowerBound() < CharTraits::lowerUpperBound())
            ? CharTraits::upper(tcChar) : CharTraits::lower(tcChar);
      }
   static TypeChar isSensitive(const TypeChar& tcChar)
      {  return CharTraits::isLower(tcChar) || CharTraits::isUpper(tcChar); }

  protected:
   virtual ComparisonResult _compare(const EnhancedObject& asource) const override
      {  return inherited::_compare(asource); }

  public:
   TAVLBasedCharSet() {}
   TAVLBasedCharSet(const thisType& source) : inherited(source, true) {}
   thisType& operator=(const thisType& source)
      {  inherited::operator=(source);
         inherited::duplicate(source);
         return *this;
      }

   virtual ~TAVLBasedCharSet() { inherited::freeAll(); }
   Template2DefineCopy(TAVLBasedCharSet, TypeChar, CharTraits)
   DTemplate2DefineAssign(TAVLBasedCharSet, TypeChar, CharTraits)

   thisType& add(const TypeChar& chadd);
   thisType& add(const thisType& source);
   thisType& addNegation(const thisType& source);
   thisType& addInterval(const TypeChar& lower, const TypeChar& upper);
   thisType& addAll()
      {  inheritedImplementation::freeAll();
         inheritedImplementation::addAsFirst(new Interval(CharTraits::getLowerBound(), CharTraits::getUpperBound()));
         return *this;
      }
   thisType& addNot(const TypeChar& achar);
   thisType& add(const TypeChar& achar, const ComparisonMode& mode);
   thisType& addInterval(const TypeChar& lower, const TypeChar& upper, const ComparisonMode& mode);
   thisType& addNot(const TypeChar& achar, const ComparisonMode& mode);

   thisType& remove(const TypeChar& achar);
   thisType& removeInterval(const TypeChar& lower, const TypeChar& upper);
   thisType& removeInterval(const TypeChar& lower, const TypeChar& upper, const ComparisonMode& mode);
   thisType& clear() { inheritedImplementation::freeAll(); return *this; }
   thisType& remove(const TypeChar& achar, const ComparisonMode& mode);
   thisType& negate();
   thisType& remove(const thisType& source);
   thisType& mergeWith(const thisType& source) { return add(source); }
   thisType& intersectWith(const thisType& source);
   const thisType& separateWith(const thisType& source,
         thisType& diff1, thisType& intersect, thisType& diff2) const;
   thisType& insensitive();

   /* get methods */
   bool accept(const TypeChar& achar) const
      {  typename inheritedImplementation::Cursor cursor(*this);
         return inheritedImplementation::tlocateBefore(CharLocate(achar, cursor)) == Interval::BEqual;
      }
   bool isAny() const
      {  bool result = (inheritedImplementation::count() == 1);
         if (result) {
            const Interval* interval = inheritedImplementation::root();
            result = (interval->min() == CharTraits::getLowerBound())
               && (interval->max() == CharTraits::getUpperBound());
         };
         return result;
      }
   bool isEmpty() const {  return inherited::isEmpty(); }
   bool contain(const thisType& source) const;
   bool isIntersectionEmpty(const thisType& source) const;
   int queryCount() const;
   void retrieveNegativeIntervalAround(TypeChar achar, TypeChar& min, TypeChar& max) const;
   void retrieveIntervalAround(TypeChar achar, TypeChar& min, TypeChar& max) const 
      {  typename inheritedImplementation::Cursor cursor(*this);
         bool hasLocate = (inheritedImplementation::tlocateBefore(CharLocate(achar, cursor)) == Interval::BEqual);
         AssumeCondition(hasLocate)
         min = cursor.elementAt()->min();
         max = cursor.elementAt()->max();
      }
   bool invariant() const;
};

template <typename TypeChar, class CharTraits>
inline auto
TAVLBasedCharSet<TypeChar, CharTraits>::addNot(const TypeChar& achar) -> thisType& {
   if (accept(achar)) {
      inheritedImplementation::freeAll();
      inheritedImplementation::addAsFirst(new Interval(CharTraits::getLowerBound(), CharTraits::getUpperBound()));
   }
   else {
      TypeChar previous = achar, next = achar;
      inheritedImplementation::freeAll();
      if (CharTraits::getLowerBound() != previous) {
         CharTraits::setToPrevious(previous);
         inheritedImplementation::addAsFirst(new Interval(CharTraits::getLowerBound(), previous));
      };
      if (CharTraits::getUpperBound() != next) {
         CharTraits::setToNext(next);
         inheritedImplementation::addAsLast(new Interval(next, CharTraits::getUpperBound()));
      };
   };
   return *this;
}

template <typename TypeChar, class CharTraits>
inline auto
TAVLBasedCharSet<TypeChar, CharTraits>::add(const TypeChar& achar, const ComparisonMode& mode) -> thisType& {
   add(achar);
   if (mode.isCaseInsensitive()) {
      if (CharTraits::isLower(achar))
         add(CharTraits::upper(achar));
      else if (CharTraits::isUpper(achar))
         add(CharTraits::lower(achar));
   };
   return *this;
}

template <typename TypeChar, class CharTraits>
inline auto
TAVLBasedCharSet<TypeChar, CharTraits>::remove(const TypeChar& achar, const ComparisonMode& mode) -> thisType& {
   remove(achar);
   if (mode.isCaseInsensitive()) {
      if (CharTraits::isLower(achar))
         remove(CharTraits::upper(achar));
      else if (CharTraits::isUpper(achar))
         remove(CharTraits::lower(achar));
   };
   return *this;
}

} // end of namespace STG

#endif // STG_SetOfCharsH

