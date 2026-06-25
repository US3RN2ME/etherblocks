#ifndef ETHERBLOCKS_GAME_WORLD_HPP
#define ETHERBLOCKS_GAME_WORLD_HPP

#include <functional>
#include <glm/glm.hpp>
#include <vector>

namespace etherblocks::game {

   /**
    * @brief Available block materials stored by the voxel world.
    */
   enum class BlockType {
      Empty,
      VoidCore,
      VoidCrystal,
      VoidGlass,
      VoidBedrock,
      VoidStone,
   };

   /**
    * @brief Fixed-size voxel world storing block types in a dense array.
    */
   class World {
   public:
      /**
       * @brief Callback used when visiting every block in the world.
       *
       * @param position Voxel coordinate of the visited block.
       * @param block Block type stored at the coordinate.
       */
      using BlockVisitor = std::function<void(glm::ivec3, BlockType)>;

      /**
       * @brief Construct a world with the requested voxel dimensions.
       *
       * @param size World size measured in voxels.
       */
      explicit World(glm::ivec3 size = {16, 16, 16});

      /**
       * @brief Set the block type at a voxel position.
       *
       * @param position Voxel coordinate to modify.
       * @param block Block type to store.
       *
       * @return True when the position is inside the world.
       */
      [[nodiscard]] bool setBlock(glm::ivec3 position, BlockType block) noexcept;

      /**
       * @brief Read the block type at a voxel position.
       *
       * @param position Voxel coordinate to read.
       *
       * @return Stored block type, or Empty when the position is outside the world.
       */
      [[nodiscard]] BlockType block(glm::ivec3 position) const noexcept;

      /**
       * @brief Test whether a voxel contains a non-empty block.
       *
       * @param position Voxel coordinate to test.
       *
       * @return True when the position is inside the world and not Empty.
       */
      [[nodiscard]] bool isSolid(glm::ivec3 position) const noexcept;

      /**
       * @brief Return the world dimensions.
       *
       * @return World size measured in voxels.
       */
      [[nodiscard]] glm::ivec3 size() const noexcept;

      /**
       * @brief Visit every block in storage order.
       *
       * @param visitor Callback invoked once per voxel.
       */
      void forEachBlock(const BlockVisitor& visitor) const;

   private:
      [[nodiscard]] bool contains(glm::ivec3 position) const noexcept;
      [[nodiscard]] std::size_t index(glm::ivec3 position) const noexcept;

      glm::ivec3 size_;
      std::vector<BlockType> blocks_;
   };

} // namespace etherblocks::game

#endif // ETHERBLOCKS_GAME_WORLD_HPP
