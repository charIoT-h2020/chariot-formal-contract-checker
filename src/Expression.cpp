#include "Expression.h"
#include "TString/TernaryTreeInterface.h"
#include "TString/TernaryTreeInterface.template"
#include "Numerics/Integer.h"
#include "Numerics/HostFloating.h"

STG::SubString
VirtualExpressionNode::ssJSonContent = STG::SString("content");

bool
VirtualExpressionNode::setArgumentFromText(PNT::TMngPointer<VirtualExpressionNode>& argument,
      const STG::SubString& text, STG::JSon::CommonParser::Arguments& context,
      struct _DomainElementFunctions* functions) {
   if (text == "register")
      argument.absorbElement(new RegisterAccessNode);
   else if (text == "indirect")
      argument.absorbElement(new IndirectionNode);
   else if (text == "domain")
      argument.absorbElement(new DomainNode(functions));
   else if (text == "operation")
      argument.absorbElement(new OperationNode);
   else if (!context.addErrorMessage(STG::SString("unknown type for expression")))
      return false;
   return true;
}

STG::SubString
VirtualExpressionNode::getTextFromArgument(VirtualExpressionNode& argument) {
   auto typeExpression = argument.getTypeExpression();
   switch (typeExpression) {
      case TERegisterAccess: return STG::SString("register");
      case TEIndirection: return STG::SString("indirect");
      case TEDomain: return STG::SString("domain");
      case TEOperation: return STG::SString("operation");
   };
   return STG::SString();
}

/* Implementation of the class RegisterAccessNode */

RegisterAccessNode::ReadResult
RegisterAccessNode::readJSon(STG::JSon::CommonParser::State& state,
      STG::JSon::CommonParser::Arguments& arguments) {
   typedef STG::JSon::CommonParser Parser;
   ReadResult result = RRContinue;
   bool isEqual;

   enum Delimiters
      {  DBegin, DAfterBegin, DReadContent, DEnd };

   #define DefineGoto(Target) case D##Target: goto L##Target;
   switch (state.point()) {
      DefineGoto(Begin)
      DefineGoto(AfterBegin)
      DefineGoto(ReadContent)
      DefineGoto(End)
   };
   #undef DefineGoto

LBegin:
   if (!arguments.isOpenObject()) {
      if (!arguments.addErrorMessage(STG::SString("expected '{'")))
         return result;
   }
   ++state.point();
   if (!arguments.setToNextToken(result)) return result;

LAfterBegin:
   while (!arguments.isCloseObject()) {
      while (arguments.isOpen()) {
         if (!Parser::skipNodeInLoop(state, arguments, result) || result == RRNeedChars) return result;
         if (!arguments.setToNextToken(result)) return result;
      };

      if (arguments.isAddKey()
            && ((result = arguments.isEqualKeyValue(isEqual, ssJSonContent)), isEqual)) {
         if (result == RRNeedChars) return result;
         ++state.point();
         if (!arguments.setToNextToken(result)) return result;

LReadContent:
         if (arguments.isSetString()) {
            if (arguments.setArgumentTextValue() == RRNeedChars) return RRNeedChars;
            ssRegisterName = arguments.valueAsText();
         };
      }
      state.point() = DAfterBegin;
      if (!arguments.setToNextToken(result)) return result;
   }
   state.point() = DEnd;
LEnd:
   arguments.reduceState(state);
   return RRHasToken;
}

RegisterAccessNode::WriteResult
RegisterAccessNode::writeJSon(STG::JSon::CommonWriter::State& state,
      STG::JSon::CommonWriter::Arguments& arguments) const {
   WriteResult result = WRNeedEvent;
   enum Delimiters
      {  DBegin, DAfterBegin, DWriteContent, DEnd };

   #define DefineGoto(Target) case D##Target: goto L##Target;
   switch (state.point()) {
      DefineGoto(Begin)
      DefineGoto(AfterBegin)
      DefineGoto(WriteContent)
      DefineGoto(End)
   };
   #undef DefineGoto

LBegin:
   arguments.setOpenObject();
   ++state.point();
   if (!arguments.writeEvent(result)) return result;

LAfterBegin:
   arguments.setAddKey(ssJSonContent);
   ++state.point();
   if (!arguments.writeEvent(result)) return result;

LWriteContent:
   arguments.setStringValue(ssRegisterName);
   ++state.point();
   if (!arguments.writeEvent(result)) return result;

LEnd:
   arguments.setCloseObject();
   arguments.reduceState(state);
   if (!arguments.writeEvent(result)) return result;
   return WRNeedEvent;
}

/* Implementation of the class IndirectionNode */

IndirectionNode::ReadResult
IndirectionNode::readJSon(STG::JSon::CommonParser::State& state,
      STG::JSon::CommonParser::Arguments& arguments) {
   typedef STG::JSon::CommonParser Parser;
   ReadResult result = RRContinue;

   enum Delimiters
      {  DBegin, DAfterBegin, DReadAddress, DReadAddressContent, DIdentifyAddressContent,
         DReadSize, DEnd
      };

   #define DefineGoto(Target) case D##Target: goto L##Target;
   switch (state.point()) {
      DefineGoto(Begin)
      DefineGoto(AfterBegin)
      DefineGoto(ReadAddress)
      DefineGoto(ReadAddressContent)
      DefineGoto(IdentifyAddressContent)
      DefineGoto(ReadSize)
      DefineGoto(End)
   };
   #undef DefineGoto

LBegin:
   if (!arguments.isOpenObject()) {
      if (!arguments.addErrorMessage(STG::SString("expected '{'")))
         return result;
   }
   ++state.point();
   if (!arguments.setToNextToken(result)) return result;

LAfterBegin:
   while (!arguments.isCloseObject()) {
      while (arguments.isOpen()) {
         if (!Parser::skipNodeInLoop(state, arguments, result) || result == RRNeedChars) return result;
         if (!arguments.setToNextToken(result)) return result;
      };

      if (arguments.isAddKey()
            && ((result = arguments.setArgumentKey()), arguments.key() == "address")) {
         if (result == RRNeedChars) return result;
         ++state.point();
         if (!arguments.setToNextToken(result)) return result;
LReadAddress:
         if (arguments.isOpenObject()) {
            ++state.point();
            if (!arguments.setToNextToken(result)) return result;
LReadAddressContent:
            while (!arguments.isCloseObject()) {
               while (arguments.isOpen()) {
                  if (!Parser::skipNodeInLoop(state, arguments, result) || result == RRNeedChars) return result;
                  if (!arguments.setToNextToken(result)) return result;
               };
               if (arguments.isCloseArray())
                  {  AssumeUncalled }
               else if (arguments.isAddKey()
                     && ((result = arguments.setArgumentKey()), arguments.key() == "type")) {
                  if (result == RRNeedChars) return result;
                  state.point() = DIdentifyAddressContent;
                  if (!arguments.setToNextToken(result)) return result;
LIdentifyAddressContent:
                  if (arguments.isSetString()) {
                     if (arguments.setArgumentTextValue() == RRNeedChars) return RRNeedChars;
                     if (!setArgumentFromText(mpAddress, arguments.valueAsText(), arguments,
                              state.getResult((RuleResult*) nullptr)))
                        return RRContinue;
                  }
               }
               else
                  if (result == RRNeedChars) return result;
               if (arguments.isAddKey()
                     && ((result = arguments.setArgumentKey()), arguments.key() == "content")) {
                  if (result == RRNeedChars) return result;
                  state.point() = DReadAddressContent;
                  auto ruleResult = state.getResult((VirtualExpressionNode::RuleResult*) nullptr);
                  arguments.shiftState(state, *mpAddress, &VirtualExpressionNode::readJSon,
                        (VirtualExpressionNode::RuleResult*) nullptr);
                  state.setResult(std::move(ruleResult));
                  if (!arguments.setToNextToken(result)) return result;
                  if (!arguments.parseTokens(state, result)) return result;
                  continue;
               }
               else
                  if (result == RRNeedChars) return result;
               state.point() = DReadAddressContent;
               if (!arguments.setToNextToken(result)) return result;
            }
         }
         state.point() = DAfterBegin;
      }
      else if (arguments.isAddKey() && (result == RRNeedChars))
         return result;

      if (arguments.isAddKey()
            && ((result = arguments.setArgumentKey()), arguments.key() == "size")) {
         if (result == RRNeedChars) return result;
         state.point() = DReadSize;
         if (!arguments.setToNextToken(result)) return result;
LReadSize:
         if (arguments.isSetValue()
               && arguments.setArgumentToInt() == RRNeedChars) return RRNeedChars;
         if (arguments.isSetInt())
            uSizeInBytes = arguments.valueAsInt();
      }
      else if (arguments.isAddKey() && (result == RRNeedChars))
         return result;
      state.point() = DAfterBegin;
      if (!arguments.setToNextToken(result)) return result;
   }
   state.point() = DEnd;
LEnd:
   arguments.reduceState(state);
   return RRHasToken;
}

IndirectionNode::WriteResult
IndirectionNode::writeJSon(STG::JSon::CommonWriter::State& state,
      STG::JSon::CommonWriter::Arguments& arguments) const {
   WriteResult result = WRNeedEvent;
   enum Delimiters
      {  DBegin, DAfterBegin, DWriteAddress, DWriteAddressType, DIdentifyAddressType,
         DWriteAddressContent, DCloseAddress, DWriteSize, DWriteSizeContent, DEnd
      };

   #define DefineGoto(Target) case D##Target: goto L##Target;
   switch (state.point()) {
      DefineGoto(Begin)
      DefineGoto(AfterBegin)
      DefineGoto(WriteAddress)
      DefineGoto(WriteAddressType)
      DefineGoto(IdentifyAddressType)
      DefineGoto(WriteAddressContent)
      DefineGoto(CloseAddress)
      DefineGoto(WriteSize)
      DefineGoto(WriteSizeContent)
      DefineGoto(End)
   };
   #undef DefineGoto

LBegin:
   arguments.setOpenObject();
   ++state.point();
   if (!arguments.writeEvent(result)) return result;

LAfterBegin:
   if (mpAddress.isValid()) {
      arguments.setAddKey(STG::SString("address"));
      ++state.point();
      if (!arguments.writeEvent(result)) return result;

LWriteAddress:
      arguments.setOpenObject();
      ++state.point();
      if (!arguments.writeEvent(result)) return result;

LWriteAddressType:
      arguments.setAddKey(STG::SString("type"));
      ++state.point();
      if (!arguments.writeEvent(result)) return result;

LIdentifyAddressType:
      arguments.setStringValue(getTextFromArgument(*mpAddress));
      ++state.point();
      if (!arguments.writeEvent(result)) return result;

LWriteAddressContent:
      arguments.setAddKey(STG::SString("content"));
      ++state.point();
      arguments.shiftState(state, *mpAddress, &VirtualExpressionNode::writeJSon,
            (char*) nullptr);
      if (!arguments.writeEvent(result)) return result;
      if (!arguments.writeTokens(state, result)) return result;

LCloseAddress:
      arguments.setCloseObject();
      ++state.point();
      if (!arguments.writeEvent(result)) return result;
   }
   else
      state.point() = DWriteSize;

LWriteSize:
   arguments.setAddKey(STG::SString("size"));
   ++state.point();
   if (!arguments.writeEvent(result)) return result;

LWriteSizeContent:
   arguments.setIntValue(uSizeInBytes);
   ++state.point();
   if (!arguments.writeEvent(result)) return result;

LEnd:
   arguments.setCloseObject();
   arguments.reduceState(state);
   if (!arguments.writeEvent(result)) return result;
   return WRNeedEvent;
}

/* Implementation of the class DomainNode::AbstractToken */

class DomainNode::AbstractToken : public EnhancedObject, public ExtendedParameters {
  public:
   enum Type
      {  TUndefined, TNumber, TIdentifier, TKeyword, TOperatorPunctuator };
  protected:
   DefineExtendedParameters(3, ExtendedParameters)
   DefineSubExtendedParameters(Type, 3, INHERITED)

   void setType(Type type) { AssumeCondition(!hasTypeField()); mergeTypeField(type); }
  
  public:
   AbstractToken() {}
   AbstractToken(Type type) { mergeTypeField(type); }
   AbstractToken(const AbstractToken& source) = default;
   AbstractToken& operator=(const AbstractToken& source) = default;
   DefineCopy(AbstractToken)

   virtual bool isValid() const override { return hasTypeField(); }
   Type getType() const { return (Type) queryTypeField(); }
};

class DomainNode::NumberToken : public AbstractToken {
  private:
   STG::SubString ssContent;
   int uSize;
   int uAdditionalSize;

   enum IntegerExtension { IENoExtension, IEBit };
   enum Coding { CDecimal, CBit, COctal, CHexaDecimal };

  protected:
   DefineExtendedParameters(4, AbstractToken)
   DefineSubExtendedParameters(BasicType, 1, INHERITED)
   DefineSubExtendedParameters(IntegerExtension, 1, BasicType)
   DefineSubExtendedParameters(Coding, 2, IntegerExtension)

  public:
   NumberToken() : ssContent(STG::SString()), uSize(0), uAdditionalSize(0)
      {  setType(TNumber); }
   NumberToken(const STG::SubString& content, bool isFloat)
      :  ssContent(content), uSize(0), uAdditionalSize(0)
      {  setType(TNumber); 
         mergeBasicTypeField(isFloat);
      }
   NumberToken(const NumberToken& source) = default;
   DefineCopy(NumberToken)

   const STG::SubString& getContent() const { return ssContent; }

   bool isInteger() const { return !hasBasicTypeField() && !hasIntegerExtensionField(); }
   bool isFloatingPoint() const { return hasBasicTypeField(); }
   bool isBit() const { return hasIntegerExtensionField(); }
   bool isIntegerOrBit() const { return !hasBasicTypeField(); }

   void setIntegerType() { clearBasicTypeField(); }
   void setFloatingPointType() { mergeBasicTypeField(1); }
   int getSize() const { return uSize; }
   void setSize(int size) { uSize = size; }
   void setDoubleSize(int firstSize, int secondSize) { uSize = firstSize; uAdditionalSize = secondSize; }
   int getFirstSize() const { return uSize; }
   int getSecondSize() const { return uAdditionalSize; }

   void addBitExtension() { mergeIntegerExtensionField(IEBit); }
   bool hasBitExtension() const { return queryIntegerExtensionField() == IEBit; }
   bool hasExtension() const { return hasIntegerExtensionField(); }

   void setBitCoding() { AssumeCondition(!hasCodingField()) mergeCodingField(CBit); }
   void setOctalCoding() { AssumeCondition(!hasCodingField()) mergeCodingField(COctal); }
   void setHexaDecimalCoding() { AssumeCondition(!hasCodingField()) mergeCodingField(CHexaDecimal); }
   bool hasSpecialCoding() const { return hasCodingField(); }
   bool hasBitCoding() const { return queryCodingField() == CBit; }
   bool hasOctalCoding() const { return queryCodingField() == COctal; }
   bool hasHexaDecimalCoding() const { return queryCodingField() == CHexaDecimal; }
};

class DomainNode::KeywordToken : public AbstractToken {
  private:
   typedef AbstractToken inherited;

  public:
   enum Type
      {  TUndefined, TMerge, TTop, TTopBit, TSigned, TUnsigned, TSymbolic,
         TMergeSymbolic, TTopSymbolic, TTopBitSymbolic, TSignedSymbolic, TUnsignedSymbolic,
      };

  protected:
   DefineExtendedParameters(6, inherited)
   DefineSubExtendedParameters(Type, 4, INHERITED)
   DefineSubExtendedParameters(Size, 2, Type)

  public:
   KeywordToken(Type type)
      {  mergeTypeField(type);
         inherited::setType(AbstractToken::TKeyword);
      }
   KeywordToken(const KeywordToken& source) = default;
   KeywordToken& operator=(const KeywordToken& source) = default;
   DefineCopy(KeywordToken)

   Type getType() const { return (Type) queryTypeField(); }
   bool isSized() const { return hasSizeField(); }
   bool isSingleSized() const { return querySizeField() == 1; }
   bool isDoubleSized() const { return querySizeField() == 2; }
};

class DomainNode::IdentifierToken : public AbstractToken {
  private:
   STG::SubString ssIdentifier;

  public:
   IdentifierToken() : ssIdentifier(STG::SString())
      {  setType(TIdentifier); }
   IdentifierToken(const STG::SubString& identifier) : ssIdentifier(identifier)
      {  setType(TIdentifier); }
   IdentifierToken(const IdentifierToken& source) = default;
   IdentifierToken& operator=(const IdentifierToken& source) = default;
   DefineCopy(IdentifierToken)
   DDefineAssign(IdentifierToken)

   const STG::SubString& identifier() const { return ssIdentifier; }
};

class DomainNode::SizedKeywordToken : public KeywordToken {
  private:
   int uSize;

  public:
   SizedKeywordToken(Type type, int size) : KeywordToken(type), uSize(size)
      {  mergeSizeField(1); }
   SizedKeywordToken(const SizedKeywordToken& source) = default;
   SizedKeywordToken& operator=(const SizedKeywordToken& source) = default;
   DefineCopy(SizedKeywordToken)

   int getSize() const { return uSize; }
};

class DomainNode::FloatSizedKeywordToken : public KeywordToken {
  private:
   int uSizeExponent, uSizeMantissa;

  public:
   FloatSizedKeywordToken(Type type, int sizeExponent, int sizeMantissa)
      :  KeywordToken(type), uSizeExponent(sizeExponent), uSizeMantissa(sizeMantissa)
      {  mergeSizeField(2); }
   FloatSizedKeywordToken(const FloatSizedKeywordToken& source) = default;
   DefineCopy(FloatSizedKeywordToken)

