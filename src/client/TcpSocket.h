#pragma once

#include <QByteArray>
#include <QHostAddress>
#include <QObject>
#include <QTcpSocket>
#include <QTimer>

#include "TelegramBuffer.h"

namespace client {

/**
 * @brief Low-level communication layer implementing client functionality.
 *
 * This class acts as a wrapper around QTcpSocket, providing a foundational
 * interface for handling client-side communication. It manages the underlying
 * TCP socket functionality and facilitates communication with the server.
 * It serves as a fundamental building block for establishing and managing
 * connections between the client and the server, enabling bidirectional
 * data transmission and interaction.
 */
class TcpSocket : public QObject {
  Q_OBJECT

  static const int CONNECTION_WATCHER_INTERVAL_MS = 1000;
  static const int CONNECT_TO_HOST_TIMEOUT_MS = 5000;
  static const int CRITICAL_ERRORS_NUM = 5;

 public:
  TcpSocket();
  ~TcpSocket();

  void setServerIsRunning(bool serverIsRunning);

  void startConnectionWatcher() { m_connectionWatcher.start(); }
  void stopConnectionWatcher() { m_connectionWatcher.stop(); }

  bool connect();
  bool isConnected() const;
  bool write(const QByteArray&);

 signals:
  void connectedChanged(bool);
  void dataRecieved(QByteArray, bool isCompressed);

 private:
  QTcpSocket m_socket;
  bool m_serverIsRunning = false;
  QTimer m_connectionWatcher;
  comm::TelegramBuffer m_telegramBuff;

  bool ensureConnected();

 private slots:
  void handleStateChanged(QAbstractSocket::SocketState);
  void handleDataReady();
  void handleError(QAbstractSocket::SocketError);
};

}  // namespace client

