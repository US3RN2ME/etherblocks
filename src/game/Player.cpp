#include <algorithm>
#include <cmath>
#include <etherblocks/game/Player.hpp>
#include <etherblocks/system/Input.hpp>
#include <etherblocks/system/Logger.hpp>

namespace etherblocks::game {

   namespace {
      constexpr auto kGroundProbeDistance = 0.01f;
      constexpr auto kCollisionEpsilon = 0.0001f;
      constexpr auto kMaxCollisionStep = 0.25f;

      glm::vec3 normalizeOrZero(const glm::vec3& value) {
         const auto length = glm::length(value);
         return length > kCollisionEpsilon ? value / length : glm::vec3{};
      }

      glm::vec3 horizontalDirection(const glm::vec3& value) {
         return normalizeOrZero({value.x, 0.0f, value.z});
      }
   } // namespace

   Player::Player(const glm::vec3& cameraPosition, PlayerSettings settings)
       : camera_(cameraPosition)
       , settings_(settings)
       , position_(cameraPosition - glm::vec3{0.0f, settings.eyeHeight, 0.0f}) {
      settings_.creativeSpeed = std::max(0.0f, settings_.creativeSpeed);
      settings_.survivalSpeed = std::max(0.0f, settings_.survivalSpeed);
      settings_.sprintMultiplier = std::max(0.0f, settings_.sprintMultiplier);
      settings_.jumpSpeed = std::max(0.0f, settings_.jumpSpeed);
      settings_.height = std::max(0.0f, settings_.height);
      settings_.eyeHeight = std::clamp(settings_.eyeHeight, 0.0f, settings_.height);
      settings_.radius = std::max(0.0f, settings_.radius);
      updateCameraPosition();
   }

   void Player::update(const system::Input& input, float deltaTime, const CollisionQuery& isSolid) {
      const auto mouseDelta = glm::vec2(input.mouseDelta());
      camera_.rotateByMouseOffset(mouseDelta.x, -mouseDelta.y);

      if (input.isKeyPressed(system::Key::F) && (mode_ == PlayerMode::Survival || !collidesAt(position_, isSolid))) {
         toggleMode();
      }

      if (mode_ == PlayerMode::Creative) {
         updateCreative(input, deltaTime);
      } else {
         updateSurvival(input, deltaTime, isSolid);
      }
   }

   const engine::FpsCamera& Player::camera() const noexcept {
      return camera_;
   }

   const glm::vec3& Player::position() const noexcept {
      return position_;
   }

   PlayerMode Player::mode() const noexcept {
      return mode_;
   }

   bool Player::occupiesVoxel(glm::ivec3 voxel) const noexcept {
      const auto min = position_ + glm::vec3{-settings_.radius, kCollisionEpsilon, -settings_.radius};
      const auto max = position_ + glm::vec3{settings_.radius, settings_.height - kCollisionEpsilon, settings_.radius};
      const auto voxelMin = glm::vec3(voxel) - glm::vec3{0.5f};
      const auto voxelMax = glm::vec3(voxel) + glm::vec3{0.5f};
      return min.x < voxelMax.x && max.x > voxelMin.x && min.y < voxelMax.y && max.y > voxelMin.y && min.z < voxelMax.z &&
             max.z > voxelMin.z;
   }

   void Player::setMode(PlayerMode mode) {
      mode_ = mode;
      velocity_ = {};
      system::logger().log(system::LogLevel::Info,
                           mode_ == PlayerMode::Creative ? "Player mode: creative" : "Player mode: survival");
   }

   void Player::toggleMode() {
      setMode(mode_ == PlayerMode::Creative ? PlayerMode::Survival : PlayerMode::Creative);
   }