   int getSizeExponent() const { return uSizeExponent; }
   int getSizeMantissa() const { return uSizeMantissa; }
};

class DomainNode::OperatorPunctuatorToken : public AbstractToken {
  private:
   typedef AbstractToken inherited;

  public:
   enum Type
      {  TUndefined, TOpenParen, TCloseParen, TOpenBracket, TCloseBracket,
         TOpenBrace, TCloseBrace, TComma, TMinus, TPlus };

  protected:
   DefineExtendedParameters(3, inherited)

  public:
   OperatorPunctuatorToken(Type type)
      {  mergeOwnField(type);
         inherited::setType(AbstractToken::TOperatorPunctuator);
      }
   OperatorPunctuatorToken(const OperatorPunctuatorToken& source) = default;
   DefineCopy(OperatorPunctuatorToken)
   Type getType() const { return (Type) queryOwnField(); }
};

/* Implementation of the class DomainNode::Lexer */

struct DomainNode::IdentifierTokenResult {
   enum Choice { CUndefined, CIdentifier, CKeyword, CSizedKeyword, CFloatSizedKeyword } choice = CUndefined;
   IdentifierToken identifier;
   KeywordToken keyword = KeywordToken(KeywordToken::TUndefined);
   SizedKeywordToken sizedKeyword = SizedKeywordToken(KeywordToken::TUnsigned, 0);
   FloatSizedKeywordToken floatSizedKeyword = FloatSizedKeywordToken(KeywordToken::TUnsigned, 0, 0);

   IdentifierTokenResult() {}

   bool isKeyword() const { return choice >= CKeyword; }
   const KeywordToken& getKeyword() const
      {  return (choice == CKeyword) ? keyword : ((choice == CSizedKeyword)
            ? (const KeywordToken&) sizedKeyword : (const KeywordToken&) floatSizedKeyword); }
   bool isDomain() const
      {  bool result = false;
         if (choice >= CKeyword) {
            auto type = getKeyword().getType();
            result = type == KeywordToken::TTop || type == KeywordToken::TTopBit
                  || type == KeywordToken::TTopSymbolic || type == KeywordToken::TTopBitSymbolic;
         }
         return result;
      }
   bool isQualifier() const
      {  bool result = false;
         if (choice == CKeyword) {
            auto type = keyword.getType();
            result = type == KeywordToken::TUnsigned || type == KeywordToken::TSigned
                  || type == KeywordToken::TUnsignedSymbolic || type == KeywordToken::TSignedSymbolic;
         }
         return result;
      }
};

class DomainNode::Lexer : public STG::Lexer::Base {
  private:
   bool fReadNumber = false;
   bool fReadIdentifier = false;
   char chContext = '\0';
   char chExtension = '\0';
   AbstractToken tToken;
   typedef COL::TCopyCollection<COL::TTernaryTree<STG::SubString, KeywordToken> > KeywordsCollection;
   static KeywordsCollection ttKeywords;
   STG::JSon::CommonParser::Arguments& jpaErrorList;

  protected:
   ReadResult readMain(STG::SubString& in, unsigned& pos);
   ReadResult readNumberToken(STG::SubString& in, unsigned& pos, NumberToken* container);
   ReadResult readIdentifierToken(STG::SubString& in, unsigned& pos, IdentifierTokenResult* container);

  public:
   Lexer(STG::JSon::CommonParser::Arguments& errorList);

   ReadResult readToken(STG::SubString& in, unsigned& pos)
      {  return fReadNumber ? readNumberToken(in, pos, nullptr) :
               (fReadIdentifier ? readIdentifierToken(in, pos, nullptr) : readMain(in, pos));
      }
   ReadResult readNumericToken(STG::SubString& in, NumberToken& result, unsigned& pos)
      {  AssumeCondition(fReadNumber)
         return readNumberToken(in, pos, &result);
      }
   ReadResult readIdentifierToken(STG::SubString& in, IdentifierTokenResult& result, unsigned& pos)
      {  AssumeCondition(fReadIdentifier)
         return readIdentifierToken(in, pos, &result);
      }

   void setToken(const AbstractToken& token)
      {  AssumeCondition(tToken.getType() == AbstractToken::TUndefined);
         tToken = token;
      }
   const AbstractToken& getToken() { return tToken; }
   bool doesNeedClear() const { return tToken.getType(); }
   void clearToken()
      {  fReadNumber = false;
         fReadIdentifier = false;
         chContext = '\0';
         chExtension = '\0';
         tToken = AbstractToken();
      }

   bool addErrorMessage(const STG::SubString& message, unsigned pos)
      {  unsigned column = jpaErrorList.getColumn();
         jpaErrorList.setColumn(column+pos);
         bool result = jpaErrorList.addErrorMessage(message);
         jpaErrorList.setColumn(column);
         return result;
      }
   bool doesStopAfterTooManyErrors() const
      {  return jpaErrorList.doesStopAfterTooManyErrors(); }
};

DomainNode::Lexer::KeywordsCollection DomainNode::Lexer::ttKeywords;

DomainNode::Lexer::Lexer(STG::JSon::CommonParser::Arguments& errorList)
   :  jpaErrorList(errorList) {
   if (ttKeywords.isEmpty())
      ttKeywords = KeywordsCollection(COL::TTernaryTree<STG::SubString, KeywordToken>::InitialNewValues()
         << STG::SString("vv") << KeywordToken(KeywordToken::TMerge)
         << STG::SString("T") << KeywordToken(KeywordToken::TTop)
         << STG::SString("Tb") << KeywordToken(KeywordToken::TTopBit)
         << STG::SString("S") << KeywordToken(KeywordToken::TSigned)
         << STG::SString("U") << KeywordToken(KeywordToken::TUnsigned)
         << STG::SString("_s") << KeywordToken(KeywordToken::TSymbolic)
         << STG::SString("vv_s") << KeywordToken(KeywordToken::TMergeSymbolic)
         << STG::SString("T_s") << KeywordToken(KeywordToken::TTopSymbolic)
         << STG::SString("Tb_s") << KeywordToken(KeywordToken::TTopBitSymbolic)
         << STG::SString("S_s") << KeywordToken(KeywordToken::TSignedSymbolic)
         << STG::SString("U_s") << KeywordToken(KeywordToken::TUnsignedSymbolic));
}

DomainNode::Lexer::ReadResult
DomainNode::Lexer::readMain(STG::SubString& in, unsigned& apos) {
   auto chunk = in.getChunk();
   if (chunk.length == 0)
      return Lexer::RRFinished;
   unsigned pos = 0;
   while (chunk.length > pos
         && STG::SubString::Traits::isspace(chunk.string[pos]))
      ++pos;
   in.advance(pos);
   if (chunk.length == pos) {
      apos += pos;
      return Lexer::RRFinished;
   }

   switch (chunk.string[pos]) {
      case '-':
         pos += 1; apos += pos;
         in.advance();
         tToken = OperatorPunctuatorToken(OperatorPunctuatorToken::TMinus);
         return RRHasToken;
      case '+':
         pos += 1; apos += pos;
         in.advance();
         tToken = OperatorPunctuatorToken(OperatorPunctuatorToken::TPlus);
         return RRHasToken;
      case '(':
         pos += 1; apos += pos;
         in.advance();
         tToken = OperatorPunctuatorToken(OperatorPunctuatorToken::TOpenParen);
         return RRHasToken;
      case ')':
         pos += 1; apos += pos;
         in.advance();
         tToken = OperatorPunctuatorToken(OperatorPunctuatorToken::TCloseParen);
         return RRHasToken;
      case '{':
         pos += 1; apos += pos;
         in.advance();
         tToken = OperatorPunctuatorToken(OperatorPunctuatorToken::TOpenBrace);
         return RRHasToken;
      case '}':
         pos += 1; apos += pos;
         in.advance();
         tToken = OperatorPunctuatorToken(OperatorPunctuatorToken::TCloseBrace);
         return RRHasToken;
      case '[':
         pos += 1; apos += pos;
         in.advance();
         tToken = OperatorPunctuatorToken(OperatorPunctuatorToken::TOpenBracket);
         return RRHasToken;
      case ']':
         pos += 1; apos += pos;
         in.advance();
         tToken = OperatorPunctuatorToken(OperatorPunctuatorToken::TCloseBracket);
         return RRHasToken;
      case '.':
         if (STG::SubString::Traits::isdigit(chunk.string[pos+1])) {
            fReadNumber = true;
            chContext = (char) 0;
            apos += pos;
            tToken = NumberToken();
            return RRHasToken;
         }
         break;
      case ',':
         pos += 1; apos += pos;
         in.advance();
         tToken = OperatorPunctuatorToken(OperatorPunctuatorToken::TComma);
         return RRHasToken;
   };

   char ch = chunk.string[pos];
   if (ch == '0') {
      char chs = chunk.string[pos+1];
      if ((chs == 'x') || (chs == 'X')) {
         fReadNumber = true;
         chContext = (char) 3;
         apos += pos;
         tToken = NumberToken();
         return RRHasToken;
      }
      else if ((chs == 'b') || (chs == 'B')) {
         fReadNumber = true;
         chContext = (char) 1;
         apos += pos;
         tToken = NumberToken();
         return RRHasToken;
      }
      else if (chs >= '0' && chs <= '7') {
         fReadNumber = true;
         chContext = (char) 2;
         apos += pos;
         tToken = NumberToken();
         return RRHasToken;
      };
   };
   if (STG::SubString::Traits::isdigit(ch)) {
      fReadNumber = true;
      chContext = (char) 0;
      apos += pos;
      tToken = NumberToken();
      return RRHasToken;
   };
   if (STG::SubString::Traits::isalnum(ch) || ch == '_') {
      fReadIdentifier = true; 
      apos += pos;
      tToken = IdentifierToken();
      return RRHasToken;
   };
   apos += pos;
   if (!addErrorMessage(STG::SString("unknown character"), apos))
      return RRFinished;
   return RRContinue;
}

namespace Details {

inline bool isCharDigit(char ch, int context) {
   return (context == 0)
      ? STG::SubString::Traits::isdigit(ch) : ((context == 1)
      ? (ch == '0' || ch == '1') : ((context == 2)
      ? STG::SubString::Traits::isoctaldigit(ch) : ((context == 3)
      ? STG::SubString::Traits::ishexadigit(ch) : false)));
}

}

DomainNode::Lexer::ReadResult
DomainNode::Lexer::readNumberToken(STG::SubString& in, unsigned& apos, NumberToken* result) {
   auto chunk = in.getChunk();
   if (chunk.length == 0)
      return Lexer::RRFinished;
   int pos = 0;
   while (Details::isCharDigit(chunk.string[pos], chContext & 0x3) && (chContext >> 2) < 16) {
      if ((chContext >> 2) == 2)
         chContext = (3 << 2) | (chContext & 0x3);
      ++pos;
   };
   if (chunk.string[pos] == '.' && (chContext >> 2) == 0) {
      ++pos;
      chContext = (1 << 2) | (chContext & 0x3);
      while (Details::isCharDigit(chunk.string[pos], chContext & 0x3))
         ++pos;
   };
   if ((chunk.string[pos] == 'e' || chunk.string[pos] == 'E') && (chContext >> 2 == 1)) {
      ++pos;
      chContext = (2 << 2) | (chContext & 0x3);
   };
   if ((chunk.string[pos] == '+' || chunk.string[pos] == '-') && (chContext >> 2 == 2)) {
      ++pos;
      chContext = (3 << 2) | (chContext & 0x3);
      while (Details::isCharDigit(chunk.string[pos], chContext & 0x3))
         ++pos;
   };
   if (chunk.string[pos] == '_' && (chContext >> 2 < 4)) {
      ++pos;
      chContext |= (4 << 2);
      while (STG::SubString::Traits::isdigit(chunk.string[pos]))
         ++pos;
   };
   if (chunk.string[pos] == '_' && (chContext >> 2 < 8)) {
      ++pos;
      chContext |= (8 << 2);
      while (STG::SubString::Traits::isdigit(chunk.string[pos]))
         ++pos;
   };

   enum Shift { SBit = 1, SUnsigned = 2 };

   char ch = chunk.string[pos];
   while ((ch == 'b') || (ch == 'B')) {
      unsigned shift = 0;
      if (ch == 'b' || ch == 'B')
         shift = SBit;
      if ((chContext >> 2) < 16) {
         chExtension = 0;
         chContext |= (16 << 2);
      };
      chExtension |= (1 << shift);
      ++pos;
      ch = chunk.string[pos];
   };

   STG::SubString extendedNumber(in);
   extendedNumber.setUpperClosed().setLength(pos);
   STG::SubString number(extendedNumber);
   number.setUpperClosed();
   int firstSize= 0, secondSize = 0;
   if ((chContext >> 2) & 4) {
      int pos = number.scanPos('_');
      AssumeCondition(pos >= 0)
      STG::SubString first(number, pos+1);
      firstSize = first.readInteger();
      if ((chContext >> 2) & 8) {
         int pos2 = first.scanPos('_');
         AssumeCondition(pos2 >= 0)
         STG::SubString second(first, pos2+1);
         secondSize = second.readInteger();
      };
      number.setLength(pos);
   };
   bool isInteger = (((chContext >> 2) & 0x3) == 0);
   if (result)
      *result = NumberToken(number, !isInteger /* isFloat */);
   if (result)
      tToken = AbstractToken(*result);
   else
      tToken = AbstractToken(NumberToken(number, !isInteger /* isFloat */));
   if (result && ((chContext >> 2) & 4)) {
      if ((chContext >> 2) & 8)
         result->setDoubleSize(firstSize, secondSize);
      else
         result->setSize(firstSize);
   };
      
   bool hasExtensionIssue = false;
   if (result && isInteger) {
      result->setIntegerType();
      if (chExtension & (1 << SBit))
         result->addBitExtension();
   }
   else if (result)
      result->setFloatingPointType();
   fReadNumber = false;
   in.advance(pos);
   apos += pos;
   if (hasExtensionIssue)
      if (!addErrorMessage(STG::SString("bad extension for number"), apos)) return RRFinished;
   return RRHasToken;
}

DomainNode::Lexer::ReadResult
DomainNode::Lexer::readIdentifierToken(STG::SubString& in, unsigned& apos, IdentifierTokenResult* container) {
   auto chunk = in.getChunk();
   if (chunk.length == 0)
      return Lexer::RRFinished;
   unsigned pos = 0;
   while (chunk.length > pos
         && (STG::SubString::Traits::isalnum(chunk.string[pos])
            || (chunk.string[pos] == '_')))
      ++pos;

   STG::SubString identifier(in);
   identifier.setUpperClosed().setLength(pos);
   in.advance(pos);

   STG::SubString extension(identifier);
   extension.setUpperClosed().advance(extension.length()-1);
   if (STG::SubString::Traits::isdigit(extension.getSChar())) {
      while (extension.cstart().getPosition() > 0) {
         extension.advance(-1);
         if (!STG::SubString::Traits::isdigit(extension.getSChar()))
            break;
      };
      if (extension.getSChar() != '_')
         extension.clear();
      else {
         extension.advance(-1);
         if (STG::SubString::Traits::isdigit(extension.getSChar())) {
            STG::SubString firstExtension(extension);
            while (extension.cstart().getPosition() > 0) {
               extension.advance(-1);
               if (!STG::SubString::Traits::isdigit(extension.getSChar()))
                  break;
            };
            if (extension.getSChar() != '_')
               extension = firstExtension;
         }
         else
            extension.advance();
      };
   }
   else
      extension.clear();
   {  STG::SubString content = identifier;
      if (extension.length() > 0)
         content.setUpperClosed().setLength(identifier.getSubStringPos(extension));
      COL::TTernaryTree<STG::SubString, KeywordToken>::Cursor
         keyCusor(ttKeywords);
      if (ttKeywords.locateKey(content, keyCusor, COL::VirtualCollection::RPExact)) {
         tToken = keyCusor.elementAt();
         fReadIdentifier = false;
         if (extension.length() > 0) {
            AssumeCondition(extension.getSChar() == '_')
            extension.advance();
            int first = extension.readInteger();
            if (extension.length() > 0) {
               AssumeCondition(extension.getSChar() == '_')
               extension.advance();
               int second = extension.readInteger();
               if (container) {
                  container->floatSizedKeyword = FloatSizedKeywordToken(
                     ((const KeywordToken&) tToken).getType(), first, second);
                  container->choice = IdentifierTokenResult::CFloatSizedKeyword;
               }
               else
                  tToken = FloatSizedKeywordToken(
                     ((const KeywordToken&) tToken).getType(), first, second);
            }
            else {
               if (container) {
                  container->sizedKeyword = SizedKeywordToken(
                     ((const KeywordToken&) tToken).getType(), first);
                  container->choice = IdentifierTokenResult::CSizedKeyword;
               }
               else
                  tToken = SizedKeywordToken(
                     ((const KeywordToken&) tToken).getType(), first);
            };
         }
         else if (container) {
            container->keyword = KeywordToken(((const KeywordToken&) tToken).getType());
            container->choice = IdentifierTokenResult::CKeyword;
         }
         apos += pos;
         return RRHasToken;
      };
   };
   if (container) {
      container->identifier = IdentifierToken(identifier);
      container->choice = IdentifierTokenResult::CIdentifier;
      tToken = IdentifierToken(container->identifier);
   }
   else
      tToken = IdentifierToken(identifier);
   fReadIdentifier = true;
   return RRHasToken;
}

