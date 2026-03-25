#include <ArsTimoris/Sockscript/Interpreter.h>
#include <ArsTimoris/Game/GameState.h>
#include <iostream>
#include <format>
#include <ArsTimoris/Util/String.hpp>
#include <ArsTimoris/Util/Random.hpp>

Interpreter::Interpreter(void) {

}

int32_t Interpreter::ParseIntExpression(GameState& a_gameState, InterpreterContext& a_context, std::string a_expression) {
    #ifdef SCRIPT_PARSER_DEBUG_LOGGING
    DebugLogging::ParsingOut << "Parsing Int Expression: \n" << a_expression << std::endl;
    #endif
    InterpreterContext context = InterpreterContext();

    bool inString = false;
    bool escaped = false;
    bool justOperated = false;
    int32_t innerLevel = 0;
    #ifdef SCRIPT_PARSER_DEBUG_LOGGING
    size_t step = 0;
    #endif
    for (char character : a_expression) {
        #ifdef SCRIPT_PARSER_DEBUG_LOGGING
        DebugLogging::ParsingOut << std::format("\x1b[1mINT_EX\x1b[22m Step {:0>4}) [{}] <{}> {}InString\x1b[39m {}Escaped\x1b[39m Register {:0>2}: [{:0>3}] {}", step++, ReaderModeDisplayName(context.mode.back()), OperatorEvaluationDisplayName(context.operatorEvaluation), (inString ? "\x1b[32m" : "\x1b[31m"), (escaped ? "\x1b[32m" : "\x1b[31m"), context.stringRegister, context.stringRegisters[context.stringRegister].size(), context.stringRegisters[context.stringRegister]) << std::endl;
        #endif
        switch (context.mode.back()) {
            case ReaderMode::READ: {
                switch (character) {
                    case ' ':
                    case '\n':
                    case '\t':
                    case '\v':
                        if (context.stringRegisters[context.stringRegister] != "") {
                            ParseRawType(a_gameState, context);
                        }
                        justOperated = false;
                        break;
                    case '@':
                        context.stringRegister += 1;
                        context.mode.push_back(ReaderMode::VAR_NAME);
                        break;
                    case '{':
                    case '}':
                        justOperated = false;
                        DebugLogging::ParsingOut << "[ERROR] Invalid characters." << std::endl;
                        break;
                    case '.':
                        justOperated = false;
                        ParseObjectName(a_gameState, context);
                        break;
                    case '+':
                        context.operatorEvaluation = OperatorEvaluation::ADDI;
                        justOperated = true;
                        break;
                    case '-':
                        context.operatorEvaluation = OperatorEvaluation::SUBT;
                        justOperated = true;
                        break;
                    case '*':
                        context.operatorEvaluation = OperatorEvaluation::MULT;
                        justOperated = true;
                        break;
                    case '/':
                        context.operatorEvaluation = OperatorEvaluation::DIVI;
                        justOperated = true;
                        break;
                    case '!':
                        context.operatorEvaluation = OperatorEvaluation::NOT;
                        justOperated = true;
                        break;
                    case '%':
                        context.operatorEvaluation = OperatorEvaluation::MODU;
                        justOperated = true;
                        break;
                    case '&':
                        context.operatorEvaluation = OperatorEvaluation::BAND;
                        justOperated = true;
                        break;
                    case '|':
                        context.operatorEvaluation = OperatorEvaluation::BOR;
                        justOperated = true;
                        break;
                    case '^':
                        context.operatorEvaluation = OperatorEvaluation::BXOR;
                        justOperated = true;
                        break;
                    case '~':
                        context.operatorEvaluation = OperatorEvaluation::BINV;
                        justOperated = true;
                        break;
                    case '<':
                        switch (context.operatorEvaluation) {
                            case OperatorEvaluation::NONE:
                                context.operatorEvaluation = OperatorEvaluation::LT;
                                break;
                            case OperatorEvaluation::LT:
                                context.operatorEvaluation = OperatorEvaluation::LSHIFT;
                                break;
                        }
                        justOperated = true;
                        break;
                    case '>':
                        switch (context.operatorEvaluation) {
                            case OperatorEvaluation::NONE:
                                context.operatorEvaluation = OperatorEvaluation::GT;
                                break;
                            case OperatorEvaluation::GT:
                                context.operatorEvaluation = OperatorEvaluation::RSHIFT;
                                break;
                        }
                        justOperated = true;
                        break;
                    case '=':
                        switch (context.operatorEvaluation) {
                            case OperatorEvaluation::NONE:
                                context.operatorEvaluation = OperatorEvaluation::EQ;
                                break;
                            case OperatorEvaluation::NOT:
                                context.operatorEvaluation = OperatorEvaluation::NEQ;
                                break;
                            case OperatorEvaluation::LT:
                                context.operatorEvaluation = OperatorEvaluation::LTE;
                                break;
                            case OperatorEvaluation::GT:
                                context.operatorEvaluation = OperatorEvaluation::GTE;
                                break;
                        }
                        justOperated = true;
                        break;
                    default:
                        justOperated = false;
                        context.stringRegisters[context.stringRegister] += character;
                        break;
                }
                break;
            }
            case ReaderMode::VAR_NAME: {
                switch (character) {
                    case '@': {
                        ParseVariableName(a_gameState, context);
                        break;
                    }
                    default:
                        context.stringRegisters[context.stringRegister] += character;
                        break;
                }
                break;
            }
            case ReaderMode::OBJECT: {
                switch (character) {
                    case ' ':
                    case '\n':
                    case '\t':
                    case '\v':
                    case ')': {
                        ParseObjectMember(a_gameState, context);
                        break;
                    }
                    case '(': {
                        ParseObjectMethod(a_gameState, context);
                        break;
                    }
                    default:
                        context.stringRegisters[context.stringRegister] += character;
                        break;
                }
                break;
            }
            case ReaderMode::ARGS: {
                switch (character) {
                    case ' ':
                        context.stringRegisters[context.stringRegister] += character;
                        escaped = false;
                    case '\n':
                    case '\t':
                    case '\v':
                        break;
                    case '"':
                        context.stringRegisters[context.stringRegister] += character;
                        if (inString) {
                            if (!escaped) {
                                inString = false;
                            }
                            escaped = false;
                        } else {
                            inString = true;
                        }
                        break;
                    case '\\':
                        if (inString) {
                            if (escaped) {
                                context.stringRegisters[context.stringRegister] += character;
                                escaped = false;
                            } else {
                                escaped = true;
                            }
                        }
                        break;
                    case 'n':
                        if (inString && escaped) {
                            context.stringRegisters[context.stringRegister] += "\\";
                        }
                        context.stringRegisters[context.stringRegister] += character;
                        break;
                    case '(': {
                        if (!inString) {
                            ++innerLevel;
                        } else {
                            context.stringRegisters[context.stringRegister] += character;
                            escaped = false;
                        }
                        break;
                    }
                    case ')': {
                        if (!inString) {
                            if (innerLevel == 0) {
                                ParseObjectMethodArgs(a_gameState, context);
                            } else {
                                --innerLevel;
                            }
                        } else {
                            context.stringRegisters[context.stringRegister] += character;
                            escaped = false;
                        }
                        break;
                    }
                    default:
                        context.stringRegisters[context.stringRegister] += character;
                        break;
                }
                break;
            }
        }
    }

    #ifdef SCRIPT_PARSER_DEBUG_LOGGING
    DebugLogging::ParsingOut << std::format("\x1b[1mINT_EX\x1b[22m Step {:0>4}) [{}] {}InString\x1b[39m {}Escaped\x1b[39m Register {:0>2}: [{:0>3}] {}", step++, ReaderModeDisplayName(context.mode.back()), (inString ? "\x1b[32m" : "\x1b[31m"), (escaped ? "\x1b[32m" : "\x1b[31m"), context.stringRegister, context.stringRegisters[context.stringRegister].size(), context.stringRegisters[context.stringRegister]) << std::endl;
    #endif

    if (context.stringRegisters[context.stringRegister] != "") {
        switch (context.mode.back()) {
            case ReaderMode::READ:
                ParseRawType(a_gameState, context);
                break;
            case ReaderMode::OBJECT:
                ParseObjectMember(a_gameState, context);
                break;
        }
    }

    int32_t toReturn;
    switch (context.lastRegister) {
        case RegisterType::INT:
            toReturn = context.intRegisters[context.intRegister];
            break;
        case RegisterType::FLOAT:
            toReturn = context.floatRegisters[context.floatRegister];
            break;
        case RegisterType::BOOL:
        case RegisterType::STRING:
        case RegisterType::ERROR:
            toReturn = 0;
            break;
    }

    #ifdef SCRIPT_PARSER_DEBUG_LOGGING
    DebugLogging::ParsingOut << "Returning: " << toReturn << std::endl;
    #endif

    return toReturn;
}

