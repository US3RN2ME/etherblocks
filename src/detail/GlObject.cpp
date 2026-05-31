#include <etherblocks/detail/GlObject.hpp>
#include <utility>

namespace etherblocks::detail {

   GlObject::GlObject(GlObject&& other) noexcept
       : id_(std::exchange(other.id_, 0)) {}

   GlObject& GlObject::moveAssign(GlObject&& other) noexcept {
      if (this != &other) {
         id_ = std::exchange(other.id_, 0);
      }
      return *this;
   }

} // namespace etherblocks::detail
