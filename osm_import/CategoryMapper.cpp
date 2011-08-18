#include "CategoryMapper.h"

#include <QSettings>
#include <QStringList>

CategoryMapper::CategoryMapper(const QString &filename) {
	QSettings settings(filename, QSettings::IniFormat);

	foreach (QString key, settings.allKeys()) {
		m_map.insert(key, settings.value(key).toString());
	}

}
