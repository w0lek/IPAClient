#pragma once

#include <QWidget>
#include <QLabel>

#include "client/GateIO.h"

class NCriticalPathWidget : public QWidget {
  Q_OBJECT

 public:
  explicit NCriticalPathWidget(QWidget* parent = nullptr);
  ~NCriticalPathWidget()=default;

 private slots:
  void requestPathList(const QString& initiator);

 private:
  class NCriticalPathView* m_view{nullptr};
  class NCriticalPathToolsWidget* m_toolsWidget{nullptr};
  QLabel* m_vprCmd{nullptr};

  client::GateIO m_gateIO;

  void notifyError(QString, QString);
};

