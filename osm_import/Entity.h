#ifndef ENTITY_H
#define ENTITY_H

#include "Settings.h"

#include <QVariantMap>

#include <geos/algorithm/CentroidArea.h>
#include <osmium.hpp>

class EntityException {

};

class Entity : public QVariantMap {
public:
	explicit Entity(const Osmium::OSM::Object *object, char typeChar, const QString &categoryId) {
		QVariantMap osmTags;

		// add properties first (so that they won't be able to overwrite any static data
		foreach (Osmium::OSM::Tag tag, object->tags()) {
			osmTags.insert(tag.key(), QString::fromUtf8(tag.value()));
		}

		// add static OSM stuff
		QString osmPrefixedId = QString("%1%2").arg(typeChar).arg(object->id());
		osmTags.insert("_id", osmPrefixedId);
		osmTags.insert("_rev", object->version());
		osmTags.insert("_user", QString::fromUtf8(object->user()));
		osmTags.insert("_uid", object->uid());

		// get the latitude and longitude
		double lat, lon;
		if (const Osmium::OSM::Node *node = dynamic_cast<const Osmium::OSM::Node*>(object)) {
			lat = node->get_lat();
			lon = node->get_lon();
		}
		else if (const Osmium::OSM::Way *way = dynamic_cast<const Osmium::OSM::Way*>(object)) {
			if (way->is_closed()) {
				Osmium::Geometry::Polygon polygon(way->nodes());
				geos::geom::Geometry *geometry = polygon.create_geos_geometry();
				geos::geom::Coordinate centoid;

				if (geometry->getCentroid(centoid)) {
					lat = centoid.y;
					lon = centoid.x;
				}
				else {
					throw EntityException();
				}

				delete geometry;
			}
			else if (way->node_count() > 0) {
				lat = way->get_lat(0);
				lon = way->get_lon(0);
			}
			else {
				throw EntityException();
			}
		}
		else {
			// Relations aren't supported yet
			throw EntityException();
		}

		osmTags.insert("_lat", lat);
		osmTags.insert("_lon", lon);

		// finally add the real stuff
		QVariantMap addressMap;
		QVariantMap posMap;
		posMap.insert("lat", lat);
		posMap.insert("lon", lon);
		addressMap.insert("pos", posMap);
		if (osmTags.contains("addr:city")) {
			addressMap.insert("city", osmTags["addr:city"]);
		}
		if (osmTags.contains("addr:country")) {
			addressMap.insert("countrycode", osmTags["addr:country"]);
		}
		if (osmTags.contains("addr:postcode")) {
			addressMap.insert("postalcode", osmTags["addr:postcode"]);
		}
		if (osmTags.contains("addr:street")) {
			addressMap.insert("street", osmTags["addr:street"]);
		}

		insert("address", addressMap);

		if (!categoryId.isEmpty()) {
			insert("categoryId", categoryId);
		}

		insert("docType", "shop");
		insert("osm", osmTags);
		insert("creatorId", Settings::getBotUID());

		// create title
		QString title;

		if (osmTags.contains("name")) {
			title = osmTags["name"].toString();
		}
		else if (osmTags.contains("operator")) {
			title = osmTags["operator"].toString();
		}

		if (!title.isEmpty()) {
			insert("title", title);
		}
		else {
			// no title could be extracted => fail
			throw EntityException();
		}
	}

signals:

public slots:

};

#endif // ENTITY_H
