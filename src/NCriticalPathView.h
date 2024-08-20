#pragma once

#include <QTreeView>

class QPushButton;
class QCheckBox;
class QMouseEvent;

class NCriticalPathItem;
class NCriticalPathModel;
class RoundProgressWidget;

class NCriticalPathView final : public QTreeView {
  Q_OBJECT

  const int INDENT_SIZE = 150;

 public:
  explicit NCriticalPathView(QWidget* parent = nullptr);
  ~NCriticalPathView() override final = default;

  void clear();

 protected:
  void mousePressEvent(QMouseEvent* event) override final;
  void mouseReleaseEvent(QMouseEvent* event) override final;

  void handleSelectionChanged(const QItemSelection& selected,
                              const QItemSelection& deselected);

 signals:
  void pathElementSelectionChanged(const QString&, const QString&);
  void loadFromString(const QString&);
  void dataLoaded();
  void dataCleared();

 private:
  QList<std::pair<QModelIndex, bool>>
      m_pathSourceIndexesToResolveChildrenSelection;
  bool m_isClearAllSelectionsPending = false;

  NCriticalPathModel* m_sourceModel = nullptr;

  QString getSelectedPathElements() const;
  void updateChildrenSelectionFor(const QModelIndex& sourcePathIndex,
                                  bool selected) const;
  void scroll(int steps);

  void clearSelection();

  void setupModels();
};
