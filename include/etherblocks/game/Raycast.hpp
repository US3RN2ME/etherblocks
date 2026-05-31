#ifndef ETHERBLOCKS_GAME_RAYCAST_HPP
#define ETHERBLOCKS_GAME_RAYCAST_HPP

#include <functional>
#include <glm/glm.hpp>
#include <optional>

namespace etherblocks::game {

   struct RayHit {
      glm::ivec3 voxel;
      glm::ivec3 normal;
      float distance{};
   };

   using SolidQuery = std::function<bool(glm::ivec3)>;

   [[nodiscard]] std::optional<RayHit> castVoxelRay(glm::vec3 origin, glm::vec3 direction, float maxDistance,
                                                    const SolidQuery& isSolid);

} // namespace etherblocks::game

#endif // ETHERBLOCKS_GAME_RAYCAST_HPP
