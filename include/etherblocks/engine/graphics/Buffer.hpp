#ifndef ETHERBLOCKS_ENGINE_GRAPHICS_BUFFER_HPP
#define ETHERBLOCKS_ENGINE_GRAPHICS_BUFFER_HPP

#include <cstddef>
#include <etherblocks/engine/graphics/detail/GlObject.hpp>
#include <span>

namespace etherblocks::engine::graphics {

   /**
    * @brief OpenGL buffer binding target.
    */
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

   /**
    * @brief OpenGL buffer usage hint.
    */
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

   /**
    * @brief RAII wrapper for an OpenGL buffer object.
    */
   class Buffer : private detail::GlObject {
   public:
      /**
       * @brief Create a buffer for a binding target.
       *
       * @param target Buffer binding target.
       */
      explicit Buffer(BufferTarget target);

      /**
       * @brief Move construct a buffer.
       *
       * @param other Buffer to move from.
       */
      Buffer(Buffer&& other) noexcept;

      /**
       * @brief Move assign a buffer.
       *
       * @param other Buffer to move from.
       *
       * @return This buffer.
       */
      Buffer& operator=(Buffer&& other) noexcept;

      /**
       * @brief Destroy the OpenGL buffer object.
       */
      ~Buffer();

      /**
       * @brief Bind this buffer to its target.
       */
      void bind() const noexcept;

      /**
       * @brief Unbind the current buffer from a target.
       *
       * @param target Buffer binding target to clear.
       */
      static void unbind(BufferTarget target) noexcept;

      /**
       * @brief Allocate or replace the complete buffer data store.
       *
       * @param size Size in bytes.
       * @param data Optional source data.
       * @param usage Buffer usage hint.
       */
      void allocate(std::size_t size, const void* data, BufferUsage usage) const;

      /**
       * @brief Update a byte range inside the existing buffer data store.
       *
       * @param offset Destination offset in bytes.
       * @param size Size in bytes.
       * @param data Source data.
       */
      void update(std::size_t offset, std::size_t size, const void* data) const;

      /**
       * @brief Upload a typed contiguous span to the buffer.
       *
       * @tparam T Element type.
       *
       * @param data Elements to upload.
       * @param usage Buffer usage hint.
       */
      template <typename T>
      void upload(std::span<const T> data, BufferUsage usage) const {
         allocate(data.size_bytes(), data.data(), usage);
      }

      /**
       * @brief Return the buffer binding target.
       *
       * @return Buffer target.
       */
      [[nodiscard]] BufferTarget target() const noexcept;

   private:
      void reset() noexcept;

      BufferTarget target_{};
   };

} // namespace etherblocks::engine::graphics

#endif // ETHERBLOCKS_ENGINE_GRAPHICS_BUFFER_HPP
