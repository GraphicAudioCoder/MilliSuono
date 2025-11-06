#pragma once
#include "Port.hpp"
#include <string>
#include <unordered_map>
#include <vector>

/**
 * @file Node.hpp
 * @brief Defines the Node and Parameter structures for the MilliSuono system.
 *
 * It provides the core data strctures for representing processing units and
 * their configurable parameters within the MilliSuono framework.
 */

namespace ms {

/**
 * @brief Represents a named parameter of a Node.
 *
 * A parameter stores a name and a corresponding ControlValue.
 * Parameters can represent any configurable property such as gain, frequency,
 * or mode.
 */
struct Param {
  /** The unique name identifying the parameter. */
  std::string name;

  /** The current value of the parameter. */
  ControlValue value;

  /**
   * @brief Constructs a Param with a given name and value.
   * @param paramName The name of the parameter.
   * @param paramValue The value of the parameter.
   */
  Param(const std::string &paramName, const ControlValue &paramValue)
      : name(paramName), value(paramValue) {}
};

/**
 * @brief Represents a processing unit in the MilliSuono graph.
 *
 * A Node defines a functional unit (e.g. an oscillator, filter, or mixer)
 * with a unique identifier and a set of configurable parameters.
 * Nodes can be connected via Ports to form complex audio processing graphs.
 */
class Node {
public:
  /**
   * @brief Constructs a Node with a given identifier.
   * @param id The unique string identifier for the Node.
   */
  Node(const std::string &id) : id_(id) {}

  /**
   * @brief Virtual destructor for proper cleanup in derived classes.
   */
  virtual ~Node() = default;

  /**
   * @brief Returns the unique identifier of the Node.
   * @return The Node's identifier string.
   */
  const std::string &getId() const { return id_; }

  /**
   * @brief Returns the list of parameters associated with the Node (read-only).
   * @return A const reference to the vector of Params.
   */
  const std::vector<Param> &getParams() const { return params_; }

  /**
   * @brief Returns the list of parameters associated with the Node (mutable).
   * @return A const reference to the vector of Params.
   */
  std::vector<Param> getParams() { return params_; }

  /**
   * @brief Retrieves a parameter value by name.
   * @param name The name of the parameter to retrieve.
   * @return A pointer to the ControlValue if found, nullptr otherwise.
   */
  const ControlValue *getParam(const std::string &name) const {
    for (const auto &param : params_) {
      if (param.name == name) {
        return &param.value;
      }
    }
    return nullptr;
  }

  /**
   * @brief Sets the parameters of the Node.
   * @param newParams A vector of Params to set for the Node.
   */
  void setParams(const std::vector<Param> &newParams) { params_ = newParams; }

  /**
   * @brief Sets a parameter value by name.
   * @param name The name of the parameter to set.
   * @param value The new value to assign to the parameter.
   * @return True if the parameter was found and set, false otherwise.
   */
  bool setParam(const std::string &name, const ControlValue &value) {
    for (auto &param : params_) {
      if (param.name == name) {
        param.value = value;
        return true;
      }
    }
    return false;
  }

  /**
   * @brief Gets the fade-in duration in milliseconds.
   * @return The fade-in duration in milliseconds.
   */
  float getFadeInDuration() const { return fadeInDurationMs_; }

  /**
   * @brief Sets fade-in duration in milliseconds.
   * @param durationMs The fade-in duration in milliseconds (0 = disabled,
   * default = 50ms).
   */
  void setFadeInDuration(float durationMs) {
    fadeInDurationMs_ = durationMs;
    updateFadeInSamples();
  }

  /**
   * @brief Resets the fade-in effect to start from the beginning
   * (useful when re-activating a node).
   */
  void resetFadeIn() {
    currentFadeInSample_ = 0;
    fadeInActive_ = (fadeInDurationMs_ > 0.0f);
  }

  /**
   * @brief Returns the list of input ports for the Node.
   * @return A const reference to the vector of input Ports.
   */
  const std::vector<Port> &getInputPorts() const { return inputPorts_; }

