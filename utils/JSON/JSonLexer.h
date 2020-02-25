/**************************************************************************/
/*                                                                        */
/*  Copyright (C) 2016-2019                                               */
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
// Library   : JSon
// Unit      : Lexer
// File      : JSonLexer.h
// Description :
//   Definition of a lexer of JSON document.
//

#ifndef STG_JSON_JSonLexerH

#include "TString/SString.h"

namespace STG { namespace JSon {

/****************************************/
/* Definition of the class GenericLexer */
/****************************************/

class GenericLexer : public IOObject, public STG::Lexer::Base {
  public:
   class AbstractToken : public COL::List::Node {
     public:
      enum Type
         {  TUndefined, TOpenBrace, TCloseBrace, TOpenBracket, TCloseBracket,
            TComma, TColon, TString, TTrue, TFalse, TNull, TNumber, TypesNB
         };

     private:
      typedef COL::List::Node inherited;
      Type tType;

     protected:
      Type& type() { return tType; }
      
     public:
      AbstractToken(Type type) : tType(type) {}
      AbstractToken(const AbstractToken& source) = default;
      DefineCopy(AbstractToken)
      DDefineAssign(AbstractToken)

      const Type& getType() const { return tType; }
      static void read(Type& type, IOObject::ISBase& in, const IOObject::FormatParameters& params);
      static void write(Type type, IOObject::OSBase& out, const IOObject::FormatParameters& params);
      static AbstractToken* createToken(Type type);
   };

   class StringToken : public AbstractToken {
     private:
      SString sString;

     public:
      StringToken() : AbstractToken(TString) {}
      StringToken(const StringToken& source) = default;
      DefineCopy(StringToken)
      DDefineAssign(StringToken)

      SString& string() { return sString; }
      const SString& getString() const { return sString; }
   };

   class Token;
   class NumberToken : public AbstractToken {
     private:
      enum SpecialCoding { SCNone, SCBit, SCOctal, SCHexa };
      SString sContent;
      bool fFloat;
      bool fNegative;
      SpecialCoding scCoding;

      void setSubTypeFromContent();
      friend class Token;

     public:
      NumberToken() : AbstractToken(TNumber), fFloat(false), fNegative(false), scCoding(SCNone) {}
      NumberToken(const NumberToken& source) = default;
      DefineCopy(NumberToken)
      DDefineAssign(NumberToken)

      void setFloat() { fFloat = true; }
      void setNegative() { fNegative = true; }
      void setBitCoding() { scCoding = SCBit; }
      void setOctalCoding() { scCoding = SCOctal; }
      void setHexaDecimalCoding() { scCoding = SCHexa; }

      SString& content() { return sContent; }
      const SString& getContent() const { return sContent; }
      bool isInteger() const { return !fFloat; }
      bool isFloat() const { return fFloat; }
      bool isNegative() const { return fNegative; }
      bool hasSpecialCoding() const { return scCoding != SCNone; }
      bool isBitCoding() const { return scCoding == SCBit; }
      bool isOctalCoding() const { return scCoding == SCOctal; }
      bool isHexaCoding() const { return scCoding == SCHexa; }
   };

   class Token {
     private:
      AbstractToken::Type tType;
      PNT::PassPointer<AbstractToken> ppatContent;

     public:
      Token() : tType(AbstractToken::TUndefined) {}
      Token(AbstractToken::Type type) : tType(type) {}
      Token(AbstractToken* token)
         {  AssumeAllocation(token)
            ppatContent.absorbElement(token);
            tType = token->getType();
         }
      Token(const Token& source)
         :  tType(source.tType), ppatContent(source.ppatContent)
         {  if (ppatContent.isValid()) // for setPartialToken mode
               const_cast<Token&>(source).tType = AbstractToken::TUndefined;
         }
      Token& operator=(const Token& source)
         {  tType = source.tType; ppatContent = source.ppatContent;
            if (ppatContent.isValid()) // for setPartialToken mode
               const_cast<Token&>(source).tType = AbstractToken::TUndefined;
            return *this;
         }

      bool isValid() const { return tType != AbstractToken::TUndefined; }
      const AbstractToken::Type& getType() const
         {  AssumeCondition(tType != AbstractToken::TUndefined) return tType; }
      void assumeContent()
         {  AssumeCondition(tType != AbstractToken::TUndefined)
            if (!ppatContent.isValid())
               ppatContent.absorbElement(new AbstractToken(tType));
         }
      bool hasContent() const { return ppatContent.isValid(); }
      AbstractToken* createContent() { return ppatContent.extractElement(); }
      PNT::PassPointer<AbstractToken> extractContent() { return ppatContent; }
      const AbstractToken& getContent() const { return *ppatContent; }

