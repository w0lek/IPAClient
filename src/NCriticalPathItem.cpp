#include "NCriticalPathItem.h"

NCriticalPathItem::NCriticalPathItem() {
  m_itemData.resize(Column::END);
  m_itemData[Column::DATA] = "";
  m_itemData[Column::VAL1] = "";
  m_itemData[Column::VAL2] = "";
}

NCriticalPathItem::NCriticalPathItem(const QString& data, const QString& val1,
                                     const QString& val2, Type type, int id,
                                     int pathId, bool isSelectable)
    : m_id(id),
      m_dataOrig(data),
      m_pathId(pathId),
      m_type(type),
      m_isSelectable(isSelectable) {
  m_itemData.resize(Column::END);
  m_itemData[Column::DATA] = data;
  m_itemData[Column::VAL1] = val1;
  m_itemData[Column::VAL2] = val2;

#ifdef DEBUG_NCRITICAL_PATH_ITEM_PROPERTIES
  m_itemData[Column::TYPE] = type;
  m_itemData[Column::ID] = id;
  m_itemData[Column::PATH_ID] = pathId;
  m_itemData[Column::IS_SELECTABLE] = isSelectable;
#endif

  if (isPath()) {
    QList<QString> d = data.split('\n');
    for (const QString& e : std::as_const(d)) {
      if (e.startsWith("Startpoint")) {
        m_startPointLine = e;
      } else if (e.startsWith("Endpoint")) {
        m_endPointLine = e;
      }
    }
  }
}

NCriticalPathItem::~NCriticalPathItem() { deleteChildItems(); }

void NCriticalPathItem::deleteChildItems() {
  if (!m_childItems.isEmpty()) {
    qDeleteAll(m_childItems);
    m_childItems.clear();
  }
}

void NCriticalPathItem::appendChild(NCriticalPathItem* item) {
  m_childItems.append(item);
}

NCriticalPathItem* NCriticalPathItem::child(int row) {
  if ((row < 0) || (row >= m_childItems.size())) {
    return nullptr;
  }
  return m_childItems.at(row);
}

int NCriticalPathItem::childCount() const { return m_childItems.count(); }

int NCriticalPathItem::columnCount() const { return m_itemData.count(); }

QVariant NCriticalPathItem::data(int column) const {
  if ((column < 0) || (column >= m_itemData.size())) {
    return QVariant();
  }
  return m_itemData.at(column);
}

int NCriticalPathItem::row() const {
  if (m_parentItem) {
    return m_parentItem->m_childItems.indexOf(
        const_cast<NCriticalPathItem*>(this));
  }

  return 0;
}

bool NCriticalPathItem::limitLineCharsNum(std::size_t lineCharsMaxNum) {
  bool processData = false;
  if (m_appliedLineCharsMaxNumOpt) {
    if (m_appliedLineCharsMaxNumOpt.value() < lineCharsMaxNum) {
      processData = true;
    }
  }

  if (m_dataOrig.size() > lineCharsMaxNum) {
    processData = true;
  }

  if (processData) {
    QString dataMod;
    dataMod.reserve(m_dataOrig.size() + m_dataOrig.size() / lineCharsMaxNum);

    std::size_t count = 0;
    for (QChar ch : m_dataOrig) {
      dataMod.append(ch);
      ++count;
      if (ch == '\n') {
        count = 0;
      } else if (count == lineCharsMaxNum) {
        dataMod.append('\n');
        count = 0;
      }
    }
    if (dataMod.endsWith('\n')) {
      dataMod.chop(1);
    }

    m_itemData[Column::DATA] = dataMod;
    m_appliedLineCharsMaxNumOpt = lineCharsMaxNum;
  }

  return processData;
}

