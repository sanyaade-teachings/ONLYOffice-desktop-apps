#ifndef GTKMAINWINDOW_H
#define GTKMAINWINDOW_H

#include <QWidget>
#include <QIcon>
#include <QCloseEvent>


class GtkMainWindow : public QObject
{
    Q_OBJECT
public:
    GtkMainWindow(QWidget *parent);
    ~GtkMainWindow();

    void setGeometry(const QRect &rc);
    void setWindowIcon(const QIcon &icon);
    void setWindowTitle(const QString &title);
    void setCentralWidget(QWidget *w);
    void setStyleSheet(const QString &css);
    void setLayoutDirection(Qt::LayoutDirection direct);
    void setFocus();
    void setAcceptDrops(bool);
    void setMouseTracking(bool);
    void show();
    void showMinimized();
    void showMaximized();
    void showNormal();
    void activateWindow();
    void setMinimumSize(int w, int h);
    void hide() const;
    bool isMaximized();
    bool isMinimized();
    bool isActiveWindow();
    bool isVisible() const;
    QString windowTitle() const;
    QPoint mapFromGlobal(const QPoint &pt) const;
    QSize size() const;
    QRect geometry() const;
    QRect normalGeometry() const;
    Qt::WindowStates windowState() const;
    QWidget *underlay() const;
    void *handle();

protected:
    virtual void closeEvent(QCloseEvent *ev);

private:
    class GtkMainWindowPrivate;
    GtkMainWindowPrivate *pimpl;
};

#endif // GTKMAINWINDOW_H
