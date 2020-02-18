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
// File      : SString.h
// Description :
//   Definition of the class SString that defines full strings.
//

#ifndef STG_SStringH
#define STG_SStringH

#include <fstream>
#ifdef __GNUC__
#include <ctype.h>
#include <string.h>
#endif
#include "TString/SubString.h"
#include "Collection/ConcreteCollection/BasicList.h"

namespace STG {

/* Definition of the persistent classes */

namespace DIOObject {

typedef DSubString::PersistencyAccess PersistencyAccess;

class IFStream : public STG::IOObject::ISBase {
  public:
   typedef STG::IOObject::ISBase ISBase;

  private:
   std::ifstream fsIn;

  public:
   IFStream(const char* name) : fsIn(name) {}
   IFStream(const char* name, std::ios_base::openmode mode) : fsIn(name, mode) {}
   IFStream(const IFStream& source) = delete;

   bool isValid() const { return true; }
   virtual bool isStreamBased() const { return true; }
   std::ifstream& inputStream() { return fsIn; }

   #define DefineDeclareIMethods
   #include "StandardClasses/IOStreamMethods.inch"
   #undef DefineDeclareIMethods
};

template <class TypeSubString>
class TISSubString : public STG::IOObject::ISBase {
  public:
   typedef STG::IOObject::ISBase ISBase;

  private:
   typedef TISSubString<TypeSubString> thisType;
   TypeSubString ssIn;
   int iLastRead;

  public:
   TISSubString(const TypeSubString& source) : ssIn(source) {}
   TISSubString(const thisType& source) : ssIn(source.ssIn), iLastRead(0) {}
   thisType& operator=(const thisType& source)
      { ssIn = source.ssIn; iLastRead = 0; return *this; }

   virtual bool isStringBased() const { return true; }
   TypeSubString& buffer() { return ssIn; }
   int getBufferLength() const { return ssIn.length(); }
   #define DefineDeclareIMethods
   #include "StandardClasses/IOStreamMethods.inch"
   #undef DefineDeclareIMethods
};

typedef TISSubString<SubString> ISSubString;

class OFStream : public STG::IOObject::OSBase {
  public:
   typedef STG::IOObject::OSBase OSBase;

  private:
   std::ofstream ofsOut;

  public:
   OFStream(const char* fileName) : ofsOut(fileName) {}
   OFStream(const char* fileName, std::ios_base::openmode mode) : ofsOut(fileName, mode) {}
   OFStream(const OFStream& source) = delete;

   bool isValid() const { return true; }
   void close() { ofsOut.close(); }
   virtual bool isStreamBased() const { return true; }

   #define DefineDeclareOMethods
   #include "StandardClasses/IOStreamMethods.inch"
   #undef DefineDeclareOMethods
};

class FStream : public STG::IOObject::OSBase {
  public:
   typedef STG::IOObject::OSBase OSBase;

  private:
   std::fstream fsOut;

  public:
   FStream(const char* fileName) : fsOut(fileName) {}
   FStream(const FStream& source) = delete;

   virtual bool isStreamBased() const { return true; }
   bool isValid() const { return true; }
   void close() { fsOut.close(); }

   #define DefineDeclareOMethods
   #include "StandardClasses/IOStreamMethods.inch"
   #undef DefineDeclareOMethods
};

template <class TypeSubString>
class TOSSubString : public STG::IOObject::OSBase, public TypeSubString {
  public:
   typedef STG::IOObject::OSBase OSBase;

  private:
   typedef TOSSubString<TypeSubString> thisType;
   typedef OSBase inherited;
   typedef TypeSubString inheritedImplementation;
   
  public:
   TOSSubString() : inheritedImplementation(typename TypeSubString::String()) {}
   TOSSubString(const thisType& source) = default;
   thisType& operator=(const thisType& source)
      {  inheritedImplementation::operator=(source); return *this; }
   StaticInheritConversions(thisType, inheritedImplementation)

   TypeSubString& buffer() { return *this; }
   const char* stringBuffer() const { return inheritedImplementation::getChunk().string; }
   int lengthBuffer() const { return inheritedImplementation::length(); }
   virtual bool isStringBased() const { return true; }

