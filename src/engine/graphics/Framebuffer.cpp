#include <etherblocks/engine/graphics/Framebuffer.hpp>
#include <etherblocks/system/Logger.hpp>
#include <glad/glad.h>
#include <stdexcept>
#include <utility>

namespace etherblocks::engine::graphics {

   Framebuffer::Framebuffer() {
      glGenFramebuffers(1, &id_);
      if (id_ == 0) {
         system::log(system::LogLevel::Error, "glGenFramebuffers failed");
         throw std::runtime_error{"glGenFramebuffers failed"};
      }
   }

   Framebuffer::Framebuffer(Framebuffer&& other) noexcept
       : detail::GlObject(std::move(other)) {}

   Framebuffer& Framebuffer::operator=(Framebuffer&& other) noexcept {
      if (this != &other) {
         reset();
         moveAssign(std::move(other));
      }
      return *this;
   }

   Framebuffer::~Framebuffer() {
      reset();
   }

   void Framebuffer::bind() const noexcept {
      glBindFramebuffer(GL_FRAMEBUFFER, id_);
   }

   void Framebuffer::bindDefault() noexcept {
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
   }

   void Framebuffer::reset() noexcept {
      if (id_ != 0) {
         glDeleteFramebuffers(1, &id_);
         id_ = 0;
      }
   }

} // namespace etherblocks::engine::graphics
