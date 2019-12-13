#include "VoxaNovusException.h"
#include <sstream>

VoxaNovusException::VoxaNovusException(int line, const char* file) noexcept 
	: line(line), file(file) { }

const char* VoxaNovusException::what() const noexcept {
	std::ostringstream sb;
	sb << GetType() << std::endl << GetOriginString();
	whatBuffer = sb.str();
	return whatBuffer.c_str();
}

const char* VoxaNovusException::GetType() const noexcept {
	return "Voxa Novus Exception";
}

int VoxaNovusException::GetLine() const noexcept {
	return line;
}

std::string VoxaNovusException::GetOriginString() const noexcept {
	std::ostringstream sb;
	sb << line << std::endl
		<< "At file '" << file << "'" << std::endl;
	return sb.str();
}