   #define DefineDeclareOMethods
   #include "StandardClasses/IOStreamMethods.inch"
   #undef DefineDeclareOMethods
   virtual void assumeAlloc(int incSize);
};

template <class TypeSubString>
class TOSRSubString : public STG::IOObject::OSBase {
  public:
   typedef STG::IOObject::OSBase OSBase;

  private:
   typedef TOSRSubString<TypeSubString> thisType;
   typedef OSBase inherited;
   TypeSubString* pssReference;
   
  public:
   TOSRSubString(TypeSubString& source) : pssReference(&source) {}
   TOSRSubString(const thisType& source) = default;
   thisType& operator=(const thisType& source) { pssReference = source.pssReference; return *this; }

   TypeSubString& buffer() { return *pssReference; }
   const char* stringBuffer() const { return pssReference->getChunk().string; }
   int lengthBuffer() const { return pssReference->length(); }
   virtual bool isStringBased() const { return true; }

   #define DefineDeclareOMethods
   #include "StandardClasses/IOStreamMethods.inch"
   #undef DefineDeclareOMethods
   virtual void assumeAlloc(int incSize);
};
typedef TOSSubString<SubString> OSSubString;
typedef TOSRSubString<SubString> OSRSubString;

template <class TypeBase>
class TIOObject : public TypeBase {
  public:
   typedef IOObject::FormatParameters FormatParameters;
   typedef IOObject::ISBase ISBase;
   typedef IOObject::ISStream ISStream;
   typedef IOObject::OSBase OSBase;
   typedef IOObject::OSStream OSStream;
   typedef DIOObject::IFStream IFStream;
   typedef DIOObject::ISSubString ISSubString;
   typedef DIOObject::OFStream OFStream;
   typedef DIOObject::FStream FStream;
   typedef DIOObject::OSSubString OSSubString;
   typedef DIOObject::OSRSubString OSRSubString;

   // input/output methods
   void read(ISBase& in, const FormatParameters& params)
      {  this->_read(in, params); }
   void read(std::istream& in, const FormatParameters& params)
      {  ISStream instream(in); this->_read(instream, params); }
   void read(const SubString& in, const FormatParameters& params)
      {  ISSubString instream(in); this->_read(instream, params); }
   void read(SubString& in, const FormatParameters& params)
      {  ISSubString instream(in); this->_read(instream, params);
         in.setToEnd(instream.buffer());
      }
   void read(std::istream& in)
      {  ISStream instream(in); this->_read(instream, FormatParameters()); }
   void read(const SubString& in)
      {  ISSubString instream(in); this->_read(instream, FormatParameters()); }
   void read(SubString& in)
      {  ISSubString instream(in); this->_read(instream, FormatParameters());
         in.setToEnd(instream.buffer());
      }

   void write(OSBase& out, const FormatParameters& params) const
      {  this->_write(out, params); }
   void write(std::ostream& out, const FormatParameters& params) const
      {  OSStream outstream(out); this->_write(outstream, params); }
   void write(SubString& out, const FormatParameters& params) const
      {  if (out.isUpperInfinite()) {
            OSRSubString outstream(out);
            this->_write(outstream, params);
         }
         else {
            OSSubString outstream;
            this->_write(outstream, params); out.cat(outstream.buffer());
         };
      }
   void write(std::ostream& out) const
      {  OSStream outstream(out); this->_write(outstream, IOObject::FormatParameters()); }
   void write(SubString& out) const
      {  if (out.isUpperInfinite()) {
            OSRSubString outstream(out);
            this->_write(outstream, IOObject::FormatParameters());
         }
         else {
            OSSubString outstream;
            this->_write(outstream, IOObject::FormatParameters());
            out.cat(outstream.buffer());
         }
      }

