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
  /** The unique name identifying the paraemter. */
  std::string name;
  /** The current value of the parameter. */
  ControlValue value;

  /** Constuctor for convenience. */
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

protected:
private:
  /** The unique identifier of the Node. */
  const std::string id_;
  /** The list of parameters associated with the Node. */
  std::vector<Param> params_;
  /**
   * @brief The list of input ports for the Node.
   */
  std::vector<Port> inputPorts_;
  /**
   * @brief The list of output ports for the Node.
   */
  std::vector<Port> outputPorts_;
};

} // namespace ms
