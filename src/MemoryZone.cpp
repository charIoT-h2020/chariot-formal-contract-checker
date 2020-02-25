#include "MemoryZone.h"
#include "Collection/Collection.template"

STG::Lexer::Base::ReadResult
MemoryZoneCreate::readJSon(STG::JSon::CommonParser::State& state,
      STG::JSon::CommonParser::Arguments& arguments) {
   typedef STG::JSon::CommonParser Parser;
   ReadResult result = RRContinue;
   bool hasHit = false;

   enum Delimiters { DBegin, DAfterBegin, DAfterStart, DAfterLength, DName, DEnd };

   #define DefineGoto(Target) case D##Target: goto L##Target;
   switch (state.point()) {
      DefineGoto(Begin)
      DefineGoto(AfterBegin)
      DefineGoto(AfterStart)
      DefineGoto(AfterLength)
      DefineGoto(Name)
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
      while (arguments.isOpen()) {
         if (!Parser::skipNodeInLoop(state, arguments, result) || result == RRNeedChars) return result;
         if (!arguments.setToNextToken(result)) return result;
      };
      AssumeCondition(!arguments.isCloseArray())
      hasHit = false;
      if (arguments.isAddKey()
            && ((result = arguments.setArgumentKey()), arguments.key() == "start")) {
         if (result == RRNeedChars) return result;
         {  auto ruleResult = state.getResult((VirtualExpressionNode::RuleResult*) nullptr);
            state.point() = DAfterStart;
            arguments.shiftState(state, eStartAddress, &Expression::readJSon,
                  (VirtualExpressionNode::RuleResult*) nullptr);
            state.setResult(std::move(ruleResult));
         }
         if (!arguments.setToNextToken(result)) return result;
         if (!arguments.parseTokens(state, result)) return result;
LAfterStart:
         state.point() = DAfterBegin;
         if (!arguments.setToNextToken(result)) return result;
         hasHit = true;
      }
      else
         if (result == RRNeedChars) return result;

      if (arguments.isAddKey()
            && ((result = arguments.setArgumentKey()), arguments.key() == "length")) {
         if (result == RRNeedChars) return result;
         state.point() = DAfterLength;
         {  auto ruleResult = state.getResult((VirtualExpressionNode::RuleResult*) nullptr);
            arguments.shiftState(state, eLength, &Expression::readJSon,
                  (VirtualExpressionNode::RuleResult*) nullptr);
            state.setResult(std::move(ruleResult));
         }
         if (!arguments.setToNextToken(result)) return result;
         if (!arguments.parseTokens(state, result)) return result;
LAfterLength:
         state.point() = DAfterBegin;
         if (!arguments.setToNextToken(result)) return result;
         hasHit = true;
      }
      else
         if (result == RRNeedChars) return result;

      if (arguments.isAddKey()
            && ((result = arguments.setArgumentKey()), arguments.key() == "name")) {
         if (result == RRNeedChars) return result;
         state.point() = DName;
         if (!arguments.setToNextToken(result)) return result;
LName:
         if (arguments.isSetString()) {
            if (arguments.setArgumentTextValue() == RRNeedChars) return RRNeedChars;
            ssName = arguments.valueAsText();
         }
         state.point() = DAfterBegin;
         if (!arguments.setToNextToken(result)) return result;
         hasHit = true;
      }
      else
         if (result == RRNeedChars) return result;
      state.point() = DAfterBegin;
      if (!hasHit)
         if (!arguments.setToNextToken(result)) return result;
   }
   state.point() = DEnd;
LEnd:
   arguments.reduceState(state);
   return RRHasToken;
}

