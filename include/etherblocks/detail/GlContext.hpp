
#ifndef ETHERBLOCKS_DETAIL_GLCONTEXT_HPP
#define ETHERBLOCKS_DETAIL_GLCONTEXT_HPP

namespace etherblocks::detail {
   class GlContext {
   public:
      static GlContext& instance();

      static void load();

      GlContext(const GlContext&) = delete;
      GlContext& operator=(const GlContext&) = delete;

      GlContext(GlContext&&) noexcept = delete;
      GlContext& operator=(GlContext&&) noexcept = delete;

      ~GlContext();

   private:
      GlContext();
   };
} // namespace etherblocks::detail

#endif // ETHERBLOCKS_DETAIL_GLCONTEXT_HPP
