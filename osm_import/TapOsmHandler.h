#include <osmium.hpp>
#include <osmium/handler/coordinates_for_ways.hpp>
#include <osmium/storage/byid.hpp>

#include "CategoryMapper.h"
#include "CouchOutput.h"

typedef Osmium::Storage::Mmap<Osmium::OSM::Position> TapOsmHandler_NodePosStorage;

class TapOsmHandler: public Osmium::Handler::Base {
private:
	int m_amenityCount;
	int m_shopCount;
	int m_bothCount;

	int m_nodeCount;
	int m_relationCount;
	int m_wayCount;

	const char *m_nodeStr;
	const char *m_relationStr;
	const char *m_wayStr;

	QMap<QString, CategoryMapper*> m_categoryMappers;

	TapOsmHandler_NodePosStorage m_posNodes;
	TapOsmHandler_NodePosStorage m_negNodes;

	Osmium::Handler::CoordinatesForWays<TapOsmHandler_NodePosStorage, TapOsmHandler_NodePosStorage> m_coordinateHandler;

public:
	/**
	 * \brief Constructor; Initializes the values of the statistic counters
	 */
	TapOsmHandler(): m_coordinateHandler(m_posNodes, m_negNodes) {
		m_amenityCount = 0;
		m_shopCount = 0;
		m_bothCount = 0;

		m_nodeCount = 0;
		m_wayCount = 0;
		m_relationCount = 0;

		m_nodeStr = "node";
		m_relationStr = "relation";
		m_wayStr = "way";

		_initMappers();
	}

	virtual ~TapOsmHandler() {
		foreach (CategoryMapper *mapper, m_categoryMappers) {
			delete mapper;
		}
	}

	void after_nodes() {
		m_coordinateHandler.after_nodes();
	}

	/**
	 * \brief Call _object() for the specified node
	 * \param node Osmium::OSM::Node to parse
	 */
	void node(const Osmium::OSM::Node *node) {
		m_coordinateHandler.node(node);
		_object(node, m_nodeStr);
	}

	/**
	 * \brief Call _object() for the specified way
	 * \param way Osmium::OSM::Way to parse
	 */
	void way(Osmium::OSM::Way *way) {
		m_coordinateHandler.way(way);
		_object(way, m_wayStr);
	}

	void _initMappers() {
		m_categoryMappers.insert("amenity", new CategoryMapper("mappers/amenity.ini"));
		m_categoryMappers.insert("shop", new CategoryMapper("mappers/shop.ini"));
	}

	void _object(const Osmium::OSM::Object *object, const char *typeStr) {
		const char *amenityValue = object->tags().get_tag_by_key("amenity");
		const char *shopValue = object->tags().get_tag_by_key("shop");
		const char *value = 0;
		QString categoryId;

		if (shopValue||amenityValue) {
			//printf("%s %i (rev: %i, user: %s)\n", typeStr, object->id(), object->version(), object->user());
			if (amenityValue) {
				//printf("\tamenity=%s\n", amenityValue);
				CategoryMapper *mapper = m_categoryMappers["amenity"];
				if (mapper->hasCategory(amenityValue)) {
					m_amenityCount++;
					value = amenityValue;
					categoryId = mapper->getCategoryId(amenityValue);
				}
			}
			if (shopValue) {
				//printf("\tshop=%s\n", shopValue);
				m_shopCount++;
				CategoryMapper *mapper = m_categoryMappers["shop"];
				if (mapper->hasCategory(shopValue)) {
					value = shopValue;
					categoryId = mapper->getCategoryId(shopValue);
				}
			}
			if (amenityValue && shopValue) {
				m_bothCount++;
			}

			if (typeStr == m_nodeStr) m_nodeCount++;
			else if (typeStr == m_relationStr) m_relationCount++;
			else if (typeStr == m_wayStr) m_wayCount++;

			if (value) {
				CouchOutput::addObject(Entity(object, typeStr, categoryId));
			}
		}
	}

	/**
	 * \brief print some statistics (after everything else has finished)
	 */
	void final() {
		printf("Statistics:\n");
		printf("\tAmenities: %i\n", m_amenityCount);
		printf("\tShops    : %i\n", m_shopCount);
		printf("\tBoth     : %i\n", m_bothCount);
		printf("\n");
		printf("Types:\n");
		printf("\tNodes: %i\n", m_nodeCount);
		printf("\tRelations: %i\n", m_relationCount);
		printf("\tWays: %i\n", m_wayCount);
	}
};
