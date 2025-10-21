#pragma once
#include "Port.hpp"
#include <string>
#include <unordered_map>
#include <vector>

namespace ms {

/* Node parameter structure */
struct Param {
  std::string name;
  float value;
};

class Node {
public:
  Node(const std::string &id) : id(id) {}
  virtual ~Node() = default;

  /* Non-realtime preparaion */
  virtual void prepare(int sampleRate, int blockSize) {
    sampleRate_ = sampleRate;
    blockSize_ = blockSize;

    updateFadeInSamples();
    currentFadeSample_ = 0;
    fadeInActive_ = (fadeInDurationMs_ > 0.0f);
  }

  /* Set fade-in duration in milliseconds */
  void setFadeInDuration(float durationMs) {
    fadeInDurationMs_ = durationMs;
    updateFadeInSamples();
  }

  /* Get current fade-in duration in milliseconds */
  float getFadeInDuration() const { return fadeInDurationMs_; }

  /* Reset fade-in duration */
  void resetFadeIn() {
    currentFadeSample_ = 0;
    fadeInActive_ = (fadeInDurationMs_ > 0.0f);
  }

  /* Realtime audio processing */
  virtual void process(const float *const *audioInputs, float **audioOutputs,
                       int numFrames) = 0;

  /* Realtime control processing */
  virtual void processControl(
      const std::unordered_map<std::string, ControlValue> &controlInputs,
      std::unordered_map<std::string, ControlValue> &controlOutputs) {}

  /* Realtime event processing */
  virtual void processEvents(
      const std::unordered_map<std::string, std::vector<Event>> &eventInputs,
      std::unordered_map<std::string, std::vector<Event>> &eventOutputs) {}

  /* Get input port descriptors */
  const std::vector<Port> &getInputPorts() const { return inputPorts_; }

  /* Get output port descriptors */
  const std::vector<Port> &getOutputPorts() const { return outputPorts_; }

  const std::string id;
  std::vector<Param> params;

protected:
  /* Apply fade-in to audio buffer */
  void applyFadeIn(float *buffer, int nFrames) {
    if (!fadeInActive_) {
      return;
    }

    for (int i = 0; i < nFrames; ++i) {
      if (currentFadeSample_ < fadeInSamples_) {
        float fadeGain = static_cast<float>(currentFadeSample_) /
                         static_cast<float>(fadeInSamples_);
        buffer[i] *= fadeGain;
        currentFadeSample_++;
      } else {
        fadeInActive_ = false;
        break;
      }
    }
  }

  /* Add output port */
  void addInputPort(const std::string &name, PortType type) {
    inputPorts_.push_back(Port(name, type));
  }

  /* Add output port */
  void addOutputPort(const std::string &name, PortType type) {
    outputPorts_.push_back(Port(name, type));
  }

  /* Get physical input buffer for a given channel index */
  const float *getPhysicalInput(int channelIndex) const;

  std::vector<Port> inputPorts_;
  std::vector<Port> outputPorts_;

  int sampleRate_ = 44100;
  int blockSize_ = 512;

  /* Fade-in state */
  float fadeInDurationMs_ = 50.0f;
  int fadeInSamples_ = 0;
  int currentFadeSample_ = 0;
  bool fadeInActive_ = false;

  class GraphManager *graphManager_ = nullptr;

private:
  /* Update fade-in sample from duration and sample rate */
  void updateFadeInSamples() {
    fadeInSamples_ =
        static_cast<int>((fadeInDurationMs_ / 1000.0f) * sampleRate_);
  }
};

} // namespace ms
