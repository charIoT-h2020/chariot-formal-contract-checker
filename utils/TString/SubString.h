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
// File      : SubString.h
// Description :
//   Definition of the class SubString (and the template class TSubString).
//

#ifndef STG_TSubStringH
#define STG_TSubStringH

#include "TString/StringRep.h"
#include "TString/SetOfChars.h"

#ifdef strtod

#define DefineOldStrtop strtod
#undef strtod

#endif

namespace STG {

namespace DSubString {

class SearchParameters : public ComparisonMode {
  private:
   typedef ComparisonMode inherited;
   
  protected:
   DefineExtendedParameters(3, ComparisonMode)
   DefineSubExtendedParameters(Direction, 1, INHERITED)
   DefineSubExtendedParameters(Bound, 1, Direction)
   DefineSubExtendedParameters(ContainZero, 1, Bound)
     
  public:
   SearchParameters() {}
   SearchParameters(const SearchParameters& params) : inherited(params) {}
   SearchParameters(const ComparisonMode& source) : inherited(source) {}
   SearchParameters& operator=(const SearchParameters& source)
      {  return (SearchParameters&) inherited::operator=(source); }
   SearchParameters& operator=(const ComparisonMode& source)
      {  return (SearchParameters&) inherited::operator=(source); }

   bool isForward() const { return !hasDirectionField(); }
   bool isBackward() const { return hasDirectionField(); }
   bool isBound() const { return !hasOwnField(); }
   bool isUnbound() const { return hasOwnField(); }
   bool doesSupportContainZero() const { return hasContainZeroField(); }

   SearchParameters& setInverseDirection()
      {  if (hasDirectionField()) clearDirectionField(); else mergeDirectionField(1); return *this; }
   SearchParameters& setForward() { clearDirectionField(); return *this; }
   SearchParameters& setBackward() { mergeDirectionField(1); return *this; }
   SearchParameters& setBound() { clearBoundField(); return *this; }
   SearchParameters& setUnbound() { mergeBoundField(1); return *this; }
   SearchParameters& setSupportContainZero() { mergeContainZeroField(1); return *this; }
   SearchParameters& setSupportFreeFromZero() { clearContainZeroField(); return *this; }
   SearchParameters& setCaseSensitive() { return (SearchParameters&) inherited::setCaseSensitive(); }
   SearchParameters& setCaseInsensitive() { return (SearchParameters&) inherited::setCaseInsensitive(); }
};

template <class TypeBase, typename TypeChar>
class TSearchTraits : public TypeBase {
  public:
   static TypeChar tolower(const TypeChar& cChar) { return TypeBase::tolower(cChar); }
     
  protected:
   static int maxPrefix(const TypeChar* szString, int uLength, const TypeChar* szPattern,
         int uPatternLength, int uStart, const SearchParameters& spParams);

  public:
   class ScanResult {
     private:
      int uResultPosition;
      int uPatternPosition;
      int uVariant, uResultVariant;
      bool fFound;

     public:
      ScanResult()
         :  uResultPosition(-1), uPatternPosition(0), uVariant(0), uResultVariant(0), fFound(false) {}
      ScanResult(const ScanResult& source) = default;
      ScanResult& operator=(const ScanResult& source) = default;
      friend class TSearchTraits<TypeBase, TypeChar>;

      void setVariant(const TypeChar* pattern, int patternLength, const SearchParameters& params);
      int result() const { return uResultPosition; }
      bool isValid() const { return fFound; }
   };

   static void scanPartial(TypeChar* string, int length, const TypeChar* pattern,
         int patternLength, const SearchParameters& params, ScanResult& result);
};

template <typename TypeChar>
class TSubStringTraits {
  public:
   typedef STG::TCharArithmetic<TypeChar> CharTraits;
   typedef TAVLBasedCharSet<TypeChar, STG::TCharArithmetic<TypeChar> > CharSet;
   typedef TAVLBasedCharSet<TypeChar, STG::TCharArithmetic<TypeChar> > ECharSet;
   class CharResult {
     private:
      TypeChar tcReadChar;
      bool fValid;

     public:
      CharResult() : tcReadChar(0), fValid(false) {}
      CharResult(const CharResult& source) = default;
      void setChar(const TypeChar& achar) { tcReadChar = achar; fValid = true; }
      bool isValid() const { return fValid; }
      void invalidate() { fValid = false; tcReadChar = 0; }
      const TypeChar& getReadChar() const { AssumeCondition(fValid) return tcReadChar; }
      const TypeChar& getERead() const { if (!fValid) throw EReadError(); return tcReadChar; }
      operator TypeChar() const { AssumeCondition(fValid) return tcReadChar; }
   };

   static const TypeChar* strchr(const TypeChar* string, const TypeChar& search) { AssumeUncalled return nullptr; }
   static const TypeChar* strrchr(const TypeChar* string, const TypeChar& search) { AssumeUncalled return nullptr; }
   static ComparisonResult strnicmp(const TypeChar* s1, const TypeChar* s2, int maxlen) { AssumeUncalled return CREqual; }
   static ComparisonResult strncmp(const TypeChar* s1, const TypeChar* s2, int maxlen) { AssumeUncalled return CREqual; }
   static ComparisonResult compare(const TypeChar& c1, const TypeChar& c2) { AssumeUncalled return CREqual; }
   static int atoi(const TypeChar* s1) { AssumeUncalled return 0; }
   static unsigned int atou(const TypeChar* s1) { AssumeUncalled return 0; }
   static long int atol(const TypeChar* s1) { AssumeUncalled return 0; }
   static unsigned long int atoul(const TypeChar* s1) { AssumeUncalled return 0; }
   static bool isalnum(const TypeChar& achar) { return false; }
   static bool issign(const TypeChar& achar) { return false; }
   static bool ispoint(const TypeChar& achar) { return false; }
   static bool isexponent(const TypeChar& achar) { return false; }
   static bool ispositive(const TypeChar& achar) { return false; }
   static bool isnegative(const TypeChar& achar) { return false; }
   static bool isdigit(const TypeChar& achar) { return false; }
   static int getDigit(const TypeChar& achar) { AssumeUncalled return 0; }
   static bool isoctaldigit(const TypeChar& achar) { return false; }
   static int getOctalDigit(const TypeChar& achar) { AssumeUncalled return 0; }
   static bool ishexadigit(const TypeChar& achar) { return false; }
   static int getHexaDigit(const TypeChar& achar) { AssumeUncalled return 0; }
   static TypeChar convertHexa(int aint) { AssumeUncalled return TypeChar(); }
   static void itoa(int source, TypeChar* string) { AssumeUncalled string[0] = 0; }
   static void utoa(unsigned int source, TypeChar* string) { AssumeUncalled string[0] = 0; }
   static void ltoa(long int source, TypeChar* string) { AssumeUncalled string[0] = 0; }
   static void ultoa(unsigned long int source, TypeChar* string) { AssumeUncalled string[0] = 0; }
   static void gcvt(double adouble, TypeChar* string) { AssumeUncalled string[0] = 0; }
   static double strtod(const TypeChar* s, TypeChar** endptr) { AssumeUncalled return 0; }
   static bool islower(const TypeChar& achar) { return false; }
   static bool isupper(const TypeChar& achar) { return false; }
   static TypeChar tolower(const TypeChar& achar) { AssumeUncalled return TypeChar(); }
   static TypeChar toupper(const TypeChar& achar) { AssumeUncalled return TypeChar(); }

   class Strings {
     private:
      int uNbStrings;
      const TypeChar** acArray;

     public:
      Strings() : uNbStrings(0), acArray(nullptr) {}
      Strings(int nbStrings, const TypeChar** array) : uNbStrings(nbStrings), acArray(array) {}
      Strings(const Strings& source) = default;

      const int& getNbStrings() const { return uNbStrings; }
      const TypeChar* getString(int index) const
         {  AssumeCondition((index >= 0) && (index < uNbStrings))
            return acArray[index];
         }
   };
   static Strings trueStrings() { return Strings(); }
   static Strings falseStrings() { return Strings(); }
   static int strlen(const TypeChar* string)
      {  int index = -1;
         if (string)
            while (string[++index] != 0);
         else
            index = 0;
         return index;
      }
   static bool isspace(const TypeChar& achar) { return false; }
   static TypeChar space() { AssumeUncalled }
};

class PersistencyAccess {
  public:
   template <class TypeSubString>
   class TStringProperty : public VirtualStringProperty {
     private:
      TypeSubString& ssSubString;
      mutable typename TypeSubString::Position pPosition;
      mutable TChunk<typename TypeSubString::Char> cChunk;
      mutable int uLeftCount;
      
     public:
      TStringProperty(const TypeSubString& subString)
         :  ssSubString(const_cast<TypeSubString&>(subString)),
            uLeftCount(subString.length()) {}
      TStringProperty(const TStringProperty<TypeSubString>& source) = default;

      StringProperty asString() { return AllStringProperty(*this); }
      StringProperty asSubString() { return SubStringProperty(*this); }

