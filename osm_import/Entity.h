#ifndef ENTITY_H
#define ENTITY_H

#include <QVariantMap>

#include <geos.h>
#include <geos/algorithm/CentroidArea.h>
#include <osmium.hpp>

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
		osm_object_type_t type = object->get_type();
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
					clear();
					return;
				}

				delete geometry;
			}
			else if (way->node_count() > 0) {
				lat = way->get_lat(0);
				lon = way->get_lon(0);
			}
			else {
				clear();
				return;
			}
		}
		else {
			// Relations aren't supported yet
			clear();
			return;
		}

		osmTags.insert("_lat", lat);
		osmTags.insert("_lon", lon);

		insert("osm", osmTags);

		// finally add the real stuff
		insert("lat", lat);
		insert("lng", lon);
		if (!categoryId.isEmpty()) {
			insert("shopCategoryId", categoryId);
		}
		insert("user", "osmImportBot");
		insert("docType", "shop");
	}

signals:

public slots:

};

#endif // ENTITY_H
