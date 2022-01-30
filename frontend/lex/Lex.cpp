//
// Copyright 2022 Patrick Flynn
// This file is part of the Eos compiler.
// Eos is licensed under the BSD-3 license. See the COPYING file for more information.
//
#include <iostream>
#include <cctype>

#include <lex/Lex.hpp>

// The token debug function
Token::Token() {
    type = EmptyToken;
    id_val = "";
    i32_val = 0;
}

// The scanner functions
Scanner::Scanner(std::string input) {
    reader = std::ifstream(input.c_str());
    if (!reader.is_open()) {
        std::cout << "Unknown input file." << std::endl;
        error = true;
    }
}

Scanner::~Scanner() {
    reader.close();
}

void Scanner::rewind(Token token) {
    token_stack.push(token);
}

// The main scanning function
Token Scanner::getNext() {
    if (token_stack.size() > 0) {
        Token top = token_stack.top();
        token_stack.pop();
        return top;
    }

    Token token;
    if (reader.eof()) {
        token.type = Eof;
        return token;
    }
    
    for (;;) {
        char next = reader.get();
        if (reader.eof()) {
            token.type = Eof;
            break;
        }
        
        rawBuffer += next;
        
        if (next == '#') {
            std::string comment = "#";
            while (next != '\n' && !reader.eof()) {
                next = reader.get();
                rawBuffer += next;
                comment += next;
            }
            if (comment == "#pragma nocount\n") {
                skipNextLineCount = true;
            } else {
               ++currentLine;
            }
            continue;
        }
        
        // TODO: This needs some kind of error handleing
        if (next == '\'') {
            char c = reader.get();
            rawBuffer += c;
            if (c == '\\') {
                c = reader.get();
                if (c == 'n') {
                    c = '\n';
                    rawBuffer += c;
                }
            }
        
            Token charL;
            charL.i8_val = c;
            charL.type = CharL;
            
            next = reader.get();
            rawBuffer += next;
            return charL;
        }
        
        if (next == '\"') {
            if (inQuote) {
                Token str;
                str.type = String;
                str.id_val = buffer;
                
                buffer = "";
                inQuote = false;
                return str;
            } else {
                inQuote = true;
                continue;
            }
        }
        
        if (inQuote) {
            if (next == '\\') {
                next = reader.get();
                rawBuffer += next;
                switch (next) {
                    case 'n': buffer += '\n'; break;
                    case 't': buffer += '\t'; break;
                    default: buffer += '\\' + next;
                }
            } else {
                buffer += next;
            }
            continue;
        }
        
        if (next == ' ' || next == '\n' || isSymbol(next)) {
            if (next == '\n') {
                if (skipNextLineCount) skipNextLineCount = false;
                else ++currentLine;
            }
        
            if (buffer.length() == 0) {
                if (isSymbol(next)) {
                    Token sym;
                    sym.type = getSymbol(next);
                    return sym;
                }
                continue;
            }
            
            // Check if we have a symbol
            // Here, we also check to see if we have a floating point
            if (next == '.') {
                if (isInt()) {
                    buffer += ".";
                    continue;
                } else {
                    Token sym;
                    sym.type = getSymbol(next);
                    token_stack.push(sym);
                }
            } else if (isSymbol(next)) {
                Token sym;
                sym.type = getSymbol(next);
                token_stack.push(sym);
            }
            
            // Now check the buffer
            token.type = getKeyword();
            if (token.type != EmptyToken) {
                buffer = "";
                break;
            }
            
            if (isInt()) {
                token.type = Int32;
                token.i32_val = std::stoi(buffer);
            } else if (isHex()) {
                token.type = Int32;
                token.i32_val = std::stoi(buffer, 0, 16);
            } else {
                token.type = Id;
                token.id_val = buffer;
            }
            
            // Reset everything
            buffer = "";
            break;
        } else {
            buffer += next;
        }
    }
    
    return token;
}