   void Player::updateCreative(const system::Input& input, float deltaTime) {
      auto movement = movementInput(input);
      movement.y += input.isKeyHeld(system::Key::Space) ? 1.0f : 0.0f;
      movement.y -= input.isKeyHeld(system::Key::LeftShift) ? 1.0f : 0.0f;

      const auto speedMultiplier = input.isKeyHeld(system::Key::LeftCtrl) ? settings_.sprintMultiplier : 1.0f;
      camera_.setMovementSpeed(settings_.creativeSpeed * speedMultiplier);
      camera_.move(movement, deltaTime);
      position_ = camera_.position() - glm::vec3{0.0f, settings_.eyeHeight, 0.0f};
   }

   void Player::updateSurvival(const system::Input& input, float deltaTime, const CollisionQuery& isSolid) {
      const auto speedMultiplier = input.isKeyHeld(system::Key::LeftCtrl) ? settings_.sprintMultiplier : 1.0f;
      const auto movement = movementInput(input);
      const auto horizontalOffset = normalizeOrZero(horizontalDirection(camera_.right()) * movement.x +
                                                    horizontalDirection(camera_.front()) * movement.z) *
                                    settings_.survivalSpeed * speedMultiplier * deltaTime;

      const auto grounded = isGrounded(isSolid);
      if (grounded && input.isKeyPressed(system::Key::Space)) {
         velocity_.y = settings_.jumpSpeed;
      }
      velocity_.y += settings_.gravity * deltaTime;

      moveWithCollisions({horizontalOffset.x, velocity_.y * deltaTime, horizontalOffset.z}, isSolid);
      updateCameraPosition();
   }

   void Player::moveWithCollisions(glm::vec3 offset, const CollisionQuery& isSolid) {
      const auto maxOffset = std::max({std::abs(offset.x), std::abs(offset.y), std::abs(offset.z)});
      const auto stepCount = std::max(1, static_cast<int>(std::ceil(maxOffset / kMaxCollisionStep)));
      offset /= static_cast<float>(stepCount);

      for (auto step = 0; step < stepCount; ++step) {
         for (auto axis = 0; axis < 3; ++axis) {
            auto candidate = position_;
            candidate[axis] += offset[axis];
            if (!collidesAt(candidate, isSolid)) {
               position_ = candidate;
            } else if (axis == 1) {
               velocity_.y = 0.0f;
            }
         }
      }
   }

   bool Player::collidesAt(const glm::vec3& position, const CollisionQuery& isSolid) const {
      const auto min = position + glm::vec3{-settings_.radius, kCollisionEpsilon, -settings_.radius};
      const auto max = position + glm::vec3{settings_.radius, settings_.height - kCollisionEpsilon, settings_.radius};
      const auto minVoxel = glm::ivec3(glm::floor(min + glm::vec3{0.5f}));
      const auto maxVoxel = glm::ivec3(glm::floor(max + glm::vec3{0.5f}));

      for (auto y = minVoxel.y; y <= maxVoxel.y; ++y) {
         for (auto z = minVoxel.z; z <= maxVoxel.z; ++z) {
            for (auto x = minVoxel.x; x <= maxVoxel.x; ++x) {
               if (isSolid({x, y, z})) {
                  return true;
               }
            }
         }
      }
      return false;
   }

   bool Player::isGrounded(const CollisionQuery& isSolid) const {
      return collidesAt(position_ - glm::vec3{0.0f, kGroundProbeDistance, 0.0f}, isSolid);
   }

   glm::vec3 Player::movementInput(const system::Input& input) const noexcept {
      glm::vec3 movement{};
      movement.z += input.isKeyHeld(system::Key::W) ? 1.0f : 0.0f;
      movement.z -= input.isKeyHeld(system::Key::S) ? 1.0f : 0.0f;
      movement.x -= input.isKeyHeld(system::Key::A) ? 1.0f : 0.0f;
      movement.x += input.isKeyHeld(system::Key::D) ? 1.0f : 0.0f;
      return movement;
   }

   void Player::updateCameraPosition() noexcept {
      camera_.setPosition(position_ + glm::vec3{0.0f, settings_.eyeHeight, 0.0f});
   }

} // namespace etherblocks::game
