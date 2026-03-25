#pragma once
#ifndef ARS_TIMORIS__SOCKSCRIPT__INTERPRETER_H
#define ARS_TIMORIS__SOCKSCRIPT__INTERPRETER_H
struct GameState;

#define SCRIPT_PARSER_DEBUG_LOGGING 1
//#define LEGACY_COMMA_REPLACEMENT 1

#include <ArsTimoris/Sockscript/InterpreterContext.h>
#include <ArsTimoris/DataComponents/DataContainer.h>

struct Interpreter {
public:
    DataComponents::DataContainer variables = DataComponents::DataContainer();

    Interpreter(void);

    template <typename T>
    T ParseStatement(GameState& a_gameState, RegisterType a_returnType, std::string a_statement);

    int32_t ParseIntExpression(GameState& a_gameState, InterpreterContext& a_context, std::string a_expression);

    float ParseFloatExpression(GameState& a_gameState, InterpreterContext& a_context, std::string a_expression);

    int32_t ParseBoolExpression(GameState& a_gameState, InterpreterContext& a_context, std::string a_expression);

    std::string FormatString(GameState& a_gameState, InterpreterContext& a_context, std::string a_string);

    void ParseRawType(GameState& a_gameState, InterpreterContext& a_context);

    void ParseVariableName(GameState& a_gameState, InterpreterContext& a_context);

    void ParseObjectName(GameState& a_gameState, InterpreterContext& a_context);

    void ParseObjectMember(GameState& a_gameState, InterpreterContext& a_context);

    void ParseObjectMethod(GameState& a_gameState, InterpreterContext& a_context);

    void ParseObjectMethodArgs(GameState& a_gameState, InterpreterContext& a_context);
};

#include <ArsTimoris/Sockscript/Interpreter.tpp>
#endif