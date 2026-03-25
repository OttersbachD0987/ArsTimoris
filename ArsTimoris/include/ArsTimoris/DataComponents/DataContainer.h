#pragma once
#ifndef ARS_TIMORIS__DATA_COMPONENTS__DATA_CONTAINER_H
#define ARS_TIMORIS__DATA_COMPONENTS__DATA_CONTAINER_H
#include <string>
#include <filesystem>
#include <cstdint>
#include <any>
#include <unordered_map>
#include <fstream>

namespace DataComponents {
    /*
    # EBNF Form
    
    <value_char> ::= ([a-z] | [A-Z] | [0-9] | "_" | "-" | "\\n" | "\\\"" | "\\t" | "\\v" | "\\;" | "|" | "." | "," | "'" | ":" | "[" | "]" | "{" | "}" | "/" | "?" | "<" | ">" | "=" | "+" | "!" | "`" | "~" | "@" | "#" | "$" | "%" | "^" | "&" | "*" | "(" | ")")
    <value_string> ::= ("\"" (<value_char> | ";" | <deadspace>)+ "\"" | (<value_char> | <deadspace>)+)
    <value_uint> ::= [0-9]+
    <value_int> ::= "-"? <value_uint>

    <type_int> ::= "INT" ("8" | "16" | "32" | "64")
    <type_uint> ::= "U" <type_int>

    <delim> ::= ":"

    <identifier> ::= ([a-z] | [A-Z] | [0-9] | "_")+
    <deadspace> ::= (" " | "\n" | "\t" | "\v")
    <statement_name> ::= <deadspace>* <identifier> <deadspace>*
    <statement_data> ::= <deadspace>* <delim> <deadspace>*
    <statement> ::= (<statement_uint> | <statement_int> | <statement_bool> | <statement_string>)

    <statement_uint> ::= <statement_name> "<" <deadspace>* <type_uint> <deadspace>* ">" <statement_data> <value_uint> <deadspace>* ";"
    <statement_int> ::= <statement_name> "<" <deadspace>* <type_int> <deadspace>* ">" <statement_data> <value_int> <deadspace>* ";"
    <statement_bool> ::= <statement_name> "<" <deadspace>* "BOOL" <deadspace>* ">" <statement_data> ((("t" | "T") ("r" | "R") ("u" | "U") ("e" | "E")) | (("f" | "F") ("a" | "A") ("l" | "L") ("s" | "S") ("e" | "E"))) <deadspace>* ";"
    <statement_string> ::= <statement_name> "<" <deadspace>* "STRING" <deadspace>* ">" <statement_data> (<value_string> <deadspace>*)+ ";"
    */

    enum class DataType {
        UINT8,
        UINT16,
        UINT32,
        UINT64,
        INT8,
        INT16,
        INT32,
        INT64,
        BOOL,
        STRING,
        FLOAT,
        INVALID,
    };

    struct DataHolder {
    public:
        std::string key;
        DataType type;
        std::any data;
        size_t size;

        DataHolder(std::string_view a_key, const DataType& a_dataType, const std::any a_data, size_t a_size);

        void SetData(std::any a_data, size_t a_size);

        ~DataHolder();
    };

    struct DataContainer {
    public:
        std::unordered_map<std::string, DataHolder> data;

        DataContainer(void);

        template<typename T>
        T Get(const std::string& a_key);

        void Set(const std::string& a_key, DataType a_dataType, const std::any a_data, size_t a_size);

        void Del(const std::string& a_key);
    };

    enum class ReaderMode {
        COLLECTING_NAME,
        COLLECTING_TYPE,
        COLLECTING_DELM,
        COLLECTING_DATA,
    };

    DataContainer ParseDataFile(std::filesystem::path a_path);
}

#include <ArsTimoris/DataComponents/DataContainer.tpp>

#endif