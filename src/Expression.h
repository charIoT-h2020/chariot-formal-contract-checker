#pragma once

#include "Pointer/ExtPointer.hpp"
#include "JSON/JSonParser.h"
#include "dll.h"
#include "decsec_callback.h"
#include "DomainValue.h"

const char* debugPrint(STG::IOObject* object);

class Expression;
class VirtualExpressionNode : public PNT::MngElement, public STG::IOObject, public STG::Lexer::Base {
  public:
   typedef struct _DomainElementFunctions* RuleResult;
   enum TypeExpression { TERegisterAccess, TEIndirection, TEDomain, TEOperation };

  protected:
   virtual ComparisonResult _compare(const EnhancedObject& asource) const override
      {  ComparisonResult result = PNT::MngElement::_compare(asource);
         if (result == CREqual) {
            const auto& source = static_cast<const VirtualExpressionNode&>(castFromCopyHandler(asource));
            result = fcompare(getType(), source.getType());
         }
         return result;
      }

   VirtualExpressionNode() = default;
   VirtualExpressionNode(const VirtualExpressionNode&) = default;
   static STG::SubString ssJSonContent;

   static bool setArgumentFromText(PNT::TMngPointer<VirtualExpressionNode>& argument,
         const STG::SubString& text, STG::JSon::CommonParser::Arguments& context,
         struct _DomainElementFunctions* functions);
   static STG::SubString getTextFromArgument(VirtualExpressionNode& argument);
   friend class Expression;

  public:
   DefineCopy(VirtualExpressionNode)
   StaticInheritConversions(VirtualExpressionNode, PNT::MngElement)
   DCompare(VirtualExpressionNode)

   virtual bool isValid() const override { return PNT::MngElement::isValid(); }
   virtual DomainType getType() const { return DTUndefined; }
   virtual TypeExpression getTypeExpression() const { AssumeUncalled return TERegisterAccess; }
   virtual ReadResult readJSon(STG::JSon::CommonParser::State& state, STG::JSon::CommonParser::Arguments& arguments) { AssumeUncalled return RRContinue; }
   virtual WriteResult writeJSon(STG::JSon::CommonWriter::State& state, STG::JSon::CommonWriter::Arguments& arguments) const { AssumeUncalled return WRNeedEvent; }
};

class RegisterAccessNode : public VirtualExpressionNode {
  private:
   STG::SubString ssRegisterName = STG::SString();

  protected:
   virtual ComparisonResult _compare(const EnhancedObject& asource) const override
      {  ComparisonResult result = VirtualExpressionNode::_compare(asource);
         if (result == CREqual) {
            const auto& source = static_cast<const RegisterAccessNode&>(castFromCopyHandler(asource));
            result = ssRegisterName.compare(source.ssRegisterName);
         }
         return result;
      }

  public:
   RegisterAccessNode() = default;
   RegisterAccessNode(const RegisterAccessNode&) = default;
   DefineCopy(RegisterAccessNode)

   const STG::SubString& getName() const { return ssRegisterName; }
   virtual DomainType getType() const override { return DTInteger; }
   virtual TypeExpression getTypeExpression() const override { return TERegisterAccess; }
   virtual ReadResult readJSon(STG::JSon::CommonParser::State& state, STG::JSon::CommonParser::Arguments& arguments) override;
   virtual WriteResult writeJSon(STG::JSon::CommonWriter::State& state, STG::JSon::CommonWriter::Arguments& arguments) const override;
};

class IndirectionNode : public VirtualExpressionNode {
  private:
   PNT::TMngPointer<VirtualExpressionNode> mpAddress;
   int uSizeInBytes = 0;

  protected:
   virtual ComparisonResult _compare(const EnhancedObject& asource) const override
      {  ComparisonResult result = VirtualExpressionNode::_compare(asource);
         if (result == CREqual) {
            const auto& source = static_cast<const IndirectionNode&>(castFromCopyHandler(asource));
            if (!mpAddress.isValid())
               result = !source.mpAddress.isValid() ? CREqual : CRLess;
            else if (!source.mpAddress.isValid())
               result = CRGreater;
            else
               result = mpAddress->compare(*source.mpAddress);
         }
         return result;
      }

  public:
   IndirectionNode() = default;
   IndirectionNode(const IndirectionNode&) = default;
   DefineCopy(IndirectionNode)

   const VirtualExpressionNode& getAddress() const { return *mpAddress; }
   int getSizeInBytes() const { return uSizeInBytes; }

