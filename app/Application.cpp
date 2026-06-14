#include "Application.hpp"

#include <algorithm>
#include <array>
#include <etherblocks/engine/WorldChunks.hpp>
#include <etherblocks/engine/WorldRendering.hpp>
#include <etherblocks/engine/graphics/Material.hpp>
#include <etherblocks/engine/graphics/Mesh.hpp>
#include <etherblocks/engine/graphics/Renderer.hpp>
#include <etherblocks/engine/graphics/Texture.hpp>
#include <etherblocks/game/Player.hpp>
#include <etherblocks/game/Raycast.hpp>
#include <etherblocks/game/World.hpp>
#include <etherblocks/system/FrameClock.hpp>
#include <etherblocks/system/Input.hpp>
#include <etherblocks/system/Logger.hpp>
#include <etherblocks/system/Window.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <variant>
#include <vector>

#include "AppSettings.hpp"
#include "BlockAssets.hpp"
#include "GameMenu.hpp"
#include "WorldPersistence.hpp"

namespace etherblocks::app {

   namespace {
      namespace graphics = engine::graphics;
      namespace sys = system;

      using engine::buildSelectionBox;
      using engine::CrosshairRenderer;
      using engine::meshLayout;
      using engine::MeshVertex;
      using engine::WorldChunks;

      template <typename... Ts>
      struct Overloaded : Ts... {
         using Ts::operator()...;
      };

      template <typename... Ts>
      Overloaded(Ts...) -> Overloaded<Ts...>;

      [[nodiscard]] glm::vec3 initialCameraPosition(const game::World& world) noexcept {
         const auto size = world.size();
         const auto x = size.x / 2;
         const auto z = size.z / 2;
         auto spawnY = 4.0f;
         for (auto y = size.y - 1; y >= 0; --y) {
            if (world.isSolid({x, y, z})) {
               spawnY = static_cast<float>(y) + 3.0f;
               break;
            }
         }
         return {static_cast<float>(x), spawnY, static_cast<float>(z)};
      }
   } // namespace

   class Application::Impl {
   public:
      Impl()
          : settings_(loadAppSettings(kSettingsPath))
          , window_({settings_.window.size, settings_.window.title, settings_.window.vsync, settings_.window.fullscreen})
          , world_(settings_.world.size)
          , player_(initialCameraPosition(world_))
          , worldChunks_(world_, settings_.world.chunkSize)
          , screenSize_(settings_.window.size)
          , selectionMesh_(sizeof(MeshVertex), meshLayout())
          , worldMaterial_("./shaders/basic.vertex.glsl", "./shaders/basic.fragment.glsl")
          , blockTexturePaths_(loadBlockTexturePaths(settings_.assets.blockManifest))
          , voidCoreTexture_(blockTexturePaths_.voidCore)
          , voidCrystalTexture_(blockTexturePaths_.voidCrystal)
          , voidGlassTexture_(blockTexturePaths_.voidGlass)
          , voidBedrockTexture_(blockTexturePaths_.voidBedrock)
          , voidStoneTexture_(blockTexturePaths_.voidStone) {
         refreshWorldList();
         resolutions_ = window_.availableResolutions();
         renderer_.setViewport(window_.framebufferSize());
         renderer_.enable(graphics::RenderFeature::DepthTest);
         renderer_.setAlphaBlending();
         renderer_.enable(graphics::RenderFeature::ProgramPointSize);
         applyInputMode();

         worldMaterial_.shader().set("uTexture", 0);
         worldMaterial_.shader().set("uModel", glm::mat4{1.0f});
         applyRenderingSettings();
         loadWorldFromDisk();
         sys::log(sys::LogLevel::Info, "Application initialized");
      }

      ~Impl() {
         saveActiveWorld();
         sys::log(sys::LogLevel::Info, "Application shutting down");
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
                              requestClose();
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
            setMenuOpen(!menuOpen_);
         }
         if (menuOpen_) {
            return;
         }

         updateHotkeys(input);
         player_.update(input, deltaTime, [this](glm::ivec3 position) {
            return world_.isSolid(position);
         });
         selectedBlock_ = findSelectedBlock();
         editWorld(input);
      }

