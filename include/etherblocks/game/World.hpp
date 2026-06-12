#ifndef ETHERBLOCKS_GAME_WORLD_HPP
#define ETHERBLOCKS_GAME_WORLD_HPP

#include <functional>
#include <glm/glm.hpp>
#include <vector>

namespace etherblocks::game {

   enum class BlockType {
      Empty,
      EtherCrystalBlue,
      EtherCrystalViolet,
      EtherGlass,
      EtherStoneBlue,
      EtherStoneViolet,
   };

   class World {
   public:
      using BlockVisitor = std::function<void(glm::ivec3, BlockType)>;

      explicit World(glm::ivec3 size = {16, 16, 16});

      [[nodiscard]] bool setBlock(glm::ivec3 position, BlockType block) noexcept;
      [[nodiscard]] BlockType block(glm::ivec3 position) const noexcept;
      [[nodiscard]] bool isSolid(glm::ivec3 position) const noexcept;
      [[nodiscard]] glm::ivec3 size() const noexcept;

      void forEachBlock(const BlockVisitor& visitor) const;

   private:
      [[nodiscard]] bool contains(glm::ivec3 position) const noexcept;
      [[nodiscard]] std::size_t index(glm::ivec3 position) const noexcept;

      glm::ivec3 size_;
      std::vector<BlockType> blocks_;
   };

} // namespace etherblocks::game

#endif // ETHERBLOCKS_GAME_WORLD_HPP