  /**
   * @brief Returns the list of output ports for the Node.
   * @return A const reference to the vector of output Ports.
   */
  const std::vector<Port> &getOutputPorts() const { return outputPorts_; }

  /**
   * @brief Prepares the Node for processing.
   * This function initializes the Node with the given sample rate and block
   * size.
   * @param sampleRate The sample rate in Hz.
   * @param blockSize The block size in samples.
   */
  virtual void prepare(int sampleRate, int blockSize) {
    sampleRate_ = sampleRate;
    blockSize_ = blockSize;
    updateFadeInSamples();
    currentFadeInSample_ = 0;
    fadeInActive_ = (fadeInDurationMs_ > 0.0f);
  }

  /**
   * @brief Processes audio data for the Node.
   * This is a pure virtual function that must be implemented by subclasses.
   * @param inputs An array of input audio buffers.
   * @param outputs An array of output audio buffers.
   * @param nFrames The number of frames to process.
   */
  virtual void process(const float *const *inputsm float **outputs, int nFrames) = 0;

  /**
   * @brief Processes control data for the Node.
   * Subclasses can override this to handle control data.
   * @param inputControls A map of input control values.
   * @param outputControls A map to store output control values.
   */
  virtual void processControl(
      const std::unordered_map<std::string, ControlValue> &inputControls,
      std::unordered_map<std::string, ControlValue> &outputControls) {}
  /**
   * @brief Processes events for the Node.
   * Subclasses can override this to handle events.
   * @param inputEvents A map of input events.
   * @param outputEvents A map to store output events.
   */
  virtual void processEvent(
      const std::unordered_map<std::string, Event> &inputEvents,
      std::unordered_map<std::string, Event> &outputEvents) {}

protected:
  /**
   * @brief Applies fade-in envelope to an audio buffer
   * Subclasses should call this at the end of process() on their output buffers
   * @param buffer The audio buffer to apply fade-in to.
   * @param nFrames The number of frames in the buffer.
   */
  void applyFadeIn(float *buffer, int nFrames);

  /**
   * @brief Adds an input port to the Node.
   * @param name The name of the input port.
   * @param type The type of the input port.
   */
  void addInputPort(const std::string &name, PortType type) {
    inputPorts_.push_back(Port(name, type));
  }

  /**
   * @brief Adds an output port to the Node.
   * @param name The name of the output port.
   * @param type The type of the output port.
   */
  void addOutputPort(const std::string &name, PortType type) {
    outputPorts_.push_back(Port(name, type));
  }

  /**
   * @brief Get physical audio input from hardware
   * For nodes that need direct hardware access (e.g., audio input nodes)
   * @param channelIndex The Physical channel index to read from
   * @return Pointer to the float buffer of the physical input channel or
   * nullptr
   */
  const float *getPhysicalInput(int channelIndex) const;

  /** The list of input ports for the Node. */
  std::vector<Port> inputPorts_;

  /** The list of output ports for the Node. */
  std::vector<Port> outputPorts_;

  /** The sample rate at which the Node operates. */
  int sampleRate_ = 44100;
  /** The block size for processing audio samples. */
  int blockSize_ = 512;

private:
  /** The unique identifier of the Node. */
  const std::string id_;

  /** The list of parameters associated with the Node. */
  std::vector<Param> params_;

  /** The duration of the fade-in effect in milliseconds. */
  float fadeInDurationMs_ = 50.0f;
  /** The number of samples over which the fade-in effect occurs. */
  int fadeInSamples_ = 0;
  /** The current sample index within the fade-in effect. */
  int currentFadeInSample_ = 0;
  /** Flag indicating whether the fade-in effect is active. */
  bool fadeInActive_ = false;

  /**
   * @brief Updates the number of samples for the fade-in effect based on the
   * current duration and sample rate.
   */
  void updateFadeInSamples() {
    fadeInSamples_ = static_cast<int>((fadeInDurationMs_ / 1000.0f) *
                                      static_cast<float>(sampleRate_));
  }
};

} // namespace ms