      virtual void copy(void* achunk) const
         {  auto chunk = (TChunk<typename TypeSubString::Char>*) achunk;
            ssSubString.copy(chunk->string, chunk->length);
         }
      virtual void cat(void* achunk) const 
         {  auto chunk = (TChunk<typename TypeSubString::Char>*) achunk;
            ssSubString.cat(chunk->string, chunk->length);
         }
      virtual ComparisonResult compareBound(void* achunk, int& charsNumber, bool isCaseInsensitive) const;
      virtual int length() const { return ssSubString.length(); }
      virtual void* getChunk() const { return &cChunk; }
      virtual bool setToFirstChunk() const;
      virtual bool setToNextChunk() const;
      virtual void read(void* in, int maxChars) const
         {  ssSubString.read(*((std::basic_istream<typename TypeSubString::Char>*) in), maxChars); }
      virtual void readall(void* in) const
         {  ssSubString.read(*((std::basic_istream<typename TypeSubString::Char>*) in)); }
      virtual void write(void* out) const
         {  ssSubString.write(*((std::basic_ostream<typename TypeSubString::Char>*) out)); }
   };
};

template <class TypeSubString>
inline ComparisonResult
PersistencyAccess::TStringProperty<TypeSubString>::compareBound(void* achunk,
      int& charsNumber, bool isCaseInsensitive) const {
   auto chunkSource = (TChunk<typename TypeSubString::Char>*) achunk;
   int localThisLength = (uLeftCount < (int) cChunk.length) ? uLeftCount : cChunk.length;
   int localSourceLength = (charsNumber < (int) chunkSource->length) ? charsNumber : chunkSource->length;

   while (/* (localSourceLength > 0) && */ (localThisLength <= localSourceLength)) {
      int compare = isCaseInsensitive
         ? TypeSubString::Traits::strnicmp(cChunk.string, chunkSource->string, localThisLength)
         : TypeSubString::Traits::strncmp(cChunk.string, chunkSource->string, localThisLength);
      if (compare < 0)
         return CRLess;
      else if (compare > 0)
         return CRGreater;
      charsNumber -= localThisLength;
      chunkSource->string += localThisLength;
      chunkSource->length -= localThisLength;
      if (uLeftCount <= (int) cChunk.length)
         return CREqual;

      setToNextChunk();
      localSourceLength -= localThisLength;
      localThisLength = (uLeftCount < (int) cChunk.length) ? uLeftCount : cChunk.length;
   };
   if (localSourceLength == 0)
      return (charsNumber > 0) ? CREqual : CRGreater;
   int compare = isCaseInsensitive
      ? TypeSubString::Traits::strnicmp(cChunk.string, chunkSource->string, localSourceLength)
      : TypeSubString::Traits::strncmp(cChunk.string, chunkSource->string, localSourceLength);
   if (compare < 0)
      return CRLess;
   else if (compare > 0)
      return CRGreater;
   charsNumber -= localSourceLength;
   uLeftCount -= localSourceLength;
   cChunk.string += localSourceLength;
   cChunk.length -= localSourceLength;
   return (charsNumber > 0) ? CREqual : CRGreater;
}

template <class TypeSubString>
inline bool
PersistencyAccess::TStringProperty<TypeSubString>::setToFirstChunk() const {
   uLeftCount = ssSubString.length();
   pPosition = ssSubString.getStartPosition();
   ssSubString.retrieveChunk(pPosition, cChunk);
   if ((int) cChunk.length > uLeftCount)
      cChunk.length = uLeftCount;
   return uLeftCount > 0;
}

template <class TypeSubString>
inline bool
PersistencyAccess::TStringProperty<TypeSubString>::setToNextChunk() const {
   AssumeCondition(uLeftCount);
   uLeftCount -= cChunk.length;
   bool result = ssSubString.setToNextChunk(pPosition, cChunk);
   if ((int) cChunk.length > uLeftCount)
      cChunk.length = uLeftCount;
   return result && uLeftCount > 0;
}

} // end of namespace DSubString

namespace List {

template <typename TypeChar>
#ifndef _MSC_VER
class GlobalPolicy
#else
class LGlobalPolicy // conflict with ::GlobalPolicy
#endif
{ public:
   typedef TListRepository<TypeChar, DListRepository::GlobalRepositoryPolicy<TypeChar> >
      Repository;
};

template <typename TypeChar>
class LocalPolicy {
  public:
   typedef TListRepository<TypeChar, DListRepository::LocalRepositoryPolicy<TypeChar> >
      Repository;
};

template <typename TypeChar>
class LightLocalPolicy {
  public:
   typedef TListRepository<TypeChar, DListRepository::LightLocalRepositoryPolicy<TypeChar> >
      Repository;
};

} // end of namespace List

template <typename TypeChar>
class GlobalPolicy {
  public:
   typedef TRepository<TypeChar> Repository;
};

template <typename TypeChar>
class TSubStringTraits
   : public DSubString::TSearchTraits<DSubString::TSubStringTraits<TypeChar>, TypeChar>
{};

template <>
class TSubStringTraits<char>
   : public DSubString::TSearchTraits<DSubString::TSubStringTraits<char>, char> {
  private:
   typedef DSubString::TSearchTraits<DSubString::TSubStringTraits<char>, char> inherited;
   class BMSearch;

  public:
   typedef STG::TCharArithmetic<char> CharTraits;
   typedef SetOfChars CharSet;
   typedef ESetOfChars ECharSet;
#ifndef EOF
#define HasDefinedEOF
#define EOF -1
#endif
   class CharResult {
     private:
      int uReadChar;

     public:
      CharResult() : uReadChar(EOF) {}
      CharResult(const CharResult& source) = default;
      void setChar(char achar) { uReadChar = (unsigned int) (unsigned char) achar;}
      bool isValid() const { return uReadChar != EOF; }
      void invalidate() { uReadChar = EOF; }
      char getReadChar() const { AssumeCondition(uReadChar != EOF) return (char) uReadChar; }
      char getERead() const { if (uReadChar == EOF) throw EReadError(); return (char) uReadChar; }
      int getInternal() const { return uReadChar; }
      operator char() const { AssumeCondition(uReadChar != EOF) return (char) uReadChar; }
   };
#ifdef HasDefinedEOF
#undef HasDefinedEOF
#undef EOF
#endif
   static const char* strchr(const char* string, char search);
   static const char* strrchr(const char* string, char search);
   static bool isalnum(char achar);
   static bool issign(char achar) { return ((achar == '+') || (achar == '-')); }
   static bool ispoint(char achar) { return (achar == '.'); }
   static bool isexponent(char achar) { return (achar == 'e') || (achar == 'E'); }
   static bool ispositive(char achar) { return achar == '+'; }
   static bool isnegative(char achar) { return achar == '-'; }
   static bool isdigit(char achar);
   static int getDigit(char achar)
      {  return (achar >= '0' && achar <= '9') ? (int) achar - '0' : 0; }
   static bool isoctaldigit(char achar)
      {  return (achar >= '0' && achar <= '7'); }
   static int getOctalDigit(char achar)
      {  return (achar >= '0' && achar <= '7') ? (int) achar - '0' : 0; }
   static bool ishexadigit(char achar)
      {  return (achar >= '0' && achar <= '9') || (achar >= 'a' && achar <= 'f')
            || (achar >= 'A' && achar <= 'F');
      }
   static int getHexaDigit(char achar)
      {  return (achar >= '0' && achar <= '9') ? (int) achar - '0'
            : ((achar >= 'a' && achar <= 'f') ? (int) (achar-'a') + 10
            : ((achar >= 'A' && achar <= 'F') ? (int) (achar-'A') + 10 : 0));
      }
   static char convertHexa(int aint)
      {  AssumeCondition(aint >= 0 && aint < 16)
         return (aint < 10) ? (char) ('0' + aint) : (char) ('A' + aint - 10);
      }
   static int strncmp(const char* s1, const char* s2, int maxlen);
   static int strnicmp(const char *s1, const char *s2, int maxlen);
   static ComparisonResult compare(char c1, char c2)
      {  return (c1 < c2) ? CRLess : ((c1 > c2) ? CRGreater : CREqual); }
   static int strlen(const char* string);
   static int atoi(const char* s1);
   static unsigned int atou(const char* s1);
   static long int atol(const char* s1);
   static unsigned long int atoul(const char* s1);
   static bool isspace(char achar);
   static char space() { return ' '; }
   static void itoa(int source, char* string);
   static void utoa(unsigned int source, char* string);
   static void ltoa(long int source, char* string);
   static void ultoa(unsigned long int source, char* string);
   static void gcvt(double adouble, char* string);
   static double strtod(const char *s, char **endptr);
   static bool islower(char achar) { return (achar >='a') && (achar <= 'z'); }
   static bool isupper(char achar) { return (achar >='A') && (achar <= 'Z'); }
   static char tolower(char achar);
   static char toupper(char achar);
   static inherited::Strings trueStrings();
   static inherited::Strings falseStrings();

   typedef DSubString::SearchParameters SearchParameters;
   class ScanResult {
     private:
      int uResultPosition;
      int uPatternPosition;
      int* aBadChar;
      int* aGoodSuffix;
      bool fFound;

      ScanResult(const ScanResult& source) = delete;
      ScanResult& operator=(const ScanResult& source) = delete;

     public:
      ScanResult()
         :  uResultPosition(-1), uPatternPosition(0), aBadChar(nullptr), aGoodSuffix(nullptr), fFound(false) {}
      ScanResult(ScanResult&& source)
         :  uResultPosition(source.uResultPosition), uPatternPosition(source.uPatternPosition),
            aBadChar(source.aBadChar), aGoodSuffix(source.aGoodSuffix), fFound(source.fFound)
         {  const_cast<ScanResult&>(source).aBadChar = nullptr;
            const_cast<ScanResult&>(source).aGoodSuffix = nullptr;
         }
      ~ScanResult()
         {  if (aBadChar) delete [] aBadChar;
            if (aGoodSuffix) delete [] aGoodSuffix;
         }
      ScanResult& operator=(ScanResult&& source)
         {  uResultPosition = source.uResultPosition;
            uPatternPosition = source.uPatternPosition;
            aBadChar = source.aBadChar;
            aGoodSuffix = source.aGoodSuffix;
            const_cast<ScanResult&>(source).aBadChar = nullptr;
            const_cast<ScanResult&>(source).aGoodSuffix = nullptr;
            fFound = source.fFound;
            return *this;
         }

      friend class TSubStringTraits<char>;

      void setVariant(const char* pattern, int patternLength, const SearchParameters& params);
      int result() const { return uResultPosition; }
      bool isValid() const { return fFound; }
   };

   static void scanPartial(char* string, int length, const char* pattern,
         int patternLength, const SearchParameters& params, ScanResult& result);
};

template <typename TypeChar, template <typename TChar> class TypePolicy, class SubStringTraits>
class TString;
   
template <typename TypeChar, template <typename TChar> class TypePolicy = GlobalPolicy,
   class SubStringTraits = TSubStringTraits<TypeChar> >
class TSubString : public TypePolicy<TypeChar>::Repository::BaseSubString {
  private:
   typedef TypePolicy<TypeChar> Policy;

