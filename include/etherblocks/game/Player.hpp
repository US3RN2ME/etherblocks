#ifndef ETHERBLOCKS_GAME_PLAYER_HPP
#define ETHERBLOCKS_GAME_PLAYER_HPP

#include <etherblocks/engine/Camera.hpp>
#include <functional>
#include <glm/glm.hpp>

namespace etherblocks::system {
   class Input;
}

namespace etherblocks::game {

   enum class PlayerMode { Creative, Survival };

   struct PlayerSettings {
      float creativeSpeed{5.0f};
      float survivalSpeed{4.3f};
      float sprintMultiplier{1.5f};
      float gravity{-20.0f};
      float jumpSpeed{7.0f};
      float eyeHeight{1.62f};
      float height{1.8f};
      float radius{0.3f};
   };

   class Player {
   public:
      using CollisionQuery = std::function<bool(glm::ivec3)>;

      explicit Player(const glm::vec3& cameraPosition = {}, PlayerSettings settings = {});

      void update(const system::Input& input, float deltaTime, const CollisionQuery& isSolid);

      [[nodiscard]] const engine::FpsCamera& camera() const noexcept;
      [[nodiscard]] const glm::vec3& position() const noexcept;
      [[nodiscard]] PlayerMode mode() const noexcept;
      [[nodiscard]] bool occupiesVoxel(glm::ivec3 voxel) const noexcept;

      void setCameraPose(const glm::vec3& cameraPosition, float yaw, float pitch);
      void setMode(PlayerMode mode);
      void toggleMode();

   private:
      void updateCreative(const system::Input& input, float deltaTime);
      void updateSurvival(const system::Input& input, float deltaTime, const CollisionQuery& isSolid);
      void moveWithCollisions(glm::vec3 offset, const CollisionQuery& isSolid);
      [[nodiscard]] bool collidesAt(const glm::vec3& position, const CollisionQuery& isSolid) const;
      [[nodiscard]] bool isGrounded(const CollisionQuery& isSolid) const;
      [[nodiscard]] glm::vec3 movementInput(const system::Input& input) const noexcept;
      void updateCameraPosition() noexcept;

      engine::FpsCamera camera_;
      PlayerSettings settings_;
      glm::vec3 position_{};
      glm::vec3 velocity_{};
      PlayerMode mode_{PlayerMode::Creative};
   };

} // namespace etherblocks::game

#endif // ETHERBLOCKS_GAME_PLAYER_HPP
