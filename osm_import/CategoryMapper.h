#ifndef CATEGORYMAPPER_H
#define CATEGORYMAPPER_H

#include <QMap>
#include <QString>

class CategoryMapper {
	QMap<QString, QString> m_map;
public:
	CategoryMapper(const QString &filename);

	QString getCategoryId(const QString &typeName) const {
		return m_map[typeName];
	}

	bool hasCategory(const QString &typeName) const {
		return m_map.contains(typeName);
	}
};

#endif // TYPEMAPPER_H