  public:
   typedef TypeChar Char;
   typedef typename SubStringTraits::CharResult CharResult;
   typedef DSubString::SearchParameters SearchParameters;
   typedef TString<TypeChar, TypePolicy, SubStringTraits> String;
   typedef SubStringTraits Traits;
   typedef typename Traits::CharTraits CharTraits;
   typedef typename Policy::Repository::Position Position;
   typedef typename SubStringTraits::CharSet CharSet;
   typedef typename SubStringTraits::ECharSet ECharSet;

  private:
   typedef typename Policy::Repository Repository;
   typedef typename Repository::MoveNotification MoveNotification;

#ifndef _MSC_VER
   typedef typename Repository::BaseSubString inherited;
#else
   typedef typename TypePolicy<TypeChar>::Repository::BaseSubString inherited;
#endif
   typedef TSubString<TypeChar, TypePolicy, SubStringTraits> thisType;

   enum TypeBoundary { TBRestricted=0, TBClosed=1, TBOpened=2, TBInfinite=3 };
   enum Operation { OCopy, OCat, OInsert };
   thisType& setInteger(int source, Operation operation);
   thisType& setUInteger(unsigned int source, Operation operation);
   thisType& setHexaInteger(int source, Operation operation);
   thisType& setHexaUInteger(unsigned int source, Operation operation);
   thisType& setLInteger(long int source, Operation operation);
   thisType& setULInteger(unsigned long int source, Operation operation);
   thisType& setHexaLInteger(long int source, Operation operation);
   thisType& setHexaULInteger(unsigned long int source, Operation operation);
   thisType& setFloat(double source, Operation operation);
   
   void move(Repository& repository, MoveNotification& notification, int oldLength)
      {  if (!repository.move(notification)) {
            end().plusAssign(length());
            if (!isUpperOpened() && (uTypeLength > (unsigned int) CTypeMask))
               end().dec();
         }
         else if ((oldLength == 0) && !isUpperOpened() && (uTypeLength > (unsigned int) CTypeMask))
            end().dec();
         else if ((oldLength > 0) && (uTypeLength <= (unsigned int) TBClosed))
            end().setToStart();
      }

  private:
   unsigned int uTypeLength;

   static const int CTypeMask = 0x03;
   static const int CTypeShift = 2;

   TSubString() : uTypeLength(0) {}
   
  protected:
   class ReplaceNotification;
   class InsertionNotification;
   class SuppressionNotification;
   friend class ReplaceNotification;
   friend class InsertionNotification;
   friend class SuppressionNotification;

   typename inherited::WriteStartPosition start() { return inherited::start(); }
   typename inherited::WriteEndPosition end() { return inherited::end(); }

  public:
   typename inherited::ReadStartPosition start() const { return inherited::start(); }
   typename inherited::ReadStartPosition cstart() const { return inherited::start(); }

  protected:
   TSubString(Repository* newRepository)
      :  inherited(newRepository, PNT::Pointer::Init()), uTypeLength(TBInfinite)
      {  start().setToBegin(); end().setToEnd();
         uTypeLength |= (newRepository->length() << CTypeShift);
      }

   class Allocation {
     private:
      int uSize;

     public:
      Allocation(int size) : uSize(size) {}
      friend class TSubString<TypeChar, TypePolicy, SubStringTraits>;
   };
   
   TSubString(Repository* newRepository, const Allocation& allocation)
      : inherited(newRepository, PNT::Pointer::Init()), uTypeLength(TBInfinite)
      {  try {
            newRepository->realloc(allocation.uSize);
            start().setToBegin();
            end().setToEnd();
          }
         catch(...) { inherited::release(); throw; }
      }
   TSubString(const thisType& source, int first, TypeBoundary type);

   TypeBoundary getUpperType() const { return (TypeBoundary) (uTypeLength & CTypeMask); }
   thisType& setUpperType(TypeBoundary type)
      {  auto oldType = (TypeBoundary) (uTypeLength & CTypeMask);
         uTypeLength &= ~CTypeMask;
         uTypeLength |= type;
         if (type == TBInfinite) {
            const auto& repository = getRepository();
            uTypeLength = ((repository.length() - start().getPosition()) << CTypeShift) | TBInfinite;
            end().setToEnd();
         }
         else if ((type == TBOpened) && (oldType <= TBClosed)) {
            if (uTypeLength > (unsigned int) CTypeMask)
               end().inc();
         }
         else if ((type <= TBClosed) && (oldType >= TBOpened)) {
            if (uTypeLength > (unsigned int) CTypeMask)
               end().dec();
         };
         return *this;
      }

   Repository& getRepository() const { return inherited::getElement(); }
   TypeChar getStringChar(int pos) const
      {  TypeChar achar = (TypeChar) 0;
         if ((pos >= 0) && (pos < length())) {
            Position position;
            start().retrievePosition(position);
            const auto& repository = getRepository();
            position.plusAssign(pos, repository);
            achar = repository[position];
         };
         return achar;
      }
   void setStringChar(int pos, const TypeChar& value)
      {  AssumeCondition((pos >= 0) && (pos < length()))
         Position position;
         start().retrievePosition(position);
         Repository& repository = getRepository();
         position.plusAssign(pos, repository);
         repository[position] = value;
      }

   /* comparison methods */
   virtual ComparisonResult _compare(const EnhancedObject& asource) const override
      {  ComparisonResult result = inherited::_compare(asource);
         return (result == CREqual)
            ? compare((const thisType&) inherited::castFromCopyHandler(asource), ComparisonMode())
            : result;
      }
   ComparisonResult compare(const TChunk<TypeChar>& chunk, const ComparisonMode& mode) const;
   ComparisonResult compareBounded(const TChunk<TypeChar>& chunk, const ComparisonMode& mode) const;

   /* search methods */
   int scanPos(const TypeChar* search, const SearchParameters& params) const
      {  int result = -1;
         if (search != nullptr) {
            TChunk<TypeChar> pattern;
            pattern.string = const_cast<TypeChar*>(search);
            pattern.length = SubStringTraits::strlen((const TypeChar*) search);
            result = scanPos(pattern, params);
         }
         return result;
      }
   int scanPos(const TypeChar& search, const SearchParameters& params) const;
   int scanPos(const CharSet& search, const SearchParameters& params) const;
   int scanPos(const TChunk<TypeChar>& pattern, const SearchParameters& params) const;
   int scanPos(const thisType& search, const SearchParameters& params) const
      {  int result = -1;
         if (search.length() > 0) {
            TChunk<TypeChar> pattern = search.getChunk();
            if ((int) pattern.length == search.length())
               result = scanPos(pattern, params);
            else {
               pattern.length = search.length();
               pattern.string = search.createString();
               try {
                  result = scanPos(pattern, params);
               }
               catch (...) {
                  if (pattern.string) delete [] pattern.string;
                  throw;
               };
               if (pattern.string) delete [] pattern.string;
            };
         };
         return result;
      }

  public:
   TSubString(const thisType& source, int first, int len);
   TSubString(const thisType& source, int first=0);
   virtual ~TSubString() { inherited::release(); }
   thisType& operator=(const thisType& source);
   Template3DefineCopy(TSubString, TypeChar, TypePolicy, SubStringTraits)
   DTemplate3DefineAssign(TSubString, TypeChar, TypePolicy, SubStringTraits)
   DTemplate3Compare(TSubString, TypeChar, TypePolicy, SubStringTraits)
   void swap(thisType& source)
      {  inherited::swap(source);
         unsigned int temp = uTypeLength;
         uTypeLength = source.uTypeLength;
         source.uTypeLength = temp;
      }

   virtual bool isValid() const override
      {  bool result = inherited::isValid() && inherited::doesSupportNotificationLength(
            length() + ((isUpperOpened() && length() > 0) ? 1 : 0));
         if (result) {
            Position endPos, startPos;
            start().retrievePosition(endPos);
            start().retrievePosition(startPos);
            result = !endPos.plusAssign(length(), getRepository())
               && startPos.isValid(getRepository());
         };
         return result;
      }

   /* public low level get and set methods */
   thisType& setUpperInfinite()    { setUpperType(TBInfinite); return *this; }
   thisType& setUpperOpened()      { setUpperType(TBOpened); return *this; }
   thisType& setUpperClosed()      { setUpperType(TBClosed); return *this; }
   thisType& setUpperRestricted()  { setUpperType(TBRestricted); return *this; }
   
   bool isUpperInfinite() const { return ((uTypeLength & TBInfinite) == TBInfinite); }
   bool isUpperRestricted() const { return !(uTypeLength & TBInfinite); }
   bool isUpperBounded() const { return ((uTypeLength & TBInfinite) < TBInfinite); }
   bool isUpperClosed() const  { return !(uTypeLength & TBOpened); }
   bool isUpperOpened() const  { return (uTypeLength & TBOpened); }
   bool isEmpty() const { return uTypeLength <= TBInfinite; }
   bool hasControlOnRepository() const { return getRepository().isSingleReferenced(); }

