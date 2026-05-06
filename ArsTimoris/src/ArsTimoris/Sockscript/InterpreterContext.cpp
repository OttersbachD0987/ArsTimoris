#include <ArsTimoris/Sockscript/InterpreterContext.h>
#include <iostream>
#include <format>
#include <ArsTimoris/Util/DebugLogging.hpp>

std::string ReaderModeDisplayName(ReaderMode a_mode) {
    switch (a_mode) {
        case ReaderMode::READ:
            return "READING";
        case ReaderMode::VAR_NAME:
            return "VAR_NAM";
        case ReaderMode::VAR_TYPE:
            return "VAR_TYP";
        case ReaderMode::VAR_VALUE:
            return "VAR_VAL";
        case ReaderMode::VAR_EXPR:
            return "VAR_EXP";
        case ReaderMode::RET_EXPR:
            return "RET_EXP";
        case ReaderMode::DEL_EXPR:
            return "DEL_EXP";
        case ReaderMode::OBJECT:
            return "OBJECTS";
        case ReaderMode::EXPRESSION:
            return "EXPRESS";
        case ReaderMode::ARGS:
            return "ARGUMEN";
        case ReaderMode::FALLTHROUGH:
            return "FALLTHR";
        case ReaderMode::FINISHED:
            return "FINISHE";
        default:
            return "INVALID";
    }
}

std::string OperatorEvaluationDisplayName(OperatorEvaluation a_operator) {
    switch (a_operator) {
        case OperatorEvaluation::NONE:
            return "NON";
        case OperatorEvaluation::ADDI:
            return "ADD";
        case OperatorEvaluation::SUBT:
            return "SUB";
        case OperatorEvaluation::MULT:
            return "MUL";
        case OperatorEvaluation::DIVI:
            return "DIV";
        case OperatorEvaluation::MODU:
            return "MOD";
        case OperatorEvaluation::LSHIFT:
            return "LSH";
        case OperatorEvaluation::RSHIFT:
            return "RSH";
        case OperatorEvaluation::BAND:
            return "BAN";
        case OperatorEvaluation::BOR:
            return "BOR";
        case OperatorEvaluation::BXOR:
            return "BXO";
        case OperatorEvaluation::BINV:
            return "BIN";
        case OperatorEvaluation::NOT:
            return "NOT";
        case OperatorEvaluation::AND:
            return "AND";
        case OperatorEvaluation::OR:
            return "LOR";
        case OperatorEvaluation::GT:
            return "GRT";
        case OperatorEvaluation::GTE:
            return "GTE";
        case OperatorEvaluation::LT:
            return "LST";
        case OperatorEvaluation::LTE:
            return "LTE";
        case OperatorEvaluation::EQ:
            return "EQU";
        case OperatorEvaluation::NEQ:
            return "NEQ";
        default:
            return "IVD";
    }
}

std::string RegisterTypeDisplayName(RegisterType a_registerType) {
    switch (a_registerType) {
        case RegisterType::ERROR_TYPE:
            return "ERR";
        case RegisterType::STRING:
            return "STR";
        case RegisterType::INT:
            return "INT";
        case RegisterType::FLOAT:
            return "FLT";
        case RegisterType::BOOL:
            return "BOL";
        default:
            return "IVD";
    }
}

void InterpreterContext::DumpStringRegister(void) {
    for (size_t i = 0; i < stringRegisters.size(); ++i) {
        DebugLogging::ParsingOut << std::format("{}String Register {:0>2}: {}\x1b[39m", i == stringRegister ? "\x1b[32m" : "\x1b[34m", i + 1, stringRegisters[i]) << std::endl;
    }
}

void InterpreterContext::DumpIntRegister(void) {
    for (size_t i = 0; i < intRegisters.size(); ++i) {
        DebugLogging::ParsingOut << (i == intRegister ? "\x1b[32m" : "\x1b[34m") << "Int Register " << (i + 1) << ": " << intRegisters[i] << "\x1b[39m" << std::endl;
    }
}

