#include <graphics.h>

#include <iostream>

int main()
{
    initgraph(1280, 720);

    int x = 300, y = 300;
    int radius = 10;

    ExMessage m;

    BeginBatchDraw();

    while (true)
    {
        m = getmessage(EX_MOUSE | EX_KEY);
        switch (m.message)
        {
        case WM_MOUSEMOVE:
            cleardevice();
            solidcircle(m.x, m.y, radius);
            break;
        case WM_MOUSEWHEEL:
            if (m.wheel > 0)
            {
                radius -= 2;
                solidcircle(m.x, m.y, radius);
            }
            else if (m.wheel < 0)
            {
                radius += 2;
                solidcircle(m.x, m.y, radius);
            }
            else
                solidcircle(m.x, m.y, radius);
            break;

        case WM_KEYDOWN:
            if (m.vkcode == VK_ESCAPE)
            {
                return 0;
            }
        }
        FlushBatchDraw();
    }
    EndBatchDraw();

    return 0;
}