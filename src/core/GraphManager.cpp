#include "core/GraphManager.hpp"
#include <algorithm>
#include <cstring>
#include <iostream>
#include <string>

namespace ms {

GraphManager::GraphManager() {}
GraphManager::~GraphManager() { clear(); }

NodePtr GraphManager::createNode(const std::string &id, NodePtr node) {
  std::lock_guard<std::mutex> lock(graphMutex_);

  if (nodes_.find(id) != nodes_.end()) {
    std::cerr << "Node with ID " << id << " already exists." << std::endl;
    return nullptr;
  }

  nodes_[id] = node;
  orderedNodes_.push_back(node);

  if (isPrepared_) {
    node->setGraphManager(this);
    node->prepare(sampleRate_, blockSize_);
    allocateBuffersForNode(id);
    std::cout << "Node " << id << " created and prepared." << std::endl;
  }

  return node;
}

bool GraphManager::removeNode(const std::string &id) {
  std::lock_guard<std::mutex> lock(graphMutex_);

  auto it = nodes_.find(id);
  if (it == nodes_.end()) {
    std::cerr << "Node with ID " << id << " does not exist." << std::endl;
    return false;
  }

  disconnectAll(id);

  orderedNodes_.erase(
      std::remove_if(orderedNodes_.begin(), orderedNodes_.end(),
                     [&id](const NodePtr &node) { return node->id == id; }),
      orderedNodes_.end());

  nodes_.erase(it);
  audioBuffers_.erase(id);
  controlValues_.erase(id);
  eventBuffers_.erase(id);

  std::cout << "Node " << id << " removed." << std::endl;
  return true;
}

NodePtr GraphManager::getNode(const std::string &id) const {
  auto it = nodes_.find(id);
  if (it != nodes_.end()) {
    return it->second;
  }
  return nullptr;
}

void GraphManager::connect(const std::string &fromId,
                           const std::string &fromPort, const std::string &toId,
                           const std::string &toPort) {
  std::lock_guard<std::mutex> lock(graphMutex_);

  if (nodes_.find(fromId) == nodes_.end()) {
    std::cerr << "Error: Source node '" << fromId << "' not found!"
              << std::endl;
    return;
  }
  if (nodes_.find(toId) == nodes_.end()) {
    std::cerr << "Error: Destination node '" << toId << "' not found!"
              << std::endl;
    return;
  }

  connections_.push_back({fromId, fromPort, toId, toPort});

  std::cout << "Connected: " << fromId << "." << fromPort << " -> " << toId
            << "." << toPort << std::endl;
}

bool GraphManager::disconnect(const std::string &fromId,
                              const std::string &fromPort,
                              const std::string &toId,
                              const std::string &toPort) {
  std::lock_guard<std::mutex> lock(graphMutex_);

  auto it = std::remove_if(
      connections_.begin(), connections_.end(), [&](const Connection &conn) {
        return conn.fromNodeId == fromId && conn.fromPortName == fromPort &&
               conn.toNodeId == toId && conn.toPortName == toPort;
      });

  if (it != connections_.end()) {
    connections_.erase(it, connections_.end());
    std::cout << "Disconnected: " << fromId << "." << fromPort << " -> " << toId
              << "." << toPort << std::endl;
    return true;
  }

  std::cerr << "Warning: Connection not found!" << std::endl;
  return false;
}

void GraphManager::disconnectAll(const std::string &nodeId) {
  connections_.erase(std::remove_if(connections_.begin(), connections_.end(),
                                    [&nodeId](const Connection &conn) {
                                      return conn.fromNodeId == nodeId ||
                                             conn.toNodeId == nodeId;
                                    }),
                     connections_.end());
  std::cout << "All connections for node '" << nodeId << "' removed."
            << std::endl;
}

void GraphManager::clear() {
  nodes_.clear();
  orderedNodes_.clear();
  connections_.clear();
  audioBuffers_.clear();
  controlValues_.clear();
  eventBuffers_.clear();
  std::cout << "Graph cleared." << std::endl;
}

void GraphManager::prepare(int sampleRate, int blockSize) {
  std::lock_guard<std::mutex> lock(graphMutex_);

  sampleRate_ = sampleRate;
  blockSize_ = blockSize;

  for (auto &node : orderedNodes_) {
    node->setGraphManager(this);
    node->prepare(sampleRate, blockSize);
  }

  allocateBuffers();
  isPrepared_ = true;

  std::cout << "GraphManager prepared: " << sampleRate << "Hz, " << blockSize
            << " samples/block" << std::endl;
}

void GraphManager::allocateBuffers() {
  audioBuffers_.clear();
  controlValues_.clear();
  eventBuffers_.clear();

  for (auto &[id, node] : nodes_) {
    allocateBuffersForNode(id);
  }
}

void GraphManager::allocateBuffersForNode(const std::string &nodeId) {
  auto it = nodes_.find(nodeId);
  if (it == nodes_.end())
    return;

  auto &node = it->second;
  std::vector<std::vector<float>> buffers;

  for (const auto &port : node->getOutputPorts()) {
    if (port.type == PortType::Audio) {
      buffers.push_back(std::vector<float>(blockSize_, 0.0f));
    }
  }

  if (!buffers.empty()) {
    audioBuffers_[nodeId] = std::move(buffers);
  }

  controlValues_[nodeId] = std::unordered_map<std::string, ControlValue>();
  eventBuffers_[nodeId] = std::unordered_map<std::string, std::vector<Event>>();
}

} // namespace ms
