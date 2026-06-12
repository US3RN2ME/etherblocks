#include "Application.hpp"

#include <algorithm>
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
#include <nlohmann/json.hpp>
#include <optional>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include "SceneRendering.hpp"

namespace etherblocks::app {

   namespace {
      namespace graphics = engine::graphics;
      namespace sys = system;

      constexpr std::string_view kSettingsPath = "./assets/config/settings.json";
      constexpr std::array<char, 4> kWorldSaveMagic{'E', 'B', 'W', '3'};

      template <typename... Ts>
      struct Overloaded : Ts... {
         using Ts::operator()...;
      };

      template <typename... Ts>
      Overloaded(Ts...) -> Overloaded<Ts...>;

      struct AppSettings {
         struct Window {
            glm::ivec2 size{800, 600};
            std::string title{"etherblocks"};
            bool vsync{true};
            bool cursorDisabled{true};
            bool rawMouseMotion{true};
         };

         struct World {
            glm::ivec3 size{256, 64, 256};
            int chunkSize{16};
            std::string savePath{"./world.ebw"};
         };

         struct Rendering {
            graphics::Color clearColor{0.11f, 0.18f, 0.24f, 1.0f};
            glm::vec4 skyLight{1.08f, 1.14f, 1.2f, 1.0f};
            glm::vec4 groundLight{0.42f, 0.46f, 0.52f, 1.0f};
            float viewDistance{29.0f};
            float nearPlane{0.1f};
            float farPlane{100.0f};
            float fogStart{16.0f};
            float fogEnd{29.0f};
         };

         struct Player {
            float interactionRange{8.0f};
         };

         struct Assets {
            std::string blockManifest{"./assets/blocks/manifest.json"};
         };

         Window window{};
         World world{};
         Rendering rendering{};
         Player player{};
         Assets assets{};
      };

      struct BlockTexturePaths {
         std::string voidCore;
         std::string voidCrystal;
         std::string voidGlass;
         std::string voidBedrock;
         std::string voidStone;
      };

      template <typename T>
      T jsonValue(const nlohmann::json& json, std::string_view key, T fallback) {
         const auto it = json.find(std::string(key));
         if (it == json.end() || it->is_null()) {
            return fallback;
         }
         return it->get<T>();
      }

      glm::ivec2 jsonIvec2(const nlohmann::json& json, std::string_view key, glm::ivec2 fallback) {
         const auto it = json.find(std::string(key));
         if (it == json.end() || !it->is_array() || it->size() != 2) {
            return fallback;
         }
         return {(*it)[0].get<int>(), (*it)[1].get<int>()};
      }

      glm::ivec3 jsonIvec3(const nlohmann::json& json, std::string_view key, glm::ivec3 fallback) {
         const auto it = json.find(std::string(key));
         if (it == json.end() || !it->is_array() || it->size() != 3) {
            return fallback;
         }
         return {(*it)[0].get<int>(), (*it)[1].get<int>(), (*it)[2].get<int>()};
      }

      glm::vec4 jsonVec4(const nlohmann::json& json, std::string_view key, glm::vec4 fallback) {
         const auto it = json.find(std::string(key));
         if (it == json.end() || !it->is_array() || it->size() != 4) {
            return fallback;
         }
         return {(*it)[0].get<float>(), (*it)[1].get<float>(), (*it)[2].get<float>(), (*it)[3].get<float>()};
      }

      graphics::Color jsonColor(const nlohmann::json& json, std::string_view key, graphics::Color fallback) {
         const auto color = jsonVec4(json, key, {fallback.r, fallback.g, fallback.b, fallback.a});
         return {color.r, color.g, color.b, color.a};
      }

      AppSettings validateSettings(AppSettings settings) {
         settings.window.size.x = std::max(settings.window.size.x, 1);
         settings.window.size.y = std::max(settings.window.size.y, 1);
         settings.world.size.x = std::max(settings.world.size.x, 1);
         settings.world.size.y = std::max(settings.world.size.y, 1);
         settings.world.size.z = std::max(settings.world.size.z, 1);
         settings.world.chunkSize = std::max(settings.world.chunkSize, 1);
         settings.rendering.viewDistance = std::max(settings.rendering.viewDistance, 1.0f);
         settings.rendering.nearPlane = std::max(settings.rendering.nearPlane, 0.01f);
         settings.rendering.farPlane = std::max(settings.rendering.farPlane, settings.rendering.nearPlane + 1.0f);
         settings.rendering.fogEnd = std::max(settings.rendering.fogEnd, settings.rendering.fogStart + 0.01f);
         settings.player.interactionRange = std::max(settings.player.interactionRange, 0.1f);
         return settings;
      }