float Interpreter::ParseFloatExpression(GameState& a_gameState, InterpreterContext& a_context, std::string a_expression) {
    #ifdef SCRIPT_PARSER_DEBUG_LOGGING
    DebugLogging::ParsingOut << "Parsing Float Expression: \n" << a_expression << std::endl;
    #endif
    InterpreterContext context = InterpreterContext();

    int32_t innerLevel = 0;

    bool inString = false;
    bool escaped = false;
    bool justOperated = false;
    #ifdef SCRIPT_PARSER_DEBUG_LOGGING
    size_t step = 0;
    #endif
    for (char character : a_expression) {
        #ifdef SCRIPT_PARSER_DEBUG_LOGGING
        DebugLogging::ParsingOut << std::format("\x1b[1mFLT_EX\x1b[22m Step {:0>4}) [{}] <{}> {}InString\x1b[39m {}Escaped\x1b[39m Register {:0>2}: [{:0>3}] {}", step++, ReaderModeDisplayName(context.mode.back()), OperatorEvaluationDisplayName(context.operatorEvaluation), (inString ? "\x1b[32m" : "\x1b[31m"), (escaped ? "\x1b[32m" : "\x1b[31m"), context.stringRegister, context.stringRegisters[context.stringRegister].size(), context.stringRegisters[context.stringRegister]) << std::endl;
        #endif
        switch (context.mode.back()) {
            case ReaderMode::READ: {
                switch (character) {
                    case ' ':
                    case '\n':
                    case '\t':
                    case '\v':
                        if (context.stringRegisters[context.stringRegister] != "") {
                            ParseRawType(a_gameState, context);
                        }
                        justOperated = false;
                        break;
                    case '@':
                        context.stringRegister += 1;
                        context.mode.push_back(ReaderMode::VAR_NAME);
                        break;
                    case '{':
                    case '}':
                        justOperated = false;
                        DebugLogging::ParsingOut << "[ERROR] Invalid characters." << std::endl;
                        break;
                    case '.':
                        justOperated = false;
                        ParseObjectName(a_gameState, context);
                        break;
                    case '+':
                        context.operatorEvaluation = OperatorEvaluation::ADDI;
                        justOperated = true;
                        break;
                    case '-':
                        context.operatorEvaluation = OperatorEvaluation::SUBT;
                        justOperated = true;
                        break;
                    case '*':
                        context.operatorEvaluation = OperatorEvaluation::MULT;
                        justOperated = true;
                        break;
                    case '/':
                        context.operatorEvaluation = OperatorEvaluation::DIVI;
                        justOperated = true;
                        break;
                    case '!':
                        context.operatorEvaluation = OperatorEvaluation::NOT;
                        justOperated = true;
                        break;
                    case '%':
                        context.operatorEvaluation = OperatorEvaluation::MODU;
                        justOperated = true;
                        break;
                    case '&':
                        context.operatorEvaluation = OperatorEvaluation::BAND;
                        justOperated = true;
                        break;
                    case '|':
                        context.operatorEvaluation = OperatorEvaluation::BOR;
                        justOperated = true;
                        break;
                    case '^':
                        context.operatorEvaluation = OperatorEvaluation::BXOR;
                        justOperated = true;
                        break;
                    case '~':
                        context.operatorEvaluation = OperatorEvaluation::BINV;
                        justOperated = true;
                        break;
                    case '<':
                        switch (context.operatorEvaluation) {
                            case OperatorEvaluation::NONE:
                                context.operatorEvaluation = OperatorEvaluation::LT;
                                break;
                            case OperatorEvaluation::LT:
                                context.operatorEvaluation = OperatorEvaluation::LSHIFT;
                                break;
                        }
                        justOperated = true;
                        break;
                    case '>':
                        switch (context.operatorEvaluation) {
                            case OperatorEvaluation::NONE:
                                context.operatorEvaluation = OperatorEvaluation::GT;
                                break;
                            case OperatorEvaluation::GT:
                                context.operatorEvaluation = OperatorEvaluation::RSHIFT;
                                break;
                        }
                        justOperated = true;
                        break;
                    case '=':
                        switch (context.operatorEvaluation) {
                            case OperatorEvaluation::NONE:
                                context.operatorEvaluation = OperatorEvaluation::EQ;
                                break;
                            case OperatorEvaluation::NOT:
                                context.operatorEvaluation = OperatorEvaluation::NEQ;
                                break;
                            case OperatorEvaluation::LT:
                                context.operatorEvaluation = OperatorEvaluation::LTE;
                                break;
                            case OperatorEvaluation::GT:
                                context.operatorEvaluation = OperatorEvaluation::GTE;
                                break;
                        }
                        justOperated = true;
                        break;
                    default:
                        justOperated = false;
                        context.stringRegisters[context.stringRegister] += character;
                        break;
                }
                break;
            }
            case ReaderMode::VAR_NAME: {
                switch (character) {
                    case '@': {
                        ParseVariableName(a_gameState, context);
                        break;
                    }
                    default:
                        context.stringRegisters[context.stringRegister] += character;
                        break;
                }
                break;
            }
            case ReaderMode::OBJECT: {
                switch (character) {
                    case ' ':
                    case '\n':
                    case '\t':
                    case '\v':
                    case ')': {
                        ParseObjectMember(a_gameState, context);
                        break;
                    }
                    case '(': {
                        ParseObjectMethod(a_gameState, context);
                        break;
                    }
                    default:
                        context.stringRegisters[context.stringRegister] += character;
                        break;
                }
                break;
            }
            case ReaderMode::ARGS: {
                switch (character) {
                    case ' ':
                        context.stringRegisters[context.stringRegister] += character;
                        escaped = false;
                    case '\n':
                    case '\t':
                    case '\v':
                        break;
                    case '"':
                        context.stringRegisters[context.stringRegister] += character;
                        if (inString) {
                            if (!escaped) {
                                inString = false;
                            }
                            escaped = false;
                        } else {
                            inString = true;
                        }
                        break;
                    case '\\':
                        if (inString) {
                            if (escaped) {
                                context.stringRegisters[context.stringRegister] += character;
                                escaped = false;
                            } else {
                                escaped = true;
                            }
                        }
                        break;
                    case 'n':
                        if (inString && escaped) {
                            context.stringRegisters[context.stringRegister] += "\\";
                        }
                        context.stringRegisters[context.stringRegister] += character;
                        break;
                    case '(': {
                        if (!inString) {
                            ++innerLevel;
                        }
                        context.stringRegisters[context.stringRegister] += character;
                        escaped = false;
                        break;
                    }
                    case ')': {
                        if (!inString) {
                            if (innerLevel == 0) {
                                ParseObjectMethodArgs(a_gameState, context);
                            } else {
                                --innerLevel;
                                context.stringRegisters[context.stringRegister] += character;
                                escaped = false;
                            }
                        } else {
                            context.stringRegisters[context.stringRegister] += character;
                            escaped = false;
                        }
                        break;
                    }
                    default:
                        context.stringRegisters[context.stringRegister] += character;
                        break;
                }
                break;
            }
        }
    }

    #ifdef SCRIPT_PARSER_DEBUG_LOGGING
    DebugLogging::ParsingOut << std::format("\x1b[1mFLT_EX\x1b[22m Step {:0>4}) [{}] {}InString\x1b[39m {}Escaped\x1b[39m Register {:0>2}: [{:0>3}] {}", step++, ReaderModeDisplayName(context.mode.back()), (inString ? "\x1b[32m" : "\x1b[31m"), (escaped ? "\x1b[32m" : "\x1b[31m"), context.stringRegister, context.stringRegisters[context.stringRegister].size(), context.stringRegisters[context.stringRegister]) << std::endl;
    #endif

    if (context.stringRegisters[context.stringRegister] != "") {
        switch (context.mode.back()) {
            case ReaderMode::READ:
                ParseRawType(a_gameState, context);
                break;
            case ReaderMode::OBJECT:
                ParseObjectMember(a_gameState, context);
                break;
        }
    }

    float toReturn;
    switch (context.lastRegister) {
        case RegisterType::INT:
            toReturn = context.intRegisters[context.intRegister];
            break;
        case RegisterType::FLOAT:
            toReturn = (int32_t)context.floatRegisters[context.floatRegister];
            break;
        case RegisterType::BOOL:
        case RegisterType::STRING:
        case RegisterType::ERROR:
            toReturn = 0;
            break;
    }

    #ifdef SCRIPT_PARSER_DEBUG_LOGGING
    DebugLogging::ParsingOut << "Returning: " << toReturn << std::endl;
    #endif

    return toReturn;
}

