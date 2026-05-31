#ifndef ETHERBLOCKS_SYSTEM_UTILS_HPP
#define ETHERBLOCKS_SYSTEM_UTILS_HPP

#include <string>
#include <string_view>

namespace etherblocks::system {

   struct Utils {
      static std::string readFile(std::string_view path);
   };

} // namespace etherblocks::system

#endif // ETHERBLOCKS_SYSTEM_UTILS_HPP
