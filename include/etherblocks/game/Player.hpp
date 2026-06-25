#ifndef ETHERBLOCKS_GAME_PLAYER_HPP
#define ETHERBLOCKS_GAME_PLAYER_HPP

#include <etherblocks/engine/Camera.hpp>
#include <functional>
#include <glm/glm.hpp>

namespace etherblocks::system {
   class Input;
}

namespace etherblocks::game {

   /**
    * @brief Player movement simulation mode.
    */
   enum class PlayerMode { Creative, Survival };

   /**
    * @brief Tunable player movement and collision dimensions.
    */
   struct PlayerSettings {
      /**
       * @brief Creative-mode movement speed.
       */
      float creativeSpeed{5.0f};

      /**
       * @brief Survival-mode walking speed.
       */
      float survivalSpeed{4.3f};

      /**
       * @brief Sprint speed multiplier.
       */
      float sprintMultiplier{1.5f};

      /**
       * @brief Downward acceleration in survival mode.
       */
      float gravity{-20.0f};

      /**
       * @brief Initial upward velocity when jumping.
       */
      float jumpSpeed{7.0f};

      /**
       * @brief Camera height above the player body origin.
       */
      float eyeHeight{1.62f};

      /**
       * @brief Player collision capsule height.
       */
      float height{1.8f};

      /**
       * @brief Player collision capsule radius.
       */
      float radius{0.3f};
   };

   /**
    * @brief Player controller owning camera pose and movement state.
    */
   class Player {
   public:
      /**
       * @brief Callback used to test collisions against solid voxels.
       *
       * @param position Voxel coordinate to test.
       *
       * @return True when the voxel blocks player movement.
       */
      using CollisionQuery = std::function<bool(glm::ivec3)>;

      /**
       * @brief Construct a player from an initial camera position.
       *
       * @param cameraPosition Initial camera position.
       * @param settings Player movement settings.
       */
      explicit Player(const glm::vec3& cameraPosition = {}, PlayerSettings settings = {});

      /**
       * @brief Update player movement, look, and physics for one frame.
       *
       * @param input Current input state.
       * @param deltaTime Frame delta time in seconds.
       * @param isSolid Collision query.
       */
      void update(const system::Input& input, float deltaTime, const CollisionQuery& isSolid);

      /**
       * @brief Return the player camera.
       *
       * @return First-person camera.
       */
      [[nodiscard]] const engine::FpsCamera& camera() const noexcept;

      /**
       * @brief Return the player body position.
       *
       * @return World-space body position.
       */
      [[nodiscard]] const glm::vec3& position() const noexcept;

      /**
       * @brief Return the active movement mode.
       *
       * @return Player mode.
       */
      [[nodiscard]] PlayerMode mode() const noexcept;

      /**
       * @brief Test whether the player collision body overlaps a voxel.
       *
       * @param voxel Voxel coordinate to test.
       *
       * @return True when the player occupies the voxel.
       */
      [[nodiscard]] bool occupiesVoxel(glm::ivec3 voxel) const noexcept;

      /**
       * @brief Set camera position and look angles.
       *
       * @param cameraPosition New camera position.
       * @param yaw New yaw in degrees.
       * @param pitch New pitch in degrees.
       */
      void setCameraPose(const glm::vec3& cameraPosition, float yaw, float pitch);

      /**
       * @brief Set the movement simulation mode.
       *
       * @param mode New player mode.
       */
      void setMode(PlayerMode mode);

      /**
       * @brief Toggle between creative and survival mode.
       */
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
