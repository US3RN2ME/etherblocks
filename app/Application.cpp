#include "Application.hpp"

#include <array>
#include <cstdint>
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
#include <fstream>
#include <glm/gtc/matrix_transform.hpp>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <variant>

#include "SceneRendering.hpp"

namespace etherblocks::app {

   namespace {
      namespace graphics = engine::graphics;
      namespace sys = system;

      constexpr auto kWindowSize = glm::ivec2{800, 600};
      constexpr auto kInteractionRange = 8.0f;
      constexpr auto kSkyLight = glm::vec4{1.08f, 1.14f, 1.2f, 1.0f};
      constexpr auto kGroundLight = glm::vec4{0.42f, 0.46f, 0.52f, 1.0f};
      constexpr auto kSkyColor = graphics::Color{0.11f, 0.18f, 0.24f, 1.0f};
      constexpr auto kFogColor = glm::vec4{kSkyColor.r, kSkyColor.g, kSkyColor.b, kSkyColor.a};
      constexpr auto kFogStart = 16.0f;
      constexpr auto kFogEnd = 29.0f;
      constexpr std::string_view kWorldSavePath = "./world.ebw";
      constexpr std::array<char, 4> kWorldSaveMagic{'E', 'B', 'W', '2'};

      template <typename... Ts>
      struct Overloaded : Ts... {
         using Ts::operator()...;
      };

      template <typename... Ts>
      Overloaded(Ts...) -> Overloaded<Ts...>;

      std::uint8_t encodeBlock(game::BlockType block) noexcept {
         return std::to_underlying(block);
      }

      std::optional<game::BlockType> decodeBlock(std::uint8_t value) noexcept {
         switch (value) {
            case 0:
               return game::BlockType::Empty;
            case 1:
               return game::BlockType::EtherCrystalBlue;
            case 2:
               return game::BlockType::EtherCrystalViolet;
            case 3:
               return game::BlockType::EtherGlass;
            case 4:
               return game::BlockType::EtherStoneBlue;
            case 5:
               return game::BlockType::EtherStoneViolet;
            default:
               return {};
         }
      }

      bool saveWorld(const game::World& world, std::string_view path) {
         std::ofstream file{std::string(path), std::ios::binary};
         if (!file) {
            sys::log(sys::LogLevel::Error, "Failed to open world save for writing: " + std::string(path));
            return false;
         }

         const auto size = world.size();
         const std::array<std::int32_t, 3> dimensions{size.x, size.y, size.z};
         const auto blockCount = static_cast<std::uint32_t>(size.x * size.y * size.z);

         file.write(kWorldSaveMagic.data(), kWorldSaveMagic.size());
         file.write(reinterpret_cast<const char*>(dimensions.data()), sizeof(dimensions));
         file.write(reinterpret_cast<const char*>(&blockCount), sizeof(blockCount));

         world.forEachBlock([&](glm::ivec3, game::BlockType block) {
            const auto value = encodeBlock(block);
            file.write(reinterpret_cast<const char*>(&value), sizeof(value));
         });

         if (!file) {
            sys::log(sys::LogLevel::Error, "Failed while writing world save: " + std::string(path));
            return false;
         }

         sys::log(sys::LogLevel::Info, "World saved: " + std::string(path));
         return true;
      }

