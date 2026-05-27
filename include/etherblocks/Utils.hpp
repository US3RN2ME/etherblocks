#ifndef ETHERBLOCKS_UTILS_HPP
#define ETHERBLOCKS_UTILS_HPP

#include <string>
#include <string_view>

namespace etherblocks {

   struct Utils {
      static std::string readFile(std::string_view path);
   };

} // namespace etherblocks

#endif // ETHERBLOCKS_UTILS_HPP
