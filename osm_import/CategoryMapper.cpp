#include "CategoryMapper.h"
#include "CouchOutput.h"

#include <QFileInfo>
#include <QSettings>
#include <QStringList>

CategoryMapper::CategoryMapper(const QString &filename) {
	QSettings settings(filename, QSettings::IniFormat);
	int totalCount = 0, failedCount = 0;

	fprintf(stderr, "Reading category mappers for tag '%s'...", QFileInfo(filename).baseName().toLocal8Bit().constData());
	foreach (QString key, settings.allKeys()) {
		QString tapId = settings.value(key).toString();
		if (CouchOutput::hasCategory(tapId)) {
			m_map.insert(key, tapId);
		}
		else failedCount++;

		totalCount++;
	}

	if (failedCount) {
		fprintf(stderr, "done (%i of %i ignored because they weren't found in the CouchDB)\n", failedCount, totalCount);
	}
	else {
		fprintf(stderr, "done (%i categories added)", totalCount);
	}
}
