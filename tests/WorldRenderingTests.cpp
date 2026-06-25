#include <algorithm>
#include <cstddef>
#include <etherblocks/engine/WorldRendering.hpp>
#include <etherblocks/game/World.hpp>
#include <glm/glm.hpp>

#include "ut_main.hpp"

namespace {
   void clearWorld(etherblocks::game::World& world) {
      world.forEachBlock([&](glm::ivec3 position, etherblocks::game::BlockType) {
         static_cast<void>(world.setBlock(position, etherblocks::game::BlockType::Empty));
      });
   }

   bool containsPosition(const std::array<etherblocks::engine::MeshVertex, 24>& vertices, glm::vec3 position) {
      return std::any_of(vertices.begin(), vertices.end(), [position](const etherblocks::engine::MeshVertex& vertex) {
         return vertex.position == position;
      });
   }

   suite<"[WorldRendering]"> _ = [] {
      "[MeshLayoutDescribesMeshVertexAttributes]"_test = [] {
         const auto layout = etherblocks::engine::meshLayout();

         expect(eq(layout.size(), std::size_t{3}));
         expect(eq(layout[0].index, 0_u));
         expect(eq(layout[0].componentCount, 3_i));
         expect(eq(layout[1].index, 1_u));
         expect(eq(layout[1].componentCount, 2_i));
         expect(eq(layout[2].index, 2_u));
         expect(eq(layout[2].componentCount, 3_i));
      };

      "[BuildsSixFacesForIsolatedBlock]"_test = [] {
         etherblocks::game::World world{{3, 3, 3}};
         clearWorld(world);
         static_cast<void>(world.setBlock({1, 1, 1}, etherblocks::game::BlockType::VoidCore));

         const auto vertices = etherblocks::engine::buildWorldMesh(world, etherblocks::game::BlockType::VoidCore);

         expect(eq(vertices.size(), std::size_t{36}));
      };

      "[OmitsInternalFacesBetweenAdjacentSolidBlocks]"_test = [] {
         etherblocks::game::World world{{3, 3, 3}};
         clearWorld(world);
         static_cast<void>(world.setBlock({1, 1, 1}, etherblocks::game::BlockType::VoidCore));
         static_cast<void>(world.setBlock({2, 1, 1}, etherblocks::game::BlockType::VoidCore));

         const auto vertices = etherblocks::engine::buildWorldMesh(world, etherblocks::game::BlockType::VoidCore);

         expect(eq(vertices.size(), std::size_t{60}));
      };

      "[DifferentSolidBlockTypesStillOccludeFaces]"_test = [] {
         etherblocks::game::World world{{3, 3, 3}};
         clearWorld(world);
         static_cast<void>(world.setBlock({1, 1, 1}, etherblocks::game::BlockType::VoidCore));
         static_cast<void>(world.setBlock({2, 1, 1}, etherblocks::game::BlockType::VoidStone));

         const auto vertices = etherblocks::engine::buildWorldMesh(world, etherblocks::game::BlockType::VoidCore);

         expect(eq(vertices.size(), std::size_t{30}));
      };

      "[RegionMeshOnlyIncludesRequestedRange]"_test = [] {
         etherblocks::game::World world{{4, 2, 2}};
         clearWorld(world);
         static_cast<void>(world.setBlock({0, 0, 0}, etherblocks::game::BlockType::VoidCore));
         static_cast<void>(world.setBlock({3, 0, 0}, etherblocks::game::BlockType::VoidCore));

         const auto vertices =
             etherblocks::engine::buildWorldMesh(world, etherblocks::game::BlockType::VoidCore, {3, 0, 0}, {4, 1, 1});

         expect(eq(vertices.size(), std::size_t{36}));
      };

      "[SelectionBoxBuildsTwelveEdgesAroundVoxel]"_test = [] {
         const auto vertices = etherblocks::engine::buildSelectionBox({2, 3, 4});

         expect(eq(vertices.size(), std::size_t{24}));
         expect(containsPosition(vertices, {1.495f, 2.495f, 3.495f}));
         expect(containsPosition(vertices, {2.505f, 3.505f, 4.505f}));
      };
   };
} // namespace
