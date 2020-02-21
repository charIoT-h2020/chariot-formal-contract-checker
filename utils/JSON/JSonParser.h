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
// Unit      : Parser
// File      : JSonParser.h
// Description :
//   Definition of a parser of JSON document.
//

#ifndef STG_JSON_JSonParserH
#define STG_JSON_JSonParserH

#include "JSON/JSonLexer.h"
#include "Collection/Collection.hpp"
#include "Pointer/DescentParse.h"
#include <cstdint>

namespace STG { namespace JSon {

/***************************************/
/* Definition of the class BasicWriter */
/***************************************/

namespace DJSon {

class Base : public Lexer::Base {};

}

class BasicWriter : public IOObject, protected ExtendedParameters, public Lexer::Base {
  public:
   class State : public ExtendedParameters {
     public:
      typedef WriteResult (State::*WriteMethod)(SubString&, bool);

     private:
      STG::SString sBuffer;
      STG::SubString ssValue;
      int uDocumentLevel;
      WriteMethod wmEvent;
      int uWriteSpaces;

      friend class BasicWriter;
      DefineExtendedParameters(8, ExtendedParameters)
      DefineSubExtendedParameters(Point, 3, INHERITED)
      DefineSubExtendedParameters(ValueType, 2, Point)
      DefineSubExtendedParameters(Follow, 1, ValueType)
      DefineSubExtendedParameters(Comma, 1, Follow)
      DefineSubExtendedParameters(Ender, 1, Comma)

      static WriteMethod getMethod(int code);
      static int getCode(WriteMethod method);
      void setFromCode(int codeParse);
      int getCode() const;

      WriteResult writeStartObject(SubString& out, bool isPretty);
      WriteResult writeKey(SubString& out, bool isPretty);
      WriteResult writeString(SubString& out, bool isPretty);
      WriteResult writeContent(SubString& out, bool isPretty);
      WriteResult writeStartArray(SubString& out, bool isPretty);
      WriteResult writeEndArray(SubString& out, bool isPretty);
      WriteResult writeEndObject(SubString& out, bool isPretty);

      void enterElement(WriteMethod newEvent)
         {  AssumeCondition(!hasPointField())
            wmEvent = newEvent;
         }

      WriteResult writeTextSpaces(SubString& out);
      static WriteResult writeText(SubString& text, SubString& out);

     public:
      State() : sBuffer(), ssValue(sBuffer), uDocumentLevel(0), wmEvent(nullptr), uWriteSpaces(0)
         {  ssValue.setUpperClosed(); }
      State(const State& source)
         :  ExtendedParameters(source),
            sBuffer(source.sBuffer), ssValue(sBuffer),
            uDocumentLevel(source.uDocumentLevel), wmEvent(source.wmEvent),
            uWriteSpaces(source.uWriteSpaces)
         {  if (source.sBuffer.contain(source.ssValue)) {
               ssValue = sBuffer;
               ssValue.setUpperClosed();
               ssValue.advance(source.sBuffer.getSubStringPos(source.ssValue));
               ssValue.setLength(source.ssValue.length());
            };
         }
      State& operator=(const State& source)
         {  ExtendedParameters::operator=(source);
            sBuffer = source.sBuffer;
            ssValue = sBuffer;
            uDocumentLevel = source.uDocumentLevel;
            wmEvent = source.wmEvent;
            uWriteSpaces = source.uWriteSpaces;
            if (source.sBuffer.contain(source.ssValue)) {
               ssValue = sBuffer;
               ssValue.setUpperClosed();
               ssValue.advance(source.sBuffer.getSubStringPos(source.ssValue));
               ssValue.setLength(source.ssValue.length());
            };
            return *this;
         }

      void read(ISBase& in, const FormatParameters& params);
      void write(OSBase& out, const FormatParameters& params) const;

      WriteResult start() { return WRNeedEvent; }
      WriteResult openObject()
         {  enterElement(&State::writeStartObject);
            return WRNeedWrite;
         }
      WriteResult addKey(const SubString& key)
         {  enterElement(&State::writeKey);
            ssValue = key;
            return WRNeedWrite;
         }
      WriteResult addString(const SubString& text)
         {  enterElement(&State::writeString);
            ssValue = text;
            return WRNeedWrite;
         }
      WriteResult addBool(bool value)
         {  enterElement(&State::writeContent);
            if (value)
               sBuffer.copy("true");
            else
               sBuffer.copy("false");
            ssValue = sBuffer;
            ssValue.setUpperClosed();
            return WRNeedWrite;
         }
      WriteResult addInt(int value)
         {  enterElement(&State::writeContent);
            sBuffer.copyInteger(value);
            ssValue = sBuffer;
            ssValue.setUpperClosed();
            return WRNeedWrite;
         }
      WriteResult addUInt(unsigned int value)
         {  enterElement(&State::writeContent);
            sBuffer.copyUInteger(value);
            ssValue = sBuffer;
            ssValue.setUpperClosed();
            return WRNeedWrite;
         }
      WriteResult addLInt(int64_t value)
         {  enterElement(&State::writeContent);
            sBuffer.copyLInteger(value);
            ssValue = sBuffer;
            ssValue.setUpperClosed();
            return WRNeedWrite;
         }
      WriteResult addLUInt(uint64_t value)
         {  enterElement(&State::writeContent);
            sBuffer.copyULInteger(value);
            ssValue = sBuffer;
            ssValue.setUpperClosed();
            return WRNeedWrite;
         }
      WriteResult addFloat(double value)
         {  enterElement(&State::writeContent);
            sBuffer.copyFloat(value);
            ssValue = sBuffer;
            ssValue.setUpperClosed();
            return WRNeedWrite;
         }
      WriteResult addNull()
         {  enterElement(&State::writeContent);
            sBuffer.copy("null");
            ssValue = sBuffer;
            ssValue.setUpperClosed();
            return WRNeedWrite;
         }
      WriteResult openArray()
         {  enterElement(&State::writeStartArray);
            return WRNeedWrite;
         }
      WriteResult closeArray()
         {  enterElement(&State::writeEndArray);
            return WRNeedWrite;
         }
      WriteResult closeObject()
         {  enterElement(&State::writeEndObject);
            return WRNeedWrite;
         }
      WriteResult end()
         {  if (uDocumentLevel != 0)
               throw STG::EWriteError();
            return WRFinished;
         }
      bool isValid() const { return wmEvent != nullptr; }
      const int& getDocumentLevel() const { return uDocumentLevel; }

      WriteResult write(SubString& out, bool isPretty)
         {  return isValid() ? (this->*wmEvent)(out, isPretty) : WRNeedEvent; }
      WriteResult completeWrite(SubString& out, bool isPretty)
         {  WriteResult result = wmEvent ? (this->*wmEvent)(out, isPretty) : WRFinished;
            return (result == WRNeedEvent) ? WRFinished : result;
         }
   };