int32_t Interpreter::ParseBoolExpression(GameState& a_gameState, InterpreterContext& a_context, std::string a_expression) {
    #ifdef SCRIPT_PARSER_DEBUG_LOGGING
    DebugLogging::ParsingOut << "Parsing Bool Expression: \n" << a_expression << std::endl;
    #endif
    InterpreterContext context = InterpreterContext();

    size_t innerLevel = 0;

    bool inString = false;
    bool escaped = false;
    bool justOperated = false;
    #ifdef SCRIPT_PARSER_DEBUG_LOGGING
    size_t step = 0;
    #endif
    for (char character : a_expression) {
        #ifdef SCRIPT_PARSER_DEBUG_LOGGING
        DebugLogging::ParsingOut << std::format("\x1b[1mBOO_EX\x1b[22m Step {:0>4}) [{}] <{}> {}InString\x1b[39m {}Escaped\x1b[39m Register {:0>2}: [{:0>3}] {}", step++, ReaderModeDisplayName(context.mode.back()), OperatorEvaluationDisplayName(context.operatorEvaluation), (inString ? "\x1b[32m" : "\x1b[31m"), (escaped ? "\x1b[32m" : "\x1b[31m"), context.stringRegister, context.stringRegisters[context.stringRegister].size(), context.stringRegisters[context.stringRegister]) << std::endl;
        #endif
        switch (context.mode.back()) {
            case ReaderMode::READ: {
                switch (character) {
                    case ' ':
                    case '\n':
                    case '\t':
                    case '\v':
                        if (context.stringRegisters[context.stringRegister] != "") {
                            ParseRawType(a_gameState, context);
                        }
                        justOperated = false;
                        break;
                    case '@':
                        context.stringRegister += 1;
                        context.mode.push_back(ReaderMode::VAR_NAME);
                        break;
                    case '{':
                    case '}':
                        justOperated = false;
                        DebugLogging::ParsingOut << "[ERROR] Invalid characters." << std::endl;
                        break;
                    case '.':
                        justOperated = false;
                        ParseObjectName(a_gameState, context);
                        break;
                    case '+':
                        context.operatorEvaluation = OperatorEvaluation::ADDI;
                        justOperated = true;
                        break;
                    case '-':
                        context.operatorEvaluation = OperatorEvaluation::SUBT;
                        justOperated = true;
                        break;
                    case '*':
                        context.operatorEvaluation = OperatorEvaluation::MULT;
                        justOperated = true;
                        break;
                    case '/':
                        context.operatorEvaluation = OperatorEvaluation::DIVI;
                        justOperated = true;
                        break;
                    case '!':
                        context.operatorEvaluation = OperatorEvaluation::NOT;
                        justOperated = true;
                        break;
                    case '%':
                        context.operatorEvaluation = OperatorEvaluation::MODU;
                        justOperated = true;
                        break;
                    case '&':
                        context.operatorEvaluation = OperatorEvaluation::BAND;
                        justOperated = true;
                        break;
                    case '|':
                        context.operatorEvaluation = OperatorEvaluation::BOR;
                        justOperated = true;
                        break;
                    case '^':
                        context.operatorEvaluation = OperatorEvaluation::BXOR;
                        justOperated = true;
                        break;
                    case '~':
                        context.operatorEvaluation = OperatorEvaluation::BINV;
                        justOperated = true;
                        break;
                    case '<':
                        switch (context.operatorEvaluation) {
                            case OperatorEvaluation::NONE:
                                context.operatorEvaluation = OperatorEvaluation::LT;
                                break;
                            case OperatorEvaluation::LT:
                                context.operatorEvaluation = OperatorEvaluation::LSHIFT;
                                break;
                        }
                        justOperated = true;
                        break;
                    case '>':
                        switch (context.operatorEvaluation) {
                            case OperatorEvaluation::NONE:
                                context.operatorEvaluation = OperatorEvaluation::GT;
                                break;
                            case OperatorEvaluation::GT:
                                context.operatorEvaluation = OperatorEvaluation::RSHIFT;
                                break;
                        }
                        justOperated = true;
                        break;
                    case '=':
                        switch (context.operatorEvaluation) {
                            case OperatorEvaluation::NONE:
                                context.operatorEvaluation = OperatorEvaluation::EQ;
                                break;
                            case OperatorEvaluation::NOT:
                                context.operatorEvaluation = OperatorEvaluation::NEQ;
                                break;
                            case OperatorEvaluation::LT:
                                context.operatorEvaluation = OperatorEvaluation::LTE;
                                break;
                            case OperatorEvaluation::GT:
                                context.operatorEvaluation = OperatorEvaluation::GTE;
                                break;
                        }
                        justOperated = true;
                        break;
                    default:
                        justOperated = false;
                        context.stringRegisters[context.stringRegister] += character;
                        break;
                }
                break;
            }
            case ReaderMode::VAR_NAME: {
                switch (character) {
                    case '@': {
                        ParseVariableName(a_gameState, context);
                        break;
                    }
                    default:
                        context.stringRegisters[context.stringRegister] += character;
                        break;
                }
                break;
            }
            case ReaderMode::OBJECT: {
                switch (character) {
                    case ' ':
                    case '\n':
                    case '\t':
                    case '\v':
                    case ')': {
                        ParseObjectMember(a_gameState, context);
                        break;
                    }
                    case '(': {
                        ParseObjectMethod(a_gameState, context);
                        break;
                    }
                    default:
                        context.stringRegisters[context.stringRegister] += character;
                        break;
                }
                break;
            }
            case ReaderMode::ARGS: {
                switch (character) {
                    case ' ':
                        context.stringRegisters[context.stringRegister] += character;
                        escaped = false;
                    case '\n':
                    case '\t':
                    case '\v':
                        break;
                    case '"':
                        context.stringRegisters[context.stringRegister] += character;
                        if (inString) {
                            if (!escaped) {
                                inString = false;
                            }
                            escaped = false;
                        } else {
                            inString = true;
                        }
                        break;
                    case '\\':
                        if (inString) {
                            if (escaped) {
                                context.stringRegisters[context.stringRegister] += character;
                                escaped = false;
                            } else {
                                escaped = true;
                            }
                        }
                        break;
                    case 'n':
                        if (inString && escaped) {
                            context.stringRegisters[context.stringRegister] += "\\";
                        }
                        context.stringRegisters[context.stringRegister] += character;
                        break;
                    case '(': {
                        if (!inString) {
                            ++innerLevel;
                        }
                        context.stringRegisters[context.stringRegister] += character;
                        escaped = false;
                        break;
                    }
                    case ')': {
                        if (!inString) {
                            if (innerLevel == 0) {
                                ParseObjectMethodArgs(a_gameState, context);
                            } else {
                                --innerLevel;
                                context.stringRegisters[context.stringRegister] += character;
                                escaped = false;
                            }
                        } else {
                            context.stringRegisters[context.stringRegister] += character;
                            escaped = false;
                        }
                        break;
                    }
                    default:
                        context.stringRegisters[context.stringRegister] += character;
                        break;
                }
                break;
            }
        }
    }

    if (context.stringRegisters[context.stringRegister] != "") {
        #ifdef SCRIPT_PARSER_DEBUG_LOGGING
        DebugLogging::ParsingOut << std::format("\x1b[1mBOO_EX\x1b[22m Step {:0>4}) [{}] <{}> {}InString\x1b[39m {}Escaped\x1b[39m Register {:0>2}: [{:0>3}] {}", step++, ReaderModeDisplayName(context.mode.back()), OperatorEvaluationDisplayName(context.operatorEvaluation), (inString ? "\x1b[32m" : "\x1b[31m"), (escaped ? "\x1b[32m" : "\x1b[31m"), context.stringRegister, context.stringRegisters[context.stringRegister].size(), context.stringRegisters[context.stringRegister]) << std::endl;
        #endif
        switch (context.mode.back()) {
            case ReaderMode::READ:
                ParseRawType(a_gameState, context);
                break;
            case ReaderMode::OBJECT:
                ParseObjectMember(a_gameState, context);
                break;
        }
    }

    
    #ifdef SCRIPT_PARSER_DEBUG_LOGGING
    DebugLogging::ParsingOut << "Returning: " << (context.boolRegisters[context.boolRegister] ? "true" : "false") << std::endl;
    #endif

    return context.boolRegisters[context.boolRegister];
}