   SubString asString() const
      {  SString result(50); TOSRSubString<SubString> out(result); this->_write(out, FormatParameters()); return result; }
   WideSubString asWideString() const
      {  WideSString result(50); TOSRSubString<WideSubString> out(result); this->_write(out, FormatParameters().setWide()); return result; }
};

/* Definition of the inline internal stream methods */

#define DefineInlineIStreamMethods

#define DefineTypeObject IFStream
#define DefineLinkStream fsIn
#include "StandardClasses/IOStreamMethods.inch"
#undef DefineLinkStream
#undef DefineTypeObject

#undef DefineInlineIStreamMethods

#define DefineInlineOStreamMethods

#define DefineTypeObject FStream
#define DefineLinkStream fsOut
#include "StandardClasses/IOStreamMethods.inch"
#undef DefineLinkStream
#undef DefineTypeObject

#define DefineTypeObject OFStream
#define DefineLinkStream ofsOut
#include "StandardClasses/IOStreamMethods.inch"
#undef DefineLinkStream
#undef DefineTypeObject

#undef DefineInlineOStreamMethods

/* template TISSubString */

template <class TypeSubString>
inline int
TISSubString<TypeSubString>::gcount() { return iLastRead; }

template <class TypeSubString>
inline bool
TISSubString<TypeSubString>::canread() { return ssIn.length() > 0; }

template <class TypeSubString>
inline bool
TISSubString<TypeSubString>::good() { return ssIn.length() > 0; }

template <class TypeSubString>
inline int
TISSubString<TypeSubString>::peek()
   { return (ssIn.length() > 0) ? ((typename TypeSubString::Char) ssIn) : EOF; }

template <class TypeSubString>
inline int
TISSubString<TypeSubString>::tellg() {
   SubString support(ssIn);
   support.seek(0, 0);
   return support.getSubStringPos(ssIn);
}

template <class TypeSubString>
inline IOObject::ISBase&
TISSubString<TypeSubString>::seekg(int pos) {
   ssIn.seek(pos, 0);
   return *this;
}

template <class TypeSubString>
inline IOObject::ISBase&
TISSubString<TypeSubString>::seekg(int pos, int origin) {
   ssIn.seek(pos, origin);
   return *this;
}

template <class TypeSubString>
inline IOObject::ISBase&
TISSubString<TypeSubString>::unget() {
   int length = ssIn.length();
   ssIn.advance(-1);
   iLastRead = length-ssIn.length();
   return *this;
}

template <class TypeSubString>
inline IOObject::ISBase&
TISSubString<TypeSubString>::read(char& ch, bool) {
   if (ssIn.length() > 0) {
      iLastRead = 1;
      ch = (char) (typename TypeSubString::Char) ssIn;
      ssIn.advance(1);
   }
   else {
      iLastRead = 0;
      throw EReadError();
   };
   return *this;
}


template <class TypeSubString>
inline int
TISSubString<TypeSubString>::get() {
   if (ssIn.length() > 0) {
      iLastRead = 1;
      int iResult = (typename TypeSubString::Char) ssIn;
      ssIn.advance(1);
      return iResult;
   }
   else {
      iLastRead = 0;
      return EOF;
   };
}

template <class TypeSubString>
inline IOObject::ISBase&
TISSubString<TypeSubString>::get(char& ch) {
   if (ssIn.length() > 0) {
      iLastRead = 1;
      ch = (char) ssIn;
      ssIn.advance(1);
   }
   else {
      iLastRead = 0;
      throw EReadError();
   };
   return *this;
}

template <class TypeSubString>
inline IOObject::ISBase&
TISSubString<TypeSubString>::read(bool& b, bool isRaw) {
   char ch;
   if (ssIn.length() > 0) {
      iLastRead = 1;
      ch = (char) (typename TypeSubString::Char) ssIn;
      ssIn.advance(1);
   }
   else {
      iLastRead = 0;
      throw EReadError();
   };
   b = isRaw ? (bool) ch : (bool) (ch - '0');
   return *this;
}

template <class TypeSubString>
inline IOObject::ISBase&
TISSubString<TypeSubString>::ignorespaces() {
   int iLength = ssIn.length();
   ssIn.leftTrim();
   iLastRead = iLength-ssIn.length();
   return *this;
}

/* template IOObject::TOSSubString */

#define DefineTypeObject TOSSubString
#define DefineReference inheritedImplementation::
#include "TString/IOSubStringMethods.inch"
#undef DefineReference
#undef DefineTypeObject

#define DefineTypeObject TOSRSubString
#define DefineReference pssReference->
#include "TString/IOSubStringMethods.inch"
#undef DefineReference
#undef DefineTypeObject

} // end of namespace DIOObject

namespace Lexer {

/************************************************/
/* Definition of the template class TTextBuffer */
/************************************************/

template<class TypeSubString>
class TTextBuffer : public IOObject {
  public:
   typedef typename TypeSubString::String String;
   typedef TypeSubString SubString;

