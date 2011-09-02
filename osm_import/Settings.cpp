#include "Settings.h"

QSettings Settings::m_settings("./config.ini", QSettings::IniFormat);

QString Settings::getBotUID() {
	return m_settings.value("tap/uid").toString();
}

QUrl Settings::getCouchBaseUrl() {
	QUrl rc;

	rc.setScheme("http");
	rc.setHost(getCouchHost());
	rc.setPort(getCouchPort());
	rc.setUserName(getCouchUser());
	rc.setPassword(getCouchPwd());
	rc.setPath(getCouchDb());

	return rc;
}

QString Settings::getCouchDb() {
	return m_settings.value("couchdb/db", "tagaprice").toString();
}

QString Settings::getCouchHost() {
	return m_settings.value("couchdb/host", "localhost").toString();
}

int Settings::getCouchPort() {
	return m_settings.value("couchdb/port", 5984).toInt();
}

QString Settings::getCouchPwd() {
	return m_settings.value("couchdb/password").toString();
}

QString Settings::getCouchUser() {
	return m_settings.value("couchdb/user").toString();
}