   virtual DomainType getType() const override { return DTInteger; }
   virtual TypeExpression getTypeExpression() const override { return TEIndirection; }
   virtual ReadResult readJSon(STG::JSon::CommonParser::State& state, STG::JSon::CommonParser::Arguments& arguments) override;
   virtual WriteResult writeJSon(STG::JSon::CommonWriter::State& state, STG::JSon::CommonWriter::Arguments& arguments) const override;
};

class DomainNode : public VirtualExpressionNode {
  private:
   DomainValue deValue;

   class AbstractToken;
   class NumberToken;
   class IdentifierToken;
   class KeywordToken;
   class SizedKeywordToken;
   class FloatSizedKeywordToken;
   class OperatorPunctuatorToken;
   class Lexer;
   class Operator;
   class OperatorStack;
   struct IdentifierTokenResult;
   template <typename> class Parser;

   template <typename T>
   DomainValue convertTokenToValue(const NumberToken& token,
         typename Parser<T>::Arguments& arguments, bool& doesRetry);
   template <typename T>
   DomainValue convertTokenToValue(const IdentifierTokenResult& token,
         typename Parser<T>::Arguments& arguments, bool& doesRetry);
   void convertTokenToQualification(const IdentifierTokenResult& token, Operator& lastOperator);
   template <typename T>
   void convertTokenToError(const IdentifierTokenResult& token,
         typename Parser<T>::Arguments& arguments, bool& doesRetry);
   template <typename T>
   bool readParenToken(typename Parser<T>::State& state,
         typename Parser<T>::Arguments& arguments, STG::Lexer::Base::ReadResult& result);
   template <typename T>
   bool readIntervalToken(typename Parser<T>::State& state,
         typename Parser<T>::Arguments& arguments, STG::Lexer::Base::ReadResult& result,
         bool& hasReadToken);
   template <typename T>
   bool readSetToken(typename Parser<T>::State& state,
         typename Parser<T>::Arguments& arguments, STG::Lexer::Base::ReadResult& result,
         bool& hasReadToken);
   template <typename T>
   ReadResult readToken(typename Parser<T>::State& state, typename Parser<T>::Arguments& arguments);

  protected:
   virtual ComparisonResult _compare(const EnhancedObject& asource) const override
      {  ComparisonResult result = VirtualExpressionNode::_compare(asource);
         if (result == CREqual) {
            const auto& source = static_cast<const DomainNode&>(castFromCopyHandler(asource));
            result = deValue.compare(source.deValue);
         }
         return result;
      }

  public:
   DomainNode(struct _DomainElementFunctions* functions) : deValue(functions) {}
   DomainNode(DomainElement&& value, struct _DomainElementFunctions* functions)
      :  deValue(std::move(value), functions) {}
   DomainNode(DomainNode&& source) = default;
   DomainNode(const DomainNode& source) = default;
   DefineCopy(DomainNode)

   DomainNode& operator=(DomainNode&& source) = default;
   DomainNode& operator=(const DomainNode& source) = default;

   virtual DomainType getType() const override { return deValue.getType(); }
   virtual TypeExpression getTypeExpression() const override { return TEDomain; }
   ZeroResult queryZeroResult() const { return deValue.queryZeroResult(); }
   int getSizeInBits() const { return deValue.getSizeInBits(); }
   const DomainValue& getValue() const { return deValue; }

   virtual ReadResult readJSon(STG::JSon::CommonParser::State& state, STG::JSon::CommonParser::Arguments& arguments) override;
   virtual WriteResult writeJSon(STG::JSon::CommonWriter::State& state, STG::JSon::CommonWriter::Arguments& arguments) const override;
};

class OperationNode : public VirtualExpressionNode {
  private:
   DomainType dtType = DTUndefined;
   int uOperationCode = 0;
   bool fSymbolic = false;
   PNT::TMngPointer<VirtualExpressionNode> mpFirst;
   PNT::TMngPointer<VirtualExpressionNode> mpSecond;
   int uSizeInBits = 0;
   int uStart = 0;
   bool fSigned = false;

