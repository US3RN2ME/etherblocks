#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <etherblocks/game/World.hpp>
#include <etherblocks/system/Logger.hpp>
#include <random>
#include <string>

#define STB_PERLIN_IMPLEMENTATION
#include <stb_perlin.h>

namespace etherblocks::game {

   namespace {
      constexpr auto kTerrainFrequency = 0.085f;
      constexpr auto kMaterialFrequency = 0.045f;
      constexpr auto kVeinFrequency = 0.13f;
      constexpr auto kSeedScale = 0.013f;

      std::uint32_t makeSeed() {
         std::random_device random;
         const auto timeSeed = static_cast<std::uint32_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
         return random() ^ (timeSeed + 0x9e3779b9U);
      }

      int terrainHeight(int x, int z, int maxHeight, std::uint32_t seed) {
         if (maxHeight <= 1) {
            return 1;
         }

         const auto seedOffsetX = static_cast<float>(seed & 0xffffU) * kSeedScale;
         const auto seedOffsetZ = static_cast<float>((seed >> 16U) & 0xffffU) * kSeedScale;
         const auto noise = stb_perlin_noise3(static_cast<float>(x) * kTerrainFrequency + seedOffsetX, 0.0f,
                                              static_cast<float>(z) * kTerrainFrequency + seedOffsetZ, 0, 0, 0);
         const auto normalized = std::clamp(noise * 0.5f + 0.5f, 0.0f, 1.0f);
         return 1 + static_cast<int>(normalized * static_cast<float>(maxHeight - 1));
      }

      float biomeNoise(int x, int y, int z, float frequency, std::uint32_t seed, float offset) {
         const auto seedOffsetX = static_cast<float>(seed & 0xffffU) * kSeedScale + offset;
         const auto seedOffsetZ = static_cast<float>((seed >> 16U) & 0xffffU) * kSeedScale - offset;
         return stb_perlin_noise3(static_cast<float>(x) * frequency + seedOffsetX, static_cast<float>(y) * frequency * 0.65f,
                                  static_cast<float>(z) * frequency + seedOffsetZ, 0, 0, 0);
      }

      BlockType biomeBlock(int x, int y, int z, int height, std::uint32_t seed) {
         const auto surfaceDepth = height - 1 - y;
         const auto broad = biomeNoise(x, y, z, kMaterialFrequency, seed, 19.0f);
         const auto vein = std::abs(biomeNoise(x, y, z, kVeinFrequency, seed, 71.0f));
         const auto crystalPocket = biomeNoise(x, y, z, kVeinFrequency * 0.72f, seed, 131.0f);

         if (surfaceDepth == 0) {
            if (crystalPocket > 0.48f) {
               return BlockType::VoidCore;
            }
            if (vein < 0.16f) {
               return BlockType::VoidGlass;
            }
            return broad > 0.0f ? BlockType::VoidStone : BlockType::VoidBedrock;
         }

         if (surfaceDepth <= 3) {
            if (vein < 0.13f) {
               return BlockType::VoidCrystal;
            }
            return broad > -0.18f ? BlockType::VoidStone : BlockType::VoidBedrock;
         }

         if (vein < 0.08f) {
            return crystalPocket > 0.0f ? BlockType::VoidCore : BlockType::VoidCrystal;
         }

         return y < 3 ? BlockType::VoidBedrock : BlockType::VoidStone;
      }
   } // namespace

   World::World(glm::ivec3 size)
       : size_(glm::max(size, glm::ivec3{0}))
       , blocks_(static_cast<std::size_t>(size_.x) * static_cast<std::size_t>(size_.y) * static_cast<std::size_t>(size_.z)) {
      const auto seed = makeSeed();
      const auto maxTerrainHeight = std::max(1, size_.y - 2);
      for (auto z = 0; z < size_.z; ++z) {
         for (auto x = 0; x < size_.x; ++x) {
            const auto height = terrainHeight(x, z, maxTerrainHeight, seed);
            for (auto y = 0; y < height; ++y) {
               static_cast<void>(setBlock({x, y, z}, biomeBlock(x, y, z, height, seed)));
            }
         }
      }
      system::log(system::LogLevel::Info, "World initialized: " + std::to_string(size_.x) + "x" + std::to_string(size_.y) +
                                              "x" + std::to_string(size_.z) + ", seed=" + std::to_string(seed));
   }

   bool World::setBlock(glm::ivec3 position, BlockType block) noexcept {
      if (!contains(position) || blocks_[index(position)] == block) {
         return false;
      }

      blocks_[index(position)] = block;
      return true;
   }

   BlockType World::block(glm::ivec3 position) const noexcept {
      return contains(position) ? blocks_[index(position)] : BlockType::Empty;
   }

   bool World::isSolid(glm::ivec3 position) const noexcept {
      return block(position) != BlockType::Empty;
   }

   glm::ivec3 World::size() const noexcept {
      return size_;
   }

   void World::forEachBlock(const BlockVisitor& visitor) const {
      for (auto y = 0; y < size_.y; ++y) {
         for (auto z = 0; z < size_.z; ++z) {
            for (auto x = 0; x < size_.x; ++x) {
               const glm::ivec3 position{x, y, z};
               visitor(position, blocks_[index(position)]);
            }
         }
      }
   }

   bool World::contains(glm::ivec3 position) const noexcept {
      return position.x >= 0 && position.y >= 0 && position.z >= 0 && position.x < size_.x && position.y < size_.y &&
             position.z < size_.z;
   }

   std::size_t World::index(glm::ivec3 position) const noexcept {
      return static_cast<std::size_t>(position.x) +
             (static_cast<std::size_t>(size_.x) * (static_cast<std::size_t>(position.y) +
                                                   static_cast<std::size_t>(size_.y) * static_cast<std::size_t>(position.z)));
   }

} // namespace etherblocks::game
