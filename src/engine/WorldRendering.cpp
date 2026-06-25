#include <algorithm>
#include <cstddef>
#include <etherblocks/engine/WorldRendering.hpp>

namespace etherblocks::engine {

   namespace {
      namespace graphics = etherblocks::engine::graphics;

      constexpr std::array kMeshLayout{
          graphics::VertexAttribute{0, 3, graphics::VertexAttributeType::Float, false, offsetof(MeshVertex, position)},
          graphics::VertexAttribute{1, 2, graphics::VertexAttributeType::Float, false, offsetof(MeshVertex, textureCoordinate)},
          graphics::VertexAttribute{2, 3, graphics::VertexAttributeType::Float, false, offsetof(MeshVertex, normal)},
      };

      struct FaceDefinition {
         float vertices[6][5];
      };

      enum class BlockFace : std::size_t { Back, Front, Left, Right, Bottom, Top };

      constexpr glm::vec4 rectForFace(BlockFace face) noexcept {
         constexpr auto tileSize = 512.0f;
         switch (face) {
            case BlockFace::Back:
               return {tileSize, tileSize, tileSize, tileSize};
            case BlockFace::Front:
               return {0.0f, tileSize, tileSize, tileSize};
            case BlockFace::Left:
               return {0.0f, tileSize * 2.0f, tileSize, tileSize};
            case BlockFace::Right:
               return {tileSize, tileSize * 2.0f, tileSize, tileSize};
            case BlockFace::Bottom:
               return {tileSize, 0.0f, tileSize, tileSize};
            case BlockFace::Top:
               return {0.0f, 0.0f, tileSize, tileSize};
         }
         return {0.0f, tileSize, tileSize, tileSize};
      }

      constexpr glm::vec2 atlasTextureCoordinate(glm::vec2 textureCoordinate, glm::vec4 rect) noexcept {
         constexpr auto textureSize = glm::vec2{1024.0f, 1536.0f};
         const auto bottomLeftRectOrigin = glm::vec2{rect.x, textureSize.y - rect.y - rect.w};
         const auto pixelCoordinate = bottomLeftRectOrigin + textureCoordinate * glm::vec2{rect.z, rect.w};
         return pixelCoordinate / textureSize;
      }

      constexpr glm::vec3 normalForFace(BlockFace face) noexcept {
         switch (face) {
            case BlockFace::Back:
               return {0.0f, 0.0f, -1.0f};
            case BlockFace::Front:
               return {0.0f, 0.0f, 1.0f};
            case BlockFace::Left:
               return {-1.0f, 0.0f, 0.0f};
            case BlockFace::Right:
               return {1.0f, 0.0f, 0.0f};
            case BlockFace::Bottom:
               return {0.0f, -1.0f, 0.0f};
            case BlockFace::Top:
               return {0.0f, 1.0f, 0.0f};
         }
         return {0.0f, 1.0f, 0.0f};
      }

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
          {{{-0.5f, 0.5f, 0.5f, 0, 1},
            {-0.5f, 0.5f, -0.5f, 1, 1},
            {-0.5f, -0.5f, -0.5f, 1, 0},
            {-0.5f, -0.5f, -0.5f, 1, 0},
            {-0.5f, -0.5f, 0.5f, 0, 0},
            {-0.5f, 0.5f, 0.5f, 0, 1}}},
          {{{0.5f, 0.5f, 0.5f, 1, 1},
            {0.5f, 0.5f, -0.5f, 0, 1},
            {0.5f, -0.5f, -0.5f, 0, 0},
            {0.5f, -0.5f, -0.5f, 0, 0},
            {0.5f, -0.5f, 0.5f, 1, 0},
            {0.5f, 0.5f, 0.5f, 1, 1}}},
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

   std::span<const graphics::VertexAttribute> meshLayout() noexcept {
      return kMeshLayout;
   }

   std::vector<MeshVertex> buildWorldMesh(const game::World& world, game::BlockType blockType) {
      return buildWorldMesh(world, blockType, glm::ivec3{0}, world.size());
   }

   std::vector<MeshVertex> buildWorldMesh(const game::World& world, game::BlockType blockType, glm::ivec3 minInclusive,
                                          glm::ivec3 maxExclusive) {
      std::vector<MeshVertex> vertices;
      const auto size = world.size();
      minInclusive = glm::clamp(minInclusive, glm::ivec3{0}, size);
      maxExclusive = glm::clamp(maxExclusive, minInclusive, size);
      const auto regionSize = maxExclusive - minInclusive;
      vertices.reserve(static_cast<std::size_t>(regionSize.x) * static_cast<std::size_t>(regionSize.y) *
                       static_cast<std::size_t>(regionSize.z) * kCubeFaces.size() * 6);

      for (auto y = minInclusive.y; y < maxExclusive.y; ++y) {
         for (auto z = minInclusive.z; z < maxExclusive.z; ++z) {
            for (auto x = minInclusive.x; x < maxExclusive.x; ++x) {
               const glm::ivec3 position{x, y, z};
               if (world.block(position) != blockType) {
                  continue;
               }

               for (std::size_t faceIndex = 0; faceIndex < kCubeFaces.size(); ++faceIndex) {
                  if (world.isSolid(position + kNeighborOffsets[faceIndex])) {
                     continue;
                  }

                  const auto face = static_cast<BlockFace>(faceIndex);
                  for (const auto& vertex : kCubeFaces[faceIndex].vertices) {
                     vertices.push_back({
                         glm::vec3(position) + glm::vec3{vertex[0], vertex[1], vertex[2]},
                         atlasTextureCoordinate({vertex[3], vertex[4]}, rectForFace(face)),
                         normalForFace(face),
                     });
                  }
               }
            }
         }
      }
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

   void CrosshairRenderer::draw(const graphics::Renderer& renderer) {
      material_.shader().set("uType", 0);
      material_.shader().set("uSize", 8.0f);
      material_.shader().set("uColor", glm::vec4{1.0f});
      renderer.draw(mesh_, material_, graphics::Primitive::Points);
   }

} // namespace etherblocks::engine