  private:
   typedef IOObject inherited;
   State sState;

  protected:
   DefineExtendedParameters(1, ExtendedParameters)

   const State& state() const { return sState; }
   State& state() { return sState; }
   bool isPretty() const { return !hasOwnField(); }
   bool isFlatPrint() const { return hasOwnField(); }
   BasicWriter& setPretty() { clearOwnField(); return *this; }
   BasicWriter& setFlatPrint() { mergeOwnField(1); return *this; }

   WriteResult openObject() { return sState.openObject(); }
   WriteResult addKey(const SubString& key) { return sState.addKey(key); }
   WriteResult addString(const SubString& text) { return sState.addString(text); }
   WriteResult addBool(bool value) { return sState.addBool(value); }
   WriteResult addInt(int value) { return sState.addInt(value); }
   WriteResult addUInt(unsigned int value) { return sState.addUInt(value); }
   WriteResult addLInt(int64_t value) { return sState.addLInt(value); }
   WriteResult addLUInt(uint64_t value) { return sState.addLUInt(value); };
   WriteResult addFloat(double value) { return sState.addFloat(value); };
   WriteResult addNull() { return sState.addNull(); }
   WriteResult openArray() { return sState.openArray(); }
   WriteResult closeArray() { return sState.closeArray(); }
   WriteResult closeObject() { return sState.closeObject(); }

   virtual void _read(ISBase& in, const FormatParameters& params) override
      {  sState.read(in, params); }
   virtual void _write(OSBase& out, const FormatParameters& params) const override
      {  sState.write(out, params); }

  public:
   BasicWriter() {}
   BasicWriter(const BasicWriter& source) = default;
   BasicWriter& operator=(const BasicWriter& source)
      {  inherited::operator=(source);
         sState = source.sState;
         setOwnField(source.queryOwnField());
         return *this;
      }
   DefineCopy(BasicWriter)
   DDefineAssign(BasicWriter)

   virtual WriteResult getEvent() { AssumeUncalled return WRFinished; }
   virtual void setWriteBuffer(SubString& out) {}
   void write(OSBase& out);
   WriteResult writeEvent(SubString& out, bool isReentry=false);
   WriteResult writeChunk(SubString& out, WriteResult result = WRNeedEvent);

   void istart()
      {  sState.start(); }
   void iopenObject(STG::SubString& buffer, STG::IOObject::OSBase& out)
      {  openObject();
         while (writeEvent(buffer) == WRNeedPlace)
            {  out.writeall(buffer.asPersistent()); buffer.clear(); }
      }
   void iaddKey(const SubString& key, STG::SubString& buffer, STG::IOObject::OSBase& out)
      {  addKey(key);
         while (writeEvent(buffer) == WRNeedPlace)
            {  out.writeall(buffer.asPersistent()); buffer.clear(); }
      }
   void iaddString(const SubString& text, STG::SubString& buffer, STG::IOObject::OSBase& out)
      {  addString(text);
         while (writeEvent(buffer) == WRNeedPlace)
            {  out.writeall(buffer.asPersistent()); buffer.clear(); }
      }
   void iaddBool(bool value, STG::SubString& buffer, STG::IOObject::OSBase& out)
      {  addBool(value);
         while (writeEvent(buffer) == WRNeedPlace)
            {  out.writeall(buffer.asPersistent()); buffer.clear(); }
      }
   void iaddInt(int value, STG::SubString& buffer, STG::IOObject::OSBase& out)
      {  addInt(value);
         while (writeEvent(buffer) == WRNeedPlace)
            {  out.writeall(buffer.asPersistent()); buffer.clear(); }
      }
   void iaddUInt(unsigned int value, STG::SubString& buffer, STG::IOObject::OSBase& out)
      {  addUInt(value);
         while (writeEvent(buffer) == WRNeedPlace)
            {  out.writeall(buffer.asPersistent()); buffer.clear(); }
      }
   void iaddLInt(int64_t value, STG::SubString& buffer, STG::IOObject::OSBase& out)
      {  addLInt(value);
         while (writeEvent(buffer) == WRNeedPlace)
            {  out.writeall(buffer.asPersistent()); buffer.clear(); }
      }
   void iaddLUInt(uint64_t value, STG::SubString& buffer, STG::IOObject::OSBase& out)
      {  addLUInt(value);
         while (writeEvent(buffer) == WRNeedPlace)
            {  out.writeall(buffer.asPersistent()); buffer.clear(); }
      }
   void iaddFloat(double value, STG::SubString& buffer, STG::IOObject::OSBase& out)
      {  addFloat(value);
         while (writeEvent(buffer) == WRNeedPlace)
            {  out.writeall(buffer.asPersistent()); buffer.clear(); }
      }
   void iaddNull(STG::SubString& buffer, STG::IOObject::OSBase& out)
      {  addNull();
         while (writeEvent(buffer) == WRNeedPlace)
            {  out.writeall(buffer.asPersistent()); buffer.clear(); }
      }
   void iopenArray(STG::SubString& buffer, STG::IOObject::OSBase& out)
      {  openArray();
         while (writeEvent(buffer) == WRNeedPlace)
            {  out.writeall(buffer.asPersistent()); buffer.clear(); }
      }
   void icloseArray(STG::SubString& buffer, STG::IOObject::OSBase& out)
      {  closeArray();
         while (writeEvent(buffer) == WRNeedPlace)
            {  out.writeall(buffer.asPersistent()); buffer.clear(); }
      }
   void icloseObject(STG::SubString& buffer, STG::IOObject::OSBase& out)
      {  closeObject();
         while (writeEvent(buffer) == WRNeedPlace)
            {  out.writeall(buffer.asPersistent()); buffer.clear(); }
      }
   void iend(STG::SubString& buffer, STG::IOObject::OSBase& out)
      {  sState.end(); out.writeall(buffer.asPersistent()); buffer.clear(); }
};

/***************************************/
/* Definition of the class BasicParser */
/***************************************/

class GenericLexer;
class BasicParser : public IOObject, protected ExtendedParameters, public Lexer::Base {
  private:
   class State : protected ExtendedParameters {
     public:
      typedef ReadResult (BasicParser::*ReadPointerMethod)(GenericLexer::Token,
            unsigned line, unsigned column, SubString& additionalContent);

     private:
      class BigInteger : public COL::TVector<unsigned> {
        private:
         typedef COL::TVector<unsigned> inherited;

        public:
         BigInteger() {}
         BigInteger(const BigInteger& source) = default;