      void read(IOObject::ISBase& in, const IOObject::FormatParameters& params);
      void write(IOObject::OSBase& out, const IOObject::FormatParameters& params) const;
   };

  private:
   class SubStringContainer {
     private:
      const bool fImmediateUse : 1;
      const bool fCompare : 1;
      bool fDifferent : 1;
      SubString* const pssSubString;
      Lexer::TextBuffer* const ptbBuffer;
      NumberToken* pntNumberToken;
      friend class GenericLexer;

     public:
      SubStringContainer()
         :  fImmediateUse(true), fCompare(false), fDifferent(false), pssSubString(nullptr),
            ptbBuffer(nullptr), pntNumberToken(nullptr) {}
      SubStringContainer(SubString& subString)
         :  fImmediateUse(true), fCompare(true), fDifferent(false), pssSubString(&subString),
            ptbBuffer(nullptr), pntNumberToken(nullptr) {}
      SubStringContainer(SubString& subString, bool immediateUse)
         :  fImmediateUse(immediateUse), fCompare(false), fDifferent(false),
            pssSubString(&subString), ptbBuffer(nullptr), pntNumberToken(nullptr) {}
      SubStringContainer(Lexer::TextBuffer& buffer)
         :  fImmediateUse(false), fCompare(false), fDifferent(false), pssSubString(nullptr),
            ptbBuffer(&buffer), pntNumberToken(nullptr) {}
      SubStringContainer(NumberToken& numberToken)
         :  fImmediateUse(false), fCompare(false), fDifferent(false), pssSubString(nullptr),
            ptbBuffer(nullptr), pntNumberToken(&numberToken) {}

      bool isDifferent() const { AssumeCondition(fCompare) return fDifferent; }
   };

   typedef ReadResult (GenericLexer::*ReadPointerMethod)(SubString&, unsigned&, unsigned&, bool, SubStringContainer* container);
   typedef IOObject inherited;
   enum Options { OFullToken, OPartialToken };

   Lexer::TextBuffer tbCurrentToken;
   ReadPointerMethod rpmState, rpmFstState;
   char              chContext;
   Token             tToken;
   Options           oOptions;

  protected:
   virtual void _read(ISBase& in, const FormatParameters& params) override;
   virtual void _write(OSBase& out, const FormatParameters& params) const override;

   ReadResult readMain(SubString& in, unsigned& line, unsigned& column, bool doesForce, SubStringContainer* container);
   ReadResult readNumberToken(SubString& in, unsigned& line, unsigned& column, bool doesForce, SubStringContainer* container);
   ReadResult readAlphaToken(SubString& in, unsigned& line, unsigned& column, bool doesForce, SubStringContainer* container);
   ReadResult readStringToken(SubString& in, unsigned& line, unsigned& column, bool doesForce, SubStringContainer* container);

  public:
   GenericLexer()
   :  rpmState(&GenericLexer::readMain), rpmFstState(nullptr), chContext('\0'), oOptions(OFullToken) {}
   GenericLexer(const GenericLexer& source) = default;
   DefineCopy(GenericLexer)
   DDefineAssign(GenericLexer)

   void setPartialToken() { oOptions = OPartialToken; }
   void setFullToken() { oOptions = OFullToken; }
   bool isPartialToken() const { return oOptions == OPartialToken; }
   bool isFullToken() const { return oOptions == OFullToken; }