      AppSettings loadAppSettings(std::string_view path) {
         AppSettings settings{};
         std::ifstream file{std::string(path)};
         if (!file) {
            sys::log(sys::LogLevel::Warning, "Failed to open app settings, using defaults: " + std::string(path));
            return settings;
         }

         try {
            const auto json = nlohmann::json::parse(file);
            const auto window = json.value("window", nlohmann::json::object());
            const auto world = json.value("world", nlohmann::json::object());
            const auto rendering = json.value("rendering", nlohmann::json::object());
            const auto projection = rendering.value("projection", nlohmann::json::object());
            const auto fog = rendering.value("fog", nlohmann::json::object());
            const auto player = json.value("player", nlohmann::json::object());
            const auto assets = json.value("assets", nlohmann::json::object());

            settings.window.title = jsonValue(window, "title", settings.window.title);
            settings.window.size = jsonIvec2(window, "size", settings.window.size);
            settings.window.vsync = jsonValue(window, "vsync", settings.window.vsync);
            settings.window.cursorDisabled = jsonValue(window, "cursor_disabled", settings.window.cursorDisabled);
            settings.window.rawMouseMotion = jsonValue(window, "raw_mouse_motion", settings.window.rawMouseMotion);

            settings.world.size = jsonIvec3(world, "size", settings.world.size);
            settings.world.chunkSize = jsonValue(world, "chunk_size", settings.world.chunkSize);
            settings.world.savePath = jsonValue(world, "save_path", settings.world.savePath);

            settings.rendering.clearColor = jsonColor(rendering, "clear_color", settings.rendering.clearColor);
            settings.rendering.skyLight = jsonVec4(rendering, "sky_light", settings.rendering.skyLight);
            settings.rendering.groundLight = jsonVec4(rendering, "ground_light", settings.rendering.groundLight);
            settings.rendering.viewDistance = jsonValue(rendering, "view_distance", settings.rendering.viewDistance);
            settings.rendering.nearPlane = jsonValue(projection, "near", settings.rendering.nearPlane);
            settings.rendering.farPlane = jsonValue(projection, "far", settings.rendering.farPlane);
            settings.rendering.fogStart = jsonValue(fog, "start", settings.rendering.fogStart);
            settings.rendering.fogEnd = jsonValue(fog, "end", settings.rendering.fogEnd);

            settings.player.interactionRange = jsonValue(player, "interaction_range", settings.player.interactionRange);
            settings.assets.blockManifest = jsonValue(assets, "block_manifest", settings.assets.blockManifest);
         } catch (const std::exception& exception) {
            sys::log(sys::LogLevel::Error, "Failed to parse app settings, using defaults: " + std::string(exception.what()));
         }

         return validateSettings(std::move(settings));
      }

      std::string blockTexturePath(const nlohmann::json& manifest, std::string_view blockName) {
         return manifest.at("blocks").at(std::string(blockName)).at("texture").get<std::string>();
      }

      BlockTexturePaths loadBlockTexturePaths(std::string_view path) {
         std::ifstream file{std::string(path)};
         if (!file) {
            throw std::runtime_error{"Failed to open block texture manifest: " + std::string(path)};
         }

         const auto manifest = nlohmann::json::parse(file);
         return {
             blockTexturePath(manifest, "void_core"),  blockTexturePath(manifest, "void_crystal"),
             blockTexturePath(manifest, "void_glass"), blockTexturePath(manifest, "void_bedrock"),
             blockTexturePath(manifest, "void_stone"),
         };
      }

      std::uint8_t encodeBlock(game::BlockType block) noexcept {
         return std::to_underlying(block);
      }

      std::optional<game::BlockType> decodeBlock(std::uint8_t value) noexcept {
         switch (value) {
            case 0:
               return game::BlockType::Empty;
            case 1:
               return game::BlockType::VoidCore;
            case 2:
               return game::BlockType::VoidCrystal;
            case 3:
               return game::BlockType::VoidGlass;
            case 4:
               return game::BlockType::VoidBedrock;
            case 5:
               return game::BlockType::VoidStone;
            default:
               return {};
         }
      }

      [[nodiscard]] int ceilDiv(int value, int divisor) noexcept {
         return (value + divisor - 1) / divisor;
      }

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

