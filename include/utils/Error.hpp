#pragma once

#include <stdexcept>
#include <string>

class BookstoreError : public std::runtime_error {
 public:
  explicit BookstoreError(std::string&& message);

  const std::string& message() const noexcept;

 private:
  std::string message_;
};
