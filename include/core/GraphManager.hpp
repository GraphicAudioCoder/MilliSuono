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
   * @brief Removes a node and all its connections from the graph by its ID.
   * @param id The unique identifier of the node to remove.
   * @return true if the node was successfully removed, false otherwise.
   */
  bool removeNode(const std::string& id);

  /** 
   * Retrieves a node by its ID.
   * @param id The unique identifier of the node.
   * @return NodePtr to the requested node, or nullptr if not found.
   */
  NodePtr getNode(const std::string& id) const;

  /** 
   * Connects the output port of one node to the input port of another node.
   * @param fromId The ID of the source node.
   * @param fromPort The name of the output port on the source node.
   * @param toId The ID of the destination node.
   * @param toPort The name of the input port on the destination node.
   */
  void connect(const std::string &fromId, const std::string &fromPort,
               const std::string &toId, const std::string &toPort);

  /** 
   * Disconnects the output port of one node from the input port of another node.
   * @param fromId The ID of the source node.
   * @param fromPort The name of the output port on the source node.
   * @param toId The ID of the destination node.
   * @param toPort The name of the input port on the destination node.
   * @return true if the disconnection was successful, false otherwise.
   */
  bool disconnect(const std::string &fromId, const std::string &fromPort,
                  const std::string &toId, const std::string &toPort);
  
  /**
   * @brief Disconnects all connections to and from the specified node.
   * @param nodeId The ID of the node to disconnect.
   */
  void disconnectAll(const std::string &nodeId);

  /** 
   * Clears all nodes and connections from the graph.
   */
  void clear();

  /** 
   * Prepares the graph for processing by allocating necessary buffers and sorting nodes.
   * @param sampleRate The sample rate for audio processing.
   * @param blockSize The block size for audio processing.
   */
  void prepare(int sampleRate, int blockSize);

  /** 
   * Processes the graph for a given number of frames.
   * @param nFrames The number of frames to process.
   */
  void process(int nFrames);

  /** 
   * Retrieves the output audio buffer of a node by its ID and output index.
   * @param nodeId The unique identifier of the node.
   * @param outputIndex The index of the output channel (default is 0).
   * @return Pointer to the float buffer of the requested output channel.
   */
  const float *getNodeOutput(const std::string &nodeId, int outputIndex = 0) const;

  /** 
   * Sets the physical audio input buffer for a specific channel index.
   * @param channelIndex The index of the physical input channel.
   * @param data Pointer to the float buffer containing the input data.
   * @param nFrames The number of frames in the input buffer.
   */
  void setPhysicalInput(int channelIndex, const float* data, int nFrames);

  /** 
   * Retrieves the physical audio input buffer for a specific channel index.
   * @param channelIndex The index of the physical input channel.
   * @return Pointer to the float buffer of the physical input channel.
   */
  const float* getPhysicalInput(int channelIndex) const;

  /** 
   * Gets the number of physical audio input channels.
   * @return The number of physical input channels.
   */
  int getNumPhysicalInputs() const { return physicalInputBuffers_.size(); }

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
