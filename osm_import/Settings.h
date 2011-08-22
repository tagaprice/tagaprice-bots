#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QUrl>

class Settings {
private:
	static QSettings m_settings;
public:
	static QString getCouchHost();
	static int getCouchPort();
	static QString getCouchUser();
	static QString getCouchPwd();
	static QString getCouchDb();

	static QUrl getCouchBaseUrl();

	static QString getBotUID();
};

#endif // SETTINGS_H
