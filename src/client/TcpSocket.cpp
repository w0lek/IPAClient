#include "TcpSocket.h"

namespace client {

TcpSocket::TcpSocket() {
  m_connectionWatcher.setInterval(CONNECTION_WATCHER_INTERVAL_MS);
  QObject::connect(&m_connectionWatcher, &QTimer::timeout, this,
                   [this]() { ensureConnected(); });

  QObject::connect(&m_socket, &QAbstractSocket::stateChanged, this,
                   &TcpSocket::handleStateChanged);
  QObject::connect(&m_socket, &QIODevice::readyRead, this,
                   &TcpSocket::handleDataReady);
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
  QObject::connect(
      &m_socket,
      qOverload<QAbstractSocket::SocketError>(&QAbstractSocket::error), this,
      &TcpSocket::handleError);
#else
  QObject::connect(&m_socket, &QAbstractSocket::errorOccurred, this,
                   &TcpSocket::handleError);
#endif
}

TcpSocket::~TcpSocket() {
  m_connectionWatcher.stop();
  m_socket.close();
}

bool TcpSocket::connect() {
  m_socket.connectToHost(QHostAddress{QHostAddress::LocalHostIPv6}, SERVER_PORT_NUM);
  if (m_socket.waitForConnected(CONNECT_TO_HOST_TIMEOUT_MS)) {
      qInfo() << "connected to host" << QHostAddress{QHostAddress::LocalHostIPv6}.toString() << SERVER_PORT_NUM;
  }
  return isConnected();
}

bool TcpSocket::isConnected() const {
  return (m_socket.state() == QAbstractSocket::ConnectedState);
}

bool TcpSocket::write(const QByteArray& bytes) {
  if (ensureConnected()) {
    m_socket.write(bytes);
    return m_socket.waitForBytesWritten();
  } else {
    return false;
  }
}

bool TcpSocket::ensureConnected() {
  if (!isConnected()) {
    return connect();
  }
  return true;
}

void TcpSocket::setServerIsRunning(bool serverIsRunning) {
  m_serverIsRunning = serverIsRunning;
  if (serverIsRunning) {
    startConnectionWatcher();
  } else {
    stopConnectionWatcher();
  }
}

void TcpSocket::handleStateChanged(QAbstractSocket::SocketState state) {
  if ((state == QAbstractSocket::ConnectedState) ||
      (state == QAbstractSocket::ConnectingState)) {
    stopConnectionWatcher();
  } else {
    if (m_serverIsRunning) {
      startConnectionWatcher();
    }
  }

  if (state == QAbstractSocket::ConnectedState) {
    emit connectedChanged(true);
  } else {
    emit connectedChanged(false);
  }
}

void TcpSocket::handleDataReady() {
  QByteArray bytes = m_socket.readAll();
  m_telegramBuff.append(bytes);

  std::vector<comm::TelegramFramePtr> telegramFrames =
      m_telegramBuff.takeTelegramFrames();
  for (const comm::TelegramFramePtr& telegramFrame : telegramFrames) {
    QByteArray bytes{telegramFrame->body};
    qInfo() << "received" << telegramFrame->header.info().c_str();
    emit dataRecieved(bytes, telegramFrame->header.isBodyCompressed());
  }

  std::vector<std::string> errors;
  m_telegramBuff.takeErrors(errors);
  for (const std::string& error : errors) {
    qWarning() << error.c_str();
  }
}

void TcpSocket::handleError(QAbstractSocket::SocketError error) {
    m_telegramBuff.clear();
    qDebug() << "socket error" << m_socket.errorString() << error;
}

}  // namespace client