std::string Interpreter::FormatString(GameState& a_gameState, InterpreterContext& a_context, std::string a_string) {
    #ifdef SCRIPT_PARSER_DEBUG_LOGGING
    DebugLogging::ParsingOut << "Formatting String: \n" << a_string << std::endl;
    #endif
    InterpreterContext context = InterpreterContext();
    std::string result = "";
    int32_t innerLevel = 0;
    RegisterType registerType = RegisterType::INT;
    bool inString = true;
    bool escaped = false;
    #ifdef SCRIPT_PARSER_DEBUG_LOGGING
    size_t step = 0;
    #endif
    for (char character : a_string) {
        #ifdef SCRIPT_PARSER_DEBUG_LOGGING
        if (inString) {
            DebugLogging::ParsingOut << std::format("\x1b[1mSTR_FM\x1b[22m Step {:0>4}) [{}] <{}> {}InString\x1b[39m {}Escaped\x1b[39m [{:0>3}] {}", step++, ReaderModeDisplayName(context.mode.back()), OperatorEvaluationDisplayName(context.operatorEvaluation), (inString ? "\x1b[32m" : "\x1b[31m"), (escaped ? "\x1b[32m" : "\x1b[31m"), result.size(), result) << std::endl;
        } else {
            DebugLogging::ParsingOut << std::format("\x1b[1mSTR_FM\x1b[22m Step {:0>4}) [{}] <{}> {}InString\x1b[39m {}Escaped\x1b[39m Register {:0>2}: [{:0>3}] {}", step++, ReaderModeDisplayName(context.mode.back()), OperatorEvaluationDisplayName(context.operatorEvaluation), (inString ? "\x1b[32m" : "\x1b[31m"), (escaped ? "\x1b[32m" : "\x1b[31m"), context.stringRegister, context.stringRegisters[context.stringRegister].size(), context.stringRegisters[context.stringRegister]) << std::endl;
        }
        #endif

        switch (context.mode.back()) {
            case ReaderMode::READ: {
                switch (character) {
                    case ' ':
                    case '\n':
                    case '\t':
                    case '\v':
                        if (inString) {
                            result += character;
                        }
                        break;
                    case '\\':
                        if (inString) {
                            if (escaped) {
                                result += character;
                                escaped = false;
                            } else {
                                escaped = true;
                            }
                        }
                        break;
                    case 'n':
                        if (inString) {
                            if (escaped) {
                                result += '\n';
                            } else {
                                result += character;
                            }
                            escaped = false;
                        } else {
                            context.stringRegisters[context.stringRegister] += character;
                        }
                        break;
                    case '@':
                        if (inString) {
                            result += character;
                        } else {
                            context.stringRegister += 1;
                            context.mode.push_back(ReaderMode::VAR_NAME);
                        }
                        break;
                    case '{':
                        if (inString) {
                            if (escaped) {
                                result += character;
                                escaped = false;
                            } else {
                                inString = false;
                            }
                        } else {
                            DebugLogging::ParsingOut << "[ERROR] Invalid characters." << std::endl;
                        }
                        break;
                    case '}':
                        if (inString) {
                            if (escaped) {
                                result += character;
                                escaped = false;
                            } else {
                                DebugLogging::ParsingOut << "[ERROR] Invalid characters." << std::endl;
                            }
                        } else {
                            inString = true;
                            // Match Register Type?
                            switch (registerType) {
                                case RegisterType::STRING:
                                    result += context.stringRegisters[context.stringRegister];
                                    break;
                                case RegisterType::INT:
                                    result += std::to_string(context.intRegisters[context.intRegister]);
                                    break;
                                case RegisterType::FLOAT:
                                    result += std::to_string(context.floatRegisters[context.floatRegister]);
                                    break;
                                case RegisterType::BOOL:
                                    result += context.boolRegisters[context.boolRegister] ? "true" : "false";
                                    break;
                            }
                        }
                        break;
                    case '.':
                        if (inString) {
                            result += character;
                        } else {
                            ParseObjectName(a_gameState, context);
                        }
                        break;
                    case '+':
                        if (inString) {
                            result += character;
                        } else {
                            context.operatorEvaluation = OperatorEvaluation::ADDI;
                        }
                        break;
                    case '-':
                        if (inString) {
                            result += character;
                        } else {
                            context.operatorEvaluation = OperatorEvaluation::SUBT;
                        }
                        break;
                    case '*':
                        if (inString) {
                            result += character;
                        } else {
                            context.operatorEvaluation = OperatorEvaluation::MULT;
                        }
                        break;
                    case '/':
                        if (inString) {
                            result += character;
                        } else {
                            context.operatorEvaluation = OperatorEvaluation::DIVI;
                        }
                        break;
                    case '!':
                        if (inString) {
                            result += character;
                        } else {
                            context.operatorEvaluation = OperatorEvaluation::NOT;
                        }
                        break;
                    case '%':
                        if (inString) {
                            result += character;
                        } else {
                            context.operatorEvaluation = OperatorEvaluation::MODU;
                        }
                        break;
                    case '&':
                        if (inString) {
                            result += character;
                        } else {
                            context.operatorEvaluation = OperatorEvaluation::BAND;
                        }
                        break;
                    case '|':
                        if (inString) {
                            result += character;
                        } else {
                            context.operatorEvaluation = OperatorEvaluation::BOR;
                        }
                        break;
                    case '^':
                        if (inString) {
                            result += character;
                        } else {
                            context.operatorEvaluation = OperatorEvaluation::BXOR;
                        }
                        break;
                    case '~':
                        if (inString) {
                            result += character;
                        } else {
                            context.operatorEvaluation = OperatorEvaluation::BINV;
                        }
                        break;
                    case '<':
                        if (inString) {
                            result += character;
                        } else {
                            switch (context.operatorEvaluation) {
                                case OperatorEvaluation::NONE:
                                    context.operatorEvaluation = OperatorEvaluation::LT;
                                    break;
                                case OperatorEvaluation::LT:
                                    context.operatorEvaluation = OperatorEvaluation::LSHIFT;
                                    break;
                            }
                        }
                        break;
                    case '>':
                        if (inString) {
                            result += character;
                        } else {
                            switch (context.operatorEvaluation) {
                                case OperatorEvaluation::NONE:
                                    context.operatorEvaluation = OperatorEvaluation::GT;
                                    break;
                                case OperatorEvaluation::GT:
                                    context.operatorEvaluation = OperatorEvaluation::RSHIFT;
                                    break;
                            }
                        }
                        break;
                    case '=':
                        if (inString) {
                            result += character;
                        } else {
                            switch (context.operatorEvaluation) {
                                case OperatorEvaluation::NONE:
                                    context.operatorEvaluation = OperatorEvaluation::EQ;
                                    break;
                                case OperatorEvaluation::NOT:
                                    context.operatorEvaluation = OperatorEvaluation::NEQ;
                                    break;
                                case OperatorEvaluation::LT:
                                    context.operatorEvaluation = OperatorEvaluation::LTE;
                                    break;
                                case OperatorEvaluation::GT:
                                    context.operatorEvaluation = OperatorEvaluation::GTE;
                                    break;
                            }
                        }
                        break;
                    default:
                        if (inString) {
                            result += character;
                        } else {
                            context.stringRegisters[context.stringRegister] += character;
                        }
                        break;
                }
                break;
            }
            case ReaderMode::VAR_NAME: {
                switch (character) {
                    case '@': {
                        ParseVariableName(a_gameState, context);
                        break;
                    }
                    default:
                        context.stringRegisters[context.stringRegister] += character;
                        break;
                }
                break;
            }
            case ReaderMode::OBJECT: {
                switch (character) {
                    case ' ':
                    case '\n':
                    case '\t':
                    case '\v':
                    case ')': {
                        ParseObjectMember(a_gameState, context);
                        break;
                    }
                    case '(': {
                        ParseObjectMethod(a_gameState, context);
                        break;
                    }
                    default:
                        context.stringRegisters[context.stringRegister] += character;
                        break;
                }
                break;
            }
            case ReaderMode::ARGS: {
                switch (character) {
                    case ' ':
                        context.stringRegisters[context.stringRegister] += character;
                        escaped = false;
                    case '\n':
                    case '\t':
                    case '\v':
                        break;
                    case '(': {
                        if (!inString) {
                            ++innerLevel;
                        }
                        context.stringRegisters[context.stringRegister] += character;
                        escaped = false;
                        break;
                    }
                    case ')': {
                        if (!inString) {
                            if (innerLevel == 0) {
                                ParseObjectMethodArgs(a_gameState, context);
                            } else {
                                --innerLevel;
                                context.stringRegisters[context.stringRegister] += character;
                                escaped = false;
                            }
                        } else {
                            context.stringRegisters[context.stringRegister] += character;
                            escaped = false;
                        }
                        break;
                    }
                    default:
                        context.stringRegisters[context.stringRegister] += character;
                        break;
                }
                break;
            }
        }
    }

    return result;
}

