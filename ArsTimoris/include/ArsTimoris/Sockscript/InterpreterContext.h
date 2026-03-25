#pragma once
#ifndef ARS_TIMORIS__SOCKSCRIPT__INTERPRETER_CONTEXT_H
#define ARS_TIMORIS__SOCKSCRIPT__INTERPRETER_CONTEXT_H
#include <vector>
#include <string>

enum struct ReaderMode {
    READ,
    VAR_NAME,
    VAR_TYPE,
    VAR_VALUE,
    VAR_EXPR,
    RET_EXPR,
    DEL_EXPR,
    OBJECT,
    EXPRESSION,
    ARGS,
    FALLTHROUGH,
    FINISHED,
};

std::string ReaderModeDisplayName(ReaderMode a_mode);

enum struct OperatorEvaluation {
    NONE,
    ADDI,
    SUBT,
    MULT,
    DIVI,
    MODU,
    LSHIFT,
    RSHIFT,
    BAND,
    BOR,
    BXOR,
    BINV,
    NOT,
    AND,
    OR,
    GT,
    GTE,
    LT,
    LTE,
    EQ,
    NEQ,
};

std::string OperatorEvaluationDisplayName(OperatorEvaluation a_operator);

enum struct RegisterType {
    ERROR,
    STRING,
    INT,
    FLOAT,
    BOOL,
};

struct InterpreterContext {
public:
    std::vector<std::string> stringRegisters = {"", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""};
    std::vector<int32_t> intRegisters = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::vector<float> floatRegisters = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::vector<bool> boolRegisters = {true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true};
    std::vector<std::string> context = std::vector<std::string>();
    int32_t stringRegister = 0;
    int32_t intRegister = -1;
    int32_t floatRegister = -1;
    int32_t boolRegister = -1;
    size_t targetLevel = 0;
    size_t currentLevel = 0;
    RegisterType lastRegister = RegisterType::ERROR;
    OperatorEvaluation operatorEvaluation = OperatorEvaluation::NONE;
    std::vector<ReaderMode> mode = {ReaderMode::READ};

    void DumpStringRegister(void);

    void DumpIntRegister(void);
    
    void DumpFloatRegister(void);
    
    void DumpBoolRegister(void);

    RegisterType SetIntRegister(int32_t a_value);

    RegisterType SetFloatRegister(float a_value);

    RegisterType SetBoolRegister(bool a_value);
};
#endif