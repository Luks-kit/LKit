#include <iostream>
#include <cstdlib>
#include <string>
#include "parser.hpp"
#include "lexer.hpp"

std::unique_ptr<Decl> Parser::parse_var_decl() {
    advance(); // consume 'let'
    expect(TokenType::Ident);
    std::string name = current.lexeme;
    advance();
        
    if (current.type == TokenType::Eq) {
        advance();
        auto init = parse_expr();
        expect(TokenType::Semi);
        advance();
        return std::make_unique<VarDecl>(name, "", std::move(init)); 
    } else if (current.type == TokenType::Semi) {
        advance();
        return std::make_unique<VarDecl>(name, "", std::make_unique<LiteralExpr>(Value(0)));
    } else {
        throw std::runtime_error("Expected '=' or ';' after variable name");
    }
}

std::unique_ptr<Decl> Parser::parse_subr_decl() {
    advance(); // consume 'subr'
    expect(TokenType::Ident);
    std::string name = current.lexeme;
    advance();
    expect(TokenType::LParen);
    advance(); // consume '('
    
    std::vector<std::pair<std::string, std::string>> params;
    if (current.type != TokenType::RParen) {
        while (true) {
            expect(TokenType::Ident);
            std::string param_name = current.lexeme;
            advance();
            expect(TokenType::Colon);
            advance(); // consume ':'
            expect(TokenType::Ident);
            std::string param_type = current.lexeme;
            advance();
            params.emplace_back(param_name, param_type);
            if (current.type == TokenType::Comma) {
                advance(); // consume ','
            } else {
                break;
            }
        }
    }
    expect(TokenType::RParen);
    advance(); // consume ')'
    
    std::string return_type = "void"; // default return type
    if (current.type == TokenType::Colon) {
        advance(); // consume '->'
        expect(TokenType::Ident);
        return_type = current.lexeme;
        advance();
    }
    
    auto body = parse_block();
    
    auto subr = std::make_unique<SubrDecl>(name, return_type);
    subr->params = std::move(params);
    subr->body = std::move(body);
    return subr;
}   

std::unique_ptr<Decl> Parser::parse_struct_decl() {
    advance(); // consume 'struct'
    expect(TokenType::Ident);
    std::string name = current.lexeme;
    advance();
    expect(TokenType::LBrace);
    advance(); // consume '{'
    
    std::vector<std::pair<std::string, std::string>> fields;
    while (current.type != TokenType::RBrace) {
        expect(TokenType::Ident);
        std::string field_name = current.lexeme;
        advance();
        expect(TokenType::Colon);
        advance(); // consume ':'
        expect(TokenType::Ident);
        std::string field_type = current.lexeme;
        advance();
        expect(TokenType::Semi);
        advance(); // consume ';'
        fields.emplace_back(field_name, field_type);
    }
    expect(TokenType::RBrace);
    advance(); // consume '}'
    
    auto struct_decl = std::make_unique<StructDecl>(name);
    struct_decl->fields = std::move(fields);
    return struct_decl;
}

std::unique_ptr<Decl> Parser::parse_enum_decl() {
    advance(); // consume 'enum'
    expect(TokenType::Ident);
    std::string name = current.lexeme;
    advance();
    expect(TokenType::LBrace);
    advance(); // consume '{'
    
    std::vector<std::string> values;
    while (current.type != TokenType::RBrace) {
        expect(TokenType::Ident);
        values.push_back(current.lexeme);
        advance();
        if (current.type == TokenType::Comma) {
            advance(); // consume ','
        } else {
            break;
        }
    }
    expect(TokenType::RBrace);
    advance(); // consume '}'
    auto enum_decl = std::make_unique<EnumDecl>(name);
    enum_decl->values = std::move(values);
    return enum_decl;
}  
std::unique_ptr<Decl> Parser::parse_union_decl() {
    advance(); // consume 'union'
    expect(TokenType::Ident);
    std::string name = current.lexeme;
    advance();
    expect(TokenType::LBrace);
    advance(); // consume '{'
    
    std::vector<std::pair<std::string, std::string>> variants;
    while (current.type != TokenType::RBrace) {
        expect(TokenType::Ident);
        std::string variant_name = current.lexeme;
        advance();
        expect(TokenType::Colon);
        advance(); // consume ':'
        expect(TokenType::Ident);
        std::string variant_type = current.lexeme;
        advance();
        expect(TokenType::Semi);
        advance(); // consume ';'
        variants.emplace_back(variant_name, variant_type);
    }
    expect(TokenType::RBrace);
    advance(); // consume '}'
    
    auto union_decl = std::make_unique<UnionDecl>(name);
    union_decl->variants = std::move(variants);
    return union_decl;
}   
std::unique_ptr<Decl> Parser::parse_tool_decl() {
    advance(); // consume 'tool'
    expect(TokenType::Ident);
    std::string name = current.lexeme;
    advance();
    expect(TokenType::LBrace);
    advance(); // consume '{'
    
    std::vector<std::unique_ptr<Decl>> methods;
    while (current.type != TokenType::RBrace) {
        if (current.type == TokenType::KwSubr) {
            methods.push_back(parse_subr_decl());
        } else {
            throw std::runtime_error("Expected 'subr' in tool declaration");
        }
    }
    expect(TokenType::RBrace);
    advance(); // consume '}'
    
    auto tool_decl = std::make_unique<ToolDecl>(name);
    tool_decl->methods = std::move(methods);
    return tool_decl;
}

std::unique_ptr<Decl> Parser::parse_kit_decl() {
    advance(); // consume 'kit'
    expect(TokenType::Ident);
    std::string name = current.lexeme;
    advance();
    expect(TokenType::LBrace);
    advance(); // consume '{'
    
    std::vector<std::unique_ptr<Decl>> exports;
    while (current.type != TokenType::RBrace) {
        if (current.type == TokenType::KwSubr || current.type == TokenType::KwStruct ||
            current.type == TokenType::KwEnum || current.type == TokenType::KwUnion ||
            current.type == TokenType::KwTool) {
            exports.push_back(parse_decl());
        } else {
            throw std::runtime_error("Expected declaration in kit");
        }
    }
    expect(TokenType::RBrace);
    advance(); // consume '}'
    
    auto kit_decl = std::make_unique<KitDecl>(name);
    kit_decl->exports = std::move(exports);
    return kit_decl;
}

std::unique_ptr<Decl> Parser::parse_decl() {
    if (current.type == TokenType::KwLet) return parse_var_decl();
    if (current.type == TokenType::KwSubr) return parse_subr_decl();
    if (current.type == TokenType::KwStruct) return parse_struct_decl();
    if (current.type == TokenType::KwEnum) return parse_enum_decl();
    if (current.type == TokenType::KwUnion) return parse_union_decl();
    if (current.type == TokenType::KwTool) return parse_tool_decl();
    if (current.type == TokenType::KwKit) return parse_kit_decl();
    throw std::runtime_error("Expected declaration");
}