/* Implementation of the class DomainNode::Parser */

template <typename T>
class DomainNode::Parser : public STG::Lexer::Base {
  public:
   class Arguments {
     private:
      Lexer lLexer;
      int uLocalStackHeight;
      DomainEvaluationEnvironment deeEvaluationEnvironment;
      const STG::IOObject::FormatParameters* pfpFormatParameters;
      STG::SubString* pssBuffer;
      unsigned uPos;
      PNT::AutoPointer<NumberToken> apntNumberToken;
      IdentifierTokenResult itrIdentifierResult;
      friend class Parser;

     public:
      Arguments(STG::JSon::CommonParser::Arguments& errorList)
         :  lLexer(errorList), uLocalStackHeight(0), deeEvaluationEnvironment{},
            pssBuffer(nullptr), uPos(0)
         {  deeEvaluationEnvironment.defaultDomainType = DISUndefined; // DISShareTop;
         }

      DomainEvaluationEnvironment& evaluationEnvironment() { return deeEvaluationEnvironment; }
      bool addErrorMessage(const STG::SubString& message)
         {  return lLexer.addErrorMessage(message, uPos); }
      bool setToNextToken(ReadResult& result)
         {  bool booleanResult = false;
            AssumeCondition(pssBuffer)
            if (isValidRange()) {
               if (lLexer.doesNeedClear()) {
                  lLexer.clearToken();
                  apntNumberToken.release();
               }
               do {
                  result = lLexer.readToken(*pssBuffer, uPos);
               } while (result == RRContinue);
               booleanResult = (result == RRHasToken);
            }
            else
               result = RRNeedChars;
            if (!booleanResult)
               clearRange();
            return booleanResult;
         }
      bool parseTokens(STG::Parser::TStateStack<Arguments>& state, ReadResult& result)
         {  bool booleanResult = false;
            size_t originalSize = state.getTotalSize();
            int originalPoint = state.hasUpLast() ? state.upLast().point() : -1;
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
            return booleanResult;
         }

      bool isValidRange() const
         {  return (uLocalStackHeight >= 0 && uLocalStackHeight <= 50); }
      void shift() { ++uLocalStackHeight; }
      void reduce() { --uLocalStackHeight; }
      void clearRange() { uLocalStackHeight = 0; }

      Lexer& lexer() { return lLexer; }

      const STG::IOObject::FormatParameters& getFormatParameters() const
         {  AssumeCondition(pfpFormatParameters) return *pfpFormatParameters; }
      void setBuffer(STG::SubString& buffer) { pssBuffer = &buffer; }
      void clearBuffer() { pssBuffer = nullptr; }
      STG::SubString& getBuffer() const { AssumeCondition(pssBuffer) return *pssBuffer; }
      typedef STG::Lexer::Base::ReadResult ResultAction;

      void clearEvaluationEnvironment()
         {  memset(&deeEvaluationEnvironment, 0, sizeof(deeEvaluationEnvironment));
            deeEvaluationEnvironment.defaultDomainType = DISUndefined; // DISShareTop;
         }
      const NumberToken& getNumberToken() const { return *apntNumberToken; }
      ReadResult setArgumentToNumber()
         {  AssumeCondition(lLexer.getToken().getType() == AbstractToken::TNumber)
            ReadResult result = RRContinue;
            if (!apntNumberToken.isValid()) {
               apntNumberToken.absorbElement(new NumberToken());
               result = lLexer.readNumericToken(*pssBuffer, *apntNumberToken, uPos);
            };
            if (isValidRange())
               return result;
            return RRContinue;
         }
      const IdentifierTokenResult& getIdentifierResult() const { return itrIdentifierResult; }
      ReadResult setArgumentToIdentifier()
         {  AssumeCondition(lLexer.getToken().getType() == AbstractToken::TIdentifier)
            ReadResult result = lLexer.readIdentifierToken(*pssBuffer, itrIdentifierResult, uPos);
            if (isValidRange())
               return result;
            return RRContinue;
         }
      void reduceState(STG::Parser::TStateStack<Arguments>& state)
         {  state.reduce();
            reduce();
         }
      template <class TypeObject, typename ReadPointerMethod, class TypeResult>
      void shiftState(STG::Parser::TStateStack<Arguments>& state,
            TypeObject& object, ReadPointerMethod parseMethod, TypeResult* ruleResult)
         {  shift();
            state.shift(object, parseMethod, ruleResult);
         }
   };

   typedef STG::Parser::TStateStack<Arguments> State;

  private:
   State sState;
   Arguments aArguments;

  public:
   template <class TypeObject> Parser(TypeObject& object,
         STG::JSon::CommonParser::Arguments& errorList)
      :  aArguments(errorList) {}
   // {  sState.shift(object, &TypeObject::readToken); }

   State& state() { return sState; }
   const State& state() const { return sState; }
   void parse(STG::SubString& in)
      {  aArguments.setBuffer(in);
         ReadResult parseResult = RRNeedChars;
         while (parseResult != RRFinished) {
            while (parseResult == RRNeedChars) {
               do {
                  parseResult = aArguments.lLexer.readToken(in, aArguments.uPos);
               } while (parseResult == RRContinue);
               if (parseResult == RRFinished)
                  return;
            };
            if (parseResult == RRHasToken) {
               do {
                  parseResult = sState.parse(aArguments);
               } while (parseResult == RRContinue || parseResult == RRHasToken);
               if (parseResult == RRNeedChars && aArguments.lLexer.doesNeedClear()) {
                  aArguments.lLexer.clearToken();
                  aArguments.apntNumberToken.release();
               }
            };
         };
         aArguments.clearBuffer();
      }
};

/* Implementation of the class DomainNode::OperatorStack */

class DomainNode::Operator : public COL::List::Node, public ExtendedParameters {
  private:
   /* operator precedence:
      ->[1] conditional expression ? :
      ->[...] assignment expression =, +=, ...
      ->[2] 'vv',
      ->[3] '^^',
      ->[4] '||',
      ->[5] '&&',
      ->[6] '|'
      ->[7] '^'
      ->[8] '&'
      ->[9] '==' | '!='
      ->[10] '<' | '>' | "<=" | '>='
      ->[11] '<<' | '>>'
      ->[12] '+' | '-'
      ->[13] '*' | '/' | '%'
      ->[14] casts
      ->[15] unary '+' | '-' | '~' | '!'
      ->[16] postfix call '()'
      ->[17] constant | identifier | '('element')' | '[min, max]'
   */

   enum Precedence
      {  PUndefined, PMerge, PUnary, PPostFix
      };
   enum Type
      {  TUndefined, TUnaryPlus, TUnaryMinus, TMerge, TMergeSymbolic, TInterval
      };
   enum AdditionalTypeInformation
      {  ATIUndefined, ATIUnsigned, ATISigned, ATIFloat };

  private:
   DomainValue dFirst;
   DomainValue dSecond;
   int uSize;
   int uSizeExponent;
   int uSizeMantissa;
   // unsigned uStartLine;
   // unsigned uStartColumn;
   friend class OperatorStack;

  protected:
   DefineExtendedParameters(15, ExtendedParameters)
   DefineSubExtendedParameters(Precedence, 2, INHERITED)
   DefineSubExtendedParameters(Type, 3, Precedence)
   DefineSubExtendedParameters(AdditionalType, 2, Type)
   DefineSubExtendedParameters(Symbolic, 1, AdditionalType)
   DefineSubExtendedParameters(LeftSubExpressions, 2, Symbolic)
   DefineSubExtendedParameters(RightAssociative, 1, LeftSubExpressions)
   DefineSubExtendedParameters(FirstArgumentType, 2, RightAssociative)
   DefineSubExtendedParameters(SecondArgumentType, 2, FirstArgumentType)

   friend class DomainNode;

  public:
   Operator(struct _DomainElementFunctions* functions)
      :  dFirst(functions), dSecond(functions), uSize(0), uSizeExponent(0), uSizeMantissa(0) {}
   Operator(const Operator& source) = default;
   Operator& operator=(const Operator& source) = default;
   DefineCopy(Operator)
   DDefineAssign(Operator)

   bool hasFirstArgument() const { return dFirst.isValid(); }
   bool hasSecondArgument() const { return dSecond.isValid(); }
   DomainValue extractLastArgument()
      {  incLeftSubExpressions();
         return dSecond.isValid() ? std::move(dSecond) : std::move(dFirst);
      }
   DomainValue extractFirstArgument()
      {  incLeftSubExpressions();
         return std::move(dFirst);
      }
   DomainValue extractSecondArgument()
      {  incLeftSubExpressions();
         return std::move(dSecond);
      }
   const DomainValue& getFirstArgument() const { return dFirst; }
   const DomainValue& getSecondArgument() const { return dSecond; }
   const DomainValue& getLastArgument() const { return dSecond.isValid() ? dSecond : dFirst; }
   Precedence getPrecedence() const { return (Precedence) queryPrecedenceField(); }

   Operator& setType(Type type, bool isPrefix=false);
   Operator& setUnsignedTag() { AssumeCondition(!hasAdditionalTypeField()) mergeAdditionalTypeField(ATIUnsigned); return *this; }
   Operator& setSignedTag() { AssumeCondition(!hasAdditionalTypeField()) mergeAdditionalTypeField(ATISigned); return *this; }
   Operator& setFloatTag() { AssumeCondition(!hasAdditionalTypeField()) mergeAdditionalTypeField(ATIFloat); return *this; }
   Operator& setSymbolic() { mergeSymbolicField(1); return *this; }
   bool hasUnsignedTag() const { return queryAdditionalTypeField() == ATIUnsigned; }
   bool hasSignedTag() const { return queryAdditionalTypeField() == ATISigned; }
   bool hasFloatTag() const { return queryAdditionalTypeField() == ATIFloat; }
   bool hasTag() const { return hasAdditionalTypeField(); }
   bool isSymbolic() const { return hasSymbolicField(); }
   Type getType() const { return (Type) queryTypeField(); }
   int getSize() const { AssumeCondition(uSize > 0) return uSize; }
   bool hasSize() const { return uSize > 0; }
   int getSizeMantissa() const { AssumeCondition(uSizeMantissa > 0) return uSizeMantissa; }
   bool hasSizeMantissa() const { return uSizeMantissa > 0; }
   void setSizeMantissa(int sizeMantissa) { uSizeMantissa = sizeMantissa; }
   int getSizeExponent() const { AssumeCondition(uSizeExponent > 0) return uSizeExponent; }
   bool hasSizeExponent() const { return uSizeExponent > 0; }
   void setSizeExponent(int sizeExponent) { uSizeExponent = sizeExponent; }
   void setBitSize(int size) { AssumeCondition(uSize == 0) uSize = size; }
   void setFloatSize(int sizeExponent, int sizeMantissa)
      {  AssumeCondition(uSizeExponent == 0 && uSizeMantissa == 0)
         uSizeExponent = sizeExponent; uSizeMantissa = sizeMantissa;
      }
   STG::SubString queryOperatorName() const;
   void setSize(int size) { uSize = size; }
   void setExtension(int extension) { uSize = extension; }
   int getExtension() const { return uSize; }
   
   Operator& setLeftSubExpressions(int leftSubExpressions)
      {  AssumeCondition(leftSubExpressions < 4 && !hasLeftSubExpressionsField());
         mergeLeftSubExpressionsField(leftSubExpressions);
         return *this;
      }
   void incLeftSubExpressions()
      {  int leftSubExpressions = queryLeftSubExpressionsField();
         AssumeCondition(leftSubExpressions < 3);
         setLeftSubExpressionsField(leftSubExpressions+1);
      }
   void decLeftSubExpressions()
      {  int leftSubExpressions = queryLeftSubExpressionsField();
         AssumeCondition(leftSubExpressions > 0);
         setLeftSubExpressionsField(leftSubExpressions-1);
      }
   bool isFinished() const { return !hasLeftSubExpressionsField(); }
   int getLeftSubExpressions() const { return queryLeftSubExpressionsField(); }
   void advance()
      {  unsigned leftSubExpressions = queryLeftSubExpressionsField();
         AssumeCondition(leftSubExpressions > 0)
         setLeftSubExpressionsField(leftSubExpressions-1);
      }

   bool isLeftAssociative() { return !hasRightAssociativeField(); }
   bool isRightAssociative() { return hasRightAssociativeField(); }
   bool isValid() const override { return hasTypeField(); }
   Operator& absorbFirstArgument(DomainValue&& domain) { dFirst = std::move(domain); return *this; }
   Operator& absorbSecondArgument(DomainValue&& domain) { dSecond = std::move(domain); return *this; }
   Operator& absorbLastArgument(DomainValue&& domain)
      {  (dFirst.isValid() ? dFirst : dSecond) = std::move(domain);
         decLeftSubExpressions();
         return *this;
      } 
   int queryArgumentsNumber() const
      {  int result = 0;
         Type type = (Type) queryTypeField();
         if (type == TMerge || type == TMergeSymbolic)
            result = 2;
         else if (type >= TUnaryPlus && type <= TUnaryMinus)
            result = 1;
         return result;
      }
};

STG::SubString
DomainNode::Operator::queryOperatorName() const {
   switch (queryTypeField()) {
      case TUnaryPlus: return STG::SString("+");
      case TUnaryMinus: return STG::SString("-");
      case TMerge: return STG::SString("vv");
      default:
         break;
   };
   return STG::SString();
}

DomainNode::Operator&
DomainNode::Operator::setType(Type type, bool isPrefix) {
   AssumeCondition(!hasTypeField() && !hasPrecedenceField() && !hasRightAssociativeField())
   mergeTypeField(type);
   bool isRightAssociative = false;
   Precedence precedence = PUndefined;
   switch (type) {
      case TUnaryPlus:
      case TUnaryMinus:
         isRightAssociative = true;
         precedence = PUnary;
         break;
      case TMerge:
         precedence = PMerge;
         break;
      case TInterval:
         precedence = PPostFix;
         break;
      default:
         break;
   };
   AssumeCondition(precedence);
   mergePrecedenceField(precedence);
   mergeRightAssociativeField(isRightAssociative);
   return *this;
}

class DomainNode::OperatorStack : public COL::TCopyCollection<COL::TList<Operator> > {
  private:
   static DomainValue apply(Operator& operation, Parser<char>::Arguments& context);
   static DomainValue applyInterval(Operator& operation, Parser<char>::Arguments& context);
   static DomainValue applyCallFunction(Operator& operation, Parser<char>::Arguments& context);
   friend class DomainNode;

   PNT::AutoPointer<DomainNode> apceSubElement;

  public:
   OperatorStack() {}
   OperatorStack(const OperatorStack& source) = default;
   OperatorStack(OperatorStack&& source)
      {  COL::TList<Operator>::swap(source);
         apceSubElement.swap(source.apceSubElement);
      }

   void pushLastArgument(DomainValue&& domain)
      {  Operator* lastOperator;
         if (isEmpty())
            insertNewAtEnd(lastOperator = new Operator(&domain.functionTable()));
         else
            lastOperator = &getSLast();
         lastOperator->absorbLastArgument(std::move(domain));
      }
   const DomainValue& getLastArgument() const
      {  return getLast().getLastArgument(); }

   Operator& pushOperator(Operator::Type typeOperator, int leftSubExpressions,
         Parser<char>::Arguments& context, bool& hasFailed, struct _DomainElementFunctions* functions);
   Operator& pushPrefixUnaryOperator(Operator::Type typeOperator, struct _DomainElementFunctions* functions)
      {  Operator* result;
         insertNewAtEnd(result = new Operator(functions));
         result->setType(typeOperator, true).setLeftSubExpressions(1);
         return *result;
      }
   Operator& pushPrefixBinaryOperator(Operator::Type typeOperator, struct _DomainElementFunctions* functions)
      {  Operator* result;
         insertNewAtEnd(result = new Operator(functions));
         result->setType(typeOperator, true).setLeftSubExpressions(2);
         return *result;
      }
   Operator& pushBinaryOperator(Operator::Type typeOperator, struct _DomainElementFunctions* functions)
      {  Operator* result;
         insertNewAtEnd(result = new Operator(functions));
         result->setType(typeOperator, true).setLeftSubExpressions(2);
         return *result;
      }
   Operator& pushBinaryOperator(Operator::Type typeOperator,
         Parser<char>::Arguments& context, bool& hasFailed, struct _DomainElementFunctions* functions)
      {  return pushOperator(typeOperator, 1, context, hasFailed, functions); }
   DomainValue clear(Parser<char>::Arguments& context, struct _DomainElementFunctions* functions);
   void absorbSubElement(DomainNode* subElement) { apceSubElement.absorbElement(subElement); }
   DomainNode* extractSubElement() { return apceSubElement.extractElement(); }
   DomainNode& getSubElement() const { return *apceSubElement; }
};

DomainValue
DomainNode::OperatorStack::applyInterval(Operator& operation, Parser<char>::Arguments& context) {
   AssumeCondition(operation.getType() == Operator::TInterval
         && operation.dFirst.isValid() && operation.dSecond.isValid())
   DomainValue secondArgument = operation.extractSecondArgument();
   DomainValue firstArgument = operation.extractFirstArgument();
   auto type = firstArgument.getType();
   if (secondArgument.getType() != type) {
      context.addErrorMessage(STG::SString(
         "interval has incompatible types for elements"));
      return firstArgument;
   };
   if (type == DTInteger) {
      DomainValue result((*firstArgument.functionTable().multibit_create_interval_and_absorb)(
            &firstArgument.svalue(), &secondArgument.svalue(), operation.hasSignedTag(),
            operation.isSymbolic()), &firstArgument.functionTable());
      firstArgument.svalue().content = secondArgument.svalue().content = nullptr;
      return result;
   }
   else if (type == DTFloating) {
      DomainValue result((*firstArgument.functionTable().multifloat_create_interval_and_absorb)(
            &firstArgument.svalue(), &secondArgument.svalue(),
            operation.isSymbolic()), &firstArgument.functionTable());
      firstArgument.svalue().content = secondArgument.svalue().content = nullptr;
      return result;
   };
   return firstArgument;
}


