#ifndef ETHERBLOCKS_ENGINE_GRAPHICS_BUFFER_HPP
#define ETHERBLOCKS_ENGINE_GRAPHICS_BUFFER_HPP

#include <cstddef>
#include <etherblocks/engine/graphics/detail/GlObject.hpp>
#include <span>

namespace etherblocks::engine::graphics {

   enum class BufferTarget {
      Array,
      ElementArray,
      Uniform,
      ShaderStorage,
      CopyRead,
      CopyWrite,
      PixelPack,
      PixelUnpack,
   };

   enum class BufferUsage {
      StreamDraw,
      StreamRead,
      StreamCopy,
      StaticDraw,
      StaticRead,
      StaticCopy,
      DynamicDraw,
      DynamicRead,
      DynamicCopy,
   };

   class Buffer : private detail::GlObject {
   public:
      explicit Buffer(BufferTarget target);
      Buffer(Buffer&& other) noexcept;
      Buffer& operator=(Buffer&& other) noexcept;
      ~Buffer();

      void bind() const noexcept;
      static void unbind(BufferTarget target) noexcept;

      void allocate(std::size_t size, const void* data, BufferUsage usage) const;
      void update(std::size_t offset, std::size_t size, const void* data) const;

      template <typename T>
      void upload(std::span<const T> data, BufferUsage usage) const {
         allocate(data.size_bytes(), data.data(), usage);
      }

      [[nodiscard]] BufferTarget target() const noexcept;

   private:
      void reset() noexcept;

      BufferTarget target_{};
   };

} // namespace etherblocks::engine::graphics

#endif // ETHERBLOCKS_ENGINE_GRAPHICS_BUFFER_HPP
