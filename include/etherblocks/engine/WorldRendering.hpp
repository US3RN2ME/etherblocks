#ifndef ETHERBLOCKS_ENGINE_WORLDRENDERING_HPP
#define ETHERBLOCKS_ENGINE_WORLDRENDERING_HPP

#include <array>
#include <etherblocks/engine/graphics/Material.hpp>
#include <etherblocks/engine/graphics/Mesh.hpp>
#include <etherblocks/engine/graphics/Renderer.hpp>
#include <etherblocks/engine/graphics/VertexArray.hpp>
#include <etherblocks/game/World.hpp>
#include <glm/glm.hpp>
#include <span>
#include <vector>

namespace etherblocks::engine {

   /**
    * @brief Vertex format used by generated world meshes.
    */
   struct MeshVertex {
      /**
       * @brief Vertex position in world space.
       */
      glm::vec3 position{};

      /**
       * @brief Texture coordinate.
       */
      glm::vec2 textureCoordinate{};

      /**
       * @brief Vertex normal in world space.
       */
      glm::vec3 normal{0.0f, 1.0f, 0.0f};
   };

   /**
    * @brief Return the vertex attribute layout for MeshVertex.
    *
    * @return Mesh vertex layout.
    */
   [[nodiscard]] std::span<const graphics::VertexAttribute> meshLayout() noexcept;

   /**
    * @brief Build visible faces for one block type across the whole world.
    *
    * @param world Source voxel world.
    * @param blockType Block type to mesh.
    *
    * @return Generated triangle vertices.
    */
   [[nodiscard]] std::vector<MeshVertex> buildWorldMesh(const game::World& world, game::BlockType blockType);

   /**
    * @brief Build visible faces for one block type inside a world-space region.
    *
    * @param world Source voxel world.
    * @param blockType Block type to mesh.
    * @param minInclusive Inclusive minimum voxel coordinate.
    * @param maxExclusive Exclusive maximum voxel coordinate.
    *
    * @return Generated triangle vertices.
    */
   [[nodiscard]] std::vector<MeshVertex> buildWorldMesh(const game::World& world, game::BlockType blockType,
                                                        glm::ivec3 minInclusive, glm::ivec3 maxExclusive);

   /**
    * @brief Build line vertices for a selected voxel outline.
    *
    * @param position Selected voxel coordinate.
    *
    * @return Selection box line vertices.
    */
   [[nodiscard]] std::array<MeshVertex, 24> buildSelectionBox(glm::ivec3 position);

   /**
    * @brief Renderer for the centered UI crosshair.
    */
   class CrosshairRenderer {
   public:
      /**
       * @brief Construct crosshair GPU resources.
       */
      CrosshairRenderer();

      /**
       * @brief Draw the crosshair.
       *
       * @param renderer Renderer used for drawing.
       */
      void draw(const graphics::Renderer& renderer);

   private:
      graphics::Mesh mesh_;
      graphics::Material material_;
   };

} // namespace etherblocks::engine

#endif // ETHERBLOCKS_ENGINE_WORLDRENDERING_HPP