      // access to the length of the sub-string
   int length() const { return (int) (uTypeLength >> CTypeShift); }
   int getPlace() const { return getRepository().getPlace(getStartPosition()); }
   void bookPlace(int places)
      {  Position startPos;
         start().retrievePosition(startPos);
         if (!getRepository().bookPlace(startPos, places)) {
            start().clearToPosition(startPos);
            if (length() > 0)
               end().plusAssign(length() + (isUpperOpened() ? 1 : 0));
         };
      }
   thisType& setLength(int len);
   thisType& clear();
   thisType& setUpperBoundTo(const thisType& source);
   thisType& setLowerBoundTo(const thisType& source)
      {  if (inherited::key() == source.inherited::key()) {
            const Repository& repository = getRepository();
            Position endPosSource, endPosThis;
            source.start().retrievePosition(endPosSource);
            start().retrievePosition(endPosThis);
            endPosSource.plusAssign(source.length(), repository);
            endPosThis.plusAssign(length(), repository);
            int endSourceStart = endPosSource.minus(getStartPosition(), repository);
            int sourceStartEnd = source.getStartPosition().minus(endPosThis, repository);
            if ((endSourceStart >= -1) && (sourceStartEnd <= 1)) {
               int diff = start().minus(source.start());
               if (diff < 0) {
                  if ((sourceStartEnd == -1) && !isUpperOpened())
                     end().inc();
                  start().increaseToPosition(source.getStartPosition());
               }
               else if (diff > 0) {
                  start().decreaseToPosition(source.getStartPosition());
                  if (uTypeLength <= TBClosed)
                     end().dec();
               };
               uTypeLength += (diff << CTypeShift);
            };
         };
         return *this;
      }
   thisType& restrictToLength(int nbChars=1)
      {  return (nbChars > 0) ? setLength(length()-nbChars) : *this; }

      // access to the sub-strings
   bool contain(const thisType& source) const
      {  bool result = (inherited::key() == source.key());
         if (result) {
            const auto& repository = getRepository();
            ComparisonResult compareStart = source.start().compare(start());
            result = (compareStart == CRGreater) || (compareStart == CREqual);
            if (result) {
               Position endSource, endThis;
               source.start().retrievePosition(endSource);
               start().retrievePosition(endThis);
               endSource.plusAssign(source.length(), repository);
               endThis.plusAssign(length(), repository);
               ComparisonResult compare = endSource.compare(endThis, repository);
               result = (compare == CRLess) || ((compare == CREqual)
                     && (source.getUpperType() <= getUpperType()));
            };
         };
         return result;
      }
   int getSubStringPos(const thisType& subString) const
      {  return (contain(subString)) ? subString.start().minus(start()) : -1; }

      // position methods
   thisType& advance(int pos=1)
      {  Position oldStart;
         start().retrievePosition(oldStart);
         Position newStart = oldStart;
         int subLength = !newStart.plusAssign(pos, getRepository()) ? pos
            : (newStart.minus(oldStart, getRepository()));
         if (subLength <= length()) {
            if (subLength < 0) {
               start().decreaseToPosition(newStart);
               if (uTypeLength <= TBClosed)
                  end().dec();
            }
            else if (subLength > 0) {
               if (!isUpperOpened() && (length() == subLength))
                  end().inc();
               start().increaseToPosition(newStart);
            };
            uTypeLength += ((-subLength) << CTypeShift);
         }
         else { // subLength > length()
            end().increaseToPosition(newStart);
            start().increaseToPosition(newStart);
            uTypeLength &= CTypeMask;
         }
         return *this;
      }
   thisType& setToBegin(const thisType& subString)
      {  AssumeCondition(inherited::key() == subString.key())
         return advance(subString.start().minus(((const thisType&) *this).start()));
      }
   thisType& setToEnd(const thisType& subString)
      {  AssumeCondition(inherited::key() == subString.key())
         return advance((subString.start().minus(((const thisType&) *this).start())) + subString.length());
      }
   thisType& setToEnd()
      {  if (!isUpperOpened())
            end().inc();
         start().plusAssign(length());
         uTypeLength = uTypeLength & CTypeMask;
         return *this;
      }
   thisType& setToSupport()
      {  start().setToBegin();
         end().setToEnd();
         uTypeLength &= CTypeMask;
         uTypeLength |= getRepository().length() << CTypeShift;
         if (!isUpperOpened())
            end().dec();
         return *this;
      }
   int seek(int offset, int origin);

   /* comparison methods */
   ComparisonResult compareFirstPosition(const thisType& source) const
      {  AssumeCondition(&getRepository() == &source.getRepository())
         return start().compare(source.start());
      }
   ComparisonResult compare(const TypeChar* source) const
      {  return compare(source, ComparisonMode()); }
   ComparisonResult compare(const thisType& source, const ComparisonMode& mode) const
      {  ComparisonResult lengthCompare = fcompare(length(), source.length());
         ComparisonResult result = compare(source, (lengthCompare == CRLess)
               ? length() : source.length(), mode);
         return (result != CREqual) ? result : lengthCompare;
      }
   ComparisonResult compare(const TypeChar* source, const ComparisonMode& mode) const
      {  return source
            ? compare(TChunk<TypeChar>().setString(const_cast<TypeChar*>(source))
               .setLocalLength(SubStringTraits::strlen(source)), mode)
            : ((uTypeLength > (unsigned int) CTypeMask) ? CRGreater : CREqual);
      }
   ComparisonResult compareI(const thisType& source) const
      {  return compare(source, ComparisonMode().setCaseInsensitive()); }
   ComparisonResult compareI(const TypeChar* source) const
      {  return source
            ? compare(TChunk<TypeChar>().setString(const_cast<TypeChar*>(source))
                  .setLocalLength(SubStringTraits::strlen(source)), ComparisonMode().setCaseInsensitive())
            : ((uTypeLength > (unsigned int) CTypeMask) ? CRGreater : CREqual);
      }
   ComparisonResult compare(const thisType& source, int len, const ComparisonMode& mode) const;
   ComparisonResult compare(const thisType& source, int len) const
      {  return compare(source, len, ComparisonMode()); }
   ComparisonResult compare(const TypeChar* source, int len) const
      {  return compareBounded(TChunk<TypeChar>().setString(const_cast<TypeChar*>(source))
                  .setLocalLength(len), ComparisonMode());
      }
   ComparisonResult compareI(const thisType& source, int len) const
      {  return compare(source, len, ComparisonMode().setCaseInsensitive()); }
   ComparisonResult compareI(const TypeChar* source, int len) const
      {  return compareBounded(TChunk<TypeChar>().setString(const_cast<TypeChar*>(source))
                  .setLocalLength(len), ComparisonMode().setCaseInsensitive());
      }
   ComparisonResult compareSub(const thisType& source) const
      {  ComparisonResult result = fcompare(length(), source.length());
         return (compare(source, (result == CRLess) ? length() : source.length(),
               ComparisonMode()) != CREqual) ? CRNonComparable : result;
      }
   ComparisonResult compareSub(const TypeChar* source) const
      {  int sourceLen = SubStringTraits::strlen(source);
         ComparisonResult result = fcompare(length(), sourceLen);
         return (compareBounded(TChunk<TypeChar>().setString(const_cast<TypeChar*>(source))
                  .setLocalLength((result == CRLess) ? length() : sourceLen), ComparisonMode())
               != CREqual) ? CRNonComparable : result;
      }
   ComparisonResult compareISub(const thisType& source) const
      {  ComparisonResult result = fcompare(length(), source.length());
         return (compare(source, (result == CRLess) ? length() : source.length(),
               ComparisonMode().setCaseInsensitive()) != CREqual) ? CRNonComparable : result;
      }
   ComparisonResult compareISub(const TypeChar* source) const
      {  int sourceLen = SubStringTraits::strlen(source);
         ComparisonResult result = fcompare(length(), sourceLen);
         return (compareBounded(TChunk<TypeChar>().setString(const_cast<TypeChar*>(source))
                  .setLocalLength((result == CRLess) ? length() : sourceLen),
               ComparisonMode().setCaseInsensitive()) != CREqual) ? CRNonComparable : result;
      }

   bool operator<(const TypeChar* source) const { return compare(source) == CRLess; }
   bool operator<=(const TypeChar* source) const
      {  ComparisonResult result = compare(source);
         return ((result == CRLess) || (result == CREqual));
      }
   bool operator>(const TypeChar* source) const
      {  return compare(source) == CRGreater; }
   bool operator>=(const TypeChar* source) const
      {  ComparisonResult result = compare(source);
         return ((result == CRGreater) || (result == CREqual));
      }
   bool operator==(const TypeChar* source) const
      {  return compare(source) == CREqual; }
   bool operator!=(const TypeChar* source) const
      {  ComparisonResult result = compare(source);
         return ((result == CRGreater) || (result == CRLess));
      }

   /* conversion methods */
   TypeChar* createString() const;
   TChunk<TypeChar> getChunk() const
      {  TChunk<TypeChar> chunk;
         inherited::getElement().retrieveChunk(getStartPosition(), chunk);
         int len = length();
         if ((int) chunk.length > len)
            chunk.length = len;
         return chunk;
      }
   Position getStartPosition() const
      {  Position startPos;
         start().retrievePosition(startPos);
         return startPos;
      }
   void retrieveChunk(const Position& position, TChunk<TypeChar>& chunk) const
      {  inherited::getElement().retrieveChunk(position, chunk); }
   bool setToNextChunk(Position& position, TChunk<TypeChar>& chunk) const
      {  return inherited::getElement().setToNextChunk(position, chunk); }
   int copyTo(TypeChar* destination, int len) const;

