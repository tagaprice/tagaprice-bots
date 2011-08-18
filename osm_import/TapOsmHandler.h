#include "CategoryMapper.h"
#include "CouchOutput.h"

#include <QDir>

#include <osmium.hpp>
#include <osmium/handler/coordinates_for_ways.hpp>
#include <osmium/storage/byid.hpp>

typedef Osmium::Storage::Mmap<Osmium::OSM::Position> TapOsmHandler_NodePosStorage;

class TapOsmHandler: public Osmium::Handler::Base {
private:
	int m_nodeCount;
	int m_wayCount;

	const char *m_nodeStr;
	const char *m_wayStr;

	QMap<QString, CategoryMapper*> m_categoryMappers;
	QMap<QString, int> m_tagStats;

	TapOsmHandler_NodePosStorage m_posNodes;
	TapOsmHandler_NodePosStorage m_negNodes;

	Osmium::Handler::CoordinatesForWays<TapOsmHandler_NodePosStorage, TapOsmHandler_NodePosStorage> m_coordinateHandler;

public:
	/**
	 * \brief Constructor; Initializes the values of the statistic counters
	 */
	TapOsmHandler(): m_coordinateHandler(m_posNodes, m_negNodes) {
		m_nodeCount = 0;
		m_wayCount = 0;

		m_nodeStr = "node";
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
		QDir mappersDir("mappers/");
		foreach (QString filename, mappersDir.entryList(QStringList("*.ini"), QDir::Files)) {
			QFileInfo fileInfo(mappersDir.absoluteFilePath(filename));
			QString tagName = fileInfo.baseName();
			m_categoryMappers.insert(tagName, new CategoryMapper(fileInfo.absoluteFilePath()));
			m_tagStats.insert(tagName, 0);
		}
	}

	void _object(const Osmium::OSM::Object *object, const char *typeStr) {
		const char *value = 0;
		QString categoryId;

		// try to find a matching tag
		foreach (QString tagName, m_categoryMappers.keys()) {
			CategoryMapper *mapper = m_categoryMappers[tagName];
			const char *curValue = object->tags().get_tag_by_key(tagName.toUtf8().constData());
			if (curValue) {
				if (mapper->hasCategory(curValue)) {
					QString curCategoryId = mapper->getCategoryId(curValue);
					value = curValue;
					m_tagStats[tagName]++;

					if (!curCategoryId.isEmpty()) {
						categoryId = curCategoryId;
						break;
					}
				}
			}
		}

		if (value) {
			if (typeStr == m_nodeStr) m_nodeCount++;
			else if (typeStr == m_wayStr) m_wayCount++;

			CouchOutput::addObject(Entity(object, typeStr, categoryId));
		}
	}

	/**
	 * \brief print some statistics (after everything else has finished)
	 */
	void final() {
		fprintf(stderr, "Statistics:\n");
		foreach (QString tagName, m_tagStats.keys()) {
			fprintf(stderr, "\t%s: %i\n", tagName.toLocal8Bit().constData(), m_tagStats[tagName]);
		}
		fprintf(stderr, "\n");
		fprintf(stderr, "Types:\n");
		fprintf(stderr, "\tNodes: %i\n", m_nodeCount);
		fprintf(stderr, "\tWays: %i\n", m_wayCount);
	}
};