         bool cbitArray(int index) const
            {  if ((int) (index*8*sizeof(unsigned)) >= count())
                  return false;
               int cellIndex = index/(8*sizeof(unsigned));
               index %= (8*sizeof(unsigned));
               return (bool) (elementAt(cellIndex) & (1U << index));
            }
         void setBitArray(int index, bool value)
            {  while ((int) (index*8*sizeof(unsigned)) >= count())
                  insertAtEnd(0);
               int cellIndex = index/(8*sizeof(unsigned));
               index %= (8*sizeof(unsigned));
               if (value)
                  referenceAt(cellIndex) |= (1U << index);
               else
                  referenceAt(cellIndex) &= ~(1U << index);
            }
         void setTrueBitArray(int index)
            {  while ((int) (index*8*sizeof(unsigned)) >= count())
                  insertAtEnd(0);
               int cellIndex = index/(8*sizeof(unsigned));
               index %= (8*sizeof(unsigned));
               referenceAt(cellIndex) |= (1U << index);
            }
         void setFalseBitArray(int index)
            {  while ((int) (index*8*sizeof(unsigned)) >= count())
                  insertAtEnd(0);
               int cellIndex = index/(8*sizeof(unsigned));
               index %= (8*sizeof(unsigned));
               referenceAt(cellIndex) &= ~(1U << index);
            }
      };

      ReadPointerMethod ppmParseMethod;
      BigInteger biTypeStack;
      int uDocumentLevel;
      int uPoint;

     protected:
      DefineExtendedParameters(1, ExtendedParameters)
      DefineSubExtendedParameters(TextContinuation, 1, INHERITED)

     public:
      State() : ppmParseMethod(&BasicParser::parseDocument), uDocumentLevel(0), uPoint(0) {}
      State(const State& source) = default;
      State& operator=(const State& sSource) = default;
      void read(ISBase& in, const FormatParameters& params);
      void write(OSBase& out, const FormatParameters& params) const;

      void clear() { clearOwnField(); uDocumentLevel = 0; uPoint = 0; }

      // result == RRNeedChars or RRContinue or RRFinished
      ReadResult parse(BasicParser& parser, const GenericLexer::Token& token, SubString& additionalContent, unsigned line, unsigned column)
         {  return (parser.*ppmParseMethod)(token, line, column, additionalContent); }
      ReadResult parseAgain(BasicParser& parser, const GenericLexer::Token& token,
            SubString& additionalContent, unsigned& line, unsigned& column, bool doesForce);

      const int& point() const { return uPoint; }
      int& point() { return uPoint; }
      bool isInObject() const { return uDocumentLevel > 0 && biTypeStack.cbitArray(uDocumentLevel-1) == false; }
      bool isInArray() const { return uDocumentLevel > 0 && biTypeStack.cbitArray(uDocumentLevel-1) != false; }
      bool wasInObject() const { return uDocumentLevel > 1 && biTypeStack.cbitArray(uDocumentLevel-2) == false; }
      bool wasInArray() const { return uDocumentLevel > 1 && biTypeStack.cbitArray(uDocumentLevel-2) != false; }
      void enterObject() { uDocumentLevel++; }
      void enterArray() { uDocumentLevel++; biTypeStack.setTrueBitArray(uDocumentLevel-1); }
      bool exitObject()
         {  AssumeCondition(uDocumentLevel > 0)
            return (--uDocumentLevel) == 0;
         }
      bool exitArray()
         {  AssumeCondition(uDocumentLevel > 0)
            biTypeStack.setFalseBitArray(uDocumentLevel-1);
            return (--uDocumentLevel) == 0;
         }
      void clearTextContinuation() { clearTextContinuationField(); }
      void setTextContinuation() { mergeTextContinuationField(1); }
      bool hasTextContinuation() const { return hasTextContinuationField(); }
      const int& getDocumentLevel() const { return uDocumentLevel; }
   };

   GenericLexer glLexer;
   State        sState;

  protected:
   virtual void _read(ISBase& in, const FormatParameters& params) override;
   virtual void _write(OSBase& out, const FormatParameters& params) const override;
   virtual void setLocalization(unsigned& line, unsigned& column, bool doesForce) {}

  protected:
   DefineExtendedParameters(1, ExtendedParameters)

   friend class State;
   ReadResult parseDocument(GenericLexer::Token token, unsigned line, unsigned column, SubString& additionalContent);
   ReadResult parseError(GenericLexer::Token token, SubString& additionalContent);

  public:
   BasicParser() {}
   BasicParser(const BasicParser& source) = default;
   BasicParser& operator=(const BasicParser& source) = default;
   DefineCopy(BasicParser)
   DDefineAssign(BasicParser)

   GenericLexer& lexer() { return glLexer; }
   const int& getDocumentLevel() const { return sState.getDocumentLevel(); }
   bool isInObject() const { return sState.isInObject(); }
   bool isInArray() const { return sState.isInArray(); }
   bool wasInObject() const { return sState.wasInObject(); }
   bool wasInArray() const { return sState.wasInArray(); }

   bool isPretty() const { return !hasOwnField(); }
   bool isFlatPrint() const { return hasOwnField(); }
   BasicParser& setPretty() { clearOwnField(); return *this; }
   BasicParser& setFlatPrint() { mergeOwnField(1); return *this; }

   void setPartialToken() { glLexer.setPartialToken(); }
   void setFullToken() { glLexer.setFullToken(); }
   bool isPartialToken() { return glLexer.isPartialToken(); }
   bool isFullToken() { return glLexer.isFullToken(); }

   virtual void startDocument() {}
   virtual ReadResult openObject() { return RRContinue; }
   virtual ReadResult openObject(const GenericLexer::ContentReader& reader, SubString& additionalContent) { return RRContinue; }
   virtual ReadResult addKey(const GenericLexer::ContentReader& reader, SubString& additionalContent) { return RRContinue; }
   virtual ReadResult continueAddKey(const GenericLexer::ContentReader& reader, SubString& additionalContent) { return RRContinue; }
   virtual ReadResult addKey(const SubString& key) { return RRContinue; }
   virtual ReadResult addString(const GenericLexer::ContentReader& reader, SubString& additionalContent) { return RRContinue; }
   virtual ReadResult continueAddString(const GenericLexer::ContentReader& reader, SubString& additionalContent) { return RRContinue; }
   virtual ReadResult addString(const SubString& text) { return RRContinue; }
   virtual ReadResult addBool(bool value) { return RRContinue; }
   virtual ReadResult addNumeric(const GenericLexer::ContentReader& reader, SubString& additionalContent) { return RRContinue; }
   virtual ReadResult continueAddNumeric(const GenericLexer::ContentReader& reader, SubString& additionalContent) { return RRContinue; }
   virtual ReadResult addInt(int value) { return RRContinue; }
   virtual ReadResult addUInt(unsigned int value) { return RRContinue; }
   virtual ReadResult addLInt(int64_t value) { return RRContinue; }
   virtual ReadResult addLUInt(uint64_t value) { return RRContinue; }
   virtual ReadResult addFloat(double value) { return RRContinue; }
   virtual ReadResult addNull() { return RRContinue; }
   virtual ReadResult openArray() { return RRContinue; }
   virtual ReadResult openArray(const GenericLexer::ContentReader& reader, SubString& additionalContent) { return RRContinue; }
   virtual ReadResult closeArray() { return RRContinue; }
   virtual ReadResult closeObject() { return RRContinue; }
   virtual void endDocument() {}

