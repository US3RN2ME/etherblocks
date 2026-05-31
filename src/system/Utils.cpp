#include <etherblocks/system/Logger.hpp>
#include <etherblocks/system/Utils.hpp>
#include <fstream>
#include <sstream>

namespace etherblocks::system {

   std::string Utils::readFile(std::string_view path) {
      const std::string filePath(path);
      std::ifstream in(filePath, std::ios::binary);
      if (!in) {
         log(LogLevel::Error, "Failed to open file: " + filePath);
         return {};
      }

      std::ostringstream content;
      content << in.rdbuf();
      if (in.bad()) {
         log(LogLevel::Error, "Failed to read file: " + filePath);
         return {};
      }
      return content.str();
   }

} // namespace etherblocks::system
