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
//   Implementation of a lexer of JSON document.
//

#include "JSON/JSonLexer.h"
// #include "JSON/JSonParser.h"

namespace STG { namespace JSon {

/***************************************************/
/* Implementation of the class GenericLexer::Token */
/***************************************************/

void
GenericLexer::AbstractToken::read(Type& type, IOObject::ISBase& in, const IOObject::FormatParameters& params) {
   if (params.isRaw()) {
      char typeValue;
      in.read(typeValue, true);
      if (typeValue < 0 || typeValue >= TypesNB)
         throw EReadError();
      type = (Type) typeValue;
   }
   else {
      int typeValue;
      in.read(typeValue, false);
      if (typeValue < 0 || typeValue >= TypesNB)
         throw EReadError();
      type = (Type) typeValue;
   };
}

void
GenericLexer::AbstractToken::write(Type type, IOObject::OSBase& out, const IOObject::FormatParameters& params) {
   if (params.isRaw())
      out.write((int) type, true);
   else
      out.write((char) type, false);
}

GenericLexer::AbstractToken*
GenericLexer::AbstractToken::createToken(Type type) {
   switch (type) {
      case TUndefined: case TOpenBrace: case TCloseBrace: case TOpenBracket: case TCloseBracket:
      case TComma: case TColon: case TTrue: case TFalse: case TNull:
         return nullptr;
      case TNumber: return new NumberToken;
      case TString: return new StringToken;
      default: break;
   };
   return nullptr;
}

void
GenericLexer::NumberToken::setSubTypeFromContent() {
   SubString scan(sContent);
   scan.setUpperClosed();
   if (scan[0] == '-' || scan[0] == '+') {
      fNegative = scan[0] == '-';
      scan.advance();
   };
   static SetOfChars floatChars = SetOfChars().add('.').add('e').add('E');
   fFloat = scan.scanPos(floatChars) >= 0;
}

void
GenericLexer::Token::read(IOObject::ISBase& in, const IOObject::FormatParameters& params) {
   AbstractToken::read(tType, in, params);
   if (tType != AbstractToken::TNumber && tType != AbstractToken::TString)
      return;
   bool isRaw = params.isRaw();
   if (!isRaw) in.assume(' ');
   if (((char) in.get()) - '0') {
      if (!isRaw) in.assume(' ');
      ppatContent.absorbElement(AbstractToken::createToken(tType));
      if (tType == AbstractToken::TNumber) {
         in.read(((NumberToken&) *ppatContent).content().asPersistent(), isRaw);
         ((NumberToken&) *ppatContent).setSubTypeFromContent();
      }
      else
         in.read(((StringToken&) *ppatContent).string().asPersistent(), isRaw);
   };
}

void
GenericLexer::Token::write(IOObject::OSBase& out, const IOObject::FormatParameters& params) const {
   AbstractToken::write(tType, out, params);
   if (tType != AbstractToken::TNumber && tType != AbstractToken::TString)
      return;

   bool isRaw = params.isRaw();
   if (!isRaw) out.put(' ');
   out.put(ppatContent.isValid() ? '1' : '0');
   if (ppatContent.isValid()) {
      if (!isRaw) out.put(' ');
      if (tType == AbstractToken::TNumber)
         out.write(((NumberToken&) *ppatContent).getContent().asPersistent(), isRaw);
      else
         out.write(((StringToken&) *ppatContent).getString().asPersistent(), isRaw);
   };
}

/********************************************/
/* Implementation of the class GenericLexer */
/********************************************/

void
GenericLexer::_read(ISBase& in, const FormatParameters& params) {
   bool isRaw = params.isRaw();
   tToken.read(in, params);
   if (!isRaw)
      in.assume('\n');
   tbCurrentToken.read(in, params);
   if (!isRaw)
      in.assume('\n');
   int state = 0;
   in.read(state, isRaw);
   if ((state < 0) || (state > 3))
      throw EReadError();
   rpmState = (state == 0) ? nullptr : &GenericLexer::readMain;
   if (state > 1)
      rpmFstState = (state == 2) ? &GenericLexer::readNumberToken : &GenericLexer::readStringToken;
   if (!isRaw)
      in.assume(' ');
   in.read(chContext, isRaw);
}

void
GenericLexer::_write(OSBase& out, const FormatParameters& params) const {
   bool isRaw = params.isRaw();
   tToken.write(out, params);
   if (!isRaw)
      out.put('\n');
   tbCurrentToken.write(out, params);
   if (!isRaw)
      out.put('\n');
   int state = 0;
   if (rpmState == &GenericLexer::readMain) {
      state = 1;
      if (rpmFstState) {
         if (rpmFstState == &GenericLexer::readNumberToken)
            state = 2;
         else
            state = 3;
      };
   };
   out.write(state, isRaw);
   if (!isRaw)
      out.put(' ');
   out.write(chContext, isRaw);
}

namespace DGenericLexer {

enum NumberContext
   {  NCNone, NCHexa=0x1, NCBinary, NCOctal, NCPositive=0x4, NCNegative,
      NCInteger=0x8, NCFractional=0x16, NCSignExponent, NCExponent
   };

inline bool isCharDigit(char ch, NumberContext context) {
   return (context == NCNone)
      ? SubString::Traits::isdigit(ch) : ((context == NCBinary)
      ? (ch == '0' || ch == '1') : ((context == NCOctal)
      ? SubString::Traits::isoctaldigit(ch) : ((context == NCHexa)
      ? SubString::Traits::ishexadigit(ch) : false)));
}

}

GenericLexer::ReadResult
GenericLexer::readNumberToken(SubString& in, unsigned& line, unsigned& column, bool doesForce, SubStringContainer* container) {
   using namespace DGenericLexer;
   static const int NCFormat = NCOctal;
   static const int UPrefix = 0x7;
   STG::TChunk<char> chunk = in.getChunk();
   if (chunk.length == 0)
      return doesForce ? RRFinished : RRNeedChars;

   int pos = 0;
   if (chContext == NCPositive || chContext == NCNegative) {
      while (SubString::Traits::isspace(chunk.string[pos])
            && chunk.string[pos] != '\n')
         ++pos;
      if (chunk.string[pos] == '0') {
         char chs = chunk.string[pos+1];
         if ((chs == 'x') || (chs == 'X')) {
            chContext |= NCHexa;
            pos += 2;
         }
         else if ((chs == 'b')|| (chs == 'B')) {
            chContext |= NCBinary;
            pos += 2;
         }
         else if (chs >= '0' && chs <= '7') {
            chContext |= NCOctal;
            pos += 1;
         }
         else if (chs == '\0') {
            STG::SubString prenumber(in);
            prenumber.setUpperClosed().setLength(pos);
            in.advance(pos);
            column += pos;
            if (doesForce) {
               in.advance();
               column += 1;
               goto LCreateToken;
            };
            if (!container || !container->fImmediateUse)
               ((!container || !container->ptbBuffer) ? tbCurrentToken : *container->ptbBuffer) << prenumber;
            else if (container->pssSubString) {
               if (container->fCompare) {
                  int referenceLength = container->pssSubString->length(), prenumberLength = prenumber.length();
                  if (referenceLength < prenumberLength)
                     container->fDifferent = true;
                  else {
                     container->fDifferent = container->pssSubString->compare(prenumber, prenumberLength) != CREqual;
                     if (!container->fDifferent)
                        container->pssSubString->advance(prenumberLength);
                  };
               }
               else if (container->fImmediateUse)
                  *container->pssSubString = prenumber;
            }
            in.setToEnd(prenumber);
            return RRNeedChars;
         };
      };
      if (chunk.string[pos] == '\0' && !doesForce) {
         STG::SubString prenumber(in);
         prenumber.setUpperClosed().setLength(pos);
         in.advance(pos);
         column += pos;
         if (!container || !container->fImmediateUse)
            ((!container || !container->ptbBuffer) ? tbCurrentToken : *container->ptbBuffer) << prenumber;
         else if (container->pssSubString) {
            if (container->fCompare) {
               int referenceLength = container->pssSubString->length(), prenumberLength = prenumber.length();
               if (referenceLength < prenumberLength)
                  container->fDifferent = true;
               else {
                  container->fDifferent = container->pssSubString->compare(prenumber, prenumberLength) != CREqual;
                  if (!container->fDifferent)
                     container->pssSubString->advance(prenumberLength);
               };
            }
            else if (container->fImmediateUse)
               *container->pssSubString = prenumber;
         }
         in.setToEnd(prenumber);
         return RRNeedChars;
      };
   };
   if ((chContext < NCSignExponent) && isCharDigit(chunk.string[pos],
         (NumberContext) (chContext & NCFormat))) {
      if ((chContext & ~UPrefix) == NCNone)
         chContext = NCInteger | (chContext & UPrefix);
      ++pos;
      while (isCharDigit(chunk.string[pos], (NumberContext) (chContext & NCFormat)))
         ++pos;
   };
   if ((chContext < NCFractional) && chunk.string[pos] == '.') {
      ++pos;
      chContext = NCFractional | (chContext & UPrefix);
      while (isCharDigit(chunk.string[pos], (NumberContext) (chContext & NCFormat)))
         ++pos;
   };
   if ((chContext < NCSignExponent) && (chunk.string[pos] == 'e' || chunk.string[pos] == 'E')) {
      ++pos;
      chContext = NCSignExponent | (chContext & UPrefix);
   };
   if (((chContext & ~UPrefix) == NCSignExponent)
         && (chunk.string[pos] == '+' || chunk.string[pos] == '-')) {
      ++pos;
      chContext = NCExponent | (chContext & UPrefix);
      while (SubString::Traits::isdigit(chunk.string[pos]))
         ++pos;
   };
   while ((chContext >= NCSignExponent) && SubString::Traits::isdigit(chunk.string[pos]))
      ++pos;

   if ((pos > 0) && (!container || !container->fImmediateUse))
      ((!container || !container->ptbBuffer) ? tbCurrentToken : *container->ptbBuffer)
         << STG::SubString(in).setUpperClosed().setLength(pos);

   if (pos < in.length() || doesForce) {
LCreateToken:
      SubString number(in);
      number.setUpperClosed().setLength(pos);
      if (!container || !container->fImmediateUse) {
         if (container && container->pssSubString && !container->fImmediateUse)
            tbCurrentToken >> *container->pssSubString;
      }
      else if (container && container->pssSubString) {
         if (container->fCompare) {
            container->fDifferent = container->pssSubString->length() != number.length()
                  || *container->pssSubString != number;
            if (!container->fDifferent)
               container->pssSubString->setToEnd();
         }
         else if (container->fImmediateUse)
            *container->pssSubString = number;
      }

      in.advance(pos);
      if (!container || container->pntNumberToken) {
         NumberToken* token = nullptr;
         if (!container)
            tToken = Token(token = new NumberToken);
         else
            token = container->pntNumberToken;
         tbCurrentToken >> token->content();
         if (chContext & NCFormat) {
            if ((chContext & NCFormat) == NCBinary)
               token->setBitCoding();
            else if ((chContext & NCFormat) == NCOctal)
               token->setOctalCoding();
            else if ((chContext & NCFormat) == NCHexa)
               token->setHexaDecimalCoding();
         };
         if ((chContext & NCNegative) == NCNegative)
            token->setNegative();
         if (chContext >= NCFractional)
            token->setFloat();
         rpmFstState = nullptr;
         chContext = '\0';
         column += pos;
         return RRHasToken;
      };
      if (!container->fImmediateUse && !container->pssSubString && !container->ptbBuffer)
         tbCurrentToken.clear();
      rpmFstState = nullptr;
      chContext = '\0';
      column += pos;
      return RRContinue;
   };
   in.advance(pos);
   column += pos;
   return RRNeedChars;
}

GenericLexer::ReadResult
GenericLexer::readAlphaToken(SubString& in, unsigned& line, unsigned& column, bool doesForce, SubStringContainer* container) {
   using namespace DGenericLexer;
   STG::TChunk<char> chunk = in.getChunk();
   if (chunk.length == 0)
      return doesForce ? RRFinished : RRNeedChars;
   if (chunk.length < 6 && !doesForce)
      return RRNeedChars;

   int pos = 0;
   char ch = chunk.string[pos];
   while ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9') || ch == '_') {
      ++pos;
      ch = chunk.string[pos];
   }
   ComparisonResult compare = CRNonComparable;
   if (pos == 4 && (compare = in.compareI("true", pos)) == CREqual)
      tToken = Token(AbstractToken::TTrue);
   else if (pos == 5 && (compare = in.compareI("false", pos)) == CREqual)
      tToken = Token(AbstractToken::TFalse);
   else if (pos == 4 && (compare = in.compareI("null")) == CREqual)
      tToken = Token(AbstractToken::TNull);
   else
      throw EReadError();
   in.advance(pos);
   rpmFstState = nullptr;
   chContext = '\0';
   column += pos;
   return RRHasToken;
}

GenericLexer::ReadResult
GenericLexer::readStringToken(SubString& in, unsigned& line, unsigned& column, bool doesForce, SubStringContainer* container) {
   int endPos;
   unsigned lineEnd = line;
   unsigned columnEnd = column;
   endPos = in.scanPos('"');
   if (endPos >= 0)
      columnEnd += endPos + 1;
   else
      columnEnd += in.length() - 1;

   if (endPos >= 0 || doesForce) {
      bool hasEndString = true;
      if (endPos < 0) {
         ++columnEnd;
         endPos = in.length();
         hasEndString = false;
         // leErrorList.insertNewAtEnd(new Error(line, column,
         //    SString("string not terminated")));
      }
      SubString text(in);
      text.setUpperClosed().setLength(endPos);
      if (!container || !container->fImmediateUse) {
         ((!container || !container->ptbBuffer) ? tbCurrentToken : *container->ptbBuffer) << text;
         if (container && container->pssSubString && !container->fImmediateUse)
            tbCurrentToken >> *container->pssSubString;
      }
      else if (container && container->pssSubString) {
         if (container->fCompare) {
            container->fDifferent = container->pssSubString->length() != text.length()
                  || *container->pssSubString != text;
            if (!container->fDifferent)
               container->pssSubString->setToEnd();
         }
         else if (container->fImmediateUse)
            *container->pssSubString = text;
      }
      in.advance(hasEndString ? (endPos+1) : endPos);

      if (!container) {
         StringToken* result = new StringToken;
         tToken = Token(result);
         tbCurrentToken >> result->string();
      }
      else if (!container->fImmediateUse && !container->pssSubString && !container->ptbBuffer)
         tbCurrentToken.clear();
      rpmFstState = nullptr;
   }
   else if (!doesForce) {
      STG::SubString pretext(in);
      if (!container || !container->fImmediateUse)
         ((!container || !container->ptbBuffer) ? tbCurrentToken : *container->ptbBuffer) << pretext;
      else if (container->pssSubString) {
         if (container->fCompare) {
            int referenceLength = container->pssSubString->length(), pretextLength = pretext.length();
            if (referenceLength < pretextLength)
               container->fDifferent = true;
            else {
               container->fDifferent = container->pssSubString->compare(pretext, pretextLength) != CREqual;
               if (!container->fDifferent)
                  container->pssSubString->advance(pretextLength);
            };
         }
         else if (container->fImmediateUse)
            *container->pssSubString = pretext;
      }
      in.setToEnd(pretext);
      line = lineEnd;
      column = columnEnd;
      return RRNeedChars;
   }
   line = lineEnd;
   column = columnEnd;
   return RRHasToken;
}

GenericLexer::ReadResult
GenericLexer::readMain(SubString& buffer, unsigned& line, unsigned& column, bool doesForce, SubStringContainer* container) {
   using namespace DGenericLexer;
   STG::TChunk<char> chunk = buffer.getChunk();
   if (chunk.length == 0)
      return doesForce ? RRFinished : RRNeedChars;
   unsigned pos = 0;
   while (chunk.length > pos
         && SubString::Traits::isspace(chunk.string[pos])) {
      if (chunk.string[pos] == '\n') {
         ++line;
         column = 1;
      }
      else
         ++column;
      ++pos;
   };
   buffer.advance(pos);
   if (chunk.length == pos)
      return doesForce ? RRFinished : RRNeedChars;

   char ch = chunk.string[pos];
   switch (ch) {
      case '[':
         pos += 1; column += 1;
         buffer.advance();
         tToken = Token(AbstractToken::TOpenBracket);
         return RRHasToken;
      case ']':
         pos += 1; column += 1;
         buffer.advance();
         tToken = Token(AbstractToken::TCloseBracket);
         return RRHasToken;
      case '{':
         pos += 1; column += 1;
         buffer.advance();
         tToken = Token(AbstractToken::TOpenBrace);
         return RRHasToken;
      case '}':
         pos += 1; column += 1;
         buffer.advance();
         tToken = Token(AbstractToken::TCloseBrace);
         return RRHasToken;
      case ',':
         pos += 1; column += 1;
         buffer.advance();
         tToken = Token(AbstractToken::TComma);
         return RRHasToken;
      case ':':
         pos += 1; column += 1;
         buffer.advance();
         tToken = Token(AbstractToken::TColon);
         return RRHasToken;
   };
   if (ch == '0') {
      if (!doesForce && chunk.length - pos < 2)
         return RRNeedChars;
      char chs = chunk.string[pos+1];
      if ((chs == 'x') || (chs == 'X')) {
         rpmFstState = &GenericLexer::readNumberToken; 
         pos += 2; column += 2;
         if (oOptions == OFullToken)
            tbCurrentToken << SubString(buffer).setUpperClosed().setLength(2);
         buffer.advance(2);
         chContext = NCHexa;
         if (oOptions == OPartialToken) {
            tToken = Token(AbstractToken::TNumber);
            return RRHasToken;
         }
         return (this->*rpmFstState)(buffer, line, column, doesForce, container);
      }
      else if ((chs == 'b')|| (chs == 'B')) {
         rpmFstState = &GenericLexer::readNumberToken; 
         pos += 2; column += 2;
         if (oOptions == OFullToken)
            tbCurrentToken << SubString(buffer).setUpperClosed().setLength(2);
         buffer.advance(2);
         chContext = NCBinary;
         if (oOptions == OPartialToken) {
            tToken = Token(AbstractToken::TNumber);
            return RRHasToken;
         }
         return (this->*rpmFstState)(buffer, line, column, doesForce, container);
      }
      else if (chs >= '0' && chs <= '7') {
         rpmFstState = &GenericLexer::readNumberToken; 
         chContext = NCOctal;
         if (oOptions == OFullToken)
            tbCurrentToken << ch;
         pos += 1; column += 1;
         buffer.advance();
         if (oOptions == OPartialToken) {
            tToken = Token(AbstractToken::TNumber);
            return RRHasToken;
         }
         return (this->*rpmFstState)(buffer, line, column, doesForce, container);
      };
   };
   if (ch == '+' || ch == '-') {
      rpmFstState = &GenericLexer::readNumberToken; 
      chContext = (ch == '+') ? NCPositive : NCNegative;
      pos += 1; column += 1;
      if (oOptions == OFullToken)
         tbCurrentToken << ch;
      buffer.advance();
      if (oOptions == OPartialToken) {
         tToken = Token(AbstractToken::TNumber);
         return RRHasToken;
      }
      return (this->*rpmFstState)(buffer, line, column, doesForce, container);
   }
   if (SubString::Traits::isdigit(ch)) {
      rpmFstState = &GenericLexer::readNumberToken; 
      chContext = (char) 0;
      if (oOptions == OPartialToken) {
         tToken = Token(AbstractToken::TNumber);
         return RRHasToken;
      }
      return (this->*rpmFstState)(buffer, line, column, doesForce, container);
   };
   if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || ch == '_') {
      rpmFstState = &GenericLexer::readAlphaToken; 
      chContext = (char) 0;
      return (this->*rpmFstState)(buffer, line, column, doesForce, container);
   };
   if (ch == '"') {
      pos += 1; column += 1;
      buffer.advance();
      rpmFstState = &GenericLexer::readStringToken; 
      if (oOptions == OPartialToken) {
         tToken = Token(AbstractToken::TString);
         return RRHasToken;
      }
      return (this->*rpmFstState)(buffer, line, column, doesForce, container);
   }

   throw EReadError();
}

}} // end of namespace STG::JSon

