#include <etherblocks/engine/graphics/Mesh.hpp>

namespace etherblocks::engine::graphics {

   Mesh::Mesh(std::size_t vertexStride, std::span<const VertexAttribute> attributes) {
      vertexArray_.bind();
      vertexBuffer_.bind();
      for (const auto& attribute : attributes) {
         vertexArray_.setAttribute(attribute.index, attribute.componentCount, attribute.type, attribute.normalized,
                                   vertexStride, attribute.offset);
         vertexArray_.enableAttribute(attribute.index);
      }
      VertexArray::unbind();
   }

   std::size_t Mesh::vertexCount() const noexcept {
      return vertexCount_;
   }

   void Mesh::setVertexCount(std::size_t vertexCount) noexcept {
      vertexCount_ = vertexCount;
   }

   void Mesh::bind() const noexcept {
      vertexArray_.bind();
   }

} // namespace etherblocks::engine::graphics
