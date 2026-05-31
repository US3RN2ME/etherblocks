#ifndef ETHERBLOCKS_VERTEXARRAY_HPP
#define ETHERBLOCKS_VERTEXARRAY_HPP

#include <cstddef>
#include <etherblocks/detail/GlObject.hpp>

namespace etherblocks {

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

   class VertexArray : private detail::GlObject {
   public:
      VertexArray();
      VertexArray(VertexArray&& other) noexcept;
      VertexArray& operator=(VertexArray&& other) noexcept;
      ~VertexArray();

      void bind() const noexcept;
      static void unbind() noexcept;

      void enableAttribute(unsigned int index) const noexcept;
      void disableAttribute(unsigned int index) const noexcept;
      void setAttribute(unsigned int index, int size, VertexAttributeType type, bool normalized, std::size_t stride,
                        std::size_t offset) const;

   private:
      void reset() noexcept;
   };

} // namespace etherblocks

#endif // ETHERBLOCKS_VERTEXARRAY_HPP
