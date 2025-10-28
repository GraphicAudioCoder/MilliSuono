#pragma once
#include <string>
#include <variant>
namespace ms {

/**
 * @brief Enumeration representing the available port types.
 */
enum class PortType { Audio, Control, Event };

/**
 * @brief Alias representing a control value.
 *
 * Can be a value of type float, int, bool, or string.
 */
using ControlValue = std::variant<float, int, bool, std::string>;

/**
 * @brief Structure representing an event.
 *
 * Contains information about the event type, the associated value,
 * and the sample offset where it occurs.
 */
struct Event {
  std::string type;   ///< The type of the event.
  ControlValue value; ///< The value associated with the event.
  int sampleOffset;   ///< The sample offset where the event occurs.

  /**
   * @brief Constructor to initialize an event.
   * @param type The type of the event.
   * @param value The value associated with the event.
   * @param sampleOffset The sample offset where the event occurs.
   */
  Event(const std::string &type, const ControlValue &value, int sampleOffset)
      : type(type), value(value), sampleOffset(sampleOffset) {}
};

/**
 * @brief Structure representing a port.
 *
 * Contains the port name and its type.
 */
struct Port {
  std::string name; ///< The name of the port.
  PortType type;    ///< The type of the port.

  /**
   * @brief Constructor to initialize a port.
   * @param name The name of the port.
   * @param type The type of the port.
   */
  Port(const std::string &name, PortType type) : name(name), type(type) {}
};

} // namespace ms
