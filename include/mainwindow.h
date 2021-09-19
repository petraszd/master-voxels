#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include <QtGui/QMainWindow>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
    private:
        Ui::MainWindow *m_ui;

    public:
        MainWindow(QWidget *parent = 0);
        ~MainWindow();

    protected:
        void changeEvent(QEvent *e);

    protected slots:
        void on_actionAbout_triggered();
        void on_actionAboutQt_triggered();
        void on_actionOpen_triggered();

    signals:
        void fileOpened(const QString&);

};

#endif

