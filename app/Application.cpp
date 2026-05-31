#include "Application.hpp"

#include <etherblocks/engine/graphics/Material.hpp>
#include <etherblocks/engine/graphics/Mesh.hpp>
#include <etherblocks/engine/graphics/Renderer.hpp>
#include <etherblocks/engine/graphics/Texture.hpp>
#include <etherblocks/game/Player.hpp>
#include <etherblocks/game/Raycast.hpp>
#include <etherblocks/game/World.hpp>
#include <etherblocks/system/FrameClock.hpp>
#include <etherblocks/system/Input.hpp>
#include <etherblocks/system/Window.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <optional>
#include <span>
#include <variant>

#include "SceneRendering.hpp"

namespace etherblocks::app {

   namespace {
      namespace graphics = engine::graphics;
      namespace sys = system;

      constexpr auto kWindowSize = glm::ivec2{800, 600};
      constexpr auto kInteractionRange = 8.0f;

      template <typename... Ts>
      struct Overloaded : Ts... {
         using Ts::operator()...;
      };

      template <typename... Ts>
      Overloaded(Ts...) -> Overloaded<Ts...>;
   } // namespace

   class Application::Impl {
   public:
      Impl()
          : window_({kWindowSize, "etherblocks"})
          , player_({0.0f, 3.0f, 3.0f})
          , worldMesh_(sizeof(MeshVertex), meshLayout())
          , selectionMesh_(sizeof(MeshVertex), meshLayout())
          , worldMaterial_("./shaders/basic.vertex.glsl", "./shaders/basic.fragment.glsl")
          , worldTexture_("./assets/container.png") {
         renderer_.setViewport(window_.framebufferSize());
         renderer_.enable(graphics::RenderFeature::DepthTest);
         renderer_.enable(graphics::RenderFeature::ProgramPointSize);
         window_.setCursorMode(sys::CursorMode::Disabled);
         window_.setRawMouseMotion(true);

         worldMaterial_.setTexture(worldTexture_);
         worldMaterial_.shader().set("uTexture", 0);
         worldMaterial_.shader().set("uModel", glm::mat4{1.0f});
         rebuildWorldMesh();
      }

      void run() {
         while (window_.isOpen()) {
            const auto deltaTime = clock_.tick(sys::Window::elapsedTime());
            processEvents();
            update(deltaTime);
            render();
            window_.display();
         }
      }

   private:
      void processEvents() {
         window_.pollEvents();
         while (const auto event = window_.nextEvent()) {
            std::visit(Overloaded{
                           [this](const sys::event::Closed&) {
                              window_.close();
                           },
                           [this](const sys::event::Resized& resized) {
                              screenSize_ = resized.size;
                              renderer_.setViewport(resized.size);
                           },
                           [](const auto&) {},
                       },
                       *event);
         }
      }

      void update(float deltaTime) {
         const auto& input = window_.input();
         if (input.isKeyPressed(sys::Key::Escape)) {
            window_.close();
         }

         player_.update(input, deltaTime, [this](glm::ivec3 position) {
            return world_.isSolid(position);
         });

         selectedBlock_ = findSelectedBlock();
         editWorld(input);
         if (worldMeshDirty_) {
            rebuildWorldMesh();
         }
      }

      [[nodiscard]] std::optional<game::RayHit> findSelectedBlock() const {
         return game::castVoxelRay(player_.camera().position(), player_.camera().front(), kInteractionRange,
                                   [this](glm::ivec3 position) {
                                      return world_.isSolid(position);
                                   });
      }

      void editWorld(const sys::Input& input) {
         if (!selectedBlock_) {
            return;
         }

         if (input.isMouseButtonPressed(sys::MouseButton::Left)) {
            const auto position = selectedBlock_->voxel + selectedBlock_->normal;
            if (!player_.occupiesVoxel(position)) {
               worldMeshDirty_ |= world_.setBlock(position, game::BlockType::Basic);
            }
         }
         if (input.isMouseButtonPressed(sys::MouseButton::Right)) {
            worldMeshDirty_ |= world_.setBlock(selectedBlock_->voxel, game::BlockType::Empty);
         }
         if (worldMeshDirty_) {
            selectedBlock_ = findSelectedBlock();
         }
      }

      void rebuildWorldMesh() {
         const auto vertices = buildWorldMesh(world_);
         worldMesh_.upload(std::span{vertices}, graphics::BufferUsage::DynamicDraw);
         worldMeshDirty_ = false;
      }

      void render() {
         renderer_.clear({0.2f, 0.3f, 0.3f, 1.0f}, graphics::ClearBuffer::Color | graphics::ClearBuffer::Depth);

         worldMaterial_.shader().set("uView", player_.camera().createViewMatrix());
         worldMaterial_.shader().set("uProjection", createProjectionMatrix());
         worldMaterial_.shader().set("uUseOverrideColor", false);
         renderer_.draw(worldMesh_, worldMaterial_);

         if (selectedBlock_) {
            drawSelectionBox(selectedBlock_->voxel);
         }

         renderer_.disable(graphics::RenderFeature::DepthTest);
         crosshair_.draw(renderer_);
         renderer_.enable(graphics::RenderFeature::DepthTest);
      }

      [[nodiscard]] glm::mat4 createProjectionMatrix() const {
         const auto aspectRatio =
             screenSize_.y > 0 ? static_cast<float>(screenSize_.x) / static_cast<float>(screenSize_.y) : 1.0f;
         return glm::perspective(glm::radians(player_.camera().zoom()), aspectRatio, 0.1f, 100.0f);
      }

      void drawSelectionBox(glm::ivec3 position) {
         const auto vertices = buildSelectionBox(position);
         selectionMesh_.upload(std::span<const MeshVertex>{vertices}, graphics::BufferUsage::DynamicDraw);
         worldMaterial_.shader().set("uUseOverrideColor", true);
         worldMaterial_.shader().set("uOverrideColor", glm::vec4{1.0f});
         renderer_.setLineWidth(2.0f);
         renderer_.draw(selectionMesh_, worldMaterial_, graphics::Primitive::Lines);
      }

      sys::Window window_;
      graphics::Renderer renderer_;
      game::Player player_;
      sys::FrameClock clock_;
      glm::ivec2 screenSize_{kWindowSize};
      game::World world_;
      graphics::Mesh worldMesh_;
      graphics::Mesh selectionMesh_;
      graphics::Material worldMaterial_;
      graphics::Texture worldTexture_;
      CrosshairRenderer crosshair_;
      std::optional<game::RayHit> selectedBlock_;
      bool worldMeshDirty_{true};
   };

   Application::Application()
       : impl_(std::make_unique<Impl>()) {}

   Application::~Application() = default;

   void Application::run() {
      impl_->run();
   }

} // namespace etherblocks::app
