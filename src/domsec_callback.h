/////////////////////////////////
//
// Library   : Static Analysis
// Unit      : armsec
// File      : armsec_memory_callback.h
// Copyright : CEA LIST - 2020
//
// Description :
//   Definition of a generic C Interface to manipulate abstract domains
//   = interval, disjunctions, ...
//

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef _MSC_VER
#pragma pack(push,4)
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _DomainElement DomainElement;
typedef struct _DomainElementFunctions DomainElementFunctions;

typedef uint32_t DomainOperation;
static const int DomainArgumentsNumberSize = 2;
static const int DomainConstnessSize = 2;
static const int DomainDefinedSize = 1; /* for symbolic domains */
static const int DomainBitTypeSize = 5;
static const int DomainMultiBitTypeSize = 5;
static const int DomainMultiFloatTypeSize = 5;

typedef enum _DomainBitUnaryOperation 
{  DBUOUndefined, DBUOPrev, DBUONext, DBUONegate
} DomainBitUnaryOperation;

typedef enum _DomainBitBinaryOperation 
{
   DBBOUndefined, DBBOPlus, DBBOMinus, DBBOMin, DBBOMax, DBBOOr, DBBOAnd,
   DBBOExclusiveOr
} DomainBitBinaryOperation;

typedef enum _DomainBitCompareOperation 
{  DBCOUndefined, DBCOCompareLess, DBCOCompareLessOrEqual, DBCOCompareEqual,
   DBCOCompareDifferent, DBCOCompareGreaterOrEqual, DBCOCompareGreater,
} DomainBitCompareOperation;


typedef enum _DomainMultiBitUnaryOperation 
{  DMBUOUndefined, DMBUOPrevSigned, DMBUOPrevUnsigned, DMBUONextSigned,
   DMBUONextUnsigned, DMBUOBitNegate, DMBUOOppositeSigned, DMBUOOppositeFloat,
   DMBUOBitScanReverse
} DomainMultiBitUnaryOperation;

typedef enum _DomainMultiBitExtendType {
   DMBEOUndefined, DMBEOExtendWithZero, DMBEOExtendWithSign
} DomainMultiBitExtendType;

typedef struct _DomainMultiBitExtendOperation 
{  
   DomainMultiBitExtendType type;
   int new_size;
} DomainMultiBitExtendOperation;

typedef struct _DomainMultiBitReduceOperation 
{  
   int first_bit, last_bit;
} DomainMultiBitReduceOperation;

typedef struct _DomainMultiBitSetOperation 
{  
   int first_bit, last_bit;
} DomainMultiBitSetOperation;

typedef enum _DomainMultiBitBinaryOperation 
{
   DMBBOUndefined, DMBBOConcat,
   DMBBOPlusSigned, DMBBOPlusUnsigned, DMBBOPlusUnsignedWithSigned, DMBBOPlusFloat, DMBBOMinusSigned,
      DMBBOMinusUnsigned, DMBBOMinusFloat,
   DMBBOMinSigned, DMBBOMinUnsigned, DMBBOMinFloat, DMBBOMaxSigned, DMBBOMaxUnsigned, DMBBOMaxFloat,
   DMBBOTimesSigned, DMBBOTimesUnsigned, DMBBOTimesFloat,
      DMBBODivideSigned, DMBBODivideUnsigned, DMBBODivideFloat,
   DMBBOModuloSigned, DMBBOModuloUnsigned, DMBBOBitOr, DMBBOBitAnd, DMBBOBitExclusiveOr,
      DMBBOLeftShift, DMBBOLogicalRightShift, DMBBOArithmeticRightShift, DMBBOLeftRotate,
      DMBBORightRotate,
} DomainMultiBitBinaryOperation;

