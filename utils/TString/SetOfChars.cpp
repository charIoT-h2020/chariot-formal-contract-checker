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
// File      : SetOfChars.cpp
// Description :
//   Implementation of sets of chars.
//

#include "TString/SubString.h"
// #include "TString/SetOfChars.h"
// #include "TString/String.hpp"

namespace STG {}

#include <iostream>
#include <ctype.h>
#include "TString/SetOfChars.template"

namespace STG {

/******************************************/
/* Implementation de la classe SetOfChars */
/******************************************/

SetOfChars::SetOfChars(Special) {
   memset(acTable, 0, ByteSize);
   add('?').add('^').add('@').add('!').add('\\').add('_').add('-').add('[')
   .add(']').add('(').add(')').add('|').add('+').add('*');
}

SetOfChars::SetOfChars() { memset(acTable, 0, ByteSize); }

SetOfChars
SetOfChars::special = SetOfChars(Special());

SetOfChars&
SetOfChars::addAll() { memset(acTable, 0xFF, ByteSize); return *this; } // saturation

SetOfChars&
SetOfChars::clear() { memset(acTable, 0, ByteSize); return *this; } // set to the empty set

void
SetOfChars::read(SubString& buffer, const ComparisonMode& mode) {
   clear();
   TParser<SetOfChars, char, TSubString<char>, TSpecialCharParser<TSubString<char> > >
      parser;
   ReadResult result = parser.parse(*this, buffer, mode);
   while (result == RRContinue)
      result = parser.parse(*this, buffer, mode);
   if (result != RRFinished)
      throw STG::EReadError();
}

std::ostream&
operator <<(std::ostream& out, const SetOfChars& source) {
   int charToPrint = -1;
   for (int achar = 0; achar < 255;) {
      while ((achar < 255) && !source.accept((char) achar))
         achar++;
      if (achar < 255) {
         if (charToPrint != -1)
            out << '|';
         else
            out << '[';
         charToPrint = achar;
         while ((++achar <= 255) && source.accept((char) achar));
         if (achar - charToPrint > 1) {
            if (isprint(charToPrint))
               out << '[' << (char) charToPrint;
            else
               out << "[\\" << charToPrint;
            if (isprint(achar-1))
               out << '-' << (char) (achar-1) << ']';
            else
               out << "-\\" << (achar-1) << ']';
         }
         else
            if (isprint(charToPrint))
               out << (char) charToPrint;
            else
               out << '\\' << charToPrint;
      };
   };
   if (charToPrint != -1)
     out << ']';
   return out;
}

void
SetOfChars::prettyPrint(SubString& buffer) {
   int charToPrint = -1;
   for (int achar = 0; achar < 255;) {
      while ((achar < 255) && !accept((char) achar))
         achar++;
      if (achar < 255) {
         if (charToPrint != -1)
            buffer.cat('|');
         else
            buffer.cat('[');
         charToPrint = achar;
         while ((++achar <= 255) && accept((char) achar));
         if (achar - charToPrint > 1) {
            if (isprint(charToPrint))
               buffer.cat('[').cat((char) charToPrint);
            else
               buffer.cat("[\\").catInteger((int) charToPrint);
            if (isprint(achar-1))
               buffer.cat('-').cat((char) (achar-1)).cat(']');
            else
               buffer.cat("-\\").catInteger((int) (achar-1)).cat(']');
         }
         else
            if (isprint(charToPrint))
               buffer.cat((char) charToPrint);
            else
               buffer.cat('\\').cat((char) charToPrint);
      };
   };
   if (charToPrint != -1)
     buffer.cat(']');
}

/* specific methods to handle chars */

SetOfChars&
SetOfChars::insensitive() {
   int lowerIndex = 'a' / (8*sizeof(unsigned int));
   int lastLowerIndex = 'z' / (8*sizeof(unsigned int));
   int lowerRemainder = 'a' % (8*sizeof(unsigned int));
   int lastLowerRemainder = 'z' % (8*sizeof(unsigned int));
   int upperIndex = 'A' / (8*sizeof(unsigned int));
   int upperRemainder = 'A' % (8*sizeof(unsigned int));
   int lastUpperIndex = 'Z' / (8*sizeof(unsigned int));
   int lastUpperRemainder = 'Z' % (8*sizeof(unsigned int));

   if ((lowerIndex == lastLowerIndex) && (upperIndex == lastUpperIndex)) {
      unsigned int lower = acTable[lowerIndex];
      lower &= ~(~0U << (lastLowerRemainder - lowerRemainder + 1)) << lowerRemainder;
      unsigned int upper = acTable[upperIndex];
      upper &= ~(~0U << (lastUpperRemainder - upperRemainder + 1)) << upperRemainder;
      if (lowerRemainder < upperRemainder) {
         lower <<= (upperRemainder - lowerRemainder);
         upper >>= (upperRemainder - lowerRemainder);
      }
      else if (lowerRemainder > upperRemainder) {
         lower >>= (lowerRemainder - upperRemainder);
         upper <<= (lowerRemainder - upperRemainder);
      };
      acTable[upperIndex] |= lower;
      acTable[lowerIndex] |= upper;
      return *this;
   };
   typedef unsigned BigInt[(ByteSize-1)/sizeof(unsigned int)+1];
   BigInt table;
   for (int index = 0; index <= (int) ((ByteSize-1)/sizeof(unsigned int)); ++index)
      table[index] = acTable[index];
   BigInt lower, upper;
   memcpy(lower, table, sizeof(unsigned int)*((ByteSize-1)/sizeof(unsigned int)+1));
   memcpy(upper, table, sizeof(unsigned int)*((ByteSize-1)/sizeof(unsigned int)+1));
   lower[lowerIndex] &= (~0U << lowerRemainder);
   lower[lastLowerIndex] &= ~(~0U << ((8*sizeof(unsigned int)) - lastLowerRemainder));
   upper[upperIndex] &= (~0U << upperRemainder);
   upper[lastUpperIndex] &= ~(~0U << ((8*sizeof(unsigned int)) - lastUpperRemainder));

   int firstShift;

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning (disable: 4308)
#endif
   if ('a' < 'A')
      firstShift = (('A'-'a') % (8*sizeof(unsigned int)));
   else {
      firstShift = (('a'-'A') % (8*sizeof(unsigned int)));
      if (firstShift > 0)
         firstShift = (8*sizeof(unsigned int)) - firstShift;
   };
#ifdef _MSC_VER
#pragma warning( pop )
#endif
  
   if (firstShift > 0) {
      for (int index = 0; index < (lastLowerIndex - lowerIndex); ++index) {
         table[upperIndex+index] |= (lower[lowerIndex+index]) << firstShift;
         if (index < (lastLowerIndex - lowerIndex - 1))
            table[upperIndex+index] |= (lower[lowerIndex+index+1] >> (8*sizeof(unsigned int) - firstShift));
      };
      for (int index = 0; index < (lastUpperIndex - upperIndex); ++index) {
         table[lowerIndex+index] |= (upper[upperIndex+index]) << (8*sizeof(unsigned int) - firstShift);
         if (index < (lastUpperIndex - upperIndex - 1))
            table[lowerIndex+index] |= (upper[upperIndex+index+1] >> firstShift);
      };
   }
   else for (int index = 0; index < (lastLowerIndex - lowerIndex); ++lowerIndex) {
      table[upperIndex+index] |= lower[lowerIndex+index];
      table[lowerIndex+index] |= upper[upperIndex+index];
   };

   for (int index = 0; index <= (int) ((ByteSize-1)/sizeof(unsigned int)); ++index)
      acTable[index] |= table[index];
   return *this;
}

/* add methods */

SetOfChars&
SetOfChars::add(const SetOfChars& source) {
   int index = (ByteSize-1)/sizeof(unsigned int)+1;
   while (--index >= 0)
      acTable[index] |= source.acTable[index];
   return *this;
}

SetOfChars&
SetOfChars::addNegation(const SetOfChars& source) {
   int index = (ByteSize-1)/sizeof(unsigned int)+1;
   while (--index >= 0)
      acTable[index] |= ~source.acTable[index];
   if (ByteSize % sizeof(unsigned int) > 0)
      acTable[ByteSize / sizeof(unsigned int)] &= ~(~0U << (ByteSize % sizeof(unsigned int)));
   return *this;
}

void
SetOfChars::_addInterval(char lower, char upper) {
   if (lower >= upper) {
      if (lower == upper)
         add(lower);
   }
   else {
      int index = lower / (8*sizeof(unsigned int));
      int lastIndex = upper / (8*sizeof(unsigned int));
      int remainder = lower % (8*sizeof(unsigned int));
      int lastRemainder = upper % (8*sizeof(unsigned int));
   
      if (index == lastIndex)
         acTable[index] |= ~(~0U << (lastRemainder - remainder + 1)) << remainder;
      else {
         acTable[index] |= ~0U << remainder;
         acTable[lastRemainder] |= ~(~0U << (lastRemainder+1));
         if (lastRemainder > index+1)
            memset(acTable+index+1, 0xFF, (lastRemainder-index-1)*sizeof(unsigned int));
      };
   };
}

SetOfChars&
SetOfChars::add(char chadd, const ComparisonMode& mode) {
   if (mode.isCaseInsensitive()) {
      add((char) tolower(chadd));
      add((char) toupper(chadd));
   }
   else
      add(chadd);
   return *this;
}

SetOfChars&
SetOfChars::addInterval(char lower, char upper, const ComparisonMode& mode) {
   if (mode.isCaseInsensitive()) {
      if (isalpha(lower) && isalpha(upper)
            && ((lower == tolower(lower)) == (upper == tolower(upper)))) {
         addInterval((char) tolower(lower), (char) tolower(upper));
         addInterval((char) toupper(lower), (char) toupper(upper));
      }
      else if (isEmpty()) {
         addInterval(lower, upper);
         insensitive();
      }
      else {
         SetOfChars interval;
         interval.addInterval(lower, upper);
         interval.insensitive();
         add(interval);
      };
   }
   else
      addInterval(lower, upper);
   return *this;
}

SetOfChars&
SetOfChars::addNot(char achar, const ComparisonMode& mode) {
   if (mode.isCaseInsensitive()) {
      if (accept(achar))
         addAll();
      else {
         addAll();
         remove((char) tolower(achar));
         remove((char) toupper(achar));
      };
   }
   else {
      if (accept(achar))
         addAll();
      else {
         addAll();
         remove(achar);
      };
   }
   return *this;
}

/* suppression methods */

SetOfChars&
SetOfChars::remove(const SetOfChars& source)  {
   int index = (ByteSize-1)/sizeof(unsigned int)+1;
   while (--index >= 0)
      acTable[index] &= ~source.acTable[index];
   return *this;
}

SetOfChars&
SetOfChars::remove(char achar, const ComparisonMode& mode) {
   if (mode.isCaseInsensitive()) {
      remove((char) tolower(achar));
      remove((char) toupper(achar));
   }
   else
      remove(achar);
   return *this;
}

/* set based methods */

SetOfChars&
SetOfChars::negate() {
   int index = (ByteSize-1)/sizeof(unsigned int)+1;
   while (--index >= 0)
      acTable[index] = ~acTable[index];
   return *this;
}

SetOfChars&
SetOfChars::intersectWith(const SetOfChars& source) {
   int index = (ByteSize-1)/sizeof(unsigned int)+1;
   while (--index >= 0)
      acTable[index] &= source.acTable[index];
   return *this;
}

const SetOfChars&
SetOfChars::separateWith(const SetOfChars& source, SetOfChars& diff1,
      SetOfChars& intersect, SetOfChars& diff2) const {
   int index = (ByteSize-1)/sizeof(unsigned int)+1;
   while (--index >= 0) {
      intersect.acTable[index] = acTable[index] & source.acTable[index];
      diff1.acTable[index] = acTable[index] & ~source.acTable[index];
      diff2.acTable[index] = ~acTable[index] & source.acTable[index];
   };
   return *this;
}

/* query methods */

bool
SetOfChars::contain(const SetOfChars& source) const {
   int index = (ByteSize-1)/sizeof(unsigned int)+1;
   while ((--index >= 0) && (acTable[index] == (acTable[index] | source.acTable[index])));
   return index < 0;
}

bool
SetOfChars::isIntersectionEmpty(const SetOfChars& source) const {
   int index = (ByteSize-1)/sizeof(unsigned int)+1;
   while ((--index >= 0) && ((acTable[index] & source.acTable[index]) == 0));
   return index < 0;
}

int
SetOfChars::queryCount() const {
   int index = (ByteSize-1)/sizeof(unsigned int)+1;
   int result = 0;
   while (--index >= 0) {
      unsigned int value = acTable[index];
      while (value != 0) {
         result += (value & 1);
         value >>= 1;
      };
   };
   return result;
}

void
SetOfChars::_retrieveIntervalAround(char achar, char& min, char& max, bool isNegative) const {
   int index = ((unsigned char) achar) / (8*sizeof(unsigned int));
   int charIndex = ((unsigned char) achar) % (8*sizeof(unsigned int));
   AssumeCondition(!isNegative ? (acTable[index] & (1U << charIndex)) : !(acTable[index] & (1U << charIndex)))
   if (charIndex < (int) (8*sizeof(unsigned int))-1) {
      while (--charIndex >= 0) {
         bool doesAccept = acTable[index] & (1U << charIndex); 
         if (!isNegative ? !doesAccept : doesAccept) {
            min = (unsigned char) (index * (8*sizeof(unsigned int)) + charIndex+1);
            goto LMaxFind;
         };
      };
      charIndex = (8*sizeof(unsigned int))-1;
      --index;
   };
   while ((index >= 0) && (!isNegative ? (acTable[index] == ~0U) : (acTable[index] == 0U)))
      --index;
   if (index >= 0) {
      while (charIndex >= 0) {
         bool doesAccept = acTable[index] & (1U << charIndex); 
         if (!isNegative ? !doesAccept : doesAccept) {
            min = (unsigned char) (index * (8*sizeof(unsigned int)) + charIndex+1);
            goto LMaxFind;
         };
         --charIndex;
      };
   };
   min = (char) (index * (8*sizeof(unsigned int)) + charIndex+1);
   
LMaxFind:
   index = ((unsigned char) achar) / (8*sizeof(unsigned int));
   charIndex = ((unsigned char) achar) % (8*sizeof(unsigned int));
   if (charIndex > 0) {
      while (++charIndex < (int) (8*sizeof(unsigned int))) {
         bool doesAccept = acTable[index] & (1U << charIndex); 
         if (!isNegative ? !doesAccept : doesAccept) {
            max = (unsigned char) (index * (8*sizeof(unsigned int)) + charIndex-1);
            return;
         };
      };
      charIndex = 0;
      ++index;
   };
   while ((index <= (ByteSize-1)/((int) sizeof(unsigned int)))
         && (!isNegative ? (acTable[index] == ~0U) : (acTable[index] == 0U)))
      ++index;
   if (index <= (ByteSize-1)/((int) sizeof(unsigned int))) {
      while (charIndex < (int) (8*sizeof(unsigned int))) {
         bool doesAccept = acTable[index] & (1U << charIndex); 
         if (!isNegative ? !doesAccept : doesAccept) {
            max = (unsigned char) (index * (8*sizeof(unsigned int)) + charIndex-1);
            return;
         };
         ++charIndex;
      };
   };
   max = (unsigned char) (index * (8*sizeof(unsigned int)) + charIndex-1);
}

void
SetOfChars::write(std::ostream& out) const
   { out.write((const char*) acTable, sizeof(acTable)); }

void
SetOfChars::read(std::istream& in)
   { in.read((char*) acTable, sizeof(acTable)); }

} // end of namespace STG

template class COL::DSTG::DTAVLBasedCharSet::IntervalsSet<wchar_t, STG::TCharArithmetic<wchar_t> >;
template class STG::TAVLBasedCharSet<char, STG::TCharArithmetic<char> >;
template class STG::TAVLBasedCharSet<wchar_t, STG::TCharArithmetic<wchar_t> >;
