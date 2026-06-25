#ifndef APP_BLOCKASSETS_HPP
#define APP_BLOCKASSETS_HPP

#include <string>
#include <string_view>

namespace etherblocks::app {

   struct BlockTexturePaths {
      std::string voidCore;
      std::string voidCrystal;
      std::string voidGlass;
      std::string voidBedrock;
      std::string voidStone;
   };

   [[nodiscard]] BlockTexturePaths loadBlockTexturePaths(std::string_view path);

} // namespace etherblocks::app

#endif // APP_BLOCKASSETS_HPP
