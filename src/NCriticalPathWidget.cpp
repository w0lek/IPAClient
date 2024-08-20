#include "NCriticalPathWidget.h"

#include <QFileInfo>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

#include "NCriticalPathToolsWidget.h"
#include "NCriticalPathView.h"

NCriticalPathWidget::NCriticalPathWidget(QWidget* parent)
    : QWidget(parent),
      m_view(new NCriticalPathView(this)),
      m_toolsWidget(new NCriticalPathToolsWidget(this)),
      m_gateIO(m_toolsWidget->parameters()) {

  QVBoxLayout* layout = new QVBoxLayout;
  layout->setContentsMargins(3, 3, 3, 3);
  layout->setSpacing(3);
  setLayout(layout);

  /// viewer setup
  m_view->header()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
  m_view->header()->setHidden(true);
  ///

  /// toolbox setup
  QHBoxLayout* toolBarLayout = new QHBoxLayout;

  toolBarLayout->addWidget(m_toolsWidget);
  toolBarLayout->addStretch();
  ///

  m_vprCmd = new QLabel;
  connect(m_toolsWidget, &NCriticalPathToolsWidget::vprCmdUsed, this, [this](QString vprCmd){
    m_vprCmd->setText(vprCmd);
  });
  m_vprCmd->setWordWrap(true);
  m_vprCmd->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);

  ///
  layout->addLayout(toolBarLayout);
  layout->addWidget(m_view);
  layout->addWidget(m_vprCmd);

  layout->setStretch(0, 0);
  layout->setStretch(1, 1);
  layout->setStretch(2, 0);

  // view connections
  connect(m_view, &NCriticalPathView::pathElementSelectionChanged, &m_gateIO,
          &client::GateIO::requestPathItemsHighLight);

  // toolswidget connections
  connect(m_toolsWidget, &NCriticalPathToolsWidget::PnRViewRunStatusChanged,
          this, [this](bool isRunning) {
            if (!isRunning) {
              m_view->clear();
            }
            m_gateIO.setServerIsRunning(isRunning);
          });
  connect(m_toolsWidget, &NCriticalPathToolsWidget::pathListRequested, this,
          &NCriticalPathWidget::requestPathList);
  connect(m_toolsWidget, &NCriticalPathToolsWidget::highLightModeChanged,
          &m_gateIO, &client::GateIO::onHightLightModeChanged);
  connect(m_toolsWidget, &NCriticalPathToolsWidget::vprProcessErrorOccured,
          this, [this](QString errorMsg) { notifyError("VPR", errorMsg); });

  // client connections
  connect(&m_gateIO, &client::GateIO::pathListDataReceived, m_view,
          &NCriticalPathView::loadFromString);
  connect(&m_gateIO, &client::GateIO::connectedChanged, this,
          [this](bool isConnected) {
            m_toolsWidget->onConnectionStatusChanged(isConnected);
          });

  QTimer::singleShot(200, this, [this](){
      m_toolsWidget->tryRunPnRView();  // startup run
  });
}

void NCriticalPathWidget::requestPathList(const QString& initiator) {
  if (m_gateIO.isConnected()) {
    m_view->clear();
    m_gateIO.requestPathList(initiator);
  } else {
    qCritical() << "cannot requestPathList by" << initiator << "because client is not connected";
  }
}

void NCriticalPathWidget::notifyError(QString title, QString errorMsg) {
    QMessageBox::warning(this, title, errorMsg, QMessageBox::Ok);
}