      void updateHotkeys(const sys::Input& input) {
         if (input.isKeyPressed(sys::Key::F9)) {
            loadWorldFromDisk();
         }
         if (input.isKeyPressed(sys::Key::Num1)) {
            selectedBuildBlock_ = game::BlockType::VoidCore;
         }
         if (input.isKeyPressed(sys::Key::Num2)) {
            selectedBuildBlock_ = game::BlockType::VoidCrystal;
         }
         if (input.isKeyPressed(sys::Key::Num3)) {
            selectedBuildBlock_ = game::BlockType::VoidGlass;
         }
         if (input.isKeyPressed(sys::Key::Num4)) {
            selectedBuildBlock_ = game::BlockType::VoidBedrock;
         }
         if (input.isKeyPressed(sys::Key::Num5)) {
            selectedBuildBlock_ = game::BlockType::VoidStone;
         }
      }

      [[nodiscard]] std::optional<game::RayHit> findSelectedBlock() const {
         return game::castVoxelRay(player_.camera().position(), player_.camera().front(), settings_.player.interactionRange,
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
            if (!player_.occupiesVoxel(position) && world_.setBlock(position, selectedBuildBlock_)) {
               hasUnsavedWorldChanges_ = true;
               worldChunks_.markDirty(position);
               selectedBlock_ = findSelectedBlock();
            }
         }
         if (input.isMouseButtonPressed(sys::MouseButton::Right)) {
            if (world_.setBlock(selectedBlock_->voxel, game::BlockType::Empty)) {
               hasUnsavedWorldChanges_ = true;
               worldChunks_.markDirty(selectedBlock_->voxel);
               selectedBlock_ = findSelectedBlock();
            }
         }
      }

      void render() {
         renderer_.clear(settings_.rendering.clearColor, graphics::ClearBuffer::Color | graphics::ClearBuffer::Depth);

         worldMaterial_.shader().set("uView", player_.camera().createViewMatrix());
         worldMaterial_.shader().set("uProjection", createProjectionMatrix());
         const auto cameraPosition = player_.camera().position();
         worldMaterial_.shader().set("uCameraPosition", glm::vec4{cameraPosition, 1.0f});
         worldMaterial_.shader().set("uUseOverrideColor", false);
         worldChunks_.draw(renderer_, worldMaterial_, blockTextures(), cameraPosition, settings_.rendering.viewDistance);

         if (selectedBlock_) {
            drawSelectionBox(selectedBlock_->voxel);
         }

         renderer_.disable(graphics::RenderFeature::DepthTest);
         if (menuOpen_) {
            renderer_.enable(graphics::RenderFeature::Blending);
            handleMenuResult(
                menu_.draw(renderer_, window_.input(), screenSize_, settings_, worlds_, activeWorldIndex_, resolutions_));
            renderer_.disable(graphics::RenderFeature::Blending);
         } else {
            crosshair_.draw(renderer_);
         }
         renderer_.enable(graphics::RenderFeature::DepthTest);
      }

      [[nodiscard]] std::array<const graphics::Texture*, 5> blockTextures() const noexcept {
         return {&voidCoreTexture_, &voidCrystalTexture_, &voidGlassTexture_, &voidBedrockTexture_, &voidStoneTexture_};
      }

      [[nodiscard]] glm::mat4 createProjectionMatrix() const {
         const auto aspectRatio =
             screenSize_.y > 0 ? static_cast<float>(screenSize_.x) / static_cast<float>(screenSize_.y) : 1.0f;
         return glm::perspective(glm::radians(player_.camera().zoom()), aspectRatio, settings_.rendering.nearPlane,
                                 settings_.rendering.farPlane);
      }

      void drawSelectionBox(glm::ivec3 position) {
         const auto vertices = buildSelectionBox(position);
         selectionMesh_.upload(std::span<const MeshVertex>{vertices}, graphics::BufferUsage::DynamicDraw);
         worldMaterial_.shader().set("uUseOverrideColor", true);
         worldMaterial_.shader().set("uOverrideColor", glm::vec4({1.0f, 0.690196f, 0.0f, 1.0f}));
         renderer_.setLineWidth(2.0f);
         renderer_.draw(selectionMesh_, worldMaterial_, graphics::Primitive::Lines);
      }

