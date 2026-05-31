#include "SceneRendering.hpp"

#include <cstddef>

namespace etherblocks::app {

   namespace {
      namespace graphics = engine::graphics;

      constexpr std::array kMeshLayout{
          graphics::VertexAttribute{0, 3, graphics::VertexAttributeType::Float, false, offsetof(MeshVertex, position)},
          graphics::VertexAttribute{1, 2, graphics::VertexAttributeType::Float, false, offsetof(MeshVertex, textureCoordinate)},
      };

      struct FaceDefinition {
         float vertices[6][5];
      };

      constexpr std::array<FaceDefinition, 6> kCubeFaces{{
          {{{-0.5f, -0.5f, -0.5f, 0, 0},
            {0.5f, -0.5f, -0.5f, 1, 0},
            {0.5f, 0.5f, -0.5f, 1, 1},
            {0.5f, 0.5f, -0.5f, 1, 1},
            {-0.5f, 0.5f, -0.5f, 0, 1},
            {-0.5f, -0.5f, -0.5f, 0, 0}}},
          {{{-0.5f, -0.5f, 0.5f, 0, 0},
            {0.5f, -0.5f, 0.5f, 1, 0},
            {0.5f, 0.5f, 0.5f, 1, 1},
            {0.5f, 0.5f, 0.5f, 1, 1},
            {-0.5f, 0.5f, 0.5f, 0, 1},
            {-0.5f, -0.5f, 0.5f, 0, 0}}},
          {{{-0.5f, 0.5f, 0.5f, 1, 0},
            {-0.5f, 0.5f, -0.5f, 1, 1},
            {-0.5f, -0.5f, -0.5f, 0, 1},
            {-0.5f, -0.5f, -0.5f, 0, 1},
            {-0.5f, -0.5f, 0.5f, 0, 0},
            {-0.5f, 0.5f, 0.5f, 1, 0}}},
          {{{0.5f, 0.5f, 0.5f, 1, 0},
            {0.5f, 0.5f, -0.5f, 1, 1},
            {0.5f, -0.5f, -0.5f, 0, 1},
            {0.5f, -0.5f, -0.5f, 0, 1},
            {0.5f, -0.5f, 0.5f, 0, 0},
            {0.5f, 0.5f, 0.5f, 1, 0}}},
          {{{-0.5f, -0.5f, -0.5f, 0, 1},
            {0.5f, -0.5f, -0.5f, 1, 1},
            {0.5f, -0.5f, 0.5f, 1, 0},
            {0.5f, -0.5f, 0.5f, 1, 0},
            {-0.5f, -0.5f, 0.5f, 0, 0},
            {-0.5f, -0.5f, -0.5f, 0, 1}}},
          {{{-0.5f, 0.5f, -0.5f, 0, 1},
            {0.5f, 0.5f, -0.5f, 1, 1},
            {0.5f, 0.5f, 0.5f, 1, 0},
            {0.5f, 0.5f, 0.5f, 1, 0},
            {-0.5f, 0.5f, 0.5f, 0, 0},
            {-0.5f, 0.5f, -0.5f, 0, 1}}},
      }};

      constexpr std::array<glm::ivec3, 6> kNeighborOffsets{{
          {0, 0, -1},
          {0, 0, 1},
          {-1, 0, 0},
          {1, 0, 0},
          {0, -1, 0},
          {0, 1, 0},
      }};
   } // namespace

   std::span<const engine::graphics::VertexAttribute> meshLayout() noexcept {
      return kMeshLayout;
   }

   std::vector<MeshVertex> buildWorldMesh(const game::World& world) {
      std::vector<MeshVertex> vertices;
      const auto size = world.size();
      vertices.reserve(static_cast<std::size_t>(size.x) * static_cast<std::size_t>(size.y) * static_cast<std::size_t>(size.z) *
                       kCubeFaces.size() * 6);

      world.forEachBlock([&](glm::ivec3 position, game::BlockType block) {
         if (block == game::BlockType::Empty) {
            return;
         }

         for (std::size_t faceIndex = 0; faceIndex < kCubeFaces.size(); ++faceIndex) {
            if (world.isSolid(position + kNeighborOffsets[faceIndex])) {
               continue;
            }

            for (const auto& vertex : kCubeFaces[faceIndex].vertices) {
               vertices.push_back({
                   glm::vec3(position) + glm::vec3{vertex[0], vertex[1], vertex[2]},
                   {vertex[3], vertex[4]},
               });
            }
         }
      });
      return vertices;
   }

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

   CrosshairRenderer::CrosshairRenderer()
       : mesh_(0, {})
       , material_("./shaders/crosshair.vertex.glsl", "./shaders/crosshair.fragment.glsl") {
      mesh_.setVertexCount(1);
   }

   void CrosshairRenderer::draw(const engine::graphics::Renderer& renderer) {
      material_.shader().set("uType", 0);
      material_.shader().set("uSize", 8.0f);
      material_.shader().set("uColor", glm::vec4{1.0f});
      renderer.draw(mesh_, material_, engine::graphics::Primitive::Points);
   }

} // namespace etherblocks::app