   // setFullToken:
   //    result = RRNeedChars => buffer to be refilled
   //    result = RRContinue  => to be called again
   //    result = RRHasToken  => getToken is valid and complete
   //
   // setPartialToken:
   //    before RRHasToken: !hasContentToken()
   //       result = RRNeedChars => buffer to be refilled
   //       result = RRContinue  => to be called again
   //       result = RRHasToken  => getToken is valid (it has a type) but it has no content
   //          method ...ContentToken should be called to access to the beginning of the content
   //    after RRHasToken: hasContentToken()
   //       result = RRNeedChars => buffer to be refilled
   //          method ...ContentToken should be called to access to the middle of the content
   //       result = RRContinue  => getToken is still valid
   //          method ...ContentToken should be called to access to the end of the content
   ReadResult readToken(SubString& in, unsigned& line, unsigned& column,
         bool doesForce=false)
      {  return rpmFstState
            ? (this->*rpmFstState)(in, line, column, doesForce, nullptr)
            : (this->*rpmState)(in, line, column, doesForce, nullptr);
      }
   const Token& getToken() { return tToken; }
   bool hasContentToken() const { return rpmFstState; }
   ReadResult readContentToken(SubString& buffer, SubString& result, unsigned& line,
         unsigned& column, bool doesForce, bool immediateUse=false)
      {  AssumeCondition(rpmFstState)
         SubStringContainer container(result, immediateUse);
         return (this->*rpmFstState)(buffer, line, column, doesForce, &container);
      }
   ReadResult readNumericContentToken(SubString& buffer, NumberToken& result, unsigned& line,
         unsigned& column, bool doesForce)
      {  AssumeCondition(rpmFstState && rpmFstState == &GenericLexer::readNumberToken)
         SubStringContainer container(result);
         return (this->*rpmFstState)(buffer, line, column, doesForce, &container);
      }
   ReadResult readContentToken(SubString& buffer, Lexer::TextBuffer& result, unsigned& line,
         unsigned& column, bool doesForce)
      {  AssumeCondition(rpmFstState)
         SubStringContainer container(result);
         return (this->*rpmFstState)(buffer, line, column, doesForce, &container);
      }
   ReadResult skipContentToken(SubString& buffer, unsigned& line, unsigned& column, bool doesForce)
      {  AssumeCondition(rpmFstState)
         SubStringContainer container;
         return (this->*rpmFstState)(buffer, line, column, doesForce, &container);
      }
   ReadResult assumeContentToken(SubString& buffer, SubString& comparisonContent, bool& isEqual,
         unsigned& line, unsigned& column, bool doesForce)
      {  AssumeCondition(rpmFstState)
         if (!isEqual) {
            SubStringContainer container;
            return (this->*rpmFstState)(buffer, line, column, doesForce, &container);
         };
         SubStringContainer container(comparisonContent);
         ReadResult result = (this->*rpmFstState)(buffer, line, column, doesForce, &container);
         isEqual = !container.isDifferent();
         return result;
      }
   class ContentReader {
     private:
      GenericLexer* plLexer;

     public:
      ContentReader() : plLexer(nullptr) {}
      ContentReader(GenericLexer& lexer) : plLexer(&lexer) {}
      ContentReader(const ContentReader&) = default;
      ContentReader& operator=(const ContentReader&) = default;

      bool isValid() const { return plLexer && plLexer->hasContentToken(); }
      void clear() { plLexer = nullptr; }
      ReadResult readContentToken(SubString& buffer, SubString& result, unsigned& line,
            unsigned& column, bool doesForce, bool immediateUse=false) const
         {  AssumeCondition(plLexer) return plLexer->readContentToken(buffer, result, line,
               column, doesForce, immediateUse);
         }
      ReadResult readNumericContentToken(SubString& buffer, NumberToken& result, unsigned& line,
            unsigned& column, bool doesForce) const
         {  AssumeCondition(plLexer) return plLexer->readNumericContentToken(buffer, result, line,
               column, doesForce);
         }
      ReadResult readContentToken(SubString& buffer, Lexer::TextBuffer& result, unsigned& line,
            unsigned& column, bool doesForce) const
         {  AssumeCondition(plLexer) return plLexer->readContentToken(buffer, result, line, column, doesForce); }
      ReadResult skipContentToken(SubString& buffer, unsigned& line,
            unsigned& column, bool doesForce) const
         {  AssumeCondition(plLexer) return plLexer->skipContentToken(buffer, line, column, doesForce); }
      ReadResult assumeContentToken(SubString& buffer, SubString& comparisonContent, bool& isEqual, unsigned& line,
            unsigned& column, bool doesForce) const
         {  AssumeCondition(plLexer) return plLexer->assumeContentToken(buffer, comparisonContent, isEqual, line, column, doesForce); }
   };
   ContentReader getContentReader() { return ContentReader(*this); }

   void clear()
      {  tbCurrentToken.clear();
         rpmState = &GenericLexer::readMain;
         rpmFstState = nullptr;
         chContext = '\0';
         tToken = Token();
      }
};

}} // end of namespace STG::JSon

#endif // STG_JSON_JSonLexerH
