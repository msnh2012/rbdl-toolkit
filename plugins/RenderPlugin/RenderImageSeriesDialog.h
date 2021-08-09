#ifndef RENDERIMAGESERIESDIALOG_H
#define RENDERIMAGESERIESDIALOG_H

#include <QFileDialog>

#include "ui_RenderImageSeriesDialog.h"
#include "toolkit_config.h"

class TOOLKIT_PLUGIN_DLLAPI RenderImageSeriesDialog : public QDialog, public Ui::RenderImageSeriesDialog {
    Q_OBJECT
 
public:
		RenderImageSeriesDialog (QWidget *parent = 0) {
			setupUi(this);

			connect(filenameSelect, &QPushButton::clicked, this, &RenderImageSeriesDialog::action_select_directory);
		}

		void action_select_directory() {
			QFileDialog file_dialog (this, "Select Save Directory");
			file_dialog.setFileMode(QFileDialog::Directory);

			if (file_dialog.exec()) {
				filenameEdit->setText(file_dialog.selectedFiles().at(0));
			}
		}

};
#endif