DomainValue
DomainNode::OperatorStack::apply(Operator& operation, Parser<char>::Arguments& context) {
   class GuardLatticeCreation {
     private:
      DomainEvaluationEnvironment oldEnv;
      DomainEvaluationEnvironment& newEnv;

     public:
      GuardLatticeCreation(DomainEvaluationEnvironment& env) : oldEnv(env), newEnv(env) {}
      ~GuardLatticeCreation() { newEnv = oldEnv; }
   };

   GuardLatticeCreation guard(context.evaluationEnvironment());
   if (operation.hasSymbolicField())
      context.evaluationEnvironment().defaultDomainType = DISFormal;
   Operator::Type type = operation.getType();
   int argumentsNumber = operation.queryArgumentsNumber();
   if (argumentsNumber == 1) {
      DomainValue argument = operation.extractLastArgument();
      switch (type) {
         case Operator::TUnaryPlus:
            return argument;
         case Operator::TUnaryMinus:
            context.clearEvaluationEnvironment();
            {  bool boolResult = true;
               auto argumentType = argument.getType();
               if (argumentType == DTInteger)
                  boolResult = (*argument.functionTable().multibit_unary_apply_assign)
                     (&argument.svalue(), DMBUOOppositeSigned, &context.evaluationEnvironment());
               else if (argumentType == DTFloating)
                  boolResult = (*argument.functionTable().multifloat_unary_apply_assign)
                     (&argument.svalue(), DMFUOOpposite, &context.evaluationEnvironment());
               else
                  context.addErrorMessage(STG::SString(
                     "unsupported type for the unary - operation"));
               AssumeCondition(boolResult)
            }
            return argument;
         default:
            context.addErrorMessage(STG::SString("unknown unary operation type"));
            return argument;
      };
   }
   else if (argumentsNumber == 2) {
      DomainValue secondArgument = operation.extractSecondArgument();
      DomainValue firstArgument = operation.extractFirstArgument();
      auto argumentType = firstArgument.getType();
      if (argumentType != secondArgument.getType()) {
         context.addErrorMessage(STG::SString("arguments have incompatible types"));
         return firstArgument;
      };

      if (argumentType == DTInteger) {
         DomainMultiBitBinaryOperation applyType = DMBBOUndefined;
         switch (type) {
            case Operator::TMerge:
               context.clearEvaluationEnvironment();
               (*firstArgument.functionTable().merge)(&firstArgument.svalue(),
                     secondArgument.svalue(), &context.evaluationEnvironment());
               return firstArgument;
            default:
               context.addErrorMessage(STG::SString("unknown binary operation type for multibit"));
               return firstArgument;
         };
         context.clearEvaluationEnvironment();
         (*firstArgument.functionTable().multibit_binary_apply_assign)(&firstArgument.svalue(),
               applyType, secondArgument.svalue(), &context.evaluationEnvironment());
         return firstArgument;
      }
      else if (argumentType == DTBit) {
         DomainBitBinaryOperation applyType = DBBOUndefined;
         switch (type) {
            case Operator::TMerge:
               context.clearEvaluationEnvironment();
               (*firstArgument.functionTable().merge)(&firstArgument.svalue(),
                     secondArgument.svalue(), &context.evaluationEnvironment());
               return firstArgument;
            default:
               context.addErrorMessage(STG::SString("unknown binary operation type for bit"));
               return firstArgument;
         };
         context.clearEvaluationEnvironment();
         (*firstArgument.functionTable().bit_binary_apply_assign)(&firstArgument.svalue(),
               applyType, secondArgument.svalue(), &context.evaluationEnvironment());
         return firstArgument;
      }
      else {
         AssumeCondition(argumentType == DTFloating)
         DomainMultiFloatBinaryOperation applyType = DMFBOUndefined;
         switch (type) {
            case Operator::TMerge:
               context.clearEvaluationEnvironment();
               (*firstArgument.functionTable().merge)(&firstArgument.svalue(),
                     secondArgument.svalue(), &context.evaluationEnvironment());
               return firstArgument;
            default:
               context.addErrorMessage(STG::SString("unsupported binary operation for floating point type"));
               return firstArgument;
         };
         context.clearEvaluationEnvironment();
         (*firstArgument.functionTable().multifloat_binary_apply_assign)(&firstArgument.svalue(),
               applyType, secondArgument.svalue(), &context.evaluationEnvironment());
         return firstArgument;
      }
   }
   else
      {  AssumeUncalled }
   return DomainValue(nullptr);
}

DomainNode::Operator&
DomainNode::OperatorStack::pushOperator(Operator::Type typeOperator,
      int leftSubExpressions, Parser<char>::Arguments& context, bool& hasFailed,
      struct _DomainElementFunctions* functions) {
   Operator* result = &getSLast();
   if (!result->isValid())
      result->setType(typeOperator).setLeftSubExpressions(leftSubExpressions);
   else {
      PNT::AutoPointer<Operator> expressionOperator(new Operator(functions), PNT::Pointer::Init());
      expressionOperator->setType(typeOperator)
         .setLeftSubExpressions(leftSubExpressions);
      Operator* lastOperator = nullptr;
      bool isLeftAssociative = expressionOperator->isLeftAssociative();
      DomainValue resultElement(functions);
      while (!isEmpty() && (isLeftAssociative
            ? ((lastOperator = &getSLast())->getPrecedence()
               >= expressionOperator->getPrecedence())
            : ((lastOperator = &getSLast())->getPrecedence()
               > expressionOperator->getPrecedence()))) {
         if (resultElement.isValid())
            lastOperator->absorbLastArgument(std::move(resultElement));
         if (!lastOperator->isFinished())
            break;
         resultElement = apply(*lastOperator, context); 
         if (!resultElement.isValid()) {
            hasFailed = true;
            return *lastOperator;
         };
         lastOperator = nullptr;
         freeLast();
      };
      if (!resultElement.isValid()) {
         AssumeCondition(lastOperator != nullptr);
         resultElement = lastOperator->extractLastArgument();
      };
      expressionOperator->absorbFirstArgument(std::move(resultElement));
      insertNewAtEnd(result = expressionOperator.extractElement());
   };
   return *result;
}


DomainValue
DomainNode::OperatorStack::clear(Parser<char>::Arguments& context,
      struct _DomainElementFunctions* functions) {
   DomainValue result(functions);
   if (count() == 1) {
      Operator* operation = &getSLast();
      if (!operation->isValid()) {
         result = operation->extractLastArgument();
         freeAll();
         return result;
      };
   };

   while (!isEmpty()) {
      PNT::AutoPointer<Operator> operation(&extractLast(), PNT::Pointer::Init());
      if (result.isValid())
         operation->absorbLastArgument(std::move(result));

      if (!operation->isFinished()) {
         context.addErrorMessage(STG::SString("unexpected end of expression"));
         return DomainValue(functions);
      };
      result = apply(*operation, context);
      if (!result.isValid())
         return result;
   };
   return result;
}

/* Implementation of the class DomainNode */

template <typename T> DomainValue
DomainNode::convertTokenToValue(const NumberToken& token,
      typename Parser<T>::Arguments& arguments, bool& doesRetry) {
   DomainValue element(&deValue.functionTable());
   doesRetry = false;
   if (token.isIntegerOrBit()) {
      Numerics::BigInteger val;
      STG::DIOObject::ISSubString in(token.getContent());
      Numerics::BigInteger::FormatParameters format;
      format.setText();
      if (token.hasSpecialCoding()) {
         if (token.hasHexaDecimalCoding())
            format.setText().setHexaDecimal();
         else if (token.hasOctalCoding()) {
            if (!arguments.addErrorMessage(STG::SString(
                  "unsupported octal coding for number in expression")))
               doesRetry = true;
            return DomainValue(&deValue.functionTable());
            // format.setText().setOctal();
         }
         else if (token.hasBitCoding()) {
            if (!arguments.addErrorMessage(STG::SString(
                  "unsupported bit coding for number in expression")))
               doesRetry = true;
            return DomainValue(&deValue.functionTable());
            // format.setText().setBitCoding();
         };
      };
      val.read(in, format);
      if (token.hasBitExtension())
         element = DomainValue((*deValue.functionTable().bit_create_constant)
               (!val.isZero()), &deValue.functionTable());
      else {
         val.adjustBitSize(token.getSize());
         element = DomainValue((*deValue.functionTable().multibit_create_constant)
               (DomainIntegerConstant{ token.getSize(), false, val[0]}), &deValue.functionTable());
      };
   }
   else {
      AssumeCondition(token.isFloatingPoint())
      int sizeExponent = token.getFirstSize();
      int sizeMantissa = token.getSecondSize();
      if (sizeExponent == 0 || sizeMantissa == 0) {
         if (!arguments.addErrorMessage(STG::SString(
               "exponent and mantissa should have a positive size in floating point number")))
            doesRetry = true;
         return DomainValue(&deValue.functionTable());
      };
      Numerics::DDouble::Access::ReadParameters format;
      format.setText();
      if (token.hasSpecialCoding()) {
         if (!arguments.addErrorMessage(STG::SString(
               "unsupported special coding for floating number in expression")))
            doesRetry = true;
         return DomainValue(&deValue.functionTable());
         // if (token.hasHexaDecimalCoding())
         //    format.setHexaDecimal();
         // else if (token.hasOctalCoding())
         //    format.setOctal();
         // else if (token.hasBitCoding())
         //    format.setBit();
      };
      STG::DIOObject::ISSubString in(token.getContent());
      Numerics::TDoubleElement<Numerics::TFloatingBase<Numerics::DoubleTraits> > val;
      val.read(in, format);
      element = DomainValue((*deValue.functionTable().multifloat_create_constant)
            (DomainFloatingPointConstant{ sizeExponent+sizeMantissa+1, val.implementation() }),
            &deValue.functionTable());
   }
   return element;
}

template <typename T> DomainValue
DomainNode::convertTokenToValue(const IdentifierTokenResult& token,
      typename Parser<T>::Arguments& arguments, bool& doesRetry) {
   DomainValue element(&deValue.functionTable());
   doesRetry = false;
   switch (token.getKeyword().getType()) {
      case KeywordToken::TTop:
      case KeywordToken::TTopSymbolic:
         if (token.choice <= IdentifierTokenResult::CKeyword) {
            if (!arguments.addErrorMessage(STG::SString(
                  "the top keyword should have a size")))
               doesRetry = true;
            return DomainValue(&deValue.functionTable());
         };
         if (token.choice == IdentifierTokenResult::CSizedKeyword)
            return DomainValue((*deValue.functionTable().multibit_create_top)(
                  token.sizedKeyword.getSize(),
                  token.getKeyword().getType() == KeywordToken::TTopSymbolic /* isSymbolic */),
                  &deValue.functionTable());
         else
            return DomainValue((*deValue.functionTable().multifloat_create_top)(
                  token.floatSizedKeyword.getSizeExponent()+token.floatSizedKeyword.getSizeMantissa()+1,
                  token.getKeyword().getType() == KeywordToken::TTopSymbolic /* isSymbolic */),
                  &deValue.functionTable());
      case KeywordToken::TTopBit:
      case KeywordToken::TTopBitSymbolic:
         return DomainValue((*deValue.functionTable().bit_create_top)
               (token.getKeyword().getType() == KeywordToken::TTopBitSymbolic /* isSymbolic */),
               &deValue.functionTable());
      default:
         break;
   };
   if (!arguments.addErrorMessage(STG::SString(
         "keyword encountered where an expression was expected")))
      doesRetry = true;
   return DomainValue(&deValue.functionTable());
}

void
DomainNode::convertTokenToQualification(const IdentifierTokenResult& token, Operator& lastOperator) {
   switch (token.keyword.getType()) {
      case KeywordToken::TUnsigned:
      case KeywordToken::TSigned:
         if (token.keyword.getType() == KeywordToken::TUnsigned)
            lastOperator.setUnsignedTag();
         else
            lastOperator.setSignedTag();
         break;
      case KeywordToken::TSymbolic:
         lastOperator.setSymbolic();
         break;
      case KeywordToken::TSignedSymbolic:
      case KeywordToken::TUnsignedSymbolic:
         lastOperator.setSymbolic();
         if (token.keyword.getType() == KeywordToken::TUnsignedSymbolic)
            lastOperator.setUnsignedTag();
         else
            lastOperator.setSignedTag();
         break;
      default:
         break;
   };
}

template <typename T> void
DomainNode::convertTokenToError(const IdentifierTokenResult& token,
      typename Parser<T>::Arguments& arguments, bool& doesRetry) {
   doesRetry = false;
   if (!arguments.addErrorMessage(STG::SString(
         "keyword encountered where an expression was expected")))
      doesRetry = true;
}

template <typename T> bool
DomainNode::readParenToken(typename Parser<T>::State& state,
      typename Parser<T>::Arguments& arguments,
      STG::Lexer::Base::ReadResult& result) {
   result = RRContinue;
   PNT::AutoPointer<DomainNode> subElement;
   bool hasFailed = false;
   enum Delimiters
      {  DBegin, DBeginStack,
         DParen, DParenStack, DEndParen, DEndParenStack,
         DInterval, DIntervalStack, DEndBracket, DEndBracketStack,
            DAfterInterval, DAfterIntervalStack,
         DSet, DSetStack, DFirstSetElement, DFirstSetElementStack, DSetElement, DSetElementStack,
            DEndSet, DEndSetStack, DAfterSet, DAfterSetStack,
         DAfterPrimary, DAfterPrimaryStack
      };

   if (state.point() != DBegin && state.point() != DBeginStack) {
      #define DefineGoto(Target) case D##Target: case D##Target##Stack: goto L##Target;
      switch (state.point()) {
         DefineGoto(Begin)
         DefineGoto(Paren)
         DefineGoto(EndParen)
      };
      #undef DefineGoto
   };

LBegin:
   {  DomainNode* subElement;
      if (state.point() == DBegin) {
         state.absorbUnionResult(DomainNode(&deValue.functionTable()));
         subElement = &state.getUnionResult((DomainNode*) nullptr);
      }
      else
         state.getUnionResult((OperatorStack*) nullptr).absorbSubElement(subElement
               = new DomainNode(&deValue.functionTable()));
      state.point() += (DParen-DBegin);
      arguments.shiftState(state, *subElement, &DomainNode::readToken<T>,
            (typename Parser<T>::State::template UnionResult<DomainNode, OperatorStack>*) nullptr);
   };
   if (!arguments.setToNextToken(result)) return false;
   if (!arguments.parseTokens(state, result)) return false;
LParen: // DParen, DParenStack
   if (state.point() == DParen) {
      subElement.absorbElement(new DomainNode(
            std::move(state.getUnionResult((DomainNode*) nullptr))));
      state.freeUnionResult((DomainNode*) nullptr);
   }
   else
      subElement.absorbElement(state.getUnionResult((OperatorStack*) nullptr)
            .extractSubElement());
   hasFailed = true;
   if (subElement->deValue.isValid()) {
      if (state.point() == DParen)
         deValue = std::move(subElement->deValue);
      else
         state.getUnionResult((OperatorStack*) nullptr).pushLastArgument(
            std::move(subElement->deValue));
      subElement.release();

      AbstractToken token = arguments.lexer().getToken();
      if (token.getType() == AbstractToken::TOperatorPunctuator
            && ((const OperatorPunctuatorToken&) token).getType()
               == OperatorPunctuatorToken::TCloseParen) {
         hasFailed = false;
         state.point() += DBegin - DParen;
      }
   }
   else {
      AbstractToken token = arguments.lexer().getToken();
      if (token.getType() == AbstractToken::TOperatorPunctuator
            && ((const OperatorPunctuatorToken&) token).getType()
               == OperatorPunctuatorToken::TCloseParen) {
         arguments.reduceState(state);
         return false;
      }
   }
   if (hasFailed) {
      if (!arguments.addErrorMessage(STG::SString(
            "')' was expected during the parsing of a subexpression"))) return false;
      state.point() = DEndParen;
      if (!arguments.setToNextToken(result)) return result;
LEndParen:
      AbstractToken token = arguments.lexer().getToken();
      while (token.getType() != AbstractToken::TOperatorPunctuator
            || ((const OperatorPunctuatorToken&) token).getType()
               != OperatorPunctuatorToken::TCloseParen) { // could count open paren
         if (!arguments.setToNextToken(result)) return result;
         token = arguments.lexer().getToken();
      }
      arguments.reduceState(state);
      return false;
   };
   return true;
}

