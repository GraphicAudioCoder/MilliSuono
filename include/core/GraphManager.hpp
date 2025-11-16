#pragma once 
#include "Node.hpp"
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <mutex>
#include <atomic>

/**
 * @file GraphManager.hpp
 * @brief Manages the creation, connection, and processing of nodes in the MilliSuono graph.
 *
 * GraphManager is responsible for node instantiation, connection management, buffer allocation,
 * and orchestrating audio, control, and event processing across the entire graph. It ensures thread safety
 * for dynamic modifications and real-time operations.
 */

namespace ms {

/**
 * @brief Smart pointer type for Node objects.
 *
 * Used to manage the lifetime and ownership of nodes within the graph.
 */
using NodePtr = std::shared_ptr<Node>;

/**
 * @brief Represents a directed connection between two nodes in the graph.
 *
 * A Connection links an output port of one node to an input port of another node,
 * specifying the source and destination node IDs and port names.
 */
struct Connection {
  /** The ID of the source node. */
  std::string fromNodeId;
  /** The ID of the destination node. */
  std::string toNodeId;
  /** The name of the output port on the source node. */
  std::string fromPortName;
  /** The name of the input port on the destination node. */
  std::string toPortName;
};

class GraphManager {
public:
  /** 
   * @brief Constructs a GraphManager instance.
   */
  GraphManager();

  /** 
   * @brief Destructs the GraphManager instance.
   */
  ~GraphManager();

  /** 
   * Creates a new node with the given ID and adds it to the graph.
   * @param id The unique identifier for the node.
   * @param node The node object to add to the graph.
   * @return NodePtr to the added node.
   */
  NodePtr createNode(const std::string& id, NodePtr node);

  /**
   * @brief Removes a node from the graph by its ID.
  bool removeNode(const std::string& id);
  
private:

  /** 
   * Allocates audio, control, and event buffers for all nodes in the graph. 
   * Ensures that each node has the necessary resources for processing.
   */
  void allocateBuffers();

  /** 
   * Sorts nodes based on their dependencies to ensure correct processing order. 
   * Nodes are ordered so that all inputs are processed before their outputs.
   */
  void sortNodes();
  
  /** 
   * Allocates audio, control, and event buffers for a specific node identified by nodeId. 
   * Ensures that the node has the necessary resources for processing.
   */
  void allocateBuffersForNode(const std::string& nodeId);

  /** 
   * Maps node names (strings) to node objects (pointers). 
   * Allows quick access to a node by its name.
   */
  std::unordered_map<std::string, NodePtr> nodes_; 

  /** 
   * Vector of ordered nodes. Used to process nodes in a specific order 
   * (e.g., for sequential audio processing).
   */
  std::vector<NodePtr> orderedNodes_;

  /** 
   * List of connections between nodes. Represents how nodes are connected 
   * in the audio graph.
   */
  std::vector<Connection> connections_;

  /** 
   * Maps node names to their output audio buffers (matrices of floats). Each node 
   * can have one or more audio channels, each with its own data.
   */
  std::unordered_map<std::string, std::vector<std::vector<float>>> audioBuffers_;

  /** 
   * Maps node names to their output control parameters (e.g., volume, frequency). 
   * Each node can have multiple controls, identified by name.
   */
  std::unordered_map<std::string, std::unordered_map<std::string, ControlValue>> controlValues_;

  /** 
   * Maps node names to their output event buffers. Each node can receive different types 
   * of events (e.g., triggers, messages), each stored in a vector.
   */
  std::unordered_map<std::string, std::unordered_map<std::string, std::vector<Event>>> eventBuffers_;

  /** 
   * Sample rate for audio processing. Determines how many samples per second 
   * are processed in the graph.
   */
  int sampleRate_ = 44100;   

  /** 
   * Block size for audio processing. Determines how many samples are processed 
   * in each block.
   */
  int blockSize_ = 512;

  /** 
   * Flag indicating whether the graph has been prepared (buffers allocated, 
   * nodes sorted, etc.) and is ready for processing.
   */
  bool isPrepared_ = false;

  /** 
   * Mutex for thread-safe access to the graph structure. Ensures that 
   * modifications to the graph are safe in a multi-threaded environment.
   */
  mutable std::mutex graphMutex_;
  
  /** 
   * Atomic flag indicating whether buffer reallocation is needed. 
   * Used to signal when buffers should be reallocated due to changes 
   * in the graph.
   */
  std::atomic<bool> needsBufferReallocation_{false};

};
} // namespace ms
