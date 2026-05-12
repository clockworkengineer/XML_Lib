#pragma once

#include "interface/IAction.hpp"

#include <string>
#include <vector>

namespace XML_Lib {

/// @brief Recording stub of IAction for unit-testing tree traversal.
///
/// Every `on*` callback appends an entry to the `events` log so tests can
/// assert which callbacks were fired and in what order.
struct Mock_IAction : IAction
{
  struct Event {
    std::string name;   ///< e.g. "onElement", "onContent"
    bool isMutable;     ///< true when the mutable overload was called
  };

  void onNode(Node &) override          { events.push_back({ "onNode",          true  }); }
  void onNode(const Node &) override    { events.push_back({ "onNode",          false }); }
  void onCDATA(Node &) override         { events.push_back({ "onCDATA",         true  }); }
  void onCDATA(const Node &) override   { events.push_back({ "onCDATA",         false }); }
  void onComment(Node &) override       { events.push_back({ "onComment",       true  }); }
  void onComment(const Node &) override { events.push_back({ "onComment",       false }); }
  void onContent(Node &) override       { events.push_back({ "onContent",       true  }); }
  void onContent(const Node &) override { events.push_back({ "onContent",       false }); }
  void onDeclaration(Node &) override         { events.push_back({ "onDeclaration",   true  }); }
  void onDeclaration(const Node &) override   { events.push_back({ "onDeclaration",   false }); }
  void onDTD(Node &) override           { events.push_back({ "onDTD",           true  }); }
  void onDTD(const Node &) override     { events.push_back({ "onDTD",           false }); }
  void onElement(Node &) override       { events.push_back({ "onElement",       true  }); }
  void onElement(const Node &) override { events.push_back({ "onElement",       false }); }
  void onEntityReference(Node &) override         { events.push_back({ "onEntityReference", true  }); }
  void onEntityReference(const Node &) override   { events.push_back({ "onEntityReference", false }); }
  void onPI(Node &) override            { events.push_back({ "onPI",            true  }); }
  void onPI(const Node &) override      { events.push_back({ "onPI",            false }); }
  void onProlog(Node &) override        { events.push_back({ "onProlog",        true  }); }
  void onProlog(const Node &) override  { events.push_back({ "onProlog",        false }); }
  void onRoot(Node &) override          { events.push_back({ "onRoot",          true  }); }
  void onRoot(const Node &) override    { events.push_back({ "onRoot",          false }); }
  void onSelf(Node &) override          { events.push_back({ "onSelf",          true  }); }
  void onSelf(const Node &) override    { events.push_back({ "onSelf",          false }); }

  /// Reset the event log between test sections.
  void clear() { events.clear(); }

  /// Return true if @p eventName appears in the event log.
  [[nodiscard]] bool fired(const std::string &eventName) const
  {
    for (const auto &e : events) { if (e.name == eventName) return true; }
    return false;
  }

  std::vector<Event> events;
};

}// namespace XML_Lib
