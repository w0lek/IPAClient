#pragma once

#include <QWidget>

#include "NCriticalPathParameters.h"
#include "VprProcess.h"

class QLabel;
class QLineEdit;
class QComboBox;
class QCheckBox;
class QPushButton;
class QFormLayout;

class CustomMenu;
class RefreshIndicatorButton;

class NCriticalPathToolsWidget : public QWidget {
  Q_OBJECT
 public:
  NCriticalPathToolsWidget(QWidget* parent = nullptr);
  ~NCriticalPathToolsWidget() = default;

  void resetConfigurationUI();

  NCriticalPathParametersPtr parameters() const { return m_parameters; }

 public slots:
  void onConnectionStatusChanged(bool);
  void tryRunPnRView();
  void deactivatePlaceAndRouteViewProcess();
  void enablePlaceAndRouteViewButton();

 signals:
  void pathListRequested(const QString&);
  void PnRViewRunStatusChanged(bool);
  void highLightModeChanged();
  void vprProcessErrorOccured(QString) const;
  void vprCmdUsed(QString) const;

 private:
  QLineEdit* m_leVprFilePath{nullptr};
  QLineEdit* m_leDeviceXmlFilePath{nullptr};
  QLineEdit* m_leEblifFilePath{nullptr};

  QLineEdit* m_leNCriticalPathNum{nullptr};
  QComboBox* m_cbHighlightMode{nullptr};
  QCheckBox* m_cbDrawCritPathContour{nullptr};
  QComboBox* m_cbPathType{nullptr};
  QComboBox* m_cbDetail{nullptr};

  Process m_vprProcess;

  NCriticalPathParametersPtr m_parameters;

  CustomMenu* m_pathsOptionsMenu = nullptr;

  QPushButton* m_bnRunPnRView = nullptr;

  void setupCriticalPathsOptionsMenu(QPushButton*);

  std::pair<QString, QString> vprCommand() const;
  void restartVpr();

  void addRowToFormLayout(QFormLayout* formLayout, const QString& labelText,
                          QWidget* widget) const;
};
