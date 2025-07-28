#include <graphics.h>
#include <string>

int idx_current_anim = 0;
const int PLAYER_ANIM_COUNT = 6;

IMAGE img_player_left[PLAYER_ANIM_COUNT];
IMAGE img_player_right[PLAYER_ANIM_COUNT];

class Player{
    public:
        POINT pos;
        int speed = 10;
        Player(){
            pos.x = 500;
            pos.y = 500;
            speed = 10;
        }
        Player(int x, int y){
            pos.x = x;
            pos.y = y;
            speed = 10;
        }
};


void load_player_anim()
{
    for (size_t i = 0; i < PLAYER_ANIM_COUNT; i++)
    {
        std::wstring path = L"img/paimon_left_" + std::to_wstring(i) + L".png";
        loadimage(&img_player_left[i], path.c_str());
    }
    
    for (size_t i = 0; i < PLAYER_ANIM_COUNT; i++)
    {
        std::wstring path = L"img/paimon_right_" + std::to_wstring(i) + L".png";
        loadimage(&img_player_right[i], path.c_str());
    }
}

#pragma comment(lib, "MSIMG32.LIB")

inline void putimage_alpha(int x, int y, IMAGE *img)
{
    int w = img->getwidth();
    int h = img->getheight();
    AlphaBlend(GetImageHDC(NULL), x, y, w, h,
    GetImageHDC(img), 0, 0, w, h, {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA});
}

int main()
{
    initgraph(1280, 720);
    bool running = true;
    Player player1;

    BeginBatchDraw();
    
    load_player_anim();

    ExMessage msg;
    IMAGE background_img;
    loadimage(&background_img, _T("img/background.png"));

    while (running)
    {

        DWORD start_time = GetTickCount();

        while (peekmessage(&msg, EX_MOUSE | EX_KEY))
        {
            if(msg.message == WM_KEYDOWN){
                switch(msg.vkcode){
                    case VK_LEFT:
                        player1.pos.x -= player1.speed;
                        break;
                    case VK_RIGHT:
                        player1.pos.x += player1.speed;
                        break;
                    case VK_UP:
                        player1.pos.y -= player1.speed;
                        break;
                    case VK_DOWN:
                        player1.pos.y += player1.speed;
                        break;
                }
            }
        }

        static int count = 0;
        if (++count % 5 == 0)
            idx_current_anim++;

        // make animation loop
        idx_current_anim %= PLAYER_ANIM_COUNT;

        cleardevice();
        putimage(0, 0, &background_img);
        putimage_alpha(player1.pos.x, player1.pos.y, &img_player_left[idx_current_anim]);

        FlushBatchDraw();

        DWORD delta_time = GetTickCount() - start_time;
        if (delta_time < 1000 / 144)
        {
            Sleep(1000 / 14 - delta_time);
        }
    }
    EndBatchDraw();
    return 0;
}