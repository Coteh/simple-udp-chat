#pragma once
#include <string>

namespace cipher {
	std::string encrypt(std::string _inputStr, int _offset);
	std::string decrypt(std::string _inputStr, int _offset);
	bool checkIfUppercase(char _charToCheck);
	bool checkIfLowercase(char _charToCheck);
	char bound(char _charToCheck, char _left, char _right);
}