      void handleMenuResult(MenuResult result) {
         switch (result.action) {
            case MenuAction::None:
               break;
            case MenuAction::Resume:
               setMenuOpen(false);
               break;
            case MenuAction::LoadWorld:
               loadWorldFromDisk();
               break;
            case MenuAction::CreateWorld:
               createNewWorld();
               break;
            case MenuAction::SelectWorld:
               setActiveWorld(result.worldIndex);
               break;
            case MenuAction::DeleteWorld:
               deleteActiveWorld();
               break;
            case MenuAction::Quit:
               requestClose();
               break;
            case MenuAction::ApplySettings:
               applyRenderingSettings();
               saveSettings();
               break;
            case MenuAction::ApplyWindowSettings:
               applyWindowSettings();
               saveSettings();
               break;
            case MenuAction::ApplyInputSettings:
               applyInputMode();
               saveSettings();
               break;
            case MenuAction::SaveSettings:
               settings_ = validateSettings(std::move(settings_));
               applyWindowSettings();
               applyInputMode();
               applyRenderingSettings();
               saveSettings();
               break;
         }
      }

      void loadWorldFromDisk() {
         if (activeWorldIndex_ < 0 || activeWorldIndex_ >= static_cast<int>(worlds_.size())) {
            return;
         }
         if (loadWorld(world_, worlds_[static_cast<std::size_t>(activeWorldIndex_)].savePath)) {
            if (!loadWorldPlayerState(player_, worlds_[static_cast<std::size_t>(activeWorldIndex_)])) {
               player_ = game::Player(initialCameraPosition(world_));
            }
            hasUnsavedWorldChanges_ = false;
            worldChunks_.markAllDirty();
            selectedBlock_ = findSelectedBlock();
         }
      }

      bool saveActiveWorld(bool force = false) {
         if (activeWorldIndex_ < 0 || activeWorldIndex_ >= static_cast<int>(worlds_.size())) {
            return false;
         }
         const auto& activeWorld = worlds_[static_cast<std::size_t>(activeWorldIndex_)];
         auto savedWorld = true;
         if (force || hasUnsavedWorldChanges_) {
            savedWorld = saveWorld(world_, activeWorld.savePath);
         }
         const auto savedPlayer = saveWorldPlayerState(player_, activeWorld);
         if (!savedWorld) {
            return false;
         }
         hasUnsavedWorldChanges_ = false;
         return savedPlayer;
      }

      void createNewWorld() {
         static_cast<void>(saveActiveWorld());
         auto info = createWorldEntry(settings_.world.saveDirectory);
         worlds_.push_back(info);
         activeWorldIndex_ = static_cast<int>(worlds_.size()) - 1;
         settings_.world.activeWorld = info.id;
         resetGeneratedWorld();
         static_cast<void>(saveAppSettings(settings_, kSettingsPath));
         hasUnsavedWorldChanges_ = true;
         static_cast<void>(saveActiveWorld(true));
      }

      void setActiveWorld(int index) {
         if (index < 0 || index >= static_cast<int>(worlds_.size())) {
            return;
         }
         if (index == activeWorldIndex_) {
            return;
         }
         static_cast<void>(saveActiveWorld());
         activeWorldIndex_ = index;
         settings_.world.activeWorld = worlds_[static_cast<std::size_t>(index)].id;
         static_cast<void>(saveAppSettings(settings_, kSettingsPath));
         resetGeneratedWorld();
         loadWorldFromDisk();
      }

      void refreshWorldList() {
         worlds_ = listWorlds(settings_.world.saveDirectory);
         if (worlds_.empty()) {
            worlds_.push_back(createWorldEntry(settings_.world.saveDirectory));
         }

         activeWorldIndex_ = 0;
         for (auto i = std::size_t{0}; i < worlds_.size(); ++i) {
            if (worlds_[i].id == settings_.world.activeWorld) {
               activeWorldIndex_ = static_cast<int>(i);
               break;
            }
         }
         settings_.world.activeWorld = worlds_[static_cast<std::size_t>(activeWorldIndex_)].id;
      }

      void resetGeneratedWorld() {
         world_ = game::World(settings_.world.size);
         player_ = game::Player(initialCameraPosition(world_));
         worldChunks_.markAllDirty();
         selectedBlock_ = findSelectedBlock();
      }

      void setMenuOpen(bool open) {
         if (menuOpen_ == open) {
            return;
         }
         menuOpen_ = open;
         applyInputMode();
      }

