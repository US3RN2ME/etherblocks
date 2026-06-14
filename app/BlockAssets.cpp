#include "BlockAssets.hpp"

#include <fstream>
#include <nlohmann/json.hpp>
#include <stdexcept>

namespace etherblocks::app {

   namespace {
      std::string blockTexturePath(const nlohmann::json& manifest, std::string_view blockName) {
         return manifest.at("blocks").at(std::string(blockName)).at("texture").get<std::string>();
      }
   } // namespace

   BlockTexturePaths loadBlockTexturePaths(std::string_view path) {
      std::ifstream file{std::string(path)};
      if (!file) {
         throw std::runtime_error{"Failed to open block texture manifest: " + std::string(path)};
      }

      const auto manifest = nlohmann::json::parse(file);
      return {
          blockTexturePath(manifest, "void_core"),  blockTexturePath(manifest, "void_crystal"),
          blockTexturePath(manifest, "void_glass"), blockTexturePath(manifest, "void_bedrock"),
          blockTexturePath(manifest, "void_stone"),
      };
   }

} // namespace etherblocks::app