   virtual void warning(const STG::SubString& error, unsigned line, unsigned column) {}
   virtual void error(const STG::SubString& error, unsigned line, unsigned column) {}

   // result == RRNeedChars or RRContinue or RRFinished
   //    RRNeedChars => need to refill subString
   //    RRContinue  => call our method again
   //    RRFinished  => end of parsing
   ReadResult parseToken(SubString& subString, unsigned& line, unsigned& column, bool doesForce=false, bool isReentry=false);
   ReadResult parseChunk(SubString& subString, unsigned& line, unsigned& column, bool doesForce=false);
   void parse(ISBase& in);
   void clear() { glLexer.clear(); sState.clear(); }
};

/****************************************/
/* Definition of the class CommonParser */
/****************************************/

class CommonParser : public BasicParser {
  public:
   class Arguments {
     private:
      enum Event
         {  EUndefined, EOpenObject, EAddKey, ESetString, ESetBool, ESetNumeric, ESetInt,
            ESetUInt, ESetLInt, ESetLUInt, ESetNull, EOpenArray, ECloseArray, ECloseObject
         };

      Event eEvent;
      SubString ssKey;
      SubString ssTextValue;
      union {
         bool valAsBool;
         int valAsInt;
         unsigned valAsUnsigned;
         int64_t valAsLong;
         uint64_t valAsUnsignedLong;
      } uValue;
      GenericLexer::ContentReader lcrReader;
      SubString* pssAdditionalContent;
      bool fContinuedToken;
      bool fCompareEqual = false;
      bool fOldToken;
      friend class CommonParser;

      /* for inlining */
      class ErrorMessage : public COL::List::Node {
        private:
         STG::SubString ssMessage;
         STG::SubString ssFilePos;
         unsigned uLinePos;
         unsigned uColumnPos;

        public:
         ErrorMessage(const STG::SubString& message, const STG::SubString& filePos,
               unsigned linePos, unsigned columnPos)
            :  ssMessage(message), ssFilePos(filePos), uLinePos(linePos), uColumnPos(columnPos) {}
         const STG::SubString& getMessage() const { return ssMessage; }
         const STG::SubString& filepos() const { return ssFilePos; }
         unsigned linepos() const { return uLinePos; }
         unsigned columnpos() const { return uColumnPos; }
      };

      CommonParser* pcpParser;
      int uLocalStackHeight = 0;
      COL::TCopyCollection<COL::TList<ErrorMessage> >* plemErrorMessages = nullptr;
      int uCountErrors = 0;
      bool fDoesStopOnError = false;
      unsigned *puLine = nullptr, *puColumn = nullptr;
      bool fDoesForce = false;

     protected:
      bool hasAdditionalContent() const { return pssAdditionalContent; }
      void setAdditionalContent(STG::SubString& content) { pssAdditionalContent = &content; }

     public:
      Arguments()
         :  eEvent(EUndefined), ssKey(SString()), ssTextValue(SString()),
            pssAdditionalContent(nullptr), fContinuedToken(false), fOldToken(false), pcpParser(nullptr)
         {  uValue.valAsUnsignedLong = 0UL; }
      Arguments(CommonParser& parser)
         :  eEvent(EUndefined), ssKey(SString()), ssTextValue(SString()),
            pssAdditionalContent(nullptr), fContinuedToken(false), fOldToken(false), pcpParser(&parser)
         {  uValue.valAsUnsignedLong = 0UL; }
      Arguments(const Arguments& source) = default;
      Arguments& operator=(const Arguments& source) = default;

      typedef ReadResult ResultAction;
      bool hasEvent() const      { return eEvent != EUndefined; }
      bool isOpen() const        { return eEvent == EOpenObject || eEvent == EOpenArray; }
      bool isOpenObject() const  { return eEvent == EOpenObject; }
      bool isAddKey() const      { return eEvent == EAddKey; }
      bool isSetString() const   { return eEvent == ESetString; }
      bool isSetBool() const     { return eEvent == ESetBool; }
      bool isSetNumeric() const  { return eEvent == ESetNumeric; }
      bool isSetInt() const      { return eEvent == ESetInt; }
      bool isSetUInt() const     { return eEvent == ESetUInt; }
      bool isSetLInt() const     { return eEvent == ESetLInt; }
      bool isSetLUInt() const    { return eEvent == ESetLUInt; }
      bool isSetNull() const     { return eEvent == ESetNull; }
      bool isSetValue() const    { return eEvent >= ESetString && eEvent <= ESetNull; }
      bool isOpenArray() const   { return eEvent == EOpenArray; }
      bool isCloseArray() const  { return eEvent == ECloseArray; }
      bool isCloseObject() const { return eEvent == ECloseObject; }
      bool isClose() const       { return eEvent == ECloseObject || eEvent == ECloseArray; }

      void setLocalization(unsigned& line, unsigned& column, bool doesForce)
         {  puLine = &line; puColumn = &column; fDoesForce = doesForce; }
      unsigned getColumn() const    { return puColumn ? *puColumn : 0; }
      unsigned getLine() const      { return puLine ? *puLine : 0; }
      void setLine(unsigned line)   { if (puLine) *puLine = line; }
      void setColumn(unsigned column){ if (puColumn) *puColumn = column; }
      void setNewToken()            { fOldToken = false; }
      bool isNewToken() const       { return !fOldToken; }
      bool isContinuedToken() const { return fContinuedToken; }
      bool isFullToken() const      { AssumeCondition(pcpParser) return pcpParser->isFullToken(); }
      bool hasFullToken() const     { return !lcrReader.isValid(); }
      bool hasPartialToken() const  { return lcrReader.isValid(); }

      SubString& key()           { return ssKey; }
      SubString& valueAsText()   { return ssTextValue; }
      bool valueAsBool() const   { return uValue.valAsBool; }
      int valueAsInt() const     { return uValue.valAsInt; }
      unsigned valueAsUInt() const { return uValue.valAsUnsigned; }
      int64_t valueAsLInt() const   { return uValue.valAsLong; }
      uint64_t valueAsLUInt() const { return uValue.valAsUnsignedLong; }

      ReadResult isEqualKeyValue(bool& result, const STG::SubString& value)
         {  if (isFullToken())
               result = ssKey == value;
            else {
               STG::SubString oldKey = getKeyCompare();
               if (!isContinuedToken())
                  setKeyCompare(value);
               if (assumeReaderKey() == RRNeedChars) {
                  result = isCompareEqual();
                  return RRNeedChars;
               }
               resetKeyCompare(oldKey);
               result = isCompareEqual();
            }
            return RRContinue;
         }
      ReadResult setArgumentKey()
         {  if (!isFullToken()) {
               if (convertReaderToKey() == RRNeedChars)
                  return RRNeedChars;
            }
            return RRContinue;
         }

