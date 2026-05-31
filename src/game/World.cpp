#include <algorithm>
#include <cstddef>
#include <etherblocks/game/World.hpp>
#include <etherblocks/system/Logger.hpp>
#include <string>

namespace etherblocks::game {

   World::World(glm::ivec3 size)
       : size_(glm::max(size, glm::ivec3{0}))
       , blocks_(static_cast<std::size_t>(size_.x) * static_cast<std::size_t>(size_.y) * static_cast<std::size_t>(size_.z)) {
      for (auto z = 0; z < size_.z; ++z) {
         for (auto x = 0; x < size_.x; ++x) {
            static_cast<void>(setBlock({x, 0, z}, BlockType::Basic));
         }
      }
      system::log(system::LogLevel::Info, "World initialized: " + std::to_string(size_.x) + "x" + std::to_string(size_.y) +
                                              "x" + std::to_string(size_.z));
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
