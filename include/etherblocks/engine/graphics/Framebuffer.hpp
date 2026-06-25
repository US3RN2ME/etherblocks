#ifndef ETHERBLOCKS_ENGINE_GRAPHICS_FRAMEBUFFER_HPP
#define ETHERBLOCKS_ENGINE_GRAPHICS_FRAMEBUFFER_HPP

#include <etherblocks/engine/graphics/detail/GlObject.hpp>

namespace etherblocks::engine::graphics {

   /**
    * @brief RAII wrapper for an OpenGL framebuffer object.
    */
   class Framebuffer : private detail::GlObject {
   public:
      /**
       * @brief Create a framebuffer object.
       */
      Framebuffer();

      /**
       * @brief Move construct a framebuffer.
       *
       * @param other Framebuffer to move from.
       */
      Framebuffer(Framebuffer&& other) noexcept;

      /**
       * @brief Move assign a framebuffer.
       *
       * @param other Framebuffer to move from.
       *
       * @return This framebuffer.
       */
      Framebuffer& operator=(Framebuffer&& other) noexcept;

      /**
       * @brief Destroy the framebuffer object.
       */
      ~Framebuffer();

      /**
       * @brief Bind this framebuffer as the active framebuffer.
       */
      void bind() const noexcept;

      /**
       * @brief Bind the default framebuffer.
       */
      static void bindDefault() noexcept;

   private:
      void reset() noexcept;
   };

} // namespace etherblocks::engine::graphics

#endif // ETHERBLOCKS_ENGINE_GRAPHICS_FRAMEBUFFER_HPP