typedef enum _DomainMultiBitCompareOperation 
{  DMBCOUndefined, DMBCOCompareLessSigned, DMBCOCompareLessOrEqualSigned, DMBCOCompareLessUnsigned,
      DMBCOCompareLessOrEqualUnsigned, DMBCOCompareLessFloat, DMBCOCompareLessOrEqualFloat,
      DMBCOCompareEqual, DMBCOCompareEqualFloat, DMBCOCompareDifferentFloat, DMBCOCompareDifferent,
      DMBCOCompareGreaterOrEqualFloat, DMBCOCompareGreaterFloat, DMBCOCompareGreaterOrEqualUnsigned,
      DMBCOCompareGreaterUnsigned, DMBCOCompareGreaterOrEqualSigned, DMBCOCompareGreaterSigned
} DomainMultiBitCompareOperation;

typedef enum _DomainMultiFloatToIntOperation
{
   DMFIOUndefined, DMFIOIsInftyExponent, DMFIOIsNaN, DMFIOIsQNaN, DMFIOIsSNaN, DMFIOIsPositive, DMFIOIsZeroExponent,
      DMFIOIsNegative, DMFIOCastMantissa,
} DomainMultiFloatToIntOperation;

typedef enum _DomainMultiFloatUnaryOperation
{
   DMFUOUndefined, DMFUOCastFloat, DMFUOCastDouble, DMFUOOpposite, DMFUOAbs,
   DMFUOAcos, DMFUOAsin, DMFUOAtan, DMFUOCeil, DMFUOCos, DMFUOCosh, DMFUOExp, DMFUOFabs, DMFUOFloor,
      DMFUOLog, DMFUOLog10, DMFUOPow, DMFUOSin, DMFUOSinh, DMFUOSqrt, DMFUOTan, DMFUOTanh,
   DMFUOSetToNaN, DMFUOSetQuietBit
} DomainMultiFloatUnaryOperation;

typedef enum _DomainMultiFloatBinaryOperation
{
   DMFBOUndefined, DMFBOPlus, DMFBOMinus, DMFBOMin, DMFBOMax, DMFBOTimes, DMFBODivide,
   DMFBOAtan2, DMFBOFmod, DMFBOFrexp, DMFBOLdexp, DMFBOModf
} DomainMultiFloatBinaryOperation;

typedef enum _DomainMultiFloatCompareOperation
{
   DMFCOUndefined, DMFCOCompareLess, DMFCOCompareLessOrEqual, DMFCOCompareEqual, DMFCOCompareDifferent,
      DMFCOCompareGreaterOrEqual, DMFCOCompareGreater
} DomainMultiFloatCompareOperation;

typedef enum _DomainMultiFloatTernaryOperation
{
   DMFTOUndefined, DMFTOMultAdd, DMFTOMultSub, DMFTONegMultAdd, DMFTONegMultSub
} DomainMultiFloatTernaryOperation;

typedef enum _DomainMultiFloatQueryOperation
{  DMFTQUndefined, DMFTQIsInvalid
} DomainMultiFloatQueryOperation;

typedef enum _DomainEvaluationError
{  DEENoError=0, DEEPositiveOverflow=1<<1, DEENegativeOverflow=1<<2,
   DEEPositiveUnderflow=1<<3, DEENegativeUnderflow=1<<4, DEEDivisionByZero=1<<5,
   DEENegativeModulo=1<<6, DEENaN=1<<7
} DomainEvaluationError;

typedef enum _DomainTypeVerdict { DEVNotPerformed=0, DEVExactApplied=1, DEVDegradateApplied=3 }
   DomainTypeVerdict;

typedef enum _DomainInitStructure
{
   DISUndefined,
   DISFormal,      /* an operation creates a symbolic expression */
   DISDisjunction, /* an operation creates disjunction of possible values */
   DISInterval,    /* an operation creates an interval */
   DISShareTop,    /* an operation creates a value with all the possible range;
                      it keeps the equality relationships between existing memory */
   DISTop          /* an operation creates a value with all the possible range
                      and without any relationship with the existing memory */
} DomainInitStructure;

typedef struct _DomainEvaluationEnvironment {
   unsigned int warnings : 8;
   bool emptyResult : 1;
   DomainTypeVerdict verdict : 2;
   DomainInitStructure defaultDomainType : 3;
} DomainEvaluationEnvironment;

