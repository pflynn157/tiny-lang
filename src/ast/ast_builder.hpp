#pragma once

#include <string>

#include <ast/ast.hpp>

namespace AstBuilder {

//
// The builders for data types
//
AstDataType *buildVoidType();
AstDataType *buildBoolType();
AstDataType *buildCharType();
AstDataType *buildInt8Type(bool isUnsigned = false);
AstDataType *buildInt16Type(bool isUnsigned = false);
AstDataType *buildInt32Type(bool isUnsigned = false);
AstDataType *buildInt64Type(bool isUnsigned = false);
AstDataType *buildStringType();
AstPointerType *buildPointerType(AstDataType *base);
AstStructType *buildStructType(std::string name);

}

