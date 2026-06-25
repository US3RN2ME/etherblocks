#include <cmath>
#include <etherblocks/engine/Camera.hpp>
#include <glm/glm.hpp>

#include "ut_main.hpp"

namespace {
   constexpr auto kEpsilon = 0.0001f;

   bool near(float lhs, float rhs) {
      return std::abs(lhs - rhs) < kEpsilon;
   }

   bool near(glm::vec3 lhs, glm::vec3 rhs) {
      return near(lhs.x, rhs.x) && near(lhs.y, rhs.y) && near(lhs.z, rhs.z);
   }

   suite<"[Camera]"> _ = [] {
      "[SanitizesSettingsOnConstruction]"_test = [] {
         etherblocks::engine::CameraSettings settings;
         settings.movementSpeed = -4.0f;
         settings.mouseSensitivity = -2.0f;
         settings.minZoom = 50.0f;
         settings.maxZoom = 10.0f;
         settings.zoom = 100.0f;
         settings.minPitch = 45.0f;
         settings.maxPitch = -45.0f;

         const etherblocks::engine::FpsCamera camera{{},
                                                     etherblocks::engine::CameraBase::kDefaultWorldUp,
                                                     etherblocks::engine::CameraBase::kDefaultYaw,
                                                     90.0f,
                                                     settings};

         expect(eq(camera.movementSpeed(), 0.0_f));
         expect(eq(camera.mouseSensitivity(), 0.0_f));
         expect(eq(camera.zoom(), 50.0_f));
         expect(eq(camera.pitch(), 45.0_f));
         expect(eq(camera.settings().minZoom, 10.0_f));
         expect(eq(camera.settings().maxZoom, 50.0_f));
         expect(eq(camera.settings().minPitch, -45.0_f));
         expect(eq(camera.settings().maxPitch, 45.0_f));
      };

      "[ClampsPitchUnlessExplicitlyUnconstrained]"_test = [] {
         etherblocks::engine::FpsCamera camera;

         camera.setPitch(180.0f);
         expect(eq(camera.pitch(), 89.0_f));

         camera.setPitch(180.0f, false);
         expect(eq(camera.pitch(), 180.0_f));
      };

      "[ClampsZoomFromSetAndScroll]"_test = [] {
         etherblocks::engine::FpsCamera camera;

         camera.setZoom(-100.0f);
         expect(eq(camera.zoom(), 1.0_f));

         camera.setZoom(100.0f);
         expect(eq(camera.zoom(), 45.0_f));

         camera.zoomByScrollOffset(100.0f);
         expect(eq(camera.zoom(), 1.0_f));
      };

      "[FpsMovementStaysOnHorizontalPlane]"_test = [] {
         etherblocks::engine::CameraSettings settings;
         settings.movementSpeed = 2.0f;
         etherblocks::engine::FpsCamera camera{{0.0f, 0.0f, 0.0f},
                                               etherblocks::engine::CameraBase::kDefaultWorldUp,
                                               etherblocks::engine::CameraBase::kDefaultYaw,
                                               80.0f,
                                               settings};

         camera.move(etherblocks::engine::CameraMovement::Forward, 0.5f);

         expect(near(camera.position(), {0.0f, 0.0f, -1.0f}));
      };

      "[FreeCameraMovementFollowsPitch]"_test = [] {
         etherblocks::engine::CameraSettings settings;
         settings.movementSpeed = 2.0f;
         etherblocks::engine::FreeCamera camera{{0.0f, 0.0f, 0.0f},
                                                etherblocks::engine::CameraBase::kDefaultWorldUp,
                                                etherblocks::engine::CameraBase::kDefaultYaw,
                                                45.0f,
                                                settings};

         camera.move(etherblocks::engine::CameraMovement::Forward, 0.5f);

         expect(camera.position().y > 0.70_f);
         expect(camera.position().z < -0.70_f);
      };

      "[SetWorldUpFallsBackWhenZeroVectorIsProvided]"_test = [] {
         etherblocks::engine::FpsCamera camera;
         const auto previousWorldUp = camera.worldUp();

         camera.setWorldUp({0.0f, 0.0f, 0.0f});

         expect(near(camera.worldUp(), previousWorldUp));
      };
   };
} // namespace