void InterpreterContext::DumpFloatRegister(void) {
    for (size_t i = 0; i < floatRegisters.size(); ++i) {
        DebugLogging::ParsingOut << (i == floatRegister ? "\x1b[32m" : "\x1b[34m") << "Float Register " << (i + 1) << ": " << floatRegisters[i] << "\x1b[39m" << std::endl;
    }
}

void InterpreterContext::DumpBoolRegister(void) {
    for (size_t i = 0; i < boolRegisters.size(); ++i) {
        DebugLogging::ParsingOut << (i == boolRegister ? "\x1b[32m" : "\x1b[34m") << "Bool Register " << (i + 1) << ": " << (boolRegisters[i] ? "true" : "false") << "\x1b[39m" << std::endl;
    }
}

RegisterType InterpreterContext::SetIntRegister(int32_t a_value) {
    switch (operatorEvaluation) {
        case OperatorEvaluation::NONE:
            intRegisters[++intRegister] = a_value;
            lastRegister = RegisterType::INT;
            operatorEvaluation = OperatorEvaluation::NONE;
            return RegisterType::INT;
        case OperatorEvaluation::ADDI:
            operatorEvaluation = OperatorEvaluation::NONE;
            switch (lastRegister) {
                case RegisterType::INT:
                    intRegisters[intRegister] = intRegisters[intRegister] + a_value;
                    lastRegister = RegisterType::INT;
                    return RegisterType::INT;
                case RegisterType::FLOAT:
                    intRegisters[intRegister] = (int32_t)floatRegisters[floatRegister] + a_value;
                    lastRegister = RegisterType::FLOAT;
                    return RegisterType::FLOAT;
                case RegisterType::BOOL:
                case RegisterType::STRING:
                case RegisterType::ERROR_TYPE:
                    DebugLogging::ParsingOut << std::format("[ERROR] Can not ADD register of type INT '{}' to a register of type {}.", a_value, RegisterTypeDisplayName(lastRegister)) << std::endl;
                    lastRegister = RegisterType::ERROR_TYPE;
                    return RegisterType::ERROR_TYPE;
            }
        case OperatorEvaluation::SUBT:
            operatorEvaluation = OperatorEvaluation::NONE;
            switch (lastRegister) {
                case RegisterType::INT:
                    intRegisters[intRegister] = intRegisters[intRegister] - a_value;
                    lastRegister = RegisterType::INT;
                    return RegisterType::INT;
                case RegisterType::FLOAT:
                    intRegisters[intRegister] = (int32_t)floatRegisters[floatRegister] - a_value;
                    lastRegister = RegisterType::FLOAT;
                    return RegisterType::FLOAT;
                case RegisterType::BOOL:
                case RegisterType::STRING:
                case RegisterType::ERROR_TYPE:
                    DebugLogging::ParsingOut << std::format("[ERROR] Can not SUB register of type INT '{}' to a register of type {}.", a_value, RegisterTypeDisplayName(lastRegister)) << std::endl;
                    lastRegister = RegisterType::ERROR_TYPE;
                    return RegisterType::ERROR_TYPE;
            }
        case OperatorEvaluation::MULT:
            operatorEvaluation = OperatorEvaluation::NONE;
            switch (lastRegister) {
                case RegisterType::INT:
                    intRegisters[intRegister] = intRegisters[intRegister] * a_value;
                    lastRegister = RegisterType::INT;
                    return RegisterType::INT;
                case RegisterType::FLOAT:
                    intRegisters[intRegister] = (int32_t)(floatRegisters[floatRegister] * a_value);
                    lastRegister = RegisterType::FLOAT;
                    return RegisterType::FLOAT;
                case RegisterType::BOOL:
                case RegisterType::STRING:
                case RegisterType::ERROR_TYPE:
                    DebugLogging::ParsingOut << std::format("[ERROR] Can not MUL register of type INT '{}' to a register of type {}.", a_value, RegisterTypeDisplayName(lastRegister)) << std::endl;
                    lastRegister = RegisterType::ERROR_TYPE;
                    return RegisterType::ERROR_TYPE;
            }
        case OperatorEvaluation::DIVI:
            operatorEvaluation = OperatorEvaluation::NONE;
            switch (lastRegister) {
                case RegisterType::INT:
                    intRegisters[intRegister] = intRegisters[intRegister] / a_value;
                    lastRegister = RegisterType::INT;
                    return RegisterType::INT;
                case RegisterType::FLOAT:
                    intRegisters[intRegister] = (int32_t)(floatRegisters[floatRegister] / a_value);
                    lastRegister = RegisterType::FLOAT;
                    return RegisterType::FLOAT;
                case RegisterType::BOOL:
                case RegisterType::STRING:
                case RegisterType::ERROR_TYPE:
                    DebugLogging::ParsingOut << std::format("[ERROR] Can not DIV register of type INT '{}' to a register of type {}.", a_value, RegisterTypeDisplayName(lastRegister)) << std::endl;
                    lastRegister = RegisterType::ERROR_TYPE;
                    return RegisterType::ERROR_TYPE;
            }
        case OperatorEvaluation::MODU:
            intRegisters[intRegister] = intRegisters[intRegister] % a_value;
            lastRegister = RegisterType::INT;
            operatorEvaluation = OperatorEvaluation::NONE;
            return RegisterType::INT;
        case OperatorEvaluation::LSHIFT:
            intRegisters[intRegister] = intRegisters[intRegister] << a_value;
            lastRegister = RegisterType::INT;
            operatorEvaluation = OperatorEvaluation::NONE;
            return RegisterType::INT;
        case OperatorEvaluation::RSHIFT:
            intRegisters[intRegister] = intRegisters[intRegister] >> a_value;
            lastRegister = RegisterType::INT;
            operatorEvaluation = OperatorEvaluation::NONE;
            return RegisterType::INT;
        case OperatorEvaluation::BAND:
            intRegisters[intRegister] = intRegisters[intRegister] & a_value;
            lastRegister = RegisterType::INT;
            operatorEvaluation = OperatorEvaluation::NONE;
            return RegisterType::INT;
        case OperatorEvaluation::BOR:
            intRegisters[intRegister] = intRegisters[intRegister] | a_value;
            lastRegister = RegisterType::INT;
            operatorEvaluation = OperatorEvaluation::NONE;
            return RegisterType::INT;
        case OperatorEvaluation::BXOR:
            intRegisters[intRegister] = intRegisters[intRegister] ^ a_value;
            lastRegister = RegisterType::INT;
            operatorEvaluation = OperatorEvaluation::NONE;
            return RegisterType::INT;
        case OperatorEvaluation::BINV:
            intRegisters[intRegister] = ~a_value;
            lastRegister = RegisterType::INT;
            operatorEvaluation = OperatorEvaluation::NONE;
            return RegisterType::INT;
        case OperatorEvaluation::GT:
            operatorEvaluation = OperatorEvaluation::NONE;
            switch (lastRegister) {
                case RegisterType::INT:
                    return SetBoolRegister(intRegisters[intRegister] > a_value);
                case RegisterType::FLOAT:
                    return SetBoolRegister(floatRegisters[floatRegister] > a_value);
                case RegisterType::BOOL:
                case RegisterType::STRING:
                case RegisterType::ERROR_TYPE:
                    DebugLogging::ParsingOut << std::format("[ERROR] Can not GRT register of type INT '{}' to a register of type {}.", a_value, RegisterTypeDisplayName(lastRegister)) << std::endl;
                    lastRegister = RegisterType::ERROR_TYPE;
                    return RegisterType::ERROR_TYPE;
            }
        case OperatorEvaluation::GTE:
            operatorEvaluation = OperatorEvaluation::NONE;
            switch (lastRegister) {
                case RegisterType::INT:
                    return SetBoolRegister(intRegisters[intRegister] >= a_value);
                case RegisterType::FLOAT:
                    return SetBoolRegister(floatRegisters[floatRegister] >= a_value);
                case RegisterType::BOOL:
                case RegisterType::STRING:
                case RegisterType::ERROR_TYPE:
                    DebugLogging::ParsingOut << std::format("[ERROR] Can not GTE register of type INT '{}' to a register of type {}.", a_value, RegisterTypeDisplayName(lastRegister)) << std::endl;
                    lastRegister = RegisterType::ERROR_TYPE;
                    return RegisterType::ERROR_TYPE;
            }
        case OperatorEvaluation::LT:
            operatorEvaluation = OperatorEvaluation::NONE;
            switch (lastRegister) {
                case RegisterType::INT:
                    return SetBoolRegister(intRegisters[intRegister] < a_value);
                case RegisterType::FLOAT:
                    return SetBoolRegister(floatRegisters[floatRegister] < a_value);
                case RegisterType::BOOL:
                case RegisterType::STRING:
                case RegisterType::ERROR_TYPE:
                    DebugLogging::ParsingOut << std::format("[ERROR] Can not LST register of type INT '{}' to a register of type {}.", a_value, RegisterTypeDisplayName(lastRegister)) << std::endl;
                    lastRegister = RegisterType::ERROR_TYPE;
                    return RegisterType::ERROR_TYPE;
            }
        case OperatorEvaluation::LTE:
            operatorEvaluation = OperatorEvaluation::NONE;
            switch (lastRegister) {
                case RegisterType::INT:
                    return SetBoolRegister(intRegisters[intRegister] <= a_value);
                case RegisterType::FLOAT:
                    return SetBoolRegister(floatRegisters[floatRegister] <= a_value);
                case RegisterType::BOOL:
                case RegisterType::STRING:
                case RegisterType::ERROR_TYPE:
                    DebugLogging::ParsingOut << std::format("[ERROR] Can not LTE register of type INT '{}' to a register of type {}.", a_value, RegisterTypeDisplayName(lastRegister)) << std::endl;
                    lastRegister = RegisterType::ERROR_TYPE;
                    return RegisterType::ERROR_TYPE;
            }
        case OperatorEvaluation::EQ:
            operatorEvaluation = OperatorEvaluation::NONE;
            switch (lastRegister) {
                case RegisterType::INT:
                    return SetBoolRegister(intRegisters[intRegister] == a_value);
                case RegisterType::FLOAT:
                    return SetBoolRegister(floatRegisters[floatRegister] == a_value);
                case RegisterType::BOOL:
                case RegisterType::STRING:
                case RegisterType::ERROR_TYPE:
                    DebugLogging::ParsingOut << std::format("[ERROR] Can not EQU register of type INT '{}' to a register of type {}.", a_value, RegisterTypeDisplayName(lastRegister)) << std::endl;
                    lastRegister = RegisterType::ERROR_TYPE;
                    return RegisterType::ERROR_TYPE;
            }
        case OperatorEvaluation::NEQ:
            operatorEvaluation = OperatorEvaluation::NONE;
            switch (lastRegister) {
                case RegisterType::INT:
                    return SetBoolRegister(intRegisters[intRegister] != a_value);
                case RegisterType::FLOAT:
                    return SetBoolRegister(floatRegisters[floatRegister] != a_value);
                case RegisterType::BOOL:
                case RegisterType::STRING:
                case RegisterType::ERROR_TYPE:
                    DebugLogging::ParsingOut << std::format("[ERROR] Can not NEQ register of type INT '{}' to a register of type {}.", a_value, RegisterTypeDisplayName(lastRegister)) << std::endl;
                    lastRegister = RegisterType::ERROR_TYPE;
                    return RegisterType::ERROR_TYPE;
            }
        default:
            DebugLogging::ParsingOut << std::format("[ERROR] Can not {} register of type INT '{}' to a register of type {}.", OperatorEvaluationDisplayName(operatorEvaluation), a_value, RegisterTypeDisplayName(lastRegister)) << std::endl;
            lastRegister = RegisterType::ERROR_TYPE;
            operatorEvaluation = OperatorEvaluation::NONE;
            return RegisterType::ERROR_TYPE;
    }
    
}

