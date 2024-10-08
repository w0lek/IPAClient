#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

enum Role { PATH, SEGMENT, OTHER };

/**
 * Helper structure for Path Information, containing parsed data for a specific
 * path.
 */
struct PathInfo {
  int index = -1;
  std::string start;
  std::string end;
  std::string slack;
  std::string id() const { return start + ":" + end; }
  bool isValid() const { return (index != -1); }
};

struct Line {
  std::string line;
  Role role;
  bool isMultiColumn = true;
};

struct Element {
  std::vector<Line> lines;

  int currentRole() {
    if (!lines.empty()) {
      return lines[0].role;
    }
    return OTHER;
  }
};
using ElementPtr = std::shared_ptr<Element>;

struct Group {
  Group() { getNextCurrentElement(); }
  std::vector<ElementPtr> elements;
  bool isPath() { return pathInfo.isValid(); }

  void getNextCurrentElement() {
    currentElement = std::make_shared<Element>();
    elements.push_back(currentElement);
  }

  ElementPtr currentElement;
  PathInfo pathInfo;
};
using GroupPtr = std::shared_ptr<Group>;

/**
 * @brief Parser for the Critical Path Report generated by VPR.
 *
 * This parser is designed to process the Critical Path Report output generated
 * by VPR (Versatile Place and Route) tool.
 */
class NCriticalPathReportParser {
 public:
  static std::vector<GroupPtr> parseReport(
      const std::vector<std::string>& lines);
  static void parseMetaData(const std::vector<std::string>& lines,
                            std::map<int, std::pair<int, int>>& metadata);
};