      [[nodiscard]] std::size_t chunkIndex(glm::ivec2 chunk, glm::ivec2 chunkGridSize) noexcept {
         return static_cast<std::size_t>(chunk.x) +
                static_cast<std::size_t>(chunkGridSize.x) * static_cast<std::size_t>(chunk.y);
      }

      [[nodiscard]] int meshIndex(game::BlockType block) noexcept {
         switch (block) {
            case game::BlockType::VoidCore:
               return 0;
            case game::BlockType::VoidCrystal:
               return 1;
            case game::BlockType::VoidGlass:
               return 2;
            case game::BlockType::VoidBedrock:
               return 3;
            case game::BlockType::VoidStone:
               return 4;
         }
         return 0;
      }

      [[nodiscard]] game::BlockType blockTypeForMesh(int index) noexcept {
         switch (index) {
            case 0:
               return game::BlockType::VoidCore;
            case 1:
               return game::BlockType::VoidCrystal;
            case 2:
               return game::BlockType::VoidGlass;
            case 3:
               return game::BlockType::VoidBedrock;
            case 4:
               return game::BlockType::VoidStone;
            default:
               return game::BlockType::Empty;
         }
      }

      struct WorldChunk {
         explicit WorldChunk(glm::ivec2 origin)
             : origin(origin)
             , meshes{graphics::Mesh{sizeof(MeshVertex), meshLayout()}, graphics::Mesh{sizeof(MeshVertex), meshLayout()},
                      graphics::Mesh{sizeof(MeshVertex), meshLayout()}, graphics::Mesh{sizeof(MeshVertex), meshLayout()},
                      graphics::Mesh{sizeof(MeshVertex), meshLayout()}} {}

         glm::ivec2 origin{};
         std::array<graphics::Mesh, 5> meshes;
         bool dirty{true};
      };

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
          : settings_(loadAppSettings(kSettingsPath))
          , window_({settings_.window.size, settings_.window.title, settings_.window.vsync})
          , world_(settings_.world.size)
          , player_(initialCameraPosition(world_))
          , screenSize_(settings_.window.size)
          , selectionMesh_(sizeof(MeshVertex), meshLayout())
          , worldMaterial_("./shaders/basic.vertex.glsl", "./shaders/basic.fragment.glsl")
          , blockTexturePaths_(loadBlockTexturePaths(settings_.assets.blockManifest))
          , voidCoreTexture_(blockTexturePaths_.voidCore)
          , voidCrystalTexture_(blockTexturePaths_.voidCrystal)
          , voidGlassTexture_(blockTexturePaths_.voidGlass)
          , voidBedrockTexture_(blockTexturePaths_.voidBedrock)
          , voidStoneTexture_(blockTexturePaths_.voidStone) {
         renderer_.setViewport(window_.framebufferSize());
         renderer_.enable(graphics::RenderFeature::DepthTest);
         renderer_.enable(graphics::RenderFeature::ProgramPointSize);
         window_.setCursorMode(settings_.window.cursorDisabled ? sys::CursorMode::Disabled : sys::CursorMode::Normal);
         window_.setRawMouseMotion(settings_.window.rawMouseMotion);

         worldMaterial_.shader().set("uTexture", 0);
         worldMaterial_.shader().set("uModel", glm::mat4{1.0f});
         worldMaterial_.shader().set("uSkyLight", settings_.rendering.skyLight);
         worldMaterial_.shader().set("uGroundLight", settings_.rendering.groundLight);
         worldMaterial_.shader().set("uFogColor", fogColor());
         worldMaterial_.shader().set("uFogStart", settings_.rendering.fogStart);
         worldMaterial_.shader().set("uFogEnd", settings_.rendering.fogEnd);
         createWorldChunks();
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
            static_cast<void>(saveWorld(world_, settings_.world.savePath));
         }
         if (input.isKeyPressed(sys::Key::F9)) {
            if (loadWorld(world_, settings_.world.savePath)) {
               markAllWorldChunksDirty();
               selectedBlock_ = findSelectedBlock();
            }
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

         player_.update(input, deltaTime, [this](glm::ivec3 position) {
            return world_.isSolid(position);
         });

         selectedBlock_ = findSelectedBlock();
         editWorld(input);
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
            if (!player_.occupiesVoxel(position)) {
               if (world_.setBlock(position, selectedBuildBlock_)) {
                  markWorldChunkDirty(position);
                  selectedBlock_ = findSelectedBlock();
               }
            }
         }
         if (input.isMouseButtonPressed(sys::MouseButton::Right)) {
            if (world_.setBlock(selectedBlock_->voxel, game::BlockType::Empty)) {
               markWorldChunkDirty(selectedBlock_->voxel);
               selectedBlock_ = findSelectedBlock();
            }
         }
      }