   thisType& setChar(const TypeChar& achar)
      {  if (length() > 1)
            getRepository()[getStartPosition()] = achar;
         return *this;
      }
   thisType& setChar(const TypeChar& achar, int pos)
      {  if ((pos >= 0) && (pos < length())) {
            Position position;
            start().retrievePosition(position);
            Repository& repository = getRepository();
            position.plusAssign(pos, repository);
            repository[position] = achar;
         };
         return *this;
      }
   typename SubStringTraits::CharResult getChar() const
      {  typename SubStringTraits::CharResult result;
         if (uTypeLength >= (1 << CTypeShift))
            result.setChar(getRepository()[getStartPosition()]);
         return result;
      }
   typename SubStringTraits::CharResult getChar(int pos) const
      {  const Repository& repository = getRepository();
         typename SubStringTraits::CharResult result;
         if (pos < (int) length()) {
            Position position;
            start().retrievePosition(position);
            position.plusAssign(pos, repository);
            result.setChar(repository[position]);
         };
         return result;
      }
   const TypeChar& getSChar() const
      {  AssumeCondition(uTypeLength >= (1 << CTypeShift))
         return getRepository()[getStartPosition()];
      }
   const TypeChar& getSChar(int pos) const
      {  AssumeCondition(pos < length())
         Position position;
         start().retrievePosition(position);
         position.plusAssign(pos, getRepository());
         return getRepository()[position];
      }
   typename SubStringTraits::CharResult operator[](int pos) const { return getChar(pos); }
   operator TypeChar() const
      {  return (uTypeLength >= (1 << CTypeShift))
            ? getRepository()[getStartPosition()] : (TypeChar) 0;
      }
   typename SubStringTraits::CharResult extractChar()
      {  typename SubStringTraits::CharResult result;
         if (uTypeLength >= (1 << CTypeShift)) {
            const Repository& repository = getRepository();
            Position startPos;
            start().retrievePosition(startPos);
            result.setChar(repository[startPos]);
            if (!isUpperOpened() && (uTypeLength < (2 << CTypeShift)))
               end().inc();
            start().inc();
            uTypeLength -= (1 << CTypeShift);
         };
         return result;
      }

   bool queryBoolean() const
      {  return (elementOfI(SubStringTraits::trueStrings())) ? true
            : ((elementOfI(SubStringTraits::falseStrings())) ? false
            : (bool) queryUInteger());
      }
   thisType& setBoolean(bool source)
      {  return source ? copy(SubStringTraits::trueStrings().getString(0))
            : copy(SubStringTraits::falseStrings().getString(0));
      }
   bool readBoolean();

   int queryInteger() const;
   unsigned queryUInteger() const;
   long int queryLInteger() const;
   unsigned long queryULInteger() const;
   operator int() const { return queryInteger(); }

   thisType& setInteger(int source) { return setInteger(source, OCopy); }
   thisType& setUInteger(unsigned int source) { return setUInteger(source, OCopy); }
   thisType& setLInteger(long int source) { return setLInteger(source, OCopy); }
   thisType& setULInteger(unsigned long int source) { return setULInteger(source, OCopy); }
   int readInteger();
   unsigned int readUInteger();
   long int readLInteger();
   unsigned long int readULInteger();
   unsigned int queryHexaUInteger() const;
   int queryHexaInteger() const { return queryHexaUInteger(); }
   unsigned long int queryHexaULInteger() const;
   long int queryHexaLInteger() const { return queryHexaULInteger(); }
   thisType& setHexaInteger(int source) { return setHexaInteger(source, OCopy); }
   thisType& setHexaUInteger(unsigned int source) { return setHexaUInteger(source, OCopy); }
   thisType& setHexaLInteger(long int source) { return setHexaLInteger(source, OCopy); }
   thisType& setHexaULInteger(unsigned long int source) { return setHexaULInteger(source, OCopy); }
   int readHexaInteger();
   unsigned int readHexaUInteger();
   long int readHexaLInteger();
   unsigned long int readHexaULInteger();

   double queryFloat() const;
   operator double() const { return queryFloat(); }
   thisType& setFloat(double source) { return setFloat(source, OCopy); }
   double readFloat();

   /* extraction of sub-strings */
   PNT::PassPointer<thisType> newSubString(int start=0) const
      {  return PNT::PassPointer<thisType>(new thisType(*this, start), PNT::Pointer::Init()); }
   PNT::PassPointer<thisType> newSubString(int start, int len) const
      {  return PNT::PassPointer<thisType>(new thisType(*this, start, len), PNT::Pointer::Init()); }
   PNT::PassPointer<thisType> newString(int start=0) const
      {  return newString(start, length() - start); }
   PNT::PassPointer<thisType> newString(int start, int len) const;

   /* read and write stream methods */
   DSubString::PersistencyAccess::TStringProperty<thisType> asPersistent() const
      {  return DSubString::PersistencyAccess::TStringProperty<thisType>(*this); }

   void read(std::basic_istream<TypeChar>& in);
   void read(std::basic_istream<TypeChar>& in, int maxChars);
   void read(std::basic_istream<TypeChar>& in, int maxChars, TypeChar delim);
   void read(FILE* file, int chars);
   void write(std::basic_ostream<TypeChar>& out) const;
   void write(std::basic_ostream<TypeChar>& out, int maxChars) const;

   /* search methods */
   PNT::PassPointer<thisType> scan(const thisType& search, const SearchParameters& params) const
      {  int pos = scanPos(search, params);
         PNT::PassPointer<thisType> result;
         if (pos >= 0) 
            result = params.isBound() ? newSubString(pos, search.length()) : newSubString(pos);
         return result;
      }
   PNT::PassPointer<thisType> scan(const thisType& search) const
      {  int pos = scanPos(search, SearchParameters());
         PNT::PassPointer<thisType> result;
         if (pos >= 0)
            result = newSubString(pos);
         return result;
      }
   PNT::PassPointer<thisType> scanI(const thisType& search) const
      {  int pos = scanPos(search, SearchParameters().setCaseInsensitive());
         PNT::PassPointer<thisType> result;
         if (pos >= 0)
            result = newSubString(pos);
         return result;
      }
   PNT::PassPointer<thisType> scanB(const thisType& search) const
      {  int pos = scanPos(search, SearchParameters());
         PNT::PassPointer<thisType> result;
         if (pos >= 0) 
            result = newSubString(pos, search.length());
         return result;
      }
   PNT::PassPointer<thisType> scanBI(const thisType& search) const
      {  int pos = scanPos(search, SearchParameters().setCaseInsensitive());
         PNT::PassPointer<thisType> result;
         if (pos >= 0) 
            result = newSubString(pos, search.length());
         return result;
      }
   PNT::PassPointer<thisType> scanR(const thisType& search) const
      {  int pos = scanPos(search, SearchParameters().setBackward());
         PNT::PassPointer<thisType> result;
         if (pos >= 0)
            result = newSubString(pos);
         return result;
      }
   PNT::PassPointer<thisType> scanRI(const thisType& search) const
      {  int pos = scanPos(search, SearchParameters().setBackward().setCaseInsensitive());
         PNT::PassPointer<thisType> result;
         if (pos >= 0)
            result = newSubString(pos);
         return result;
      }
   PNT::PassPointer<thisType> scanBR(const thisType& search, const ComparisonMode& mode) const
      {  int pos = scanPos(search, SearchParameters(mode).setBackward());
         PNT::PassPointer<thisType> result;
         if (pos >= 0)
            result = newSubString(pos, search.length());
         return result;
      }
   PNT::PassPointer<thisType> scanBRI(const thisType& search) const
      {  int pos = scanPos(search, SearchParameters().setBackward().setCaseInsensitive());
         PNT::PassPointer<thisType> result;
         if (pos >= 0)
            result = newSubString(pos, search.length());
         return result;
      }

