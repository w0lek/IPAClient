#include "NCriticalPathToolsWidget.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QFormLayout>
#include <QIntValidator>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

#include "CustomMenu.h"
#include "NCriticalPathParameters.h"

NCriticalPathToolsWidget::NCriticalPathToolsWidget(QWidget* parent)
    : QWidget(parent),
    m_vprProcess("vpr"),
    m_parameters(std::make_shared<NCriticalPathParameters>())
{
    QHBoxLayout* layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(2);
    setLayout(layout);

    QPushButton* bnPathsOptions = new QPushButton("Configuration");
    bnPathsOptions->setToolTip(
        tr("grouped project settings relevant for the Interactive Path Analysis "
           "Tool"));
    layout->addWidget(bnPathsOptions);
    setupCriticalPathsOptionsMenu(bnPathsOptions);

    // bnRunPnRView
    m_bnRunPnRView = new QPushButton("Run P&&R View");
    m_bnRunPnRView->setToolTip(
        tr("run P&R View manually if it was terminated or closed"));
    layout->addWidget(m_bnRunPnRView);
    connect(m_bnRunPnRView, &QPushButton::clicked, this,
            &NCriticalPathToolsWidget::tryRunPnRView);
    connect(&m_vprProcess, &Process::runStatusChanged, this,
            [this](bool isRunning) {
                m_bnRunPnRView->setEnabled(!isRunning);
                emit PnRViewRunStatusChanged(isRunning);
            });
    connect(&m_vprProcess, &Process::innerErrorOccurred, this,
            &NCriticalPathToolsWidget::vprProcessErrorOccured);

    onConnectionStatusChanged(false);
}

void NCriticalPathToolsWidget::deactivatePlaceAndRouteViewProcess() {
    m_bnRunPnRView->setEnabled(false);
    m_vprProcess.stop();
}

void NCriticalPathToolsWidget::enablePlaceAndRouteViewButton() {
    m_bnRunPnRView->setEnabled(true);
}

void NCriticalPathToolsWidget::setupCriticalPathsOptionsMenu(
    QPushButton* caller) {
    if (m_pathsOptionsMenu) {
        return;
    }

    m_pathsOptionsMenu = new CustomMenu(caller);
    m_pathsOptionsMenu->setButtonToolTips(
        tr("apply interactive path analysis configuration and save it to main "
           "project settings file"),
        tr("discard settings"));
    connect(m_pathsOptionsMenu, &CustomMenu::declined, this,
            [this]() { resetConfigurationUI(); });
    connect(m_pathsOptionsMenu, &CustomMenu::accepted, this, [this]() {
        bool restartVprRequest = false;
        bool refreshPathListRequest = false;
        bool refreshDrawingRequest = false;
        if (m_parameters->setVprExecFilePath(m_leVprFilePath->text()) ||
            m_parameters->setXmlDeviceFilePath(m_leDeviceXmlFilePath->text()) ||
            m_parameters->setEblifFilePath(m_leEblifFilePath->text())) {
            restartVprRequest = true;
        }
        if (m_parameters->setPathType(m_cbPathType->currentText()) ||
            m_parameters->setPathDetailLevel(m_cbDetail->currentText()) ||
            m_parameters->setCriticalPathNum(m_leNCriticalPathNum->text().toInt())) {
            refreshPathListRequest = true;
        }

        if (m_parameters->setHighLightMode(m_cbHighlightMode->currentText()) ||
            m_parameters->setIsDrawCriticalPathContourEnabled(m_cbDrawCritPathContour->isChecked())) {
            refreshDrawingRequest = true;
        }

        if (restartVprRequest) {
            restartVpr();
            return;
        } else {
            if (refreshPathListRequest) {
                emit pathListRequested("autorefresh because path list configuration changed");
            }

            if (refreshDrawingRequest) {
                emit highLightModeChanged();
            }
        }
    });

    QFormLayout* formLayout = new QFormLayout;
    m_pathsOptionsMenu->addContentLayout(formLayout);

    //
    m_leVprFilePath = new QLineEdit;
    m_leDeviceXmlFilePath = new QLineEdit;
    m_leEblifFilePath = new QLineEdit;

    m_leVprFilePath->setText(m_parameters->vprExecFilePath());
    m_leDeviceXmlFilePath->setText(m_parameters->xmlDeviceFilePath());
    m_leEblifFilePath->setText(m_parameters->eblifFilePath());

    addRowToFormLayout(formLayout, tr("Vpr file path:"), m_leVprFilePath);
    addRowToFormLayout(formLayout, tr("Device XML file path:"), m_leDeviceXmlFilePath);
    addRowToFormLayout(formLayout, tr("EBLIF file path:"), m_leEblifFilePath);
    //

    //
    m_cbHighlightMode = new QComboBox;
    m_cbHighlightMode->setToolTip(
        m_parameters->getHighLightModeToolTip());
    for (const QString& item : m_parameters->getHighLightAvailableOptions()) {
        m_cbHighlightMode->addItem(item);
    }

    addRowToFormLayout(formLayout, tr("Hight light mode:"), m_cbHighlightMode);

    //
    m_cbDrawCritPathContour = new QCheckBox;
    m_cbDrawCritPathContour->setToolTip(
        m_parameters->getIsDrawCriticalPathContourEnabledToolTip());
    addRowToFormLayout(formLayout, tr("Draw path contour:"),
                       m_cbDrawCritPathContour);

    //
    m_cbPathType = new QComboBox;
    m_cbPathType->setToolTip(m_parameters->getPathTypeToolTip());
    for (const QString& item :
         m_parameters->getCritPathTypeAvailableOptions()) {
        m_cbPathType->addItem(item);
    }
    addRowToFormLayout(formLayout, tr("Path type:"), m_cbPathType);

    //
    m_cbDetail = new QComboBox;
    m_cbDetail->setToolTip(m_parameters->getPathDetailLevelToolTip());
    for (const QString& item :
         m_parameters->getPathDetailAvailableOptions()) {
        m_cbDetail->addItem(item);
    }
    addRowToFormLayout(formLayout, tr("Timing report detail:"), m_cbDetail);

    //
    m_leNCriticalPathNum = new QLineEdit();
    m_leNCriticalPathNum->setToolTip(
        m_parameters->getCriticalPathNumToolTip());
    QIntValidator intValidator(m_leNCriticalPathNum);
    m_leNCriticalPathNum->setValidator(&intValidator);

    addRowToFormLayout(formLayout, tr("Timing report npaths:"),
                       m_leNCriticalPathNum);

    resetConfigurationUI();
}

