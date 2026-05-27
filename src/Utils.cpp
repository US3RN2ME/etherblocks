#include <etherblocks/Utils.hpp>
#include <fstream>
#include <iostream>

namespace etherblocks {

   std::string Utils::readFile(std::string_view path) {
      std::ifstream in(path.data(), std::ios::binary);
      if (!in) {
         std::cout << "Failed to open file: " << path << std::endl;
         return {};
      }

      return std::string{std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
   }

} // namespace etherblocks
