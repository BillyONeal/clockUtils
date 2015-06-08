#include "clockUtils/argParser/Parser.h"

#include <iostream>
#include <map>

#include "clockUtils/errors.h"
#include "clockUtils/argParser/BasicVariable.h"

namespace clockUtils {
namespace argParser {

	std::vector<BasicVariable *> Parser::variableList = std::vector<BasicVariable *>();
	std::string Parser::error = std::string();
	bool Parser::errorOnFlag = true;
	bool Parser::help = false;
	std::vector<std::string> * Parser::arguments = nullptr;

	ClockError Parser::parseArguments(const char ** argv, int argc) {
		error = "";
		help = false;
		ClockError result = ClockError::SUCCESS;
		std::map<std::string, bool> parsed;
		while (argc > 0) {
			if (argv[0][0] == '-' && argv[0][1] != '\0') {
				std::string name(&argv[0][1]);

				bool found = false;

				for (BasicVariable * bv : variableList) {
					if (name.find(bv->getName()) == 0) {
						found = true;
						if (bv->isBool()) {
							if (bv->getName().length() == name.length()) {
								bv->setValue("1");
								if (parsed.find(bv->getName()) != parsed.end()) {
									arguments = nullptr;
									return ClockError::INVALID_USAGE;
								}
								bv->_set = true;
								parsed[bv->getName()] = true;
							} else {
								found = false;
								continue;
							}
						} else {
							if (name.length() > bv->getName().length()) {
								size_t startIndex = bv->getName().length();
								if (name.at(startIndex) == '=') {
									startIndex++;
								}
								if (!bv->setValue(name.substr(startIndex, name.length()))) {
									error = std::string(argv[1]) + std::string(" is not a valid value for variable ") + name;
									arguments = nullptr;
									return ClockError::INVALID_USAGE;
								}
							} else if (argc == 1) {
								error = name + std::string(" requires a value: -") + name + std::string(" <value> or -") + name + std::string("<value> or -") + name + std::string("=<value>");
								arguments = nullptr;
								return ClockError::INVALID_USAGE;
							} else {
								if (!bv->setValue(argv[1])) {
									error = std::string(argv[1]) + std::string(" is not a valid value for variable ") + name;
									arguments = nullptr;
									return ClockError::INVALID_USAGE;
								}
								if (parsed.find(bv->getName()) != parsed.end()) {
									arguments = nullptr;
									return ClockError::INVALID_USAGE;
								}
								bv->_set = true;
								parsed[bv->getName()] = true;
								argc--;
								argv++;
							}
						}
						break;
					}
				}

				if (!found) {
					if (name == "-help") {
						help = true;
					} else if (errorOnFlag) {
						error = std::string("argument -") + name + std::string(" not registered!");
						arguments = nullptr;
						return ClockError::INVALID_USAGE;
					} else {
						if (arguments == nullptr) {
							arguments = nullptr;
							return ClockError::INVALID_USAGE;
						} else {
							arguments->push_back(argv[0]);
						}
					}
				}
			} else {
				if (arguments == nullptr) {
					arguments = nullptr;
					return ClockError::INVALID_USAGE;
				} else {
					arguments->push_back(argv[0]);
				}
			}

			argv++;
			argc--;
		}

		arguments = nullptr;
		return result;
	}

	std::string Parser::getLastParserError() {
		return error;
	}

	std::string Parser::getHelpText() {
		std::map<std::string, BasicVariable *> map;
		for (auto v : variableList) {
			map.insert(std::make_pair(v->getName(), v));
		}
		std::string text;
		for (auto it = map.begin(); it != map.end();) {
			text += "\t-" + it->second->getName() + "\t[Default: " + it->second->_defaultValue + "]\t\t" + it->second->_description;
			it++;
			if (it != map.end()) {
				text += "\n";
			}
		}
		return text;
	}

} /* namespace argParser */
} /* namespace clockUtils */