      ReadResult isEqualTextValue(bool& result, const STG::SubString& value)
         {  if (isFullToken())
               result = ssTextValue == value;
            else {
               STG::SubString oldTextValue = getValueCompare();
               if (!isContinuedToken())
                  setValueCompare(value);
               if (assumeReaderValue() == RRNeedChars) {
                  result = isCompareEqual();
                  return RRNeedChars;
               }
               resetValueCompare(oldTextValue);
               result = isCompareEqual();
            }
            return RRContinue;
         }
      ReadResult setArgumentTextValue()
         {  if (!isFullToken()) {
               if (convertReaderToValue() == RRNeedChars)
                  return RRNeedChars;
            }
            return RRContinue;
         }
      ReadResult setArgumentToInt() { return convertReaderToInt(); }
      ReadResult setArgumentToLUInt() { return convertReaderToInt(); }

      const GenericLexer::ContentReader& getContentReader() const { return lcrReader; }
      SubString& getAdditionalContent() const { AssumeCondition(pssAdditionalContent) return *pssAdditionalContent; }
      ReadResult convertReaderToInt()
         {  if ((eEvent >= ESetInt && eEvent <= ESetNull) || eEvent == ESetBool) {
               if (eEvent == ESetUInt)
                  setIntValue(valueAsUInt());
               else if (eEvent == ESetLInt)
                  setIntValue(valueAsLInt());
               else if (eEvent == ESetLUInt)
                  setIntValue(valueAsLUInt());
               else if (eEvent == ESetNull)
                  setIntValue(0);
               else if (eEvent == ESetBool)
                  setIntValue(valueAsBool());
               return RRContinue;
            }
            if (eEvent != ESetNumeric) {
               addErrorMessage(STG::SString("int value expected"));
               setIntValue(0);
               return RRContinue;
            }
            GenericLexer::NumberToken res;
            auto result = lcrReader.readNumericContentToken(*pssAdditionalContent, res, *puLine, *puColumn, fDoesForce);
            if (result == RRNeedChars)
               return result;
            fContinuedToken = false;
            if (res.isFloat()) {
               addErrorMessage(STG::SString("int value expected (not float)"));
               setIntValue((int) res.getContent().queryFloat());
            }
            else
               setIntValue(res.getContent().queryInteger());
            return result;
         }
      ReadResult convertReaderToString(bool isValue=true)
         {  if (!lcrReader.isValid())
               return RRContinue;
            STG::SubString res = STG::SString();
            auto result = lcrReader.readContentToken(*pssAdditionalContent, res, *puLine, *puColumn, fDoesForce, true);
            if (!fContinuedToken)
               (isValue ? ssTextValue : ssKey).copy(res);
            else
               (isValue ? ssTextValue : ssKey).cat(res);
            if (result != RRNeedChars)
               fContinuedToken = false;
            return result;
         }
      void setValueCompare(const STG::SubString& value) { ssTextValue = value; fCompareEqual = true; }
      void resetValueCompare(const STG::SubString& value)
         {  if (fCompareEqual)
               fCompareEqual = ssTextValue.length() == 0;
            if (value.length() == 0)
               ssTextValue = value;
            else 
               ssTextValue = SString();
         }
      const STG::SubString& getValueCompare() const { return ssTextValue; }
      void setKeyCompare(const STG::SubString& attribute) { ssKey = attribute; fCompareEqual = true; }
      void resetKeyCompare(const STG::SubString& value)
         {  if (fCompareEqual)
               fCompareEqual = ssKey.length() == 0;
            if (value.length() == 0)
               ssKey = value;
            else 
               ssKey = SString();
         }
      const STG::SubString& getKeyCompare() const { return ssKey; }
      ReadResult assumeReaderValue()
         {  return lcrReader.assumeContentToken(*pssAdditionalContent, ssTextValue, fCompareEqual, *puLine, *puColumn, fDoesForce); }
      ReadResult assumeReaderKey()
         {  return lcrReader.assumeContentToken(*pssAdditionalContent, ssKey, fCompareEqual, *puLine, *puColumn, fDoesForce); }
      bool isCompareEqual() const { return fCompareEqual && ssTextValue.length() == 0; }
      ReadResult convertReaderToValue()
         {  return convertReaderToString(true); }
      ReadResult convertReaderToKey()
         {  return convertReaderToString(false); }
      ReadResult skipReaderValue()
         {  return lcrReader.skipContentToken(*pssAdditionalContent, *puLine, *puColumn, fDoesForce); }
      ReadResult skipReaderKey()
         {  return lcrReader.skipContentToken(*pssAdditionalContent, *puLine, *puColumn, fDoesForce); }

      void setOpenEvent(Event event, const GenericLexer::ContentReader& reader, SubString& additionalContent)
         {  eEvent = event;
            lcrReader = reader;
            pssAdditionalContent = &additionalContent;
            clearValue(); clearKey();
            fContinuedToken = false;
            fOldToken = true;
         }
      void setOpenEventExceptKey(Event event, const GenericLexer::ContentReader& reader, SubString& additionalContent)
         {  eEvent = event;
            lcrReader = reader;
            pssAdditionalContent = &additionalContent;
            clearValue();
            fContinuedToken = false;
            fOldToken = true;
         }
      void continueEvent(Event event, const GenericLexer::ContentReader& reader, SubString& additionalContent)
         {  AssumeCondition(eEvent == event)
            pssAdditionalContent = &additionalContent;
            fContinuedToken = true;
            fOldToken = true;
         }
      void setOpenObject()  { eEvent = EOpenObject; }
      void setOpenObject(const GenericLexer::ContentReader& reader, SubString& additionalContent)
         {  setOpenEventExceptKey(EOpenObject, reader, additionalContent); }
      void setAddKey(const SubString& key)
         {  AssumeCondition(eEvent == EUndefined)
            eEvent = EAddKey; ssKey = key; fOldToken = true;
         }
      void setOpenAddKey(const GenericLexer::ContentReader& reader, SubString& additionalContent)
         {  setOpenEvent(EAddKey, reader, additionalContent); }
      void continueAddKey(const GenericLexer::ContentReader& reader, SubString& additionalContent)
         {  continueEvent(EAddKey, reader, additionalContent); }
      void setStringValue(const SubString& text)
         {  AssumeCondition(eEvent == EUndefined)
            eEvent = ESetString; ssTextValue = text; fOldToken = true;
         }
      void setOpenStringValue(const GenericLexer::ContentReader& reader, SubString& additionalContent)
         {  setOpenEventExceptKey(ESetString, reader, additionalContent); }
      void continueStringValue(const GenericLexer::ContentReader& reader, SubString& additionalContent)
         {  continueEvent(ESetString, reader, additionalContent); }
      void setBoolValue(bool value) { eEvent = ESetBool; uValue.valAsBool = value; fOldToken = true; }
      void setOpenNumericValue(const GenericLexer::ContentReader& reader, SubString& additionalContent)
         {  setOpenEvent(ESetNumeric, reader, additionalContent); }
      void continueNumericValue(const GenericLexer::ContentReader& reader, SubString& additionalContent)
         {  continueEvent(ESetNumeric, reader, additionalContent); }
      void setIntValue(int value) { eEvent = ESetInt; uValue.valAsInt = value; fOldToken = true; }
      void setUIntValue(unsigned value) { eEvent = ESetUInt; uValue.valAsUnsigned = value; fOldToken = true; }
      void setLIntValue(int64_t value) {  eEvent = ESetLInt; uValue.valAsLong = value; fOldToken = true; }
      void setLUIntValue(uint64_t value) {  eEvent = ESetLUInt; uValue.valAsUnsignedLong = value; fOldToken = true; }
      void setNull()        { eEvent = ESetNull; fOldToken = true; }
      void setOpenArray()   { eEvent = EOpenArray; }
      void setOpenArray(const GenericLexer::ContentReader& reader, SubString& additionalContent)
         {  setOpenEventExceptKey(EOpenArray, reader, additionalContent); }
      void setCloseArray()  { eEvent = ECloseArray; fOldToken = true; }
      void setCloseObject() { eEvent = ECloseObject; fOldToken = true; }