      void applyInputMode() {
         if (menuOpen_) {
            window_.setCursorMode(sys::CursorMode::Normal);
            window_.setRawMouseMotion(false);
            return;
         }
         window_.setCursorMode(settings_.window.cursorDisabled ? sys::CursorMode::Disabled : sys::CursorMode::Normal);
         window_.setRawMouseMotion(settings_.window.rawMouseMotion);
      }

      void applyWindowSettings() {
         window_.setVsync(settings_.window.vsync);
         if (settings_.window.fullscreen) {
            window_.setSize(settings_.window.size);
            window_.setFullscreen(true);
         } else {
            window_.setFullscreen(false);
            window_.setSize(settings_.window.size);
         }
         screenSize_ = window_.framebufferSize();
         renderer_.setViewport(screenSize_);
      }

      void deleteActiveWorld() {
         if (activeWorldIndex_ < 0 || activeWorldIndex_ >= static_cast<int>(worlds_.size())) {
            return;
         }

         const auto deletedIndex = activeWorldIndex_;
         if (!deleteWorld(worlds_[static_cast<std::size_t>(deletedIndex)])) {
            return;
         }

         worlds_ = listWorlds(settings_.world.saveDirectory);
         if (worlds_.empty()) {
            worlds_.push_back(createWorldEntry(settings_.world.saveDirectory));
         }

         activeWorldIndex_ = std::min(deletedIndex, static_cast<int>(worlds_.size()) - 1);
         settings_.world.activeWorld = worlds_[static_cast<std::size_t>(activeWorldIndex_)].id;
         static_cast<void>(saveAppSettings(settings_, kSettingsPath));

         hasUnsavedWorldChanges_ = false;
         resetGeneratedWorld();
         if (loadWorld(world_, worlds_[static_cast<std::size_t>(activeWorldIndex_)].savePath)) {
            if (!loadWorldPlayerState(player_, worlds_[static_cast<std::size_t>(activeWorldIndex_)])) {
               player_ = game::Player(initialCameraPosition(world_));
            }
         } else {
            hasUnsavedWorldChanges_ = true;
            static_cast<void>(saveActiveWorld(true));
         }
         worldChunks_.markAllDirty();
         selectedBlock_ = findSelectedBlock();
      }

      void saveSettings() {
         settings_ = validateSettings(std::move(settings_));
         static_cast<void>(saveAppSettings(settings_, kSettingsPath));
      }

      void requestClose() {
         static_cast<void>(saveActiveWorld());
         window_.close();
      }

      void applyRenderingSettings() {
         const auto color = settings_.rendering.clearColor;
         worldMaterial_.shader().set("uSkyLight", settings_.rendering.skyLight);
         worldMaterial_.shader().set("uGroundLight", settings_.rendering.groundLight);
         worldMaterial_.shader().set("uFogColor", glm::vec4{color.r, color.g, color.b, color.a});
         worldMaterial_.shader().set("uFogStart", settings_.rendering.fogStart);
         worldMaterial_.shader().set("uFogEnd", settings_.rendering.fogEnd);
      }

      AppSettings settings_;
      sys::Window window_;
      graphics::Renderer renderer_;
      game::World world_;
      game::Player player_;
      WorldChunks worldChunks_;
      sys::FrameClock clock_;
      glm::ivec2 screenSize_{};
      graphics::Mesh selectionMesh_;
      graphics::Material worldMaterial_;
      BlockTexturePaths blockTexturePaths_;
      graphics::Texture voidCoreTexture_;
      graphics::Texture voidCrystalTexture_;
      graphics::Texture voidGlassTexture_;
      graphics::Texture voidBedrockTexture_;
      graphics::Texture voidStoneTexture_;
      CrosshairRenderer crosshair_;
      GameMenu menu_;
      std::vector<WorldInfo> worlds_;
      std::vector<glm::ivec2> resolutions_;
      int activeWorldIndex_{-1};
      std::optional<game::RayHit> selectedBlock_;
      game::BlockType selectedBuildBlock_{game::BlockType::VoidCore};
      bool menuOpen_{true};
      bool hasUnsavedWorldChanges_{false};
   };

   Application::Application()
       : impl_(std::make_unique<Impl>()) {}

   Application::~Application() = default;

   void Application::run() {
      impl_->run();
   }

} // namespace etherblocks::app
