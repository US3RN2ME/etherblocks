#include <cmath>
#include <etherblocks/game/Raycast.hpp>
#include <limits>

namespace etherblocks::game {

   std::optional<RayHit> castVoxelRay(glm::vec3 origin, glm::vec3 direction, float maxDistance, const SolidQuery& isSolid) {
      if (glm::length(direction) == 0.0f) {
         return {};
      }

      direction = glm::normalize(direction);
      origin += glm::vec3(0.5f);

      glm::ivec3 voxel = glm::floor(origin);
      glm::ivec3 normal{};
      const glm::ivec3 step{direction.x > 0.0f ? 1 : -1, direction.y > 0.0f ? 1 : -1, direction.z > 0.0f ? 1 : -1};

      constexpr auto infinity = std::numeric_limits<float>::infinity();
      const glm::vec3 deltaDistance{direction.x != 0.0f ? std::abs(1.0f / direction.x) : infinity,
                                    direction.y != 0.0f ? std::abs(1.0f / direction.y) : infinity,
                                    direction.z != 0.0f ? std::abs(1.0f / direction.z) : infinity};
      const glm::vec3 nextBoundary{step.x > 0 ? static_cast<float>(voxel.x + 1) : static_cast<float>(voxel.x),
                                   step.y > 0 ? static_cast<float>(voxel.y + 1) : static_cast<float>(voxel.y),
                                   step.z > 0 ? static_cast<float>(voxel.z + 1) : static_cast<float>(voxel.z)};
      glm::vec3 sideDistance{direction.x != 0.0f ? (nextBoundary.x - origin.x) / direction.x : infinity,
                             direction.y != 0.0f ? (nextBoundary.y - origin.y) / direction.y : infinity,
                             direction.z != 0.0f ? (nextBoundary.z - origin.z) / direction.z : infinity};

      float distance = 0.0f;
      while (distance <= maxDistance) {
         if (isSolid(voxel)) {
            return RayHit{voxel, normal, distance};
         }

         if (sideDistance.x < sideDistance.y && sideDistance.x < sideDistance.z) {
            voxel.x += step.x;
            distance = sideDistance.x;
            sideDistance.x += deltaDistance.x;
            normal = {-step.x, 0, 0};
         } else if (sideDistance.y < sideDistance.z) {
            voxel.y += step.y;
            distance = sideDistance.y;
            sideDistance.y += deltaDistance.y;
            normal = {0, -step.y, 0};
         } else {
            voxel.z += step.z;
            distance = sideDistance.z;
            sideDistance.z += deltaDistance.z;
            normal = {0, 0, -step.z};
         }
      }
      return {};
   }

} // namespace etherblocks::game
