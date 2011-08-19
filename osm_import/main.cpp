#include "TapOsmHandler.h"

#include <QCoreApplication>

#include <osmium.hpp>
#include <cstdio>

int main(int argc, char *argv[]) {
	QCoreApplication app(argc, argv);
	if (argc <= 1) {
		fprintf(stderr, "USAGE: %s <infile>\n", argv[0]);
		exit(1);
	}

	Osmium::init();

	Osmium::OSMFile infile(argv[1]);

	TapOsmHandler handler;
	infile.read(handler);
}