STG::Lexer::Base::WriteResult
MemoryZoneCreate::writeJSon(STG::JSon::CommonWriter::State& state,
      STG::JSon::CommonWriter::Arguments& arguments) const {
   WriteResult result = WRNeedEvent;
   enum Delimiters { DBegin, DStart, DLength, DBeforeName, DName, DEnd };

   #define DefineGoto(Target) case D##Target: goto L##Target;
   switch (state.point()) {
      DefineGoto(Begin)
      DefineGoto(Start)
      DefineGoto(Length)
      DefineGoto(BeforeName)
      DefineGoto(Name)
      DefineGoto(End)
   }

LBegin:
   arguments.setOpenObject();
   ++state.point();
   if (!arguments.writeEvent(result)) return result;

LStart:
   arguments.setAddKey(STG::SString("start"));
   ++state.point();
   arguments.shiftState(state, eStartAddress, &Expression::writeJSon,
         (char*) nullptr);
   if (!arguments.writeEvent(result)) return result;
   if (!arguments.writeTokens(state, result)) return result;

LLength:
   arguments.setAddKey(STG::SString("length"));
   ++state.point();
   arguments.shiftState(state, eLength, &Expression::writeJSon,
         (char*) nullptr);
   if (!arguments.writeEvent(result)) return result;
   if (!arguments.writeTokens(state, result)) return result;

LBeforeName:
   arguments.setAddKey(STG::SString("name"));
   ++state.point();
   if (!arguments.writeEvent(result)) return result;

LName:
   arguments.setStringValue(ssName);
   ++state.point();
   if (!arguments.writeEvent(result)) return result;

LEnd:
   arguments.setCloseObject();
   arguments.reduceState(state);
   if (!arguments.writeEvent(result)) return result;
   return WRNeedEvent;
}

STG::Lexer::Base::ReadResult
MemoryZoneRename::readJSon(STG::JSon::CommonParser::State& state,
      STG::JSon::CommonParser::Arguments& arguments) {
   typedef STG::JSon::CommonParser Parser;
   ReadResult result = RRContinue;

   enum Delimiters { DBegin, DAfterBegin, DOldName, DNewName, DEnd };

   #define DefineGoto(Target) case D##Target: goto L##Target;
   switch (state.point()) {
      DefineGoto(Begin)
      DefineGoto(AfterBegin)
      DefineGoto(OldName)
      DefineGoto(NewName)
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
      while (arguments.isOpen()) {
         if (!Parser::skipNodeInLoop(state, arguments, result) || result == RRNeedChars) return result;
         if (!arguments.setToNextToken(result)) return result;
      };
      AssumeCondition(!arguments.isCloseArray())
      if (arguments.isAddKey()
            && ((result = arguments.setArgumentKey()), arguments.key() == "old_name")) {
         if (result == RRNeedChars) return result;
         state.point() = DOldName;
         if (!arguments.setToNextToken(result)) return result;
LOldName:
         if (arguments.isSetString()) {
            if (arguments.setArgumentTextValue() == RRNeedChars) return RRNeedChars;
            ssOldName = arguments.valueAsText();
         }
         state.point() = DAfterBegin;
      }
      else
         if (result == RRNeedChars) return result;

      if (arguments.isAddKey()
            && ((result = arguments.setArgumentKey()), arguments.key() == "new_name")) {
         if (result == RRNeedChars) return result;
         state.point() = DNewName;
         if (!arguments.setToNextToken(result)) return result;
LNewName:
         if (arguments.isSetString()) {
            if (arguments.setArgumentTextValue() == RRNeedChars) return RRNeedChars;
            ssNewName = arguments.valueAsText();
         }
         state.point() = DAfterBegin;
      }
      else
         if (result == RRNeedChars) return result;

      if (!arguments.setToNextToken(result)) return result;
   }
   state.point() = DEnd;
LEnd:
   arguments.reduceState(state);
   return RRHasToken;
}

