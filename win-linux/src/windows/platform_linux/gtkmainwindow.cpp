#include <gtk/gtk.h>
#include "gtkmainwindow.h"
#include <QVBoxLayout>
#include <gtk/gtkx.h>
#include <functional>

typedef std::function<void(QCloseEvent*)> FnCloseEvent;


static gboolean on_configure_event(GtkWidget *wgt, GdkEvent *ev, gpointer data)
{
    if (QWidget *w = (QWidget*)data) {
        gint x = 0, y = 0;
        gtk_window_get_size(GTK_WINDOW(wgt), &x, &y);
        w->resize(x, y);
    }
    return FALSE;
}

static gboolean on_window_state_event(GtkWidget *wgt, GdkEventWindowState *ev, gpointer data)
{
    if (GdkWindowState *state = (GdkWindowState*)data)
        *state = ev->new_window_state;
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

class GtkMainWindow::GtkMainWindowPrivate
{
public:
    GtkMainWindowPrivate() {}
    ~GtkMainWindowPrivate() {}

    GtkWidget *wnd = nullptr;
    QWidget *cw = nullptr;
    GdkWindowState state = GDK_WINDOW_STATE_WITHDRAWN;
    FnCloseEvent close_event;
};

GtkMainWindow::GtkMainWindow(QWidget *parent) :
    QObject(parent),
    pimpl(new GtkMainWindowPrivate)
{
    gtk_init(NULL, NULL);
    pimpl->wnd = gtk_window_new(GTK_WINDOW_TOPLEVEL);
//    gtk_window_set_title(GTK_WINDOW(pimpl->wnd), "GtkMainWindow");
    g_signal_connect(G_OBJECT (pimpl->wnd), "window-state-event", G_CALLBACK(on_window_state_event), &pimpl->state);
    pimpl->close_event = std::bind(&GtkMainWindow::closeEvent, this, std::placeholders::_1);
    g_signal_connect(G_OBJECT (pimpl->wnd), "delete-event", G_CALLBACK(on_delete_event), &pimpl->close_event);
    gtk_window_set_position(GTK_WINDOW(pimpl->wnd), GtkWindowPosition::GTK_WIN_POS_CENTER);
//    g_signal_connect(G_OBJECT(pimpl->window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider, "decoration {border-radius: 3px 3px 0px 0px;}", -1, NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    GtkWidget *header = gtk_header_bar_new();
    gtk_window_set_titlebar(GTK_WINDOW(pimpl->wnd), header);
    gtk_widget_destroy(header);

    GtkWidget *socket = gtk_socket_new();
    gtk_widget_set_name(socket, "socket");
    gtk_widget_show(socket);
    gtk_container_add(GTK_CONTAINER(pimpl->wnd), socket);
//    gtk_widget_realize(socket);

    pimpl->cw = new QWidget(nullptr, /*Qt::FramelessWindowHint |*/ Qt::BypassWindowManagerHint);
    pimpl->cw->setObjectName("centralWidget");
    g_signal_connect(G_OBJECT(pimpl->wnd), "configure-event", G_CALLBACK(on_configure_event), pimpl->cw);
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

void GtkMainWindow::setGeometry(const QRect &rc)
{
    gtk_window_move(GTK_WINDOW(pimpl->wnd), rc.x(), rc.y());
    gtk_window_set_default_size(GTK_WINDOW(pimpl->wnd), rc.width(), rc.height());
//    gtk_window_resize(GTK_WINDOW(pimpl->wnd), rc.width(), rc.height());
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

void GtkMainWindow::show()
{
    gtk_widget_show_all(pimpl->wnd);
    pimpl->cw->show();
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

QString GtkMainWindow::windowTitle() const
{
    gtk_window_get_title(GTK_WINDOW(pimpl->wnd));
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
    if (pimpl->state & GDK_WINDOW_STATE_WITHDRAWN)
        ws.setFlag(Qt::WindowState::WindowNoState);
    if (pimpl->state & GDK_WINDOW_STATE_MAXIMIZED)
        ws.setFlag(Qt::WindowState::WindowMaximized);
    if (pimpl->state & GDK_WINDOW_STATE_ICONIFIED)
        ws.setFlag(Qt::WindowState::WindowMinimized);
    if (pimpl->state & GDK_WINDOW_STATE_FULLSCREEN)
        ws.setFlag(Qt::WindowState::WindowFullScreen);
    if (pimpl->state & GDK_WINDOW_STATE_FOCUSED)
        ws.setFlag(Qt::WindowState::WindowActive);
    return ws;
}

QWidget* GtkMainWindow::underlay() const
{
    return pimpl->cw;
}

void* GtkMainWindow::handle()
{
    return pimpl->wnd;
}

void GtkMainWindow::closeEvent(QCloseEvent *ev)
{
    ev->accept();
}