   PNT::PassPointer<thisType> scan(const TypeChar* search, const SearchParameters& params) const
      {  TChunk<TypeChar> chunk;
         int pos = scanPos(chunk.setString(const_cast<TypeChar*>(search)).setLocalLength(
                  SubStringTraits::strlen(search)), params);
         PNT::PassPointer<thisType> result;
         if (pos >= 0)
            result = params.isBound() ? newSubString(pos, chunk.length) : newSubString(pos);
         return result;
      }
   PNT::PassPointer<thisType> scan(const TypeChar* search) const
      {  TChunk<TypeChar> chunk;
         int pos = scanPos(chunk.setString(const_cast<TypeChar*>(search)).setLocalLength(
                  SubStringTraits::strlen(search)), SearchParameters());
         PNT::PassPointer<thisType> result;
         if (pos >= 0)
            result = newSubString(pos);
         return result;
      }
   PNT::PassPointer<thisType> scanI(const TypeChar* search) const
      {  TChunk<TypeChar> chunk;
         int pos = scanPos(chunk.setString(const_cast<TypeChar*>(search)).setLocalLength(
                  SubStringTraits::strlen(search)), SearchParameters().setCaseInsensitive());
         PNT::PassPointer<thisType> result;
         if (pos >= 0)
            result = newSubString(pos);
         return result;
      }
   PNT::PassPointer<thisType> scanB(const TypeChar* search) const
      {  TChunk<TypeChar> chunk;
         int pos = scanPos(chunk.setString(const_cast<TypeChar*>(search)).setLocalLength(
                  SubStringTraits::strlen(search)), SearchParameters());
         PNT::PassPointer<thisType> result;
         if (pos >= 0)
            result = newSubString(pos, chunk.length);
         return result;
      }
   PNT::PassPointer<thisType> scanBI(const TypeChar* search) const
      {  TChunk<TypeChar> chunk;
         int pos = scanPos(chunk.setString(const_cast<TypeChar*>(search)).setLocalLength(
                  SubStringTraits::strlen(search)), SearchParameters().setCaseInsensitive());
         PNT::PassPointer<thisType> result;
         if (pos >= 0)
            result = newSubString(pos, chunk.length);
         return result;
      }
   PNT::PassPointer<thisType> scanR(const TypeChar* search) const
      {  TChunk<TypeChar> chunk;
         int pos = scanPos(chunk.setString(const_cast<TypeChar*>(search)).setLocalLength(
                  SubStringTraits::strlen(search)), SearchParameters().setBackward());
         PNT::PassPointer<thisType> result;
         if (pos >= 0)
            result = newSubString(pos);
         return result;
      }
   PNT::PassPointer<thisType> scanRI(const TypeChar* search) const
      {  TChunk<TypeChar> chunk;
         int pos = scanPos(chunk.setString(const_cast<TypeChar*>(search))
               .setLocalLength(SubStringTraits::strlen(search)),
               SearchParameters().setBackward().setCaseInsensitive());
         PNT::PassPointer<thisType> result;
         if (pos >= 0)
            result = newSubString(pos);
         return result;
      }
   PNT::PassPointer<thisType> scanBR(const TypeChar* search) const
      {  TChunk<TypeChar> chunk;
         int pos = scanPos(chunk.setString(const_cast<TypeChar*>(search)).setLocalLength(
                  SubStringTraits::strlen(search)), SearchParameters().setBackward());
         PNT::PassPointer<thisType> result;
         if (pos >= 0)
            result = newSubString(pos, chunk.length);
         return result;
      }
   PNT::PassPointer<thisType> scanBRI(const TypeChar* search) const
      {  TChunk<TypeChar> chunk;
         int pos = scanPos(chunk.setString(const_cast<TypeChar*>(search))
                  .setLocalLength(SubStringTraits::strlen(search)),
               SearchParameters().setBackward().setCaseInsensitive());
         PNT::PassPointer<thisType> result;
         if (pos >= 0)
            result = newSubString(pos, chunk.length);
         return result;
      }

   PNT::PassPointer<thisType> scan(const TypeChar& search, const SearchParameters& params) const
      {  int pos = scanPos(search, params);
         PNT::PassPointer<thisType> result;
         if (pos >= 0)
            result = params.isBound() ? newSubString(pos, 1) : newSubString(pos);
         return result;
      }
   PNT::PassPointer<thisType> scan(const TypeChar& search) const
      {  int pos = scanPos(search, SearchParameters());
         PNT::PassPointer<thisType> result;
         if (pos >= 0) 
            result = newSubString(pos);
         return result;
      }
   PNT::PassPointer<thisType> scanI(const TypeChar& search) const
      {  int pos = scanPos(search, SearchParameters().setCaseInsensitive());
         PNT::PassPointer<thisType> result;
         if (pos >= 0)
            result = newSubString(pos);
         return result;
      }
   PNT::PassPointer<thisType> scanB(const TypeChar& search) const
      {  int pos = scanPos(search, SearchParameters());
         PNT::PassPointer<thisType> result;
         if (pos >= 0)
            result = newSubString(pos, 1);
         return result;
      }
   PNT::PassPointer<thisType> scanBI(const TypeChar& search) const
      {  int pos = scanPos(search, SearchParameters().setCaseInsensitive());
         PNT::PassPointer<thisType> result;
         if (pos >= 0)
            result = newSubString(pos, 1);
         return result;
      }
   PNT::PassPointer<thisType> scanR(const TypeChar& search) const
      {  int pos = scanPos(search, SearchParameters().setBackward());
         PNT::PassPointer<thisType> result;
         if (pos >= 0)
            result = newSubString(pos);
         return result;
      }
   PNT::PassPointer<thisType> scanRI(const TypeChar& search) const
      {  int pos = scanPos(search, SearchParameters().setBackward().setCaseInsensitive());
         PNT::PassPointer<thisType> result;
         if (pos >= 0)
            result = newSubString(pos);
         return result;
      }
   PNT::PassPointer<thisType> scanBR(const TypeChar& search) const
      {  int pos = scanPos(search, SearchParameters().setBackward());
         PNT::PassPointer<thisType> result;
         if (pos >= 0)
            result = newSubString(pos, 1);
         return result;
      }
   PNT::PassPointer<thisType> scanBRI(const TypeChar& search) const
      {  int pos = scanPos(search, SearchParameters().setBackward().setCaseInsensitive());
         PNT::PassPointer<thisType> result;
         if (pos >= 0)
            result = newSubString(pos, 1);
         return result;
      }
   
   PNT::PassPointer<thisType> scan(const CharSet& search, const SearchParameters& params) const
      {  int pos = scanPos(search, params);
         PNT::PassPointer<thisType> result;
         if (pos >= 0)
            result = params.isBound() ? newSubString(pos, 1) : newSubString(pos);
         return result;
      }
   PNT::PassPointer<thisType> scan(const CharSet& search) const
      {  int pos = scanPos(search, SearchParameters());
         PNT::PassPointer<thisType> result;
         if (pos >= 0)
            result = newSubString(pos);
         return result;
      }
   PNT::PassPointer<thisType> scanI(const CharSet& search) const
      {  int pos = scanPos(search, SearchParameters().setCaseInsensitive());
         PNT::PassPointer<thisType> result;
         if (pos >= 0)
            result = newSubString(pos);
         return result;
      }
   PNT::PassPointer<thisType> scanB(const CharSet& search) const
      {  int pos = scanPos(search, SearchParameters());
         PNT::PassPointer<thisType> result;
         if (pos >= 0)
            result = newSubString(pos, 1);
         return result;
      }
   PNT::PassPointer<thisType> scanBI(const CharSet& search) const
      {  int pos = scanPos(search, SearchParameters().setCaseInsensitive());
         PNT::PassPointer<thisType> result;
         if (pos >= 0)
            result = newSubString(pos, 1);
         return result;
      }
   PNT::PassPointer<thisType> scanR(const CharSet& search) const
      {  int pos = scanPos(search, SearchParameters().setBackward());
         PNT::PassPointer<thisType> result;
         if (pos >= 0)
            result = newSubString(pos);
         return result;
      }
   PNT::PassPointer<thisType> scanRI(const CharSet& search) const
      {  int pos = scanPos(search, SearchParameters().setCaseInsensitive().setBackward());
         PNT::PassPointer<thisType> result;
         if (pos >= 0)
            result = newSubString(pos);
         return result;
      }
   PNT::PassPointer<thisType> scanBR(const CharSet& search) const
      {  int pos = scanPos(search, SearchParameters().setBackward());
         PNT::PassPointer<thisType> result;
         if (pos >= 0)
            result = newSubString(pos, 1);
         return result;
      }
   PNT::PassPointer<thisType> scanBRI(const CharSet& search) const
      {  int pos = scanPos(search, SearchParameters().setBackward().setCaseInsensitive());
         PNT::PassPointer<thisType> result;
         if (pos >= 0)
            result = newSubString(pos, 1);
         return result;
      }

   int scanPos(const thisType& search) const { return scanPos(search, SearchParameters()); }
   int scanIPos(const thisType& search) const { return scanPos(search, SearchParameters().setCaseInsensitive()); }
   int scanRPos(const thisType& search) const { return scanPos(search, SearchParameters().setBackward()); }
   int scanRIPos(const thisType& search) const { return scanPos(search, SearchParameters().setCaseInsensitive().setBackward()); }
   int scanPos(const TypeChar* search) const { return scanPos(search, SearchParameters()); }
   int scanIPos(const TypeChar* search) const { return scanPos(search, SearchParameters().setCaseInsensitive()); }
   int scanRPos(const TypeChar* search) const { return scanPos(search, SearchParameters().setBackward()); }
   int scanRIPos(const TypeChar* search) const { return scanPos(search, SearchParameters().setCaseInsensitive().setBackward()); }
   int scanPos(const TypeChar& search) const { return scanPos(search, SearchParameters()); }
   int scanIPos(const TypeChar& search) const { return scanPos(search, SearchParameters().setCaseInsensitive()); }
   int scanRPos(const TypeChar& search) const { return scanPos(search, SearchParameters().setBackward()); }
   int scanRIPos(const TypeChar& search) const { return scanPos(search, SearchParameters().setCaseInsensitive().setBackward()); }
   int scanPos(const CharSet& search) const { return scanPos(search, SearchParameters()); }
   int scanIPos(const CharSet& search) const { return scanPos(search, SearchParameters().setCaseInsensitive()); }
   int scanRPos(const CharSet& search) const { return scanPos(search, SearchParameters().setBackward()); }
   int scanRIPos(const CharSet& search) const { return scanPos(search, SearchParameters().setCaseInsensitive().setBackward()); }