template <typename T> bool
DomainNode::readIntervalToken(typename Parser<T>::State& state,
      typename Parser<T>::Arguments& arguments, STG::Lexer::Base::ReadResult& result,
      bool& hasReadToken) {
   result = RRContinue;
   PNT::AutoPointer<DomainNode> subElement;
   bool hasFailed = false;
   OperatorStack* operatorStack = nullptr;
   enum Delimiters
      {  DBegin, DBeginStack,
         DParen, DParenStack, DEndParen, DEndParenStack,
         DInterval, DIntervalStack, DEndBracket, DEndBracketStack,
            DAfterInterval, DAfterIntervalStack,
         DSet, DSetStack, DFirstSetElement, DFirstSetElementStack, DSetElement, DSetElementStack,
            DEndSet, DEndSetStack, DAfterSet, DAfterSetStack,
         DAfterPrimary, DAfterPrimaryStack
      };

   if (state.point() != DBegin && state.point() != DBeginStack) {
      #define DefineGoto(Target) case D##Target: case D##Target##Stack: goto L##Target;
      switch (state.point()) {
         DefineGoto(Begin)
         DefineGoto(Interval)
         DefineGoto(EndBracket)
         DefineGoto(AfterInterval)
      };
      #undef DefineGoto
   };

LBegin:
   if (state.point() == DBegin)
      state.absorbUnionResult(OperatorStack());
   {  OperatorStack* operatorStack = &state.getUnionResult((OperatorStack*) nullptr);
      DomainNode* subElement = new DomainNode(&deValue.functionTable());
      operatorStack->absorbSubElement(subElement);
      operatorStack->pushPrefixBinaryOperator(Operator::TInterval, &deValue.functionTable());
      state.point() += (DInterval-DBegin);
      arguments.shiftState(state, *subElement, &DomainNode::readToken<T>,
            (typename Parser<T>::State::template UnionResult<DomainNode, OperatorStack>*) nullptr);
   }
   if (!arguments.setToNextToken(result)) return false;
   if (!arguments.parseTokens(state, result)) return false;
LInterval:
   {  operatorStack = &state.getUnionResult((OperatorStack*) nullptr);
      subElement.absorbElement(operatorStack->extractSubElement());
      Operator* interval = &operatorStack->getSLast();
      AssumeCondition(interval->getType() == Operator::TInterval)
      AbstractToken token = arguments.lexer().getToken();
      hasFailed = true;
      if (subElement->deValue.isValid()) {
         operatorStack->pushLastArgument(std::move(subElement->deValue));
         if (interval->getLeftSubExpressions() == 1) {
            if (token.getType() == AbstractToken::TOperatorPunctuator
                  && ((const OperatorPunctuatorToken&) token).getType()
                     == OperatorPunctuatorToken::TComma) {
               operatorStack->absorbSubElement(subElement.extractElement());
               arguments.shiftState(state, operatorStack->getSubElement(), &DomainNode::readToken<T>,
                  (typename Parser<T>::State::template UnionResult<DomainNode, OperatorStack>*) nullptr);
               if (!arguments.setToNextToken(result)) return false;
               if (!arguments.parseTokens(state, result)) return false;
               goto LInterval;
            };
         }
         else {
            AssumeCondition(interval->getLeftSubExpressions() == 0)
            if (token.getType() == AbstractToken::TOperatorPunctuator
                  && ((const OperatorPunctuatorToken&) token).getType()
                     == OperatorPunctuatorToken::TCloseBracket)
               hasFailed = false;
         };
      };
   };
   if (hasFailed) {
      operatorStack->freeLast();
      operatorStack = nullptr;
      {  AbstractToken token = arguments.lexer().getToken();
         if (token.getType() == AbstractToken::TOperatorPunctuator
               && ((const OperatorPunctuatorToken&) token).getType()
                  == OperatorPunctuatorToken::TCloseBracket) {
            arguments.reduceState(state);
            return false;
         }
      }
      if (!arguments.addErrorMessage(STG::SString(
         "',]' was expected during the parsing of an interval"))) return false;
      ++state.point();
      if (!arguments.setToNextToken(result)) return false;
LEndBracket:
      {  AbstractToken token = arguments.lexer().getToken();
         while (token.getType() != AbstractToken::TOperatorPunctuator
               || ((const OperatorPunctuatorToken&) token).getType()
                  != OperatorPunctuatorToken::TCloseBracket) { // could count open bracket
            if (!arguments.setToNextToken(result)) return false;
            AbstractToken token = arguments.lexer().getToken();
         }
      }
   }
   else
      operatorStack = nullptr;
   ++state.point();
   if (!arguments.setToNextToken(result)) return false;

LAfterInterval:
   {  OperatorStack* operatorStack = &state.getUnionResult((OperatorStack*) nullptr);
      Operator* interval = &operatorStack->getSLast();
      AssumeCondition(interval->getType() == Operator::TInterval)
      AssumeCondition(interval->getLeftSubExpressions() == 0)
      if ((result = arguments.setArgumentToIdentifier()) == RRNeedChars) return false;
      AbstractToken token = arguments.lexer().getToken();
      hasReadToken = false;
      if (arguments.getIdentifierResult().isQualifier()) {
         convertTokenToQualification(arguments.getIdentifierResult(), *interval);
         hasReadToken = true;
      };
      DomainValue resultElement = operatorStack->applyInterval(*interval, arguments);
      operatorStack->freeLast();
      interval = nullptr;
      if (operatorStack->isEmpty()) {
         state.freeUnionResult((OperatorStack*) nullptr);
         operatorStack = nullptr;
         deValue = std::move(resultElement);
         state.point() = DBegin;
      }
      else {
         state.point() = DBeginStack;
         operatorStack->pushLastArgument(std::move(resultElement));
      }
   };
   return true;
}

template <typename T> bool
DomainNode::readSetToken(typename Parser<T>::State& state,
      typename Parser<T>::Arguments& arguments, STG::Lexer::Base::ReadResult& result,
      bool& hasReadToken) {
   result = RRContinue;
   PNT::AutoPointer<DomainNode> subElement;
   bool isEndSet = false;
   bool hasFailed = false;
   enum Delimiters
      {  DBegin, DBeginStack,
         DParen, DParenStack, DEndParen, DEndParenStack,
         DInterval, DIntervalStack, DEndBracket, DEndBracketStack,
            DAfterInterval, DAfterIntervalStack,
         DSet, DSetStack, DFirstSetElement, DFirstSetElementStack, DSetElement, DSetElementStack,
            DEndSet, DEndSetStack, DAfterSet, DAfterSetStack,
         DAfterPrimary, DAfterPrimaryStack
      };

   if (state.point() != DBegin && state.point() != DBeginStack) {
      #define DefineGoto(Target) case D##Target: case D##Target##Stack: goto L##Target;
      switch (state.point()) {
         DefineGoto(Begin)
         DefineGoto(Set)
         DefineGoto(FirstSetElement)
         DefineGoto(EndSet)
         DefineGoto(SetElement)
         DefineGoto(AfterSet)
      };
      #undef DefineGoto
   };

LBegin:
   state.point() += (DSet - DBegin);
   if (!arguments.setToNextToken(result)) return false;

LSet:
   {  AbstractToken token = arguments.lexer().getToken();
      if (token.getType() == AbstractToken::TOperatorPunctuator) {
         auto type = ((const OperatorPunctuatorToken&) token).getType();
         if (type == OperatorPunctuatorToken::TCloseBrace) {
            if (!arguments.addErrorMessage(STG::SString(
               "empty set is not allowed"))) return false;
            state.point() -= DSet - DBegin;
            return true;
         };
      }
      DomainNode* subElement;
      if (state.point() == DBegin) {
         state.absorbUnionResult(DomainNode(&deValue.functionTable()));
         subElement = &state.getUnionResult((DomainNode*) nullptr);
      }
      else
         state.getUnionResult((OperatorStack*) nullptr)
            .absorbSubElement(subElement = new DomainNode(&deValue.functionTable()));
      ++state.point();
      arguments.shiftState(state, *subElement, &DomainNode::readToken<T>,
            (typename Parser<T>::State::template UnionResult<DomainNode, OperatorStack>*) nullptr);
   };
   if (!arguments.parseTokens(state, result)) return false;
LFirstSetElement:
   if (state.point() == DFirstSetElement) {
      subElement.absorbElement(new DomainNode(std::move(state.getUnionResult((DomainNode*) nullptr))));
      state.freeUnionResult((DomainNode*) nullptr);
   }
   else
      subElement.absorbElement(state.getUnionResult((OperatorStack*) nullptr).extractSubElement());
   {  AbstractToken token = arguments.lexer().getToken();
      hasFailed = true;
      if (subElement->deValue.isValid()) {
         DomainValue elementAsDisjunction((*deValue.functionTable().create_disjunction_and_absorb)
               (&subElement->deValue.svalue()), &deValue.functionTable());
         if (state.point() == DFirstSetElement)
            deValue = std::move(elementAsDisjunction);
         else
            state.getUnionResult((OperatorStack*) nullptr).pushLastArgument(
               std::move(elementAsDisjunction));

         if (token.getType() == AbstractToken::TOperatorPunctuator) {
            auto type = ((const OperatorPunctuatorToken&) token).getType();
            if (type == OperatorPunctuatorToken::TCloseBrace) {
               isEndSet = true;
               hasFailed = false;
            }
            else if (type == OperatorPunctuatorToken::TComma) {
               hasFailed = false;
               DomainNode* element;
               if (state.point() == DFirstSetElement) {
                  state.absorbUnionResult(std::move(*subElement));
                  element = &state.getUnionResult((DomainNode*) nullptr);
               }
               else
                  state.getUnionResult((OperatorStack*) nullptr)
                     .absorbSubElement(element = subElement.extractElement());
               state.point() += DSetElement - DFirstSetElement;
               arguments.shiftState(state, *element, &DomainNode::readToken<T>,
                  (typename Parser<T>::State::template UnionResult<DomainNode, OperatorStack>*) nullptr);
               if (!arguments.setToNextToken(result)) return false;
               if (!arguments.parseTokens(state, result)) return false;
            }
         }
      };
   };
   if (hasFailed) {
      if (!arguments.addErrorMessage(STG::SString(
            "'}' was expected during the parsing of a set"))) return false;
      state.point() = DEndSet;
      if (!arguments.setToNextToken(result)) return result;
LEndSet:
      AbstractToken token = arguments.lexer().getToken();
      while (token.getType() != AbstractToken::TOperatorPunctuator
            || ((const OperatorPunctuatorToken&) token).getType()
               != OperatorPunctuatorToken::TCloseBrace) { // could count open paren
         if (!arguments.setToNextToken(result)) return result;
         token = arguments.lexer().getToken();
      }
      --state.point();
      isEndSet = true;
   };

   state.point() += DSetElement - DFirstSetElement;
   while (!isEndSet) {
LSetElement:
      {  DomainValue* disjunction;
         if (state.point() == DSetElement) {
            subElement.absorbElement(new DomainNode(std::move(state.getUnionResult((DomainNode*) nullptr))));
            state.freeUnionResult((DomainNode*) nullptr);
            disjunction = &deValue;
         }
         else {
            OperatorStack* operatorStack = &state.getUnionResult((OperatorStack*) nullptr);
            subElement.absorbElement(operatorStack->extractSubElement());
            disjunction = &const_cast<DomainValue&>(operatorStack->getLastArgument());
         }
         AbstractToken token = arguments.lexer().getToken();
         hasFailed = true;
         if (subElement->deValue.isValid()) {
            (deValue.functionTable().disjunction_absorb)(&disjunction->svalue(),
                  &subElement->deValue.svalue());
            if (token.getType() == AbstractToken::TOperatorPunctuator) {
               auto type = ((const OperatorPunctuatorToken&) token).getType();
               if (type == OperatorPunctuatorToken::TCloseBrace) {
                  isEndSet = true;
                  hasFailed = false;
               }
               if (type == OperatorPunctuatorToken::TComma) {
                  hasFailed = false;
                  DomainNode* element;
                  if (state.point() == DSetElement) {
                     state.absorbUnionResult(std::move(*subElement));
                     element = &state.getUnionResult((DomainNode*) nullptr);
                  }
                  else
                     state.getUnionResult((OperatorStack*) nullptr)
                        .absorbSubElement(element = subElement.extractElement());
                  arguments.shiftState(state, *element, &DomainNode::readToken<T>,
                        (typename Parser<T>::State::template UnionResult<DomainNode, OperatorStack>*) nullptr);
                  if (!arguments.setToNextToken(result)) return false;
                  if (!arguments.parseTokens(state, result)) return false;
               }
            }
         };
         if (hasFailed) {
            if (!arguments.addErrorMessage(STG::SString(
                  "'}' was expected during the parsing of a set"))) return false;
            state.point() = DEndSet;
            if (!arguments.setToNextToken(result)) return result;
            state.point() -= (DSetElement - DFirstSetElement);
            goto LEndSet;
         }
      };
   }
   ++state.point();
   if (!arguments.setToNextToken(result)) return false;

LAfterSet:
   {  AbstractToken token = arguments.lexer().getToken();
      if (token.getType() == AbstractToken::TKeyword
            && ((const KeywordToken&) token).getType() == KeywordToken::TSymbolic)
         hasReadToken = true;
   };
   return true;
}

template <typename T> STG::Lexer::Base::ReadResult
DomainNode::readToken(typename Parser<T>::State& state,
      typename Parser<T>::Arguments& arguments) {
   ReadResult result = RRContinue;
   bool doesRetry;
   bool doesContinue = true;
   PNT::AutoPointer<DomainNode> subElement;
   bool hasReadToken = false;
   AbstractToken token;

   enum Delimiters
      {  DBegin, DBeginStack,
         DParen, DParenStack, DEndParen, DEndParenStack,
         DInterval, DIntervalStack, DEndBracket, DEndBracketStack,
            DAfterInterval, DAfterIntervalStack,
         DSet, DSetStack, DFirstSetElement, DFirstSetElementStack, DSetElement, DSetElementStack,
            DEndSet, DEndSetStack, DAfterSet, DAfterSetStack,
         DAfterPrimary, DAfterPrimaryStack
      };

   if (state.point() != DBegin) {
      #define DefineGoto(Target) case D##Target: case D##Target##Stack: goto L##Target;
      switch (state.point()) {
         DefineGoto(Begin)
         DefineGoto(Paren)
         DefineGoto(EndParen)
         DefineGoto(Interval)
         DefineGoto(EndBracket)
         DefineGoto(AfterInterval)
         DefineGoto(Set)
         DefineGoto(FirstSetElement)
         DefineGoto(EndSet)
         DefineGoto(SetElement)
         DefineGoto(AfterSet)
         DefineGoto(AfterPrimary)
      };
      #undef DefineGoto
   }

LBegin: // DBegin, DBeginStack
   while (doesContinue) { // binary, ternary operation
      AssumeCondition(!deValue.isValid())
      {  token = arguments.lexer().getToken();
         if (token.getType() == AbstractToken::TNumber) {
            if (arguments.setArgumentToNumber() == RRNeedChars) return RRNeedChars;
            DomainValue element = convertTokenToValue<T>(arguments.getNumberToken(), arguments, doesRetry);
            if (!element.isValid()) { // stop parsing and try to do something with current token
               if (doesRetry) return RRContinue;
               arguments.reduceState(state);
               return RRHasToken;
            }
            if (state.point() == DBegin)
               deValue = std::move(element);
            else
               state.getUnionResult((OperatorStack*) nullptr).pushLastArgument(std::move(element));
            state.point() += DAfterPrimary - DBegin;
            if (!arguments.setToNextToken(result)) return result;
         }
         else if (token.getType() == AbstractToken::TIdentifier) {
            if (arguments.setArgumentToIdentifier() == RRNeedChars) return RRNeedChars;
            if (arguments.getIdentifierResult().isDomain()) {
               DomainValue element = convertTokenToValue<T>(arguments.getIdentifierResult(), arguments, doesRetry);
               if (!element.isValid()) { // stop parsing and try to do something with current token
                  if (doesRetry) return RRContinue;
                  arguments.reduceState(state);
                  return RRHasToken;
               }
               if (state.point() == DBegin)
                  deValue = std::move(element);
               else
                  state.getUnionResult((OperatorStack*) nullptr).pushLastArgument(std::move(element));
               state.point() += DAfterPrimary - DBegin;
               if (!arguments.setToNextToken(result)) return result;
            }
            else if (arguments.getIdentifierResult().isQualifier()) {
               if (state.point() == DBegin) {
                  if (!arguments.addErrorMessage(STG::SString(
                        "keyword '..._U/S' encountered where an expression was expected"))) return RRContinue;
                  arguments.reduceState(state);
                  return RRHasToken;
               }
               convertTokenToQualification(arguments.getIdentifierResult(),
                     state.getUnionResult((OperatorStack*) nullptr).getSLast());
               continue;
            }
            else {
               convertTokenToError<T>(arguments.getIdentifierResult(), arguments, doesRetry);
               if (doesRetry) return RRContinue;
               arguments.reduceState(state);
               return RRHasToken;
            };
         }
         else if (token.getType() == AbstractToken::TOperatorPunctuator) {
            switch (((const OperatorPunctuatorToken&) token).getType()) {
               case OperatorPunctuatorToken::TOpenParen:
LParen: LEndParen:
                  if (!readParenToken<T>(state, arguments, result)) return result;
                  break;
               case OperatorPunctuatorToken::TOpenBracket:
LInterval: LEndBracket: LAfterInterval:
                  if (!readIntervalToken<T>(state, arguments, result, hasReadToken)) return result;
                  break;
               case OperatorPunctuatorToken::TOpenBrace:
LSet: LFirstSetElement: LSetElement: LEndSet: LAfterSet:
                  if (!readSetToken<T>(state, arguments, result, hasReadToken)) return result;
                  break;
               case OperatorPunctuatorToken::TMinus:
                  {  if (state.point() == DBegin)
                        state.absorbUnionResult(OperatorStack());
                     OperatorStack* operatorStack = &state.getUnionResult((OperatorStack*) nullptr);
                     operatorStack->pushPrefixUnaryOperator(Operator::TUnaryMinus, &deValue.functionTable());
                  };
                  continue;
               case OperatorPunctuatorToken::TPlus:
                  {  if (state.point() == DBegin)
                        state.absorbUnionResult(OperatorStack());
                     OperatorStack* operatorStack = &state.getUnionResult((OperatorStack*) nullptr);
                     operatorStack->pushPrefixUnaryOperator(Operator::TUnaryPlus, &deValue.functionTable());
                  };
                  continue;
               default:
                  if (!arguments.addErrorMessage(STG::SString(
                        "operator/punctuator encountered where an expression was expected"))) return RRContinue;
                  arguments.reduceState(state);
                  return RRHasToken;
            };
         }
      }

      state.point() += DAfterPrimary - DBegin;
      if (!hasReadToken) {
         if (!arguments.setToNextToken(result)) return result;
      }

LAfterPrimary: // DAfterPrimary, DAfterPrimaryStack
      AssumeCondition(state.point() == DAfterPrimary ? deValue.isValid() : !deValue.isValid())
      doesContinue = false;
      {  token = arguments.lexer().getToken();
         if (token.getType() == AbstractToken::TKeyword) {
            auto type = ((const KeywordToken&) token).getType();
            if (type == KeywordToken::TMerge || type == KeywordToken::TMergeSymbolic) {
               if (state.point() == DAfterPrimary) {
                  state.absorbUnionResult(OperatorStack());
                  OperatorStack* operatorStack = &state.getUnionResult((OperatorStack*) nullptr);
                  operatorStack->pushBinaryOperator(Operator::TMerge, &deValue.functionTable())
                     .absorbFirstArgument(std::move(deValue))
                     .decLeftSubExpressions();
                  if (type == KeywordToken::TMergeSymbolic)
                     operatorStack->getSLast().setSymbolic();
               }
               else {
                  OperatorStack* operatorStack = &state.getUnionResult((OperatorStack*) nullptr);
                  bool hasFailed = false;
                  operatorStack->pushBinaryOperator(Operator::TMerge, arguments, hasFailed,
                        &deValue.functionTable());
                  if (type == KeywordToken::TMergeSymbolic)
                     operatorStack->getSLast().setSymbolic();
                  if (hasFailed) {
                     arguments.reduceState(state);
                     return RRHasToken;
                  }
               }
               doesContinue = true;
            }
         }
      };
      if (!doesContinue && !deValue.isValid()) {
         AssumeCondition(state.point() == DAfterPrimaryStack)
         OperatorStack operatorStack = std::move(state.getUnionResult((OperatorStack*) nullptr));
         state.freeUnionResult((OperatorStack*) nullptr);
         deValue = operatorStack.clear(arguments, &deValue.functionTable());
         if (!deValue.isValid() && arguments.lexer().doesStopAfterTooManyErrors()) {
            arguments.reduceState(state);
            return RRFinished;
         }
      };
      if (doesContinue)
         state.point() -= (DAfterPrimary - DBegin);
   };
   arguments.reduceState(state);
   return RRHasToken;
}

