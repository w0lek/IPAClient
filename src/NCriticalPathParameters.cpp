#include "NCriticalPathParameters.h"
#include "Path.h"

NCriticalPathParameters::NCriticalPathParameters(): m_settings(APP_NAME, APP_NAME)
{
    m_vprExecFilePath = m_settings.value(KEY_VPR_EXECUTABLE_FILEPATH, "").toString();
    m_eblifFilePath = m_settings.value(KEY_EBLIF_FILEPATH, Path::instance().defaultWorkingPath()+"/unconnected.eblif").toString();
    m_xmlDeviceFilePath = m_settings.value(KEY_DEVICE_XML_FILEPATH, Path::instance().defaultWorkingPath()+"/test_post_verilog_arch.xml").toString();

    m_highLightMode = m_settings.value(KEY_HIGHLIGHT_MODE, DEFAULT_VALUE_PATHLIST_PARAM_HIGH_LIGHT_MODE).toString();
    m_pathType = m_settings.value(KEY_PATH_TYPE, DEFAULT_VALUE_PATHLIST_PARAM_TYPE).toString();
    m_pathDetailLevel = m_settings.value(KEY_PATH_DETAIL_LEVEL, DEFAULT_VALUE_PATHLIST_PARAM_DETAIL_LEVEL).toString();
    m_criticalPathNum = m_settings.value(KEY_CRITICAL_PATH_NUM, DEFAULT_VALUE_PATHLIST_PARAM_MAX_NUM).toInt();
    m_isDrawCriticalPathContourEnabled = m_settings.value(KEY_IS_DRAW_CRITICAL_PATH_COUNTUR_ENABLED, DEFAULT_VALUE_PATHLIST_DRAW_PATH_CONTOUR).toBool();
}

const std::vector<QString>&
NCriticalPathParameters::getHighLightAvailableOptions() const {
    static std::vector<QString> options = {
        comm::KEY_CRIT_PATH_FLYLINES,
        comm::KEY_CRIT_PATH_FLYLINES_DELAYES,
        comm::KEY_CRIT_PATH_ROUTING,
        comm::KEY_CRIT_PATH_ROUTING_DELAYES
    };
    return options;
}

const std::vector<QString>&
NCriticalPathParameters::getPathDetailAvailableOptions() {
    static std::vector<QString> options = {
        comm::KEY_NETLIST,
        comm::KEY_AGGREGATED,
        comm::KEY_DETAILED,
        comm::KEY_DEBUG
    };
    return options;
}

const std::vector<QString>&
NCriticalPathParameters::getCritPathTypeAvailableOptions() const {
    static std::vector<QString> options = {
        comm::KEY_SETUP_PATH_LIST,
        comm::KEY_HOLD_PATH_LIST
    };
    return options;
}

bool NCriticalPathParameters::setVprExecFilePath(const QString& value)
{
    if (m_vprExecFilePath != value) {
        m_settings.setValue(KEY_VPR_EXECUTABLE_FILEPATH, value);
        m_vprExecFilePath = value;
        return true;
    }
    return false;
}

bool NCriticalPathParameters::setEblifFilePath(const QString& value)
{
    if (m_eblifFilePath != value) {
        m_settings.setValue(KEY_EBLIF_FILEPATH, value);
        m_eblifFilePath = value;
        return true;
    }
    return false;
}

bool NCriticalPathParameters::setXmlDeviceFilePath(const QString& value)
{
    if (m_xmlDeviceFilePath != value) {
        m_settings.setValue(KEY_DEVICE_XML_FILEPATH, value);
        m_xmlDeviceFilePath = value;
        return true;
    }
    return false;
}

bool NCriticalPathParameters::setHighLightMode(const QString& value)
{
    if (m_highLightMode != value) {
        m_settings.setValue(KEY_HIGHLIGHT_MODE, value);
        m_highLightMode = value;
        return true;
    }
    return false;
}

bool NCriticalPathParameters::setPathType(const QString& value)
{
    if (m_pathType != value) {
        m_settings.setValue(KEY_PATH_TYPE, value);
        m_pathType = value;
        return true;
    }
    return false;
}

bool NCriticalPathParameters::setPathDetailLevel(const QString& value)
{
    if (m_pathDetailLevel != value) {
        m_settings.setValue(KEY_PATH_DETAIL_LEVEL, value);
        m_pathDetailLevel = value;
        return true;
    }
    return false;
}

bool NCriticalPathParameters::setCriticalPathNum(int value)
{
    if (m_criticalPathNum != value) {
        m_settings.setValue(KEY_CRITICAL_PATH_NUM, value);
        m_criticalPathNum = value;
        return true;
    }
    return false;
}

bool NCriticalPathParameters::setIsDrawCriticalPathContourEnabled(bool value)
{
    if (m_isDrawCriticalPathContourEnabled != value) {
        m_settings.setValue(KEY_IS_DRAW_CRITICAL_PATH_COUNTUR_ENABLED, value);
        m_isDrawCriticalPathContourEnabled = value;
        return true;
    }
    return false;
}