      void clearValue()
         {  if (ssTextValue.hasControlOnRepository()) {
               if (ssTextValue.length() > 0)
                  ssTextValue.clear();
            }
            else 
               ssTextValue = SString();
         }
      void clearKey() { if (ssKey.length() > 0) ssKey = SString(); }
      void clear()
         {  eEvent = EUndefined; clearValue(); clearKey(); lcrReader.clear();
            uValue.valAsUnsignedLong = 0UL;
            pssAdditionalContent = nullptr; fContinuedToken = false;
         }
      void clearExceptKey()
         {  eEvent = EUndefined; clearValue(); lcrReader.clear();
            uValue.valAsUnsignedLong = 0UL;
            pssAdditionalContent = nullptr; fContinuedToken = false;
         }

      void shift() { ++uLocalStackHeight; }
      void reduce() { AssumeCondition(uLocalStackHeight >= 0) --uLocalStackHeight; }
      void reduceState(Parser::TStateStack<Arguments>& state)
         {  state.reduce();
            reduce();
         }
      template <class TypeObject, typename ReadPointerMethod, class TypeResult>
      void shiftState(Parser::TStateStack<Arguments>& state,
            TypeObject& object, ReadPointerMethod parseMethod, TypeResult* resTemplate)
         {  state.shift(object, parseMethod, resTemplate);
            shift();
         }
      bool doesStopAfterTooManyErrors() const
         {  return (fDoesStopOnError || !plemErrorMessages || uCountErrors >= 20); }
      bool addErrorMessage(const STG::SubString& message)
         {  ++uCountErrors;
            if (plemErrorMessages)
               plemErrorMessages->insertNewAtEnd(new ErrorMessage(message, STG::SString(), puLine ? *puLine : 0, puColumn ? *puColumn : 0));
            return !doesStopAfterTooManyErrors();
         }
      bool isValidRange() const
         {  return (uLocalStackHeight >= 0 && uLocalStackHeight <= 7); }
      void clearRange() { uLocalStackHeight = 0; }
      bool hasErrors() const
         {  return plemErrorMessages && !plemErrorMessages->isEmpty(); }
      COL::TList<ErrorMessage>& errors() const
         {  AssumeCondition(plemErrorMessages); return *plemErrorMessages; }

      // ReadResult readChars(SubString& buffer) { AssumeCondition(pcpParser) return pcpParser->lexer().readChars(buffer); }
      bool setToNextToken(ReadResult& result)
         {  bool booleanResult = false;
            if (pssAdditionalContent) {
               if (isValidRange()) {
                  bool reentryField = pcpParser->hasReentryField();
                  pcpParser->mergeReentryField(1);
                  while ((result = pcpParser->parseToken(*pssAdditionalContent, *puLine,
                              *puColumn, fDoesForce, true)) == RRContinue) {}
                  booleanResult = (result == RRHasToken);
                  if (!reentryField)
                     pcpParser->clearReentryField();
               }
               else
                  result = RRHasToken;
               if (!booleanResult)
                  clearRange();
            }
            else
               result = RRHasToken;
            return booleanResult;
         }
      bool parseTokens(Parser::TStateStack<Arguments>& state, ReadResult& result)
         {  bool booleanResult = false;
            size_t originalSize = state.getTotalSize();
            int originalPoint = state.hasUpLast() ? state.upLast().point() : -1;
            if (fContinuedToken)
               fContinuedToken = false;
            if (pssAdditionalContent) {
               if (isValidRange()) {
                  result = state.parse(*this);
                  booleanResult = (result == RRHasToken)
                     && state.getTotalSize() < originalSize
                     && !state.isEmpty() && state.point() == originalPoint;
               }
               else
                  result = RRContinue;
               if (!booleanResult)
                  clearRange();
            }
            else
               result = RRContinue;
            return booleanResult;
         }
   };

   typedef Parser::TStateStack<Arguments> State;

  private:
   typedef BasicParser inherited;
   State sState;
   Arguments aArguments;

  protected:
   DefineExtendedParameters(2, inherited)
   DefineSubExtendedParameters(VerificationDisabled, 1, INHERITED)
   DefineSubExtendedParameters(Reentry, 1, VerificationDisabled)
   bool hasVerificationDisabled() const { return hasVerificationDisabledField(); }
   bool isReentry() const { return hasReentryField(); }

   ReadResult parseArgument()
      {  STG::Lexer::Base::ReadResult parseResult;
         do {
            parseResult = sState.parse(aArguments);
         } while (parseResult == RRContinue);
         return (parseResult == RRHasToken) ? RRContinue : parseResult;
      }
   void assumeCleared()
      {  if (aArguments.isSetString() || aArguments.isAddKey())
            parseArgument();
         aArguments.clear();
      }
   void assumeClearedExceptKey()
      {  if (aArguments.isSetString() || aArguments.isAddKey())
            parseArgument();
         aArguments.clearExceptKey();
      }
   virtual void setLocalization(unsigned& line, unsigned& column, bool doesForce) override
      {  aArguments.setLocalization(line, column, doesForce); }

  public:
   class Parse {};
   template <class TypeObject, class TypeResult>
   CommonParser(TypeObject& object, TypeResult* resTemplate, Parse)
      :  aArguments(*this)
      {  sState.shift(object, &TypeObject::readJSon, resTemplate); }

