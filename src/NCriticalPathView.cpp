#include "NCriticalPathView.h"

#include <QMouseEvent>
#include <QScrollBar>
#include <set>

#include "NCriticalPathItem.h"
#include "NCriticalPathItemDelegate.h"
#include "NCriticalPathModel.h"
#include "client/CommConstants.h"

#include <QDebug>

NCriticalPathView::NCriticalPathView(QWidget* parent)
    : QTreeView(parent),
      m_sourceModel(new NCriticalPathModel(this)) {
  setupModels();

  setSelectionMode(QAbstractItemView::MultiSelection);

  setExpandsOnDoubleClick(false);  // will be redirected on single mouse right button press
  setVerticalScrollMode(QTreeView::ScrollPerPixel);
  verticalScrollBar()->setSingleStep(10);

  NCriticalPathItemDelegate* customDelegate =
      new NCriticalPathItemDelegate(this);
  setItemDelegate(customDelegate);
}

void NCriticalPathView::setupModels() {
  setModel(m_sourceModel);
  connect(m_sourceModel, &NCriticalPathModel::cleared, this, &NCriticalPathView::dataCleared);
  connect(m_sourceModel, &NCriticalPathModel::loadFinished, this, &NCriticalPathView::dataLoaded);

  connect(this, &NCriticalPathView::loadFromString, m_sourceModel, &NCriticalPathModel::loadFromString);

  // selectionModel() is null before we set the model, that's why we create the
  // connection after model set
  connect(selectionModel(), &QItemSelectionModel::selectionChanged, this, &NCriticalPathView::handleSelectionChanged);
}

void NCriticalPathView::clear() { m_sourceModel->clear(); }

void NCriticalPathView::mousePressEvent(QMouseEvent* event) {
  QModelIndex index = indexAt(event->pos());
  if (index.isValid()) {
    if (event->button() == Qt::RightButton) {
      if (isExpanded(index)) {
        collapse(index);
      } else {
        expand(index);
      }
      return;
    }
  }

  QTreeView::mousePressEvent(event);
}

void NCriticalPathView::mouseReleaseEvent(QMouseEvent* event) {
  while (!m_pathSourceIndexesToResolveChildrenSelection.empty()) {
    const auto& [sourceIndex, selected] =
        m_pathSourceIndexesToResolveChildrenSelection.takeLast();
    if (sourceIndex.isValid()) {
      updateChildrenSelectionFor(sourceIndex, selected);
    }
  }

  QTreeView::mouseReleaseEvent(event);
}

void NCriticalPathView::scroll(int steps) {
  verticalScrollBar()->setValue(verticalScrollBar()->value() + steps);
}

void NCriticalPathView::handleSelectionChanged(
    const QItemSelection& selected, const QItemSelection& deselected) {
    if (!m_sourceModel) {
        return;
    }

    for (const QModelIndex& index : selected.indexes()) {
      if (index.isValid()) {
        QModelIndex sourceIndex = index;
        if (sourceIndex.column() == 0) {
          NCriticalPathItem* item =
              static_cast<NCriticalPathItem*>(sourceIndex.internalPointer());
          if (item) {
            if (item->isPath()) {
              m_pathSourceIndexesToResolveChildrenSelection.push_back(
                  std::make_pair(sourceIndex, true));
            }
          }
        }
      }
    }

    for (const QModelIndex& index : deselected.indexes()) {
      if (index.isValid()) {
        QModelIndex sourceIndex = index;
        if (sourceIndex.column() == 0) {
          NCriticalPathItem* item =
              static_cast<NCriticalPathItem*>(sourceIndex.internalPointer());
          if (item) {
            if (item->isPath()) {
              m_pathSourceIndexesToResolveChildrenSelection.push_back(
                  std::make_pair(sourceIndex, false));
            }
          }
        }
      }
    }

    if (m_pathSourceIndexesToResolveChildrenSelection.empty()) {
        QString selectedPathElements = getSelectedPathElements();
        emit pathElementSelectionChanged(selectedPathElements,
                                         "selectedPathElements");
        qInfo() << "selectedPathElements=" << selectedPathElements;
    }
}

void NCriticalPathView::updateChildrenSelectionFor(
    const QModelIndex& sourcePathIndex, bool selected) const {
  if (!m_sourceModel) {
    return;
  }

  QItemSelectionModel* selectModel = selectionModel();
  if (!selectModel) {
    return;
  }

  NCriticalPathItem* pathItem =
      static_cast<NCriticalPathItem*>(sourcePathIndex.internalPointer());
  if (!pathItem) {
    return;
  }
  if (!pathItem->isPath()) {
    return;
  }

  // collect range of selectionIndexes for path elemenets to be selected or
  // deselected

  QModelIndex sourceTopLeftIndex = m_sourceModel->index(0, 0, sourcePathIndex);
  QModelIndex sourceBottomRightIndex = m_sourceModel->index(
      pathItem->childCount() - 1, pathItem->columnCount() - 1, sourcePathIndex);

  QModelIndex selectTopLeftIndex{sourceTopLeftIndex};
  QModelIndex selectBottomRightIndex{sourceBottomRightIndex};

  // apply selection range to selection model
  if (selectTopLeftIndex.isValid() && selectBottomRightIndex.isValid()) {
    QItemSelection selectionItem(selectTopLeftIndex, selectBottomRightIndex);
    selectModel->select(selectionItem, selected
                                           ? QItemSelectionModel::Select
                                           : QItemSelectionModel::Deselect);
  }
}

QString NCriticalPathView::getSelectedPathElements() const {
  QItemSelectionModel* selectModel = selectionModel();

  std::map<std::size_t, std::set<std::size_t>> data;
  if (selectModel) {
    QModelIndexList selectedIndexes = selectModel->selectedIndexes();
    for (const QModelIndex& index : std::as_const(selectedIndexes)) {
      if (index.column() == 0) {
        QModelIndex sourceIndex{index};
        if (sourceIndex.isValid()) {
          NCriticalPathItem* item =
              static_cast<NCriticalPathItem*>(sourceIndex.internalPointer());
          if (item) {
            // QString type{item->type()};
            int elementIndex{item->id()};
            int pathIndex{item->pathIndex()};

            if (pathIndex != -1) {
              data[pathIndex].insert(elementIndex);
            }
          }
        }
      }
    }
  }

  QString result;
  for (const auto& [key, values] : data) {
    result += QString::number(key) + "#";
    for (int v : values) {
      result += QString::number(v) + ",";
    }
    if (result.endsWith(",")) {
      result.chop(1);  // remove last coma
    }
    result += "|";
  }
  if (result.endsWith("|")) {
    result.chop(1);  // remove last semicolomn
  }
  if (result.isEmpty()) {
    result = comm::CRITICAL_PATH_ITEMS_SELECTION_NONE;  // we cannot send just
                                                        // empty, because it
                                                        // breaks option parser
  }

  return result;
}


