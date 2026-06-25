#include <algorithm>
#include <etherblocks/engine/WorldChunks.hpp>
#include <span>

namespace etherblocks::engine {

   namespace {
      [[nodiscard]] int ceilDiv(int value, int divisor) noexcept {
         return (value + divisor - 1) / divisor;
      }

      [[nodiscard]] std::size_t chunkIndex(glm::ivec2 chunk, glm::ivec2 chunkGridSize) noexcept {
         return static_cast<std::size_t>(chunk.x) +
                static_cast<std::size_t>(chunkGridSize.x) * static_cast<std::size_t>(chunk.y);
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
   } // namespace

   WorldChunks::Chunk::Chunk(glm::ivec2 origin)
       : origin(origin)
       , meshes{graphics::Mesh{sizeof(MeshVertex), meshLayout()}, graphics::Mesh{sizeof(MeshVertex), meshLayout()},
                graphics::Mesh{sizeof(MeshVertex), meshLayout()}, graphics::Mesh{sizeof(MeshVertex), meshLayout()},
                graphics::Mesh{sizeof(MeshVertex), meshLayout()}} {}

   WorldChunks::WorldChunks(game::World& world, int chunkSize)
       : world_(world)
       , chunkSize_(std::max(chunkSize, 1)) {
      rebuildGrid();
   }

   void WorldChunks::reset(int chunkSize) {
      chunkSize_ = std::max(chunkSize, 1);
      rebuildGrid();
   }

   void WorldChunks::rebuildGrid() {
      const auto size = world_.size();
      gridSize_ = {ceilDiv(size.x, chunkSize_), ceilDiv(size.z, chunkSize_)};
      chunks_.clear();
      chunks_.reserve(static_cast<std::size_t>(gridSize_.x * gridSize_.y));
      for (auto chunkZ = 0; chunkZ < gridSize_.y; ++chunkZ) {
         for (auto chunkX = 0; chunkX < gridSize_.x; ++chunkX) {
            chunks_.emplace_back(glm::ivec2{chunkX * chunkSize_, chunkZ * chunkSize_});
         }
      }
   }

   void WorldChunks::draw(const graphics::Renderer& renderer, graphics::Material& material,
                          const std::array<const graphics::Texture*, 5>& textures, glm::vec3 cameraPosition,
                          float viewDistance) {
      for (auto& chunk : chunks_) {
         if (!isVisible(chunk, cameraPosition, viewDistance)) {
            continue;
         }
         if (chunk.dirty) {
            rebuild(chunk);
            chunk.dirty = false;
         }
         for (auto mesh = 0; mesh < static_cast<int>(chunk.meshes.size()); ++mesh) {
            if (chunk.meshes[static_cast<std::size_t>(mesh)].vertexCount() == 0) {
               continue;
            }
            material.setTexture(*textures[static_cast<std::size_t>(mesh)]);
            renderer.draw(chunk.meshes[static_cast<std::size_t>(mesh)], material);
         }
      }
   }

   void WorldChunks::markDirty(glm::ivec3 blockPosition) {
      const auto chunk = chunkForBlock(blockPosition);
      if (!contains(chunk)) {
         return;
      }
      chunks_[chunkIndex(chunk, gridSize_)].dirty = true;

      const auto localX = blockPosition.x - chunk.x * chunkSize_;
      const auto localZ = blockPosition.z - chunk.y * chunkSize_;
      if (localX == 0) {
         markDirty(chunk + glm::ivec2{-1, 0});
      }
      if (localX == chunkSize_ - 1) {
         markDirty(chunk + glm::ivec2{1, 0});
      }
      if (localZ == 0) {
         markDirty(chunk + glm::ivec2{0, -1});
      }
      if (localZ == chunkSize_ - 1) {
         markDirty(chunk + glm::ivec2{0, 1});
      }
   }

   void WorldChunks::markAllDirty() {
      for (auto& chunk : chunks_) {
         chunk.dirty = true;
      }
   }

   bool WorldChunks::contains(glm::ivec2 chunk) const noexcept {
      return chunk.x >= 0 && chunk.y >= 0 && chunk.x < gridSize_.x && chunk.y < gridSize_.y;
   }

   glm::ivec2 WorldChunks::chunkForBlock(glm::ivec3 position) const noexcept {
      return {position.x / chunkSize_, position.z / chunkSize_};
   }

   glm::ivec3 WorldChunks::chunkMin(const Chunk& chunk) const noexcept {
      return {chunk.origin.x, 0, chunk.origin.y};
   }

   glm::ivec3 WorldChunks::chunkMax(const Chunk& chunk) const noexcept {
      const auto size = world_.size();
      return {std::min(chunk.origin.x + chunkSize_, size.x), size.y, std::min(chunk.origin.y + chunkSize_, size.z)};
   }

   bool WorldChunks::isVisible(const Chunk& chunk, glm::vec3 cameraPosition, float viewDistance) const noexcept {
      const auto min = glm::vec3(chunkMin(chunk)) - glm::vec3{0.5f};
      const auto max = glm::vec3(chunkMax(chunk)) + glm::vec3{0.5f};
      const auto closest = glm::clamp(cameraPosition, min, max);
      return glm::length(closest - cameraPosition) <= viewDistance;
   }

   void WorldChunks::markDirty(glm::ivec2 chunk) {
      if (!contains(chunk)) {
         return;
      }
      chunks_[chunkIndex(chunk, gridSize_)].dirty = true;
   }

   void WorldChunks::rebuild(Chunk& chunk) {
      const auto min = chunkMin(chunk);
      const auto max = chunkMax(chunk);
      for (auto mesh = 0; mesh < static_cast<int>(chunk.meshes.size()); ++mesh) {
         auto vertices = buildWorldMesh(world_, blockTypeForMesh(mesh), min, max);
         chunk.meshes[static_cast<std::size_t>(mesh)].upload(std::span<const MeshVertex>{vertices},
                                                             graphics::BufferUsage::DynamicDraw);
      }
   }

} // namespace etherblocks::engine
