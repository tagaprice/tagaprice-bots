#include <osmium.hpp>

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

public:
	TapOsmHandler() {
		m_amenityCount = 0;
		m_shopCount = 0;
		m_bothCount = 0;

		m_nodeCount = 0;
		m_wayCount = 0;
		m_relationCount = 0;

		m_nodeStr = "Node";
		m_relationStr = "Relation";
		m_wayStr = "Way";
	}

	void node(const Osmium::OSM::Node *node) {
		_object(node, m_nodeStr);
	}

	void relation(const Osmium::OSM::Relation *relation) {
		_object(relation, m_relationStr);
	}

	void way(const Osmium::OSM::Way *way) {
		_object(way, m_wayStr);
	}

	void _object(const Osmium::OSM::Object *object, const char *typeStr) {
		const char *amenityValue = object->tags().get_tag_by_key("amenity");
		const char *shopValue = object->tags().get_tag_by_key("shop");
		if (shopValue||amenityValue) {
			printf("%s %i (rev: %i, user: %s)\n", typeStr, object->id(), object->version(), object->user());
			if (amenityValue) {
				printf("\tamenity=%s\n", amenityValue);
				m_amenityCount++;
			}
			if (shopValue) {
				printf("\tshop=%s\n", shopValue);
				m_shopCount++;
			}
			if (amenityValue && shopValue) {
				m_bothCount++;
			}

			if (typeStr == m_nodeStr) m_nodeCount++;
			else if (typeStr == m_relationStr) m_relationCount++;
			else if (typeStr == m_wayStr) m_wayCount++;
		}
	}

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
