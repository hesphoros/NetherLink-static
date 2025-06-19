#pragma once
#pragma comment(lib, "dwmapi.lib")
#include <QWidget>
#include <dwmapi.h>
#include <WinUser.h>
#include <windowsx.h>

constexpr int RESIZE_WINDOW_WIDTH = 8;

#ifndef _MSC_VER
// 非 MSVC 下，保持原先写法（static/internal linkage）
#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
enum DWM_WINDOW_CORNER_PREFERENCE {
    DWMWCP_DEFAULT    = 0,
    DWMWCP_DONOTROUND = 1,
    DWMWCP_ROUND      = 2,
    DWMWCP_ROUNDSMALL = 3
};
constexpr DWORD DWMWA_WINDOW_CORNER_PREFERENCE = 33;
#endif
#endif

class FramelessWindow : public QWidget {
    Q_OBJECT
public:
    explicit FramelessWindow(QWidget* parent = nullptr);
    void setTitleBar(QWidget* titleBar);
protected:
    bool nativeEvent(const QByteArray& eventType, void* message, qint64* result) override;
    bool event(QEvent* event) override;
private:
    int adjustResizeWindow(const QPoint& pos);
    QWidget* m_titleBar = nullptr;
    bool m_isMaximized = false;
};