STG::Lexer::Base::WriteResult
MemoryZoneRename::writeJSon(STG::JSon::CommonWriter::State& state,
      STG::JSon::CommonWriter::Arguments& arguments) const {
   WriteResult result = WRNeedEvent;
   enum Delimiters { DBegin, DBeforeOldName, DOldName, DBeforeNewName, DNewName, DEnd };

   #define DefineGoto(Target) case D##Target: goto L##Target;
   switch (state.point()) {
      DefineGoto(Begin)
      DefineGoto(BeforeOldName)
      DefineGoto(OldName)
      DefineGoto(BeforeNewName)
      DefineGoto(NewName)
      DefineGoto(End)
   }

LBegin:
   arguments.setOpenObject();
   ++state.point();
   if (!arguments.writeEvent(result)) return result;

LBeforeOldName:
   arguments.setAddKey(STG::SString("old_name"));
   ++state.point();
   if (!arguments.writeEvent(result)) return result;

LOldName:
   arguments.setStringValue(ssOldName);
   ++state.point();
   if (!arguments.writeEvent(result)) return result;

LBeforeNewName:
   arguments.setAddKey(STG::SString("new_name"));
   ++state.point();
   if (!arguments.writeEvent(result)) return result;

LNewName:
   arguments.setStringValue(ssNewName);
   ++state.point();
   if (!arguments.writeEvent(result)) return result;

LEnd:
   arguments.setCloseObject();
   arguments.reduceState(state);
   if (!arguments.writeEvent(result)) return result;
   return WRNeedEvent;
}

STG::Lexer::Base::ReadResult
MemoryZoneSplit::readJSon(STG::JSon::CommonParser::State& state,
      STG::JSon::CommonParser::Arguments& arguments) {
   typedef STG::JSon::CommonParser Parser;
   ReadResult result = RRContinue;
   bool hasHit = false;

   enum Delimiters { DBegin, DAfterBegin, DAfterStart, DOldName, DNewName, DEnd };

   #define DefineGoto(Target) case D##Target: goto L##Target;
   switch (state.point()) {
      DefineGoto(Begin)
      DefineGoto(AfterBegin)
      DefineGoto(AfterStart)
      DefineGoto(OldName)
      DefineGoto(NewName)
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
      while (arguments.isOpen()) {
         if (!Parser::skipNodeInLoop(state, arguments, result) || result == RRNeedChars) return result;
         if (!arguments.setToNextToken(result)) return result;
      };
      AssumeCondition(!arguments.isCloseArray())
      hasHit = false;
      if (arguments.isAddKey()
            && ((result = arguments.setArgumentKey()), arguments.key() == "start")) {
         if (result == RRNeedChars) return result;
         {  auto ruleResult = state.getResult((VirtualExpressionNode::RuleResult*) nullptr);
            state.point() = DAfterStart;
            arguments.shiftState(state, eNewStartAddress, &Expression::readJSon,
                  (VirtualExpressionNode::RuleResult*) nullptr);
            state.setResult(std::move(ruleResult));
         }
         if (!arguments.setToNextToken(result)) return result;
         if (!arguments.parseTokens(state, result)) return result;
LAfterStart:
         state.point() = DAfterBegin;
         if (!arguments.setToNextToken(result)) return result;
         hasHit = true;
      }
      else
         if (result == RRNeedChars) return result;

      if (arguments.isAddKey()
            && ((result = arguments.setArgumentKey()), arguments.key() == "old_name")) {
         if (result == RRNeedChars) return result;
         state.point() = DOldName;
         if (!arguments.setToNextToken(result)) return result;
LOldName:
         if (arguments.isSetString()) {
            if (arguments.setArgumentTextValue() == RRNeedChars) return RRNeedChars;
            ssOldName = arguments.valueAsText();
         }
         state.point() = DAfterBegin;
         if (!arguments.setToNextToken(result)) return result;
         hasHit = true;
      }
      else
         if (result == RRNeedChars) return result;

      if (arguments.isAddKey()
            && ((result = arguments.setArgumentKey()), arguments.key() == "new_name")) {
         if (result == RRNeedChars) return result;
         state.point() = DNewName;
         if (!arguments.setToNextToken(result)) return result;
LNewName:
         if (arguments.isSetString()) {
            if (arguments.setArgumentTextValue() == RRNeedChars) return RRNeedChars;
            ssNewName = arguments.valueAsText();
         }
         state.point() = DAfterBegin;
         if (!arguments.setToNextToken(result)) return result;
         hasHit = true;
      }
      else
         if (result == RRNeedChars) return result;

      if (!hasHit)
         if (!arguments.setToNextToken(result)) return result;
   }
   state.point() = DEnd;
LEnd:
   arguments.reduceState(state);
   return RRHasToken;
}

