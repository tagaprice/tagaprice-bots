#include "CouchOutput.h"
#include "Entity.h"

#include <QVariant>

#include <qjson/qobjecthelper.h>
#include <qjson/serializer.h>

CouchOutput::CouchOutput() {
}

void CouchOutput::addObject(Osmium::OSM::Object *object, const char *type) {
	QJson::Serializer serializer;
	Entity entity;

	foreach (Osmium::OSM::Tag tag, object->tags()) {
		QString tagName = QString("osm:%s").arg(tag.key());
		entity.setProperty(tagName.toUtf8().constData(), tag.value());
	}

	QVariantMap variant = QJson::QObjectHelper::qobject2qvariant(&entity);
	serializer.serialize(variant);
}
