#pragma once

#include <string>
#include <exception>
#include <stdexcept>

class AppException : public std::runtime_error {
public:
	explicit AppException(const std::string& message): runtime_error(message) {};
};

class GraphicsException : public AppException {
public:
	explicit GraphicsException(const std::string& message) : AppException(message) {};
};

class ShaderException : public GraphicsException {
public:
	explicit ShaderException(const std::string& message): GraphicsException(message) {};
};