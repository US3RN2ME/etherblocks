#ifndef ETHERBLOCKS_SYSTEM_LOGGER_HPP
#define ETHERBLOCKS_SYSTEM_LOGGER_HPP

#include <cstdint>
#include <functional>
#include <mutex>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace etherblocks::system {

   /**
    * @brief Severity assigned to a log message.
    */
   enum class LogLevel { Debug, Info, Warning, Error };

   /**
    * @brief Immutable log payload delivered to sinks.
    */
   struct LogMessage {
      /**
       * @brief Message severity.
       */
      LogLevel level{};

      /**
       * @brief Message text.
       */
      std::string text;
   };

   /**
    * @brief Thread-safe fan-out logger with removable sinks.
    */
   class Logger {
   public:
      /**
       * @brief Callback that receives log messages.
       *
       * @param message Message to consume.
       */
      using Sink = std::function<void(const LogMessage&)>;

      /**
       * @brief Opaque identifier returned when a sink is registered.
       */
      using SinkId = std::uint64_t;

      /**
       * @brief Construct a logger with no sinks.
       */
      Logger();

      /**
       * @brief Register a sink for future log messages.
       *
       * @param sink Sink callback to store.
       *
       * @return Identifier that can later remove the sink.
       */
      [[nodiscard]] SinkId addSink(Sink sink);

      /**
       * @brief Remove a previously registered sink.
       *
       * @param id Sink identifier returned by addSink.
       */
      void removeSink(SinkId id);

      /**
       * @brief Send a message to all registered sinks.
       *
       * @param level Message severity.
       * @param text Message text.
       */
      void log(LogLevel level, std::string_view text) const noexcept;

   private:
      mutable std::mutex mutex_;
      std::vector<std::pair<SinkId, Sink>> sinks_;
      SinkId nextSinkId_{};
   };

   /**
    * @brief Return the process-wide logger instance.
    *
    * @return Global logger.
    */
   [[nodiscard]] Logger& logger();

   /**
    * @brief Log a message through the process-wide logger.
    *
    * @param level Message severity.
    * @param text Message text.
    */
   void log(LogLevel level, std::string_view text) noexcept;

} // namespace etherblocks::system

#endif // ETHERBLOCKS_SYSTEM_LOGGER_HPP
