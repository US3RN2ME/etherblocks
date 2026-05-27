#include <algorithm>
#include <cmath>
#include <etherblocks/Camera.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace etherblocks {

   CameraBase::CameraBase(const glm::vec3& position, const glm::vec3& worldUp, float yaw, float pitch, CameraSettings settings)
       : position_(position)
       , worldUp_(normalizeOrFallback(worldUp, kDefaultWorldUp))
       , yaw_(yaw)
       , settings_(sanitizeSettings(settings)) {
      pitch_ = clampPitch(pitch);
      updateDirectionVectors();
   }

   CameraBase::~CameraBase() = default;

   CameraBase::CameraBase(const CameraBase&) = default;

   CameraBase::CameraBase(CameraBase&&) noexcept = default;

   CameraBase& CameraBase::operator=(const CameraBase&) = default;

   CameraBase& CameraBase::operator=(CameraBase&&) noexcept = default;

   glm::mat4 CameraBase::createViewMatrix() const {
      return glm::lookAt(position_, position_ + front_, up_);
   }

   void CameraBase::rotateByMouseOffset(float xOffset, float yOffset, bool constrainPitch) {
      yaw_ += xOffset * settings_.mouseSensitivity;
      pitch_ += yOffset * settings_.mouseSensitivity;

      if (constrainPitch) {
         pitch_ = clampPitch(pitch_);
      }

      updateDirectionVectors();
   }

   void CameraBase::zoomByScrollOffset(float yOffset) {
      settings_.zoom = std::clamp(settings_.zoom - yOffset, settings_.minZoom, settings_.maxZoom);
   }

   const glm::vec3& CameraBase::position() const noexcept {
      return position_;
   }

   const glm::vec3& CameraBase::front() const noexcept {
      return front_;
   }

   const glm::vec3& CameraBase::up() const noexcept {
      return up_;
   }

   const glm::vec3& CameraBase::right() const noexcept {
      return right_;
   }

   const glm::vec3& CameraBase::worldUp() const noexcept {
      return worldUp_;
   }

   float CameraBase::yaw() const noexcept {
      return yaw_;
   }

   float CameraBase::pitch() const noexcept {
      return pitch_;
   }

   float CameraBase::movementSpeed() const noexcept {
      return settings_.movementSpeed;
   }

   float CameraBase::mouseSensitivity() const noexcept {
      return settings_.mouseSensitivity;
   }

   float CameraBase::zoom() const noexcept {
      return settings_.zoom;
   }

   const CameraSettings& CameraBase::settings() const noexcept {
      return settings_;
   }

   void CameraBase::setPosition(const glm::vec3& position) noexcept {
      position_ = position;
   }

   void CameraBase::setWorldUp(const glm::vec3& worldUp) {
      worldUp_ = normalizeOrFallback(worldUp, worldUp_);
      updateDirectionVectors();
   }

   void CameraBase::setYaw(float yaw) {
      yaw_ = yaw;
      updateDirectionVectors();
   }

   void CameraBase::setPitch(float pitch, bool constrainPitch) {
      pitch_ = constrainPitch ? clampPitch(pitch) : pitch;
      updateDirectionVectors();
   }

   void CameraBase::setMovementSpeed(float movementSpeed) noexcept {
      settings_.movementSpeed = std::max(0.0f, movementSpeed);
   }

   void CameraBase::setMouseSensitivity(float mouseSensitivity) noexcept {
      settings_.mouseSensitivity = std::max(0.0f, mouseSensitivity);
   }

   void CameraBase::setZoom(float zoom) noexcept {
      settings_.zoom = std::clamp(zoom, settings_.minZoom, settings_.maxZoom);
   }

   void CameraBase::setSettings(CameraSettings settings) {
      settings_ = sanitizeSettings(settings);
      pitch_ = clampPitch(pitch_);
      updateDirectionVectors();
   }

   void CameraBase::moveAlong(const glm::vec3& direction, float deltaTime) {
      position_ += normalizeOrFallback(direction, glm::vec3(0.0f)) * settings_.movementSpeed * deltaTime;
   }

   glm::vec3 CameraBase::horizontalDirection(const glm::vec3& direction) {
      return normalizeOrFallback(glm::vec3(direction.x, 0.0f, direction.z), glm::vec3(0.0f));
   }

   void CameraBase::updateDirectionVectors() {
      glm::vec3 front;
      front.x = std::cos(glm::radians(yaw_)) * std::cos(glm::radians(pitch_));
      front.y = std::sin(glm::radians(pitch_));
      front.z = std::sin(glm::radians(yaw_)) * std::cos(glm::radians(pitch_));

      front_ = glm::normalize(front);
      right_ = normalizeOrFallback(glm::cross(front_, worldUp_), glm::vec3(1.0f, 0.0f, 0.0f));
      up_ = normalizeOrFallback(glm::cross(right_, front_), worldUp_);
   }

   float CameraBase::clampPitch(float pitch) const noexcept {
      return std::clamp(pitch, settings_.minPitch, settings_.maxPitch);
   }

   CameraSettings CameraBase::sanitizeSettings(CameraSettings settings) noexcept {
      settings.movementSpeed = std::max(0.0f, settings.movementSpeed);
      settings.mouseSensitivity = std::max(0.0f, settings.mouseSensitivity);

      if (settings.minZoom > settings.maxZoom) {
         std::swap(settings.minZoom, settings.maxZoom);
      }

      if (settings.minPitch > settings.maxPitch) {
         std::swap(settings.minPitch, settings.maxPitch);
      }

      settings.zoom = std::clamp(settings.zoom, settings.minZoom, settings.maxZoom);
      return settings;
   }

   glm::vec3 CameraBase::normalizeOrFallback(const glm::vec3& value, const glm::vec3& fallback) {
      constexpr auto kEpsilon = 0.000001f;
      const auto length = glm::length(value);

      if (length <= kEpsilon) {
         return fallback;
      }

      return value / length;
   }

   void FreeCamera::move(CameraMovement movement, float deltaTime) {
      switch (movement) {
         case CameraMovement::Forward:
            moveAlong(front(), deltaTime);
            break;
         case CameraMovement::Backward:
            moveAlong(-front(), deltaTime);
            break;
         case CameraMovement::Left:
            moveAlong(-right(), deltaTime);
            break;
         case CameraMovement::Right:
            moveAlong(right(), deltaTime);
            break;
      }
   }

   void FpsCamera::move(CameraMovement movement, float deltaTime) {
      switch (movement) {
         case CameraMovement::Forward:
            moveAlong(horizontalDirection(front()), deltaTime);
            break;
         case CameraMovement::Backward:
            moveAlong(-horizontalDirection(front()), deltaTime);
            break;
         case CameraMovement::Left:
            moveAlong(-horizontalDirection(right()), deltaTime);
            break;
         case CameraMovement::Right:
            moveAlong(horizontalDirection(right()), deltaTime);
            break;
      }
   }

} // namespace etherblocks
