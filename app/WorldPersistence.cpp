#include "WorldPersistence.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <etherblocks/game/Player.hpp>
#include <etherblocks/game/World.hpp>
#include <etherblocks/system/Logger.hpp>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>

namespace etherblocks::app {

   namespace {
      namespace sys = system;

      constexpr std::array<char, 4> kWorldSaveMagic{'E', 'B', 'W', '3'};
      constexpr std::string_view kWorldMetadataFile = "metadata.json";
      constexpr std::string_view kWorldSaveFile = "world.ebw";

      std::uint8_t encodeBlock(game::BlockType block) noexcept {
         return std::to_underlying(block);
      }

      std::optional<game::BlockType> decodeBlock(std::uint8_t value) noexcept {
         switch (value) {
            case 0:
               return game::BlockType::Empty;
            case 1:
               return game::BlockType::VoidCore;
            case 2:
               return game::BlockType::VoidCrystal;
            case 3:
               return game::BlockType::VoidGlass;
            case 4:
               return game::BlockType::VoidBedrock;
            case 5:
               return game::BlockType::VoidStone;
            default:
               return {};
         }
      }

      std::string makeWorldId(int index) {
         auto value = std::to_string(index);
         while (value.size() < 3) {
            value.insert(value.begin(), '0');
         }
         return "world_" + value;
      }

      [[nodiscard]] std::filesystem::path metadataPath(const WorldInfo& world) {
         return std::filesystem::path{world.directory} / kWorldMetadataFile;
      }

      [[nodiscard]] nlohmann::json readWorldMetadata(const WorldInfo& world) {
         auto json = nlohmann::json{{"schema", 1}, {"id", world.id}, {"name", world.name}};
         std::ifstream file{metadataPath(world)};
         if (!file) {
            return json;
         }

         try {
            const auto existing = nlohmann::json::parse(file);
            if (existing.is_object()) {
               json = existing;
               json["schema"] = json.value("schema", 1);
               json["id"] = json.value("id", world.id);
               json["name"] = json.value("name", world.name);
            }
         } catch (const std::exception& exception) {
            sys::log(sys::LogLevel::Warning, "Failed to read world metadata: " + world.directory + " " + exception.what());
         }
         return json;
      }

      [[nodiscard]] nlohmann::json vec3Json(glm::vec3 value) {
         return nlohmann::json::array({value.x, value.y, value.z});
      }

      [[nodiscard]] std::optional<glm::vec3> jsonVec3(const nlohmann::json& json, std::string_view key) {
         if (!json.is_object()) {
            return {};
         }
         const auto it = json.find(std::string(key));
         if (it == json.end() || !it->is_array() || it->size() != 3) {
            return {};
         }
         try {
            return glm::vec3{(*it)[0].get<float>(), (*it)[1].get<float>(), (*it)[2].get<float>()};
         } catch (const std::exception&) {
            return {};
         }
      }

      [[nodiscard]] std::string playerModeName(game::PlayerMode mode) {
         return mode == game::PlayerMode::Survival ? "survival" : "creative";
      }

      [[nodiscard]] game::PlayerMode playerModeFromName(const std::string& value) {
         return value == "survival" ? game::PlayerMode::Survival : game::PlayerMode::Creative;
      }

      WorldInfo readWorldInfo(const std::filesystem::path& directory) {
         auto info = WorldInfo{
             directory.filename().string(),
             directory.filename().string(),
             directory.generic_string(),
             (directory / kWorldSaveFile).generic_string(),
         };

         std::ifstream file{directory / kWorldMetadataFile};
         if (!file) {
            return info;
         }

         try {
            const auto json = nlohmann::json::parse(file);
            info.id = json.value("id", info.id);
            info.name = json.value("name", info.name);
         } catch (const std::exception& exception) {
            sys::log(sys::LogLevel::Warning,
                     "Failed to read world metadata: " + directory.generic_string() + " " + exception.what());
         }
         return info;
      }
   } // namespace

   bool saveWorld(const game::World& world, std::string_view path) {
      std::ofstream file{std::string(path), std::ios::binary};
      if (!file) {
         sys::log(sys::LogLevel::Error, "Failed to open world save for writing: " + std::string(path));
         return false;
      }

      const auto size = world.size();
      const std::array<std::int32_t, 3> dimensions{size.x, size.y, size.z};
      const auto blockCount = static_cast<std::uint32_t>(size.x * size.y * size.z);

      file.write(kWorldSaveMagic.data(), kWorldSaveMagic.size());
      file.write(reinterpret_cast<const char*>(dimensions.data()), sizeof(dimensions));
      file.write(reinterpret_cast<const char*>(&blockCount), sizeof(blockCount));

      world.forEachBlock([&](glm::ivec3, game::BlockType block) {
         const auto value = encodeBlock(block);
         file.write(reinterpret_cast<const char*>(&value), sizeof(value));
      });

      if (!file) {
         sys::log(sys::LogLevel::Error, "Failed while writing world save: " + std::string(path));
         return false;
      }

      sys::log(sys::LogLevel::Info, "World saved: " + std::string(path));
      return true;
   }