void NCriticalPathToolsWidget::resetConfigurationUI() {
    m_cbHighlightMode->setCurrentText(m_parameters->getHighLightMode());
    m_cbPathType->setCurrentText(m_parameters->getPathType());
    m_cbDetail->setCurrentText(m_parameters->getPathDetailLevel());
    m_leNCriticalPathNum->setText(
        QString::number(m_parameters->getCriticalPathNum()));
    m_cbDrawCritPathContour->setChecked(
        m_parameters->getIsDrawCriticalPathContourEnabled());
}

std::pair<QString, QString> NCriticalPathToolsWidget::vprCommand() const
{
    // validate project detailes
    QFileInfo vprFilePathFi{m_leVprFilePath->text()};
    if (!vprFilePathFi.exists() || vprFilePathFi.isDir()) {
        emit vprProcessErrorOccured(QString("%1 is not valid 'Vpr file path', please set valid 'Vpr file path' in Configuration").arg(vprFilePathFi.absoluteFilePath()));
        return std::make_pair("", "");
    }

    QFileInfo deviceXmlFilePathFi{m_leDeviceXmlFilePath->text()};
    if (!deviceXmlFilePathFi.exists()) {
        emit vprProcessErrorOccured(QString("'Device XML file path' %1 doesn't exist, please set valid 'Device XML file path' in Configuration").arg(m_leDeviceXmlFilePath->text()));
        return std::make_pair("", "");
    }

    QFileInfo eblifFilePathFi{m_leEblifFilePath->text()};
    if (!eblifFilePathFi.exists()) {
        emit vprProcessErrorOccured(QString("'EBLIF file path' %1 doesn't exist, please set valid 'EBLIF file path' in Configuration").arg(m_leEblifFilePath->text()));
        return std::make_pair("", "");
    }
    //

    QString workDir{vprFilePathFi.absolutePath()};
    QString vprCmd;
    vprCmd += "./vpr";
    vprCmd += " ";
    vprCmd += deviceXmlFilePathFi.filePath();
    vprCmd += " ";
    vprCmd += eblifFilePathFi.filePath();
    vprCmd += " ";
    vprCmd += "--device auto --timing_analysis on --constant_net_method route --clock_modeling ideal --exit_before_pack off --circuit_format eblif --absorb_buffer_luts off --route_chan_width 180 --flat_routing off --gen_post_synthesis_netlist on";
    vprCmd += " ";
    vprCmd += QString("--timing_report_npaths %1").arg(m_parameters->getCriticalPathNum());
    vprCmd += " ";
    vprCmd += QString("--timing_report_detail %1").arg(m_parameters->getPathDetailLevel());
    vprCmd += " ";
    vprCmd += "--allow_dangling_combinational_nodes on";
    vprCmd += " ";
    vprCmd += "--analysis";
    vprCmd += " ";
    vprCmd += "--server";
    vprCmd += " ";
    vprCmd += QString("--port %1").arg(SERVER_PORT_NUM);
    vprCmd += " ";
    vprCmd += "--disp on";

    return std::make_pair(workDir, vprCmd);
}

void NCriticalPathToolsWidget::restartVpr()
{
    if (m_vprProcess.isRunning()) {
        m_vprProcess.stop();
    }
    tryRunPnRView();
}

void NCriticalPathToolsWidget::tryRunPnRView() {
    if (m_vprProcess.isRunning()) {
        qInfo() << "skip P&R View process run, because it's already run";
        return;
    }

    auto [workPath, vprCmd] = vprCommand();
    emit vprCmdUsed(vprCmd);
    if (!vprCmd.isEmpty()) {
        qInfo() << "set working dir" << workPath;
        m_vprProcess.setWorkingDirectory(workPath);
        m_vprProcess.start(vprCmd);
    }
}

void NCriticalPathToolsWidget::onConnectionStatusChanged(bool isConnected) {
    if (isConnected) {
#ifndef BYPASS_AUTO_PATH_LIST_FETCH
        emit pathListRequested("socket connection resumed");
#endif
    }
}

void NCriticalPathToolsWidget::addRowToFormLayout(QFormLayout* formLayout,
                                                  const QString& labelText,
                                                  QWidget* widget) const
{
    QLabel* label = new QLabel(labelText);
    label->setToolTip(widget->toolTip());
    formLayout->addRow(label, widget);
}

