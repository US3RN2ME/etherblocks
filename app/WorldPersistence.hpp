#ifndef APP_WORLDPERSISTENCE_HPP
#define APP_WORLDPERSISTENCE_HPP

#include <string>
#include <string_view>
#include <vector>

namespace etherblocks::game {
   class Player;
   class World;
} // namespace etherblocks::game

namespace etherblocks::app {

   bool saveWorld(const game::World& world, std::string_view path);
   bool loadWorld(game::World& world, std::string_view path);

   struct WorldInfo {
      std::string id;
      std::string name;
      std::string directory;
      std::string savePath;
   };

   bool saveWorldPlayerState(const game::Player& player, const WorldInfo& world);
   bool loadWorldPlayerState(game::Player& player, const WorldInfo& world);
   [[nodiscard]] std::vector<WorldInfo> listWorlds(std::string_view saveDirectory);
   [[nodiscard]] WorldInfo createWorldEntry(std::string_view saveDirectory);
   bool deleteWorld(const WorldInfo& world);
   [[nodiscard]] std::string worldSavePath(std::string_view saveDirectory, std::string_view worldId);

} // namespace etherblocks::app

#endif // APP_WORLDPERSISTENCE_HPP
