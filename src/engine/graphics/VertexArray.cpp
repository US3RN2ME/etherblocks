#include <etherblocks/engine/graphics/VertexArray.hpp>
#include <glad/glad.h>
#include <limits>
#include <stdexcept>
#include <utility>

namespace etherblocks::engine::graphics {

   namespace {
      [[nodiscard]] constexpr GLenum toGl(VertexAttributeType type) noexcept {
         switch (type) {
            case VertexAttributeType::Byte:
               return GL_BYTE;
            case VertexAttributeType::UnsignedByte:
               return GL_UNSIGNED_BYTE;
            case VertexAttributeType::Short:
               return GL_SHORT;
            case VertexAttributeType::UnsignedShort:
               return GL_UNSIGNED_SHORT;
            case VertexAttributeType::Int:
               return GL_INT;
            case VertexAttributeType::UnsignedInt:
               return GL_UNSIGNED_INT;
            case VertexAttributeType::Float:
               return GL_FLOAT;
            case VertexAttributeType::Double:
               return GL_DOUBLE;
         }
         return GL_INVALID_ENUM;
      }
   } // namespace

   VertexArray::VertexArray() {
      glGenVertexArrays(1, &id_);
   }

   VertexArray::VertexArray(VertexArray&& other) noexcept
       : detail::GlObject(std::move(other)) {}

   VertexArray& VertexArray::operator=(VertexArray&& other) noexcept {
      if (this != &other) {
         reset();
         moveAssign(std::move(other));
      }
      return *this;
   }

   VertexArray::~VertexArray() {
      reset();
   }

   void VertexArray::bind() const noexcept {
      glBindVertexArray(id_);
   }

   void VertexArray::unbind() noexcept {
      glBindVertexArray(0);
   }

   void VertexArray::enableAttribute(unsigned int index) const noexcept {
      bind();
      glEnableVertexAttribArray(index);
   }

   void VertexArray::disableAttribute(unsigned int index) const noexcept {
      bind();
      glDisableVertexAttribArray(index);
   }

   void VertexArray::setAttribute(unsigned int index, int size, VertexAttributeType type, bool normalized, std::size_t stride,
                                  std::size_t offset) const {
      if (stride > static_cast<std::size_t>(std::numeric_limits<GLsizei>::max())) {
         throw std::overflow_error{"Vertex stride exceeds the OpenGL limit"};
      }
      bind();
      glVertexAttribPointer(index, size, toGl(type), normalized ? GL_TRUE : GL_FALSE, static_cast<GLsizei>(stride),
                            reinterpret_cast<const void*>(offset));
   }

   void VertexArray::reset() noexcept {
      if (id_ != 0) {
         glDeleteVertexArrays(1, &id_);
         id_ = 0;
      }
   }

} // namespace etherblocks::engine::graphics
