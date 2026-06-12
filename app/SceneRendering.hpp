#ifndef APP_SCENERENDERING_HPP
#define APP_SCENERENDERING_HPP

#include <array>
#include <etherblocks/engine/graphics/Material.hpp>
#include <etherblocks/engine/graphics/Mesh.hpp>
#include <etherblocks/engine/graphics/Renderer.hpp>
#include <etherblocks/engine/graphics/VertexArray.hpp>
#include <etherblocks/game/World.hpp>
#include <glm/glm.hpp>
#include <span>
#include <vector>

namespace etherblocks::app {

   struct MeshVertex {
      glm::vec3 position{};
      glm::vec2 textureCoordinate{};
      glm::vec3 normal{0.0f, 1.0f, 0.0f};
   };

   [[nodiscard]] std::span<const engine::graphics::VertexAttribute> meshLayout() noexcept;
   [[nodiscard]] std::vector<MeshVertex> buildWorldMesh(const game::World& world, game::BlockType blockType);
   [[nodiscard]] std::array<MeshVertex, 24> buildSelectionBox(glm::ivec3 position);

   class CrosshairRenderer {
   public:
      CrosshairRenderer();

      void draw(const engine::graphics::Renderer& renderer);

   private:
      engine::graphics::Mesh mesh_;
      engine::graphics::Material material_;
   };

} // namespace etherblocks::app

#endif // APP_SCENERENDERING_HPP