DomainNode::ReadResult
DomainNode::readJSon(STG::JSon::CommonParser::State& state, STG::JSon::CommonParser::Arguments& arguments) {
   typedef STG::JSon::CommonParser Parser;
   ReadResult result = RRContinue;
   bool isEqual;

   enum Delimiters
      {  DBegin, DAfterBegin, DReadContent, DEnd };

   #define DefineGoto(Target) case D##Target: goto L##Target;
   switch (state.point()) {
      DefineGoto(Begin)
      DefineGoto(AfterBegin)
      DefineGoto(ReadContent)
      DefineGoto(End)
   };
   #undef DefineGoto

LBegin:
   if (!arguments.isOpenObject()) {
      if (!arguments.addErrorMessage(STG::SString("expected '{'")))
         return result;
   }
   ++state.point();
   if (!arguments.setToNextToken(result)) return result;

LAfterBegin:
   while (!arguments.isCloseObject()) {
      while (arguments.isOpen()) {
         if (!Parser::skipNodeInLoop(state, arguments, result) || result == RRNeedChars) return result;
         if (!arguments.setToNextToken(result)) return result;
      };

      if (arguments.isAddKey()
            && ((result = arguments.isEqualKeyValue(isEqual, ssJSonContent)), isEqual)) {
         if (result == RRNeedChars) return result;
         ++state.point();
         if (!arguments.setToNextToken(result)) return result;

LReadContent:
         if (arguments.isSetString()) {
            if (arguments.setArgumentTextValue() == RRNeedChars) return RRNeedChars;
            DomainNode::Parser<char> parser(*this, arguments);
            parser.state().shift(*this, &DomainNode::readToken<char>,
                  (DomainNode::Parser<char>::State::UnionResult<DomainNode, OperatorStack>*) nullptr);
            parser.parse(arguments.valueAsText());
         };
      }
      state.point() = DAfterBegin;
      if (!arguments.setToNextToken(result)) return result;
   }
   state.point() = DEnd;
LEnd:
   state.reduce();
   return RRHasToken;
}

DomainNode::WriteResult
DomainNode::writeJSon(STG::JSon::CommonWriter::State& state, STG::JSon::CommonWriter::Arguments& arguments) const {
   WriteResult result = WRNeedEvent;
   enum Delimiters
      {  DBegin, DAfterBegin, DWriteContent, DEnd };

   #define DefineGoto(Target) case D##Target: goto L##Target;
   switch (state.point()) {
      DefineGoto(Begin)
      DefineGoto(AfterBegin)
      DefineGoto(WriteContent)
      DefineGoto(End)
   };
   #undef DefineGoto

LBegin:
   arguments.setOpenObject();
   ++state.point();
   if (!arguments.writeEvent(result)) return result;

LAfterBegin:
   arguments.setAddKey(ssJSonContent);
   ++state.point();
   if (!arguments.writeEvent(result)) return result;

LWriteContent:
   {  STG::DIOObject::OSSubString out;
      deValue.write(out, FormatParameters());
      arguments.setStringValue(out);
   }
   ++state.point();
   if (!arguments.writeEvent(result)) return result;

LEnd:
   arguments.setCloseObject();
   state.reduce();
   if (!arguments.writeEvent(result)) return result;
   return WRNeedEvent;
}

/* Implementation of the class OperationNode */

bool
OperationNode::setCodeFromText(const STG::SubString& text,
      STG::JSon::CommonParser::Arguments& arguments) {
   struct BoxedUnsigned : public EnhancedObject {
      unsigned val = 0;
      BoxedUnsigned() = default;
      BoxedUnsigned(unsigned value) : val(value) {}
      BoxedUnsigned(const BoxedUnsigned&) = default;
      BoxedUnsigned& operator=(const BoxedUnsigned&) = default;
      DefineCopy(BoxedUnsigned)
   };
   typedef COL::TCopyCollection<COL::TTernaryTree<STG::SubString, BoxedUnsigned> > CodeCollection;
   static CodeCollection ccCodes;
   if (!mpFirst.isValid()) {
      if (!arguments.addErrorMessage(STG::SString("operation requires at lease a \"first\" argument")))
         return false;
      return true;
   }

   if (ccCodes.isEmpty())
      ccCodes = CodeCollection(COL::TTernaryTree<STG::SubString, BoxedUnsigned>::InitialNewValues()
         << STG::SString("+")    << BoxedUnsigned((TPlus << FType)                   | (0 << FSymbolic))
         << STG::SString("+_s")  << BoxedUnsigned((TPlusSigned << FType)             | (0 << FSymbolic))
         << STG::SString("+_u")  << BoxedUnsigned((TPlusUnsigned << FType)           | (0 << FSymbolic))
         << STG::SString("+_us") << BoxedUnsigned((TPlusUnsignedWithSigned << FType) | (0 << FSymbolic))
         << STG::SString("-")    << BoxedUnsigned((TMinus << FType)                  | (0 << FSymbolic))
         << STG::SString("-_s")  << BoxedUnsigned((TMinusSigned << FType)            | (0 << FSymbolic))
         << STG::SString("-_u")  << BoxedUnsigned((TMinusUnsigned << FType)          | (0 << FSymbolic))
         << STG::SString("*")    << BoxedUnsigned((TTimes << FType)                  | (0 << FSymbolic))
         << STG::SString("*_s")  << BoxedUnsigned((TTimesSigned << FType)            | (0 << FSymbolic))
         << STG::SString("*_u")  << BoxedUnsigned((TTimesUnsigned << FType)          | (0 << FSymbolic))
         << STG::SString("/")    << BoxedUnsigned((TDivide << FType)                 | (0 << FSymbolic))
         << STG::SString("/_s")  << BoxedUnsigned((TDivideSigned << FType)           | (0 << FSymbolic))
         << STG::SString("/_u")  << BoxedUnsigned((TDivideUnsigned << FType)         | (0 << FSymbolic))
         << STG::SString("%")    << BoxedUnsigned((TModulo << FType)                 | (0 << FSymbolic))
         << STG::SString("%_s")  << BoxedUnsigned((TModuloSigned << FType)           | (0 << FSymbolic))
         << STG::SString("%_u")  << BoxedUnsigned((TModuloUnsigned << FType)         | (0 << FSymbolic))
         << STG::SString("<<")   << BoxedUnsigned((TLeftShift << FType)              | (0 << FSymbolic))
         << STG::SString(">>")   << BoxedUnsigned((TRightShift << FType)             | (0 << FSymbolic))
         << STG::SString(">>_s") << BoxedUnsigned((TRightShiftSigned << FType)       | (0 << FSymbolic))
         << STG::SString(">>_u") << BoxedUnsigned((TRightShiftUnsigned << FType)     | (0 << FSymbolic))
         << STG::SString("==")   << BoxedUnsigned((TEqual << FType)                  | (0 << FSymbolic))
         << STG::SString("!=")   << BoxedUnsigned((TDifferent << FType)              | (0 << FSymbolic))
         << STG::SString("<=")   << BoxedUnsigned((TLessOrEqual << FType)            | (0 << FSymbolic))
         << STG::SString("<=_s") << BoxedUnsigned((TLessOrEqualSigned << FType)      | (0 << FSymbolic))
         << STG::SString("<=_u") << BoxedUnsigned((TLessOrEqualUnsigned << FType)    | (0 << FSymbolic))
         << STG::SString(">=")   << BoxedUnsigned((TGreaterOrEqual << FType)         | (0 << FSymbolic))
         << STG::SString(">=_s") << BoxedUnsigned((TGreaterOrEqualSigned << FType)   | (0 << FSymbolic))
         << STG::SString(">=_u") << BoxedUnsigned((TGreaterOrEqualUnsigned << FType) | (0 << FSymbolic))
         << STG::SString("<")    << BoxedUnsigned((TLess << FType)                   | (0 << FSymbolic))
         << STG::SString("<_s")  << BoxedUnsigned((TLessSigned << FType)             | (0 << FSymbolic))
         << STG::SString("<_u")  << BoxedUnsigned((TLessUnsigned << FType)           | (0 << FSymbolic))
         << STG::SString(">")    << BoxedUnsigned((TGreater << FType)                | (0 << FSymbolic))
         << STG::SString(">_s")  << BoxedUnsigned((TGreaterSigned << FType)          | (0 << FSymbolic))
         << STG::SString(">_u")  << BoxedUnsigned((TGreaterUnsigned << FType)        | (0 << FSymbolic))
         << STG::SString("&&")   << BoxedUnsigned((TLogicalAnd << FType)             | (0 << FSymbolic))
         << STG::SString("||")   << BoxedUnsigned((TLogicalOr << FType)              | (0 << FSymbolic))
         << STG::SString("&")    << BoxedUnsigned((TBitAnd << FType)                 | (0 << FSymbolic))
         << STG::SString("|")    << BoxedUnsigned((TBitOr << FType)                  | (0 << FSymbolic))
         << STG::SString("^")    << BoxedUnsigned((TBitXOr << FType)                 | (0 << FSymbolic))
         << STG::SString("!")    << BoxedUnsigned((TNot << FType)                    | (0 << FSymbolic))
         << STG::SString("~")    << BoxedUnsigned((TComplement << FType)             | (0 << FSymbolic))
         << STG::SString("(bit)")          << BoxedUnsigned((1 << FExtended) | (TECastBit << FType)         | (0 << FSymbolic))
         << STG::SString("(integer)")      << BoxedUnsigned((1 << FExtended) | (TECastInteger << FType)     | (0 << FSymbolic))
         << STG::SString("(floating)")     << BoxedUnsigned((1 << FExtended) | (TECastFloating << FType)    | (0 << FSymbolic))
         << STG::SString("(floating_ptr)") << BoxedUnsigned((1 << FExtended) | (TECastFloatingPtr << FType) | (0 << FSymbolic))
         << STG::SString("extend_zero") << BoxedUnsigned((1 << FExtended) | (TECallExtendZero << FType)  | (0 << FSymbolic))
         << STG::SString("extend_sign") << BoxedUnsigned((1 << FExtended) | (TECallExtendSign << FType)  | (0 << FSymbolic))
         << STG::SString("concat")      << BoxedUnsigned((TCallConcat << FType)      | (0 << FSymbolic))
         << STG::SString("reduce")      << BoxedUnsigned((1 << FExtended) | (TECallReduce << FType)      | (0 << FSymbolic))
         << STG::SString("bitset")      << BoxedUnsigned((1 << FExtended) | (TECallBitSet << FType)      | (0 << FSymbolic))
         << STG::SString("shiftbit")    << BoxedUnsigned((1 << FExtended) | (TECallShiftBit << FType)    | (0 << FSymbolic))
         << STG::SString("inc")         << BoxedUnsigned((TCallInc << FType)         | (0 << FSymbolic))
         << STG::SString("inc_s")       << BoxedUnsigned((TCallIncSigned << FType)   | (0 << FSymbolic))
         << STG::SString("inc_u")       << BoxedUnsigned((TCallIncUnsigned << FType) | (0 << FSymbolic))
         << STG::SString("dec")         << BoxedUnsigned((TCallDec << FType)         | (0 << FSymbolic))
         << STG::SString("dec_s")       << BoxedUnsigned((TCallDecSigned << FType)   | (0 << FSymbolic))
         << STG::SString("dec_u")       << BoxedUnsigned((TCallDecUnsigned << FType) | (0 << FSymbolic))
         << STG::SString("min")         << BoxedUnsigned((TCallMin << FType)         | (0 << FSymbolic))
         << STG::SString("min_s")       << BoxedUnsigned((TCallMinSigned << FType)   | (0 << FSymbolic))
         << STG::SString("min_u")       << BoxedUnsigned((TCallMinUnsigned << FType) | (0 << FSymbolic))
         << STG::SString("max")         << BoxedUnsigned((TCallMax << FType)         | (0 << FSymbolic))
         << STG::SString("max_s")       << BoxedUnsigned((TCallMaxSigned << FType)   | (0 << FSymbolic))
         << STG::SString("max_u")       << BoxedUnsigned((TCallMaxUnsigned << FType) | (0 << FSymbolic))
         << STG::SString("vv")          << BoxedUnsigned((1 << FExtended) | (TEMerge << FType)     | (0 << FSymbolic))
         << STG::SString("vv_s")        << BoxedUnsigned((1 << FExtended) | (TEMerge << FType)     | (0 << FSymbolic))
         << STG::SString("vv_u")        << BoxedUnsigned((1 << FExtended) | (TEMerge << FType)     | (0 << FSymbolic))
         << STG::SString("^^")          << BoxedUnsigned((1 << FExtended) | (TEIntersect << FType) | (0 << FSymbolic))
         << STG::SString("^^_s")        << BoxedUnsigned((1 << FExtended) | (TEIntersect << FType) | (0 << FSymbolic))
         << STG::SString("^^_u")        << BoxedUnsigned((1 << FExtended) | (TEIntersect << FType) | (0 << FSymbolic)));

   COL::TTernaryTree<STG::SubString, BoxedUnsigned>::Cursor codeCusor(ccCodes);
   unsigned result = 0;
   if (ccCodes.locateKey(text, codeCusor, COL::VirtualCollection::RPExact)) {
      unsigned code = codeCusor.elementAt().val;
      auto type = mpFirst->getType();
      int arity = mpSecond.isValid() ? 2 : 1;
      if (!(code & 1)) {
         switch ((code & ~(~0U << FType) >> FExtended)) {
            case TPlus: result = (type == DTBit) ? (unsigned) DBBOPlus : ((type == DTInteger)
                           ? (unsigned) DMBBOPlusSigned : (unsigned) DMFBOPlus); break;
            case TPlusSigned: result = (unsigned) DMBBOPlusSigned; break;
            case TPlusUnsigned: result = (unsigned) DMBBOPlusUnsigned; break;
            case TPlusUnsignedWithSigned: result = (unsigned) DMBBOPlusUnsignedWithSigned; break;
            case TMinus: result = (type == DTBit)
                           ? ((arity == 1) ? (unsigned) DBUONegate : (unsigned) DBBOMinus) : ((type == DTInteger)
                           ? ((arity == 1) ? (unsigned) DMBUOOppositeSigned : (unsigned) DMBBOMinusSigned)
                           : ((arity == 1) ? (unsigned) DMFUOOpposite : (unsigned) DMFBOPlus)); break;
            case TMinusSigned: result = (unsigned) DMBBOMinusSigned; break;
            case TMinusUnsigned: result = (unsigned) DMBBOMinusUnsigned; break;
            case TTimes: result = (type == DTInteger) ? (unsigned) DMBBOTimesSigned : (unsigned) DMFBOTimes; break;
            case TTimesSigned: result = (unsigned) DMBBOTimesSigned; break;
            case TTimesUnsigned: result = (unsigned) DMBBOTimesUnsigned; break;
            case TDivide: result = (type == DTInteger) ? (unsigned) DMBBODivideSigned : (unsigned) DMFBODivide; break;
            case TDivideSigned: result = (unsigned) DMBBODivideSigned; break;
            case TDivideUnsigned: result = (unsigned) DMBBODivideUnsigned; break;
            case TModulo: result = (unsigned) DMBBOModuloSigned; break;
            case TModuloSigned: result = (unsigned) DMBBOModuloSigned; break;
            case TModuloUnsigned: result = (unsigned) DMBBOModuloUnsigned; break;
            case TLeftShift: result = (unsigned) DMBBOLeftShift; break;
            case TRightShift: result = (unsigned) DMBBOArithmeticRightShift; break;
            case TRightShiftSigned: result = (unsigned) DMBBOArithmeticRightShift; break;
            case TRightShiftUnsigned: result = (unsigned) DMBBOLogicalRightShift; break;
            case TEqual: result = (type == DTBit) ? (1U << 3) | (unsigned) DBCOCompareEqual : ((type == DTInteger)
                           ? (1U << 5) | (unsigned) DMBCOCompareEqual : (1U << 4) | (unsigned) DMFCOCompareEqual); break;
            case TDifferent: result = (type == DTBit) ? (1U << 4) | (unsigned) DBCOCompareDifferent : ((type == DTInteger)
                           ? (1U << 5) | (unsigned) DMBCOCompareDifferent : (2U << 3) | (unsigned) DMFCOCompareDifferent); break;
            case TLessOrEqual: result = (type == DTBit) ? (1U << 3) | (unsigned) DBCOCompareLessOrEqual : ((type == DTInteger)
                           ? (1U << 5) | (unsigned) DMBCOCompareLessOrEqualSigned : (1U << 4) | (unsigned) DMFCOCompareLessOrEqual); break;
            case TLessOrEqualSigned: result = (1U << 5) | (unsigned) DMBCOCompareLessOrEqualSigned; break;
            case TLessOrEqualUnsigned: result = (1U << 5) | (unsigned) DMBCOCompareLessOrEqualUnsigned; break;
            case TGreaterOrEqual: result = (type == DTBit) ? (1U << 3) | (unsigned) DBCOCompareGreaterOrEqual : ((type == DTInteger)
                           ? (1U << 5) | (unsigned) DMBCOCompareGreaterOrEqualSigned : (1U << 4) | (unsigned) DMFCOCompareGreaterOrEqual); break;
            case TGreaterOrEqualSigned: result = (1U << 5) | (unsigned) DMBCOCompareGreaterOrEqualSigned; break;
            case TGreaterOrEqualUnsigned: result = (1U << 5) | (unsigned) DMBCOCompareGreaterOrEqualUnsigned; break;
            case TLess: result = (type == DTBit) ? (1U << 3) | (unsigned) DBCOCompareLess : ((type == DTInteger)
                           ? (1U << 5) | (unsigned) DMBCOCompareLessSigned : (1U << 4) | (unsigned) DMFCOCompareLess); break;
            case TLessSigned: result = (1U << 5) | (unsigned) DMBCOCompareLessSigned; break;
            case TLessUnsigned: result = (1U << 5) | (unsigned) DMBCOCompareLessUnsigned; break;
            case TGreater: result = (type == DTBit) ? (1U << 3) | (unsigned) DBCOCompareGreater : ((type == DTInteger)
                           ? (1U << 5) | (unsigned) DMBCOCompareGreaterSigned : (1U << 4) | (unsigned) DMFCOCompareGreater); break;
            case TGreaterSigned: result = (1U << 5) | (unsigned) DMBCOCompareGreaterSigned; break;
            case TGreaterUnsigned: result = (1U << 5) | (unsigned) DMBCOCompareGreaterUnsigned; break;
            case TLogicalAnd: result = (unsigned) DBBOAnd; break;
            case TLogicalOr: result = (unsigned) DBBOOr; break;
            case TBitAnd: result = (type == DTBit) ? (unsigned) DBBOAnd : (unsigned) DMBBOBitAnd; break;
            case TBitOr: result = (type == DTBit) ? (unsigned) DBBOOr : (unsigned) DMBBOBitOr; break;
            case TBitXOr: result = (type == DTBit) ? (unsigned) DBBOExclusiveOr : (unsigned) DMBBOBitExclusiveOr; break;
            case TNot: result = (type == DTBit) ? (unsigned) DBUONegate : (unsigned) DMBUOBitNegate; break;
            case TComplement: result = (type == DTBit) ? (unsigned) DBUONegate : (unsigned) DMBUOBitNegate; break;
            case TCallInc: result = (type == DTBit) ? (unsigned) DBUONext : (unsigned) DMBUONextSigned; break;
            case TCallIncSigned: result = (type == DTBit) ? (unsigned) DBUONext : (unsigned) DMBUONextSigned; break;
            case TCallIncUnsigned: result = (type == DTBit) ? (unsigned) DBUONext : (unsigned) DMBUONextUnsigned; break;
            case TCallDec: result = (type == DTBit) ? (unsigned) DBUOPrev : (unsigned) DMBUOPrevSigned; break;
            case TCallDecSigned: result = (type == DTBit) ? (unsigned) DBUOPrev : (unsigned) DMBUOPrevSigned; break;
            case TCallDecUnsigned: result = (type == DTBit) ? (unsigned) DBUOPrev : (unsigned) DMBUOPrevUnsigned; break;
            case TCallMin: result = (type == DTBit) ? (unsigned) DBBOMin : ((type == DTInteger)
                           ? (unsigned) DMBBOMinSigned : (unsigned) DMFBOMin); break;
            case TCallMinSigned: result = (unsigned) DMBBOMinSigned; break;
            case TCallMinUnsigned: result = (unsigned) DMBBOMinUnsigned; break;
            case TCallMax: result = (type == DTBit) ? (unsigned) DBBOMax : ((type == DTInteger)
                           ? (unsigned) DMBBOMaxSigned : (unsigned) DMFBOMax); break;
            case TCallMaxSigned: result = (unsigned) DMBBOMaxSigned; break;
            case TCallMaxUnsigned: result = (unsigned) DMBBOMaxUnsigned; break;
            default:
               break;
         }
         if (!result) {
            if (!arguments.addErrorMessage(STG::SString("operation not in adequation with the types of operands")))
               return false;
         }
         uOperationCode = (result << FExtended);
         fSymbolic = (code >> FSymbolic);
      }
      else {
         uOperationCode = code & ~(~0U << FSymbolic);
         fSymbolic = (code >> FSymbolic);
      }
   }
   if (!arguments.addErrorMessage(STG::SString("unknown operation")))
      return false;
   return true;
}

