#include "Node.hpp"

namespace ms {

void Node::applyFadeIn(float *buffer, int nFrames) {
  if (!fadeInActive_) {
    return;
  }

  for (int i = 0; i < nFrames; ++i) {
    if (currentFadeInSample_ <  fadeInSamples_) {
      float fadeInGain = static_cast<float>(currentFadeInSample_) / fadeInSamples_;
      buffer[i] *= fadeInGain;
      currentFadeInSample_++;
    } else {
      fadeInActive_ = false;
      break;
    }
  }
}

const float *Node::getPhysicalInput(int channelIndex) const {
  // Placeholder implementation
  // In the real implementation, this would interface with the audio hardware
  return nullptr;
}

}
