#pragma once
#ifndef ARS_TIMORIS__SOCKSCRIPT__INTERPRETER_TPP
#define ARS_TIMORIS__SOCKSCRIPT__INTERPRETER_TPP
#include <ArsTimoris/Sockscript/Interpreter.h>
#include <iostream>
#include <format>
#include <ArsTimoris/Util/DebugLogging.hpp>

template <typename T>
T Interpreter::ParseStatement(GameState& a_gameState, RegisterType a_returnType, std::string a_statement) {
    InterpreterContext context = InterpreterContext();
    std::vector<std::pair<int32_t, bool>> ifTrue = {};
    int32_t ifLevel = 0;
    bool inString = false;
    bool escaped = false;
    int32_t embedLevel = 0;
    int32_t innerLevel = 0;
    #ifdef SCRIPT_PARSER_DEBUG_LOGGING
    size_t step = 0;
    #endif
    for (char character : a_statement) {
        #ifdef SCRIPT_PARSER_DEBUG_LOGGING
        DebugLogging::ParsingOut << std::format("\x1b[1mPARSER\x1b[22m Step {:0>4}) [{}] <{}> {}InString\x1b[39m {}Escaped\x1b[39m Register {:0>2}: [{:0>3}] {}", step++, ReaderModeDisplayName(context.mode.back()), OperatorEvaluationDisplayName(context.operatorEvaluation), (inString ? "\x1b[32m" : "\x1b[31m"), (escaped ? "\x1b[32m" : "\x1b[31m"), context.stringRegister, context.stringRegisters[context.stringRegister].size(), context.stringRegisters[context.stringRegister]) << std::endl;
        #endif
        switch (context.mode.back()) {
            case ReaderMode::READ: {
                switch (character) {
                    case ' ':
                    case '\n':
                    case '\t':
                    case '\v': {
                        if (context.stringRegisters[context.stringRegister] == "return") {
                            context.stringRegisters[context.stringRegister] = "";
                            context.mode.push_back(ReaderMode::RET_EXPR);
                        } else if (context.stringRegisters[context.stringRegister] == "delete") {
                            context.stringRegisters[context.stringRegister] = "";
                            context.mode.push_back(ReaderMode::DEL_EXPR);
                        }
                        break;
                    }
                    case '<': {
                        context.stringRegister += 1;
                        context.mode.push_back(ReaderMode::VAR_TYPE);
                        break;
                    }
                    case ';': {
                        if (context.stringRegisters[context.stringRegister] == "end") {
                            if (ifTrue.size() > 0 && ifTrue.back().first == context.currentLevel) {
                                ifTrue.pop_back();
                            } else {
                                context.mode.push_back(ReaderMode::FINISHED);
                            }
                        }
                        context.stringRegisters[context.stringRegister] = "";
                        break;
                    }
                    case '{': {
                        #ifdef SCRIPT_PARSER_DEBUG_LOGGING
                        DebugLogging::ParsingOut << "Bracket hit with: " << context.stringRegisters[context.stringRegister] << std::endl;
                        #endif
                        if (context.stringRegisters[context.stringRegister] == "else") {
                            #ifdef SCRIPT_PARSER_DEBUG_LOGGING
                            DebugLogging::ParsingOut << "Truism: <" << ifTrue.back().first << ", " << (ifTrue.back().second ? "true" : "false") << ">" << std::endl;
                            #endif
                            if (ifTrue.back().second) {
                                context.mode.push_back(ReaderMode::FALLTHROUGH);
                            } else {
                                ++context.currentLevel;
                            }
                        } else {
                            ++context.currentLevel;
                        }
                        context.stringRegisters[context.stringRegister] = "";
                        break;
                    }
                    case '}': {
                        --context.currentLevel;
                        while (ifTrue.back().first > context.currentLevel) {
                            ifTrue.pop_back();
                        }
                        break;
                    }
                    case '.': {
                        ParseObjectName(a_gameState, context);
                        break;
                    }
                    case '(': {
                        if (context.stringRegisters[context.stringRegister] == "if") {
                            context.stringRegisters[context.stringRegister] = "";
                            context.targetLevel = context.currentLevel;
                            context.mode.push_back(ReaderMode::EXPRESSION);
                        } else if (context.stringRegisters[context.stringRegister] == "elif") {
                            context.stringRegisters[context.stringRegister] = "";
                            if (ifTrue.back().second) {
                                context.mode.push_back(ReaderMode::FALLTHROUGH);
                            } else {
                                ifTrue.pop_back();
                                context.mode.push_back(ReaderMode::EXPRESSION);
                            }
                        }
                        break;
                    }
                    default: {
                        context.stringRegisters[context.stringRegister] += character;
                        break;
                    }
                }
                break;
            }
            case ReaderMode::VAR_TYPE: {
                switch (character) {
                    case ' ':
                    case '\n':
                    case '\t':
                    case '\v': {
                        break;
                    }
                    case '>': {
                        context.mode.pop_back();
                        context.mode.push_back(ReaderMode::VAR_VALUE);
                        context.stringRegister += 1;
                        break;
                    }
                    default: {
                        context.stringRegisters[context.stringRegister] += character;
                        break;
                    }
                }
                break;
            }
            case ReaderMode::VAR_VALUE: {
                switch (character) {
                    case ';': {
                        DebugLogging::ParsingOut << "[ERROR] Semicolon before variable expression provided." << std::endl;
                        break;
                    }
                    case '=': {
                        context.mode.pop_back();
                        context.mode.push_back(ReaderMode::VAR_EXPR);
                        break;
                    }
                    default: {
                        break;
                    }
                }
                break;
            }
            case ReaderMode::VAR_EXPR: {
                switch (character) {
                    case ' ':
                        context.stringRegisters[context.stringRegister] += character;
                        escaped = false;
                    case '\n':
                    case '\t':
                    case '\v':
                        break;
                    case '"': {
                        if (inString) {
                            if (escaped) {
                                context.stringRegisters[context.stringRegister] += character;
                            } else {
                                inString = false;
                            }
                            escaped = false;
                        } else {
                            inString = true;
                        }
                        break;
                    }
                    case '\\': {
                        if (inString) {
                            if (escaped) {
                                context.stringRegisters[context.stringRegister] += character;
                                escaped = false;
                            } else {
                                escaped = true;
                            }
                        }
                        break;
                    }
                    case ';': {
                        if (inString) { 
                            context.stringRegisters[context.stringRegister] += character;
                            escaped = false;
                        } else {
                            context.mode.pop_back();
                            std::string varName = context.stringRegisters[context.stringRegister - 2];
                            std::string varType = context.stringRegisters[context.stringRegister - 1];
                            if (varType == "float") {
                                float value = ParseFloatExpression(a_gameState, context, context.stringRegisters[context.stringRegister]);
                                variables.Set(varName, DataComponents::DataType::FLOAT, value, sizeof(float));
                                //variables.data.emplace(varName, DataComponents::DataHolder(varName, DataComponents::DataType::FLOAT, (void*)(&value), sizeof(float)));
                            } else if (varType == "uint32") {
                                int32_t value = ParseIntExpression(a_gameState, context, context.stringRegisters[context.stringRegister]);
                                //DebugLogging::ParsingOut << "Setting: " << varName << " : " << value << std::endl;
                                variables.Set(varName, DataComponents::DataType::INT32, value, sizeof(int32_t));
                                //DebugLogging::ParsingOut << "Set: " << varName << " : " << value << std::endl;
                                //DebugLogging::ParsingOut << "Set: " << varName << " : " << variables.Get<int32_t>(varName) << std::endl;
                            } else if (varType == "bool") {
                                bool value = ParseBoolExpression(a_gameState, context, context.stringRegisters[context.stringRegister]);
                                variables.Set(varName, DataComponents::DataType::BOOL, value, sizeof(bool));
                            } else if (varType == "string") {
                                DebugLogging::ParsingOut << "String is not a supported data type." << std::endl;
                            }

                            //context.DumpStringRegister();
                            for (size_t i = 0; i < 3; ++i) {
                                context.stringRegisters[context.stringRegister--] = "";
                            }
                            context.stringRegister += 1;
                            //context.DumpStringRegister();
                        }
                        break;
                    }
                    default: {
                        context.stringRegisters[context.stringRegister] += character;
                        escaped = false;
                        break;
                    }
                }
                break;
            }
            case ReaderMode::RET_EXPR: {
                switch (character) {
                    case ' ':
                        context.stringRegisters[context.stringRegister] += character;
                        escaped = false;
                    case '\n':
                    case '\t':
                    case '\v':
                        break;
                    case '"': {
                        if (inString) {
                            if (escaped) {
                                context.stringRegisters[context.stringRegister] += character;
                            } else {
                                inString = false;
                            }
                            escaped = false;
                        } else {
                            inString = true;
                        }
                        break;
                    }
                    case '\\': {
                        if (inString) {
                            if (escaped) {
                                context.stringRegisters[context.stringRegister] += character;
                                escaped = false;
                            } else {
                                escaped = true;
                            }
                        }
                        break;
                    }
                    case ';': {
                        if (inString) { 
                            context.stringRegisters[context.stringRegister] += character;
                            escaped = false;
                        } else {
                            context.mode.pop_back();
                            switch (a_returnType) {
                                case RegisterType::INT: {
                                    int32_t value = ParseIntExpression(a_gameState, context, context.stringRegisters[context.stringRegister]);
                                    return *(T*)(&value);
                                }
                                case RegisterType::FLOAT: {
                                    float value = ParseFloatExpression(a_gameState, context, context.stringRegisters[context.stringRegister]);
                                    return *(T*)(&value);
                                }
                                case RegisterType::BOOL: {
                                    bool value = ParseBoolExpression(a_gameState, context, context.stringRegisters[context.stringRegister]);
                                    return *(T*)(&value);
                                }
                                case RegisterType::STRING: {
                                    std::string value = FormatString(a_gameState, context, context.stringRegisters[context.stringRegister]);
                                    return *(T*)(&value);
                                }
                                case RegisterType::ERROR: {
                                    return (T)(0);
                                }
                            }
                        }
                        break;
                    }
                    default: {
                        context.stringRegisters[context.stringRegister] += character;
                        escaped = false;
                        break;
                    }
                }
                break;
            }
            case ReaderMode::DEL_EXPR: {
                switch (character) {
                    case ' ':
                        context.stringRegisters[context.stringRegister] += character;
                        escaped = false;
                    case '\n':
                    case '\t':
                    case '\v':
                        break;
                    case '"': {
                        if (inString) {
                            if (escaped) {
                                context.stringRegisters[context.stringRegister] += character;
                            } else {
                                inString = false;
                            }
                            escaped = false;
                        } else {
                            inString = true;
                        }
                        break;
                    }
                    case '\\': {
                        if (inString) {
                            if (escaped) {
                                context.stringRegisters[context.stringRegister] += character;
                                escaped = false;
                            } else {
                                escaped = true;
                            }
                        }
                        break;
                    }
                    case ';': {
                        if (inString) { 
                            context.stringRegisters[context.stringRegister] += character;
                            escaped = false;
                        } else {
                            context.mode.pop_back();
                            std::string value = FormatString(a_gameState, context, context.stringRegisters[context.stringRegister]);
                            variables.Del(value);
                        }
                        break;
                    }
                    default: {
                        context.stringRegisters[context.stringRegister] += character;
                        escaped = false;
                        break;
                    }
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
                    default: {
                        context.stringRegisters[context.stringRegister] += character;
                        break;
                    }
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
                    case '"': {
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
                    }
                    case '\\': {
                        if (inString) {
                            if (escaped) {
                                context.stringRegisters[context.stringRegister] += character;
                                escaped = false;
                            } else {
                                escaped = true;
                            }
                        }
                        break;
                    }
                    case 'n': {
                        if (inString && escaped) {
                            context.stringRegisters[context.stringRegister] += "\\";
                        }
                        context.stringRegisters[context.stringRegister] += character;
                        escaped = false;
                        break;
                    }
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
                    default: {
                        context.stringRegisters[context.stringRegister] += character;
                        escaped = false;
                        break;
                    }
                }
                break;
            }
            case ReaderMode::EXPRESSION: {
                switch (character) {
                    case ' ':
                    case '\n':
                    case '\t':
                    case '\v': {
                        context.stringRegisters[context.stringRegister] += character;
                        break;
                    }
                    case '(': {
                        embedLevel += 1;
                        context.stringRegisters[context.stringRegister] += character;
                        break;
                    }
                    case ')': {
                        if (embedLevel-- <= 0) {
                            ifTrue.push_back(std::pair<int32_t, bool>(context.currentLevel, ParseBoolExpression(a_gameState, context, context.stringRegisters[context.stringRegister])));
                            context.mode.pop_back();
                            if (!ifTrue.back().second) {
                                context.mode.push_back(ReaderMode::FALLTHROUGH);
                            }
                            context.stringRegisters[context.stringRegister] = "";
                        } else {
                            context.stringRegisters[context.stringRegister] += character;
                        }
                        break;
                    }
                    default: {
                        context.stringRegisters[context.stringRegister] += character;
                        break;
                    }
                }
                break;
            }
            case ReaderMode::FALLTHROUGH: {
                switch (character) {
                    case '\\': {
                        escaped = !escaped;
                        break;
                    }
                    case '"': {
                        if (!escaped) {
                            inString = !inString;
                        }
                        escaped = false;
                        break;
                    }
                    case '{': {
                        if (!inString && !escaped) {
                            ++context.currentLevel;
                        }
                        escaped = false;
                        break;
                    }
                    case '}': {
                        if (!inString && !escaped) {
                            --context.currentLevel;
                            if (context.currentLevel == context.targetLevel) {
                                context.mode.pop_back();
                            }
                        }
                        escaped = false;
                        break;
                    }
                    default: {
                        escaped = false;
                        break;
                    }
                }
                break;
            }
        }
    }

    switch (a_returnType) {
        case RegisterType::INT:
        case RegisterType::FLOAT:
        case RegisterType::ERROR:
        case RegisterType::BOOL:
            return (T)0;
        case RegisterType::STRING:
            return (T)"";
    }
    
    return (T)0;
}
#endif