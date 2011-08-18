#ifndef COUCHOUTPUT_H
#define COUCHOUTPUT_H

#include "Entity.h"

#include <QSet>

#include <osmium.hpp>
#include <qjson/serializer.h>

class CouchOutput {
	QSet<qint64> m_existingNodes;
	QSet<qint64> m_existingWays;
	QSet<qint64> m_existingRelations;

public:
	CouchOutput() {
		printf("[\n");
	}

	virtual ~CouchOutput() {
		printf("]\n");
	}

	static void addObject(const Entity &entity) {
		QJson::Serializer serializer;

		printf("%s,\n", serializer.serialize(entity).constData());
	}
};

#endif // COUCHOUTPUT_H
