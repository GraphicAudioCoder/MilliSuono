#pragma once
#include <string>
#include <variant>

namespace ms {

/* Port data types */
enum class PortType {
  Audio,   // Continuos audio signal
  Control, // Single control value (updated per block)
  Event    // Discrete event/message
};

using ControlValue = std::variant<float, int, bool, std::string>;

/* Event message structure */
struct Event {
  std::string type;  // Event type (trigger, note_on, custom, etc.)
  ControlValue data; // Optional event data
  int sampleOffset;  // Sample position within the block (0 to blockSize-1)

  Event(const std::string &type, ControlValue value = 0.0f,
        int sampleOffset = 0)
      : type(type), data(value), sampleOffset(sampleOffset) {}
};

/* Port descriptor */
struct Port {
  std::string name;
  PortType type;

  Port(const std::string &name, PortType type) : name(name), type(type) {}
};

} // namespace ms