RegisterType InterpreterContext::SetFloatRegister(float a_value) {
    switch (operatorEvaluation) {
        case OperatorEvaluation::NONE:
            floatRegisters[++floatRegister] = a_value;
            lastRegister = RegisterType::FLOAT;
            operatorEvaluation = OperatorEvaluation::NONE;
            return RegisterType::FLOAT;
        case OperatorEvaluation::ADDI:
            operatorEvaluation = OperatorEvaluation::NONE;
            switch (lastRegister) {
                case RegisterType::INT:
                    floatRegisters[floatRegister] = intRegisters[intRegister] + a_value;
                    break;
                case RegisterType::FLOAT:
                    floatRegisters[floatRegister] = floatRegisters[floatRegister] + a_value;
                    break;
                case RegisterType::BOOL:
                case RegisterType::STRING:
                case RegisterType::ERROR_TYPE:
                    DebugLogging::ParsingOut << std::format("[ERROR] Can not ADD register of type FLT '{}' to a register of type {}.", a_value, RegisterTypeDisplayName(lastRegister)) << std::endl;
                    lastRegister = RegisterType::ERROR_TYPE;
                    return RegisterType::ERROR_TYPE;
            }
            lastRegister = RegisterType::FLOAT;
            return RegisterType::FLOAT;
        case OperatorEvaluation::SUBT:
            operatorEvaluation = OperatorEvaluation::NONE;
            switch (lastRegister) {
                case RegisterType::INT:
                    floatRegisters[floatRegister] = intRegisters[intRegister] - a_value;
                    break;
                case RegisterType::FLOAT:
                    floatRegisters[floatRegister] = floatRegisters[floatRegister] - a_value;
                    break;
                case RegisterType::BOOL:
                case RegisterType::STRING:
                case RegisterType::ERROR_TYPE:
                    DebugLogging::ParsingOut << std::format("[ERROR] Can not SUB register of type FLT '{}' to a register of type {}.", a_value, RegisterTypeDisplayName(lastRegister)) << std::endl;
                    lastRegister = RegisterType::ERROR_TYPE;
                    return RegisterType::ERROR_TYPE;
            }
            lastRegister = RegisterType::FLOAT;
            return RegisterType::FLOAT;
        case OperatorEvaluation::MULT:
            operatorEvaluation = OperatorEvaluation::NONE;
            switch (lastRegister) {
                case RegisterType::INT:
                    floatRegisters[floatRegister] = intRegisters[intRegister] * a_value;
                    break;
                case RegisterType::FLOAT:
                    floatRegisters[floatRegister] = floatRegisters[floatRegister] * a_value;
                    break;
                case RegisterType::BOOL:
                case RegisterType::STRING:
                case RegisterType::ERROR_TYPE:
                    DebugLogging::ParsingOut << std::format("[ERROR] Can not MUL register of type FLT '{}' to a register of type {}.", a_value, RegisterTypeDisplayName(lastRegister)) << std::endl;
                    lastRegister = RegisterType::ERROR_TYPE;
                    return RegisterType::ERROR_TYPE;
            }
            lastRegister = RegisterType::FLOAT;
            return RegisterType::FLOAT;
        case OperatorEvaluation::DIVI:
            operatorEvaluation = OperatorEvaluation::NONE;
            switch (lastRegister) {
                case RegisterType::INT:
                    floatRegisters[floatRegister] = intRegisters[intRegister] / a_value;
                    break;
                case RegisterType::FLOAT:
                    floatRegisters[floatRegister] = floatRegisters[floatRegister] / a_value;
                    break;
                case RegisterType::BOOL:
                case RegisterType::STRING:
                case RegisterType::ERROR_TYPE:
                    DebugLogging::ParsingOut << std::format("[ERROR] Can not DIV register of type FLT '{}' to a register of type {}.", a_value, RegisterTypeDisplayName(lastRegister)) << std::endl;
                    lastRegister = RegisterType::ERROR_TYPE;
                    return RegisterType::ERROR_TYPE;
            }
            lastRegister = RegisterType::FLOAT;
            return RegisterType::FLOAT;
        case OperatorEvaluation::GT:
            operatorEvaluation = OperatorEvaluation::NONE;
            switch (lastRegister) {
                case RegisterType::INT:
                    return SetBoolRegister(intRegisters[intRegister] > a_value);
                case RegisterType::FLOAT:
                    return SetBoolRegister(floatRegisters[floatRegister] > a_value);
                case RegisterType::BOOL:
                case RegisterType::STRING:
                case RegisterType::ERROR_TYPE:
                    DebugLogging::ParsingOut << std::format("[ERROR] Can not GRT register of type FLT '{}' to a register of type {}.", a_value, RegisterTypeDisplayName(lastRegister)) << std::endl;
                    lastRegister = RegisterType::ERROR_TYPE;
                    return RegisterType::ERROR_TYPE;
            }
        case OperatorEvaluation::GTE:
            operatorEvaluation = OperatorEvaluation::NONE;
            switch (lastRegister) {
                case RegisterType::INT:
                    return SetBoolRegister(intRegisters[intRegister] >= a_value);
                case RegisterType::FLOAT:
                    return SetBoolRegister(floatRegisters[floatRegister] >= a_value);
                case RegisterType::BOOL:
                case RegisterType::STRING:
                case RegisterType::ERROR_TYPE:
                    DebugLogging::ParsingOut << std::format("[ERROR] Can not GTE register of type FLT '{}' to a register of type {}.", a_value, RegisterTypeDisplayName(lastRegister)) << std::endl;
                    lastRegister = RegisterType::ERROR_TYPE;
                    return RegisterType::ERROR_TYPE;
            }
        case OperatorEvaluation::LT:
            operatorEvaluation = OperatorEvaluation::NONE;
            switch (lastRegister) {
                case RegisterType::INT:
                    return SetBoolRegister(intRegisters[intRegister] < a_value);
                case RegisterType::FLOAT:
                    return SetBoolRegister(floatRegisters[floatRegister] < a_value);
                case RegisterType::BOOL:
                case RegisterType::STRING:
                case RegisterType::ERROR_TYPE:
                    DebugLogging::ParsingOut << std::format("[ERROR] Can not LST register of type FLT '{}' to a register of type {}.", a_value, RegisterTypeDisplayName(lastRegister)) << std::endl;
                    lastRegister = RegisterType::ERROR_TYPE;
                    return RegisterType::ERROR_TYPE;
            }
        case OperatorEvaluation::LTE:
            operatorEvaluation = OperatorEvaluation::NONE;
            switch (lastRegister) {
                case RegisterType::INT:
                    return SetBoolRegister(intRegisters[intRegister] <= a_value);
                case RegisterType::FLOAT:
                    return SetBoolRegister(floatRegisters[floatRegister] <= a_value);
                case RegisterType::BOOL:
                case RegisterType::STRING:
                case RegisterType::ERROR_TYPE:
                    DebugLogging::ParsingOut << std::format("[ERROR] Can not LTE register of type FLT '{}' to a register of type {}.", a_value, RegisterTypeDisplayName(lastRegister)) << std::endl;
                    lastRegister = RegisterType::ERROR_TYPE;
                    return RegisterType::ERROR_TYPE;
            }
        case OperatorEvaluation::EQ:
            operatorEvaluation = OperatorEvaluation::NONE;
            switch (lastRegister) {
                case RegisterType::INT:
                    return SetBoolRegister(intRegisters[intRegister] == a_value);
                case RegisterType::FLOAT:
                    return SetBoolRegister(floatRegisters[floatRegister] == a_value);
                case RegisterType::BOOL:
                case RegisterType::STRING:
                case RegisterType::ERROR_TYPE:
                    DebugLogging::ParsingOut << std::format("[ERROR] Can not EQU register of type FLT '{}' to a register of type {}.", a_value, RegisterTypeDisplayName(lastRegister)) << std::endl;
                    lastRegister = RegisterType::ERROR_TYPE;
                    return RegisterType::ERROR_TYPE;
            }
        case OperatorEvaluation::NEQ:
            operatorEvaluation = OperatorEvaluation::NONE;
            switch (lastRegister) {
                case RegisterType::INT:
                    return SetBoolRegister(intRegisters[intRegister] != a_value);
                case RegisterType::FLOAT:
                    return SetBoolRegister(floatRegisters[floatRegister] != a_value);
                case RegisterType::BOOL:
                case RegisterType::STRING:
                case RegisterType::ERROR_TYPE:
                    DebugLogging::ParsingOut << std::format("[ERROR] Can not NEQ register of type FLT '{}' to a register of type {}.", a_value, RegisterTypeDisplayName(lastRegister)) << std::endl;
                    lastRegister = RegisterType::ERROR_TYPE;
                    return RegisterType::ERROR_TYPE;
            }
        default:
            DebugLogging::ParsingOut << std::format("[ERROR] Can not {} register of type FLT '{}' to a register of type {}.", OperatorEvaluationDisplayName(operatorEvaluation), a_value, RegisterTypeDisplayName(lastRegister)) << std::endl;
            lastRegister = RegisterType::ERROR_TYPE;
            operatorEvaluation = OperatorEvaluation::NONE;
            return RegisterType::ERROR_TYPE;
    }
}