   thisType& restrictTo(const thisType& search, const SearchParameters& params)
      {  int pos = scanPos(search, params);
         return (pos >= 0) ? setLength(pos) : *this;
      }
   thisType& restrictTo(const thisType& search)
      {  int pos = scanPos(search, SearchParameters());
         return (pos >= 0) ? setLength(pos) : *this;
      }
   thisType& restrictTo(const TypeChar* search, const SearchParameters& params)
      {  TChunk<TypeChar> chunk;
         int pos = scanPos(chunk.setString(const_cast<TypeChar*>(search)).setLocalLength(
                  SubStringTraits::strlen(search)), params);
         return (pos >= 0) ? setLength(pos) : *this;
      }
   thisType& restrictTo(const TypeChar* search)
      {  TChunk<TypeChar> chunk;
         int pos = scanPos(chunk.setString(const_cast<TypeChar*>(search)).setLocalLength(
                  SubStringTraits::strlen(search)), SearchParameters());
         return (pos >= 0) ? setLength(pos) : *this;
      }
   thisType& restrictTo(const TypeChar& search, const SearchParameters& params)
      {  int pos = scanPos(search, params);
         return (pos >= 0) ? setLength(pos) : *this;
      }
   thisType& restrictTo(const TypeChar& search)
      {  int pos = scanPos(search, SearchParameters());
         return (pos >= 0) ? setLength(pos) : *this;
      }
   thisType& restrictTo(const CharSet& search, const SearchParameters& params)
      {  int pos = scanPos(search, params);
         return (pos >= 0) ? setLength(pos) : *this;
      }
   thisType& restrictTo(const CharSet& search)
      {  int pos = scanPos(search, SearchParameters());
         return (pos >= 0) ? setLength(pos) : *this;
      }

   /* search a sub-string in a list of strings */
   bool elementOf(const TypeChar* string...) const;
   bool elementOf(const typename SubStringTraits::Strings& strings) const;
   bool elementOfI(const TypeChar* string...) const;
   bool elementOfI(const typename SubStringTraits::Strings& strings) const;

   /* methods that modify the repository */
      // assignment in the repository
   thisType& copy(const thisType& source) { return copy(source, source.length()); }
   thisType& copy(const TypeChar* source)
      { return (source) ? copy(source, (int) SubStringTraits::strlen(source)) : clear(); }
   thisType& copy(const thisType& source, int len) { return copy(source, 0, len); }
   thisType& copy(const thisType& source, int start, int len);
   thisType& copy(const TypeChar* source, int len);
   thisType& copy(const TypeChar& source);
   thisType& copy(const TypeChar& source, int number);
   thisType& copyFloat(double source) { return setFloat(source, OCopy); }
   thisType& copyInteger(int source) { return setInteger(source, OCopy); }
   thisType& copyUInteger(unsigned int source) { return setUInteger(source, OCopy); }
   thisType& copyLInteger(long int source) { return setLInteger(source, OCopy); }
   thisType& copyULInteger(unsigned long int source) { return setULInteger(source, OCopy); }
   thisType& copyHexaInteger(int source) { return setHexaInteger(source, OCopy); }
   thisType& copyHexaUInteger(unsigned int source) { return setHexaUInteger(source, OCopy); }
   thisType& copyHexaLInteger(long int source) { return setHexaLInteger(source, OCopy); }
   thisType& copyHexaULInteger(unsigned long int source) { return setHexaULInteger(source, OCopy); }
   thisType& operator=(const TypeChar* source) { return copy(source); }
   thisType& operator=(const TypeChar& source) { return copy(source); }

      // append in the repository
   thisType& cat(const thisType& source) { return cat(source, source.length()); }
   thisType& cat(const TypeChar* source)
      { return source ? cat(source, (int) SubStringTraits::strlen(source)) : cat(source, 0); }
   thisType& cat(const thisType& source, int len) { return cat(source, 0, len); }
   thisType& cat(const thisType& source, int start, int len);
   thisType& cat(const TypeChar* source, int len);
   thisType& cat(const TypeChar& source);
   thisType& cat(const TypeChar& source, int number);
   thisType& catFloat(double source) { return setFloat(source, OCat); }
   thisType& catInteger(int source) { return setInteger(source, OCat); }
   thisType& catUInteger(unsigned source) { return setUInteger(source, OCat); }
   thisType& catLInteger(long source) { return setLInteger(source, OCat); }
   thisType& catULInteger(unsigned long source) { return setULInteger(source, OCat); }
   thisType& catHexaInteger(int source) { return setHexaInteger(source, OCat); }
   thisType& catHexaUInteger(unsigned int source) { return setHexaUInteger(source, OCat); }
   thisType& catHexaLInteger(long int source) { return setHexaLInteger(source, OCat); }
   thisType& catHexaULInteger(unsigned long int source) { return setHexaULInteger(source, OCat); }
   thisType& operator+=(const thisType& source) { return cat(source); }
   thisType& operator+=(const TypeChar* source) { return cat(source); }
   thisType& operator+=(const TypeChar& source) { return cat(source); }

      // insert in the repository
   thisType& insert(const thisType& source) { return insert(source, source.length()); }
   thisType& insert(const TypeChar* source)
      { return (source) ? insert(source, (int) SubStringTraits::strlen(source)) : *this; }
   thisType& insert(const thisType& source, int len) { return insert(source, 0, len); }
   thisType& insert(const thisType& source, int start, int len);
   thisType& insert(const TypeChar* source, int len);
   thisType& insert(const TypeChar& achar);
   thisType& insert(const TypeChar& achar, int number);
   thisType& insertFloat(double source) { return setFloat(source, OInsert); }
   thisType& insertInteger(int source) { return setInteger(source, OInsert); }
   thisType& insertUInteger(unsigned int source) { return setUInteger(source, OInsert); }
   thisType& insertLInteger(long int source) { return setLInteger(source, OInsert); }
   thisType& insertULInteger(unsigned long int source) { return setULInteger(source, OInsert); }
   thisType& insertHexaInteger(int source) { return setHexaInteger(source, OInsert); }
   thisType& insertHexaUInteger(unsigned int source) { return setHexaUInteger(source, OInsert); }
   thisType& insertHexaLInteger(long int source) { return setHexaLInteger(source, OInsert); }
   thisType& insertHexaULInteger(unsigned long int source) { return setHexaULInteger(source, OInsert); }

      // replace in the repository
   class ExtendedReplaceParameters;
   thisType& replace(const thisType& source, const thisType& destination, const ExtendedReplaceParameters& params);
   thisType& replace(const TypeChar* source, const TypeChar* destination, const ExtendedReplaceParameters& params);
   thisType& replace(const TypeChar& source, const TypeChar& destination, const ExtendedReplaceParameters& params);
   thisType& replace(const CharSet& source, const TypeChar& destination, const ExtendedReplaceParameters& params);
   thisType& replaceAllChars(const TypeChar& source, int start=0) { return replaceAllChars(source, start, length()); }
   thisType& replaceAllChars(const TypeChar& source, int start, int len);
   thisType& upperCase();
   thisType& lowerCase();

     // suppression without any repository modification
   thisType& leftTrim();
   thisType& rightTrim();
   thisType& trim() { return leftTrim().rightTrim(); }
     // Suppression with repository modification
   thisType& suppress() { return removeXChars(length()); }
   thisType& remove() { return removeXChars(length()); }
   thisType& remove(const thisType& source, const ExtendedReplaceParameters& params);
   thisType& remove(const TypeChar* source, const ExtendedReplaceParameters& params);
   thisType& remove(const TypeChar& source, const ExtendedReplaceParameters& params);
   thisType& remove(const CharSet& source, const ExtendedReplaceParameters& params);
   thisType& removeSpaces();
   thisType& removeChar() { return removeXChars(0, 1); }
   thisType& removeXChars(int chars) { return removeXChars(0, chars); }
   thisType& removeXChars(int start, int chars);
};

/******************************************/
/* Definition of the notification classes */
/******************************************/

/* Definition of the class TSubString::ReplaceNotification */

template <typename TypeChar, template <typename TChar> class TypePolicy, class SubStringTraits>
class TSubString<TypeChar, TypePolicy, SubStringTraits>::ReplaceNotification : public MoveNotification {
  private:
   typedef TypePolicy<TypeChar> Policy;

  private:
   typedef MoveNotification inherited;
   Position pFirst;
   int uOwnReplace;
   typedef typename Policy::Repository::BaseSubString BaseSubString;
   typedef typename BaseSubString::BaseNotification BaseSubStringNotification;

  protected:
   virtual void _update(BaseSubStringNotification& call) override;

  public:
   typedef typename Policy::Repository Repository;
   ReplaceNotification(const typename Policy::Repository& repository)
      :  inherited(repository), pFirst(), uOwnReplace(-1) {}
   ReplaceNotification(const TSubString<TypeChar, TypePolicy, SubStringTraits>& subString)
      :  inherited(subString, subString.getRepository()), pFirst(), uOwnReplace(-1) {}
   ReplaceNotification(const ReplaceNotification& source) = default;
   ReplaceNotification& operator=(const ReplaceNotification& source) = default;
   DefineCopy(ReplaceNotification)
   DDefineAssign(ReplaceNotification)