OperationNode::ReadResult
OperationNode::readJSon(STG::JSon::CommonParser::State& state,
      STG::JSon::CommonParser::Arguments& arguments) {
   typedef STG::JSon::CommonParser Parser;
   ReadResult result = RRContinue;

   enum Delimiters
      {  DBegin, DAfterBegin, DReadType, DReadFirst, DReadFirstContent, DIdentifyFirstContent,
         DReadSecond, DReadSecondContent, DIdentifySecondContent, DReadCode, DEnd
      };

   #define DefineGoto(Target) case D##Target: goto L##Target;
   switch (state.point()) {
      DefineGoto(Begin)
      DefineGoto(AfterBegin)
      DefineGoto(ReadType)
      DefineGoto(ReadFirst)
      DefineGoto(ReadFirstContent)
      DefineGoto(IdentifyFirstContent)
      DefineGoto(ReadSecond)
      DefineGoto(ReadSecondContent)
      DefineGoto(IdentifySecondContent)
      DefineGoto(ReadCode)
      DefineGoto(End)
   };
   #undef DefineGoto

LBegin:
   if (!arguments.isOpenObject()) {
      if (!arguments.addErrorMessage(STG::SString("expected '{'")))
         return result;
   }
   ++state.point();
   if (!arguments.setToNextToken(result)) return result;

LAfterBegin:
   while (!arguments.isCloseObject()) {
      while (arguments.isOpen()) {
         if (!Parser::skipNodeInLoop(state, arguments, result) || result == RRNeedChars) return result;
         if (!arguments.setToNextToken(result)) return result;
      };

      if (arguments.isAddKey()
            && ((result = arguments.setArgumentKey()), arguments.key() == "type")) {
         if (result == RRNeedChars) return result;
         ++state.point();
         if (!arguments.setToNextToken(result)) return result;
LReadType:
         if (arguments.isSetString()) {
            if (arguments.setArgumentTextValue() == RRNeedChars) return RRNeedChars;
            if (arguments.valueAsText() == "bit")
               dtType = DTBit;
            else if (arguments.valueAsText() == "integer")
               dtType = DTInteger;
            else if (arguments.valueAsText() == "floating")
               dtType = DTFloating;
            else {
               if (!arguments.addErrorMessage(STG::SString("unknown type")))
                  return result;
            }
         };
         state.point() = DAfterBegin;
      }
      else
         if (result == RRNeedChars) return result;
      if (arguments.isAddKey()
            && ((result = arguments.setArgumentKey()), arguments.key() == "first")) {
         if (result == RRNeedChars) return result;
         state.point() = DReadFirst;
         if (!arguments.setToNextToken(result)) return result;
LReadFirst:
         if (arguments.isOpenObject()) {
            ++state.point();
            if (!arguments.setToNextToken(result)) return result;
LReadFirstContent:
            while (!arguments.isCloseObject()) {
               while (arguments.isOpen()) {
                  if (!Parser::skipNodeInLoop(state, arguments, result) || result == RRNeedChars) return result;
                  if (!arguments.setToNextToken(result)) return result;
               };
               if (arguments.isCloseArray())
                  {  AssumeUncalled }
               else if (arguments.isAddKey()
                     && ((result = arguments.setArgumentKey()), arguments.key() == "type")) {
                  if (result == RRNeedChars) return result;
                  state.point() = DIdentifyFirstContent;
                  if (!arguments.setToNextToken(result)) return result;
LIdentifyFirstContent:
                  if (arguments.isSetString()) {
                     if (arguments.setArgumentTextValue() == RRNeedChars) return RRNeedChars;
                     if (!setArgumentFromText(mpFirst, arguments.valueAsText(), arguments,
                              state.getResult((RuleResult*) nullptr)))
                        return RRContinue;
                  }
               }
               else
                  if (result == RRNeedChars) return result;
               if (arguments.isAddKey()
                     && ((result = arguments.setArgumentKey()), arguments.key() == "content")) {
                  if (result == RRNeedChars) return result;
                  state.point() = DReadFirstContent;
                  auto ruleResult = state.getResult((VirtualExpressionNode::RuleResult*) nullptr);
                  arguments.shiftState(state, *mpFirst, &VirtualExpressionNode::readJSon,
                        (VirtualExpressionNode::RuleResult*) nullptr);
                  state.setResult(std::move(ruleResult));
                  if (!arguments.setToNextToken(result)) return result;
                  if (!arguments.parseTokens(state, result)) return result;
                  continue;
               }
               else
                  if (result == RRNeedChars) return result;
               state.point() = DReadFirstContent;
               if (!arguments.setToNextToken(result)) return result;
            }
         }
         state.point() = DAfterBegin;
      }
      else
         if (result == RRNeedChars) return result;
      if (arguments.isAddKey()
            && ((result = arguments.setArgumentKey()), arguments.key() == "second")) {
         if (result == RRNeedChars) return result;
         state.point() = DReadSecond;
         if (!arguments.setToNextToken(result)) return result;
LReadSecond:
         if (arguments.isOpenObject()) {
            ++state.point();
            if (!arguments.setToNextToken(result)) return result;
LReadSecondContent:
            while (!arguments.isCloseObject()) {
               while (arguments.isOpen()) {
                  if (!Parser::skipNodeInLoop(state, arguments, result) || result == RRNeedChars) return result;
                  if (!arguments.setToNextToken(result)) return result;
               };
               if (arguments.isCloseArray())
                  {  AssumeUncalled }
               else if (arguments.isAddKey()
                     && ((result = arguments.setArgumentKey()), arguments.key() == "type")) {
                  if (result == RRNeedChars) return result;
                  state.point() = DIdentifySecondContent;
                  if (!arguments.setToNextToken(result)) return result;
LIdentifySecondContent:
                  if (arguments.isSetString()) {
                     if (arguments.setArgumentTextValue() == RRNeedChars) return RRNeedChars;
                     if (!setArgumentFromText(mpSecond, arguments.valueAsText(), arguments,
                              state.getResult((RuleResult*) nullptr)))
                        return RRContinue;
                  }
               }
               else
                  if (result == RRNeedChars) return result;
               if (arguments.isAddKey()
                     && ((result = arguments.setArgumentKey()), arguments.key() == "content")) {
                  if (result == RRNeedChars) return result;
                  state.point() = DReadSecondContent;
                  auto ruleResult = state.getResult((VirtualExpressionNode::RuleResult*) nullptr);
                  arguments.shiftState(state, *mpSecond, &VirtualExpressionNode::readJSon,
                        (VirtualExpressionNode::RuleResult*) nullptr);
                  state.setResult(std::move(ruleResult));
                  if (!arguments.setToNextToken(result)) return result;
                  if (!arguments.parseTokens(state, result)) return result;
                  continue;
               }
               else
                  if (result == RRNeedChars) return result;
               state.point() = DReadSecondContent;
               if (!arguments.setToNextToken(result)) return result;
            }
         }
         state.point() = DAfterBegin;
      }
      else if (arguments.isAddKey() && (result == RRNeedChars))
         return result;
      if (arguments.isAddKey()
            && ((result = arguments.setArgumentKey()), arguments.key() == "code")) {
         if (result == RRNeedChars) return result;
         state.point() = DReadCode;
         if (!arguments.setToNextToken(result)) return result;
LReadCode:
         if (arguments.isSetString()) {
            if (arguments.setArgumentTextValue() == RRNeedChars) return RRNeedChars;
            if (dtType == DTUndefined) {
               if (!arguments.addErrorMessage(STG::SString("type should be set before reading the code")))
                  return RRContinue;
            }
            else
               if (!setCodeFromText(arguments.valueAsText(), arguments))
                  return RRContinue;
         }
         state.point() = DAfterBegin;
      }
      else
         if (result == RRNeedChars) return result;
      state.point() = DAfterBegin;
      if (!arguments.setToNextToken(result)) return result;
   }
   state.point() = DEnd;
LEnd:
   arguments.reduceState(state);
   return RRHasToken;
}