   CommonParser() : aArguments(*this) {}
   CommonParser(CommonParser&& source) = default;
   CommonParser(const CommonParser& source) = default;
   CommonParser& operator=(CommonParser&& source) = default;
   CommonParser& operator=(const CommonParser& source) = default;
   DefineCopy(CommonParser)
   DDefineAssign(CommonParser)

   State& state() { return sState; }
   const State& state() const { return sState; }
   CommonParser& setVerificationDisabled() { mergeVerificationDisabledField(1); return *this; }
   ReadResult parseToken(SubString& subString, unsigned& line, unsigned& column, bool doesForce, bool isReentry=false)
      {  ReadResult result;
         if (!lexer().isFullToken() && lexer().hasContentToken()) {
            ReadResult result = lexer().skipContentToken(subString, line, column, doesForce);
            if (result == RRNeedChars)
               return result;
         }
         aArguments.setNewToken();
         result = inherited::parseToken(subString, line, column, doesForce, isReentry); 
         if (isReentry && result == RRContinue) {
            if (!aArguments.isNewToken())
               result = RRHasToken;
            else if (!lexer().isFullToken() && lexer().hasContentToken())
               result = lexer().skipContentToken(subString, line, column, doesForce);
         }
         return result;
      }

   virtual ReadResult openObject() override
      {  if (isFullToken())
            assumeCleared();
         aArguments.setOpenObject();
         if (isReentry())
            return RRContinue;
         auto result = parseArgument();
         if (isFullToken())
            aArguments.clear();
         return result;
      }
   virtual ReadResult openObject(const GenericLexer::ContentReader& reader, SubString& additionalContent) override
      {  aArguments.setOpenObject(reader, additionalContent);
         if (isReentry())
            return RRContinue;
         return parseArgument();
      }
   virtual ReadResult addKey(const GenericLexer::ContentReader& reader, SubString& additionalContent) override
      {  aArguments.setOpenAddKey(reader, additionalContent);
         if (isReentry())
            return RRContinue;
         return parseArgument();
      }
   virtual ReadResult continueAddKey(const GenericLexer::ContentReader& reader, SubString& additionalContent) override
      {  if (!aArguments.hasEvent())
            return RRContinue;
         aArguments.continueAddKey(reader, additionalContent);
         if (isReentry())
            return RRContinue;
         return parseArgument();
      }
   virtual ReadResult addKey(const SubString& key) override
      {  assumeCleared();
         aArguments.setAddKey(key);
         if (isReentry())
            return RRContinue;
         auto result = parseArgument();
         aArguments.clearExceptKey();
         return result;
      }
   virtual ReadResult addString(const GenericLexer::ContentReader& reader, SubString& additionalContent) override
      {  aArguments.setOpenStringValue(reader, additionalContent);
         if (isReentry())
            return RRContinue;
         return parseArgument();
      }
   virtual ReadResult continueAddString(const GenericLexer::ContentReader& reader, SubString& additionalContent) override
      {  if (!aArguments.hasEvent())
            return RRContinue;
         aArguments.continueStringValue(reader, additionalContent);
         if (isReentry())
            return RRContinue;
         return parseArgument();
      }
   virtual ReadResult addString(const SubString& text) override
      {  assumeClearedExceptKey();
         aArguments.setStringValue(text);
         if (isReentry())
            return RRContinue;
         auto result = parseArgument();
         aArguments.clear();
         return result;
      }
   virtual ReadResult addBool(bool value) override
      {  if (isFullToken())
            assumeClearedExceptKey();
         aArguments.setBoolValue(value);
         if (isReentry())
            return RRContinue;
         auto result = parseArgument();
         if (isFullToken())
            aArguments.clear();
         return result;
      }
   virtual ReadResult addNumeric(const GenericLexer::ContentReader& reader, SubString& additionalContent) override
      {  aArguments.setOpenNumericValue(reader, additionalContent);
         if (isReentry())
            return RRContinue;
         return parseArgument();
      }
   virtual ReadResult continueAddNumeric(const GenericLexer::ContentReader& reader, SubString& additionalContent) override
      {  if (!aArguments.hasEvent())
            return RRContinue;
         aArguments.continueNumericValue(reader, additionalContent);
         if (isReentry())
            return RRContinue;
         return parseArgument();
      }
   virtual ReadResult addInt(int value) override
      {  if (isFullToken())
            assumeClearedExceptKey();
         aArguments.setIntValue(value);
         if (isReentry())
            return RRContinue;
         auto result = parseArgument();
         if (isFullToken())
            aArguments.clear();
         return result;
      }
   virtual ReadResult addUInt(unsigned int value) override
      {  if (isFullToken())
            assumeClearedExceptKey();
         aArguments.setUIntValue(value);
         if (isReentry())
            return RRContinue;
         auto result = parseArgument();
         if (isFullToken())
            aArguments.clear();
         return result;
      }
   virtual ReadResult addLInt(int64_t value) override
      {  if (isFullToken())
            assumeClearedExceptKey();
         aArguments.setLIntValue(value);
         if (isReentry())
            return RRContinue;
         auto result = parseArgument();
         if (isFullToken())
            aArguments.clear();
         return result;
      }
   virtual ReadResult addLUInt(uint64_t value) override
      {  if (isFullToken())
            assumeClearedExceptKey();
         aArguments.setLUIntValue(value);
         if (isReentry())
            return RRContinue;
         auto result = parseArgument();
         if (isFullToken())
            aArguments.clear();
         return result;
      }
   virtual ReadResult addNull() override
      {  if (isFullToken())
            assumeClearedExceptKey();
         aArguments.setNull();
         if (isReentry())
            return RRContinue;
         auto result = parseArgument();
         if (isFullToken())
            aArguments.clear();
         return result;
      }
   virtual ReadResult openArray() override
      {  if (isFullToken())
            assumeClearedExceptKey();
         aArguments.setOpenArray();
         if (isReentry())
            return RRContinue;
         auto result = parseArgument();
         if (isFullToken())
            aArguments.clear();
         return result;
      }
   virtual ReadResult openArray(const GenericLexer::ContentReader& reader, SubString& additionalContent) override
      {  aArguments.setOpenArray(reader, additionalContent);
         if (isReentry())
            return RRContinue;
         return parseArgument();
      }
   virtual ReadResult closeArray() override
      {  if (isFullToken())
            assumeCleared();
         aArguments.setCloseArray();
         if (isReentry())
            return RRContinue;
         auto result = parseArgument();
         if (isFullToken())
            aArguments.clear();
         return result;
      }
   virtual ReadResult closeObject() override
      {  if (isFullToken())
            assumeCleared();
         aArguments.setCloseObject();
         if (isReentry())
            return RRContinue;
         auto result = parseArgument();
         if (isFullToken())
            aArguments.clear();
         return result;
      }
   virtual void startDocument() override {}
   virtual void endDocument() override {}

