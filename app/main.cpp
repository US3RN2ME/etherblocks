#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <etherblocks/Camera.hpp>
#include <etherblocks/Material.hpp>
#include <etherblocks/Mesh.hpp>
#include <etherblocks/Renderer.hpp>
#include <etherblocks/Texture.hpp>
#include <etherblocks/Window.hpp>
#include <exception>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <limits>
#include <optional>
#include <span>
#include <variant>
#include <vector>

namespace {

   constexpr auto kWindowSize = glm::ivec2{800, 600};
   constexpr auto kInteractionRange = 8.0f;

   struct MeshVertex {
      glm::vec3 position{};
      glm::vec2 textureCoordinate{};
   };

   constexpr std::array kMeshLayout{
       etherblocks::VertexAttribute{0, 3, etherblocks::VertexAttributeType::Float, false, offsetof(MeshVertex, position)},
       etherblocks::VertexAttribute{1, 2, etherblocks::VertexAttributeType::Float, false,
                                    offsetof(MeshVertex, textureCoordinate)},
   };

   class FrameClock {
   public:
      [[nodiscard]] float tick(double time) noexcept {
         const auto current = static_cast<float>(time);
         const auto delta = initialized_ ? current - previous_ : 0.0f;
         previous_ = current;
         initialized_ = true;
         return delta;
      }

   private:
      float previous_{};
      bool initialized_{};
   };

   template <typename... Ts>
   struct Overloaded : Ts... {
      using Ts::operator()...;
   };

   template <typename... Ts>
   Overloaded(Ts...) -> Overloaded<Ts...>;

   std::array<MeshVertex, 24> buildSelectionBox(glm::ivec3 position) {
      constexpr auto halfExtent = 0.505f;
      const auto center = glm::vec3(position);
      const auto min = center - glm::vec3(halfExtent);
      const auto max = center + glm::vec3(halfExtent);

      const std::array<glm::vec3, 8> corners{{
          {min.x, min.y, min.z},
          {max.x, min.y, min.z},
          {max.x, max.y, min.z},
          {min.x, max.y, min.z},
          {min.x, min.y, max.z},
          {max.x, min.y, max.z},
          {max.x, max.y, max.z},
          {min.x, max.y, max.z},
      }};

      constexpr std::array<std::array<int, 2>, 12> edges{{
          {{0, 1}},
          {{1, 2}},
          {{2, 3}},
          {{3, 0}},
          {{4, 5}},
          {{5, 6}},
          {{6, 7}},
          {{7, 4}},
          {{0, 4}},
          {{1, 5}},
          {{2, 6}},
          {{3, 7}},
      }};

      std::array<MeshVertex, 24> vertices{};
      for (std::size_t index = 0; index < edges.size(); ++index) {
         const auto vertexIndex = index * 2;
         vertices[vertexIndex].position = corners[edges[index][0]];
         vertices[vertexIndex + 1].position = corners[edges[index][1]];
      }
      return vertices;
   }

   struct FaceDefinition {
      float vertices[6][5];
      glm::ivec3 neighborOffset;
   };

