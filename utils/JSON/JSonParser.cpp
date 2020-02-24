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
// File      : JSonParser.cpp
// Description :
//   Implementation of a parser of JSON document.
//

#include "JSON/JSonParser.h"

namespace STG {}

#include "Collection/Collection.template"
#include "TString/SetOfChars.template"
#include "TString/StringRep.template"
#include "TString/SubString.template"
#include "TString/SString.template"

namespace STG { namespace JSon {

/*******************************************/
/* Implementation of the class BasicWriter */
/*******************************************/

BasicWriter::State::WriteMethod
BasicWriter::State::getMethod(int code) {
   switch(code) {
      case 0: return nullptr;
      case 1: return &State::writeStartObject;
      case 2: return &State::writeKey;
      case 3: return &State::writeString;
      case 4: return &State::writeContent;
      case 5: return &State::writeStartArray;
      case 6: return &State::writeEndArray;
      case 7: return &State::writeEndObject;
      default: AssumeUncalled
   };
   return nullptr;
}

int
BasicWriter::State::getCode(WriteMethod method) {
   if (method == nullptr)
      return 0;
   if (method == &State::writeStartObject)
      return 1;
   if (method == &State::writeKey)
      return 2;
   if (method == &State::writeString)
      return 3;
   if (method == &State::writeContent)
      return 4;
   if (method == &State::writeStartArray)
      return 5;
   if (method == &State::writeEndArray)
      return 6;
   if (method == &State::writeEndObject)
      return 7;
   AssumeUncalled
   return 0;
}

void
BasicWriter::State::setFromCode(int codeParse) {
   wmEvent = getMethod(codeParse & 0x7);
   codeParse >>= 3;
   setPointField(codeParse & 0x7);
   codeParse >>= 3;
   setValueTypeField(codeParse & 0x3);
   codeParse >>= 2;
   setFollowField(codeParse & 1);
}

int
BasicWriter::State::getCode() const {
   return getCode(wmEvent)
      | (queryPointField() << 3) | (queryValueTypeField() << 6)
      | (queryFollowField() << 8);
}

void
BasicWriter::State::read(ISBase& in, const FormatParameters& params) {
   bool isRaw = params.isRaw();
   int codeParse;
   in.read(codeParse, isRaw);
   setFromCode(codeParse);
   
   if (isValid()) {
      if (!isRaw)
         in.assume(' ');
      in.read(uDocumentLevel, isRaw);
      if (!hasFollowField()) {
         if (!isRaw)
            in.assume(' ');
         in.read(uWriteSpaces, isRaw);
      };
   };
   if ((wmEvent == &State::writeKey) || (wmEvent == &State::writeString)
         || (wmEvent == &State::writeContent)) {
      if (!isRaw)
         in.assume(' ');
      in.read(sBuffer.asPersistent(), isRaw);
      ssValue = sBuffer;
   };
}

void
BasicWriter::State::write(OSBase& out, const FormatParameters& params) const {
   bool isRaw = params.isRaw();
   out.write(getCode(), isRaw);

   if (isValid()) {
      if (!isRaw)
         out.put(' ');
      out.write(uDocumentLevel, isRaw);
      if (!hasFollowField()) {
         if (!isRaw)
            out.put(' ');
         out.write(uWriteSpaces, isRaw);
      };
   };
   if ((wmEvent == &State::writeKey) || (wmEvent == &State::writeString)
         || (wmEvent == &State::writeContent)) {
      if (!isRaw)
         out.put(' ');
      out.write(ssValue.asPersistent(), isRaw);
   };
}

BasicWriter::WriteResult
BasicWriter::State::writeTextSpaces(SubString& out) {
   if (!hasFollowField()) {
      while (uWriteSpaces > 0) {
         if (out.getPlace() < 2)
            return WRNeedPlace;
         out.cat("  ");
         --uWriteSpaces;
      };
      // mergeFollowField(1);
   };
   return WRNeedWrite;
}

BasicWriter::WriteResult
BasicWriter::State::writeText(SubString& text, SubString& out) {
   WriteResult result = WRNeedWrite;
   if (out.getPlace() < text.length()) {
      SubString first(text, 0, out.getPlace()-1);
      out.cat(first);
      text.setToEnd(first);
      result = WRNeedPlace;
   }
   else
      out.cat(text);
   return result;
}

BasicWriter::WriteResult
BasicWriter::State::writeStartObject(SubString& out, bool isPretty) {
   AssumeCondition((queryPointField() >= 0) && (queryPointField() <= 2))
   if (queryPointField() == 0) {
      bool hasComma = hasCommaField();
      if (!isPretty) {
         if (hasComma) {
            if (out.getPlace() < 1)
               return WRNeedPlace;
            out.cat(',');
         }
         setPointField(2);
      }
      else {
         if (hasFollowField()) {
            if (out.getPlace() < (hasComma ? 2 : 1))
               return WRNeedPlace;
            clearFollowField();
            if (hasComma)
               out.cat(",\n");
            else
               out.cat('\n');
         }
         else if (hasComma || hasEnderField()) {
            bool hasEnder = hasEnderField();
            if (out.getPlace() < (hasComma && hasEnder ? 1 : 2))
               return WRNeedPlace;
            if (hasComma)
               out.cat(',');
            if (hasEnder)
               out.cat('\n');
            clearEnderField();
         }
         uWriteSpaces = uDocumentLevel;
         setPointField(1);
      };
      clearCommaField();
   };
   if (queryPointField() == 1) {
      if (writeTextSpaces(out) == WRNeedPlace)
         return WRNeedPlace;
      setPointField(2);
   };
   if (queryPointField() == 2) {
      if (isPretty) {
         if (out.getPlace() < 2)
            return WRNeedPlace;
         out.cat("{\n");
      }
      else {
         if (out.getPlace() < 1)
            return WRNeedPlace;
         out.cat('{');
      };
      ++uDocumentLevel;
   };
   clearPointField();
   wmEvent = nullptr;
   return WRNeedEvent;
}

BasicWriter::WriteResult
BasicWriter::State::writeEndObject(SubString& out, bool isPretty) {
   AssumeCondition((queryPointField() <= 2))
   if (queryPointField() == 0) {
      if (!isPretty)
         setPointField(2);
      else {
         if (hasFollowField() || hasEnderField()) {
            if (out.getPlace() < 1)
               return WRNeedPlace;
            clearFollowField();
            clearEnderField();
            out.cat('\n');
         };
         uWriteSpaces = uDocumentLevel-1;
         setPointField(1);
      };
      clearCommaField();
   };
   if (queryPointField() == 1) {
      if (writeTextSpaces(out) == WRNeedPlace)
         return WRNeedPlace;
      setPointField(2);
   };
   if (queryPointField() == 2) {
      if (isPretty) {
         if (out.getPlace() < 2)
            return WRNeedPlace;
         out.cat('}');
         if (uDocumentLevel > 1)
            mergeEnderField(1);
         else
            out.cat('\n');
      }
      else {
         if (out.getPlace() < 1)
            return WRNeedPlace;
         out.cat('}');
      };
      --uDocumentLevel;
   };
   clearPointField();
   mergeCommaField(1);
   wmEvent = nullptr;
   return WRNeedEvent;
}

BasicWriter::WriteResult
BasicWriter::State::writeKey(SubString& out, bool isPretty) {
   AssumeCondition((queryPointField() <= 5))
   if (queryPointField() == 0) {
      bool hasComma = hasCommaField();
      if (isPretty) {
         if (hasFollowField()) {
            if (out.getPlace() < (hasComma ? 2 : 1))
               return WRNeedPlace;
            if (hasComma)
               out.cat(",\n");
            else
               out.cat('\n');
            clearFollowField();
         }
         else if (hasComma || hasEnderField()) {
            bool hasEnder = hasEnderField();
            if (out.getPlace() < (hasComma && hasEnder ? 1 : 2))
               return WRNeedPlace;
            if (hasComma)
               out.cat(',');
            if (hasEnder)
               out.cat('\n');
            clearEnderField();
         }
         uWriteSpaces = uDocumentLevel;
         setPointField(1);
      }
      else {
         if (hasComma) {
            if (out.getPlace() < 1)
               return WRNeedPlace;
            out.cat(',');
         }
         setPointField(2);
      }
      clearCommaField();
   };
   if (queryPointField() == 1) {
      if (writeTextSpaces(out) == WRNeedPlace)
         return WRNeedPlace;
      setPointField(2);
   };
   if (queryPointField() == 2) {
      if (out.getPlace() < 1)
         return WRNeedPlace;
      out.cat('"');
      setPointField(3);
   };
   if (queryPointField() == 3) {
      if (writeText(ssValue, out) == WRNeedPlace)
         return WRNeedPlace;
      setPointField(4);
   };
   if (queryPointField() == 4) {
      if (out.getPlace() < 1)
         return WRNeedPlace;
      out.cat('"');
      setPointField(5);
   };
   if (queryPointField() == 5) {
      if (isPretty) {
         if (out.getPlace() < 2)
            return WRNeedPlace;
         out.cat(": ");
      }
      else {
         if (out.getPlace() < 1)
            return WRNeedPlace;
         out.cat(':');
      };
      mergeFollowField(1);
   };
   clearPointField();
   wmEvent = nullptr;
   return WRNeedEvent;
}

BasicWriter::WriteResult
BasicWriter::State::writeString(SubString& out, bool isPretty) {
   AssumeCondition((queryPointField() <= 2))
   if (queryPointField() == 0) {
      if (out.getPlace() < 1)
         return WRNeedPlace;
      out.cat('"');
      setPointField(1);
   };
   if (queryPointField() == 1) {
      if (writeText(ssValue, out) == WRNeedPlace)
         return WRNeedPlace;
      setPointField(2);
   };
   if (queryPointField() == 2) {
      if (out.getPlace() < 1)
         return WRNeedPlace;
      out.cat('"');
   };
   clearPointField();
   mergeCommaField(1);
   wmEvent = nullptr;
   return WRNeedEvent;
}

BasicWriter::WriteResult
BasicWriter::State::writeContent(SubString& out, bool isPretty) {
   AssumeCondition((queryPointField() == 0))
   if (writeText(ssValue, out) == WRNeedPlace)
      return WRNeedPlace;
   clearFollowField();
   clearPointField();
   mergeCommaField(1);
   wmEvent = nullptr;
   return WRNeedEvent;
}

BasicWriter::WriteResult
BasicWriter::State::writeStartArray(SubString& out, bool isPretty) {
   AssumeCondition((queryPointField() >= 0) && (queryPointField() <= 2))
   if (queryPointField() == 0) {
      bool hasComma = hasCommaField();
      if (!isPretty) {
         if (hasComma) {
            if (out.getPlace() < 1)
               return WRNeedPlace;
            out.cat(',');
         }
         setPointField(2);
      }
      else {
         if (hasFollowField()) {
            if (out.getPlace() < (hasComma ? 2 : 1))
               return WRNeedPlace;
            clearFollowField();
            if (hasComma)
               out.cat(",\n");
            else
               out.cat('\n');
         }
         else if (hasComma || hasEnderField()) {
            bool hasEnder = hasEnderField();
            if (out.getPlace() < (hasComma && hasEnder ? 2 : 1))
               return WRNeedPlace;
            if (hasComma)
               out.cat(',');
            if (hasEnder)
               out.cat('\n');
            clearEnderField();
         }
         uWriteSpaces = uDocumentLevel;
         setPointField(1);
      };
      clearCommaField();
   };
   if (queryPointField() == 1) {
      if (writeTextSpaces(out) == WRNeedPlace)
         return WRNeedPlace;
      setPointField(2);
   };
   if (queryPointField() == 2) {
      if (isPretty) {
         if (out.getPlace() < 2)
            return WRNeedPlace;
         out.cat("[\n");
      }
      else {
         if (out.getPlace() < 1)
            return WRNeedPlace;
         out.cat('[');
      };
      ++uDocumentLevel;
   };
   clearPointField();
   wmEvent = nullptr;
   return WRNeedEvent;
}

BasicWriter::WriteResult
BasicWriter::State::writeEndArray(SubString& out, bool isPretty) {
   AssumeCondition((queryPointField() <= 2))
   if (queryPointField() == 0) {
      if (!isPretty)
         setPointField(2);
      else {
         if (hasFollowField() || hasEnderField()) {
            if (out.getPlace() < 1)
               return WRNeedPlace;
            clearFollowField();
            clearEnderField();
            out.cat('\n');
         }
         uWriteSpaces = uDocumentLevel-1;
         setPointField(1);
      };
   };
   if (queryPointField() == 1) {
      if (writeTextSpaces(out) == WRNeedPlace)
         return WRNeedPlace;
      setPointField(2);
   };
   if (queryPointField() == 2) {
      if (isPretty) {
         if (out.getPlace() < 2)
            return WRNeedPlace;
         out.cat(']');
         if (uDocumentLevel > 1)
            mergeEnderField(1);
         else
            out.cat('\n');
      }
      else {
         if (out.getPlace() < 1)
            return WRNeedPlace;
         out.cat(']');
      };
      --uDocumentLevel;
   };
   clearPointField();
   mergeCommaField(1);
   wmEvent = nullptr;
   return WRNeedEvent;
}

/*******************************************/
/* Implementation of the class BasicWriter */
/*******************************************/

void
BasicWriter::write(OSBase& out) {
   SString buffer(5000);
   WriteResult result = WRNeedEvent;
   while (result != WRFinished) {
      result = writeChunk(buffer, (result == WRNeedPlace) ? WRNeedWrite : result);
      out.writeall(buffer.asPersistent());
      buffer.clear();
   };
}

BasicWriter::WriteResult
BasicWriter::writeEvent(SubString& out, bool isReentry) {
   WriteResult result = WRNeedWrite;
   if (!isReentry && !sState.isValid())
      while ((result = getEvent()) == WRNeedEvent);
   if (result == WRNeedWrite)
      result = sState.write(out, isPretty());
   else if (result == WRFinished)
      result = sState.completeWrite(out, isPretty());
   return result;
}

BasicWriter::WriteResult
BasicWriter::writeChunk(SubString& out, WriteResult result) {
   setWriteBuffer(out);
   while ((result != WRFinished) && (result != WRNeedPlace)) {
      if (result == WRNeedWrite)
         result = sState.write(out, isPretty());
      if (result == WRNeedEvent)
         result = getEvent();
   };
   if (result == WRFinished) {
      result = sState.completeWrite(out, isPretty());
      while ((result != WRFinished) && (result != WRNeedPlace)) {
         if (result == WRNeedEvent)
            result = getEvent();
         if (result == WRNeedWrite)
            result = sState.completeWrite(out, isPretty());
      };
   };
   return result;
}

/**************************************************/
/* Implementation of the class BasicParser::State */
/**************************************************/

void
BasicParser::State::read(ISBase& in, const FormatParameters& params) {
   bool isRaw = params.isRaw();

   // Read the current state of parsing
   in.read(uPoint, isRaw);
   if (!isRaw)
      in.assume(' ');

   // Read the additional parameters
   in.read(uDocumentLevel, isRaw);
   if (uDocumentLevel < 0)
      throw EReadError();
   int size = (uDocumentLevel + 8*sizeof(unsigned)-1) / (8*sizeof(unsigned));
   while (--size >= 0) {
      if (!isRaw)
         in.assume(' ');
      unsigned stackValue = 0;
      in.read(stackValue, isRaw);
      biTypeStack.referenceAt(size) = stackValue;
   };
}

void
BasicParser::State::write(OSBase& out, const FormatParameters& params) const {
   bool isRaw = params.isRaw();
   out.write(uPoint, isRaw);
   if (!isRaw) out.put(' ');
   out.write(uDocumentLevel, isRaw);
   int size = (uDocumentLevel + 8*sizeof(unsigned)-1) / (8*sizeof(unsigned));
   while (--size >= 0) {
      if (!isRaw)
         out.put(' ');
      out.write(biTypeStack[size], isRaw);
   };
}

BasicParser::ReadResult
BasicParser::State::parseAgain(BasicParser& parser, const GenericLexer::Token& token, SubString& additionalContent,
      unsigned& line, unsigned& column, bool doesForce) {
   switch (token.getType()) {
      case GenericLexer::AbstractToken::TString:
         if (hasTextContinuation())
            return parser.continueAddString(parser.glLexer.getContentReader(), additionalContent);
         else
            return parser.continueAddKey(parser.glLexer.getContentReader(), additionalContent);
      case GenericLexer::AbstractToken::TNumber:
         return parser.continueAddNumeric(parser.glLexer.getContentReader(), additionalContent);
      default:
         return RRContinue;
   };
}

/*******************************************/
/* Implementation of the class BasicParser */
/*******************************************/

void
BasicParser::_read(ISBase& in, const FormatParameters& params) {
   glLexer.read(in, params);
   if (!params.isRaw()) in.assume('\n');
   sState.read(in, params);
}

void
BasicParser::_write(OSBase& out, const FormatParameters& params) const {
   glLexer.write(out, params);
   if (!params.isRaw()) out.put('\n');
   sState.write(out, params);
}

/* Implementation of the high level parsing methods */

// result == RRNeedChars or RRContinue or RRFinished
//    RRNeedChars => need to refill subString
//    RRContinue  => call our method again
//    RRFinished  => end of parsing
BasicParser::ReadResult
BasicParser::parseToken(SubString& subString, unsigned& line, unsigned& column, bool doesForce, bool isReentry) {
   GenericLexer::ReadResult result;
   if (!glLexer.isFullToken() && glLexer.hasContentToken()) {
      result = sState.parseAgain(*this, glLexer.getToken(), subString, line, column, doesForce);
      if (result != RRNeedChars && glLexer.hasContentToken())
         result = glLexer.skipContentToken(subString, line, column, doesForce); // empty old unused token
      if (result == RRNeedChars || result == RRFinished)
         return result;
   };
   while ((result = glLexer.readToken(subString, line, column, doesForce)) == RRContinue);
   if (result == RRHasToken) {
      result = sState.parse(*this, glLexer.getToken(), subString, line, column);
      if (!isReentry && glLexer.isPartialToken() && glLexer.hasContentToken()) {
         result = sState.parseAgain(*this, glLexer.getToken(), subString, line, column, doesForce);
         if (result != RRNeedChars && glLexer.hasContentToken())
            result = glLexer.skipContentToken(subString, line, column, doesForce);
      };
   }
   return result;
}

// result == RRNeedChars or RRContinue or RRFinished
//    RRNeedChars => need to refill subString
//    RRFinished  => end of parsing
BasicParser::ReadResult
BasicParser::parseChunk(SubString& subString, unsigned& line, unsigned& column, bool doesForce) {
   GenericLexer::ReadResult result;
   BasicParser::ReadResult parseResult;
   setLocalization(line, column, doesForce);
   if (glLexer.isFullToken()) {
      do { // parseToken is inlined
         while ((result = glLexer.readToken(subString, line, column, doesForce)) == RRContinue);
         parseResult = (result == RRHasToken) ? sState.parse(*this, glLexer.getToken(), subString, line, column) : result;
      } while (parseResult == RRContinue);
   }
   else {
      do { // parseToken is inlined
         if (glLexer.hasContentToken()) {
            result = sState.parseAgain(*this, glLexer.getToken(), subString, line, column, doesForce);
            if (result != RRNeedChars && glLexer.hasContentToken())
               result = glLexer.skipContentToken(subString, line, column, doesForce);
            if (result == RRNeedChars || result == RRFinished)
               return result;
         };
         while ((result = glLexer.readToken(subString, line, column, doesForce)) == RRContinue);
         parseResult = result;
         if (result == RRHasToken) {
            parseResult = sState.parse(*this, glLexer.getToken(), subString, line, column);
            if (parseResult != RRNeedChars && glLexer.hasContentToken()) { // glLexer.isPartialToken()
               parseResult = sState.parseAgain(*this, glLexer.getToken(), subString, line, column, doesForce);
               if (parseResult != RRNeedChars && glLexer.hasContentToken())
                  result = glLexer.skipContentToken(subString, line, column, doesForce);
            };
         }
      } while (parseResult == RRContinue);
   }
   return parseResult;
}

void
BasicParser::parse(ISBase& in) {
   SubString buffer(SString(5000));
   unsigned line = 1;
   unsigned column = 1;

   ReadResult parseResult = RRNeedChars;
   if (!in.good())
      return;
   startDocument();
   while (in.good() && (parseResult != RRFinished)) {
      if (buffer.length() > 0) {
         int length = buffer.length();
         STG::SubString text(buffer);
         buffer.seek(0,0);
         buffer.copy(text).setToEnd();
         in.readsome(buffer.asPersistent(), 5000-length);
         buffer.seek(0,0);
      }
      else {
         buffer.seek(0,0);
         in.readsome(buffer.asPersistent(), 5000);
      };
      // in.read(buffer, buffer.getPlace());
      parseResult = parseChunk(buffer, line, column, !in.good());
   };
   if ((parseResult != RRFinished) && (sState.getDocumentLevel() > 0))
      error(SString("Unexpected end of JSON stream"), line, column);
   else
      endDocument();
}

/* Implementation of the low level parsing methods, event methods triggered by tokens */

BasicParser::ReadResult
BasicParser::parseError(GenericLexer::Token token, SubString& additionalContent) {
   AssumeCondition(sState.getDocumentLevel() >= 1)
   if (token.getType() == GenericLexer::AbstractToken::TCloseBrace)
      sState.exitObject();
   else if (token.getType() == GenericLexer::AbstractToken::TCloseBracket)
      sState.exitArray();
   return RRContinue;
}

BasicParser::ReadResult
BasicParser::parseDocument(GenericLexer::Token token, unsigned line, unsigned column, SubString& additionalContent) {
   enum Delimiters { DBegin, DAfterKey, DAfterKeyColon };

   ReadResult result = RRNeedChars;
   int point = sState.point();
   switch (token.getType()) {
      case GenericLexer::AbstractToken::TOpenBrace:
         if (point == DBegin) {
            if (sState.isInObject())
               error(SString("Unexpected JSON '{' in object: key expected"), line, column);
         }
         else if (point == DAfterKey)
            error(SString("Expect ':' after key and not '{'"), line, column);
         sState.enterObject();
         sState.point() = DBegin;
         if (isFullToken())
            result = openObject();
         else
            result = openObject(glLexer.getContentReader(), additionalContent);
         return (result == RRNeedChars || result == RRFinished) ? result : RRContinue;
      case GenericLexer::AbstractToken::TCloseBrace:
         {  if (point != DBegin)
               error(SString("Unexpected JSON '}' after key"), line, column);
            bool isFinished = sState.exitObject();
            sState.point() = DBegin;
            result = closeObject();
            return (!isFinished && result != RRFinished) ? RRContinue : RRFinished;
         };
      case GenericLexer::AbstractToken::TOpenBracket:
         if (point == DBegin) {
            if (sState.isInObject())
               error(SString("Unexpected JSON '[' in object: key expected"), line, column);
         }
         else if (point == DAfterKey)
            error(SString("Expect ':' after key and not '['"), line, column);
         sState.enterArray();
         sState.point() = DBegin;
         if (isFullToken())
            result = openArray();
         else
            result = openArray(glLexer.getContentReader(), additionalContent);
         return (result == RRNeedChars || result == RRFinished) ? result : RRContinue;
      case GenericLexer::AbstractToken::TCloseBracket:
         {  if (point != DBegin)
               error(SString("Unexpected JSON ']' after key"), line, column);
            bool isFinished = sState.exitArray();
            sState.point() = DBegin;
            result = closeArray();
            return (!isFinished && result != RRFinished) ? RRContinue : RRFinished;
         };
      case GenericLexer::AbstractToken::TComma:
         if (point >= DAfterKey)
            error(SString("Unexpected JSON ',' in object"), line, column);
         return RRContinue;
      case GenericLexer::AbstractToken::TColon:
         if (point != DAfterKey)
            error(SString("Unexpected JSON ':'"), line, column);
         else
            sState.point() = DAfterKeyColon;
         return RRContinue;
      case GenericLexer::AbstractToken::TString:
         if (point == DBegin) {
            if (sState.isInObject()) {
               sState.point() = DAfterKey;
               sState.clearTextContinuation();
               if (!token.hasContent())
                  result = addKey(glLexer.getContentReader(), additionalContent);
               else
                  result = addKey(((const GenericLexer::StringToken&) *token.extractContent()).getString());
            }
            else {
               sState.setTextContinuation();
               if (!token.hasContent())
                  result = addString(glLexer.getContentReader(), additionalContent);
               else
                  result = addString(((const GenericLexer::StringToken&) *token.extractContent()).getString());
            }
         }
         else if (point == DAfterKey)
            error(SString("Unexpected JSON string after key"), line, column);
         else if (point == DAfterKeyColon) {
            sState.point() = DBegin;
            sState.setTextContinuation();
            if (!token.hasContent())
               result = addString(glLexer.getContentReader(), additionalContent);
            else
               result = addString(((const GenericLexer::StringToken&) *token.extractContent()).getString());
         };
         return (result == RRNeedChars || result == RRFinished) ? result : RRContinue;
      case GenericLexer::AbstractToken::TTrue:
         if ((sState.isInObject() && (point == DAfterKeyColon))
               || (sState.isInArray() && (point == DBegin))) {
            sState.point() = DBegin;
            result = addBool(true);
         }
         else
            error(SString("Unexpected JSON boolean after key"), line, column);
         return result == RRFinished ? result : RRContinue;
      case GenericLexer::AbstractToken::TFalse:
         if ((sState.isInObject() && (point == DAfterKeyColon))
               || (sState.isInArray() && (point == DBegin))) {
            sState.point() = DBegin;
            result = addBool(false);
         }
         else
            error(SString("Unexpected JSON boolean after key"), line, column);
         return result == RRFinished ? result : RRContinue;
      case GenericLexer::AbstractToken::TNull:
         if ((sState.isInObject() && (point == DAfterKeyColon))
               || (sState.isInArray() && (point == DBegin))) {
            sState.point() = DBegin;
            result = addNull();
         }
         else
            error(SString("Unexpected JSON boolean after key"), line, column);
         return result == RRFinished ? result : RRContinue;
      case GenericLexer::AbstractToken::TNumber:
         if ((sState.isInObject() && (point == DAfterKeyColon))
               || (sState.isInArray() && (point == DBegin))) {
            sState.point() = DBegin;
            if (token.hasContent()) {
               auto atoken = token.extractContent();
               const auto& numberToken = (const GenericLexer::NumberToken&) *atoken;
               if (!numberToken.isFloat()) // isInteger
                  result = addInt(numberToken.getContent().queryInteger());
               else
                  result = addFloat(numberToken.getContent().queryFloat());
            }
            else
               result = addNumeric(glLexer.getContentReader(), additionalContent);
         }
         else
            error(SString("Unexpected JSON boolean after key"), line, column);
         return (result == RRNeedChars || result == RRFinished) ? result : RRContinue;
      default:
         error(SString("Unexpected token in JSON document"), line, column);
   };
   // if the state has decreased, it is likely to be equal to 0
   return ((sState.point() == DBegin && sState.getDocumentLevel() == 0) || result == RRFinished)
      ? RRFinished : RRContinue;
}

/********************************************/
/* Implementation of the class CommonParser */
/********************************************/

void
CommonParser::Arguments::convertIntValue(Event newEvent) {
   if (eEvent == newEvent)
      return;
   switch (eEvent) {
      case ESetBool:
         if (newEvent == ESetInt)
            uValue.valAsInt = uValue.valAsBool;
         else if (newEvent == ESetUInt)
            uValue.valAsUnsigned = uValue.valAsBool;
         else if (newEvent == ESetLInt)
            uValue.valAsLong = uValue.valAsBool;
         else if (newEvent == ESetLUInt)
            uValue.valAsUnsignedLong = uValue.valAsBool;
         break;
      case ESetInt:
         if (newEvent == ESetBool)
            uValue.valAsBool = uValue.valAsInt;
         else if (newEvent == ESetUInt)
            uValue.valAsUnsigned = uValue.valAsInt;
         else if (newEvent == ESetLInt)
            uValue.valAsLong = uValue.valAsInt;
         else if (newEvent == ESetLUInt)
            uValue.valAsUnsignedLong = uValue.valAsInt;
         break;
      case ESetUInt:
         if (newEvent == ESetBool)
            uValue.valAsBool = uValue.valAsUnsigned;
         else if (newEvent == ESetInt)
            uValue.valAsInt = uValue.valAsUnsigned;
         else if (newEvent == ESetLInt)
            uValue.valAsLong = uValue.valAsUnsigned;
         else if (newEvent == ESetLUInt)
            uValue.valAsUnsignedLong = uValue.valAsUnsigned;
         break;
      case ESetLInt:
         if (newEvent == ESetBool)
            uValue.valAsBool = uValue.valAsLong;
         else if (newEvent == ESetInt)
            uValue.valAsInt = uValue.valAsLong;
         else if (newEvent == ESetUInt)
            uValue.valAsUnsigned = uValue.valAsLong;
         else if (newEvent == ESetLUInt)
            uValue.valAsUnsignedLong = uValue.valAsLong;
         break;
      case ESetLUInt:
         if (newEvent == ESetBool)
            uValue.valAsBool = uValue.valAsUnsignedLong;
         if (newEvent == ESetInt)
            uValue.valAsInt = uValue.valAsUnsignedLong;
         else if (newEvent == ESetUInt)
            uValue.valAsUnsigned = uValue.valAsUnsignedLong;
         else if (newEvent == ESetLInt)
            uValue.valAsLong = uValue.valAsUnsignedLong;
         break;
      default:
         break;

   };
   eEvent = newEvent;
   fOldToken = true;
}

STG::Lexer::Base::ReadResult
CommonParser::Arguments::convertReaderToInt(Event newEvent) {
   AssumeCondition((newEvent >= ESetInt && newEvent <= ESetNull) || newEvent == ESetBool);
   if (newEvent == eEvent)
      return RRContinue;
   if ((eEvent >= ESetInt && eEvent <= ESetNull) || eEvent == ESetBool) {
      convertIntValue(newEvent);
      return RRContinue;
   }
   if (eEvent != ESetNumeric) {
      addErrorMessage(STG::SString("int value expected"));
      setIntValue(0);
      convertIntValue(newEvent);
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
      convertIntValue(newEvent);
   }
   else {
      if (res.getContent().compareI("false") == CREqual)
         setBoolValue(false);
      else if (res.getContent().compareI("true") == CREqual)
         setBoolValue(true);
      else {
         if (res.getContent().compareI("0x", 2) == CREqual) {
            SubString val(res.getContent());
            val.advance(2);
            if (newEvent == ESetLUInt)
               setLUIntValue(res.getContent().queryHexaULInteger());
            else if (newEvent == ESetLInt)
               setLIntValue(res.getContent().queryHexaLInteger());
            else if (newEvent == ESetLInt)
               setUIntValue(res.getContent().queryHexaLInteger());
            else
               setIntValue(res.getContent().queryHexaInteger());
         }
         else {
            if (newEvent == ESetLUInt)
               setLUIntValue(res.getContent().queryULInteger());
            else if (newEvent == ESetLInt)
               setLIntValue(res.getContent().queryLInteger());
            else if (newEvent == ESetLInt)
               setUIntValue(res.getContent().queryLInteger());
            else
               setIntValue(res.getContent().queryInteger());
         };
      };
      convertIntValue(newEvent);
   }
   return result;
}

CommonParser::ReadResult
CommonParser::parseChunkUntil(SubString& subString, const LessCondition& condition,
      unsigned& line, unsigned& column, bool doesForce) {
   BasicParser::ReadResult parseResult;
   aArguments.setLocalization(line, column, doesForce);
   do {
      if (!sState.isLessThan(condition.uLevel, condition.uPoint))
         return RRFinished;
      parseResult = parseToken(subString, line, column, doesForce);
   } while (parseResult == RRContinue);
   return parseResult;
}

void
CommonParser::parseUntil(ISBase& in, const LessCondition& condition) {
   SubString buffer(SString(5000));
   unsigned line = 1;
   unsigned column = 1;

   ReadResult parseResult = RRNeedChars;
   if (!in.good())
      return;
   startDocument();
   while (in.good() && (parseResult != RRFinished)) {
      if (buffer.length() > 0) {
         int length = buffer.length();
         STG::SubString text(buffer);
         buffer.seek(0,0);
         buffer.copy(text).setToEnd();
         in.readsome(buffer.asPersistent(), 5000-length);
         buffer.seek(0,0);
      }
      else {
         buffer.seek(0,0);
         in.readsome(buffer.asPersistent(), 5000);
      };
      // in.read(buffer, buffer.getPlace());
      parseResult = parseChunkUntil(buffer, condition, line, column, !in.good());
   };
   if (parseResult != RRFinished)
      error(SString("Unexpected end of JSON stream"), line, column);
   else
      endDocument();
}

CommonParser::ReadResult
CommonParser::SkipNode::skip(State& state, Arguments& argument) {
   if (argument.isOpenObject() || argument.isOpenArray())
      ++state.point();
   else if (argument.isCloseObject() || argument.isCloseArray()) {
      --state.point();
      if (state.point() == 0)
         argument.reduceState(state); // potential destruction of this
   };
   return RRHasToken;
}

CommonParser::ReadResult
CommonParser::SkipNode::skipInLoop(State& state, Arguments& argument) {
   CommonParser::ReadResult result;
   while (true) {
      if ((argument.isOpenObject() || argument.isOpenArray()) && !argument.isContinuedToken())
         ++state.point();
      else if (argument.isCloseObject() || argument.isCloseArray()) {
         --state.point();
         if (state.point() == 0) {
            argument.reduceState(state); // potential destruction of this
            return RRHasToken;
         }
      }
      AssumeCondition(argument.puLine)
      if (!argument.setToNextToken(result)) return result;
   };
}

template <class TypeBase>
BasicWriter::WriteResult
TCommonWriter<TypeBase>::getEvent() {
   WriteResult result = WRNeedEvent;
   if (sState.isEmpty())
      result = WRFinished;
   else if (sState.parse(aArguments)) {
      if (aArguments.isOpenObject())
         result = inherited::openObject();
      else if (aArguments.isAddKey())
         result = inherited::addKey(aArguments.key());
      else if (aArguments.isSetString())
         result = inherited::addString(aArguments.valueAsText());
      else if (aArguments.isSetBool())
         result = inherited::addBool(aArguments.valueAsBool());
      else if (aArguments.isSetInt())
         result = inherited::addInt(aArguments.valueAsInt());
      else if (aArguments.isSetUInt())
         result = inherited::addUInt(aArguments.valueAsUInt());
      else if (aArguments.isSetLInt())
         result = inherited::addLInt(aArguments.valueAsLInt());
      else if (aArguments.isSetLUInt())
         result = inherited::addLUInt(aArguments.valueAsLUInt());
      else if (aArguments.isSetNull())
         result = inherited::addNull();
      else if (aArguments.isOpenArray())
         result = inherited::openArray();
      else if (aArguments.isCloseArray())
         result = inherited::closeArray();
      else if (aArguments.isCloseObject())
         result = inherited::closeObject();
      // else
      //   throw EWriteError();
      aArguments.clear();
   }
   else if (!hasVerificationDisabled())
      throw STG::EWriteError();
   else
      result = WRFinished;
   return result;
}

CommonWriter::WriteResult
CommonWriter::writeChunkUntil(SubString& out, const LessCondition& condition, WriteResult result) {
   while ((result != WRFinished) && (result != WRNeedPlace)) {
      if (result == WRNeedWrite)
         result = BasicWriter::state().write(out, isPretty());
      if (result == WRNeedEvent) {
         if (!state().isLessThan(condition.uLevel, condition.uPoint))
            return WRFinished;
         result = getEvent();
      };
   };
   if (result == WRFinished) {
      result = BasicWriter::state().completeWrite(out, isPretty());
      while ((result != WRFinished) && (result != WRNeedPlace)) {
         if (result == WRNeedEvent) {
            if (!state().isLessThan(condition.uLevel, condition.uPoint))
               return WRFinished;
            result = getEvent();
         };
         if (result == WRNeedWrite)
            result = BasicWriter::state().completeWrite(out, isPretty());
      };
   };
   return result;
}

void
CommonWriter::writeUntil(OSBase& out, const LessCondition& condition) {
   SString buffer(5000);
   WriteResult result = WRNeedEvent;
   while (result != WRFinished) {
      result = writeChunkUntil(buffer, condition, (result == WRNeedPlace) ? WRNeedWrite : result);
      out.writeall(buffer.asPersistent());
      buffer.clear();
   };
}

}} // end of namespace STG::JSon

template class STG::JSon::TCommonWriter<STG::JSon::BasicWriter>;
template class COL::TVector<unsigned int, COL::DVector::TElementTraits<unsigned int>, COL::DVector::ReallocTraits>;

