#include <etherblocks/system/Logger.hpp>
#include <exception>

#include "Application.hpp"

// The process boundary catches both standard and unknown exceptions below.
// NOLINTNEXTLINE(bugprone-exception-escape)
int main() {
   try {
      etherblocks::app::Application application;
      application.run();
   } catch (const std::exception& error) {
      etherblocks::system::log(etherblocks::system::LogLevel::Error, error.what());
      return 1;
   } catch (...) {
      etherblocks::system::log(etherblocks::system::LogLevel::Error, "Unknown fatal error");
      return 1;
   }
}
