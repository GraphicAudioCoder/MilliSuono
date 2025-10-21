#include "core/Node.hpp"
#include "core/GraphManager.hpp"

namespace ms {

const float *Node::getPhysicalInput(int channelIndex) const {
  if (graphManager_) {
    return graphManager_->getPhysicalInput(channelIndex);
  }
  return nullptr;
}

} // namespace ms
