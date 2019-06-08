#pragma once

#include <QDialog>
#include <QFileSystemWatcher>
#include <QDir>
#include <QStringList>
#include <QTimer>
#include <QFileInfo>

namespace Ui {
class UpdateDialog;
}

namespace f1x
{
namespace openauto
{
namespace autoapp
{
namespace ui
{

class UpdateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UpdateDialog(QWidget *parent = nullptr);
    ~UpdateDialog() override;

    void updateCheck();
    void downloadCheck();
    void updateProgress();

private slots:
    void on_pushButtonUpdateCsmt_clicked();
    void on_pushButtonUpdateUdev_clicked();
    void on_pushButtonUpdateOpenauto_clicked();
    void on_pushButtonUpdateSystem_clicked();
    void on_pushButtonUpdateCheck_clicked();
    void on_pushButtonUpdateCancel_clicked();

private:
    Ui::UpdateDialog *ui_;
    QFileSystemWatcher* watcher_tmp;
    QFileSystemWatcher* watcher_download;
};

}
}
}
}
