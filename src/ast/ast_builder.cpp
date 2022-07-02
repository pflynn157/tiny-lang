#include <string>

#include <ast/ast_builder.hpp>
#include <ast/ast.hpp>

namespace AstBuilder {

//
// The builders for data types
//
AstDataType *buildVoid() {
    return new AstDataType(V_AstType::Void);
}

AstDataType *buildBool() {
    return new AstDataType(V_AstType::Bool);
}

AstDataType *buildChar() {
    return new AstDataType(V_AstType::Char);
}

AstDataType *buildInt8(bool isUnsigned) {
    if (isUnsigned) return new AstDataType(V_AstType::U8);
    return new AstDataType(V_AstType::I8);
}

AstDataType *buildInt16(bool isUnsigned) {
    if (isUnsigned) return new AstDataType(V_AstType::U16);
    return new AstDataType(V_AstType::I16);
}

AstDataType *buildInt32(bool isUnsigned) {
    if (isUnsigned) return new AstDataType(V_AstType::U32);
    return new AstDataType(V_AstType::I32);
}

AstDataType *buildInt64(bool isUnsigned) {
    if (isUnsigned) return new AstDataType(V_AstType::U64);
    return new AstDataType(V_AstType::I64);
}

AstDataType *buildString() {
    return new AstDataType(V_AstType::String);
}

AstPointerType *buildPointerType(AstDataType *base) {
    return new AstPointerType(base);
}

AstStructType *buildStructType(std::string name) {
    return new AstStructType(name);
}

} // End AstBuilder