RegisterType InterpreterContext::SetBoolRegister(bool a_value) {
    switch (operatorEvaluation) {
        case OperatorEvaluation::NONE:
            boolRegisters[++boolRegister] = a_value;
            lastRegister = RegisterType::BOOL;
            operatorEvaluation = OperatorEvaluation::NONE;
            return RegisterType::BOOL;
        case OperatorEvaluation::NOT:
            boolRegisters[++boolRegister] = !a_value;
            lastRegister = RegisterType::BOOL;
            operatorEvaluation = OperatorEvaluation::NONE;
            return RegisterType::BOOL;
        case OperatorEvaluation::AND:
            boolRegisters[boolRegister] = boolRegisters[boolRegister] && a_value;
            lastRegister = RegisterType::BOOL;
            operatorEvaluation = OperatorEvaluation::NONE;
            return RegisterType::BOOL;
        case OperatorEvaluation::OR:
            boolRegisters[boolRegister] = boolRegisters[boolRegister] || a_value;
            lastRegister = RegisterType::BOOL;
            operatorEvaluation = OperatorEvaluation::NONE;
            return RegisterType::BOOL;
        case OperatorEvaluation::EQ:
            boolRegisters[boolRegister] = boolRegisters[boolRegister] == a_value;
            lastRegister = RegisterType::BOOL;
            operatorEvaluation = OperatorEvaluation::NONE;
            return RegisterType::BOOL;
        case OperatorEvaluation::NEQ:
            boolRegisters[boolRegister] = boolRegisters[boolRegister] != a_value;
            lastRegister = RegisterType::BOOL;
            operatorEvaluation = OperatorEvaluation::NONE;
            return RegisterType::BOOL;
        default:
            DebugLogging::ParsingOut << std::format("[ERROR] Can not {} register of type BOL '{}' to a register of type {}.", OperatorEvaluationDisplayName(operatorEvaluation), a_value, RegisterTypeDisplayName(lastRegister)) << std::endl;
            lastRegister = RegisterType::ERROR_TYPE;
            operatorEvaluation = OperatorEvaluation::NONE;
            return RegisterType::ERROR_TYPE;
    }
}