   enum TypeOperation
      {  TUndefined, TPlus, TPlusSigned, TPlusUnsigned, TPlusUnsignedWithSigned,
         TMinus, TMinusSigned, TMinusUnsigned,
         TTimes, TTimesSigned, TTimesUnsigned, TDivide, TDivideSigned, TDivideUnsigned,
         TModulo, TModuloSigned, TModuloUnsigned, TLeftShift,
         TRightShift, TRightShiftSigned, TRightShiftUnsigned,
         TEqual, TDifferent, TLessOrEqual, TLessOrEqualSigned, TLessOrEqualUnsigned,
         TGreaterOrEqual, TGreaterOrEqualSigned, TGreaterOrEqualUnsigned, TLess, TLessSigned,
         TLessUnsigned, TGreater, TGreaterSigned, TGreaterUnsigned,
         TLogicalAnd, TLogicalOr, TBitAnd, TBitOr, TBitXOr,
         TNot, TComplement,
         TCallInc, TCallIncSigned, TCallIncUnsigned, TCallDec, TCallDecSigned, TCallDecUnsigned,
         TCallMin, TCallMinSigned, TCallMinUnsigned, TCallMax, TCallMaxSigned, TCallMaxUnsigned,
         TCallConcat
      };

   enum TypeExtended
      {  TEUndefined, TECastBit, TECastInteger, TECastFloating, TECastFloatingPtr,
         TECallExtendZero, TECallExtendSign, TECallReduce, TECallBitSet,
         TECallShiftBit,
         TEMerge, TEIntersect
      };

   enum Fields { FExtended = 0, FType=1, FSymbolic=8 };

   bool setCodeFromText(const STG::SubString& text, STG::JSon::CommonParser::Arguments& arguments);
   STG::SubString getTextOperationFromCode(unsigned code, bool isSymbolic) const;
   bool hasSizeExtension(int code) const;
   bool hasStartExtension(int code) const;
   bool hasSignedExtension(int code) const;

  protected:
   virtual ComparisonResult _compare(const EnhancedObject& asource) const override
      {  ComparisonResult result = VirtualExpressionNode::_compare(asource);
         if (result == CREqual) {
            const auto& source = static_cast<const OperationNode&>(castFromCopyHandler(asource));
            result = fcompare(dtType, source.dtType);
            if (result != CREqual)
               return result;
            result = fcompare(mpSecond.isValid() ? 2 : 1, source.mpSecond.isValid() ? 2 : 1);
            if (result != CREqual)
               return result;
            result = fcompare(uOperationCode, source.uOperationCode);
            if (result != CREqual)
               return result;
            result = mpFirst->compare(*source.mpFirst);
            if (result != CREqual)
               return result;
            if (mpSecond.isValid())
               result = mpSecond->compare(*source.mpSecond);
         }
         return result;
      }

  public:
   OperationNode() = default;
   OperationNode(DomainMultiBitBinaryOperation operation,
         PNT::TMngPointer<VirtualExpressionNode> first, PNT::TMngPointer<VirtualExpressionNode> second)
      :  uOperationCode(operation), mpFirst(first), mpSecond(second) {}
   OperationNode(const OperationNode&) = default;
   DefineCopy(OperationNode)

   virtual bool isValid() const override;
   const VirtualExpressionNode& getFirst() const { return *mpFirst; }
   const VirtualExpressionNode& getSecond() const { return *mpSecond; }
   bool isBinary() const { return mpSecond.isValid(); }
   void applyOperation(DomainValue& first, const DomainValue& second) const;
   virtual DomainType getType() const override { return dtType; }
   virtual TypeExpression getTypeExpression() const override { return TEOperation; }
   virtual ReadResult readJSon(STG::JSon::CommonParser::State& state, STG::JSon::CommonParser::Arguments& arguments) override;
   virtual WriteResult writeJSon(STG::JSon::CommonWriter::State& state, STG::JSon::CommonWriter::Arguments& arguments) const override;
};

class Expression : public STG::IOObject, public STG::Lexer::Base {
  public:
   typedef struct _DomainElementFunctions* RuleResult;

  private:
   PNT::TMngPointer<VirtualExpressionNode> mpContent;

  public:
   // recursive destruction in ~Expression

   Expression& operator-=(const Expression& source)
      {  PNT::TMngPointer<VirtualExpressionNode> first = mpContent, second = source.mpContent;
         mpContent.absorbElement(new OperationNode(DMBBOMinusSigned, first, second));
         return *this;
      }
   Expression& operator+=(const Expression& source)
      {  PNT::TMngPointer<VirtualExpressionNode> first = mpContent, second = source.mpContent;
         mpContent.absorbElement(new OperationNode(DMBBOPlusSigned, first, second));
         return *this;
      }
   void clear() { mpContent.release(); }
   const VirtualExpressionNode& getContent() const { return *mpContent; }
   ReadResult readJSon(STG::JSon::CommonParser::State& state, STG::JSon::CommonParser::Arguments& arguments);
   WriteResult writeJSon(STG::JSon::CommonWriter::State& state, STG::JSon::CommonWriter::Arguments& arguments) const;
};