      void createWorldChunks() {
         const auto size = world_.size();
         worldChunkGridSize_ = {ceilDiv(size.x, settings_.world.chunkSize), ceilDiv(size.z, settings_.world.chunkSize)};
         worldChunks_.clear();
         worldChunks_.reserve(static_cast<std::size_t>(worldChunkGridSize_.x * worldChunkGridSize_.y));
         for (auto chunkZ = 0; chunkZ < worldChunkGridSize_.y; ++chunkZ) {
            for (auto chunkX = 0; chunkX < worldChunkGridSize_.x; ++chunkX) {
               worldChunks_.emplace_back(glm::ivec2{chunkX * settings_.world.chunkSize, chunkZ * settings_.world.chunkSize});
            }
         }
      }

      [[nodiscard]] bool containsWorldChunk(glm::ivec2 chunk) const noexcept {
         return chunk.x >= 0 && chunk.y >= 0 && chunk.x < worldChunkGridSize_.x && chunk.y < worldChunkGridSize_.y;
      }

      [[nodiscard]] glm::ivec2 chunkForBlock(glm::ivec3 position) const noexcept {
         return {position.x / settings_.world.chunkSize, position.z / settings_.world.chunkSize};
      }

      [[nodiscard]] glm::ivec3 chunkMin(const WorldChunk& chunk) const noexcept {
         return {chunk.origin.x, 0, chunk.origin.y};
      }

      [[nodiscard]] glm::ivec3 chunkMax(const WorldChunk& chunk) const noexcept {
         const auto size = world_.size();
         return {std::min(chunk.origin.x + settings_.world.chunkSize, size.x), size.y,
                 std::min(chunk.origin.y + settings_.world.chunkSize, size.z)};
      }

      void markWorldChunkDirty(glm::ivec3 blockPosition) {
         const auto chunk = chunkForBlock(blockPosition);
         if (!containsWorldChunk(chunk)) {
            return;
         }
         worldChunks_[chunkIndex(chunk, worldChunkGridSize_)].dirty = true;

         const auto localX = blockPosition.x - chunk.x * settings_.world.chunkSize;
         const auto localZ = blockPosition.z - chunk.y * settings_.world.chunkSize;
         if (localX == 0) {
            markWorldChunkDirty(chunk + glm::ivec2{-1, 0});
         }
         if (localX == settings_.world.chunkSize - 1) {
            markWorldChunkDirty(chunk + glm::ivec2{1, 0});
         }
         if (localZ == 0) {
            markWorldChunkDirty(chunk + glm::ivec2{0, -1});
         }
         if (localZ == settings_.world.chunkSize - 1) {
            markWorldChunkDirty(chunk + glm::ivec2{0, 1});
         }
      }

      void markWorldChunkDirty(glm::ivec2 chunk) {
         if (!containsWorldChunk(chunk)) {
            return;
         }
         worldChunks_[chunkIndex(chunk, worldChunkGridSize_)].dirty = true;
      }

      void markAllWorldChunksDirty() {
         for (auto& chunk : worldChunks_) {
            chunk.dirty = true;
         }
      }

      void rebuildWorldChunk(WorldChunk& chunk) {
         const auto min = chunkMin(chunk);
         const auto max = chunkMax(chunk);
         for (auto mesh = 0; mesh < static_cast<int>(chunk.meshes.size()); ++mesh) {
            auto vertices = buildWorldMesh(world_, blockTypeForMesh(mesh), min, max);
            chunk.meshes[static_cast<std::size_t>(mesh)].upload(std::span<const MeshVertex>{vertices},
                                                                graphics::BufferUsage::DynamicDraw);
         }
      }