STG::Lexer::Base::WriteResult
MemoryZoneSplit::writeJSon(STG::JSon::CommonWriter::State& state,
      STG::JSon::CommonWriter::Arguments& arguments) const {
   WriteResult result = WRNeedEvent;
   enum Delimiters { DBegin, DStart, DBeforeOldName, DOldName, DBeforeNewName, DNewName, DEnd };

   #define DefineGoto(Target) case D##Target: goto L##Target;
   switch (state.point()) {
      DefineGoto(Begin)
      DefineGoto(Start)
      DefineGoto(BeforeOldName)
      DefineGoto(OldName)
      DefineGoto(BeforeNewName)
      DefineGoto(NewName)
      DefineGoto(End)
   }

LBegin:
   arguments.setOpenObject();
   ++state.point();
   if (!arguments.writeEvent(result)) return result;

LStart:
   arguments.setAddKey(STG::SString("start"));
   ++state.point();
   arguments.shiftState(state, eNewStartAddress, &Expression::writeJSon,
         (char*) nullptr);
   if (!arguments.writeEvent(result)) return result;
   if (!arguments.writeTokens(state, result)) return result;

LBeforeOldName:
   arguments.setAddKey(STG::SString("old_name"));
   ++state.point();
   if (!arguments.writeEvent(result)) return result;

LOldName:
   arguments.setStringValue(ssOldName);
   ++state.point();
   if (!arguments.writeEvent(result)) return result;

LBeforeNewName:
   arguments.setAddKey(STG::SString("new_name"));
   ++state.point();
   if (!arguments.writeEvent(result)) return result;

LNewName:
   arguments.setStringValue(ssNewName);
   ++state.point();
   if (!arguments.writeEvent(result)) return result;

LEnd:
   arguments.setCloseObject();
   arguments.reduceState(state);
   if (!arguments.writeEvent(result)) return result;
   return WRNeedEvent;
}

STG::Lexer::Base::ReadResult
MemoryZoneMerge::readJSon(STG::JSon::CommonParser::State& state,
      STG::JSon::CommonParser::Arguments& arguments) {
   typedef STG::JSon::CommonParser Parser;
   ReadResult result = RRContinue;

   enum Delimiters { DBegin, DAfterBegin, DFirstName, DSecondName, DEnd };

   #define DefineGoto(Target) case D##Target: goto L##Target;
   switch (state.point()) {
      DefineGoto(Begin)
      DefineGoto(AfterBegin)
      DefineGoto(FirstName)
      DefineGoto(SecondName)
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
      while (arguments.isOpen()) {
         if (!Parser::skipNodeInLoop(state, arguments, result) || result == RRNeedChars) return result;
         if (!arguments.setToNextToken(result)) return result;
      };
      AssumeCondition(!arguments.isCloseArray())
      if (arguments.isAddKey()
            && ((result = arguments.setArgumentKey()), arguments.key() == "old_name")) {
         if (result == RRNeedChars) return result;
         state.point() = DFirstName;
         if (!arguments.setToNextToken(result)) return result;
LFirstName:
         if (arguments.isSetString()) {
            if (arguments.setArgumentTextValue() == RRNeedChars) return RRNeedChars;
            ssFirstName = arguments.valueAsText();
         }
         state.point() = DAfterBegin;
      }
      else
         if (result == RRNeedChars) return result;

      if (arguments.isAddKey()
            && ((result = arguments.setArgumentKey()), arguments.key() == "new_name")) {
         if (result == RRNeedChars) return result;
         state.point() = DSecondName;
         if (!arguments.setToNextToken(result)) return result;
LSecondName:
         if (arguments.isSetString()) {
            if (arguments.setArgumentTextValue() == RRNeedChars) return RRNeedChars;
            ssSecondName = arguments.valueAsText();
         }
         state.point() = DAfterBegin;
      }
      else
         if (result == RRNeedChars) return result;

      if (!arguments.setToNextToken(result)) return result;
   }
   state.point() = DEnd;
LEnd:
   arguments.reduceState(state);
   return RRHasToken;
}