   constexpr std::array<FaceDefinition, 6> kCubeFaces{{
       {{{-0.5f, -0.5f, -0.5f, 0, 0},
         {0.5f, -0.5f, -0.5f, 1, 0},
         {0.5f, 0.5f, -0.5f, 1, 1},
         {0.5f, 0.5f, -0.5f, 1, 1},
         {-0.5f, 0.5f, -0.5f, 0, 1},
         {-0.5f, -0.5f, -0.5f, 0, 0}},
        {0, 0, -1}},
       {{{-0.5f, -0.5f, 0.5f, 0, 0},
         {0.5f, -0.5f, 0.5f, 1, 0},
         {0.5f, 0.5f, 0.5f, 1, 1},
         {0.5f, 0.5f, 0.5f, 1, 1},
         {-0.5f, 0.5f, 0.5f, 0, 1},
         {-0.5f, -0.5f, 0.5f, 0, 0}},
        {0, 0, 1}},
       {{{-0.5f, 0.5f, 0.5f, 1, 0},
         {-0.5f, 0.5f, -0.5f, 1, 1},
         {-0.5f, -0.5f, -0.5f, 0, 1},
         {-0.5f, -0.5f, -0.5f, 0, 1},
         {-0.5f, -0.5f, 0.5f, 0, 0},
         {-0.5f, 0.5f, 0.5f, 1, 0}},
        {-1, 0, 0}},
       {{{0.5f, 0.5f, 0.5f, 1, 0},
         {0.5f, 0.5f, -0.5f, 1, 1},
         {0.5f, -0.5f, -0.5f, 0, 1},
         {0.5f, -0.5f, -0.5f, 0, 1},
         {0.5f, -0.5f, 0.5f, 0, 0},
         {0.5f, 0.5f, 0.5f, 1, 0}},
        {1, 0, 0}},
       {{{-0.5f, -0.5f, -0.5f, 0, 1},
         {0.5f, -0.5f, -0.5f, 1, 1},
         {0.5f, -0.5f, 0.5f, 1, 0},
         {0.5f, -0.5f, 0.5f, 1, 0},
         {-0.5f, -0.5f, 0.5f, 0, 0},
         {-0.5f, -0.5f, -0.5f, 0, 1}},
        {0, -1, 0}},
       {{{-0.5f, 0.5f, -0.5f, 0, 1},
         {0.5f, 0.5f, -0.5f, 1, 1},
         {0.5f, 0.5f, 0.5f, 1, 0},
         {0.5f, 0.5f, 0.5f, 1, 0},
         {-0.5f, 0.5f, 0.5f, 0, 0},
         {-0.5f, 0.5f, -0.5f, 0, 1}},
        {0, 1, 0}},
   }};

   struct RayHit {
      glm::ivec3 voxel;
      glm::ivec3 normal;
      float distance{};
   };

   template <typename IsSolid>
   std::optional<RayHit> castRay(glm::vec3 origin, glm::vec3 direction, float maxDistance, IsSolid&& isSolid) {
      if (glm::length(direction) == 0.0f)
         return {};

      direction = glm::normalize(direction);
      origin += glm::vec3(0.5f);

      glm::ivec3 voxel = glm::floor(origin);
      glm::ivec3 normal{};
      const glm::ivec3 step{direction.x > 0.0f ? 1 : -1, direction.y > 0.0f ? 1 : -1, direction.z > 0.0f ? 1 : -1};

      constexpr auto infinity = std::numeric_limits<float>::infinity();
      const glm::vec3 deltaDistance{direction.x != 0.0f ? std::abs(1.0f / direction.x) : infinity,
                                    direction.y != 0.0f ? std::abs(1.0f / direction.y) : infinity,
                                    direction.z != 0.0f ? std::abs(1.0f / direction.z) : infinity};
      const glm::vec3 nextBoundary{step.x > 0 ? static_cast<float>(voxel.x + 1) : static_cast<float>(voxel.x),
                                   step.y > 0 ? static_cast<float>(voxel.y + 1) : static_cast<float>(voxel.y),
                                   step.z > 0 ? static_cast<float>(voxel.z + 1) : static_cast<float>(voxel.z)};
      glm::vec3 sideDistance{direction.x != 0.0f ? (nextBoundary.x - origin.x) / direction.x : infinity,
                             direction.y != 0.0f ? (nextBoundary.y - origin.y) / direction.y : infinity,
                             direction.z != 0.0f ? (nextBoundary.z - origin.z) / direction.z : infinity};

      float distance = 0.0f;
      while (distance <= maxDistance) {
         if (isSolid(voxel))
            return RayHit{voxel, normal, distance};

         if (sideDistance.x < sideDistance.y && sideDistance.x < sideDistance.z) {
            voxel.x += step.x;
            distance = sideDistance.x;
            sideDistance.x += deltaDistance.x;
            normal = {-step.x, 0, 0};
         } else if (sideDistance.y < sideDistance.z) {
            voxel.y += step.y;
            distance = sideDistance.y;
            sideDistance.y += deltaDistance.y;
            normal = {0, -step.y, 0};
         } else {
            voxel.z += step.z;
            distance = sideDistance.z;
            sideDistance.z += deltaDistance.z;
            normal = {0, 0, -step.z};
         }
      }
      return {};
   }

   enum class BlockType { Empty, Basic };

   template <std::size_t Size>
   class World {
   public:
      World() {
         forEach([](glm::ivec3 position, BlockType& block) {
            if (position.y == 0)
               block = BlockType::Basic;
         });
      }

      [[nodiscard]] bool setBlock(glm::ivec3 position, BlockType block) noexcept {
         if (!contains(position) || blocks_[index(position)] == block)
            return false;

         blocks_[index(position)] = block;
         return true;
      }

