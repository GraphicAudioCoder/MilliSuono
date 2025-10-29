#pragma once
#include "Port.hpp"
#include <string>
#include <unordered_map>
#include <vector>

namespace ms {
/**
 * @brief Structure representing a parameter.
 *
 * Contains the parameter's name and its value.
 */
struct Param {
  std::string name;   ///< The name of the parameter.
  ControlValue value; ///< The value of the parameter.
};

class Node {
public:
  /**
   * @brief Constructor to initialize a Node.
   * @param id The unique identifier for the Node.
   */
  Node(const std::string &id) : id(id) {}

  /// @brief Retrieves the unique identifier (id) of the node.
  /// @return const std::string& The node's id.
  const std::string &getId() const { return id; }

  /// @brief Retrieves the list of parameters (Param) for the node.
  /// @return const std::vector<Param>& The collection of parameters.
  const std::vector<Param> &getParams() const { return params; }

  /**
   * @brief Sets a new list of parameters for the node.
   * @param newParams The new collection of parameters to be assigned.
   */
  void setParams(const std::vector<Param> &newParams) { params = newParams; }

private:
  const std::string id;      ///< The node's unique identifier.
  std::vector<Param> params; ///< The node's collection of parameters.
};

} // namespace ms
