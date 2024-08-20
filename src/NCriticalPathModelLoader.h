#pragma once

#include <QThread>
#include <map>
#include <memory>

#include "NCriticalPathReportParser.h"

class NCriticalPathItem;

struct ItemsHelperStruct {
  std::vector<std::pair<NCriticalPathItem*, NCriticalPathItem*>> items;
  std::map<QString, int> inputNodes;
  std::map<QString, int> outputNodes;
};
using ItemsHelperStructPtr = std::shared_ptr<ItemsHelperStruct>;

class NCriticalPathModelLoader : public QThread {
  Q_OBJECT
 public:
  explicit NCriticalPathModelLoader(QString&& rawData)
      : QThread(nullptr), m_rawData(rawData) {}
  ~NCriticalPathModelLoader() {}

 signals:
  void itemsReady(const ItemsHelperStructPtr&);

 protected:
  void run() override final;

 private:
  QString m_rawData;

  void createItems(const std::vector<GroupPtr>& groups,
                   const std::map<int, std::pair<int, int>>& metadata);
  std::tuple<QString, QString, QString> extractRow(QString) const;
};

Q_DECLARE_METATYPE(ItemsHelperStructPtr)
