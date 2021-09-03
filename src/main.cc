#include <QApplication>

#include "ToolkitApp.h"

#include <iostream>
#include <stdlib.h>

#include <toolkit_config.h>

using namespace std;

int main(int argc, char *argv[])
{
	#ifndef TOOLKIT_DEBUG
		putenv("QT_LOGGING_RULES=qt5ct.debug=false");
	#endif
	QApplication app(argc, argv);

	QCoreApplication::setOrganizationName("ORB");
	QCoreApplication::setApplicationName("rbdl-toolkit");
	QCoreApplication::setApplicationVersion(TOOKIT_VERSION);

	ToolkitApp *rbdl_toolkit = new ToolkitApp;

	//may terminate while parsing in errors if special options like help or version are used
	rbdl_toolkit->parseCmd(app);

	//start Gui 
	rbdl_toolkit->show();
	return app.exec();
}
