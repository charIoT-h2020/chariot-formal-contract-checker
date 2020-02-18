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
// File      : SubString.cpp
// Description :
//   Implementation of the class SubString (and the template class TSubString).
//

#include "TString/SubString.h"
// #include "TString/String.hpp"

namespace STG {}

#include <cstring>
#include <math.h>
#include <ctype.h>
#include <stdlib.h>
#include "TString/StringRep.template"
#include "TString/SubString.template"

namespace STG {

DSubString::TSearchTraits<DSubString::TSubStringTraits<char>, char>::Strings
TSubStringTraits<char>::trueStrings() {
   const char* TrueStrings[] = {"true", "vrai", "wahr", "T", "on", "TT"}; 
   return inherited::Strings(6, TrueStrings);
}

DSubString::TSearchTraits<DSubString::TSubStringTraits<char>, char>::Strings
TSubStringTraits<char>::falseStrings() {
   const char* FalseStrings[] = {"false", "faux", "falsch", "F", "off", "FF"}; 
   return inherited::Strings(6, FalseStrings);
}
   
const char*
TSubStringTraits<char>::strchr(const char* string, char search)
   {  return ::strchr(string, search); }

const char*
TSubStringTraits<char>::strrchr(const char* string, char search)
   {  return ::strrchr(string, search); }

bool
TSubStringTraits<char>::isalnum(char achar)
   { return ::isalnum(achar); }

bool
TSubStringTraits<char>::isdigit(char achar)
   { return ::isdigit(achar); }

int
TSubStringTraits<char>::strncmp(const char* s1, const char* s2, int maxlen)
   { return ::strncmp(s1, s2, maxlen); }

int
TSubStringTraits<char>::strnicmp(const char *s1, const char *s2, int maxlen)
#ifndef __GNUC__
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
{  return ::_strnicmp(s1, s2, maxlen); }
#else
{  return ::strnicmp(s1, s2, maxlen); }
#endif
#else
{  int result = 0;
   while ((maxlen-- > 0) && (result == 0)) {
      char chs1 = tolower(*s1++);
      char chs2 = tolower(*s2++);
      result = (chs1 < chs2) ? -1 : ((chs1 == chs2) ? 0 : 1);
   };
   return result;
}
#endif

int
TSubStringTraits<char>::strlen(const char* string)
   { return (int) ::strlen(string); }
   
int
TSubStringTraits<char>::atoi(const char* s1)
   { return ::atoi(s1); }
   
unsigned int
TSubStringTraits<char>::atou(const char* s1)
   { return ::strtoul(s1, nullptr, 10); }
   
long int
TSubStringTraits<char>::atol(const char* s1)
   { return ::atol(s1); }
   
unsigned long int
TSubStringTraits<char>::atoul(const char* s1)
   { return ::strtoul(s1, nullptr, 10); }
   
bool
TSubStringTraits<char>::isspace(char achar)
   { return ::isspace((unsigned char) achar); }

void
TSubStringTraits<char>::itoa(int source, char* string)
#ifndef __GNUC__
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
{  _i64toa_s(source, string, (sizeof(int)*8) / 3 + 2, 10); }
#else
{  _i64toa(source, string, 10); }
#endif
#else
{  int len = (sizeof(int)*8) / 3 + 2;
   if (source < 0) {
      source = -source;
      *string = '-';
      ++string;
      --len;
      if (source < 0) {
         string[len] = (char) (-(source % 10) + '0');
         --len;
         source /= 10;
         source = -source;
      }
   };
   if (source == 0) {
      *string = '0';
      len = 1;
   }
   else {
      int index = len;
      while ((source > 0) && (index >= 0)) {
         string[index] = (char) (source % 10 + '0');
         --index;
         source /= 10;
      };
      if ((index > 0) && (len > index)) {
         memmove(string, string+index+1, len-index);
         len -= index;
      };
   };
   string[len] = '\0';
}
#endif

void
TSubStringTraits<char>::utoa(unsigned int source, char* string)
{  int len = (sizeof(unsigned int)*8) / 3 + 1;
   if (source == 0) {
      *string = '0';
      len = 1;
   }
   else {
      int index = len;
      while ((source > 0) && (index >= 0)) {
         string[index] = (char) (source % 10 + '0');
         --index;
         source /= 10;
      };
      if ((index > 0) && (len > index)) {
         memmove(string, string+index+1, len-index);
         len -= index;
      };
   };
   string[len] = '\0';
}

void
TSubStringTraits<char>::ltoa(long int source, char* string)
{  int len = (sizeof(long int)*8) / 3 + 2;
   if (source < 0) {
      source = -source;
      *string = '-';
      ++string;
      --len;
   };
   if (source == 0) {
      *string = '0';
      len = 1;
   }
   else {
      int index = len;
      while ((source > 0) && (index >= 0)) {
         string[index] = (char) (source % 10 + '0');
         --index;
         source /= 10;
      };
      if ((index > 0) && (len > index)) {
         memmove(string, string+index+1, len-index);
         len -= index;
      };
   };
   string[len] = '\0';
}

void
TSubStringTraits<char>::ultoa(unsigned long int source, char* string)
{  int len = (sizeof(unsigned long int)*8) / 3 + 1;
   if (source == 0) {
      *string = '0';
      len = 1;
   }
   else {
      int index = len;
      while ((source > 0) && (index >= 0)) {
         string[index] = (char) (source % 10 + '0');
         --index;
         source /= 10;
      };
      if ((index > 0) && (len > index)) {
         memmove(string, string+index+1, len-index);
         len -= index;
      };
   };
   string[len] = '\0';
}

void
TSubStringTraits<char>::gcvt(double source, char* string)
#if !defined(__GNUC__)
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
   {  ::_gcvt_s(string, 48, source, 10); }
#else
   {  ::_gcvt(source, 10, string); }
#endif
#else
{  int len = 48;
   if (source < 0) {
      source = -source;
      *string = '-';
      ++string;
      --len;
   };
   if (source == 0.0)
      memcpy(string, "0.0", 4);
   else {
      int exponent = (source >= 1) ? (int) log10(source) : (-(int) log10(1.0/source) - 1);
      bool negativeExponent = false;
      if (exponent < 0) {
         negativeExponent = true;
         exponent = -exponent;
         source *= pow(10, exponent);
      }
      else
         source /= pow(10, exponent);
      int index = len;
      if (source >= 1)
         source /= 10;
      source += 1e-11;
      if (source >= 1) {
         source /= 10;
         if (negativeExponent)
            --exponent;
         else
            ++exponent;
      };

      if (exponent > 0) {
         while (exponent > 0) {
            string[--index] = ((char) (exponent % 10 + '0'));
            exponent /= 10;
         };
         if (negativeExponent)
            string[--index] = '-';
         string[--index] = 'e';
      };
      int floatValue = (int) (source * pow(10, 9));
      bool insert = false;
      for (int norm = 9; --norm;) {
         if (insert || (floatValue % 10)) {
            string[--index] = (char) (floatValue % 10 + '0');
            insert = true;
         };
         floatValue /= 10;
      };
      if (insert)
         string[--index] = '.';
      AssumeCondition(floatValue <= 10)
      if (floatValue == 10) {
         floatValue = 1;
         for (int i = index; i < len; ++i) {
            if (string[i] == '9')
               string[i] = '0';
            else if (string[i] != '0')
               break;
         };
      };
      string[--index] = (char) (floatValue + '0');

      if ((index > 0) && (len > index)) {
         memmove(string, string+index, len-index);
         len -= index;
      };
      string[len] = '\0';
   };
}
#endif

#ifndef strtod
double
TSubStringTraits<char>::strtod(const char *s, char **endptr)
   { return ::strtod(s, endptr); }
#else

#define DefineOldStrtop strtod
#undef strtod
double
TSubStringTraits<char>::strtod(const char *s, char **endptr)
   { return ::DefineOldStrtop(s, endptr); }
#define strtod DefineOldStrtop

#endif

char
TSubStringTraits<char>::tolower(char achar)
   { return ::tolower(achar); }

char
TSubStringTraits<char>::toupper(char achar)
   { return ::toupper(achar); }

class TSubStringTraits<char>::BMSearch {
  public:
   typedef DSubString::SearchParameters SearchParameters;
   
