#include <etherblocks/engine/graphics/detail/GlObject.hpp>
#include <utility>

namespace etherblocks::engine::graphics::detail {

   GlObject::GlObject(GlObject&& other) noexcept
       : id_(std::exchange(other.id_, 0)) {}

   GlObject& GlObject::moveAssign(GlObject&& other) noexcept {
      if (this != &other) {
         id_ = std::exchange(other.id_, 0);
      }
      return *this;
   }

} // namespace etherblocks::engine::graphics::detail