typedef struct _DomainElement { void* content; } DomainElement;
typedef struct _DomainElement DomainBitElement;
typedef struct _DomainElement DomainMultiBitElement;
typedef struct _DomainElement DomainMultiFloatElement;

typedef struct _DomainIntegerConstant {
   int sizeInBits;
   bool isSigned;
   uint64_t integerValue;
} DomainIntegerConstant;

typedef struct _DomainFloatingPointConstant {
   int sizeInBits;
   long double floatValue;
} DomainFloatingPointConstant;

typedef enum _ZeroResult { ZRUndefined, ZRZero, ZRDifferentZero } ZeroResult;
typedef enum _ZeroResult BooleanResult;
typedef enum _DomainType { DTUndefined, DTBit, DTInteger, DTFloating } DomainType;

struct _DomainElementFunctions {
  DomainType (*get_type)(DomainElement domain);
  ZeroResult (*query_zero_result)(DomainElement domain);
  int (*get_size_in_bits)(DomainElement domain);
  bool (*is_top)(DomainElement domain);
  void (*free)(DomainElement* element);
  DomainElement (*clone)(DomainElement element);

  DomainBitElement (*bit_create_constant)(bool value);
  DomainBitElement (*bit_create_top)(bool isSymbolic);
  DomainMultiBitElement (*bit_create_cast_multibit)(
      DomainBitElement bitDomain, int sizeInBits, DomainEvaluationEnvironment* env);
  bool (*bit_unary_apply_assign)(DomainBitElement* element,
      DomainBitUnaryOperation operation, DomainEvaluationEnvironment* env);
  DomainBitElement (*bit_create_unary_apply)(DomainBitElement element,
      DomainBitUnaryOperation operation, DomainEvaluationEnvironment* env);
  bool (*bit_binary_apply_assign)(DomainBitElement* element,
      DomainBitBinaryOperation operation, DomainBitElement source,
      DomainEvaluationEnvironment* env);
  DomainBitElement (*bit_create_binary_apply)(DomainBitElement element,
      DomainBitBinaryOperation operation, DomainBitElement source,
      DomainEvaluationEnvironment* env);
  ZeroResult (*bit_binary_compare)(DomainBitElement element,
      DomainBitCompareOperation operation, DomainBitElement source,
      DomainEvaluationEnvironment* env);
  DomainBitElement (*bit_binary_compare_domain)(DomainBitElement element,
      DomainBitCompareOperation operation, DomainBitElement source,
      DomainEvaluationEnvironment* env);
  DomainBitElement (*bit_guard_assign)(DomainBitElement* condition,
      DomainBitElement* first, DomainBitElement* second,
      DomainEvaluationEnvironment* env);
  ZeroResult (*bit_query_boolean)(DomainBitElement element);
  bool (*bit_cast_multibit_constraint)(DomainMultiBitElement result,
      int sizeInBits, DomainBitElement* element, DomainEvaluationEnvironment* env);
  bool (*bit_unary_constraint)(DomainBitElement result,
      DomainBitUnaryOperation operation, DomainBitElement* element,
      DomainEvaluationEnvironment* env);
  bool (*bit_binary_constraint)(DomainBitElement result,
      DomainBitBinaryOperation operation, DomainBitElement* first, DomainBitElement* second,
      DomainEvaluationEnvironment* env);
  bool (*bit_compare_constraint)(DomainBitElement result,
      DomainBitCompareOperation operation, DomainBitElement* first, DomainBitElement* second,
      DomainEvaluationEnvironment* env);
  bool (*bit_is_constant_value)(DomainBitElement domain, bool* value);