STG::SubString
OperationNode::getTextOperationFromCode(unsigned code, bool isSymbolic) const {
   int arity = mpSecond.isValid() ? 2 : 1;
   bool isExtended = code & ~(~0U << FExtended);
   code >>= FExtended;

   if (!isExtended) {
      if (dtType == DTBit) {
         if (arity == 1) {
            AssumeCondition(code >= DBUOPrev && code <= DBUONegate)
            switch ((DomainBitUnaryOperation) code) {
               case DBUOUndefined: break;
               case DBUOPrev:    return STG::SString("dec");
               case DBUONext:    return STG::SString("inc");
               case DBUONegate:  return STG::SString("-");
            }
         }
         else {
            bool isCompare = (code & (1U << 3));
            if (!isCompare) {
               AssumeCondition(code >= DBBOPlus && code <= DBBOExclusiveOr)
               switch ((DomainBitBinaryOperation) code) {
                  case DBBOUndefined: break;
                  case DBBOPlus:    return STG::SString("+");
                  case DBBOMinus:   return STG::SString("-");
                  case DBBOMin:     return STG::SString("min");
                  case DBBOMax:     return STG::SString("max");
                  case DBBOOr:      return STG::SString("|");
                  case DBBOAnd:     return STG::SString("&");
                  case DBBOExclusiveOr: return STG::SString("^");
               }
            }
            else {
               code &= ~(1U << 3);
               AssumeCondition(code >= DBCOCompareLess && code <= DBCOCompareGreater)
               switch ((DomainBitCompareOperation) code) {
                  case DBCOUndefined: break;
                  case DBCOCompareLess:            return STG::SString("<");
                  case DBCOCompareLessOrEqual:     return STG::SString("<=");
                  case DBCOCompareEqual:           return STG::SString("==");
                  case DBCOCompareDifferent:       return STG::SString("!=");
                  case DBCOCompareGreaterOrEqual:  return STG::SString(">=");
                  case DBCOCompareGreater:         return STG::SString(">");
               }
            }
         }
      }
      else if (dtType == DTInteger) {
         if (arity == 1) {
            AssumeCondition(code >= DMBUOPrevSigned && code <= DMBUOBitScanReverse)
            switch ((DomainMultiBitUnaryOperation) code) {
               case DMBUOUndefined: break;
               case DMBUOPrevSigned:      return STG::SString("dec_s");
               case DMBUOPrevUnsigned:    return STG::SString("dec_u");
               case DMBUONextSigned:      return STG::SString("inc_s");
               case DMBUONextUnsigned:    return STG::SString("inc_u");
               case DMBUOBitNegate:       return STG::SString("^");
               case DMBUOOppositeSigned:  return STG::SString("-");
               case DMBUOOppositeFloat:   return STG::SString("-_f");
               case DMBUOBitScanReverse:  return STG::SString("bsr");
            }
         }
         else {
            bool isCompare = (code & (1U << 5));
            if (!isCompare) {
               AssumeCondition(code >= DMBBOConcat && code <= DMBBORightRotate)
               switch ((DomainMultiBitBinaryOperation) code) {
                  case DMBBOUndefined: break;
                  case DMBBOConcat:                return STG::SString("concat");
                  case DMBBOPlusSigned:            return STG::SString("+_s");
                  case DMBBOPlusUnsigned:          return STG::SString("+_u");
                  case DMBBOPlusUnsignedWithSigned:return STG::SString("+_us");
                  case DMBBOPlusFloat:             return STG::SString("+_f");
                  case DMBBOMinusSigned:           return STG::SString("-_s");
                  case DMBBOMinusUnsigned:         return STG::SString("-_u");
                  case DMBBOMinusFloat:            return STG::SString("-_f");
                  case DMBBOMinSigned:             return STG::SString("min_s");
                  case DMBBOMinUnsigned:           return STG::SString("min_u");
                  case DMBBOMinFloat:              return STG::SString("min_f");
                  case DMBBOMaxSigned:             return STG::SString("max_s");
                  case DMBBOMaxUnsigned:           return STG::SString("max_u");
                  case DMBBOMaxFloat:              return STG::SString("max_f");
                  case DMBBOTimesSigned:           return STG::SString("*_s");
                  case DMBBOTimesUnsigned:         return STG::SString("*_u");
                  case DMBBOTimesFloat:            return STG::SString("*_f");
                  case DMBBODivideSigned:          return STG::SString("/_s");
                  case DMBBODivideUnsigned:        return STG::SString("/_u");
                  case DMBBODivideFloat:           return STG::SString("/_f");
                  case DMBBOModuloSigned:          return STG::SString("%_s");
                  case DMBBOModuloUnsigned:        return STG::SString("%_u");
                  case DMBBOBitOr:                 return STG::SString("|");
                  case DMBBOBitAnd:                return STG::SString("&");
                  case DMBBOBitExclusiveOr:        return STG::SString("^");
                  case DMBBOLeftShift:             return STG::SString("<<");
                  case DMBBOLogicalRightShift:     return STG::SString(">>_u");
                  case DMBBOArithmeticRightShift:  return STG::SString(">>_s");
                  case DMBBOLeftRotate:            return STG::SString("rol");
                  case DMBBORightRotate:           return STG::SString("ror");
               }
            }
            else {
               code &= ~(1U << 5);
               AssumeCondition(code >= DMBCOCompareLessSigned && code <= DMBCOCompareGreaterSigned)
               switch ((DomainMultiBitCompareOperation) code) {
                  case DMBCOUndefined: break;
                  case DMBCOCompareLessSigned:              return STG::SString("<_s");
                  case DMBCOCompareLessOrEqualSigned:       return STG::SString("<=_s");
                  case DMBCOCompareLessUnsigned:            return STG::SString("<_u");
                  case DMBCOCompareLessOrEqualUnsigned:     return STG::SString("<=_u");
                  case DMBCOCompareLessFloat:               return STG::SString("<_f");
                  case DMBCOCompareLessOrEqualFloat:        return STG::SString("<=_f");
                  case DMBCOCompareEqual:                   return STG::SString("==");
                  case DMBCOCompareEqualFloat:              return STG::SString("==_f");
                  case DMBCOCompareDifferentFloat:          return STG::SString("!=_f");
                  case DMBCOCompareDifferent:               return STG::SString("!=");
                  case DMBCOCompareGreaterOrEqualFloat:     return STG::SString(">=_f");
                  case DMBCOCompareGreaterFloat:            return STG::SString(">_f");
                  case DMBCOCompareGreaterOrEqualUnsigned:  return STG::SString(">=_u");
                  case DMBCOCompareGreaterUnsigned:         return STG::SString(">_u");
                  case DMBCOCompareGreaterOrEqualSigned:    return STG::SString(">=_s");
                  case DMBCOCompareGreaterSigned:           return STG::SString(">_s");
               }
            }
         }
      }
      else if (dtType == DTFloating) {
         if (arity == 1) {
            AssumeCondition(code >= DBUOPrev && code <= DBUONegate)
            switch ((DomainMultiFloatUnaryOperation) code) {
               case DMFUOUndefined: break;
               case DMFUOCastFloat:       return STG::SString("(float)");
               case DMFUOCastDouble:      return STG::SString("(double)");
               case DMFUOOpposite:        return STG::SString("-");
               case DMFUOAbs:             return STG::SString("abs");
               case DMFUOAcos:            return STG::SString("acos");
               case DMFUOAsin:            return STG::SString("asin");
               case DMFUOAtan:            return STG::SString("atan");
               case DMFUOCeil:            return STG::SString("ceil");
               case DMFUOCos:             return STG::SString("cos");
               case DMFUOCosh:            return STG::SString("cosh");
               case DMFUOExp:             return STG::SString("exp");
               case DMFUOFabs:            return STG::SString("fabs");
               case DMFUOFloor:           return STG::SString("floor");
               case DMFUOLog:             return STG::SString("log");
               case DMFUOLog10:           return STG::SString("log10");
               case DMFUOPow:             return STG::SString("pow");
               case DMFUOSin:             return STG::SString("sin");
               case DMFUOSinh:            return STG::SString("sinh");
               case DMFUOSqrt:            return STG::SString("sqrt");
               case DMFUOTan:             return STG::SString("abs");
               case DMFUOTanh:            return STG::SString("abs");
               case DMFUOSetToNaN:        return STG::SString("abs");
               case DMFUOSetQuietBit:     return STG::SString("abs");
            }
         }
         else {
            bool isCompare = (code & (1U << 4));
            if (!isCompare) {
               AssumeCondition(code >= DMFBOPlus && code <= DMFBOModf)
               switch ((DomainMultiFloatBinaryOperation) code) {
                  case DMFBOUndefined: break;
                  case DMFBOPlus:   return STG::SString("+");
                  case DMFBOMinus:  return STG::SString("-");
                  case DMFBOMin:    return STG::SString("min");
                  case DMFBOMax:    return STG::SString("max");
                  case DMFBOTimes:  return STG::SString("*");
                  case DMFBODivide: return STG::SString("/");
                  case DMFBOAtan2:  return STG::SString("atan2");
                  case DMFBOFmod:   return STG::SString("fmod");
                  case DMFBOFrexp:  return STG::SString("frexp");
                  case DMFBOLdexp:  return STG::SString("ldexp");
                  case DMFBOModf:   return STG::SString("modf");
               }
            }
            else {
               code &= ~(1U << 4);
               AssumeCondition(code >= DMFCOCompareLess && code <= DMFCOCompareGreater)
               switch ((DomainMultiFloatCompareOperation) code) {
                  case DMFCOUndefined: break;
                  case DMFCOCompareLess:           return STG::SString("<");
                  case DMFCOCompareLessOrEqual:    return STG::SString("<=");
                  case DMFCOCompareEqual:          return STG::SString("==");
                  case DMFCOCompareDifferent:      return STG::SString("!=");
                  case DMFCOCompareGreaterOrEqual: return STG::SString(">=");
                  case DMFCOCompareGreater:        return STG::SString(">");
               }
            }
         }
      }
      else
         {  AssumeUncalled }
   }
   else { // isExtended
      AssumeCondition(code >= TECastBit && code <= TEIntersect)
      switch (code) {
         case TECastBit:         return STG::SString("(bit)");
         case TECastInteger:     return STG::SString("(integer)");
         case TECastFloating:    return STG::SString("(floating)");
         case TECastFloatingPtr: return STG::SString("(floating_ptr)");
         case TECallExtendZero:  return STG::SString("extend_zero");
         case TECallExtendSign:  return STG::SString("extend_sign");
         case TECallReduce:      return STG::SString("reduce");
         case TECallBitSet:      return STG::SString("bitset");
         case TECallShiftBit:    return STG::SString("shiftbit");
         case TEMerge:           return STG::SString("merge");
         case TEIntersect:       return STG::SString("intersect");
      }
   }
   return STG::SString();
}

OperationNode::WriteResult
OperationNode::writeJSon(STG::JSon::CommonWriter::State& state,
      STG::JSon::CommonWriter::Arguments& arguments) const {
   WriteResult result = WRNeedEvent;
   enum Delimiters
      {  DBegin, DAfterBegin, DWriteType,
         DWriteFirst, DWriteFirstKey, DWriteFirstType, DIdentifyFirstType, DWriteFirstContent, DCloseFirst,
         DWriteSecond, DWriteSecondKey, DWriteSecondType, DIdentifySecondType, DWriteSecondContent, DCloseSecond,
         DWriteCode, DWriteCodeContent, DEnd
      };

   #define DefineGoto(Target) case D##Target: goto L##Target;
   switch (state.point()) {
      DefineGoto(Begin)
      DefineGoto(AfterBegin)
      DefineGoto(WriteType)
      DefineGoto(WriteFirstKey)
      DefineGoto(WriteFirst)
      DefineGoto(WriteFirstType)
      DefineGoto(IdentifyFirstType)
      DefineGoto(WriteFirstContent)
      DefineGoto(CloseFirst)
      DefineGoto(WriteSecondKey)
      DefineGoto(WriteSecond)
      DefineGoto(WriteSecondType)
      DefineGoto(IdentifySecondType)
      DefineGoto(WriteSecondContent)
      DefineGoto(CloseSecond)
      DefineGoto(WriteCode)
      DefineGoto(WriteCodeContent)
      DefineGoto(End)
   };
   #undef DefineGoto

LBegin:
   arguments.setOpenObject();
   ++state.point();
   if (!arguments.writeEvent(result)) return result;

LAfterBegin:
   if (dtType != DTUndefined) {
      arguments.setAddKey(STG::SString("type"));
      ++state.point();
      if (!arguments.writeEvent(result)) return result;

LWriteType:
      if (dtType == DTBit)
         arguments.setStringValue(STG::SString("bit"));
      else if (dtType == DTInteger)
         arguments.setStringValue(STG::SString("integer"));
      else
         arguments.setStringValue(STG::SString("floating"));
      ++state.point();
      if (!arguments.writeEvent(result)) return result;
   }
   else
      state.point() = DWriteFirstKey;

LWriteFirstKey:
   if (mpFirst.isValid()) {
      arguments.setAddKey(STG::SString("first"));
      ++state.point();
      if (!arguments.writeEvent(result)) return result;

LWriteFirst:
      arguments.setOpenObject();
      ++state.point();
      if (!arguments.writeEvent(result)) return result;

LWriteFirstType:
      arguments.setAddKey(STG::SString("type"));
      ++state.point();
      if (!arguments.writeEvent(result)) return result;

LIdentifyFirstType:
      arguments.setStringValue(getTextFromArgument(*mpFirst));
      ++state.point();
      if (!arguments.writeEvent(result)) return result;

LWriteFirstContent:
      arguments.setAddKey(STG::SString("content"));
      ++state.point();
      arguments.shiftState(state, *mpFirst, &VirtualExpressionNode::writeJSon,
            (char*) nullptr);
      if (!arguments.writeEvent(result)) return result;
      if (!arguments.writeTokens(state, result)) return result;

LCloseFirst:
      arguments.setCloseObject();
      ++state.point();
      if (!arguments.writeEvent(result)) return result;
   }
   else
      state.point() = DWriteSecondKey;

LWriteSecondKey:
   if (mpSecond.isValid()) {
      arguments.setAddKey(STG::SString("first"));
      ++state.point();
      if (!arguments.writeEvent(result)) return result;

LWriteSecond:
      arguments.setOpenObject();
      ++state.point();
      if (!arguments.writeEvent(result)) return result;

LWriteSecondType:
      arguments.setAddKey(STG::SString("type"));
      ++state.point();
      if (!arguments.writeEvent(result)) return result;

LIdentifySecondType:
      arguments.setStringValue(getTextFromArgument(*mpSecond));
      ++state.point();
      if (!arguments.writeEvent(result)) return result;

LWriteSecondContent:
      arguments.setAddKey(STG::SString("content"));
      ++state.point();
      arguments.shiftState(state, *mpSecond, &VirtualExpressionNode::writeJSon,
            (char*) nullptr);
      if (!arguments.writeEvent(result)) return result;
      if (!arguments.writeTokens(state, result)) return result;

LCloseSecond:
      arguments.setCloseObject();
      ++state.point();
      if (!arguments.writeEvent(result)) return result;
   }
   else
      state.point() = DWriteSecondKey;

LWriteCode:
   if (uOperationCode > 0 && mpFirst.isValid()) {
      arguments.setAddKey(STG::SString("code"));
      ++state.point();
      if (!arguments.writeEvent(result)) return result;

LWriteCodeContent:
      arguments.setStringValue(getTextOperationFromCode(uOperationCode, fSymbolic));
      ++state.point();
      if (!arguments.writeEvent(result)) return result;
   }
   else
      state.point() = DEnd;

LEnd:
   arguments.setCloseObject();
   arguments.reduceState(state);
   if (!arguments.writeEvent(result)) return result;
   return WRNeedEvent;
}

/* Implementation of the class Expression */

Expression::ReadResult
Expression::readJSon(STG::JSon::CommonParser::State& state,
      STG::JSon::CommonParser::Arguments& arguments) {
   typedef STG::JSon::CommonParser Parser;
   ReadResult result = RRContinue;

   enum Delimiters
      {  DBegin, DAfterBegin, DReadContent, DIdentifyContent, DEnd };

   #define DefineGoto(Target) case D##Target: goto L##Target;
   switch (state.point()) {
      DefineGoto(Begin)
      DefineGoto(ReadContent)
      DefineGoto(IdentifyContent)
      DefineGoto(End)
   };
   #undef DefineGoto

LBegin:
   if (!arguments.isOpenObject()) {
      if (!arguments.addErrorMessage(STG::SString("expected '{'")))
         return result;
   }
   ++state.point();
   if (!arguments.setToNextToken(result)) return result;

LReadContent:
   while (!arguments.isCloseObject()) {
      while (arguments.isOpen()) {
         if (!Parser::skipNodeInLoop(state, arguments, result) || result == RRNeedChars) return result;
         if (!arguments.setToNextToken(result)) return result;
      };
      if (arguments.isCloseArray())
         {  AssumeUncalled }
      else if (arguments.isAddKey()
            && ((result = arguments.setArgumentKey()), arguments.key() == "type")) {
         if (result == RRNeedChars) return result;
         state.point() = DIdentifyContent;
         if (!arguments.setToNextToken(result)) return result;
LIdentifyContent:
         if (arguments.isSetString()) {
            if (arguments.setArgumentTextValue() == RRNeedChars) return RRNeedChars;
            if (!VirtualExpressionNode::setArgumentFromText(mpContent, arguments.valueAsText(),
                     arguments, state.getResult((RuleResult*) nullptr)))
               return RRContinue;
         }
         state.point() = DReadContent;
         if (!arguments.setToNextToken(result)) return result;
         // may skip two consecutive types
      }
      else
         if (result == RRNeedChars) return result;
      if (arguments.isAddKey()
            && ((result = arguments.setArgumentKey()), arguments.key() == "content")) {
         if (result == RRNeedChars) return result;
         state.point() = DReadContent;
         auto ruleResult = state.getResult((VirtualExpressionNode::RuleResult*) nullptr);
         arguments.shiftState(state, *mpContent, &VirtualExpressionNode::readJSon,
               (VirtualExpressionNode::RuleResult*) nullptr);
         state.setResult(std::move(ruleResult));
         if (!arguments.setToNextToken(result)) return result;
         if (!arguments.parseTokens(state, result)) return result;
         continue;
      }
      else
         if (result == RRNeedChars) return result;
      state.point() = DReadContent;
      if (!arguments.setToNextToken(result)) return result;
   }
   state.point() = DEnd;
LEnd:
   arguments.reduceState(state);
   return RRHasToken;
}

Expression::WriteResult
Expression::writeJSon(STG::JSon::CommonWriter::State& state,
      STG::JSon::CommonWriter::Arguments& arguments) const {
   WriteResult result = WRNeedEvent;
   enum Delimiters
      {  DBegin, DWriteType, DIdentifyType, DWriteContent, DEnd };

   #define DefineGoto(Target) case D##Target: goto L##Target;
   switch (state.point()) {
      DefineGoto(Begin)
      DefineGoto(WriteType)
      DefineGoto(IdentifyType)
      DefineGoto(WriteContent)
      DefineGoto(End)
   };
   #undef DefineGoto

LBegin:
   arguments.setOpenObject();
   ++state.point();
   if (!arguments.writeEvent(result)) return result;

LWriteType:
   if (mpContent.isValid()) {
      arguments.setAddKey(STG::SString("type"));
      ++state.point();
      if (!arguments.writeEvent(result)) return result;

LIdentifyType:
      arguments.setStringValue(VirtualExpressionNode::getTextFromArgument(*mpContent));
      ++state.point();
      if (!arguments.writeEvent(result)) return result;

LWriteContent:
      arguments.setAddKey(STG::SString("content"));
      ++state.point();
      arguments.shiftState(state, *mpContent, &VirtualExpressionNode::writeJSon,
            (char*) nullptr);
      if (!arguments.writeEvent(result)) return result;
      if (!arguments.writeTokens(state, result)) return result;
   }
   else
      state.point() = DEnd;

LEnd:
   arguments.setCloseObject();
   arguments.reduceState(state);
   if (!arguments.writeEvent(result)) return result;
   return WRNeedEvent;
}