void Interpreter::ParseRawType(GameState& a_gameState, InterpreterContext& a_context) {
    if (IsStringInt(a_context.stringRegisters[a_context.stringRegister])) {
        a_context.SetIntRegister(std::stoi(a_context.stringRegisters[a_context.stringRegister]));
        a_context.stringRegisters[a_context.stringRegister] = "";
    } else if (IsStringFloat(a_context.stringRegisters[a_context.stringRegister])) {
        a_context.SetFloatRegister(std::stof(a_context.stringRegisters[a_context.stringRegister]));
        a_context.stringRegisters[a_context.stringRegister] = "";
    } else if (IsStringBool(a_context.stringRegisters[a_context.stringRegister])) {
        a_context.SetBoolRegister(a_context.stringRegisters[a_context.stringRegister] == "true");
        a_context.stringRegisters[a_context.stringRegister] = "";
    }
}

void Interpreter::ParseVariableName(GameState& a_gameState, InterpreterContext& a_context) {
    std::string varType = a_context.stringRegisters[a_context.stringRegister - 1];
    #ifdef SCRIPT_PARSER_DEBUG_LOGGING
    DebugLogging::ParsingOut << std::format("Finished with the variable: {} {}", varType, a_context.stringRegisters[a_context.stringRegister]) << std::endl;
    DebugLogging::ParsingOut << std::format("\x1b[1mVariables\x1b[22m ({})", variables.data.size()) << std::endl;
    //for (const std::pair<std::string, VariablePtr>& varPair : variables) {
    //    DebugLogging::ParsingOut << std::format("Variable: [{}] {:0>3}", varPair.first, varPair.second.size) << std::endl;
    //}
    DebugLogging::ParsingOut << "Prae Var Val" << std::endl;
    #endif
    if (varType == "int32") {
        //DebugLogging::ParsingOut << std::format("The value of {} is {}", a_context.stringRegisters[a_context.stringRegister], variables.Get<int32_t>(a_context.stringRegisters[a_context.stringRegister])) << std::endl;
        a_context.SetIntRegister(variables.Get<int32_t>(a_context.stringRegisters[a_context.stringRegister]));
    } else if (varType == "uint32") {
        //DebugLogging::ParsingOut << std::format("The value of {} is {}", a_context.stringRegisters[a_context.stringRegister], variables.Get<int32_t>(a_context.stringRegisters[a_context.stringRegister])) << std::endl;
        a_context.SetIntRegister(variables.Get<int32_t>(a_context.stringRegisters[a_context.stringRegister]));
    } else if (varType == "float") {
        //DebugLogging::ParsingOut << std::format("The value of {} is {}", a_context.stringRegisters[a_context.stringRegister], variables.Get<float>(a_context.stringRegisters[a_context.stringRegister])) << std::endl;
        a_context.SetFloatRegister(variables.Get<float>(a_context.stringRegisters[a_context.stringRegister]));
    } else if (varType == "bool") {
        //DebugLogging::ParsingOut << std::format("The value of {} is {}", a_context.stringRegisters[a_context.stringRegister], variables.Get<bool>(a_context.stringRegisters[a_context.stringRegister])) << std::endl;
        a_context.SetBoolRegister(variables.Get<bool>(a_context.stringRegisters[a_context.stringRegister]));
    }
    #ifdef SCRIPT_PARSER_DEBUG_LOGGING
    DebugLogging::ParsingOut << "Post Var Val" << std::endl;
    #endif

    for (size_t i = 0; i < 2; ++i) {
        a_context.stringRegisters[a_context.stringRegister--] = "";
    }
    a_context.stringRegister += 1;
    a_context.mode.pop_back();
}

void Interpreter::ParseObjectName(GameState& a_gameState, InterpreterContext& a_context) {
    if (a_context.stringRegisters[a_context.stringRegister] == "player") {
        a_context.stringRegister += 1;
        a_context.mode.push_back(ReaderMode::OBJECT);
    } else if (a_context.stringRegisters[a_context.stringRegister] == "mathf") {
        a_context.stringRegister += 1;
        a_context.mode.push_back(ReaderMode::OBJECT);
    } else if (a_context.stringRegisters[a_context.stringRegister] == "mathi") {
        a_context.stringRegister += 1;
        a_context.mode.push_back(ReaderMode::OBJECT);
    } else if (a_context.stringRegisters[a_context.stringRegister] == "random") {
        a_context.stringRegister += 1;
        a_context.mode.push_back(ReaderMode::OBJECT);
    } else if (a_context.stringRegisters[a_context.stringRegister] == "console") {
        a_context.stringRegister += 1;
        a_context.mode.push_back(ReaderMode::OBJECT);
    } else if (a_context.stringRegisters[a_context.stringRegister] == "room") {
        a_context.stringRegister += 1;
        a_context.mode.push_back(ReaderMode::OBJECT);
    } else if (a_context.stringRegisters[a_context.stringRegister] == "gameState") {
        a_context.stringRegister += 1;
        a_context.mode.push_back(ReaderMode::OBJECT);
    }
}

