#include <string>

#include <ast/ast_builder.hpp>
#include <ast/ast.hpp>

namespace AstBuilder {

//
// The builders for data types
//
AstDataType *buildVoidType() {
    return new AstDataType(V_AstType::Void);
}

AstDataType *buildBoolType() {
    return new AstDataType(V_AstType::Bool);
}

AstDataType *buildCharType() {
    return new AstDataType(V_AstType::Char);
}

AstDataType *buildInt8Type(bool isUnsigned) {
    return new AstDataType(V_AstType::Int8, isUnsigned);
}

AstDataType *buildInt16Type(bool isUnsigned) {
    return new AstDataType(V_AstType::Int16, isUnsigned);
}

AstDataType *buildInt32Type(bool isUnsigned) {
    return new AstDataType(V_AstType::Int32, isUnsigned);
}

AstDataType *buildInt64Type(bool isUnsigned) {
    return new AstDataType(V_AstType::Int64, isUnsigned);
}

AstDataType *buildStringType() {
    return new AstDataType(V_AstType::String);
}

AstPointerType *buildPointerType(AstDataType *base) {
    return new AstPointerType(base);
}

AstStructType *buildStructType(std::string name) {
    return new AstStructType(name);
}

} // End AstBuilder

