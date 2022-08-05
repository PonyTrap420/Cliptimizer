#include <QApplication>

#include <cliptimizer.h>

int main(int argc, char* argv[]) {
	QApplication app(argc, argv);
	
	Cliptimizer* window = new Cliptimizer();

	window->show();

	return app.exec();
}