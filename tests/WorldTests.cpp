#include <cstddef>
#include <etherblocks/game/World.hpp>
#include <glm/glm.hpp>
#include <vector>

#include "ut_main.hpp"

namespace {
   void clearWorld(etherblocks::game::World& world) {
      world.forEachBlock([&](glm::ivec3 position, etherblocks::game::BlockType) {
         static_cast<void>(world.setBlock(position, etherblocks::game::BlockType::Empty));
      });
   }

   suite<"[World]"> _ = [] {
      "[ClampsNegativeDimensionsToZero]"_test = [] {
         const etherblocks::game::World world{{-4, 2, -1}};

         expect(eq(world.size().x, 0_i));
         expect(eq(world.size().y, 2_i));
         expect(eq(world.size().z, 0_i));
         expect(world.block({0, 0, 0}) == etherblocks::game::BlockType::Empty);
         expect(!world.isSolid({0, 0, 0}));
      };

      "[DoesNotVisitBlocksWhenAnyDimensionIsZero]"_test = [] {
         const etherblocks::game::World world{{3, 0, 2}};
         std::size_t visited = 0;

         world.forEachBlock([&](glm::ivec3, etherblocks::game::BlockType) {
            ++visited;
         });

         expect(eq(visited, std::size_t{0}));
      };

      "[InitializesAtLeastBottomLayerForPositiveHeightWorlds]"_test = [] {
         const etherblocks::game::World world{{4, 4, 4}};

         for (auto z = 0; z < world.size().z; ++z) {
            for (auto x = 0; x < world.size().x; ++x) {
               expect(world.isSolid({x, 0, z}));
            }
         }
      };

      "[StoresBlocksAndRejectsOutOfBoundsWrites]"_test = [] {
         etherblocks::game::World world{{2, 2, 2}};
         const auto position = glm::ivec3{1, 1, 1};

         static_cast<void>(world.setBlock(position, etherblocks::game::BlockType::Empty));
         expect(world.setBlock(position, etherblocks::game::BlockType::VoidCrystal));
         expect(world.block(position) == etherblocks::game::BlockType::VoidCrystal);
         expect(world.isSolid(position));
         expect(!world.setBlock({2, 1, 1}, etherblocks::game::BlockType::VoidCore));
         expect(world.block({2, 1, 1}) == etherblocks::game::BlockType::Empty);
      };

      "[RejectsNegativeOutOfBoundsWrites]"_test = [] {
         etherblocks::game::World world{{2, 2, 2}};

         expect(!world.setBlock({-1, 0, 0}, etherblocks::game::BlockType::VoidCore));
         expect(!world.setBlock({0, -1, 0}, etherblocks::game::BlockType::VoidCore));
         expect(!world.setBlock({0, 0, -1}, etherblocks::game::BlockType::VoidCore));
         expect(world.block({-1, 0, 0}) == etherblocks::game::BlockType::Empty);
         expect(world.block({0, -1, 0}) == etherblocks::game::BlockType::Empty);
         expect(world.block({0, 0, -1}) == etherblocks::game::BlockType::Empty);
      };

      "[ReturnsFalseForIdempotentWritesAndTrueForChanges]"_test = [] {
         etherblocks::game::World world{{1, 1, 1}};
         const auto position = glm::ivec3{0, 0, 0};

         static_cast<void>(world.setBlock(position, etherblocks::game::BlockType::Empty));
         expect(!world.setBlock(position, etherblocks::game::BlockType::Empty));
         expect(world.setBlock(position, etherblocks::game::BlockType::VoidCore));
         expect(!world.setBlock(position, etherblocks::game::BlockType::VoidCore));
         expect(world.setBlock(position, etherblocks::game::BlockType::VoidStone));
         expect(world.block(position) == etherblocks::game::BlockType::VoidStone);
      };

      "[ClearsBlocksBackToEmpty]"_test = [] {
         etherblocks::game::World world{{1, 1, 1}};
         const auto position = glm::ivec3{0, 0, 0};

         static_cast<void>(world.setBlock(position, etherblocks::game::BlockType::VoidGlass));
         expect(world.isSolid(position));
         expect(world.setBlock(position, etherblocks::game::BlockType::Empty));
         expect(world.block(position) == etherblocks::game::BlockType::Empty);
         expect(!world.isSolid(position));
      };

      "[VisitsEveryBlockInStorageOrder]"_test = [] {
         etherblocks::game::World world{{2, 2, 2}};
         std::size_t visited = 0;
         auto previous = glm::ivec3{0, 0, 0};

         world.forEachBlock([&](glm::ivec3 position, etherblocks::game::BlockType) {
            if (visited == 0) {
               expect(eq(position.x, 0_i));
               expect(eq(position.y, 0_i));
               expect(eq(position.z, 0_i));
            } else {
               expect(position.y > previous.y || position.z > previous.z || position.x > previous.x);
            }
            previous = position;
            ++visited;
         });

         expect(eq(visited, std::size_t{8}));
      };

      "[VisitorSeesEditedBlockValues]"_test = [] {
         etherblocks::game::World world{{2, 2, 1}};
         clearWorld(world);
         static_cast<void>(world.setBlock({1, 0, 0}, etherblocks::game::BlockType::VoidCore));
         static_cast<void>(world.setBlock({0, 1, 0}, etherblocks::game::BlockType::VoidCrystal));
         auto solidPositions = std::vector<glm::ivec3>{};

         world.forEachBlock([&](glm::ivec3 position, etherblocks::game::BlockType block) {
            if (block != etherblocks::game::BlockType::Empty) {
               solidPositions.push_back(position);
            }
         });

         expect(eq(solidPositions.size(), std::size_t{2}));
         expect(solidPositions[0] == glm::ivec3{1, 0, 0});
         expect(solidPositions[1] == glm::ivec3{0, 1, 0});
      };
   };
} // namespace