  DomainMultiBitElement (*multibit_create_constant)(DomainIntegerConstant value);
  DomainMultiBitElement (*multibit_create_top)(int sizeInBits, bool isSymbolic);
  DomainMultiBitElement (*multibit_create_interval_and_absorb)(
      DomainMultiBitElement* min, DomainMultiBitElement* max, bool isSigned, bool isSymbolic);
  DomainBitElement (*multibit_create_cast_bit)(DomainMultiBitElement multibitDomain,
      DomainEvaluationEnvironment* env);
  DomainBitElement (*multibit_create_cast_shift_bit)(DomainMultiBitElement multibitDomain,
      int shift, DomainEvaluationEnvironment* env);
  DomainMultiBitElement (*multibit_create_cast_multibit)(DomainMultiBitElement multibitSource,
        int destinationSizeInBits, bool isSigned, DomainEvaluationEnvironment* env);
  DomainMultiFloatElement (*multibit_create_cast_multifloat)(
      DomainMultiBitElement multibitDomain, int sizeInBits, bool isSigned,
      DomainEvaluationEnvironment* env);
  DomainMultiFloatElement (*multibit_create_cast_multifloat_ptr)(
      DomainMultiBitElement multibitDomain, int sizeInBits, bool isSigned,
      DomainEvaluationEnvironment* env);
  bool (*multibit_unary_apply_assign)(DomainMultiBitElement* element,
      DomainMultiBitUnaryOperation operation, DomainEvaluationEnvironment* env);
  DomainMultiBitElement (*multibit_create_unary_apply)(DomainMultiBitElement element,
      DomainMultiBitUnaryOperation operation, DomainEvaluationEnvironment* env);
  bool (*multibit_extend_apply_assign)(DomainMultiBitElement* element,
      DomainMultiBitExtendOperation operation, DomainEvaluationEnvironment* env);
  DomainMultiBitElement (*multibit_create_extend_apply)(DomainMultiBitElement element,
      DomainMultiBitExtendOperation operation, DomainEvaluationEnvironment* env);
  bool (*multibit_reduce_apply_assign)(DomainMultiBitElement* element,
      DomainMultiBitReduceOperation operation, DomainEvaluationEnvironment* env);
  DomainMultiBitElement (*multibit_create_reduce_apply)(DomainMultiBitElement element,
      DomainMultiBitReduceOperation operation, DomainEvaluationEnvironment* env);
  bool (*multibit_bitset_apply_assign)(DomainMultiBitElement* element,
      DomainMultiBitSetOperation operation, DomainMultiBitElement source,
      DomainEvaluationEnvironment* env);
  DomainMultiBitElement (*multibit_create_bitset_apply)(DomainMultiBitElement element,
      DomainMultiBitSetOperation operation, DomainMultiBitElement source,
      DomainEvaluationEnvironment* env);
  bool (*multibit_binary_apply_assign)(DomainMultiBitElement* element,
      DomainMultiBitBinaryOperation operation, DomainMultiBitElement source,
      DomainEvaluationEnvironment* env);
  DomainMultiBitElement (*multibit_create_binary_apply)(DomainMultiBitElement element,
      DomainMultiBitBinaryOperation operation, DomainMultiBitElement source,
      DomainEvaluationEnvironment* env);
  ZeroResult (*multibit_binary_compare)(DomainMultiBitElement element,
      DomainMultiBitCompareOperation operation, DomainMultiBitElement source,
      DomainEvaluationEnvironment* env);
  DomainBitElement (*multibit_binary_compare_domain)(DomainMultiBitElement element,
      DomainMultiBitCompareOperation operation, DomainMultiBitElement source,
      DomainEvaluationEnvironment* env);
  DomainMultiBitElement (*multibit_guard_assign)(DomainBitElement* condition,
      DomainMultiBitElement* first, DomainMultiBitElement* second,
      DomainEvaluationEnvironment* env);
  ZeroResult (*multibit_query_boolean)(DomainMultiBitElement element);
  bool (*multibit_cast_bit_constraint)(DomainBitElement result,
      DomainMultiBitElement* element, DomainEvaluationEnvironment* env);
  bool (*multibit_cast_shift_bit_constraint)(DomainBitElement result,
      int shift, DomainMultiBitElement* element, DomainEvaluationEnvironment* env);
  bool (*multibit_cast_multifloat_constraint)(DomainMultiFloatElement result,
      int sizeInBits, bool isSigned, DomainMultiBitElement* element,
      DomainEvaluationEnvironment* env);
  bool (*multibit_cast_multifloat_ptr_constraint)(DomainMultiFloatElement result,
      int sizeInBits, bool isSigned, DomainMultiBitElement* element,
      DomainEvaluationEnvironment* env);
  bool (*multibit_unary_constraint)(DomainMultiBitElement result,
      DomainMultiBitUnaryOperation operation, DomainMultiBitElement* element,
      DomainEvaluationEnvironment* env);
  bool (*multibit_extend_constraint)(DomainMultiBitElement result,
      DomainMultiBitExtendOperation operation, DomainMultiBitElement* element,
      DomainEvaluationEnvironment* env);
  bool (*multibit_reduce_constraint)(DomainMultiBitElement result,
      DomainMultiBitReduceOperation operation, DomainMultiBitElement* element,
      DomainEvaluationEnvironment* env);
  bool (*multibit_bitset_constraint)(DomainMultiBitElement result,
      DomainMultiBitSetOperation operation, DomainMultiBitElement* first,
      DomainMultiBitElement* second, DomainEvaluationEnvironment* env);
  bool (*multibit_binary_constraint)(DomainMultiBitElement result,
      DomainMultiBitBinaryOperation operation, DomainMultiBitElement* first,
      DomainMultiBitElement* second, DomainEvaluationEnvironment* env);
  bool (*multibit_compare_constraint)(DomainBitElement result,
      DomainMultiBitCompareOperation operation, DomainMultiBitElement* first,
      DomainMultiBitElement* second, DomainEvaluationEnvironment* env);
  bool (*multibit_is_constant_value)(DomainMultiBitElement domain,
      DomainIntegerConstant* value);

