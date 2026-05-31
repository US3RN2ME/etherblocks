#ifndef ETHERBLOCKS_ENGINE_GRAPHICS_MESH_HPP
#define ETHERBLOCKS_ENGINE_GRAPHICS_MESH_HPP

#include <cstddef>
#include <etherblocks/engine/graphics/Buffer.hpp>
#include <etherblocks/engine/graphics/VertexArray.hpp>
#include <span>

namespace etherblocks::engine::graphics {

   struct VertexAttribute {
      unsigned int index{};
      int componentCount{};
      VertexAttributeType type{VertexAttributeType::Float};
      bool normalized{};
      std::size_t offset{};
   };

   class Mesh {
   public:
      Mesh(std::size_t vertexStride, std::span<const VertexAttribute> attributes);

      template <typename T>
      void upload(std::span<const T> vertices, BufferUsage usage = BufferUsage::StaticDraw) {
         vertexBuffer_.upload(vertices, usage);
         vertexCount_ = vertices.size();
      }

      void setVertexCount(std::size_t vertexCount) noexcept;
      [[nodiscard]] std::size_t vertexCount() const noexcept;

   private:
      friend class Renderer;

      void bind() const noexcept;

      VertexArray vertexArray_;
      Buffer vertexBuffer_{BufferTarget::Array};
      std::size_t vertexCount_{};
   };

} // namespace etherblocks::engine::graphics

#endif // ETHERBLOCKS_ENGINE_GRAPHICS_MESH_HPP
