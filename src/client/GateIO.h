#pragma once

#include <QObject>

#include "../NCriticalPathParameters.h"
#include "TcpSocket.h"

namespace client {

/**
 * @brief Primary communication class for managing interactions within the
 * Interactive Path Analysis tool.
 *
 * This central class serves as the core component responsible for communication
 * operations within the Interactive Path Analysis tool. It encapsulates the
 * low-level communication layer, processes received telegrams, and categorizes
 * them based on their nature. Additionally, it handles the configuration and
 * parameterization of requests to the server, utilizing shared data
 * encapsulated within the NCriticalPathParametersPtr. This class acts as a
 * bridge between the tool's interface and underlying communication
 * functionalities, ensuring seamless interactions and data exchange.
 */
class GateIO : public QObject {
  Q_OBJECT

 public:
  GateIO(const NCriticalPathParametersPtr&);
  ~GateIO();

  bool isConnected() const;
  void startConnectionWatcher();
  void stopConnectionWatcher();
  void setServerIsRunning(bool flag) { m_socket.setServerIsRunning(flag); }

 public slots:
  void requestPathItemsHighLight(const QString&, const QString&);
  void requestPathList(const QString&);
  void onHightLightModeChanged();

 signals:
  void pathListDataReceived(const QString&);
  void highLightModeReceived();
  void connectedChanged(bool);

 private:
  NCriticalPathParametersPtr m_parameters;

  QString m_lastPathItems = comm::CRITICAL_PATH_ITEMS_SELECTION_NONE;
  TcpSocket m_socket;

  const comm::TelegramFrame m_echoTelegram;

  void sendRequest(const comm::TelegramFrame& frame, const QString& initiator);
  void handleResponse(const QByteArray&, bool isCompressed);
};

}  // namespace client