      [[nodiscard]] bool isSolid(glm::ivec3 position) const noexcept {
         return contains(position) && blocks_[index(position)] != BlockType::Empty;
      }

      [[nodiscard]] std::vector<MeshVertex> buildMesh() {
         std::vector<MeshVertex> vertices;
         vertices.reserve(Size * Size * Size * kCubeFaces.size() * 6);

         forEach([&](glm::ivec3 position, BlockType& block) {
            if (block == BlockType::Empty)
               return;

            for (const auto& face : kCubeFaces) {
               if (isSolid(position + face.neighborOffset))
                  continue;

               for (const auto& vertex : face.vertices) {
                  vertices.push_back({
                      glm::vec3(position) + glm::vec3{vertex[0], vertex[1], vertex[2]},
                      {vertex[3], vertex[4]},
                  });
               }
            }
         });
         return vertices;
      }

   private:
      template <typename Function>
      void forEach(Function&& function) {
         for (std::size_t y = 0; y < Size; ++y) {
            for (std::size_t z = 0; z < Size; ++z) {
               for (std::size_t x = 0; x < Size; ++x) {
                  const auto position = glm::ivec3{x, y, z};
                  function(position, blocks_[index(position)]);
               }
            }
         }
      }

      [[nodiscard]] static bool contains(glm::ivec3 position) noexcept {
         return position.x >= 0 && position.y >= 0 && position.z >= 0 && position.x < static_cast<int>(Size) &&
                position.y < static_cast<int>(Size) && position.z < static_cast<int>(Size);
      }

      [[nodiscard]] static std::size_t index(glm::ivec3 position) noexcept {
         return static_cast<std::size_t>(position.x) +
                (Size * (static_cast<std::size_t>(position.y) + Size * static_cast<std::size_t>(position.z)));
      }

      std::array<BlockType, Size * Size * Size> blocks_{BlockType::Empty};
   };

   class CrosshairRenderer {
   public:
      CrosshairRenderer()
          : mesh_(0, {})
          , material_("./shaders/crosshair.vertex.glsl", "./shaders/crosshair.fragment.glsl") {
         mesh_.setVertexCount(1);
      }

      void draw(const etherblocks::Renderer& renderer) {
         material_.shader().set("uType", 0);
         material_.shader().set("uSize", 8.0f);
         material_.shader().set("uColor", glm::vec4{1.0f});
         renderer.draw(mesh_, material_, etherblocks::Primitive::Points);
      }

   private:
      etherblocks::Mesh mesh_;
      etherblocks::Material material_;
   };

   class Application {
   public:
      Application()
          : window_({kWindowSize, "etherblocks"})
          , camera_({0.0f, 0.0f, 3.0f})
          , worldMesh_(sizeof(MeshVertex), kMeshLayout)
          , selectionMesh_(sizeof(MeshVertex), kMeshLayout)
          , worldMaterial_("./shaders/basic.vertex.glsl", "./shaders/basic.fragment.glsl")
          , worldTexture_("./assets/container.png") {
         renderer_.setViewport(window_.framebufferSize());
         renderer_.enable(etherblocks::RenderFeature::DepthTest);
         renderer_.enable(etherblocks::RenderFeature::ProgramPointSize);
         window_.setCursorMode(etherblocks::CursorMode::Disabled);
         window_.setRawMouseMotion(true);

         worldMaterial_.setTexture(worldTexture_);
         worldMaterial_.shader().set("uTexture", 0);
         worldMaterial_.shader().set("uModel", glm::mat4{1.0f});
         rebuildWorldMesh();
      }

