// FramelessWindow.cpp
#include "FramelessWindow.h"
#include <QEvent>

FramelessWindow::FramelessWindow(QWidget* parent)
        : QWidget(parent)
{
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
    setAttribute(Qt::WA_TranslucentBackground);

    HWND hwnd = HWND(winId());
    LONG style = ::GetWindowLong(hwnd, GWL_STYLE);
    ::SetWindowLong(hwnd, GWL_STYLE,
                    style | WS_THICKFRAME | WS_CAPTION | WS_MAXIMIZEBOX);

    UINT preference = DWMWCP_ROUND;
    DwmSetWindowAttribute(hwnd,
                          DWMWA_WINDOW_CORNER_PREFERENCE,
                          &preference, sizeof(preference));
}

void FramelessWindow::setTitleBar(QWidget* titleBar)
{
    m_titleBar = titleBar;
}

bool FramelessWindow::nativeEvent(const QByteArray& eventType, void* message, qint64* result)
{
    MSG* msg = reinterpret_cast<MSG*>(message);
    switch (msg->message) {
        case WM_NCCALCSIZE:
            *result = 0;
            return true;

        case WM_NCHITTEST: {
            const LONG gx = GET_X_LPARAM(msg->lParam);
            const LONG gy = GET_Y_LPARAM(msg->lParam);
            QPoint globalPos(gx, gy);

            int hit = adjustResizeWindow(globalPos);
            if (hit) {
                *result = hit;
                return true;
            }

            // 标题栏拖拽检测
            if (m_titleBar) {
                double dpr = devicePixelRatioF();
                QPoint local = m_titleBar->mapFromGlobal(QPoint(gx / dpr, gy / dpr));
                if (m_titleBar->rect().contains(local)) {
                    QWidget* child = m_titleBar->childAt(local);
                    if (!child) {
                        *result = HTCAPTION;
                        return true;
                    }
                }
            }
            return false;
        }
        default:
            return QWidget::nativeEvent(eventType, message, result);
    }
}

bool FramelessWindow::event(QEvent* event)
{
    if (event->type() == QEvent::WindowStateChange) {
        bool maximized = isMaximized();
        if (maximized != m_isMaximized) {
            m_isMaximized = maximized;
            if (maximized) {
                setContentsMargins(0, 0, 0, 0);
            } else {
                setContentsMargins(8, 8, 8, 8);
            }
        }
    }
    return QWidget::event(event);
}

int FramelessWindow::adjustResizeWindow(const QPoint& pos)
{
    int result = 0;

    RECT winrect;
    GetWindowRect(HWND(this->winId()), &winrect);

    int mouse_x = pos.x();
    int mouse_y = pos.y();

    bool resizeWidth = this->minimumWidth() != this->maximumWidth();
    bool resizeHieght = this->minimumHeight() != this->maximumHeight();

    if (resizeWidth) {
        if (mouse_x > winrect.left && mouse_x < winrect.left + RESIZE_WINDOW_WIDTH)
            result = HTLEFT;
        if (mouse_x < winrect.right && mouse_x >= winrect.right - RESIZE_WINDOW_WIDTH)
            result = HTRIGHT;
    }
    if (resizeHieght) {
        if (mouse_y < winrect.top + RESIZE_WINDOW_WIDTH && mouse_y >= winrect.top)
            result = HTTOP;

        if (mouse_y <= winrect.bottom && mouse_y > winrect.bottom - RESIZE_WINDOW_WIDTH)
            result = HTBOTTOM;
    }
    if (resizeWidth && resizeHieght) {
        // topleft corner
        if (mouse_x >= winrect.left && mouse_x < winrect.left + RESIZE_WINDOW_WIDTH && mouse_y >= winrect.top && mouse_y < winrect.top + RESIZE_WINDOW_WIDTH) {
            result = HTTOPLEFT;
        }
        // topRight corner
        if (mouse_x <= winrect.right && mouse_x > winrect.right - RESIZE_WINDOW_WIDTH && mouse_y >= winrect.top && mouse_y < winrect.top + RESIZE_WINDOW_WIDTH)
            result = HTTOPRIGHT;
        // leftBottom  corner
        if (mouse_x >= winrect.left && mouse_x < winrect.left + RESIZE_WINDOW_WIDTH && mouse_y <= winrect.bottom && mouse_y > winrect.bottom - RESIZE_WINDOW_WIDTH)
            result = HTBOTTOMLEFT;
        // rightbottom  corner
        if (mouse_x <= winrect.right && mouse_x > winrect.right - RESIZE_WINDOW_WIDTH && mouse_y <= winrect.bottom && mouse_y > winrect.bottom - RESIZE_WINDOW_WIDTH)
            result = HTBOTTOMRIGHT;
    }
    return result;
}
