#include <etherblocks/game/Raycast.hpp>
#include <glm/glm.hpp>

#include "ut_main.hpp"

namespace {
   suite<"[Raycast]"> _ = [] {
      "[ReturnsEmptyForZeroDirection]"_test = [] {
         const auto hit = etherblocks::game::castVoxelRay({0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, 8.0f, [](glm::ivec3) {
            return true;
         });

         expect(!hit.has_value());
      };

      "[HitsFirstSolidVoxelAndReportsFaceNormal]"_test = [] {
         const auto hit =
             etherblocks::game::castVoxelRay({0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, 8.0f, [](glm::ivec3 position) {
                return position == glm::ivec3{2, 0, 0};
             });

         expect(hit.has_value());
         expect(eq(hit->voxel.x, 2_i));
         expect(eq(hit->voxel.y, 0_i));
         expect(eq(hit->voxel.z, 0_i));
         expect(eq(hit->normal.x, -1_i));
         expect(eq(hit->normal.y, 0_i));
         expect(eq(hit->normal.z, 0_i));
         expect(hit->distance > 0.0_f);
      };

      "[ReturnsCurrentVoxelWhenOriginStartsInsideSolid]"_test = [] {
         const auto hit =
             etherblocks::game::castVoxelRay({2.0f, 3.0f, 4.0f}, {1.0f, 0.0f, 0.0f}, 8.0f, [](glm::ivec3 position) {
                return position == glm::ivec3{2, 3, 4};
             });

         expect(hit.has_value());
         expect(eq(hit->voxel.x, 2_i));
         expect(eq(hit->voxel.y, 3_i));
         expect(eq(hit->voxel.z, 4_i));
         expect(eq(hit->normal.x, 0_i));
         expect(eq(hit->normal.y, 0_i));
         expect(eq(hit->normal.z, 0_i));
         expect(eq(hit->distance, 0.0_f));
      };

      "[ReportsPositiveNormalWhenHitFromPositiveX]"_test = [] {
         const auto hit =
             etherblocks::game::castVoxelRay({3.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, 8.0f, [](glm::ivec3 position) {
                return position == glm::ivec3{1, 0, 0};
             });

         expect(hit.has_value());
         expect(eq(hit->voxel.x, 1_i));
         expect(eq(hit->normal.x, 1_i));
         expect(eq(hit->normal.y, 0_i));
         expect(eq(hit->normal.z, 0_i));
      };

      "[StepsAlongVerticalAxis]"_test = [] {
         const auto hit =
             etherblocks::game::castVoxelRay({0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, 8.0f, [](glm::ivec3 position) {
                return position == glm::ivec3{0, 2, 0};
             });

         expect(hit.has_value());
         expect(eq(hit->voxel.x, 0_i));
         expect(eq(hit->voxel.y, 2_i));
         expect(eq(hit->voxel.z, 0_i));
         expect(eq(hit->normal.x, 0_i));
         expect(eq(hit->normal.y, -1_i));
         expect(eq(hit->normal.z, 0_i));
      };

      "[RespectsMaximumDistance]"_test = [] {
         const auto hit =
             etherblocks::game::castVoxelRay({0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, 0.25f, [](glm::ivec3 position) {
                return position == glm::ivec3{2, 0, 0};
             });

         expect(!hit.has_value());
      };
   };
} // namespace