   void update(const Repository& repository) { inherited::update(repository, &pFirst); }
   ReplaceNotification& setFirst(const Position& first) { pFirst = first; return *this; }
};

template <typename TypeChar, template <typename TChar> class TypePolicy, class SubStringTraits>
inline void
TSubString<TypeChar, TypePolicy, SubStringTraits>::ReplaceNotification::_update(
      BaseSubStringNotification& call) {
   auto& subString = (TSubString<TypeChar, TypePolicy, SubStringTraits>&) BaseSubString::BaseNotificationCast::castFrom(call);
   int length = subString.length();
   if (subString.isUpperInfinite() && inherited::isInsertion())
      ++length;
   inherited::update(subString, length);
   if (subString.isUpperInfinite() && inherited::isInsertion())
      --length;

   const typename Policy::Repository& repository = subString.getRepository();
   Position start;
   subString.start().retrievePosition(start); // end = start+length
   if (uOwnReplace < 0) {
      inherited::update(pFirst);
      uOwnReplace = inherited::getNewSource().minus(pFirst, repository);
      AssumeCondition(uOwnReplace >= 0)
   };
   int compare = pFirst.minus(start, repository);
   bool testFirst = true;
   if (!subString.isUpperInfinite()) {
      int increase = subString.isUpperOpened() ? 1 : 0;
      if ((compare < length+increase) && (length <= compare + uOwnReplace)) {
         if (subString.isUpperRestricted()) {
            // pFirst < end && start < destination() && start <= end && pFirst <= destination()
            if (compare > 0) { // start <= pFirst < end
               subString.end().plusAssign(compare-length);
               length = compare;
            }
            else { // pFirst < start <= end
               length = 0;
               subString.start().decreaseToPosition(pFirst);
               subString.end().decreaseToPosition(pFirst);
               testFirst = false;
            };
         }
         else { // subString.isUpperClosed() || subString.isUpperOpened()
            int newChars = inherited::isInsertion() ? inherited::getNewChars() : 0;
            subString.end().plusAssign(compare+uOwnReplace+newChars-length);
            length = compare+uOwnReplace+newChars;
         };
      };
   };
   if (testFirst && (compare < 0) && (-compare < uOwnReplace)) {
      int newChars = inherited::isInsertion() ? inherited::getNewChars() : 0;
      length -= compare+uOwnReplace+newChars;
      if (length < 0) {
         subString.end().plusAssign(-length);
         length = 0;
      };
      subString.start().plusAssign(compare+uOwnReplace+newChars);
   };
   subString.uTypeLength = (length << CTypeShift) | (subString.uTypeLength & CTypeMask);
}

/* Definition of the class TSubString::InsertionNotification */

template <typename TypeChar, template <typename TChar> class TypePolicy, class SubStringTraits>
class TSubString<TypeChar, TypePolicy, SubStringTraits>::InsertionNotification : public MoveNotification {
  private:
   typedef TypePolicy<TypeChar> Policy;
   typedef MoveNotification inherited;
   bool fIsConcatenation;
   typedef typename Policy::Repository::BaseSubString BaseSubString;

  protected:
   virtual void _update(typename BaseSubString::BaseNotification& call) override
      {  auto& subString = (TSubString<TypeChar, TypePolicy, SubStringTraits>&) BaseSubString::BaseNotificationCast::castFrom(call);
         int length = subString.length();
         if (subString.isUpperOpened())
            ++length;
         inherited::update(subString, length, fIsConcatenation);
         if (subString.isUpperOpened())
            --length;
         subString.uTypeLength = (length << CTypeShift) | (subString.uTypeLength & CTypeMask);
      }

  public:
   InsertionNotification(const typename Policy::Repository& repository)
      :  inherited(repository), fIsConcatenation(false) {}
   InsertionNotification(const TSubString<TypeChar, TypePolicy, SubStringTraits>& subString)
      : inherited(subString, subString.getRepository()), fIsConcatenation(false) {}
   InsertionNotification(const InsertionNotification& source) = default;
   InsertionNotification& operator=(const InsertionNotification& source) = default;
   DefineCopy(InsertionNotification)
   DDefineAssign(InsertionNotification)

   InsertionNotification& setConcatenation() { fIsConcatenation = true; return *this; }
   InsertionNotification& setInsertion() { fIsConcatenation = false; return *this; }
};

/* Definition of the class TSubString::MoveSupportNotification */

template <typename TypeChar, template <typename TChar> class TypePolicy, class SubStringTraits>
class TSubString<TypeChar, TypePolicy, SubStringTraits>::SuppressionNotification : public MoveNotification {
  private:
   typedef TypePolicy<TypeChar> Policy;
   typedef MoveNotification inherited;

  protected:
   typedef typename Policy::Repository::BaseSubString BaseSubString;
   virtual void _update(typename BaseSubString::BaseNotification& call) override
      {  auto& subString = (TSubString<TypeChar, TypePolicy, SubStringTraits>&) BaseSubString::BaseNotificationCast::castFrom(call);
         int length = subString.length();
         inherited::update(subString, length);
         subString.uTypeLength = (length << CTypeShift) | (subString.uTypeLength & CTypeMask);
      }

  public:
   SuppressionNotification(const typename Policy::Repository& repository) : inherited(repository) {}
   SuppressionNotification(const TSubString<TypeChar, TypePolicy, SubStringTraits>& subString)
      : inherited(subString, subString.getRepository()) {}
   SuppressionNotification(const SuppressionNotification& source) = default;
   DefineCopy(SuppressionNotification)
};

template <typename TypeChar, template <typename TChar> class TypePolicy, class SubStringTraits>
class TSubString<TypeChar, TypePolicy, SubStringTraits>::ExtendedReplaceParameters : public ComparisonMode {
  private:
   typedef ComparisonMode inherited;

  protected:
   DefineExtendedParameters(1, ComparisonMode)

  public:
   ExtendedReplaceParameters() {}
   ExtendedReplaceParameters(const ExtendedReplaceParameters& source) : inherited(source) {}

   ExtendedReplaceParameters& setReplaceAll() { mergeOwnField(1); return *this; }
   ExtendedReplaceParameters& setCaseInsensitive()
      {  return (ExtendedReplaceParameters&) inherited::setCaseInsensitive(); }
   ExtendedReplaceParameters& setCaseSensitive()
      {  return (ExtendedReplaceParameters&) inherited::setCaseSensitive(); }
   bool isReplaceAll() const { return hasOwnField(); }
};

/********************************/
/* Définition du patron TString */
/********************************/

template <typename TypeChar, template <typename TChar> class TypePolicy = GlobalPolicy,
   class SubStringTraits = TSubStringTraits<TypeChar> >
class TString : public TSubString<TypeChar, TypePolicy, SubStringTraits> {
  private:
   typedef TypePolicy<TypeChar> Policy;
   typedef TSubString<TypeChar, TypePolicy, SubStringTraits> inherited;
   typedef TString<TypeChar, TypePolicy, SubStringTraits> thisType;
   typedef typename Policy::Repository Repository;
   
  public:
   typedef typename inherited::Allocation Allocation;
   TString() : inherited(new Repository()) {}
   TString(const Allocation& allocation) : inherited(new Repository(), allocation) {}
   TString(const TypeChar& achar, int number)
      :  inherited(new Repository(), Allocation(number)) { inherited::insert(achar, number); }
   TString(thisType&& source) : inherited(source) {}
   TString(const thisType& source)
      :  inherited(new Repository(), Allocation(source.length()))
      {  inherited::copy(source); }
   TString(const inherited& source)
      :  inherited(new Repository(), Allocation(source.length()))
      {  inherited::copy(source); }
   TString(const inherited& source, int len)
      :  inherited(new Repository(), Allocation(len))
      {  inherited::copy(source, len); }
   TString(const inherited& source, int start, int len)
      :  inherited(new Repository(), Allocation(len))
      {  inherited::copy(source, start, len); }
   TString(const TypeChar* source)
      :  inherited(new Repository(), Allocation((int) SubStringTraits::strlen(source)))
      {  inherited::copy(source); }
   TString(const TypeChar* source, int len)
      :  inherited(new Repository(), Allocation(len))
      {  inherited::copy(source, len); }

   thisType& operator=(const inherited& source)
      {  if (&source != this) {
            inherited::operator=(thisType());
            inherited::copy(source);
         };
         return *this;
      }
   thisType& operator=(const thisType& source) { return operator=((const inherited&) source); }
   Template3DefineCopy(TString, TypeChar, TypePolicy, SubStringTraits)
   DTemplate3DefineAssign(TString, TypeChar, TypePolicy, SubStringTraits)
   void assign(const inherited& source) { operator=(source); }

   bool operator<(const TypeChar* source) const { return compare(source) == CRLess; }
   bool operator<=(const TypeChar* source) const
      {  ComparisonResult result = compare(source);
         return ((result == CRLess) || (result == CREqual));
      }
   bool operator>(const TypeChar* source) const
      {  return compare(source) == CRGreater; }
   bool operator>=(const TypeChar* source) const
      {  ComparisonResult result = compare(source);
         return ((result == CRGreater) || (result == CREqual));
      }
   bool operator==(const TypeChar* source) const
      {  return compare(source) == CREqual; }
   bool operator!=(const TypeChar* source) const
      {  ComparisonResult result = compare(source);
         return ((result == CRGreater) || (result == CRLess));
      }

   ComparisonResult compare(const TypeChar* source) const {  return inherited::compare(source); }
   ComparisonResult compare(const inherited& source, const ComparisonMode& mode) const
      {  return inherited::compare(source, mode); }
   ComparisonResult compare(const TypeChar* source, const ComparisonMode& mode) const
      {  return inherited::compare(source, mode); }
};

typedef TSubString<char> SubString;
typedef TString<char> SString;
typedef TSubString<wchar_t> WideSubString;
typedef TString<wchar_t> WideSString;

#ifndef _MSC_VER
#define DefineListGlobalPolicy List::GlobalPolicy
#else
#define DefineListGlobalPolicy List::LGlobalPolicy
#endif

typedef TSubString<char, List::LightLocalPolicy> EditSubString;
typedef TSubString<wchar_t, List::LightLocalPolicy> WideEditSubString;
typedef TSubString<char, DefineListGlobalPolicy> OutputSubString;
typedef TSubString<wchar_t, DefineListGlobalPolicy> WideOutputSubString;

typedef TString<char, List::LightLocalPolicy> EditSString;
typedef TString<wchar_t, List::LightLocalPolicy> WideEditSString;
typedef TString<char, DefineListGlobalPolicy> OutputSString;
typedef TString<wchar_t, DefineListGlobalPolicy> WideOutputSString;

#undef DefineListGlobalPolicy

class SetOfChars::SubString : public TSubString<char> {
  private:
   typedef TSubString<char> inherited;

  public:
   SubString(const inherited& ssSource) : inherited(ssSource) {}
};

} // end of namespace STG

#ifdef strtod

#define strtod DefineOldStrtop

#endif

#endif // STG_TSubStringH
