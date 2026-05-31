#ifndef ETHERBLOCKS_ENGINE_CAMERA_HPP
#define ETHERBLOCKS_ENGINE_CAMERA_HPP

#include <glm/glm.hpp>

namespace etherblocks::engine {

   enum class CameraMovement { Forward, Backward, Left, Right };

   struct CameraSettings {
      float movementSpeed{2.5f};
      float mouseSensitivity{0.1f};
      float zoom{45.0f};
      float minZoom{1.0f};
      float maxZoom{45.0f};
      float minPitch{-89.0f};
      float maxPitch{89.0f};
   };

   class CameraBase {
   public:
      static constexpr auto kDefaultYaw = -90.0f;
      static constexpr auto kDefaultPitch = 0.0f;
      static constexpr auto kDefaultSpeed = 2.5f;
      static constexpr auto kDefaultSensitivity = 0.1f;
      static constexpr auto kDefaultZoom = 45.0f;
      static constexpr auto kMinZoom = 1.0f;
      static constexpr auto kMaxZoom = 45.0f;
      static constexpr auto kMinPitch = -89.0f;
      static constexpr auto kMaxPitch = 89.0f;
      static constexpr auto kDefaultPosition = glm::vec3(0.0f, 0.0f, 0.0f);
      static constexpr auto kDefaultWorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

      explicit CameraBase(const glm::vec3& position = kDefaultPosition, const glm::vec3& worldUp = kDefaultWorldUp,
                          float yaw = kDefaultYaw, float pitch = kDefaultPitch, CameraSettings settings = {});

      virtual ~CameraBase();

      CameraBase(const CameraBase&);
      CameraBase(CameraBase&&) noexcept;
      CameraBase& operator=(const CameraBase&);
      CameraBase& operator=(CameraBase&&) noexcept;

      virtual void move(CameraMovement movement, float deltaTime) = 0;

      [[nodiscard]] glm::mat4 createViewMatrix() const;
      void rotateByMouseOffset(float xOffset, float yOffset, bool constrainPitch = true);
      void zoomByScrollOffset(float yOffset);

      [[nodiscard]] const glm::vec3& position() const noexcept;
      [[nodiscard]] const glm::vec3& front() const noexcept;
      [[nodiscard]] const glm::vec3& up() const noexcept;
      [[nodiscard]] const glm::vec3& right() const noexcept;
      [[nodiscard]] const glm::vec3& worldUp() const noexcept;
      [[nodiscard]] float yaw() const noexcept;
      [[nodiscard]] float pitch() const noexcept;
      [[nodiscard]] float movementSpeed() const noexcept;
      [[nodiscard]] float mouseSensitivity() const noexcept;
      [[nodiscard]] float zoom() const noexcept;
      [[nodiscard]] const CameraSettings& settings() const noexcept;

      void setPosition(const glm::vec3& position) noexcept;
      void setWorldUp(const glm::vec3& worldUp);
      void setYaw(float yaw);
      void setPitch(float pitch, bool constrainPitch = true);
      void setMovementSpeed(float movementSpeed) noexcept;
      void setMouseSensitivity(float mouseSensitivity) noexcept;
      void setZoom(float zoom) noexcept;
      void setSettings(CameraSettings settings);

   protected:
      void moveAlong(const glm::vec3& direction, float deltaTime);
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

   class FreeCamera final : public CameraBase {
   public:
      using CameraBase::CameraBase;

      void move(CameraMovement movement, float deltaTime) override;
   };

   class FpsCamera final : public CameraBase {
   public:
      using CameraBase::CameraBase;

      void move(CameraMovement movement, float deltaTime) override;
      void move(const glm::vec3& localDirection, float deltaTime);
   };

} // namespace etherblocks::engine

#endif // ETHERBLOCKS_ENGINE_CAMERA_HPP
