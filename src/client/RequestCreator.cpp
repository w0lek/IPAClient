#include "RequestCreator.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QList>

#include "CommConstants.h"
#include "TelegramFrame.h"

namespace client {

RequestCreator& RequestCreator::instance() {
  static RequestCreator creator;
  return creator;
}

comm::TelegramFramePtr RequestCreator::getPathListRequestTelegram(
    int nCriticalPathNum, const QString& pathType, const QString& detailsLevel,
    bool isFlat) {
  QString options;
  if ((nCriticalPathNum < 0) ||
      (nCriticalPathNum > comm::CRITICAL_PATH_NUM_THRESHOLD)) {
    qInfo() << "requested value" << nCriticalPathNum
            << "for n critical path max num is out of supported range, value "
               "limited to maximum possible"
            << comm::CRITICAL_PATH_NUM_THRESHOLD;
    nCriticalPathNum = comm::CRITICAL_PATH_NUM_THRESHOLD;
  }

  options.append(
      QString("int:%1:%2;").arg(comm::OPTION_PATH_NUM).arg(nCriticalPathNum));
  options.append(
      QString("string:%1:%2;").arg(comm::OPTION_PATH_TYPE).arg(pathType));
  options.append(QString("string:%1:%2;")
                     .arg(comm::OPTION_DETAILS_LEVEL)
                     .arg(detailsLevel));
  options.append(
      QString("bool:%1:%2").arg(comm::OPTION_IS_FLOAT_ROUTING).arg(isFlat));

  return getTelegramFrame(comm::CMD_GET_PATH_LIST_ID, options);
}

comm::TelegramFramePtr RequestCreator::getDrawPathItemsTelegram(
    const QString& pathItems, const QString& highLightMode,
    bool drawPathContour) {
  QString options;
  options.append(
      QString("string:%1:%2;").arg(comm::OPTION_PATH_ELEMENTS).arg(pathItems));
  options.append(QString("string:%1:%2;")
                     .arg(comm::OPTION_HIGHLIGHT_MODE)
                     .arg(highLightMode));
  options.append(QString("bool:%1:%2")
                     .arg(comm::OPTION_DRAW_PATH_CONTOUR)
                     .arg(drawPathContour));

  return getTelegramFrame(comm::CMD_DRAW_PATH_ID, options);
}

comm::TelegramFramePtr RequestCreator::getTelegramFrame(
    int cmd, const QString& options) {
  QJsonObject ob;
  ob[comm::KEY_JOB_ID] = QString::number(getNextRequestId());
  ob[comm::KEY_CMD] = QString::number(cmd);
  ob[comm::KEY_OPTIONS] = options;

  QJsonDocument jsonDoc(ob);

  comm::TelegramFramePtr telegram = std::make_shared<comm::TelegramFrame>();

  telegram->body = jsonDoc.toJson(QJsonDocument::Compact);
  telegram->header = comm::TelegramHeader::constructFromBody(telegram->body);

  return telegram;
}

int RequestCreator::getNextRequestId() { return ++m_lastRequestId; }

}  // namespace client

