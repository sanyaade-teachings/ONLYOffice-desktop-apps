#include <gtk/gtk.h>
#include "gtkmainwindow.h"
#include <QApplication>
#include <QTimer>
#include <QVBoxLayout>
#include <gtk/gtkx.h>
#include <functional>

typedef std::function<bool(QEvent *ev)> FnEvent;
typedef std::function<bool(const QByteArray&, void*, long*)> FnNativeEvent;
typedef std::function<void(QCloseEvent*)> FnCloseEvent;
typedef std::function<void(QShowEvent*)> FnShowEvent;


class QUnderlay : public QWidget
{
public:
    QUnderlay(QWidget *p, Qt::WindowFlags f, const FnEvent &event, const FnNativeEvent &native_event, const FnShowEvent &show_event) :
        QWidget(p, f),
        m_event(event),
        m_native_event(native_event),
        m_show_event(show_event)
    {}
    ~QUnderlay()
    {}
    virtual bool event(QEvent *ev) override
    {
        if (m_event(ev))
            return true;
        return QWidget::event(ev);
    }
    virtual bool nativeEvent(const QByteArray &ev_type, void *msg, long *res) override
    {
        if (m_native_event(ev_type, msg, res))
            return true;
        return QWidget::nativeEvent(ev_type, msg, res);
    }
    virtual void showEvent(QShowEvent* ev) override
    {
        QWidget::showEvent(ev);
        m_show_event(ev);
    }

private:
    FnEvent m_event;
    FnNativeEvent m_native_event;
    FnShowEvent m_show_event;
};

class GtkMainWindowPrivate
{
public:
    GtkMainWindowPrivate() {}
    ~GtkMainWindowPrivate()
    {
        delete cw, cw = nullptr;
        gtk_widget_destroy(GTK_WIDGET(wnd));
        wnd = nullptr;
    }
    GtkWidget *wnd = nullptr;
    QUnderlay *cw = nullptr;
    guint state = 0;
    FnCloseEvent close_event;
    FnEvent event;
};

static gboolean on_configure_event(GtkWidget *wgt, GdkEvent *ev, gpointer data)
{
    if (QWidget *w = (QWidget*)data) {
        gint x = 0, y = 0;
        gtk_window_get_size(GTK_WINDOW(wgt), &x, &y);
        gint f = gtk_widget_get_scale_factor(wgt);
        w->resize(f*x, f*y);
    }
    return FALSE;
}

static gboolean on_focus_in_event(gpointer data)
{
    if (QWidget *w = (QWidget*)data)
        qApp->postEvent(w, new QEvent(Event_GtkFocusIn));
    return FALSE;
}

static gboolean on_window_state_event(GtkWidget *wgt, GdkEventWindowState *ev, gpointer data)
{
    if (GtkMainWindowPrivate *pimpl = (GtkMainWindowPrivate*)data) {
        guint state = guint(ev->new_window_state) & (GDK_WINDOW_STATE_ICONIFIED | GDK_WINDOW_STATE_MAXIMIZED | GDK_WINDOW_STATE_FULLSCREEN);
        if (pimpl->state != state) {
            pimpl->state = state;
            QTimer::singleShot(0, qApp, [pimpl]() {
                QEvent ev(QEvent::WindowStateChange);
                pimpl->event(&ev);
            });
        }
    }
    return FALSE;
}

static gboolean on_delete_event(GtkWidget *wgt, GdkEvent* ev, gpointer data)
{
    if (FnCloseEvent *close_event = (FnCloseEvent*)data) {
        QCloseEvent ce;
        (*close_event)(&ce);
        return !ce.isAccepted();
    }
    return FALSE;
}

static void on_destroy(GtkWidget *wgt, gpointer data)
{

}