      bool loadWorld(game::World& world, std::string_view path) {
         std::ifstream file{std::string(path), std::ios::binary};
         if (!file) {
            sys::log(sys::LogLevel::Info, "No world save found: " + std::string(path));
            return false;
         }

         std::array<char, 4> magic{};
         std::array<std::int32_t, 3> dimensions{};
         std::uint32_t blockCount{};
         file.read(magic.data(), magic.size());
         file.read(reinterpret_cast<char*>(dimensions.data()), sizeof(dimensions));
         file.read(reinterpret_cast<char*>(&blockCount), sizeof(blockCount));

         const auto size = world.size();
         const auto expectedBlockCount = static_cast<std::uint32_t>(size.x * size.y * size.z);
         if (!file || magic != kWorldSaveMagic || dimensions != std::array<std::int32_t, 3>{size.x, size.y, size.z} ||
             blockCount != expectedBlockCount) {
            sys::log(sys::LogLevel::Warning, "World save is incompatible or corrupt: " + std::string(path));
            return false;
         }

         for (auto y = 0; y < size.y; ++y) {
            for (auto z = 0; z < size.z; ++z) {
               for (auto x = 0; x < size.x; ++x) {
                  std::uint8_t value{};
                  file.read(reinterpret_cast<char*>(&value), sizeof(value));
                  const auto block = decodeBlock(value);
                  if (!file || !block) {
                     sys::log(sys::LogLevel::Warning,
                              "World save ended unexpectedly or contains an unknown block: " + std::string(path));
                     return false;
                  }
                  static_cast<void>(world.setBlock({x, y, z}, *block));
               }
            }
         }

         sys::log(sys::LogLevel::Info, "World loaded: " + std::string(path));
         return true;
      }
   } // namespace

   class Application::Impl {
   public:
      Impl()
          : window_({kWindowSize, "etherblocks"})
          , player_({0.0f, 3.0f, 3.0f})
          , etherCrystalBlueMesh_(sizeof(MeshVertex), meshLayout())
          , etherCrystalVioletMesh_(sizeof(MeshVertex), meshLayout())
          , etherGlassMesh_(sizeof(MeshVertex), meshLayout())
          , etherStoneBlueMesh_(sizeof(MeshVertex), meshLayout())
          , etherStoneVioletMesh_(sizeof(MeshVertex), meshLayout())
          , selectionMesh_(sizeof(MeshVertex), meshLayout())
          , worldMaterial_("./shaders/basic.vertex.glsl", "./shaders/basic.fragment.glsl")
          , etherCrystalBlueTexture_("./assets/blocks/ether_crystal_blue.png")
          , etherCrystalVioletTexture_("./assets/blocks/ether_crystal_violet.png")
          , etherGlassTexture_("./assets/blocks/ether_glass.png")
          , etherStoneBlueTexture_("./assets/blocks/ether_stone_blue.png")
          , etherStoneVioletTexture_("./assets/blocks/ether_stone_violet.png") {
         renderer_.setViewport(window_.framebufferSize());
         renderer_.enable(graphics::RenderFeature::DepthTest);
         renderer_.enable(graphics::RenderFeature::ProgramPointSize);
         window_.setCursorMode(sys::CursorMode::Disabled);
         window_.setRawMouseMotion(true);

         worldMaterial_.shader().set("uTexture", 0);
         worldMaterial_.shader().set("uModel", glm::mat4{1.0f});
         worldMaterial_.shader().set("uSkyLight", kSkyLight);
         worldMaterial_.shader().set("uGroundLight", kGroundLight);
         worldMaterial_.shader().set("uFogColor", kFogColor);
         worldMaterial_.shader().set("uFogStart", kFogStart);
         worldMaterial_.shader().set("uFogEnd", kFogEnd);
         static_cast<void>(loadWorld(world_, kWorldSavePath));
         rebuildWorldMesh();
         sys::log(sys::LogLevel::Info, "Application initialized");
      }