      void run() {
         while (window_.isOpen()) {
            const auto deltaTime = clock_.tick(etherblocks::Window::elapsedTime());
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
                           [this](const etherblocks::event::Closed&) {
                              window_.close();
                           },
                           [this](const etherblocks::event::Resized& resized) {
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
         if (input.isKeyPressed(etherblocks::Key::Escape))
            window_.close();

         const auto mouseDelta = glm::vec2(input.mouseDelta());
         camera_.rotateByMouseOffset(mouseDelta.x, -mouseDelta.y);
         camera_.zoomByScrollOffset(static_cast<float>(input.scrollDelta().y));
         moveCamera(input, deltaTime);

         selectedBlock_ = findSelectedBlock();
         editWorld(input);
         if (worldMeshDirty_)
            rebuildWorldMesh();
      }

      void moveCamera(const etherblocks::Input& input, float deltaTime) {
         if (input.isKeyHeld(etherblocks::Key::W))
            camera_.move(etherblocks::CameraMovement::Forward, deltaTime);
         if (input.isKeyHeld(etherblocks::Key::S))
            camera_.move(etherblocks::CameraMovement::Backward, deltaTime);
         if (input.isKeyHeld(etherblocks::Key::A))
            camera_.move(etherblocks::CameraMovement::Left, deltaTime);
         if (input.isKeyHeld(etherblocks::Key::D))
            camera_.move(etherblocks::CameraMovement::Right, deltaTime);
      }

      [[nodiscard]] std::optional<RayHit> findSelectedBlock() const {
         return castRay(camera_.position(), camera_.front(), kInteractionRange, [this](glm::ivec3 position) {
            return world_.isSolid(position);
         });
      }

      void editWorld(const etherblocks::Input& input) {
         if (!selectedBlock_)
            return;

         if (input.isMouseButtonPressed(etherblocks::MouseButton::Left)) {
            worldMeshDirty_ |= world_.setBlock(selectedBlock_->voxel + selectedBlock_->normal, BlockType::Basic);
         }
         if (input.isMouseButtonPressed(etherblocks::MouseButton::Right)) {
            worldMeshDirty_ |= world_.setBlock(selectedBlock_->voxel, BlockType::Empty);
         }
         if (worldMeshDirty_)
            selectedBlock_ = findSelectedBlock();
      }

      void rebuildWorldMesh() {
         const auto vertices = world_.buildMesh();
         worldMesh_.upload(std::span<const MeshVertex>{vertices}, etherblocks::BufferUsage::DynamicDraw);
         worldMeshDirty_ = false;
      }

      void render() {
         renderer_.clear({0.2f, 0.3f, 0.3f, 1.0f}, etherblocks::ClearBuffer::Color | etherblocks::ClearBuffer::Depth);

         worldMaterial_.shader().set("uView", camera_.createViewMatrix());
         worldMaterial_.shader().set("uProjection", createProjectionMatrix());
         worldMaterial_.shader().set("uUseOverrideColor", false);
         renderer_.draw(worldMesh_, worldMaterial_);

         if (selectedBlock_)
            drawSelectionBox(selectedBlock_->voxel);

         renderer_.disable(etherblocks::RenderFeature::DepthTest);
         crosshair_.draw(renderer_);
         renderer_.enable(etherblocks::RenderFeature::DepthTest);
      }

      [[nodiscard]] glm::mat4 createProjectionMatrix() const {
         const auto aspectRatio =
             screenSize_.y > 0 ? static_cast<float>(screenSize_.x) / static_cast<float>(screenSize_.y) : 1.0f;
         return glm::perspective(glm::radians(camera_.zoom()), aspectRatio, 0.1f, 100.0f);
      }

      void drawSelectionBox(glm::ivec3 position) {
         const auto vertices = buildSelectionBox(position);
         selectionMesh_.upload(std::span<const MeshVertex>{vertices}, etherblocks::BufferUsage::DynamicDraw);
         worldMaterial_.shader().set("uUseOverrideColor", true);
         worldMaterial_.shader().set("uOverrideColor", glm::vec4{1.0f});
         renderer_.setLineWidth(2.0f);
         renderer_.draw(selectionMesh_, worldMaterial_, etherblocks::Primitive::Lines);
      }

      etherblocks::Window window_;
      etherblocks::Renderer renderer_;
      etherblocks::FreeCamera camera_;
      FrameClock clock_;
      glm::ivec2 screenSize_{kWindowSize};
      World<16> world_;
      etherblocks::Mesh worldMesh_;
      etherblocks::Mesh selectionMesh_;
      etherblocks::Material worldMaterial_;
      etherblocks::Texture worldTexture_;
      CrosshairRenderer crosshair_;
      std::optional<RayHit> selectedBlock_;
      bool worldMeshDirty_{true};
   };

} // namespace

// The process boundary catches both standard and unknown exceptions below.
// NOLINTNEXTLINE(bugprone-exception-escape)
int main() {
   try {
      Application app;
      app.run();
   } catch (const std::exception& error) {
      std::cerr << error.what() << '\n';
      return 1;
   } catch (...) {
      std::cerr << "Unknown fatal error\n";
      return 1;
   }
}
