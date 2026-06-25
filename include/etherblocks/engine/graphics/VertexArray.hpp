#ifndef ETHERBLOCKS_ENGINE_GRAPHICS_VERTEXARRAY_HPP
#define ETHERBLOCKS_ENGINE_GRAPHICS_VERTEXARRAY_HPP

#include <cstddef>
#include <etherblocks/engine/graphics/detail/GlObject.hpp>

namespace etherblocks::engine::graphics {

   /**
    * @brief Scalar type used by a vertex attribute.
    */
   enum class VertexAttributeType {
      Byte,
      UnsignedByte,
      Short,
      UnsignedShort,
      Int,
      UnsignedInt,
      Float,
      Double,
   };

   /**
    * @brief RAII wrapper for an OpenGL vertex array object.
    */
   class VertexArray : private detail::GlObject {
   public:
      /**
       * @brief Create a vertex array object.
       */
      VertexArray();

      /**
       * @brief Move construct a vertex array.
       *
       * @param other Vertex array to move from.
       */
      VertexArray(VertexArray&& other) noexcept;

      /**
       * @brief Move assign a vertex array.
       *
       * @param other Vertex array to move from.
       *
       * @return This vertex array.
       */
      VertexArray& operator=(VertexArray&& other) noexcept;

      /**
       * @brief Destroy the vertex array object.
       */
      ~VertexArray();

      /**
       * @brief Bind this vertex array.
       */
      void bind() const noexcept;

      /**
       * @brief Unbind the active vertex array.
       */
      static void unbind() noexcept;

      /**
       * @brief Enable a vertex attribute slot.
       *
       * @param index Attribute location.
       */
      void enableAttribute(unsigned int index) const noexcept;

      /**
       * @brief Disable a vertex attribute slot.
       *
       * @param index Attribute location.
       */
      void disableAttribute(unsigned int index) const noexcept;

      /**
       * @brief Configure a vertex attribute pointer.
       *
       * @param index Attribute location.
       * @param size Component count.
       * @param type Component type.
       * @param normalized Whether fixed-point values are normalized.
       * @param stride Vertex stride in bytes.
       * @param offset Attribute byte offset.
       */
      void setAttribute(unsigned int index, int size, VertexAttributeType type, bool normalized, std::size_t stride,
                        std::size_t offset) const;

   private:
      void reset() noexcept;
   };

} // namespace etherblocks::engine::graphics

#endif // ETHERBLOCKS_ENGINE_GRAPHICS_VERTEXARRAY_HPP
