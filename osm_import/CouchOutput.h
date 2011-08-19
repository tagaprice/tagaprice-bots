#ifndef COUCHOUTPUT_H
#define COUCHOUTPUT_H

#include "Entity.h"

#include <QSet>

#include <osmium.hpp>
#include <qjson/parser.h>
#include <qjson/serializer.h>

class CouchOutput {
	QSet<qint64> m_existingNodes;
	QSet<qint64> m_existingWays;
	QSet<qint64> m_existingRelations;

	QVariantList m_entityList;

public:

	void addObject(const Entity &entity) {
		m_entityList.append(entity);
	}

	void print() {
		QJson::Serializer serializer;
		printf("%s\n", serializer.serialize(m_entityList).constData());
	}
};

#endif // COUCHOUTPUT_H
