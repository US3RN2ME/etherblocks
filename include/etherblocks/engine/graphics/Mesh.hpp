#ifndef ETHERBLOCKS_ENGINE_GRAPHICS_MESH_HPP
#define ETHERBLOCKS_ENGINE_GRAPHICS_MESH_HPP

#include <cstddef>
#include <etherblocks/engine/graphics/Buffer.hpp>
#include <etherblocks/engine/graphics/VertexArray.hpp>
#include <span>

namespace etherblocks::engine::graphics {

   /**
    * @brief Vertex attribute description relative to a vertex stride.
    */
   struct VertexAttribute {
      /**
       * @brief Shader attribute location.
       */
      unsigned int index{};

      /**
       * @brief Number of scalar components in the attribute.
       */
      int componentCount{};

      /**
       * @brief Scalar component type.
       */
      VertexAttributeType type{VertexAttributeType::Float};

      /**
       * @brief Whether fixed-point values should be normalized.
       */
      bool normalized{};

      /**
       * @brief Byte offset from the start of the vertex.
       */
      std::size_t offset{};
   };

   /**
    * @brief Vertex array and vertex buffer pair for non-indexed geometry.
    */
   class Mesh {
   public:
      /**
       * @brief Construct a mesh with a vertex layout.
       *
       * @param vertexStride Size of one vertex in bytes.
       * @param attributes Vertex attribute descriptions.
       */
      Mesh(std::size_t vertexStride, std::span<const VertexAttribute> attributes);

      /**
       * @brief Upload typed vertices and update the draw vertex count.
       *
       * @tparam T Vertex type.
       *
       * @param vertices Vertices to upload.
       * @param usage Buffer usage hint.
       */
      template <typename T>
      void upload(std::span<const T> vertices, BufferUsage usage = BufferUsage::StaticDraw) {
         vertexBuffer_.upload(vertices, usage);
         vertexCount_ = vertices.size();
      }

      /**
       * @brief Override the number of vertices drawn from the mesh.
       *
       * @param vertexCount Vertex count.
       */
      void setVertexCount(std::size_t vertexCount) noexcept;

      /**
       * @brief Return the current draw vertex count.
       *
       * @return Vertex count.
       */
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