  DomainMultiFloatElement (*multifloat_create_constant)(
      DomainFloatingPointConstant value);
  DomainMultiFloatElement (*multifloat_create_top)(int sizeInBits, bool isSymbolic);
  DomainMultiFloatElement (*multifloat_create_interval_and_absorb)(
      DomainMultiFloatElement* min, DomainMultiFloatElement* max, bool isSymbolic);
  DomainMultiBitElement (*multifloat_create_cast_multibit)(
      DomainMultiFloatElement multibitDomain, int sizeInBits,
      DomainEvaluationEnvironment* env);
  DomainMultiBitElement (*multifloat_query_to_multibit)(
      DomainMultiFloatElement multifloatDomain, DomainMultiFloatToIntOperation operation,
      DomainEvaluationEnvironment* env);
  bool (*multifloat_cast_multifloat_assign)(DomainMultiFloatElement* element,
      int size_in_bits, DomainEvaluationEnvironment* env);
  DomainMultiFloatElement (*multifloat_cast_multifloat)(
      DomainMultiFloatElement element, int size_in_bits,
      DomainEvaluationEnvironment* env);
  bool (*multifloat_unary_apply_assign)(DomainMultiFloatElement* element,
      DomainMultiFloatUnaryOperation operation, DomainEvaluationEnvironment* env);
  DomainMultiFloatElement (*multifloat_create_unary_apply)(
      DomainMultiFloatElement element, DomainMultiFloatUnaryOperation operation,
      DomainEvaluationEnvironment* env);
  DomainBitElement (*multifloat_flush_to_zero)(
      DomainMultiFloatElement* element, DomainEvaluationEnvironment* env);
  bool (*multifloat_binary_apply_assign)(DomainMultiFloatElement* element,
      DomainMultiFloatBinaryOperation operation, DomainMultiFloatElement source,
      DomainEvaluationEnvironment* env);
  DomainMultiFloatElement (*multifloat_create_binary_apply)(
      DomainMultiFloatElement element, DomainMultiFloatBinaryOperation operation,
      DomainMultiFloatElement source, DomainEvaluationEnvironment* env);
  ZeroResult (*multifloat_binary_compare)(DomainMultiFloatElement element,
      DomainMultiFloatCompareOperation operation, DomainMultiFloatElement source,
      DomainEvaluationEnvironment* env);
  DomainBitElement (*multifloat_binary_compare_domain)(DomainMultiFloatElement element,
      DomainMultiFloatCompareOperation operation, DomainMultiFloatElement source,
      DomainEvaluationEnvironment* env);
  DomainMultiBitElement (*multifloat_binary_full_compare_domain)(DomainMultiFloatElement element,
      DomainMultiFloatElement source, DomainEvaluationEnvironment* env);
  DomainMultiFloatElement (*multifloat_guard_assign)(DomainBitElement* condition,
      DomainMultiFloatElement* first, DomainMultiFloatElement* second,
      DomainEvaluationEnvironment* env);
  bool (*multifloat_ternary_apply_assign)(DomainMultiFloatElement* element,
      DomainMultiFloatTernaryOperation operation, DomainMultiFloatElement second,
      DomainMultiFloatElement third, DomainEvaluationEnvironment* env);
  DomainBitElement (*multifloat_ternary_query)(
      DomainMultiFloatElement multifloatDomain, DomainMultiFloatQueryOperation operation,
      DomainMultiFloatElement first, DomainMultiFloatElement second, DomainEvaluationEnvironment* env);
  DomainMultiFloatElement (*multifloat_create_ternary_apply)(
      DomainMultiFloatElement element, DomainMultiFloatTernaryOperation operation,
      DomainMultiFloatElement second, DomainMultiFloatElement third,
      DomainEvaluationEnvironment* env);
  bool (*multifloat_cast_multibit_constraint)(DomainMultiBitElement result,
      int sizeInBits, DomainMultiFloatElement* element, DomainEvaluationEnvironment* env);
  bool (*multifloat_query_to_multibit_constraint)(DomainMultiBitElement result,
      DomainMultiFloatToIntOperation operation, DomainMultiFloatElement* element,
      DomainEvaluationEnvironment* env);
  bool (*multifloat_cast_multifloat_constraint)(DomainMultiFloatElement result,
      int size_in_bits, DomainMultiFloatElement* element, DomainEvaluationEnvironment* env);
  bool (*multifloat_unary_constraint)(DomainMultiFloatElement result,
      DomainMultiFloatUnaryOperation operation, DomainMultiFloatElement* element,
      DomainEvaluationEnvironment* env);
  bool (*multifloat_binary_constraint)(DomainMultiFloatElement result,
      DomainMultiFloatBinaryOperation operation, DomainMultiFloatElement* first,
      DomainMultiFloatElement* second, DomainEvaluationEnvironment* env);
  bool (*multifloat_compare_constraint)(DomainBitElement result,
      DomainMultiFloatCompareOperation operation, DomainMultiFloatElement* first,
      DomainMultiFloatElement* second, DomainEvaluationEnvironment* env);
  bool (*multifloat_ternary_constraint)(DomainMultiFloatElement result,
      DomainMultiFloatTernaryOperation operation, DomainMultiFloatElement* first,
      DomainMultiFloatElement* second, DomainMultiFloatElement* third,
      DomainEvaluationEnvironment* env);
  bool (*multifloat_is_constant_value)(DomainMultiFloatElement domain,
      DomainFloatingPointConstant* value);

  char* (*write)(DomainElement domain, char* buffer, int buffer_size, int* length, void* writer,
      char* (*increase_buffer_size)(char* buffer, int old_length, int new_length, void* writer));
  bool (*merge)(DomainElement* element, DomainElement source,
      DomainEvaluationEnvironment* env);
  bool (*intersect)(DomainElement* element, DomainElement source,
      DomainEvaluationEnvironment* env);
  bool (*contain)(DomainElement element, DomainElement source,
      DomainEvaluationEnvironment* env);
  int (*compare)(DomainElement element, DomainElement source);
  DomainElement (*create_disjunction_and_absorb)(DomainElement* element);
  void (*disjunction_absorb)(DomainElement* disjunction, DomainElement* element);
  void (*specialize)(DomainElement* element);
};

#ifdef __cplusplus
}
#endif

#ifdef _MSC_VER
#pragma pack(pop)
#endif