std::string Scanner::getRawBuffer() {
    std::string ret = rawBuffer;
    rawBuffer = "";
    return ret;
}

bool Scanner::isSymbol(char c) {
    switch (c) {
        case ';': 
        case ':': 
        case '.':
        case '=':
        case '(':
        case ')':
        case '[':
        case ']':
        case ',': 
        case '+': 
        case '-': 
        case '*': 
        case '/':
        case '&':
        case '|':
        case '^': 
        case '>':
        case '<': 
        case '!': return true;
    }
    return false;
}

TokenType Scanner::getKeyword() {
    if (buffer == "extern") return Extern;
    else if (buffer == "func") return Func;
    else if (buffer == "struct") return Struct;
    else if (buffer == "end") return End;
    else if (buffer == "return") return Return;
    else if (buffer == "var") return VarD;
    else if (buffer == "const") return Const;
    else if (buffer == "bool") return Bool;
    else if (buffer == "char") return Char;
    else if (buffer == "i8") return I8;
    else if (buffer == "u8") return U8;
    else if (buffer == "i16") return I16;
    else if (buffer == "u16") return U16;
    else if (buffer == "i32") return I32;
    else if (buffer == "u32") return U32;
    else if (buffer == "i64") return I64;
    else if (buffer == "u64") return U64;
    else if (buffer == "string") return Str;
    else if (buffer == "if") return If;
    else if (buffer == "elif") return Elif;
    else if (buffer == "else") return Else;
    else if (buffer == "while") return While;
    else if (buffer == "is") return Is;
    else if (buffer == "then") return Then;
    else if (buffer == "do") return Do;
    else if (buffer == "break") return Break;
    else if (buffer == "continue") return Continue;
    else if (buffer == "import") return Import;
    else if (buffer == "true") return True;
    else if (buffer == "false") return False;
    else if (buffer == "and") return Logical_And;
    else if (buffer == "or") return Logical_Or;
    else if (buffer == "enum") return Enum;
    else if (buffer == "of") return Of;
    return EmptyToken;
}

TokenType Scanner::getSymbol(char c) {
    switch (c) {
        case ';': return SemiColon;
        case '(': return LParen;
        case ')': return RParen;
        case '[': return LBracket;
        case ']': return RBracket;
        case ',': return Comma;
        case '+': return Plus;
        case '*': return Mul;
        case '/': return Div;
        case '=': return EQ;
        case '.': return Dot;
        case '&': return And;
        case '|': return Or;
        case '^': return Xor;
        
        case ':': {
            char c2 = reader.get();
            if (c2 == '=') {
                rawBuffer += c2;
                return Assign;
            } else {
                reader.unget();
                return Colon;
            }
        } break;
        
        case '>': {
            char c2 = reader.get();
            if (c2 == '=') {
                rawBuffer += c2;
                return GTE;
            } else {
                reader.unget();
                return GT;
            }
        } break;
        
        case '<': {
            char c2 = reader.get();
            if (c2 == '=') {
                rawBuffer += c2;
                return LTE;
            } else {
                reader.unget();
                return LT;
            }
        } break;
        
        case '!': {
            char c2 = reader.get();
            if (c2 == '=') {
                rawBuffer += c2;
                return NEQ;
            } else {
                reader.unget();
            }
        } break;
        
        case '-': {
            char c2 = reader.get();
            if (c2 == '>') {
                rawBuffer += c2;
                return Arrow;
            } else {
                reader.unget();
                return Minus;
            }
        } break;
    }
    return EmptyToken;
}

bool Scanner::isInt() {
    for (char c : buffer) {
        if (!isdigit(c)) return false;
    }
    return true;
}

bool Scanner::isHex() {
    if (buffer.length() < 3) return false;
    if (buffer[0] != '0' || buffer[1] != 'x') return false;
    
    for (int i = 2; i<buffer.length(); i++) {
        if (!isxdigit(buffer[i])) return false;
    }
    return true;
}
