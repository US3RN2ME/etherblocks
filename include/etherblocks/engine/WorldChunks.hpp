#ifndef ETHERBLOCKS_ENGINE_WORLDCHUNKS_HPP
#define ETHERBLOCKS_ENGINE_WORLDCHUNKS_HPP

#include <array>
#include <etherblocks/engine/WorldRendering.hpp>
#include <etherblocks/engine/graphics/Material.hpp>
#include <etherblocks/engine/graphics/Mesh.hpp>
#include <etherblocks/engine/graphics/Renderer.hpp>
#include <etherblocks/engine/graphics/Texture.hpp>
#include <etherblocks/game/World.hpp>
#include <glm/glm.hpp>
#include <vector>

namespace etherblocks::engine {

   /**
    * @brief Chunked renderer cache for voxel world meshes.
    */
   class WorldChunks {
   public:
      /**
       * @brief Construct chunk storage for a world.
       *
       * @param world World to render.
       * @param chunkSize Chunk width and depth in voxels.
       */
      WorldChunks(game::World& world, int chunkSize);

      /**
       * @brief Draw visible chunks, rebuilding dirty meshes on demand.
       *
       * @param renderer Renderer used for drawing.
       * @param material Material shared by world meshes.
       * @param textures Textures for renderable block types.
       * @param cameraPosition Current camera position.
       * @param viewDistance Maximum visible distance.
       */
      void draw(const graphics::Renderer& renderer, graphics::Material& material,
                const std::array<const graphics::Texture*, 5>& textures, glm::vec3 cameraPosition, float viewDistance);

      /**
       * @brief Rebuild the chunk grid with a new chunk size.
       *
       * @param chunkSize New chunk width and depth in voxels.
       */
      void reset(int chunkSize);

      /**
       * @brief Mark the chunk containing a block as needing rebuild.
       *
       * @param blockPosition Edited block position.
       */
      void markDirty(glm::ivec3 blockPosition);

      /**
       * @brief Mark every chunk as needing rebuild.
       */
      void markAllDirty();

   private:
      struct Chunk {
         explicit Chunk(glm::ivec2 origin);

         glm::ivec2 origin{};
         std::array<graphics::Mesh, 5> meshes;
         bool dirty{true};
      };

      [[nodiscard]] bool contains(glm::ivec2 chunk) const noexcept;
      [[nodiscard]] glm::ivec2 chunkForBlock(glm::ivec3 position) const noexcept;
      [[nodiscard]] glm::ivec3 chunkMin(const Chunk& chunk) const noexcept;
      [[nodiscard]] glm::ivec3 chunkMax(const Chunk& chunk) const noexcept;
      [[nodiscard]] bool isVisible(const Chunk& chunk, glm::vec3 cameraPosition, float viewDistance) const noexcept;
      void rebuildGrid();
      void markDirty(glm::ivec2 chunk);
      void rebuild(Chunk& chunk);

      game::World& world_;
      int chunkSize_{};
      glm::ivec2 gridSize_{};
      std::vector<Chunk> chunks_;
   };

} // namespace etherblocks::engine

#endif // ETHERBLOCKS_ENGINE_WORLDCHUNKS_HPP