  private:
   static void prefixFunction(int patternLen, const char* pattern, int* prefix,
      const SearchParameters& params);

  public:
   static void badCharacterInit(int patternLen, const char* pattern, int* badChar,
      const SearchParameters& params);
   static void goodSuffixInit(int patternLen, const char* pattern, int* goodSuffix,
      const SearchParameters& params);
};

void
TSubStringTraits<char>::BMSearch::badCharacterInit(int patternLen, const char* pattern,
      int* badChar, const SearchParameters& params) {
   if (params.isBackward()) {
      // first occurence of achar in the sub-string
      if (params.isCaseInsensitive()) {
         for(int achar = 0; achar < 256; achar++)
            badChar[achar] = patternLen;
         for(int subPos = patternLen-1; subPos >= 0; subPos--) {
            badChar[(int) tolower(pattern[subPos])] = subPos;
            badChar[(int) toupper(pattern[subPos])] = subPos;
         };
      }
      else {
         for(int achar = 0; achar < 256; achar++)
            badChar[achar] = patternLen;
         for(int subPos = patternLen-1; subPos >= 0; subPos--)
            badChar[(int) pattern[subPos]] = patternLen-1-subPos;
      };
   } else {
      // last occurence of achar in the sub-string
      if (params.isCaseInsensitive()) {
         for(int achar = 0; achar < 256; achar++)
            badChar[achar] = -1;
         for(int subPos = 0; subPos < patternLen; subPos++) {
            badChar[(int) tolower(pattern[subPos])] = subPos;
            badChar[(int) toupper(pattern[subPos])] = subPos;
         };
      }
      else {
         for(int achar = 0; achar < 256; achar++)
            badChar[achar] = -1;
         for(int subPos = 0; subPos < patternLen; subPos++)
            badChar[(unsigned int) (unsigned char) pattern[subPos]] = subPos;
      };
   };
}

void
TSubStringTraits<char>::BMSearch::prefixFunction(int patternLen, const char* pattern,
      int* prefix, const SearchParameters& params) {
   int k = 0;
   int pos;
   if (params.isBackward()) {
      prefix[0] = 0;
      if (params.isCaseInsensitive()) {
         for(pos = 1; pos < patternLen; pos++) {
            while ((k > 0) && (tolower(pattern[k]) != tolower(pattern[pos])))
               k = prefix[k-1];
            if (tolower(pattern[k]) == tolower(pattern[pos]))
               k++;
            prefix[pos] = k;
         };
      }
      else {
         for(pos = 1; pos < patternLen; pos++) {
            while ((k > 0) && (pattern[k] != pattern[pos]))
               k = prefix[k-1];
            if (pattern[k] == pattern[pos])
               k++;
            prefix[pos] = k;
         };
      };
   }
   else {
      prefix[patternLen-1] = 0;
      if (params.isCaseInsensitive()) {
         for(pos = 1; pos < patternLen; pos++) {
            while ((k > 0) && (tolower(pattern[patternLen-1-k])
                               != tolower(pattern[patternLen-1-pos])))
               k = prefix[patternLen-k];
            if (tolower(pattern[patternLen-1-k]) == tolower(pattern[patternLen-1-pos]))
               k++;
            prefix[patternLen-pos-1] = k;
         };
      }
      else {
         for(pos = 1; pos < patternLen; pos++) {
            while ((k > 0) && (pattern[patternLen-1-k] != pattern[patternLen-1-pos]))
               k = prefix[patternLen-k];
            if (pattern[patternLen-1-k] == pattern[patternLen-1-pos])
               k++;
            prefix[patternLen-pos-1] = k;
         };
      } /* endif */
   };
}

void
TSubStringTraits<char>::BMSearch::goodSuffixInit(int patternLen, const char* pattern,
      int* goodSuffix, const SearchParameters& params) {
   int* prefix = new int[patternLen];
   AssumeAllocation(prefix)

   if (params.isBackward()) { // backward verification from the last
      prefixFunction(patternLen, pattern, prefix, SearchParameters(params).setInverseDirection());
      int j;
      for (j = 0; j <= patternLen; j++)
         goodSuffix[j] = patternLen - prefix[0];
      prefixFunction(patternLen, pattern, prefix, params);
      for (int l = patternLen-1; l >= 0; l--) {
         j = prefix[l];
         if (goodSuffix[j] > l - prefix[l]+1)
            goodSuffix[j] = l - prefix[l]+1;
      };
      delete [] prefix;
      goodSuffix[0] = 1;
   }
   else {
      prefixFunction(patternLen, pattern, prefix, SearchParameters(params).setInverseDirection());
      int j;
      for (j = 0; j <= patternLen; j++)
         goodSuffix[j] = patternLen - prefix[patternLen-1];
      prefixFunction(patternLen, pattern, prefix, params);
      for (int l = 0; l < patternLen-1; l++) {
         j = patternLen - prefix[l];
         if (goodSuffix[j] > patternLen-l-prefix[l])
            goodSuffix[j] = patternLen-l-prefix[l];
      };
      delete [] prefix;
      goodSuffix[patternLen] = 1;
   };
}

void
TSubStringTraits<char>::ScanResult::setVariant(const char* pattern, int patternLength,
      const SearchParameters& params) {
   AssumeCondition(!aBadChar && !aGoodSuffix)
   aBadChar = new int[256];
   AssumeAllocation(aBadChar)
   aGoodSuffix = new int[patternLength+1];
   AssumeAllocation(aGoodSuffix)
   BMSearch::badCharacterInit(patternLength, pattern, aBadChar, params);
   BMSearch::goodSuffixInit(patternLength, pattern, aGoodSuffix, params);
}

void
TSubStringTraits<char>::scanPartial(char* string, int length, const char* pattern,
      int patternLength, const SearchParameters& params, ScanResult& result) {
   if (patternLength == 0) {
      result.uResultPosition = !params.isBackward() ? 0 : length;
      result.fFound = true;
      return;
   };
   if (length == 0)
      return;
   AssumeCondition(string && pattern)
   int pos, forward;

   char posChar = '\0';
   if (params.isBackward()) {
      pos = length-1 + result.uPatternPosition;
      while (pos >= patternLength-1) {
         forward = patternLength-1;
         if (params.isCaseInsensitive()) {
            while ((forward >= result.uPatternPosition)
                  && ((posChar = (char) tolower(string[pos - forward])) == (char) tolower(pattern[patternLength-1-forward])))
               --forward;
         }
         else {
            while ((forward >= result.uPatternPosition)
                  && ((posChar = string[pos - forward]) == pattern[patternLength-1-forward]))
               --forward;
         };
         if (forward < result.uPatternPosition) {
            result.uResultPosition = pos-patternLength+1;
            result.fFound = true;
            return;
         }
         else {
            pos -= ((result.aGoodSuffix[forward] > (forward-result.aBadChar[(int) (unsigned char) posChar]))
               ?result.aGoodSuffix[forward]:(forward-result.aBadChar[(int) (unsigned char) posChar]));
            if (pos >= length)
               result.uPatternPosition = pos-length+patternLength;
            else
               result.uPatternPosition = 0;
         };
      };
      result.uPatternPosition = maxPrefix(string,
            (pos >= length) ? length : pos, pattern, patternLength,
            result.uPatternPosition, params);
   }
   else { // params.isForward()
      pos = -result.uPatternPosition;
      while (pos <= (length - patternLength)) {
         forward = patternLength-1;
         if (params.isCaseInsensitive()) {
            while ((forward >= result.uPatternPosition)
                  && ((posChar = (char) tolower(string[pos + forward])) == (char) tolower(pattern[forward])))
               --forward;
         }
         else {
            while ((forward >= result.uPatternPosition)
                  && ((posChar = string[pos + forward]) == pattern[forward]))
               --forward;
         };
         if (forward < result.uPatternPosition) {
            result.uResultPosition = pos;
            result.fFound = true;
            return;
         }
         else {
            pos += ((result.aGoodSuffix[forward+1] > (forward - result.aBadChar[(int) (unsigned char) posChar]))
               ?result.aGoodSuffix[forward+1]:(forward - result.aBadChar[(int) (unsigned char) posChar]));
            if (pos < 0)
               result.uPatternPosition = -pos;
            else
               result.uPatternPosition = 0;
         };
      };
      result.uPatternPosition = maxPrefix((pos < 0) ? string : string+pos,
            (pos < 0) ? length : length-pos, pattern, patternLength,
            result.uPatternPosition, params);
   };
}

} // end of namespace STG

#ifndef _MSC_VER
#define DefineListGlobalPolicy STG::List::GlobalPolicy
#else
#define DefineListGlobalPolicy STG::List::LGlobalPolicy
#endif

template class STG::TSubString<char, STG::List::LightLocalPolicy, STG::TSubStringTraits<char> >;
template class STG::TSubString<char, DefineListGlobalPolicy, STG::TSubStringTraits<char> >;
template class STG::TSubString<char, STG::GlobalPolicy, STG::TSubStringTraits<char> >;
template class STG::TSubString<wchar_t, STG::GlobalPolicy, STG::TSubStringTraits<wchar_t> >;
template class STG::TSubString<wchar_t, STG::List::LightLocalPolicy, STG::TSubStringTraits<wchar_t> >;
template class STG::TSubString<wchar_t, DefineListGlobalPolicy, STG::TSubStringTraits<wchar_t> >;

#undef DefineListGlobalPolicy