      void render() {
         renderer_.clear(settings_.rendering.clearColor, graphics::ClearBuffer::Color | graphics::ClearBuffer::Depth);

         worldMaterial_.shader().set("uView", player_.camera().createViewMatrix());
         worldMaterial_.shader().set("uProjection", createProjectionMatrix());
         const auto cameraPosition = player_.camera().position();
         worldMaterial_.shader().set("uCameraPosition", glm::vec4{cameraPosition, 1.0f});
         worldMaterial_.shader().set("uUseOverrideColor", false);
         drawWorldChunks(cameraPosition);

         if (selectedBlock_) {
            drawSelectionBox(selectedBlock_->voxel);
         }

         renderer_.disable(graphics::RenderFeature::DepthTest);
         crosshair_.draw(renderer_);
         renderer_.enable(graphics::RenderFeature::DepthTest);
      }

      void drawWorldMesh(const graphics::Mesh& mesh, const graphics::Texture& texture) {
         if (mesh.vertexCount() == 0) {
            return;
         }
         worldMaterial_.setTexture(texture);
         renderer_.draw(mesh, worldMaterial_);
      }

      [[nodiscard]] bool isWorldChunkVisible(const WorldChunk& chunk, glm::vec3 cameraPosition) const noexcept {
         const auto min = glm::vec3(chunkMin(chunk)) - glm::vec3{0.5f};
         const auto max = glm::vec3(chunkMax(chunk)) + glm::vec3{0.5f};
         const auto closest = glm::clamp(cameraPosition, min, max);
         return glm::length(closest - cameraPosition) <= settings_.rendering.viewDistance;
      }

      void drawWorldChunks(glm::vec3 cameraPosition) {
         for (auto& chunk : worldChunks_) {
            if (!isWorldChunkVisible(chunk, cameraPosition)) {
               continue;
            }
            if (chunk.dirty) {
               rebuildWorldChunk(chunk);
               chunk.dirty = false;
            }
            drawWorldMesh(chunk.meshes[meshIndex(game::BlockType::VoidCore)], voidCoreTexture_);
            drawWorldMesh(chunk.meshes[meshIndex(game::BlockType::VoidCrystal)], voidCrystalTexture_);
            drawWorldMesh(chunk.meshes[meshIndex(game::BlockType::VoidGlass)], voidGlassTexture_);
            drawWorldMesh(chunk.meshes[meshIndex(game::BlockType::VoidBedrock)], voidBedrockTexture_);
            drawWorldMesh(chunk.meshes[meshIndex(game::BlockType::VoidStone)], voidStoneTexture_);
         }
      }

      [[nodiscard]] glm::mat4 createProjectionMatrix() const {
         const auto aspectRatio =
             screenSize_.y > 0 ? static_cast<float>(screenSize_.x) / static_cast<float>(screenSize_.y) : 1.0f;
         return glm::perspective(glm::radians(player_.camera().zoom()), aspectRatio, settings_.rendering.nearPlane,
                                 settings_.rendering.farPlane);
      }

      [[nodiscard]] glm::vec4 fogColor() const noexcept {
         const auto color = settings_.rendering.clearColor;
         return {color.r, color.g, color.b, color.a};
      }

      void drawSelectionBox(glm::ivec3 position) {
         const auto vertices = buildSelectionBox(position);
         selectionMesh_.upload(std::span<const MeshVertex>{vertices}, graphics::BufferUsage::DynamicDraw);
         worldMaterial_.shader().set("uUseOverrideColor", true);
         worldMaterial_.shader().set("uOverrideColor", glm::vec4({1.0f, 0.690196f, 0.0f, 1.0f}));
         renderer_.setLineWidth(2.0f);
         renderer_.draw(selectionMesh_, worldMaterial_, graphics::Primitive::Lines);
      }

      AppSettings settings_;
      sys::Window window_;
      graphics::Renderer renderer_;
      game::World world_;
      game::Player player_;
      sys::FrameClock clock_;
      glm::ivec2 screenSize_{};
      glm::ivec2 worldChunkGridSize_{};
      std::vector<WorldChunk> worldChunks_;
      graphics::Mesh selectionMesh_;
      graphics::Material worldMaterial_;
      BlockTexturePaths blockTexturePaths_;
      graphics::Texture voidCoreTexture_;
      graphics::Texture voidCrystalTexture_;
      graphics::Texture voidGlassTexture_;
      graphics::Texture voidBedrockTexture_;
      graphics::Texture voidStoneTexture_;
      CrosshairRenderer crosshair_;
      std::optional<game::RayHit> selectedBlock_;
      game::BlockType selectedBuildBlock_{game::BlockType::VoidCore};
   };

   Application::Application()
       : impl_(std::make_unique<Impl>()) {}

   Application::~Application() = default;

   void Application::run() {
      impl_->run();
   }

} // namespace etherblocks::app
