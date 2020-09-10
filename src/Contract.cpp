#include "Contract.h"
#include "Collection/Collection.template"

STG::Lexer::Base::ReadResult
Contract::readJSon(STG::JSon::CommonParser::State& state,
      STG::JSon::CommonParser::Arguments& arguments) {
   typedef STG::JSon::CommonParser Parser;
   ReadResult result = RRContinue;
   bool hasHit = false;

   enum Delimiters
      {  DBegin, DAfterBegin, DNextsArray, DNext, DPreviousesArray, DPrevious, DDominator,
         DId, DAddress, DLocalization, DZones, DConstraints, DEnd 
      };

#define DefineGoto(Target) case D##Target: goto L##Target;
   switch (state.point()) {
      DefineGoto(Begin)
      DefineGoto(AfterBegin)
      DefineGoto(NextsArray)
      DefineGoto(Next)
      DefineGoto(PreviousesArray)
      DefineGoto(Previous)
      DefineGoto(Dominator)
      DefineGoto(Id)
      DefineGoto(Address)
      DefineGoto(Localization)
      DefineGoto(Zones)
      DefineGoto(Constraints)
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
   while (!arguments.isClose()) {
      hasHit = false;
      while (arguments.isOpen()) {
         if (!Parser::skipNodeInLoop(state, arguments, result) || result == RRNeedChars) return result;
         if (!arguments.setToNextToken(result)) return result;
      };
      AssumeCondition(!arguments.isCloseArray())
      if (arguments.isAddKey()
            && ((result = arguments.setArgumentKey()), arguments.key() == "nexts")) {
         if (result == RRNeedChars) return result;
         state.point() = DNextsArray;
         if (!arguments.setToNextToken(result)) return result;
LNextsArray:
         if (!arguments.isOpenArray()) {
            if (!arguments.addErrorMessage(STG::SString("expected '['")))
               return result;
            state.point() = DAfterBegin;
            continue;
         }
         state.point() = DNext;
         if (!arguments.setToNextToken(result)) return result;
LNext:
         while (!arguments.isCloseArray()) {
            AssumeCondition(!arguments.isClose())
            while (arguments.isOpen()) {
               if (!Parser::skipNodeInLoop(state, arguments, result) || result == RRNeedChars) return result;
               if (!arguments.setToNextToken(result)) return result;
            };
            if (arguments.isSetValue()
                  && arguments.setArgumentToInt() == RRNeedChars) return RRNeedChars;
            if (arguments.isSetInt()) {
               auto& rule = state.getSResult((ReadRuleResult*) nullptr);
               lecNexts.insertNewAtEnd(new EdgeContract());
               rule.lookForContractId(arguments.valueAsInt(), lecNexts.getSLast());
            }
            else
               continue;
            if (!arguments.setToNextToken(result)) return result;
         }
         state.point() = DAfterBegin;
         if (!arguments.setToNextToken(result)) return result;
         hasHit = true;
      }
      else
         if (result == RRNeedChars) return result;

      if (arguments.isAddKey()
            && ((result = arguments.setArgumentKey()), arguments.key() == "previouses")) {
         if (result == RRNeedChars) return result;
         state.point() = DPreviousesArray;
         if (!arguments.setToNextToken(result)) return result;
LPreviousesArray:
         if (!arguments.isOpenArray()) {
            if (!arguments.addErrorMessage(STG::SString("expected '['")))
               return result;
            state.point() = DAfterBegin;
            continue;
         }
         state.point() = DPrevious;
         if (!arguments.setToNextToken(result)) return result;
LPrevious:
         while (!arguments.isCloseArray()) {
            AssumeCondition(!arguments.isClose())
            while (arguments.isOpen()) {
               if (!Parser::skipNodeInLoop(state, arguments, result) || result == RRNeedChars) return result;
               if (!arguments.setToNextToken(result)) return result;
            };
            if (arguments.isSetValue()
                  && arguments.setArgumentToInt() == RRNeedChars) return RRNeedChars;
            if (arguments.isSetInt()) {
               auto& rule = state.getSResult((ReadRuleResult*) nullptr);
               lecPreviouses.insertNewAtEnd(new EdgeContract());
               rule.lookForContractId(arguments.valueAsInt(), lecPreviouses.getSLast());
            }
            else
               continue;
            if (!arguments.setToNextToken(result)) return result;
         }
         state.point() = DAfterBegin;
         if (!arguments.setToNextToken(result)) return result;
         hasHit = true;
      }
      else
         if (result == RRNeedChars) return result;

      if (arguments.isAddKey()
            && ((result = arguments.setArgumentKey()), arguments.key() == "dominator")) {
         if (result == RRNeedChars) return result;
         state.point() = DDominator;
         if (!arguments.setToNextToken(result)) return result;
LDominator:
         if (arguments.isSetValue()
               && arguments.setArgumentToInt() == RRNeedChars) return RRNeedChars;
         if (arguments.isSetInt()) {
            auto& rule = state.getSResult((ReadRuleResult*) nullptr);
            rule.lookForContractId(arguments.valueAsInt(), cpDominator);
         }
         else {
            state.point() = DAfterBegin;
            continue;
         }
         state.point() = DAfterBegin;
         if (!arguments.setToNextToken(result)) return result;
         hasHit = true;
      }
      else
         if (result == RRNeedChars) return result;

      if (arguments.isAddKey()
            && ((result = arguments.setArgumentKey()), arguments.key() == "id")) {
         if (result == RRNeedChars) return result;
         state.point() = DId;
         if (!arguments.setToNextToken(result)) return result;
LId:
         if (arguments.isSetValue()
               && arguments.setArgumentToInt() == RRNeedChars) return RRNeedChars;
         if (arguments.isSetInt()) {
            uId = arguments.valueAsInt();
            auto& rule = state.getSResult((ReadRuleResult*) nullptr);
            rule.setContractId(*this);
         }
         else {
            state.point() = DAfterBegin;
            continue;
         }
         state.point() = DAfterBegin;
         if (!arguments.setToNextToken(result)) return result;
         hasHit = true;
      }
      else
         if (result == RRNeedChars) return result;

      if (arguments.isAddKey()
            && ((result = arguments.setArgumentKey()), arguments.key() == "address")) {
         if (result == RRNeedChars) return result;
         state.point() = DAddress;
         if (!arguments.setToNextToken(result)) return result;
LAddress:
         if (arguments.isSetValue()
               && arguments.setArgumentToLUInt() == RRNeedChars) return RRNeedChars;
         if (arguments.isSetLUInt())
            uAddress = arguments.valueAsLUInt();
         else {
            state.point() = DAfterBegin;
            continue;
         }
         state.point() = DAfterBegin;
         if (!arguments.setToNextToken(result)) return result;
         hasHit = true;
      }
      else
         if (result == RRNeedChars) return result;

      if (arguments.isAddKey()
            && ((result = arguments.setArgumentKey()), arguments.key() == "localization")) {
         if (result == RRNeedChars) return result;
         state.point() = DLocalization;
         if (!arguments.setToNextToken(result)) return result;
LLocalization:
         if (arguments.isSetString()) {
            if (arguments.setArgumentTextValue() == RRNeedChars) return RRNeedChars;
            if (!setLocalizationFromText(clLocalization, arguments.valueAsText())) {
               if (!arguments.addErrorMessage(STG::SString("invalid localization")))
                  return result;
            }
         }
         state.point() = DAfterBegin;
         if (!arguments.setToNextToken(result)) return result;
         hasHit = true;
      }
      else
         if (result == RRNeedChars) return result;

      if (arguments.isAddKey()
            && ((result = arguments.setArgumentKey()), arguments.key() == "zones")) {
         if (result == RRNeedChars) return result;
         {  auto ruleResult = state.getResult((ReadRuleResult*) nullptr);
            state.point() = DZones;
            arguments.shiftState(state, zmZoneModifier, &MemoryZoneModifier::readJSon,
                  (MemoryZoneModifier::ReadRuleResult*) nullptr);
            state.getSResult((MemoryZoneModifier::ReadRuleResult*) nullptr)
               = MemoryZoneModifier::ReadRuleResult(ruleResult.elementFunctions);
         }
         if (!arguments.setToNextToken(result)) return result;
         if (!arguments.parseTokens(state, result)) return result;
LZones:
         state.point() = DAfterBegin;
         hasHit = true;
      }
      else
         if (result == RRNeedChars) return result;

      if (arguments.isAddKey()
            && ((result = arguments.setArgumentKey()), arguments.key() == "constraints")) {
         if (result == RRNeedChars) return result;
         {  MemoryStateConstraint::ReadRuleResult ruleResult = state.getResult((ReadRuleResult*) nullptr);
            state.point() = DConstraints;
            arguments.shiftState(state, scMemoryConstraints, &MemoryStateConstraint::readJSon,
                  (MemoryStateConstraint::ReadRuleResult*) nullptr);
            state.getSResult((MemoryStateConstraint::ReadRuleResult*) nullptr)
                  = std::move(ruleResult);
         }
         if (!arguments.setToNextToken(result)) return result;
         if (!arguments.parseTokens(state, result)) return result;
LConstraints:
         state.point() = DAfterBegin;
         hasHit = true;
      }
      else
         if (result == RRNeedChars) return result;

      if (!hasHit)
         if (!arguments.setToNextToken(result)) return result;
   }
   state.point() = DEnd;
   if (!arguments.setToNextToken(result)) return result;
LEnd:
   if (!isValid()) {
      if (!arguments.addErrorMessage(STG::SString("contract is not complete"))) {
         arguments.reduceState(state);
         return result;
      }
   };
   arguments.reduceState(state);
   return RRHasToken;
}

STG::Lexer::Base::WriteResult
Contract::writeJSon(STG::JSon::CommonWriter::State& state,
      STG::JSon::CommonWriter::Arguments& arguments) const {
   WriteResult result = WRNeedEvent;
   ListEdgeContract::Cursor* edgeCursor = nullptr;

   enum Delimiters
      {  DBegin, DNexts, DNextsArray, DNext, DWriteNext, DPreviouses, DPreviousesArray,
         DPrevious, DWritePrevious, DDominator, DDominatorValue, DId, DIdValue, DAddress,
         DAddressValue, DLocalization, DLocalizationValue, DZones, DConstraints, DEnd
      };

#define DefineGoto(Target) case D##Target: goto L##Target;
   switch (state.point()) {
      DefineGoto(Begin)
      DefineGoto(Nexts)
      DefineGoto(NextsArray)
      DefineGoto(Next)
      DefineGoto(WriteNext)
      DefineGoto(Previouses)
      DefineGoto(PreviousesArray)
      DefineGoto(Previous)
      DefineGoto(WritePrevious)
      DefineGoto(Dominator)
      DefineGoto(DominatorValue)
      DefineGoto(Id)
      DefineGoto(IdValue)
      DefineGoto(Address)
      DefineGoto(AddressValue)
      DefineGoto(Localization)
      DefineGoto(LocalizationValue)
      DefineGoto(Zones)
      DefineGoto(Constraints)
      DefineGoto(End)
   };
#undef DefineGoto

LBegin:
   arguments.setOpenObject();
   ++state.point();
   if (!arguments.writeEvent(result)) return result;

LNexts:
   arguments.setAddKey(STG::SString("nexts"));
   ++state.point();
   if (!arguments.writeEvent(result)) return result;

LNextsArray:
   arguments.setOpenArray();
   state.getSResult((WriteRuleResult*) nullptr).edgeCursor.absorbElement(edgeCursor = new ListEdgeContract::Cursor(lecNexts));
   ++state.point();
   if (!arguments.writeEvent(result)) return result;

LNext:
   if (!edgeCursor)
      edgeCursor = &*state.getSResult((WriteRuleResult*) nullptr).edgeCursor;
   while (edgeCursor->setToNext()) {
      state.point() = DWriteNext;
LWriteNext:
      if (!edgeCursor->elementAt().isValid()) {
         state.point() = DNext;
         continue;
      }
      arguments.setIntValue(edgeCursor->elementAt()->getId());
      state.point() = DNext;
      if (!arguments.writeEvent(result)) return result;
      if (!edgeCursor)
         edgeCursor = &*state.getSResult((WriteRuleResult*) nullptr).edgeCursor;
   }

   arguments.setCloseArray();
   state.point() = DPreviouses;
   if (!arguments.writeEvent(result)) return result;

LPreviouses:
   arguments.setAddKey(STG::SString("nexts"));
   ++state.point();
   if (!arguments.writeEvent(result)) return result;

LPreviousesArray:
   arguments.setOpenArray();
   state.getSResult((WriteRuleResult*) nullptr).edgeCursor.absorbElement(edgeCursor = new ListEdgeContract::Cursor(lecPreviouses));
   ++state.point();
   if (!arguments.writeEvent(result)) return result;

LPrevious:
   if (!edgeCursor)
      edgeCursor = &*state.getSResult((WriteRuleResult*) nullptr).edgeCursor;
   while (edgeCursor->setToNext()) {
      state.point() = DWritePrevious;
LWritePrevious:
      if (!edgeCursor->elementAt().isValid()) {
         state.point() = DPrevious;
         continue;
      }
      arguments.setIntValue(edgeCursor->elementAt()->getId());
      state.point() = DPrevious;
      if (!arguments.writeEvent(result)) return result;
      if (!edgeCursor)
         edgeCursor = &*state.getSResult((WriteRuleResult*) nullptr).edgeCursor;
   }

   arguments.setCloseArray();
   state.point() = DDominator;
   if (!arguments.writeEvent(result)) return result;

LDominator:
   if (cpDominator.isValid()) {
      arguments.setAddKey(STG::SString("dominator"));
      ++state.point();
      if (!arguments.writeEvent(result)) return result;
LDominatorValue:
      arguments.setIntValue(cpDominator->getId());
      ++state.point();
      if (!arguments.writeEvent(result)) return result;
   }
   else
      state.point() = DId;

LId:
   arguments.setAddKey(STG::SString("id"));
   ++state.point();
   if (!arguments.writeEvent(result)) return result;
LIdValue:
   arguments.setIntValue(uId);
   ++state.point();
   if (!arguments.writeEvent(result)) return result;

LAddress:
   arguments.setAddKey(STG::SString("address"));
   ++state.point();
   if (!arguments.writeEvent(result)) return result;
LAddressValue:
   arguments.setLUIntValue(uAddress);
   ++state.point();
   if (!arguments.writeEvent(result)) return result;

LLocalization:
   arguments.setAddKey(STG::SString("localization"));
   ++state.point();
   if (!arguments.writeEvent(result)) return result;
LLocalizationValue:
   arguments.setStringValue(getTextFromLocalization(clLocalization));
   ++state.point();
   if (!arguments.writeEvent(result)) return result;

LZones:
   arguments.setAddKey(STG::SString("zones"));
   ++state.point();
   arguments.shiftState(state, zmZoneModifier, &MemoryZoneModifier::writeJSon,
         (MemoryZoneModifier::WriteRuleResult*) nullptr);
   state.getSResult((MemoryZoneModifier::WriteRuleResult*) nullptr)
      = MemoryZoneModifier::WriteRuleResult();
   if (!arguments.writeEvent(result)) return result;
   if (!arguments.writeTokens(state, result)) return result;

LConstraints:
   arguments.setAddKey(STG::SString("constraints"));
   {  MemoryStateConstraint::WriteRuleResult ruleResult = state.getResult((WriteRuleResult*) nullptr);
      ++state.point();
      arguments.shiftState(state, scMemoryConstraints, &MemoryStateConstraint::writeJSon,
            (MemoryStateConstraint::WriteRuleResult*) nullptr);
      state.getSResult((MemoryStateConstraint::WriteRuleResult*) nullptr)
         = std::move(ruleResult);
   }
   if (!arguments.writeEvent(result)) return result;
   if (!arguments.writeTokens(state, result)) return result;

LEnd:
   arguments.setCloseObject();
   arguments.reduceState(state);
   if (!arguments.writeEvent(result)) return result;
   return WRNeedEvent;
}

template class COL::TSortedArray<Contract::ContractPointer, Contract::ContractPointer::Key>;

void
Contract::applyOneTo(MemoryState& memoryState, struct _Processor* processor,
      struct _ProcessorFunctions* processorFunctions) {
   zmZoneModifier.apply(memoryState.memoryZones(), uAddress);
   scMemoryConstraints.apply(memoryState, uAddress, processor, processorFunctions);
}

void
Contract::applyTo(MemoryState& memoryState, struct _Processor* processor,
      struct _ProcessorFunctions* processorFunctions) {
   std::vector<Contract*> hierarchy;
   Contract* current = this;
   while (current->cpDominator.isValid()) {
      current = &*current->cpDominator;
      hierarchy.push_back(current);
   }
   for (auto iter = hierarchy.rbegin(); iter != hierarchy.rend(); ++iter)
      (*iter)->applyOneTo(memoryState, processor, processorFunctions);
   applyOneTo(memoryState, processor, processorFunctions);
}

STG::Lexer::Base::ReadResult
ContractGraph::readJSon(STG::JSon::CommonParser::State& state, STG::JSon::CommonParser::Arguments& arguments) {
   typedef STG::JSon::CommonParser Parser;
   ReadResult result = RRContinue;
   bool isEqual;

   enum Delimiters { DBegin, DContracts, DAllocShift, DContractsContent, DContract, DEnd };

   #define DefineGoto(Target) case D##Target: goto L##Target;
   switch (state.point()) {
      DefineGoto(Begin)
      DefineGoto(Contracts)
      DefineGoto(AllocShift)
      DefineGoto(ContractsContent)
      DefineGoto(Contract)
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

LContracts:
   while (!arguments.isCloseObject()) {
      while (arguments.isOpenObject()) {
         if (!Parser::skipNodeInLoop(state, arguments, result) || result == RRNeedChars) return result;
         if (!arguments.setToNextToken(result)) return result;
      };
      if (arguments.isCloseArray())
         {  AssumeUncalled }

      if (arguments.isAddKey()) {
         isEqual = true;
         static STG::SubString allocShiftString = STG::SString("alloc-shift");
         result = arguments.isEqualKeyValue(isEqual, allocShiftString);
         if (result == RRNeedChars) return result;
         if (isEqual) {
            if (result == RRNeedChars) return result;
            state.point() = DAllocShift;
            if (!arguments.setToNextToken(result)) return result;
LAllocShift:
            if (arguments.isSetValue()
                  && arguments.setArgumentToLUInt() == RRNeedChars) return RRNeedChars;
            if (arguments.isSetLUInt())
               uAllocShift = arguments.valueAsLUInt();
            else {
               state.point() = DContracts;
               continue;
            }
         };
      }
      else if (arguments.isOpenArray()) {
         state.point() = DContractsContent;
         if (!arguments.setToNextToken(result)) return result;

LContractsContent:
         while (!arguments.isCloseArray()) {
            while (arguments.isOpenArray()) {
               if (!Parser::skipNodeInLoop(state, arguments, result) || result == RRNeedChars) return result;
               if (!arguments.setToNextToken(result)) return result;
            };
            if (arguments.isCloseObject())
               {  AssumeUncalled }

            if (arguments.isOpenObject()) {
               ++state.point();
               {  auto& ruleResult = state.getSResult((ReadRuleResult*) nullptr);
                  ruleResult.currentContract.absorbElement(new Contract());
                  Contract::ReadRuleResult subruleResult(*ruleResult.idMap, *ruleResult.pendingIds,
                           ruleResult);
                  arguments.shiftState(state, *ruleResult.currentContract, &Contract::readJSon,
                        (Contract::ReadRuleResult*) nullptr);
                  state.setResult(std::move(subruleResult));
               }
               if (!arguments.parseTokens(state, result)) return result;
LContract:
               {  auto& ruleResult = state.getSResult((ReadRuleResult*) nullptr);
                  insertNewAtEnd(new ContractPointer(ruleResult.currentContract.extractElement(),
                           PNT::Pointer::Init()));
               }
               state.point() = DContractsContent;
               continue;
            };
            if (!arguments.setToNextToken(result)) return result;
         }
      }
      state.point() = DContracts;
      if (!arguments.setToNextToken(result)) return result;
   }
   state.point() = DEnd;
LEnd:
   arguments.reduceState(state);
   return RRHasToken;
}

STG::Lexer::Base::WriteResult
ContractGraph::writeJSon(STG::JSon::CommonWriter::State& state, STG::JSon::CommonWriter::Arguments& arguments) const {
   WriteResult result = WRNeedEvent;
   enum Delimiters
      {  DBegin, DWriteContracts, DWriteContract, DAfterWriteContract, DAfterWriteContracts,
         DEnd
      };
   Cursor* cursor = nullptr;

#define DefineGoto(Target) case D##Target: goto L##Target;
   switch (state.point()) {
      DefineGoto(Begin)
      DefineGoto(WriteContracts)
      DefineGoto(WriteContract)
      DefineGoto(AfterWriteContract)
      DefineGoto(AfterWriteContracts)
      DefineGoto(End)
   };
#undef DefineGoto

LBegin:
   arguments.setOpenObject();
   ++state.point();
   if (!arguments.writeEvent(result)) return result;

LWriteContracts:
   arguments.setOpenArray();
   state.getSResult((WriteRuleResult*) nullptr).cursor.absorbElement(new Cursor(*this));
   ++state.point();
   if (!arguments.writeEvent(result)) return result;

LWriteContract:
   cursor = &*state.getSResult((WriteRuleResult*) nullptr).cursor;
   while (cursor->setToNext()) {
      ++state.point();
      {  Contract::WriteRuleResult ruleResult;
         ruleResult.setFrom(state.getResult((WriteRuleResult*) nullptr));
         arguments.shiftState(state, *cursor->elementSAt(), &Contract::writeJSon,
               (Contract::WriteRuleResult*) nullptr);
         state.getSResult((Contract::WriteRuleResult*) nullptr) = std::move(ruleResult);
      }
      if (!arguments.writeEvent(result)) return result;
      if (!arguments.writeTokens(state, result)) return result;

LAfterWriteContract:
      state.point() = DWriteContract;
      if (!cursor)
         cursor = &*state.getSResult((WriteRuleResult*) nullptr).cursor;
   }

LAfterWriteContracts:
   arguments.setCloseArray();
   ++state.point();
   if (!arguments.writeEvent(result)) return result;

LEnd:
   arguments.setCloseObject();
   arguments.reduceState(state);
   if (!arguments.writeEvent(result)) return result;
   return WRNeedEvent;
}

