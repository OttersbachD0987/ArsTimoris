#include <ArsTimoris/Commands/Command.h>
#include <vector>
#include <sstream>
#include <string>
#include <cctype>
#include <stdexcept>
#include <iostream>

namespace ArsTimoris::Commands {
    Command::Command(std::string name, std::string description) {
        this->name = name;
        this->description = description;
    }

    Command::Command(std::string name, std::function<void(const CommandHandler& commandHandler, std::vector<Parameter>)> function) {
        this->name = name;
        this->function = function;
    }

    Command::Command(std::string name, std::string description, std::function<void(const CommandHandler& commandHandler, std::vector<Parameter>)> function) {
        this->name = name;
        this->description = description;
        this->function = function;
    }

    void Command::AddParameter(ParameterType a_param) {
        parameters.push_back(a_param);
    }

    void Command::Tokenize(std::istringstream& a_stream, std::vector<Parameter>& a_parameters) {
        bool inQuotes = false;
        bool hasBeenString = false;

        std::string currentParam = "";

        // Loop over each character in the stream
        while (a_stream.good()) {
            char c = a_stream.get();
            if (!isprint(c) && c != ' ' && c != '"') {
                std::cout << "Non-printable character: " << c << std::endl;
                continue;
            }
            if (c == '"') {
                hasBeenString = true;
                inQuotes = !inQuotes;
                continue;
            }
            if (c == ' ' && !inQuotes && currentParam != "") {
                if (currentParam != "") {
                    // Add the parameter to the result, make sure to parse the correct type
                    if (hasBeenString) {
                        a_parameters.push_back(Parameter(currentParam));
                        std::cout << "Pushed string: " << currentParam << std::endl;
                        hasBeenString = false;
                    } else if (currentParam == "true" || currentParam == "false") {
                        a_parameters.push_back(Parameter(currentParam == "true"));
                        std::cout << "Pushed bool: " << currentParam << std::endl;
                    } else {
                        try {
                            if (currentParam.find('.') != std::string::npos) {
                                a_parameters.push_back(Parameter(std::stod(currentParam)));
                                std::cout << "Pushed double: " << currentParam << std::endl;
                            } else {
                                a_parameters.push_back(Parameter(std::stoi(currentParam)));
                                std::cout << "Pushed int: " << currentParam << std::endl;
                            }
                        } catch (const std::invalid_argument&) {
                            a_parameters.push_back(Parameter(currentParam));  // Fallback to string, even if quotes weren't triggered
                            hasBeenString = false;
                            std::cout << "Pushed string: " << currentParam << std::endl;
                        }
                    }
                    //parameters.push_back(Parameter(currentParam));
                    currentParam = "";
                }
                continue;
            }
            currentParam += c;
        }

        // Push the last parameter if any
        if (!currentParam.empty()) {
            if (hasBeenString) {
                a_parameters.push_back(Parameter(currentParam));
            } else if (currentParam == "true" || currentParam == "false") {
                a_parameters.push_back(Parameter(currentParam == "true"));
            } else {
                try {
                    if (currentParam.find('.') != std::string::npos) {
                        a_parameters.push_back(Parameter(std::stod(currentParam)));
                    } else {
                        a_parameters.push_back(Parameter(std::stoi(currentParam)));
                    }
                } catch (const std::invalid_argument&) {
                    a_parameters.push_back(Parameter(currentParam));
                }
            }
        }
    }
}