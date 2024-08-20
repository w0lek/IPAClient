#include "GateIO.h"

#include "CommConstants.h"
#include "ConvertUtils.h"
#include "RequestCreator.h"
#include "TcpSocket.h"
#include "TelegramParser.h"
#include "ZlibUtils.h"

namespace client {

GateIO::GateIO(const NCriticalPathParametersPtr& parameters)
    : m_parameters(parameters),
      m_echoTelegram{comm::TelegramHeader::constructFromBody(QByteArray(comm::TELEGRAM_ECHO_BODY)), comm::TELEGRAM_ECHO_BODY} {
    connect(&m_socket, &TcpSocket::connectedChanged, this,
          &GateIO::connectedChanged);
    connect(&m_socket, &TcpSocket::dataRecieved, this, &GateIO::handleResponse);
}

GateIO::~GateIO() {}

void GateIO::onHightLightModeChanged() {
  requestPathItemsHighLight(m_lastPathItems, "hight light mode changed");
}

bool GateIO::isConnected() const { return m_socket.isConnected(); }

void GateIO::startConnectionWatcher() { m_socket.startConnectionWatcher(); }

void GateIO::stopConnectionWatcher() { m_socket.stopConnectionWatcher(); }

void GateIO::handleResponse(const QByteArray& bytes, bool isCompressed) {
  static const std::string echoData{comm::TELEGRAM_ECHO_BODY};

  std::string rawData{bytes.begin(), bytes.end()};

  bool isEchoTelegram = false;
  if (rawData.size() == echoData.size()) {
    if (rawData == echoData) {
      sendRequest(
          m_echoTelegram,
          comm::TELEGRAM_ECHO_BODY);  // please don't change initiator else from
                                      // comm::TELEGRAM_ECHO_BODY here in order
                                      // to properly exclude such kind of
                                      // request from statistics
      isEchoTelegram = true;
    }
  }
  if (!isEchoTelegram) {
    std::optional<std::string> decompressedTelegramOpt;
#ifndef FORCE_DISABLE_ZLIB_TELEGRAM_COMPRESSION
    if (isCompressed) {
      decompressedTelegramOpt = tryDecompress(rawData);
    }
#endif
    if (!decompressedTelegramOpt) {
      decompressedTelegramOpt = std::move(rawData);
      rawData.clear();
    }

    const std::string& telegram = decompressedTelegramOpt.value();

    std::optional<int> jobIdOpt =
        comm::TelegramParser::tryExtractFieldJobId(telegram);
    std::optional<int> cmdOpt =
        comm::TelegramParser::tryExtractFieldCmd(telegram);
    std::optional<int> statusOpt =
        comm::TelegramParser::tryExtractFieldStatus(telegram);
    std::optional<std::string> dataOpt;
    comm::TelegramParser::tryExtractFieldData(telegram, dataOpt);

    bool isResponseConsistent = true;
    if (!jobIdOpt) {
        qWarning() << "bad response telegram, missing required field" << comm::KEY_JOB_ID;
        isResponseConsistent = false;
    }
    if (!cmdOpt) {
        qWarning() << "bad response telegram, missing required field" << comm::KEY_CMD;
        isResponseConsistent = false;
    }
    if (!statusOpt) {
        qWarning() << "bad response telegram, missing required field" << comm::KEY_STATUS;
        isResponseConsistent = false;
    }
    if (!dataOpt) {
      dataOpt = "";
    }

    if (isResponseConsistent) {
      int cmd = cmdOpt.value();
      bool status = statusOpt.value();
      QString data{dataOpt.value().c_str()};

      if (status) {
        switch (cmd) {
          case comm::CMD_GET_PATH_LIST_ID:
            emit pathListDataReceived(data);
            break;
          case comm::CMD_DRAW_PATH_ID:
            emit highLightModeReceived();
            break;
        }
      } else {
          qCritical() << "unable to perform cmd on server, error" << getTruncatedMiddleStr(data.toStdString()).c_str();
      }
    }
  }
}

void GateIO::sendRequest(const comm::TelegramFrame& frame,
                         const QString& initiator) {
    if (!m_socket.isConnected()) {
        m_socket.connect();
    }

    QByteArray telegram(
        reinterpret_cast<const char*>(frame.header.buffer().data()),
        frame.header.buffer().size());
    telegram.append(reinterpret_cast<const char*>(frame.body.data()),
                    frame.body.size());

    if (m_socket.write(telegram)) {
        qDebug() << "sent" << frame.header.info().c_str() << " data[" <<
            getTruncatedMiddleStr(frame.body.toStdString()).c_str() << "]" <<
            QString("requested by [%1]").arg(initiator);
    } else {
        qDebug() << "unable to send" << frame.header.info().c_str() << " data[" <<
            getTruncatedMiddleStr(frame.body.toStdString()).c_str() << "]" <<
            QString("requested by [%1]").arg(initiator);
    }
}

void GateIO::requestPathList(const QString& initiator) {
  m_lastPathItems =
      comm::CRITICAL_PATH_ITEMS_SELECTION_NONE;  // reset previous selection on
                                                 // new path list request
  comm::TelegramFramePtr telegram =
      RequestCreator::instance().getPathListRequestTelegram(
          m_parameters->getCriticalPathNum(),
          m_parameters->getPathType(),
          m_parameters->getPathDetailLevel(), false);
  sendRequest(*telegram, initiator);
}

void GateIO::requestPathItemsHighLight(const QString& pathItems,
                                       const QString& initiator) {
  m_lastPathItems = pathItems;
  comm::TelegramFramePtr telegram =
      RequestCreator::instance().getDrawPathItemsTelegram(
          pathItems, m_parameters->getHighLightMode(),
          m_parameters->getIsDrawCriticalPathContourEnabled());
  sendRequest(*telegram, initiator);
}

}  // namespace client