void Interpreter::ParseObjectMember(GameState& a_gameState, InterpreterContext& a_context) {
    #ifdef SCRIPT_PARSER_DEBUG_LOGGING
    DebugLogging::ParsingOut << "Operating on member " << a_context.stringRegisters[a_context.stringRegister] << std::endl;
    #endif
    std::string object = a_context.stringRegisters[a_context.stringRegister - 1];
    if (object == "player") {
        if (a_context.stringRegisters[a_context.stringRegister] == "maxMana") {
            a_context.SetIntRegister(a_gameState.player.maxMana);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "curMana") {
            a_context.SetIntRegister(a_gameState.player.curMana);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "maxHP") {
            a_context.SetIntRegister(a_gameState.player.maxHP);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "curHP") {
            a_context.SetIntRegister(a_gameState.player.curHP);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "armor") {
            a_context.SetIntRegister(a_gameState.player.armor);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "gold") {
            a_context.SetIntRegister(a_gameState.player.gold);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "xp") {
            a_context.SetIntRegister(a_gameState.player.xp);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "manaSickness") {
            a_context.SetIntRegister(a_gameState.player.manaSickness);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "manaRegen") {
            a_context.SetIntRegister(a_gameState.player.manaRegen);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "turns") {
            a_context.SetIntRegister(a_gameState.player.turns);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "usedTurns") {
            a_context.SetIntRegister(a_gameState.player.usedTurns);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "effectiveArmor") {
            a_context.SetIntRegister(a_gameState.player.GetEffectiveArmor());
        }
    } else if (object == "room") {
        if (a_context.stringRegisters[a_context.stringRegister] == "livingInhabitants") {
            a_context.SetIntRegister((int32_t)a_gameState.rooms[a_gameState.curRoom].LivingInhabitants());
        }
    } else if (object == "gameState") {
        if (a_context.stringRegisters[a_context.stringRegister] == "curRoom") {
            a_context.SetIntRegister((int32_t)a_gameState.curRoom);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "usedRooms") {
            a_context.SetIntRegister((int32_t)a_gameState.usedRooms);
        }
    }
    a_context.stringRegisters[a_context.stringRegister] = "";
    a_context.mode.pop_back();
}

void Interpreter::ParseObjectMethod(GameState& a_gameState, InterpreterContext& a_context) {
    #ifdef SCRIPT_PARSER_DEBUG_LOGGING
    DebugLogging::ParsingOut << "Operating on function " << a_context.stringRegisters[a_context.stringRegister] << std::endl;
    #endif
    std::string object = a_context.stringRegisters[a_context.stringRegister - 1];
    a_context.mode.pop_back();
    if (object == "player") {
        if (a_context.stringRegisters[a_context.stringRegister] == "HasPerk") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "Hurt") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "Heal") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "DrainMana") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "RegainMana") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "GetSkillModifier") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "SetSkillModifier") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "AddSkillModifier") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        }
    } else if (object == "mathf") {
        if (a_context.stringRegisters[a_context.stringRegister] == "Min") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "Max") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "Clamp") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "Floor") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "Ceil") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "Round") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "Pow") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "Sqrt") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "Cbrt") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        }
    } else if (object == "mathi") {
        if (a_context.stringRegisters[a_context.stringRegister] == "Min") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "Max") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "Clamp") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "Floor") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "Ceil") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "Round") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "Pow") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "Sqrt") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "Cbrt") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        }
    } else if (object == "random") {
        if (a_context.stringRegisters[a_context.stringRegister] == "RangeI") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "RangeF") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "RollDice") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "Random") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        }
    } else if (object == "console") {
        if (a_context.stringRegisters[a_context.stringRegister] == "Print") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "EatInput") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        }
    } else if (object == "room") {
        if (a_context.stringRegisters[a_context.stringRegister] == "AddInhabitant") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "AddRoomAction") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "AddShop") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "HasFlag") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "GetFlag") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        } else if (a_context.stringRegisters[a_context.stringRegister] == "SetFlag") {
            a_context.stringRegister += 1;
            a_context.mode.push_back(ReaderMode::ARGS);
        }
    }
}

