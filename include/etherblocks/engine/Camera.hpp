#ifndef ETHERBLOCKS_ENGINE_CAMERA_HPP
#define ETHERBLOCKS_ENGINE_CAMERA_HPP

#include <glm/glm.hpp>

namespace etherblocks::engine {

   /**
    * @brief Cardinal movement commands understood by camera implementations.
    */
   enum class CameraMovement { Forward, Backward, Left, Right };

   /**
    * @brief Tunable camera movement and look constraints.
    */
   struct CameraSettings {
      /**
       * @brief Movement speed in world units per second.
       */
      float movementSpeed{2.5f};

      /**
       * @brief Mouse-look sensitivity multiplier.
       */
      float mouseSensitivity{0.1f};

      /**
       * @brief Current vertical field of view in degrees.
       */
      float zoom{45.0f};

      /**
       * @brief Minimum vertical field of view in degrees.
       */
      float minZoom{1.0f};

      /**
       * @brief Maximum vertical field of view in degrees.
       */
      float maxZoom{45.0f};

      /**
       * @brief Minimum pitch in degrees.
       */
      float minPitch{-89.0f};

      /**
       * @brief Maximum pitch in degrees.
       */
      float maxPitch{89.0f};
   };

   /**
    * @brief Shared camera orientation, projection-control, and view-matrix logic.
    */
   class CameraBase {
   public:
      /**
       * @brief Default yaw in degrees.
       */
      static constexpr auto kDefaultYaw = -90.0f;

      /**
       * @brief Default pitch in degrees.
       */
      static constexpr auto kDefaultPitch = 0.0f;

      /**
       * @brief Default movement speed.
       */
      static constexpr auto kDefaultSpeed = 2.5f;

      /**
       * @brief Default mouse sensitivity.
       */
      static constexpr auto kDefaultSensitivity = 0.1f;

      /**
       * @brief Default zoom in degrees.
       */
      static constexpr auto kDefaultZoom = 45.0f;

      /**
       * @brief Minimum allowed zoom in degrees.
       */
      static constexpr auto kMinZoom = 1.0f;

      /**
       * @brief Maximum allowed zoom in degrees.
       */
      static constexpr auto kMaxZoom = 45.0f;

      /**
       * @brief Minimum allowed pitch in degrees.
       */
      static constexpr auto kMinPitch = -89.0f;

      /**
       * @brief Maximum allowed pitch in degrees.
       */
      static constexpr auto kMaxPitch = 89.0f;

      /**
       * @brief Default camera position.
       */
      static constexpr auto kDefaultPosition = glm::vec3(0.0f, 0.0f, 0.0f);

      /**
       * @brief Default world-up direction.
       */
      static constexpr auto kDefaultWorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

      /**
       * @brief Construct a camera from pose and tuning settings.
       *
       * @param position Initial camera position.
       * @param worldUp World-space up direction.
       * @param yaw Initial yaw in degrees.
       * @param pitch Initial pitch in degrees.
       * @param settings Camera tuning values.
       */
      explicit CameraBase(const glm::vec3& position = kDefaultPosition, const glm::vec3& worldUp = kDefaultWorldUp,
                          float yaw = kDefaultYaw, float pitch = kDefaultPitch, CameraSettings settings = {});

      /**
       * @brief Destroy the camera.
       */
      virtual ~CameraBase();

      /**
       * @brief Copy construct a camera.
       *
       * @param other Camera to copy.
       */
      CameraBase(const CameraBase& other);

      /**
       * @brief Move construct a camera.
       *
       * @param other Camera to move from.
       */
      CameraBase(CameraBase&& other) noexcept;

      /**
       * @brief Copy assign a camera.
       *
       * @param other Camera to copy.
       *
       * @return This camera.
       */
      CameraBase& operator=(const CameraBase& other);

      /**
       * @brief Move assign a camera.
       *
       * @param other Camera to move from.
       *
       * @return This camera.
       */
      CameraBase& operator=(CameraBase&& other) noexcept;

      /**
       * @brief Move the camera according to an implementation-specific movement command.
       *
       * @param movement Movement direction.
       * @param deltaTime Frame delta time in seconds.
       */
      virtual void move(CameraMovement movement, float deltaTime) = 0;

      /**
       * @brief Build a view matrix from the current camera pose.
       *
       * @return View matrix.
       */
      [[nodiscard]] glm::mat4 createViewMatrix() const;

      /**
       * @brief Rotate the camera by mouse movement offsets.
       *
       * @param xOffset Horizontal mouse movement.
       * @param yOffset Vertical mouse movement.
       * @param constrainPitch True to clamp pitch to configured limits.
       */
      void rotateByMouseOffset(float xOffset, float yOffset, bool constrainPitch = true);

      /**
       * @brief Adjust camera zoom from scroll movement.
       *
       * @param yOffset Scroll offset.
       */
      void zoomByScrollOffset(float yOffset);

      /**
       * @brief Return the camera position.
       *
       * @return World-space position.
       */
      [[nodiscard]] const glm::vec3& position() const noexcept;

      /**
       * @brief Return the camera forward direction.
       *
       * @return Normalized forward vector.
       */
      [[nodiscard]] const glm::vec3& front() const noexcept;