STG::Lexer::Base::WriteResult
MemoryZoneMerge::writeJSon(STG::JSon::CommonWriter::State& state,
      STG::JSon::CommonWriter::Arguments& arguments) const {
   WriteResult result = WRNeedEvent;
   enum Delimiters { DBegin, DBeforeFirstName, DFirstName, DBeforeSecondName, DSecondName, DEnd };

   #define DefineGoto(Target) case D##Target: goto L##Target;
   switch (state.point()) {
      DefineGoto(Begin)
      DefineGoto(BeforeFirstName)
      DefineGoto(FirstName)
      DefineGoto(BeforeSecondName)
      DefineGoto(SecondName)
      DefineGoto(End)
   }

LBegin:
   arguments.setOpenObject();
   ++state.point();
   if (!arguments.writeEvent(result)) return result;

LBeforeFirstName:
   arguments.setAddKey(STG::SString("old_name"));
   ++state.point();
   if (!arguments.writeEvent(result)) return result;

LFirstName:
   arguments.setStringValue(ssFirstName);
   ++state.point();
   if (!arguments.writeEvent(result)) return result;

LBeforeSecondName:
   arguments.setAddKey(STG::SString("new_name"));
   ++state.point();
   if (!arguments.writeEvent(result)) return result;

LSecondName:
   arguments.setStringValue(ssSecondName);
   ++state.point();
   if (!arguments.writeEvent(result)) return result;

LEnd:
   arguments.setCloseObject();
   arguments.reduceState(state);
   if (!arguments.writeEvent(result)) return result;
   return WRNeedEvent;
}

void
MemoryZoneMerge::apply(MemoryZones& zones, uint64_t startAddress) {
   MemoryZones::Cursor cursor(zones), otherCursor(zones);
   bool hasFirst = false, hasSecond = false;
   while (cursor.setToNext() && (!hasFirst || !hasSecond)) {
      auto& zone = cursor.elementSAt();
      if (!hasFirst && zone.getName() == ssFirstName) {
         hasFirst = true;
         if (hasSecond) {
            cursor.elementSAt().mergeWith(std::move(otherCursor.elementSAt()));
            zones.freeAt(otherCursor);
            return;
         }
         else
            otherCursor = cursor;
      }
      else if (!hasSecond && zone.getName() == ssSecondName) {
         hasSecond = true;
         if (hasFirst) {
            otherCursor.elementSAt().mergeWith(std::move(cursor.elementSAt()));
            zones.freeAt(cursor);
            return;
         }
         else
            otherCursor = cursor;
      }
   };
}