void Interpreter::ParseObjectMethodArgs(GameState& a_gameState, InterpreterContext& a_context) {
    std::vector<std::string> args = std::vector<std::string>();
    size_t argc = ParseFunctionArguments(a_context.stringRegisters[a_context.stringRegister], args);
    for (size_t i = 0; i < argc; ++i) {
        #ifdef SCRIPT_PARSER_DEBUG_LOGGING
        DebugLogging::ParsingOut << std::format("Prae Sanitize: \"{}\"", args[i]) << std::endl;
        #endif
        SanitizeString(args[i]);
        #ifdef SCRIPT_PARSER_DEBUG_LOGGING
        DebugLogging::ParsingOut << std::format("Post Sanitize: \"{}\"", args[i]) << std::endl;
        #endif
        #ifdef LEGACY_COMMA_REPLACEMENT
        int32_t index = 0;
        while ((index = args[i].find("<comma>", 0)) < args[i].size()) {
            args[i] = args[i].substr(0, index) + "," + args[i].substr(index + 7, args[i].size() - index - 7);
        }
        #endif
    }
    std::string object = a_context.stringRegisters[a_context.stringRegister - 2];
    std::string method = a_context.stringRegisters[a_context.stringRegister - 1];
    if (object == "player") {
        if (method == "HasPerk") {
            if (argc == 1) {
                Perks perk = PerkFromName(args[0]);
                if (perk != Perks::SIZE) {
                    a_context.SetBoolRegister(a_gameState.player.perks.test((size_t)perk));
                } else {
                    DebugLogging::ParsingOut << "[ERROR]: " << args[0] << " is not a valid perk id." << std::endl;
                }
            } else {
                DebugLogging::ParsingOut << "[ERROR]: player.HasPerk must have 1 argument, this call has " << argc << "." << std::endl;
            }
        } else if (method == "Hurt") {
            if (argc == 1) {
                a_gameState.player.Hurt(ParseIntExpression(a_gameState, a_context, args[0]));
            } else {
                DebugLogging::ParsingOut << "[ERROR]: player.Hurt must have 1 argument, this call has " << argc << "." << std::endl;
            }
        } else if (method == "Heal") {
            if (argc == 1) {
                a_gameState.player.Heal(ParseIntExpression(a_gameState, a_context, args[0]));
            } else {
                DebugLogging::ParsingOut << "[ERROR]: player.Heal must have 1 argument, this call has " << argc << "." << std::endl;
            }
        } else if (method == "DrainMana") {
            if (argc == 1) {
                a_gameState.player.DrainMana(ParseIntExpression(a_gameState, a_context, args[0]));
            } else {
                DebugLogging::ParsingOut << "[ERROR]: player.DrainMana must have 1 argument, this call has " << argc << "." << std::endl;
            }
        } else if (method == "RegainMana") {
            if (argc == 1) {
                a_gameState.player.RegainMana(ParseIntExpression(a_gameState, a_context, args[0]));
            } else {
                DebugLogging::ParsingOut << "[ERROR]: player.RegainMana must have 1 argument, this call has " << argc << "." << std::endl;
            }
        } else if (method == "GetSkillModifier") {
            if (argc == 1) {
                a_context.SetIntRegister(a_gameState.player.GetSkillModifier(FormatString(a_gameState, a_context, args[0])));
            } else {
                DebugLogging::ParsingOut << "[ERROR]: player.GetSkillModifier must have 1 argument, this call has " << argc << "." << std::endl;
            }
        } else if (method == "SetSkillModifier") {
            if (argc == 2) {
                a_context.SetIntRegister(a_gameState.player.SetSkillModifier(FormatString(a_gameState, a_context, args[0]), ParseIntExpression(a_gameState, a_context, args[1])));
            } else {
                DebugLogging::ParsingOut << "[ERROR]: player.SetSkillModifier must have 2 arguments, this call has " << argc << "." << std::endl;
            }
        } else if (method == "AddSkillModifier") {
            if (argc == 2) {
                a_context.SetIntRegister(a_gameState.player.SetSkillModifier(FormatString(a_gameState, a_context, args[0]), ParseIntExpression(a_gameState, a_context, args[1])));
            } else {
                DebugLogging::ParsingOut << "[ERROR]: player.AddSkillModifier must have 2 arguments, this call has " << argc << "." << std::endl;
            }
        }

        for (size_t i = 0; i < 3; ++i) {
            a_context.stringRegisters[a_context.stringRegister--] = "";
        }

        a_context.stringRegister += 1;
    } else if (object == "mathf") {
        if (method == "Min") {
            if (argc == 2) {
                a_context.SetFloatRegister(std::min<float>(ParseFloatExpression(a_gameState, a_context, args[0]), ParseFloatExpression(a_gameState, a_context, args[1])));
            } else {
                DebugLogging::ParsingOut << "[ERROR]: mathf.Min must have 2 arguments, this call has " << argc << "." << std::endl;
            }
        } else if (method == "Max") {
            if (argc == 2) {
                a_context.SetFloatRegister(std::max<float>(ParseFloatExpression(a_gameState, a_context, args[0]), ParseFloatExpression(a_gameState, a_context, args[1])));
            } else {
                DebugLogging::ParsingOut << "[ERROR]: mathf.Max must have 2 arguments, this call has " << argc << "." << std::endl;
            }
        } else if (method == "Clamp") {
            if (argc == 3) {
                a_context.SetFloatRegister(std::clamp<float>(ParseFloatExpression(a_gameState, a_context, args[0]), ParseFloatExpression(a_gameState, a_context, args[1]), ParseFloatExpression(a_gameState, a_context, args[2])));
            } else {
                DebugLogging::ParsingOut << "[ERROR]: mathf.Clamp must have 3 arguments, this call has " << argc << "." << std::endl;
            }
        } else if (method == "Floor") {
            if (argc == 1) {
                a_context.SetFloatRegister(std::floorf(ParseFloatExpression(a_gameState, a_context, args[0])));
            } else {
                DebugLogging::ParsingOut << "[ERROR]: mathf.Floor must have 1 argument, this call has " << argc << "." << std::endl;
            }
        } else if (method == "Ceil") {
            if (argc == 1) {
                a_context.SetFloatRegister(std::ceilf(ParseFloatExpression(a_gameState, a_context, args[0])));
            } else {
                DebugLogging::ParsingOut << "[ERROR]: mathf.Floor must have 1 argument, this call has " << argc << "." << std::endl;
            }
        } else if (method == "Round") {
            if (argc == 1) {
                a_context.SetFloatRegister(std::roundf(ParseFloatExpression(a_gameState, a_context, args[0])));
            } else {
                DebugLogging::ParsingOut << "[ERROR]: mathf.Floor must have 1 argument, this call has " << argc << "." << std::endl;
            }
        } else if (method == "Pow") {
            if (argc == 2) {
                a_context.SetFloatRegister(std::powf(ParseFloatExpression(a_gameState, a_context, args[0]), ParseFloatExpression(a_gameState, a_context, args[1])));
            } else {
                DebugLogging::ParsingOut << "[ERROR]: mathf.Pow must have 2 arguments, this call has " << argc << "." << std::endl;
            }
        } else if (method == "Sqrt") {
            if (argc == 1) {
                a_context.SetFloatRegister(std::sqrtf(ParseFloatExpression(a_gameState, a_context, args[0])));
            } else {
                DebugLogging::ParsingOut << "[ERROR]: mathf.Sqrt must have 1 argument, this call has " << argc << "." << std::endl;
            }
        } else if (method == "Cbrt") {
            if (argc == 1) {
                a_context.SetFloatRegister(std::cbrtf(ParseFloatExpression(a_gameState, a_context, args[0])));
            } else {
                DebugLogging::ParsingOut << "[ERROR]: mathf.Cbrt must have 1 argument, this call has " << argc << "." << std::endl;
            }
        }

        for (size_t i = 0; i < 3; ++i) {
            a_context.stringRegisters[a_context.stringRegister--] = "";
        }

        a_context.stringRegister += 1;
    } else if (object == "mathi") {
        if (method == "Min") {
            if (argc == 2) {
                a_context.SetIntRegister(std::min<int32_t>(ParseIntExpression(a_gameState, a_context, args[0]), ParseIntExpression(a_gameState, a_context, args[1])));
            } else {
                DebugLogging::ParsingOut << "[ERROR]: mathi.Min must have 2 arguments, this call has " << argc << "." << std::endl;
            }
        } else if (method == "Max") {
            if (argc == 2) {
                a_context.SetIntRegister(std::max<int32_t>(ParseIntExpression(a_gameState, a_context, args[0]), ParseIntExpression(a_gameState, a_context, args[1])));
            } else {
                DebugLogging::ParsingOut << "[ERROR]: mathi.Max must have 2 arguments, this call has " << argc << "." << std::endl;
            }
        } else if (method == "Clamp") {
            if (argc == 3) {
                a_context.SetIntRegister(std::clamp<int32_t>(ParseIntExpression(a_gameState, a_context, args[0]), ParseIntExpression(a_gameState, a_context, args[1]), ParseIntExpression(a_gameState, a_context, args[2])));
            } else {
                DebugLogging::ParsingOut << "[ERROR]: mathi.Clamp must have 3 arguments, this call has " << argc << "." << std::endl;
            }
        } else if (method == "Floor") {
            if (argc == 1) {
                a_context.SetIntRegister((int32_t)std::floorf(ParseFloatExpression(a_gameState, a_context, args[0])));
            } else {
                DebugLogging::ParsingOut << "[ERROR]: mathi.Floor must have 1 argument, this call has " << argc << "." << std::endl;
            }
        } else if (method == "Ceil") {
            if (argc == 1) {
                a_context.SetIntRegister((int32_t)std::ceilf(ParseFloatExpression(a_gameState, a_context, args[0])));
            } else {
                DebugLogging::ParsingOut << "[ERROR]: mathi.Floor must have 1 argument, this call has " << argc << "." << std::endl;
            }
        } else if (method == "Round") {
            if (argc == 1) {
                a_context.SetIntRegister((int32_t)std::roundf(ParseFloatExpression(a_gameState, a_context, args[0])));
            } else {
                DebugLogging::ParsingOut << "[ERROR]: mathi.Floor must have 1 argument, this call has " << argc << "." << std::endl;
            }
        } else if (method == "Pow") {
            if (argc == 2) {
                a_context.SetIntRegister((int32_t)std::powf(ParseFloatExpression(a_gameState, a_context, args[0]), ParseFloatExpression(a_gameState, a_context, args[1])));
            } else {
                DebugLogging::ParsingOut << "[ERROR]: mathi.Pow must have 2 arguments, this call has " << argc << "." << std::endl;
            }
        } else if (method == "Sqrt") {
            if (argc == 1) {
                a_context.SetIntRegister((int32_t)std::sqrtf(ParseFloatExpression(a_gameState, a_context, args[0])));
            } else {
                DebugLogging::ParsingOut << "[ERROR]: mathi.Sqrt must have 1 argument, this call has " << argc << "." << std::endl;
            }
        } else if (method == "Cbrt") {
            if (argc == 1) {
                a_context.SetIntRegister((int32_t)std::cbrtf(ParseFloatExpression(a_gameState, a_context, args[0])));
            } else {
                DebugLogging::ParsingOut << "[ERROR]: mathi.Cbrt must have 1 argument, this call has " << argc << "." << std::endl;
            }
        }

        for (size_t i = 0; i < 3; ++i) {
            a_context.stringRegisters[a_context.stringRegister--] = "";
        }

        a_context.stringRegister += 1;
    } else if (object == "random") {
        if (method == "RangeI") {
            if (argc == 2) {
                int32_t minimum = ParseIntExpression(a_gameState, a_context, args[0]);
                a_context.SetIntRegister(minimum + (int32_t)(PERCENT_DISTRIBUTION(a_gameState.generator) * (ParseIntExpression(a_gameState, a_context, args[1]) - minimum)));
            } else {
                DebugLogging::ParsingOut << "[ERROR]: random.RangeI must have 2 arguments, this call has " << argc << "." << std::endl;
            }
        } else if (method == "RangeF") {
            if (argc == 2) {
                float minimum = ParseFloatExpression(a_gameState, a_context, args[0]);
                a_context.SetFloatRegister(minimum + PERCENT_DISTRIBUTION(a_gameState.generator) * (ParseFloatExpression(a_gameState, a_context, args[1]) - minimum));
            } else {
                DebugLogging::ParsingOut << "[ERROR]: random.RangeF must have 2 arguments, this call has " << argc << "." << std::endl;
            }
        } else if (method == "RollDice") {
            if (argc == 2) {
                int32_t count = ParseIntExpression(a_gameState, a_context, args[0]);
                int32_t dice = ParseIntExpression(a_gameState, a_context, args[1]);
                #ifdef SCRIPT_PARSER_DEBUG_LOGGING
                DebugLogging::ParsingOut << std::format("{} & {}", args[0], args[1]) << std::endl;
                DebugLogging::ParsingOut << std::format("{}d{}", count, dice) << std::endl;
                #endif
                a_context.SetIntRegister(a_gameState.RollDice(count, dice));
            } else {
                DebugLogging::ParsingOut << "[ERROR]: random.RollDice must have 2 arguments, this call has " << argc << "." << std::endl;
            }
        } else if (method == "Random") {
            if (argc == 0) {
                a_context.SetFloatRegister(PERCENT_DISTRIBUTION(a_gameState.generator));
            } else {
                DebugLogging::ParsingOut << "[ERROR]: random.Random must have 0 arguments, this call has " << argc << "." << std::endl;
            }
        }

        for (size_t i = 0; i < 3; ++i) {
            a_context.stringRegisters[a_context.stringRegister--] = "";
        }

        a_context.stringRegister += 1;
    } else if (object == "console") {
        if (method == "Print") {
            if (argc == 1) {
                DebugLogging::ParsingOut << FormatString(a_gameState, a_context, args[0]);
            } else {
                DebugLogging::ParsingOut << "[ERROR]: console.Print must have 1 argument, this call has " << argc << "." << std::endl;
            }
        } else if (method == "EatInput") {
            if (argc == 0) {
                EatInput();
            } else {
                DebugLogging::ParsingOut << "[ERROR]: console.Print must have 0 arguments, this call has " << argc << "." << std::endl;
            }
        }

        for (size_t i = 0; i < 3; ++i) {
            a_context.stringRegisters[a_context.stringRegister--] = "";
        }

        a_context.stringRegister += 1;
    } else if (object == "room") {
        if (method == "AddInhabitant") {
            if (argc == 3) {
                std::string entityTemplate = FormatString(a_gameState, a_context, args[0]);
                std::string npcTemplate = FormatString(a_gameState, a_context, args[1]);
                std::string modifierTemplates = FormatString(a_gameState, a_context, args[2]);
                std::vector<std::string> modifiers = std::vector<std::string>();
                size_t modifierCount = SplitString(modifierTemplates, ":", modifiers);
                #ifdef SCRIPT_PARSER_DEBUG_LOGGING
                DebugLogging::ParsingOut << std::format("{}({}) = {}", entityTemplate, npcTemplate, modifierTemplates) << std::endl;
                #endif
                
                NPCData npc = NPCData(GameData::ENTITY_TEMPLATES.at(entityTemplate), &GameData::NPC_TEMPLATES.at(npcTemplate));
                for (const std::string& modifier : modifiers) {
                    npc = GameData::NPC_MODIFIERS.at(modifier)(npc);
                }

                a_gameState.rooms[a_gameState.curRoom].inhabitants.push_back(npc);
            } else if (argc == 2) {
                std::string entityTemplate = FormatString(a_gameState, a_context, args[0]);
                std::string npcTemplate = FormatString(a_gameState, a_context, args[1]);
                #ifdef SCRIPT_PARSER_DEBUG_LOGGING
                DebugLogging::ParsingOut << std::format("{}({})", entityTemplate, npcTemplate) << std::endl;
                #endif
                a_gameState.rooms[a_gameState.curRoom].inhabitants.push_back(NPCData(GameData::ENTITY_TEMPLATES.at(entityTemplate), &GameData::NPC_TEMPLATES.at(npcTemplate)));
            } else {
                DebugLogging::ParsingOut << "[ERROR]: room.AddInhabitant must have 3 arguments, this call has " << argc << "." << std::endl;
            }
        } else if (method == "AddRoomAction") {
            if (argc == 1) {
                std::string roomAction = FormatString(a_gameState, a_context, args[0]);
                #ifdef SCRIPT_PARSER_DEBUG_LOGGING
                DebugLogging::ParsingOut << roomAction << std::endl;
                #endif
                a_gameState.rooms[a_gameState.curRoom].roomActions.push_back(GameData::ROOM_ACTIONS.at(roomAction));
            } else {
                DebugLogging::ParsingOut << "[ERROR]: room.AddRoomAction must have 1 argument, this call has " << argc << "." << std::endl;
            }
        } else if (method == "AddShop") {
            if (argc == 1) {
                std::string name = FormatString(a_gameState, a_context, args[0]);
                #ifdef SCRIPT_PARSER_DEBUG_LOGGING
                DebugLogging::ParsingOut << name << std::endl;
                #endif
                a_gameState.rooms[a_gameState.curRoom].data.emplace("Shop", new ShopInstance());
                GenerateShopInstance(a_gameState, GameData::SHOP_DATA.at(name), *(ShopInstance*)a_gameState.rooms[a_gameState.curRoom].data.at("Shop"));
                a_gameState.rooms[a_gameState.curRoom].roomActions.push_back(GameData::ROOM_ACTIONS.at("Shop"));
            } else {
                DebugLogging::ParsingOut << "[ERROR]: room.AddShop must have 1 argument, this call has " << argc << "." << std::endl;
            }
        } else if (method == "HasFlag") {
            if (argc == 1) {
                std::string flag = FormatString(a_gameState, a_context, args[0]);
                #ifdef SCRIPT_PARSER_DEBUG_LOGGING
                DebugLogging::ParsingOut << flag << std::endl;
                #endif
                a_context.SetBoolRegister(a_gameState.rooms[a_gameState.curRoom].flags.contains(flag));
            } else {
                DebugLogging::ParsingOut << "[ERROR]: room.HasFlag must have 1 argument, this call has " << argc << "." << std::endl;
            }
        } else if (method == "GetFlag") {
            if (argc == 1) {
                std::string flag = FormatString(a_gameState, a_context, args[0]);
                #ifdef SCRIPT_PARSER_DEBUG_LOGGING
                DebugLogging::ParsingOut << flag << std::endl;
                #endif
                a_context.SetIntRegister(a_gameState.rooms[a_gameState.curRoom].flags.at(flag));
            } else {
                DebugLogging::ParsingOut << "[ERROR]: room.GetFlag must have 1 argument, this call has " << argc << "." << std::endl;
            }
        } else if (method == "SetFlag") {
            if (argc == 2) {
                std::string flag = FormatString(a_gameState, a_context, args[0]);
                int32_t value = ParseIntExpression(a_gameState, a_context, args[1]);
                #ifdef SCRIPT_PARSER_DEBUG_LOGGING
                DebugLogging::ParsingOut << std::format("Setting flag {} to {}.", flag, value) << std::endl;
                #endif
                a_gameState.rooms[a_gameState.curRoom].flags.insert_or_assign(flag, value);
            } else {
                DebugLogging::ParsingOut << "[ERROR]: room.SetFlag must have 2 arguments, this call has " << argc << "." << std::endl;
            }
        }

        for (size_t i = 0; i < 3; ++i) {
            a_context.stringRegisters[a_context.stringRegister--] = "";
        }

        a_context.stringRegister += 1;
    }

    a_context.mode.pop_back();
}