      ~Impl() {
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
         if (input.isKeyPressed(sys::Key::F5)) {
            static_cast<void>(saveWorld(world_, kWorldSavePath));
         }
         if (input.isKeyPressed(sys::Key::F9)) {
            if (loadWorld(world_, kWorldSavePath)) {
               worldMeshDirty_ = true;
               selectedBlock_ = findSelectedBlock();
            }
         }
         if (input.isKeyPressed(sys::Key::Num1)) {
            selectedBuildBlock_ = game::BlockType::EtherCrystalBlue;
         }
         if (input.isKeyPressed(sys::Key::Num2)) {
            selectedBuildBlock_ = game::BlockType::EtherCrystalViolet;
         }
         if (input.isKeyPressed(sys::Key::Num3)) {
            selectedBuildBlock_ = game::BlockType::EtherGlass;
         }
         if (input.isKeyPressed(sys::Key::Num4)) {
            selectedBuildBlock_ = game::BlockType::EtherStoneBlue;
         }
         if (input.isKeyPressed(sys::Key::Num5)) {
            selectedBuildBlock_ = game::BlockType::EtherStoneViolet;
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
               worldMeshDirty_ |= world_.setBlock(position, selectedBuildBlock_);
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
         auto vertices = buildWorldMesh(world_, game::BlockType::EtherCrystalBlue);
         etherCrystalBlueMesh_.upload(std::span<const MeshVertex>{vertices}, graphics::BufferUsage::DynamicDraw);

         vertices = buildWorldMesh(world_, game::BlockType::EtherCrystalViolet);
         etherCrystalVioletMesh_.upload(std::span<const MeshVertex>{vertices}, graphics::BufferUsage::DynamicDraw);

         vertices = buildWorldMesh(world_, game::BlockType::EtherGlass);
         etherGlassMesh_.upload(std::span<const MeshVertex>{vertices}, graphics::BufferUsage::DynamicDraw);

         vertices = buildWorldMesh(world_, game::BlockType::EtherStoneBlue);
         etherStoneBlueMesh_.upload(std::span<const MeshVertex>{vertices}, graphics::BufferUsage::DynamicDraw);

         vertices = buildWorldMesh(world_, game::BlockType::EtherStoneViolet);
         etherStoneVioletMesh_.upload(std::span<const MeshVertex>{vertices}, graphics::BufferUsage::DynamicDraw);
         worldMeshDirty_ = false;
      }

      void render() {
         renderer_.clear(kSkyColor, graphics::ClearBuffer::Color | graphics::ClearBuffer::Depth);

         worldMaterial_.shader().set("uView", player_.camera().createViewMatrix());
         worldMaterial_.shader().set("uProjection", createProjectionMatrix());
         const auto cameraPosition = player_.camera().position();
         worldMaterial_.shader().set("uCameraPosition", glm::vec4{cameraPosition, 1.0f});
         worldMaterial_.shader().set("uUseOverrideColor", false);
         drawWorldMesh(etherCrystalBlueMesh_, etherCrystalBlueTexture_);
         drawWorldMesh(etherCrystalVioletMesh_, etherCrystalVioletTexture_);
         drawWorldMesh(etherGlassMesh_, etherGlassTexture_);
         drawWorldMesh(etherStoneBlueMesh_, etherStoneBlueTexture_);
         drawWorldMesh(etherStoneVioletMesh_, etherStoneVioletTexture_);

         if (selectedBlock_) {
            drawSelectionBox(selectedBlock_->voxel);
         }

         renderer_.disable(graphics::RenderFeature::DepthTest);
         crosshair_.draw(renderer_);
         renderer_.enable(graphics::RenderFeature::DepthTest);
      }

      void drawWorldMesh(const graphics::Mesh& mesh, const graphics::Texture& texture) {
         worldMaterial_.setTexture(texture);
         renderer_.draw(mesh, worldMaterial_);
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
      graphics::Mesh etherCrystalBlueMesh_;
      graphics::Mesh etherCrystalVioletMesh_;
      graphics::Mesh etherGlassMesh_;
      graphics::Mesh etherStoneBlueMesh_;
      graphics::Mesh etherStoneVioletMesh_;
      graphics::Mesh selectionMesh_;
      graphics::Material worldMaterial_;
      graphics::Texture etherCrystalBlueTexture_;
      graphics::Texture etherCrystalVioletTexture_;
      graphics::Texture etherGlassTexture_;
      graphics::Texture etherStoneBlueTexture_;
      graphics::Texture etherStoneVioletTexture_;
      CrosshairRenderer crosshair_;
      std::optional<game::RayHit> selectedBlock_;
      game::BlockType selectedBuildBlock_{game::BlockType::EtherCrystalBlue};
      bool worldMeshDirty_{true};
   };

   Application::Application()
       : impl_(std::make_unique<Impl>()) {}

   Application::~Application() = default;

   void Application::run() {
      impl_->run();
   }

} // namespace etherblocks::app
