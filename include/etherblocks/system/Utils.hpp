#ifndef ETHERBLOCKS_SYSTEM_UTILS_HPP
#define ETHERBLOCKS_SYSTEM_UTILS_HPP

#include <string>
#include <string_view>

namespace etherblocks::system {

   /**
    * @brief Miscellaneous system helper functions.
    */
   struct Utils {
      /**
       * @brief Read an entire file into a string.
       *
       * @param path Path to the file.
       *
       * @return File contents.
       */
      static std::string readFile(std::string_view path);
   };

} // namespace etherblocks::system

#endif // ETHERBLOCKS_SYSTEM_UTILS_HPP
