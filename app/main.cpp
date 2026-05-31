#include <exception>
#include <iostream>

#include "Application.hpp"

// The process boundary catches both standard and unknown exceptions below.
// NOLINTNEXTLINE(bugprone-exception-escape)
int main() {
   try {
      etherblocks::app::Application application;
      application.run();
   } catch (const std::exception& error) {
      std::cerr << error.what() << '\n';
      return 1;
   } catch (...) {
      std::cerr << "Unknown fatal error\n";
      return 1;
   }
}