GtkMainWindow::GtkMainWindow(QWidget *parent) :
    QObject(parent),
    pimpl(new GtkMainWindowPrivate)
{
//    gtk_init(NULL, NULL);
    pimpl->wnd = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_type_hint(GTK_WINDOW(pimpl->wnd), GdkWindowTypeHint::GDK_WINDOW_TYPE_HINT_NORMAL);
//    gtk_window_set_title(GTK_WINDOW(pimpl->wnd), "GtkMainWindow");
    pimpl->event = std::bind(&GtkMainWindow::event, this, std::placeholders::_1);
    g_signal_connect(G_OBJECT (pimpl->wnd), "window-state-event", G_CALLBACK(on_window_state_event), pimpl);
    pimpl->close_event = std::bind(&GtkMainWindow::closeEvent, this, std::placeholders::_1);
    g_signal_connect(G_OBJECT (pimpl->wnd), "delete-event", G_CALLBACK(on_delete_event), &pimpl->close_event);
    g_signal_connect(G_OBJECT(pimpl->wnd), "destroy", G_CALLBACK(on_destroy), NULL);
    gtk_window_set_position(GTK_WINDOW(pimpl->wnd), GtkWindowPosition::GTK_WIN_POS_CENTER);

    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider, "decoration {border-radius: 3px 3px 0px 0px;} window {opacity: 0.8;}", -1, NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    GtkWidget *header = gtk_header_bar_new();
    gtk_window_set_titlebar(GTK_WINDOW(pimpl->wnd), header);
    gtk_widget_destroy(header);

    GtkWidget *socket = gtk_socket_new();
    gtk_widget_set_name(socket, "socket");
    gtk_widget_show(socket);
    gtk_container_add(GTK_CONTAINER(pimpl->wnd), socket);
//    gtk_widget_realize(socket);

    pimpl->cw = new QUnderlay(nullptr, /*Qt::FramelessWindowHint |*/ Qt::BypassWindowManagerHint,
                              std::bind(&GtkMainWindow::event, this, std::placeholders::_1),
                              std::bind(&GtkMainWindow::nativeEvent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
                              std::bind(&GtkMainWindow::showEvent, this, std::placeholders::_1));
    pimpl->cw->setObjectName("underlay");
    g_signal_connect(G_OBJECT(pimpl->wnd), "configure-event", G_CALLBACK(on_configure_event), pimpl->cw);
    g_signal_connect_swapped(G_OBJECT(pimpl->wnd), "focus-in-event", G_CALLBACK(on_focus_in_event), pimpl->cw);
    QVBoxLayout *lut = new QVBoxLayout;
    lut->setContentsMargins(0,0,0,0);
    pimpl->cw->setLayout(lut);
    pimpl->cw->createWinId();
    gtk_socket_add_id(GTK_SOCKET(socket), (Window)pimpl->cw->winId());
}

GtkMainWindow::~GtkMainWindow()
{
    delete pimpl;
}

void GtkMainWindow::move(const QPoint &pos)
{
    gtk_window_move(GTK_WINDOW(pimpl->wnd), pos.x(), pos.y());
//     gtk_widget_queue_draw(pimpl->wnd);
//     gtk_widget_queue_allocate(pimpl->wnd);
}

void GtkMainWindow::setGeometry(const QRect &rc)
{
    gint f = gtk_widget_get_scale_factor(pimpl->wnd);
//     gtk_window_set_default_size(GTK_WINDOW(pimpl->wnd), rc.width()/f, rc.height()/f);
//     gtk_widget_queue_resize(pimpl->wnd);
    GdkWindow *gdk_wnd = gtk_widget_get_window(pimpl->wnd);
    gdk_window_move_resize(gdk_wnd, rc.x(), rc.y(), rc.width()/f, rc.height()/f);
//     gtk_window_resize(GTK_WINDOW(pimpl->wnd), rc.width()/f, rc.height()/f);
//     gtk_window_move(GTK_WINDOW(pimpl->wnd), rc.x()/f, rc.y()/f);
//     gtk_widget_queue_draw(pimpl->wnd);
//     gtk_widget_queue_allocate(pimpl->wnd);
}

void GtkMainWindow::setWindowIcon(const QIcon &icon)
{
    if (!icon.isNull()) {
        QImage img = icon.pixmap(96, 96).toImage().rgbSwapped();
        if (!img.isNull()) {
            GdkPixbuf *pb = gdk_pixbuf_new_from_data(img.constBits(), GDK_COLORSPACE_RGB, TRUE, 8, img.width(), img.height(), img.bytesPerLine(), NULL, NULL);
            if (pb) {
                gtk_window_set_icon(GTK_WINDOW(pimpl->wnd), pb);
                g_object_unref(pb);
            }
        }
    }
}

void GtkMainWindow::setWindowTitle(const QString &title)
{
    gtk_window_set_title(GTK_WINDOW(pimpl->wnd), title.toLocal8Bit().constData());
}

void GtkMainWindow::setCentralWidget(QWidget *w)
{
    pimpl->cw->layout()->addWidget(w);
}

void GtkMainWindow::setStyleSheet(const QString &css)
{
    pimpl->cw->setStyleSheet(css);
}

void GtkMainWindow::setLayoutDirection(Qt::LayoutDirection direct)
{
    pimpl->cw->setLayoutDirection(direct);
}

void GtkMainWindow::setFocus()
{
    pimpl->cw->setFocus();
}

void GtkMainWindow::setAcceptDrops(bool enable)
{
    pimpl->cw->setAcceptDrops(enable);
}

void GtkMainWindow::setMouseTracking(bool enable)
{
    pimpl->cw->setMouseTracking(enable);
}

void GtkMainWindow::setWindowState(Qt::WindowStates ws)
{
    if (ws.testFlag(Qt::WindowMaximized))
        gtk_window_maximize(GTK_WINDOW(pimpl->wnd));
    if (ws.testFlag(Qt::WindowMinimized))
        gtk_window_iconify(GTK_WINDOW(pimpl->wnd));
    if (ws.testFlag(Qt::WindowFullScreen))
        gtk_window_fullscreen(GTK_WINDOW(pimpl->wnd));
    if (ws.testFlag(Qt::WindowActive))
        gtk_window_present(GTK_WINDOW(pimpl->wnd));
}

void GtkMainWindow::setFocusPolicy(Qt::FocusPolicy fp)
{
    pimpl->cw->setFocusPolicy(fp);
}

void GtkMainWindow::show()
{
    gtk_widget_show_all(pimpl->wnd);
    GdkWindow *gdk_wnd = gtk_widget_get_window(pimpl->wnd);
    Window xid = GDK_WINDOW_XID(gdk_wnd);
    pimpl->cw->setProperty("gtk_window_xid", QVariant::fromValue(xid));
    pimpl->cw->show();
    while (gtk_events_pending())
        gtk_main_iteration_do(FALSE);

}

void GtkMainWindow::showMinimized()
{
    gtk_window_iconify(GTK_WINDOW(pimpl->wnd));
}

void GtkMainWindow::showMaximized()
{
    gtk_window_maximize(GTK_WINDOW(pimpl->wnd));
}

void GtkMainWindow::showNormal()
{
    if (gtk_window_is_maximized(GTK_WINDOW(pimpl->wnd)))
        gtk_window_unmaximize(GTK_WINDOW(pimpl->wnd));
    gtk_window_present(GTK_WINDOW(pimpl->wnd));
}

void GtkMainWindow::activateWindow()
{
    pimpl->cw->activateWindow();
}

void GtkMainWindow::setMinimumSize(int w, int h)
{
    gtk_widget_set_size_request(pimpl->wnd, w, h);
}

void GtkMainWindow::updateGeometry()
{
    pimpl->cw->updateGeometry();
}

void GtkMainWindow::update()
{
    pimpl->cw->update();
}

void GtkMainWindow::hide() const
{
    gtk_widget_hide(pimpl->wnd);
}

bool GtkMainWindow::isMaximized()
{
    return gtk_window_is_maximized(GTK_WINDOW(pimpl->wnd));
}

bool GtkMainWindow::isMinimized()
{
    return pimpl->state & GDK_WINDOW_STATE_ICONIFIED;
}

bool GtkMainWindow::isActiveWindow()
{
    return gtk_window_is_active(GTK_WINDOW(pimpl->wnd));
}

bool GtkMainWindow::isVisible() const
{
    return gtk_widget_is_visible(pimpl->wnd);
}

bool GtkMainWindow::isHidden() const
{
    return !gtk_widget_is_visible(pimpl->wnd);
}

QString GtkMainWindow::windowTitle() const
{
    return gtk_window_get_title(GTK_WINDOW(pimpl->wnd));
}

QPoint GtkMainWindow::mapFromGlobal(const QPoint &pt) const
{
    return pimpl->cw->mapFromGlobal(pt);
}

QSize GtkMainWindow::size() const
{
    return pimpl->cw->size();
}

QRect GtkMainWindow::geometry() const
{
//    gint x, y, w, h;
//    gtk_window_get_position(GTK_WINDOW(pimpl->wnd), &x, &y);
//    gtk_window_get_size(GTK_WINDOW(pimpl->wnd), &w, &h);
//    return QRect(QPoint(x, y), QSize(w, h));
    return pimpl->cw->geometry();
}

QRect GtkMainWindow::normalGeometry() const
{
    //    gint x, y, w, h;
    //    gtk_window_get_position(GTK_WINDOW(pimpl->wnd), &x, &y);
    //    gtk_window_get_size(GTK_WINDOW(pimpl->wnd), &w, &h);
    //    return QRect(QPoint(x, y), QSize(w, h));
    return pimpl->cw->normalGeometry();
}

Qt::WindowStates GtkMainWindow::windowState() const
{
    Qt::WindowStates ws;
    if (pimpl->state == 0)
        ws.setFlag(Qt::WindowNoState);
    if (pimpl->state & GDK_WINDOW_STATE_MAXIMIZED)
        ws.setFlag(Qt::WindowMaximized);
    if (pimpl->state & GDK_WINDOW_STATE_ICONIFIED)
        ws.setFlag(Qt::WindowMinimized);
    if (pimpl->state & GDK_WINDOW_STATE_FULLSCREEN)
        ws.setFlag(Qt::WindowFullScreen);
//    if (pimpl->state & GDK_WINDOW_STATE_FOCUSED)
//        ws.setFlag(Qt::WindowActive);
    return ws;
}

QWidget* GtkMainWindow::underlay() const
{
    return pimpl->cw;
}

void* GtkMainWindow::handle() const
{
    return pimpl->wnd;
}

bool GtkMainWindow::event(QEvent *ev)
{
    return QObject::event(ev);
}

bool GtkMainWindow::nativeEvent(const QByteArray &ev_type, void *msg, long *res)
{
    return false;
}

void GtkMainWindow::closeEvent(QCloseEvent *ev)
{
    ev->accept();
}

void GtkMainWindow::showEvent(QShowEvent *ev)
{
    ev->accept();
}