STG::Lexer::Base::ReadResult
MemoryZoneModifier::readJSon(STG::JSon::CommonParser::State& state,
      STG::JSon::CommonParser::Arguments& arguments) {
   typedef STG::JSon::CommonParser Parser;
   ReadResult result = RRContinue;

   enum Delimiters { DBegin, DAfterBegin, DMemoryZone, DIdentifyContent, DEnd };

   #define DefineGoto(Target) case D##Target: goto L##Target;
   switch (state.point()) {
      DefineGoto(Begin)
      DefineGoto(AfterBegin)
      DefineGoto(MemoryZone)
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

LMemoryZone:
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
                  setTypeActionFromText(state.getSResult((ReadRuleResult*) nullptr).type,
                     arguments.valueAsText());
               }
               state.point() = DMemoryZone;
               if (!arguments.setToNextToken(result)) return result;
            }
            else
               if (result == RRNeedChars) return result;
            if (arguments.isAddKey()
                  && ((result = arguments.setArgumentKey()), arguments.key() == "content")) {
               if (result == RRNeedChars) return result;
               state.point() = DMemoryZone;
               const auto& ruleResult = state.getResult((ReadRuleResult*) nullptr);
               azaActions.insertNewAtEnd(newZoneAction(ruleResult.type).extractElement());
               MemoryZoneAction::RuleResult subruleResult(ruleResult.functions);
               arguments.shiftState(state, azaActions.getSLast(), &MemoryZoneAction::readJSon,
                     (MemoryZoneAction::RuleResult*) nullptr);
               state.setResult(std::move(subruleResult));
               if (!arguments.setToNextToken(result)) return result;
               if (!arguments.parseTokens(state, result)) return result;
            }
            else
               if (result == RRNeedChars) return result;
            state.point() = DMemoryZone;
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
MemoryZoneModifier::writeJSon(STG::JSon::CommonWriter::State& state, STG::JSon::CommonWriter::Arguments& arguments) const {
   WriteResult result = WRNeedEvent;
   enum Delimiters
      {  DBegin, DAfterBegin, DWriteZoneAction, DWriteActionType, DIdentifyType,
         DWriteContent, DEndContent, DEnd
      };
   MemoryZoneActions::Cursor* cursor = nullptr;

   #define DefineGoto(Target) case D##Target: goto L##Target;
   switch (state.point()) {
      DefineGoto(Begin)
      DefineGoto(AfterBegin)
      DefineGoto(WriteZoneAction)
      DefineGoto(WriteActionType)
      DefineGoto(IdentifyType)
      DefineGoto(WriteContent)
      DefineGoto(EndContent)
      DefineGoto(End)
   };
   #undef DefineGoto

LBegin:
   arguments.setOpenArray();
   state.getSResult((WriteRuleResult*) nullptr).absorbElement(new MemoryZoneActions::Cursor(azaActions));
   ++state.point();
   if (!arguments.writeEvent(result)) return result;

LAfterBegin:
   cursor = &*state.getSResult((WriteRuleResult*) nullptr);
   while (cursor->setToNext()) {
      state.point() = DWriteZoneAction;

LWriteZoneAction:
      arguments.setOpenObject();
      ++state.point();
      if (!arguments.writeEvent(result)) return result;

LWriteActionType:
      arguments.setAddKey(STG::SString("type"));
      ++state.point();
      if (!arguments.writeEvent(result)) return result;

LIdentifyType:
      if (!cursor)
         cursor = &*state.getSResult((WriteRuleResult*) nullptr);
      arguments.setStringValue(getTextFromTypeAction(cursor->elementAt().getType()));
      ++state.point();
      if (!arguments.writeEvent(result)) return result;

LWriteContent:
      arguments.setAddKey(STG::SString("content"));
      ++state.point();
      if (!cursor)
         cursor = &*state.getSResult((WriteRuleResult*) nullptr);
      arguments.shiftState(state, cursor->elementSAt(), &MemoryZoneAction::writeJSon,
            (char*) nullptr);
      if (!arguments.writeEvent(result)) return result;
      if (!arguments.writeTokens(state, result)) return result;

LEndContent:
      state.point() = DAfterBegin;
      if (!cursor)
         cursor = &*state.getSResult((WriteRuleResult*) nullptr);
   }

LEnd:
   arguments.setCloseArray();
   arguments.reduceState(state);
   if (!arguments.writeEvent(result)) return result;
   return WRNeedEvent;
}

template class COL::TSortedArray<MemoryZone, MemoryZone::Key, HandlerCopyCast<MemoryZone> >;

