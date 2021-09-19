#include <mainwindow.h>
#include <ui_mainwindow.h>
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_ui(new Ui::MainWindow)
{
    m_ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete m_ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange:
            m_ui->retranslateUi(this);
            break;
        default:
            break;
    }
}

void MainWindow::on_actionAbout_triggered()
{

    QMessageBox::about(this, tr("About"),
            tr("Voxel Rendering Demo application\n"
               "\n"
               "Author: petraszd\n"));
}

void MainWindow::on_actionAboutQt_triggered()
{
    QMessageBox::aboutQt(this, tr("About Qt"));
}

void MainWindow::on_actionOpen_triggered()
{
    QString name = QFileDialog::getOpenFileName(this,
            tr("Open 3D texture"), "./data", tr("All (*.*)"));
    if (!name.isEmpty()) {
        emit fileOpened(name);
    }
}

