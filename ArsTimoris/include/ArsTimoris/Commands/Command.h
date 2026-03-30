#ifndef COMMAND_H
#define COMMAND_H
#include <string>
#include <vector>
#include <variant>
#include <functional>
#include <iostream>
#include <sstream>

namespace ArsTimoris::Commands {
    /**
     * @typedef Parameter
     * @brief Defines a variant type for command parameters, allowing different data types.
     *
     * Parameter is used within the Command struct to represent values of varying types
     * that can be passed as arguments, such as integers, floating-point numbers, strings, 
     * or boolean values.
     */
    typedef std::variant<int, double, std::string, bool> Parameter;

    /**
     * @enum ParameterType
     * @brief Specifies the type of parameter for command handling.
     *
     * The ParameterType enum defines the types of parameters that can
     * be used in command handling within the system, allowing for
     * type-specific processing.
     */
    enum ParameterType {
        INT,    ///< Integer parameter
        DOUBLE, ///< Floating-point parameter
        STRING, ///< String parameter
        BOOL    ///< Boolean parameter
    };

    /**
     * @struct Command
     * @brief Represents a command with a name, description, parameters, and an executable function.
     *
     * The Command struct allows for defining commands that can be executed on the server, each
     * with a specific name, description, and associated parameters. It also supports a function
     * that is called when the command is executed.
     */
    struct Command;
}

#include <ArsTimoris/Commands/CommandHandler.h>

namespace ArsTimoris::Commands {
    /**
     * @struct Command
     * @brief Represents a command with a name, description, parameters, and an executable function.
     *
     * The Command struct allows for defining commands that can be executed on the server, each
     * with a specific name, description, and associated parameters. It also supports a function
     * that is called when the command is executed.
     */
    struct Command {
        std::string name; ///< The name of the command.
        std::string description; ///< The description of the command.
        std::vector<ParameterType> parameters; ///< The parameters of the command.
        std::function<void(const CommandHandler& commandHandler, const std::vector<Parameter>&)> function; ///< The function to execute when the command is executed.

        /**
         * @brief Constructs a Command with a name and description.
         * @param name The name of the command.
         * @param description A brief description of the command.
         */
        Command(std::string name, std::string description);

        /**
         * @brief Constructs a Command with a name and description.
         * @param name The name of the command.
         * @param function The function to execute when the command is called.
         */
        Command(std::string name, std::function<void(const CommandHandler& commandHandler, std::vector<Parameter>)> function);

        /**
         * @brief Constructs a Command with a name, description, and function.
         * @param name The name of the command.
         * @param description A brief description of the command.
         * @param function The function to execute when the command is called.
         */
        Command(std::string name, std::string description, std::function<void(const CommandHandler& commandHandler, std::vector<Parameter>)> function);

        /// @brief Adds a parameter to the command.
        /// @param a_param The parameter value to add to the command.
        void AddParameter(ParameterType a_param);

        /// @brief Tokenizes a command input stream into parameters.
        /// 
        /// This function processes each character from the input stream to create a list of parameters. Parameters may be of different types, including integers, doubles, booleans, or strings, depending on their format in the stream. Quotation marks denote string parameters, and the function can detect different data types by examining the format of each token.
        /// 
        /// @param a_stream The input stream containing the command.
        /// @param a_parameters The vector to store the parsed parameters.
        static void Tokenize(std::istringstream& a_stream, std::vector<Parameter>& a_parameters);
    };
}
#endif