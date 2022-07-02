#pragma once

#include <string>

#include <ast/ast.hpp>

namespace AstBuilder {

//
// The builders for data types
//
AstDataType *buildVoid();
AstDataType *buildBool();
AstDataType *buildChar();
AstDataType *buildInt8(bool isUnsigned = false);
AstDataType *buildInt16(bool isUnsigned = false);
AstDataType *buildInt32(bool isUnsigned = false);
AstDataType *buildInt64(bool isUnsigned = false);
AstDataType *buildString();
AstPointerType *buildPointerType(AstDataType *base);
AstStructType *buildStructType(std::string name);

}