  private:
   typedef IOObject inherited;
   typedef TTextBuffer<TypeSubString> thisType;
   COL::TCopyCollection<COL::TBasicList<String, HandlerCopyCast<String> > > lsBuffers;
   PNT::AutoPointer<String> apsCurrent;

   void performTranslation()
      {  int defaultSize = apsCurrent->length() + apsCurrent->getPlace();
         lsBuffers.insertAtEnd(apsCurrent.extractElement());
         apsCurrent.assign(new String(defaultSize), PNT::Pointer::Init());
      }
   void completeRead(const typename TypeSubString::Char* string, int len);
   void completeRead(SubString source);
   void completeWrite(SubString& source);
   typename TypeSubString::Char completeQueryChar(int index) const;

  protected:
   virtual void _read(ISBase& in, const FormatParameters& params) override;
   virtual void _write(OSBase& out, const FormatParameters& params) const override;

  public:
   TTextBuffer(int defaultLength=5000)
      :  apsCurrent(new String(defaultLength), PNT::Pointer::Init()) {}
   TTextBuffer(const thisType& source) = default;
   thisType& operator=(const thisType& source) = default;
   TemplateDefineCopy(TTextBuffer, TypeSubString)
   DTemplateDefineAssign(TTextBuffer, TypeSubString)

   void clear() { lsBuffers.freeAll(); apsCurrent->setLength(0); }
   thisType& operator<<(typename TypeSubString::Char ch);
   thisType& operator<<(const typename TypeSubString::Char* string);
   thisType& operator<<(SubString& string);

   thisType& operator>>(SubString& string);
   bool isEmpty() const {  return lsBuffers.isEmpty() && apsCurrent->length() == 0;}
   bool hasOnlyCurrent() const { return lsBuffers.isEmpty(); }
   TypeSubString& current() { AssumeCondition(lsBuffers.isEmpty()) return *apsCurrent; }
   typename TypeSubString::Char getChar(int index) const
      {  return lsBuffers.isEmpty() ? apsCurrent->getSChar(index) : completeQueryChar(index); }
};

template<class TypeSubString>
inline TTextBuffer<TypeSubString>&
TTextBuffer<TypeSubString>::operator<<(typename TypeSubString::Char ch) {
   if (apsCurrent->getPlace() == 0)
      performTranslation();
   apsCurrent->cat(ch);
   return *this;
}

template<class TypeSubString>
inline TTextBuffer<TypeSubString>&
TTextBuffer<TypeSubString>::operator<<(const typename TypeSubString::Char* string) {
   int len = TypeSubString::Traits::strlen(string);
   if (apsCurrent->getPlace() < len)
      completeRead(string, len);
   else
      apsCurrent->cat(string, len);
   return *this;
}

template<class TypeSubString>
inline TTextBuffer<TypeSubString>&
TTextBuffer<TypeSubString>::operator<<(SubString& string) {
   if (apsCurrent->getPlace() < string.length())
      completeRead(string);
   else
      apsCurrent->cat(string);
   string.setToEnd();
   return *this;
}

template<class TypeSubString>
inline TTextBuffer<TypeSubString>&
TTextBuffer<TypeSubString>::operator>>(SubString& string) {
   if (!lsBuffers.isEmpty())
      completeWrite(string);
   else {
      string.cat(*apsCurrent);
      apsCurrent->setLength(0);
   };
   return *this;
}

typedef TTextBuffer<SubString> TextBuffer;

}} // end of namespace STG::Lexer

#endif // STG_SStringH