   bool loadWorld(game::World& world, std::string_view path) {
      std::ifstream file{std::string(path), std::ios::binary};
      if (!file) {
         sys::log(sys::LogLevel::Info, "No world save found: " + std::string(path));
         return false;
      }

      std::array<char, 4> magic{};
      std::array<std::int32_t, 3> dimensions{};
      std::uint32_t blockCount{};
      file.read(magic.data(), magic.size());
      file.read(reinterpret_cast<char*>(dimensions.data()), sizeof(dimensions));
      file.read(reinterpret_cast<char*>(&blockCount), sizeof(blockCount));

      const auto size = world.size();
      const auto expectedBlockCount = static_cast<std::uint32_t>(size.x * size.y * size.z);
      if (!file || magic != kWorldSaveMagic || dimensions != std::array<std::int32_t, 3>{size.x, size.y, size.z} ||
          blockCount != expectedBlockCount) {
         sys::log(sys::LogLevel::Warning, "World save is incompatible or corrupt: " + std::string(path));
         return false;
      }

      for (auto y = 0; y < size.y; ++y) {
         for (auto z = 0; z < size.z; ++z) {
            for (auto x = 0; x < size.x; ++x) {
               std::uint8_t value{};
               file.read(reinterpret_cast<char*>(&value), sizeof(value));
               const auto block = decodeBlock(value);
               if (!file || !block) {
                  sys::log(sys::LogLevel::Warning,
                           "World save ended unexpectedly or contains an unknown block: " + std::string(path));
                  return false;
               }
               static_cast<void>(world.setBlock({x, y, z}, *block));
            }
         }
      }

      sys::log(sys::LogLevel::Info, "World loaded: " + std::string(path));
      return true;
   }

   bool saveWorldPlayerState(const game::Player& player, const WorldInfo& world) {
      auto json = readWorldMetadata(world);
      const auto& camera = player.camera();
      json["player"] = nlohmann::json{
          {"camera_position", vec3Json(camera.position())},
          {"yaw", camera.yaw()},
          {"pitch", camera.pitch()},
          {"mode", playerModeName(player.mode())},
      };

      std::ofstream file{metadataPath(world)};
      if (!file) {
         sys::log(sys::LogLevel::Error, "Failed to open world metadata for writing: " + world.directory);
         return false;
      }
      file << json.dump(2) << '\n';
      return true;
   }

   bool loadWorldPlayerState(game::Player& player, const WorldInfo& world) {
      const auto json = readWorldMetadata(world);
      const auto state = json.value("player", nlohmann::json::object());
      if (!state.is_object()) {
         return false;
      }
      const auto cameraPosition = jsonVec3(state, "camera_position");
      if (!cameraPosition) {
         return false;
      }

      try {
         player.setCameraPose(*cameraPosition, state.value("yaw", player.camera().yaw()),
                              state.value("pitch", player.camera().pitch()));
         player.setMode(playerModeFromName(state.value("mode", playerModeName(player.mode()))));
      } catch (const std::exception& exception) {
         sys::log(sys::LogLevel::Warning, "Failed to load world player state: " + world.directory + " " + exception.what());
         return false;
      }
      return true;
   }

   std::vector<WorldInfo> listWorlds(std::string_view saveDirectory) {
      const auto root = std::filesystem::path{saveDirectory};
      std::error_code error;
      std::filesystem::create_directories(root, error);

      auto worlds = std::vector<WorldInfo>{};
      if (!std::filesystem::exists(root, error)) {
         return worlds;
      }

      for (const auto& entry : std::filesystem::directory_iterator(root, error)) {
         if (entry.is_directory()) {
            worlds.push_back(readWorldInfo(entry.path()));
         }
      }
      std::sort(worlds.begin(), worlds.end(), [](const WorldInfo& lhs, const WorldInfo& rhs) {
         return lhs.name < rhs.name;
      });
      return worlds;
   }

   WorldInfo createWorldEntry(std::string_view saveDirectory) {
      const auto root = std::filesystem::path{saveDirectory};
      std::error_code error;
      std::filesystem::create_directories(root, error);

      auto index = 1;
      auto directory = root / makeWorldId(index);
      while (std::filesystem::exists(directory, error)) {
         ++index;
         directory = root / makeWorldId(index);
      }
      std::filesystem::create_directories(directory, error);

      auto info = WorldInfo{
          directory.filename().string(),
          "WORLD " + std::to_string(index),
          directory.generic_string(),
          (directory / kWorldSaveFile).generic_string(),
      };

      std::ofstream file{directory / kWorldMetadataFile};
      if (file) {
         file << nlohmann::json{{"schema", 1}, {"id", info.id}, {"name", info.name}}.dump(2) << '\n';
      }
      return info;
   }

   bool deleteWorld(const WorldInfo& world) {
      std::error_code error;
      const auto removed = std::filesystem::remove_all(std::filesystem::path{world.directory}, error);
      if (error) {
         sys::log(sys::LogLevel::Error, "Failed to delete world: " + world.directory + " " + error.message());
         return false;
      }
      if (removed == 0) {
         sys::log(sys::LogLevel::Warning, "World directory was not found for deletion: " + world.directory);
         return false;
      }
      sys::log(sys::LogLevel::Info, "World deleted: " + world.directory);
      return true;
   }

   std::string worldSavePath(std::string_view saveDirectory, std::string_view worldId) {
      return (std::filesystem::path{saveDirectory} / std::string(worldId) / kWorldSaveFile).generic_string();
   }

} // namespace etherblocks::app
