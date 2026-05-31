#include <etherblocks/engine/graphics/Buffer.hpp>
#include <glad/glad.h>
#include <limits>
#include <stdexcept>
#include <utility>

namespace etherblocks::engine::graphics {

   namespace {
      [[nodiscard]] constexpr GLenum toGl(BufferTarget target) noexcept {
         switch (target) {
            case BufferTarget::Array:
               return GL_ARRAY_BUFFER;
            case BufferTarget::ElementArray:
               return GL_ELEMENT_ARRAY_BUFFER;
            case BufferTarget::Uniform:
               return GL_UNIFORM_BUFFER;
            case BufferTarget::ShaderStorage:
               return GL_SHADER_STORAGE_BUFFER;
            case BufferTarget::CopyRead:
               return GL_COPY_READ_BUFFER;
            case BufferTarget::CopyWrite:
               return GL_COPY_WRITE_BUFFER;
            case BufferTarget::PixelPack:
               return GL_PIXEL_PACK_BUFFER;
            case BufferTarget::PixelUnpack:
               return GL_PIXEL_UNPACK_BUFFER;
         }
         return GL_INVALID_ENUM;
      }

      [[nodiscard]] constexpr GLenum toGl(BufferUsage usage) noexcept {
         switch (usage) {
            case BufferUsage::StreamDraw:
               return GL_STREAM_DRAW;
            case BufferUsage::StreamRead:
               return GL_STREAM_READ;
            case BufferUsage::StreamCopy:
               return GL_STREAM_COPY;
            case BufferUsage::StaticDraw:
               return GL_STATIC_DRAW;
            case BufferUsage::StaticRead:
               return GL_STATIC_READ;
            case BufferUsage::StaticCopy:
               return GL_STATIC_COPY;
            case BufferUsage::DynamicDraw:
               return GL_DYNAMIC_DRAW;
            case BufferUsage::DynamicRead:
               return GL_DYNAMIC_READ;
            case BufferUsage::DynamicCopy:
               return GL_DYNAMIC_COPY;
         }
         return GL_INVALID_ENUM;
      }

      [[nodiscard]] GLsizeiptr toGlSize(std::size_t size) {
         if (size > static_cast<std::size_t>(std::numeric_limits<GLsizeiptr>::max())) {
            throw std::overflow_error{"Buffer size exceeds the OpenGL limit"};
         }
         return static_cast<GLsizeiptr>(size);
      }

      [[nodiscard]] GLintptr toGlOffset(std::size_t offset) {
         if (offset > static_cast<std::size_t>(std::numeric_limits<GLintptr>::max())) {
            throw std::overflow_error{"Buffer offset exceeds the OpenGL limit"};
         }
         return static_cast<GLintptr>(offset);
      }
   } // namespace

   Buffer::Buffer(BufferTarget target)
       : target_(target) {
      glGenBuffers(1, &id_);
   }

   Buffer::Buffer(Buffer&& other) noexcept
       : target_(std::exchange(other.target_, BufferTarget{})) {
      moveAssign(std::move(other));
   }

   Buffer& Buffer::operator=(Buffer&& other) noexcept {
      if (this != &other) {
         const auto target = std::exchange(other.target_, BufferTarget{});
         reset();
         moveAssign(std::move(other));
         target_ = target;
      }
      return *this;
   }

   Buffer::~Buffer() {
      reset();
   }

   void Buffer::bind() const noexcept {
      glBindBuffer(toGl(target_), id_);
   }

   void Buffer::unbind(BufferTarget target) noexcept {
      glBindBuffer(toGl(target), 0);
   }

   void Buffer::allocate(std::size_t size, const void* data, BufferUsage usage) const {
      bind();
      glBufferData(toGl(target_), toGlSize(size), data, toGl(usage));
   }

   void Buffer::update(std::size_t offset, std::size_t size, const void* data) const {
      bind();
      glBufferSubData(toGl(target_), toGlOffset(offset), toGlSize(size), data);
   }

   BufferTarget Buffer::target() const noexcept {
      return target_;
   }

   void Buffer::reset() noexcept {
      if (id_ != 0) {
         glDeleteBuffers(1, &id_);
         id_ = 0;
      }
   }

} // namespace etherblocks::engine::graphics
