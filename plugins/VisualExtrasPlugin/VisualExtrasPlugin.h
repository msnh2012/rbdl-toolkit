#ifndef VISUALEXTRASPLUGIN_H_INCLUDED
#define VISUALEXTRASPLUGIN_H_INCLUDED

#include <QWidget>
#include <QString>
#include <QColor>

#include "toolkit_interfaces.h"
#include "toolkit_config.h"
#include "toolkit_errors.h"
#include "util.h"


class TOOLKIT_PLUGIN_DLLAPI VisualExtrasPlugin : public QObject, public CoreInterface {
	Q_OBJECT
	Q_INTERFACES(CoreInterface)
	Q_PLUGIN_METADATA(IID CoreInterface_iid FILE "metadata.json")
	public:
		VisualExtrasPlugin();
		virtual ~VisualExtrasPlugin();

		void init(ToolkitApp* app);
		
	private:
		ToolkitApp* parentApp;

		QColor modeltree_color;
		float modeltree_size;
		float frame_axis_size, frame_axis_diameter;
		
		void loadVisualExtrasSettings();

		void buildModelTreeWireframe(RBDLModelWrapper* model);
		void addJointFrameAxis(RBDLModelWrapper* model);
		
	public Q_SLOTS:
		
		void reload(RBDLModelWrapper* model);

};

#endif 
