#include "MemoryState.h"
#include "Collection/Collection.template"

MemoryModelFunctions MemoryState::functions={
   &MemoryState::set_number_of_registers,
   &MemoryState::set_register_value,
   &MemoryState::get_register_value,
   &MemoryState::load_multibit_value,
   &MemoryState::load_multibit_disjunctive_value,
   &MemoryState::load_multifloat_value,
   &MemoryState::store_value,
   nullptr /* &MemoryState::constraint_store_value */,
   nullptr /* &MemoryState::constraint_address */
};

template class COL::TSortedAVL<MemoryState::RegisterValue, MemoryState::RegisterValue::Key>;

STG::Lexer::Base::ReadResult
VirtualAddressConstraint::readJSon(STG::JSon::CommonParser::State& state,
      STG::JSon::CommonParser::Arguments& arguments) {
   typedef STG::JSon::CommonParser Parser;
   ReadResult result = RRContinue;

   enum Delimiters { DBegin, DAfterBegin, DConstraint, DZoneName, DEnd, DInherited };

   #define DefineGoto(Target) case D##Target: goto L##Target;
   switch (state.point()) {
      DefineGoto(Begin)
      DefineGoto(AfterBegin)
      DefineGoto(Constraint)
      DefineGoto(ZoneName)
      DefineGoto(End)
      default:
         if (state.point() >= DInherited)
            goto LInherited;
         return result;
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
   while (!arguments.isClose()) {
      while (arguments.isOpen()) {
         if (!Parser::skipNodeInLoop(state, arguments, result) || result == RRNeedChars) return result;
         if (!arguments.setToNextToken(result)) return result;
      };
      AssumeCondition(!arguments.isCloseArray())
      if (arguments.isAddKey()
            && ((result = arguments.setArgumentKey()), arguments.key() == "constraint")) {
         if (result == RRNeedChars) return result;
         {  auto ruleResult = state.getResult((ReadRuleResult*) nullptr);
            state.point() = DConstraint;
            arguments.shiftState(state, eConstraint, &Expression::readJSon,
                  (Expression::RuleResult*) nullptr);
            state.getSResult((Expression::RuleResult*) nullptr) = ruleResult.elementFunctions;
         }
         if (!arguments.setToNextToken(result)) return result;
         if (!arguments.parseTokens(state, result)) return result;
LConstraint:
         state.point() = DAfterBegin;
         if (!arguments.setToNextToken(result)) return result;
         continue;
      }
      else
         if (result == RRNeedChars) return result;

      if (arguments.isAddKey()) {
         if ((result = arguments.setArgumentKey()) == RRNeedChars) return result;
         state.point() = DInherited;
LInherited:
         state.point() -= DInherited;
         if (readFromKey(arguments.key(), state, arguments, result)) {
            state.point() += DInherited;
            if (result == RRNeedChars) return result;
            state.point() = DAfterBegin;
            if (!arguments.setToNextToken(result)) return result;
            continue;
         }
         else
            state.point() += DInherited;
      }

      if (arguments.isAddKey()
            && ((result = arguments.setArgumentKey()), arguments.key() == "zone_name")) {
         if (result == RRNeedChars) return result;
         state.point() = DZoneName;
         if (!arguments.setToNextToken(result)) return result;
LZoneName:
         if (arguments.isSetString()) {
            if (arguments.setArgumentTextValue() == RRNeedChars) return RRNeedChars;
            ssZoneName = arguments.valueAsText();
         }
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

STG::Lexer::Base::WriteResult
VirtualAddressConstraint::writeJSon(STG::JSon::CommonWriter::State& state,
      STG::JSon::CommonWriter::Arguments& arguments) const {
   WriteResult result = WRNeedEvent;
   enum Delimiters { DBegin, DStart, DBeforeZoneName, DZoneName, DEnd, DInherited };

   #define DefineGoto(Target) case D##Target: goto L##Target;
   switch (state.point()) {
      DefineGoto(Begin)
      DefineGoto(Start)
      DefineGoto(BeforeZoneName)
      DefineGoto(ZoneName)
      DefineGoto(End)
      default:
         if (state.point() >= DInherited)
            goto LInherited;
         return result;
   }

LBegin:
   arguments.setOpenObject();
   ++state.point();
   if (!arguments.writeEvent(result)) return result;

LStart:
   arguments.setAddKey(STG::SString("constraint"));
   ++state.point();
   arguments.shiftState(state, eConstraint, &Expression::writeJSon,
         (char*) nullptr);
   if (!arguments.writeEvent(result)) return result;
   if (!arguments.writeTokens(state, result)) return result;

LBeforeZoneName:
   arguments.setAddKey(STG::SString("zone_name"));
   state.point() = DInherited;
   if (!arguments.writeEvent(result)) return result;

LInherited:
   state.point() -= DInherited;
   if (writeToKey(state, arguments, result)) {
      state.point() += DInherited;
      if (result == WRNeedWrite) return result;
   };
   state.point() = DZoneName;

LZoneName:
   arguments.setStringValue(ssZoneName);
   ++state.point();
   if (!arguments.writeEvent(result)) return result;

LEnd:
   arguments.setCloseObject();
   arguments.reduceState(state);
   if (!arguments.writeEvent(result)) return result;
   return WRNeedEvent;
}

bool
RegisterConstraint::readFromKey(const STG::SubString& key, STG::JSon::CommonParser::State& state,
      STG::JSon::CommonParser::Arguments& arguments, ReadResult& result) {
   if (state.point() == 0) {
      if (key != "register")
         return false;
      ++state.point();
      if (!arguments.setToNextToken(result)) return true;
   }
   if (state.point() == 1) {
      if (arguments.isSetString()) {
         if (arguments.setArgumentTextValue() == RRNeedChars) return RRNeedChars;
         const auto& ruleResult = state.getResult((ReadRuleResult*) nullptr);
         AssumeCondition(ruleResult.processor && ruleResult.processorFunctions)
         uRegisterIndex = (*ruleResult.processorFunctions->get_register_index)
               (ruleResult.processor, arguments.valueAsText().getChunk().string);
         state.point() = 0;
      }
   }
   return true;
}

bool
RegisterConstraint::writeToKey(STG::JSon::CommonWriter::State& state,
      STG::JSon::CommonWriter::Arguments& arguments, WriteResult& result) const {
   const char* name = nullptr;
   if (state.point() == 0) {
      const auto& ruleResult = state.getResult((WriteRuleResult*) nullptr);
      AssumeCondition(ruleResult.processor && ruleResult.processorFunctions)
      name = (*ruleResult.processorFunctions->get_register_name)(ruleResult.processor,
            uRegisterIndex);
      if (name == nullptr)
         return false;
      arguments.setAddKey(STG::SString("name"));
      ++state.point();
      if (!arguments.writeEvent(result)) return true;
   }
   if (state.point() == 1) {
      if (!name) {
         const auto& ruleResult = state.getResult((WriteRuleResult*) nullptr);
         AssumeCondition(ruleResult.processor && ruleResult.processorFunctions)
         name = (*ruleResult.processorFunctions->get_register_name)(ruleResult.processor,
               uRegisterIndex);
      }
      arguments.setStringValue(STG::SString(name));
      ++state.point();
      if (!arguments.writeEvent(result)) return result;
   }
   return true;
}

bool
IndirectAddressConstraint::readFromKey(const STG::SubString& key, STG::JSon::CommonParser::State& state,
      STG::JSon::CommonParser::Arguments& arguments, ReadResult& result) {
   if (state.point() == 0) {
      if (key != "address")
         return false;
      {  auto ruleResult = state.getResult((ReadRuleResult*) nullptr);
         ++state.point();
         arguments.shiftState(state, eAddress, &Expression::readJSon,
               (VirtualExpressionNode::RuleResult*) nullptr);
         state.setResult(std::move(ruleResult.elementFunctions));
      }
      if (!arguments.setToNextToken(result)) return true;
      if (!arguments.parseTokens(state, result)) return true;
   }
   if (state.point() == 1)
      state.point() = 0;
   return true;
}

bool
IndirectAddressConstraint::writeToKey(STG::JSon::CommonWriter::State& state,
      STG::JSon::CommonWriter::Arguments& arguments, WriteResult& result) const {
   if (state.point() == 0) {
      arguments.setAddKey(STG::SString("address"));
      ++state.point();
      arguments.shiftState(state, eAddress, &Expression::writeJSon,
            (char*) nullptr);
      if (!arguments.writeEvent(result)) return true;
      if (!arguments.writeTokens(state, result)) return true;
   }
   if (state.point() == 1)
      state.point() = 0;
   return true;
}

STG::Lexer::Base::ReadResult
MemoryStateConstraint::readJSon(STG::JSon::CommonParser::State& state,
      STG::JSon::CommonParser::Arguments& arguments) {
   typedef STG::JSon::CommonParser Parser;
   ReadResult result = RRContinue;

   enum Delimiters { DBegin, DAfterBegin, DMemoryConstraint, DIdentifyContent, DEnd };

   #define DefineGoto(Target) case D##Target: goto L##Target;
   switch (state.point()) {
      DefineGoto(Begin)
      DefineGoto(AfterBegin)
      DefineGoto(MemoryConstraint)
      DefineGoto(IdentifyContent)
      DefineGoto(End)
   };
   #undef DefineGoto

LBegin:
   if (!arguments.isOpenArray()) {
      if (!arguments.addErrorMessage(STG::SString("expected '['")))
         return result;
   }
   ++state.point();
   if (!arguments.setToNextToken(result)) return result;

LAfterBegin:
   while (!arguments.isCloseArray()) {
      while (arguments.isOpenArray()) {
         if (!Parser::skipNodeInLoop(state, arguments, result) || result == RRNeedChars) return result;
         if (!arguments.setToNextToken(result)) return result;
      };
      if (arguments.isCloseObject())
         {  AssumeUncalled }

      if (arguments.isOpenObject()) {
         ++state.point();
         if (!arguments.setToNextToken(result)) return result;

LMemoryConstraint:
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
                  setTypeConstraintFromText(state.getSResult((ReadRuleResult*) nullptr).type,
                     arguments.valueAsText());
               }
               state.point() = DMemoryConstraint;
               if (!arguments.setToNextToken(result)) return result;
            }
            else
               if (result == RRNeedChars) return result;
            if (arguments.isAddKey()
                  && ((result = arguments.setArgumentKey()), arguments.key() == "content")) {
               if (result == RRNeedChars) return result;
               state.point() = DMemoryConstraint;
               const auto& ruleResult = state.getResult((ReadRuleResult*) nullptr);
               insertNewAtEnd(newAddressConstraint(ruleResult.type).extractElement());
               arguments.shiftState(state, getSLast(), &VirtualAddressConstraint::readJSon,
                     (VirtualAddressConstraint::RuleResult*) nullptr);
               state.setResult(VirtualAddressConstraint::ReadRuleResult(ruleResult));
               if (!arguments.setToNextToken(result)) return result;
               if (!arguments.parseTokens(state, result)) return result;
            }
            else
               if (result == RRNeedChars) return result;
            state.point() = DMemoryConstraint;
            if (!arguments.setToNextToken(result)) return result;
         }
      }
      state.point() = DAfterBegin;
      if (!arguments.setToNextToken(result)) return result;
   }
   state.point() = DEnd;
LEnd:
   arguments.reduceState(state);
   return RRHasToken;
}

STG::Lexer::Base::WriteResult
MemoryStateConstraint::writeJSon(STG::JSon::CommonWriter::State& state, STG::JSon::CommonWriter::Arguments& arguments) const {
   WriteResult result = WRNeedEvent;
   enum Delimiters
      {  DBegin, DAfterBegin, DWriteConstraint, DWriteType, DIdentifyType,
         DWriteContent, DEndContent, DEnd
      };
   Cursor* cursor = nullptr;

   #define DefineGoto(Target) case D##Target: goto L##Target;
   switch (state.point()) {
      DefineGoto(Begin)
      DefineGoto(AfterBegin)
      DefineGoto(WriteConstraint)
      DefineGoto(WriteType)
      DefineGoto(IdentifyType)
      DefineGoto(WriteContent)
      DefineGoto(EndContent)
      DefineGoto(End)
   };
   #undef DefineGoto

LBegin:
   arguments.setOpenArray();
   state.getSResult((WriteRuleResult*) nullptr).writeCursor.absorbElement(new Cursor(*this));
   ++state.point();
   if (!arguments.writeEvent(result)) return result;

LAfterBegin:
   cursor = &*state.getSResult((WriteRuleResult*) nullptr).writeCursor;
   while (cursor->setToNext()) {
      state.point() = DWriteConstraint;

LWriteConstraint:
      arguments.setOpenObject();
      ++state.point();
      if (!arguments.writeEvent(result)) return result;

LWriteType:
      arguments.setAddKey(STG::SString("type"));
      ++state.point();
      if (!arguments.writeEvent(result)) return result;

LIdentifyType:
      if (!cursor)
         cursor = &*state.getSResult((WriteRuleResult*) nullptr).writeCursor;
      arguments.setStringValue(getTextFromConstraint(cursor->elementAt()));
      ++state.point();
      if (!arguments.writeEvent(result)) return result;

LWriteContent:
      arguments.setAddKey(STG::SString("content"));
      ++state.point();
      if (!cursor)
         cursor = &*state.getSResult((WriteRuleResult*) nullptr).writeCursor;
      arguments.shiftState(state, cursor->elementSAt(), &VirtualAddressConstraint::writeJSon,
            (char*) nullptr);
      if (!arguments.writeEvent(result)) return result;
      if (!arguments.writeTokens(state, result)) return result;

LEndContent:
      state.point() = DAfterBegin;
      if (!cursor)
         cursor = &*state.getSResult((WriteRuleResult*) nullptr).writeCursor;
   }

LEnd:
   arguments.setCloseArray();
   arguments.reduceState(state);
   if (!arguments.writeEvent(result)) return result;
   return WRNeedEvent;
}



