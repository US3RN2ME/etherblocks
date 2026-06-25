#ifndef ETHERBLOCKS_GAME_RAYCAST_HPP
#define ETHERBLOCKS_GAME_RAYCAST_HPP

#include <functional>
#include <glm/glm.hpp>
#include <optional>

namespace etherblocks::game {

   /**
    * @brief Result of a voxel ray cast.
    */
   struct RayHit {
      /**
       * @brief Solid voxel reached by the ray.
       */
      glm::ivec3 voxel;

      /**
       * @brief Surface normal of the voxel face crossed by the ray.
       */
      glm::ivec3 normal;

      /**
       * @brief Distance from the ray origin to the hit point.
       */
      float distance{};
   };

   /**
    * @brief Callback used to test whether a voxel position is solid.
    *
    * @param position Voxel coordinate to test.
    *
    * @return True when the voxel should stop the ray.
    */
   using SolidQuery = std::function<bool(glm::ivec3)>;

   /**
    * @brief Cast a ray through a voxel grid and return the first solid voxel.
    *
    * @param origin Ray origin in world space.
    * @param direction Ray direction in world space.
    * @param maxDistance Maximum ray travel distance.
    * @param isSolid Solid-voxel query callback.
    *
    * @return Hit information when the ray reaches a solid voxel.
    */
   [[nodiscard]] std::optional<RayHit> castVoxelRay(glm::vec3 origin, glm::vec3 direction, float maxDistance,
                                                    const SolidQuery& isSolid);

} // namespace etherblocks::game

#endif // ETHERBLOCKS_GAME_RAYCAST_HPP