      /**
       * @brief Return the camera up direction.
       *
       * @return Normalized up vector.
       */
      [[nodiscard]] const glm::vec3& up() const noexcept;

      /**
       * @brief Return the camera right direction.
       *
       * @return Normalized right vector.
       */
      [[nodiscard]] const glm::vec3& right() const noexcept;

      /**
       * @brief Return the configured world-up direction.
       *
       * @return Normalized world-up vector.
       */
      [[nodiscard]] const glm::vec3& worldUp() const noexcept;

      /**
       * @brief Return the yaw angle.
       *
       * @return Yaw in degrees.
       */
      [[nodiscard]] float yaw() const noexcept;

      /**
       * @brief Return the pitch angle.
       *
       * @return Pitch in degrees.
       */
      [[nodiscard]] float pitch() const noexcept;

      /**
       * @brief Return movement speed.
       *
       * @return Movement speed in world units per second.
       */
      [[nodiscard]] float movementSpeed() const noexcept;

      /**
       * @brief Return mouse sensitivity.
       *
       * @return Mouse sensitivity multiplier.
       */
      [[nodiscard]] float mouseSensitivity() const noexcept;

      /**
       * @brief Return current zoom.
       *
       * @return Vertical field of view in degrees.
       */
      [[nodiscard]] float zoom() const noexcept;

      /**
       * @brief Return camera settings.
       *
       * @return Camera settings.
       */
      [[nodiscard]] const CameraSettings& settings() const noexcept;

      /**
       * @brief Set the camera position.
       *
       * @param position New world-space position.
       */
      void setPosition(const glm::vec3& position) noexcept;

      /**
       * @brief Set the world-up direction.
       *
       * @param worldUp New world-up direction.
       */
      void setWorldUp(const glm::vec3& worldUp);

      /**
       * @brief Set yaw.
       *
       * @param yaw New yaw in degrees.
       */
      void setYaw(float yaw);

      /**
       * @brief Set pitch.
       *
       * @param pitch New pitch in degrees.
       * @param constrainPitch True to clamp pitch to configured limits.
       */
      void setPitch(float pitch, bool constrainPitch = true);

      /**
       * @brief Set movement speed.
       *
       * @param movementSpeed New movement speed.
       */
      void setMovementSpeed(float movementSpeed) noexcept;

      /**
       * @brief Set mouse sensitivity.
       *
       * @param mouseSensitivity New mouse sensitivity.
       */
      void setMouseSensitivity(float mouseSensitivity) noexcept;

      /**
       * @brief Set zoom.
       *
       * @param zoom New vertical field of view in degrees.
       */
      void setZoom(float zoom) noexcept;

      /**
       * @brief Replace all camera settings.
       *
       * @param settings New settings.
       */
      void setSettings(CameraSettings settings);

   protected:
      /**
       * @brief Move the camera along a world-space direction.
       *
       * @param direction Movement direction.
       * @param deltaTime Frame delta time in seconds.
       */
      void moveAlong(const glm::vec3& direction, float deltaTime);

      /**
       * @brief Project a direction onto the horizontal movement plane.
       *
       * @param direction Direction to project.
       *
       * @return Normalized horizontal direction.
       */
      [[nodiscard]] static glm::vec3 horizontalDirection(const glm::vec3& direction);

   private:
      void updateDirectionVectors();
      [[nodiscard]] float clampPitch(float pitch) const noexcept;
      static CameraSettings sanitizeSettings(CameraSettings settings) noexcept;
      static glm::vec3 normalizeOrFallback(const glm::vec3& value, const glm::vec3& fallback);

      glm::vec3 position_{kDefaultPosition};
      glm::vec3 front_{0.0f, 0.0f, -1.0f};
      glm::vec3 up_{0.0f, 1.0f, 0.0f};
      glm::vec3 right_{1.0f, 0.0f, 0.0f};
      glm::vec3 worldUp_{kDefaultWorldUp};
      float yaw_{kDefaultYaw};
      float pitch_{kDefaultPitch};
      CameraSettings settings_{};
   };

   /**
    * @brief Free-flying camera that moves along the full orientation basis.
    */
   class FreeCamera final : public CameraBase {
   public:
      using CameraBase::CameraBase;

      /**
       * @brief Move the camera in free-flight mode.
       *
       * @param movement Movement direction.
       * @param deltaTime Frame delta time in seconds.
       */
      void move(CameraMovement movement, float deltaTime) override;
   };

   /**
    * @brief First-person camera constrained to horizontal movement commands.
    */
   class FpsCamera final : public CameraBase {
   public:
      using CameraBase::CameraBase;

      /**
       * @brief Move the camera in first-person mode.
       *
       * @param movement Movement direction.
       * @param deltaTime Frame delta time in seconds.
       */
      void move(CameraMovement movement, float deltaTime) override;

      /**
       * @brief Move the camera by a local-space vector.
       *
       * @param localDirection Local movement direction.
       * @param deltaTime Frame delta time in seconds.
       */
      void move(const glm::vec3& localDirection, float deltaTime);
   };

} // namespace etherblocks::engine

#endif // ETHERBLOCKS_ENGINE_CAMERA_HPP
