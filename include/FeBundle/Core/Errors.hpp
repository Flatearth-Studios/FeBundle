#ifndef INCLUDE_FEBUNDLE_CORE_ERRORS_HPP_
#define INCLUDE_FEBUNDLE_CORE_ERRORS_HPP_

namespace febundle {

enum class ErrorName {
  Null,
  BadAllocation,
  AllocationException,
  CreateWindow,
  CreateRenderer,
  EnableVSync,
  RenderCall,
  RenderSprites,
  ImGuiBackendInit,
  InitializeGameCallback,
  LoadImage,
  NullReference,
  ResizeWindow,
};

enum class ErrorType {
  Generic,
  Engine,
  TomlParser,
};

struct Error {
  ErrorType type;
  ErrorName name; 

  Error(ErrorName name) : type(ErrorType::Generic), name(name) {}
  Error(ErrorType type, ErrorName name) : type(type), name(name) {}
};

}

#endif // INCLUDE_FEBUNDLE_CORE_ERRORS_HPP_