   class SkipNode : public EnhancedObject {
     private:
      typedef EnhancedObject inherited;

     public:
      SkipNode() = default;
      SkipNode(const SkipNode& source) = default;
      DefineCopy(SkipNode)
      ReadResult skip(State& state, Arguments& arguments);
      ReadResult skipInLoop(State& state, Arguments& arguments);
   };
   
   static bool skipNode(State& state, Arguments& argument);
   static bool skipNodeInLoop(State& state, Arguments& arguments, ReadResult& result);

   struct LessCondition {
      int uLevel;
      int uPoint;
      LessCondition(int level, int point) : uLevel(level), uPoint(point) {}
   };
   ReadResult parseChunkUntil(SubString& subString, const LessCondition& condition,
         unsigned& line, unsigned& column, bool doesForce);
   void parseUntil(ISBase& in, const LessCondition& condition);
};

inline bool
CommonParser::skipNode(State& state, Arguments& argument) {
   bool result = argument.isOpenObject() || argument.isOpenArray();
   if (result) {
      auto& local = state.shiftResult((SkipNode*) nullptr, &SkipNode::skip, SkipNode());
      local.setObject(state.getSResult((SkipNode*) nullptr));
      result = state.parse(argument);
   };
   return result;
}

inline bool
CommonParser::skipNodeInLoop(State& state, Arguments& arguments, ReadResult& result) {
   result = RRContinue;
   if (arguments.isOpen() || arguments.isOpenArray()) {
      auto& local = state.shiftResult((SkipNode*) nullptr, &SkipNode::skipInLoop, SkipNode());
      SkipNode& rule = state.getSResult((SkipNode*) nullptr);
      local.setObject(rule);
      if (!arguments.parseTokens(state, result))
         return false;
   }
   return true;
}

/****************************************/
/* Definition of the class CommonWriter */
/****************************************/

template <class TypeBase>
class TCommonWriter : public TypeBase, public DJSon::Base {
  public:
   typedef CommonParser::FormatParameters FormatParameters;
   class Arguments : public CommonParser::Arguments {
     private:
      TCommonWriter<TypeBase>* pcwWriter = nullptr;

     public:
      typedef WriteResult ResultAction;

      void setAdditionalContent(STG::SubString& out, TCommonWriter<TypeBase>& writer)
         {  CommonParser::Arguments::setAdditionalContent(out);
            pcwWriter = &writer;
         }
      bool writeEvent(WriteResult& result)
         {  bool booleanResult = false;
            if (hasAdditionalContent() && pcwWriter) {
               if (isValidRange()) {
                  bool reentryField = pcwWriter->hasReentryField();
                  pcwWriter->mergeReentryField(1);
                  while ((result = pcwWriter->writeEvent(getAdditionalContent(), true)) == WRNeedEvent) {}
                  booleanResult = (result == WRNeedWrite);
                  if (!reentryField)
                     pcwWriter->clearReentryField();
               }
               else
                  result = WRNeedWrite;
               if (!booleanResult)
                  clearRange();
            }
            else
               result = WRNeedWrite;
            return booleanResult;
         }
      bool writeTokens(Parser::TStateStack<Arguments>& state, WriteResult& result)
         {  bool booleanResult = false;
            size_t originalSize = state.getTotalSize();
            int originalPoint = state.hasUpLast() ? state.upLast().point() : -1;
            if (hasAdditionalContent()) {
               if (isValidRange()) {
                  result = state.parse(*this);
                  booleanResult = (result == WRNeedWrite)
                     && state.getTotalSize() < originalSize
                     && !state.isEmpty() && state.point() == originalPoint;
               }
               else
                  result = WRNeedWrite;
               if (!booleanResult)
                  clearRange();
            }
            else
               result = WRNeedWrite;
            return booleanResult;
         }
      template <class TypeObject, typename WritePointerMethod, class TypeResult>
      void shiftState(Parser::TStateStack<Arguments>& state,
            TypeObject& object, WritePointerMethod writeMethod, TypeResult* resTemplate)
         {  shift();
            state.shift(object, writeMethod, resTemplate);
         }
      void reduceState(Parser::TStateStack<Arguments>& state)
         {  state.reduce();
            reduce();
         }
   };

   typedef Parser::TStateStack<Arguments> State;
   typedef CommonParser::LessCondition LessCondition;

  private:
   typedef TCommonWriter<TypeBase> thisType;
   typedef TypeBase inherited;
   State sState;
   Arguments aArguments;

  protected:
   TemplateDefineExtendedParameters(3, TypeBase)
   DefineSubExtendedParameters(VerificationDisabled, 1, INHERITED)
   DefineSubExtendedParameters(Reentry, 1, VerificationDisabled)
   DefineSubExtendedParameters(PartialWrite, 1, Reentry)

  public:
   class Write {};
   template <class TypeObject, class TypeResult>
   TCommonWriter(const TypeObject& object, TypeResult* resTemplate, Write)
      {  sState.shift(const_cast<TypeObject&>(object), &TypeObject::writeJSon, resTemplate); }

   TCommonWriter() {}
   TCommonWriter(thisType&& source) = default;
   TCommonWriter(const thisType& source) = default;
   thisType& operator=(thisType&& source) = default;
   thisType& operator=(const thisType& source) = default;
   TemplateDefineCopy(TCommonWriter, TypeBase)
   DTemplateDefineAssign(TCommonWriter, TypeBase)

   State& state() { return sState; }
   const State& state() const { return sState; }

   const int& getDocumentLevel() const { return inherited::state().getDocumentLevel(); }
   bool hasVerificationDisabled() const { return hasVerificationDisabledField(); }
   void setPartialWrite() { mergePartialWriteField(1); }

   virtual BasicWriter::WriteResult getEvent() override;

   virtual void setWriteBuffer(SubString& out) override
      {  if (hasPartialWriteField())
            aArguments.setAdditionalContent(out, *this);
      }
   thisType& setVerificationDisabled() { mergeVerificationDisabledField(1); return *this; }
   thisType& setPretty() { return (thisType&) inherited::setPretty(); }
   thisType& setFlatPrint() { return (thisType&) inherited::setFlatPrint(); }
};

class CommonWriter : public TCommonWriter<BasicWriter> {
  private:
   typedef TCommonWriter<BasicWriter> inherited;
   
  public:
   template <class TypeObject, class TypeResult>
   CommonWriter(const TypeObject& object, TypeResult* resTemplate, Write)
      {  state().shift(const_cast<TypeObject&>(object), &TypeObject::writeJSon, resTemplate); }

   CommonWriter() {}
   CommonWriter(const CommonWriter& source) = default;
   
   WriteResult writeChunkUntil(SubString& out, const LessCondition& condition, WriteResult result = WRNeedEvent);
   void writeUntil(OSBase& out, const LessCondition& condition);
};

}} // end of namespace STG::JSon

#endif // STG_JSON_JSonParserH

