#pragma once

#include <memory>
#include <vector>

#include <QSettings>
#include <QString>

#include "client/CommConstants.h"

/**
 * @brief Central data structure to keep parameters updated for the Interactive
 * Analysis Tool.
 *
 * It manages loading parameters from a file, saving to a file, and initializes
 * a settings file with default values. The purpose of this
 * class is to maintain a single shared data copy among instances that require
 * it, such as creation requests for the server, displaying in the UI, etc.
 */
class NCriticalPathParameters {
    const char* KEY_VPR_EXECUTABLE_FILEPATH = "vprExecFilePath";
    const char* KEY_EBLIF_FILEPATH = "eblifFilePath";
    const char* KEY_DEVICE_XML_FILEPATH = "xmlDeviceFilePath";

    const char* KEY_HIGHLIGHT_MODE = "highLightMode";
    const char* KEY_PATH_TYPE = "pathType";
    const char* KEY_PATH_DETAIL_LEVEL = "pathDetailLevel";
    const char* KEY_CRITICAL_PATH_NUM = "criticalPathNum";
    const char* KEY_IS_DRAW_CRITICAL_PATH_COUNTUR_ENABLED = "isDrawCriticalPathContourEnabled";

    const char* PARAM_HIGH_LIGHT_MODE = "high_light_mode";
    const char* PARAM_TIMING_REPORT_NPATHS = "timing_report_npaths";
    const char* PARAM_TIMING_REPORT_DETAIL = "timing_report_detail";
    const char* PARAM_DRAW_CRITICAL_PATH_CONTOUR = "draw_critical_path_contour";

    const char* DEFAULT_VALUE_PATHLIST_PARAM_HIGH_LIGHT_MODE = comm::KEY_CRIT_PATH_FLYLINES;
    const char* DEFAULT_VALUE_PATHLIST_PARAM_TYPE = comm::KEY_SETUP_PATH_LIST;
    const char* DEFAULT_VALUE_PATHLIST_PARAM_DETAIL_LEVEL = comm::KEY_NETLIST;
    const int DEFAULT_VALUE_PATHLIST_PARAM_MAX_NUM = 100;
    const bool DEFAULT_VALUE_PATHLIST_DRAW_PATH_CONTOUR = true;

public:
    NCriticalPathParameters();
    ~NCriticalPathParameters() = default;

    void setDrawCriticalPathContourEnabled(bool status) {
        m_isDrawCriticalPathContourEnabled = status;
    }
    bool isDrawCriticalPathContourEnabled() const {
        return m_isDrawCriticalPathContourEnabled;
    }

    const std::vector<QString>& getHighLightAvailableOptions() const;
    const std::vector<QString>& getPathDetailAvailableOptions();
    const std::vector<QString>& getCritPathTypeAvailableOptions() const;

    const QString& vprExecFilePath() const { return m_vprExecFilePath; }
    const QString& eblifFilePath() const { return m_eblifFilePath; }
    const QString& xmlDeviceFilePath() const { return m_xmlDeviceFilePath; }

    bool setHighLightMode(const QString& value);
    bool setPathType(const QString& value);
    bool setPathDetailLevel(const QString& value);
    bool setCriticalPathNum(int value);
    bool setIsDrawCriticalPathContourEnabled(bool value);

    bool setVprExecFilePath(const QString& value);
    bool setXmlDeviceFilePath(const QString& value);
    bool setEblifFilePath(const QString& value);

    const QString& getHighLightMode() const { return m_highLightMode; }
    const QString& getPathType() const { return m_pathType; }
    const QString& getPathDetailLevel() const { return m_pathDetailLevel; }
    int getCriticalPathNum() const { return m_criticalPathNum; }
    bool getIsDrawCriticalPathContourEnabled() const { return m_isDrawCriticalPathContourEnabled; }

    const QString& getHighLightModeToolTip() const {
        return m_highLightModeToolTip;
    }
    const QString& getPathTypeToolTip() const { return m_pathTypeToolTip; }
    const QString& getPathDetailLevelToolTip() const {
        return m_pathDetailLevelToolTip;
    }
    const QString& getCriticalPathNumToolTip() const {
        return m_criticalPathNumToolTip;
    }
    const QString& getIsDrawCriticalPathContourEnabledToolTip() const {
        return m_isDrawCriticalPathContourEnabledToolTip;
    }

private:
    QSettings m_settings;

    QString m_vprExecFilePath;
    QString m_eblifFilePath;
    QString m_xmlDeviceFilePath;

    QString m_highLightMode;
    QString m_pathType;
    QString m_pathDetailLevel;
    int m_criticalPathNum;
    bool m_isDrawCriticalPathContourEnabled;

    QString m_highLightModeToolTip;
    QString m_pathTypeToolTip;
    QString m_pathDetailLevelToolTip;
    QString m_criticalPathNumToolTip;
    QString m_isDrawCriticalPathContourEnabledToolTip;
};

using NCriticalPathParametersPtr = std::shared_ptr<NCriticalPathParameters>;
