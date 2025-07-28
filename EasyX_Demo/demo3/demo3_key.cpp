#include <graphics.h>
#include <string>
#include <vector>

int idx_current_anim = 0;
const int PLAYER_ANIM_COUNT = 6;

const int PLAYER_WIDTH = 80;
const int PLAYER_HEIGHT = 80;
const int SHADOW_WIDTH = 32;

IMAGE img_player_left[PLAYER_ANIM_COUNT];
IMAGE img_player_right[PLAYER_ANIM_COUNT];

#pragma comment(lib, "MSIMG32.LIB")

inline void putimage_alpha(int x, int y, IMAGE *img)
{
    int w = img->getwidth();
    int h = img->getheight();
    AlphaBlend(GetImageHDC(NULL), x, y, w, h,
               GetImageHDC(img), 0, 0, w, h, {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA});
}

class Animation
{
public:
    Animation(LPCTSTR path, int num, int interval)
    {
        interval_ms = interval;
        timer = 0;
        idx_frame = 0;

        TCHAR path_file[256];
        for (size_t i = 0; i < num; ++i)
        {
            _stprintf_s(path_file, L"%s_%zd.png", path, i);

            IMAGE *frame = new IMAGE;
            loadimage(frame, path_file);
            frame_list.push_back(frame);
        }
    }
    ~Animation()
    {
        for (size_t i = 0; i < frame_list.size(); ++i)
            delete frame_list[i];
    }
    void play(int x, int y, int delta)
    {
        timer += delta;
        if (timer >= interval_ms)
        {
            idx_frame = (idx_frame + 1) % frame_list.size();
            timer = 0;
        }

        putimage_alpha(x, y, frame_list[idx_frame]);
    }

private:
    std::vector<IMAGE *> frame_list;
    int timer;
    int interval_ms;
    int idx_frame;
};

void load_animation()
{
    for (size_t i = 0; i < PLAYER_ANIM_COUNT; ++i)
    {
        std::wstring path = L"img/paimon_left_" + std::to_wstring(i) + L".png";
        loadimage(&img_player_left[i], path.c_str());
    }
    for (size_t i = 0; i < PLAYER_ANIM_COUNT; ++i)
    {
        std::wstring path = L"img/paimon_right_" + std::to_wstring(i) + L".png";
        loadimage(&img_player_right[i], path.c_str());
    }
}

class Player
{
public:
    POINT pos;
    int speed = 10;
    Player()
    {
        pos.x = 500;
        pos.y = 500;
        speed = 10;
    }
    Player(int x, int y)
    {
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

void draw_player(Player &player, int delta, int dir_x, Animation &anim_left, Animation &anim_right, IMAGE &shadow_img)
{
    static bool facing_left = false;

    int pos_shadow_x = player.pos.x + (PLAYER_WIDTH - SHADOW_WIDTH) / 2;
    int pos_shadow_y = player.pos.y + PLAYER_HEIGHT - 8;

    putimage_alpha(pos_shadow_x, pos_shadow_y, &shadow_img);

    if (dir_x > 0)
    {
        facing_left = false;
    }
    else if (dir_x < 0)
    {
        facing_left = true;
    }

    if (facing_left)
        anim_left.play(player.pos.x, player.pos.y, delta);
    else
        anim_right.play(player.pos.x, player.pos.y, delta);
}

int main()
{
    initgraph(1280, 720);
    bool running = true;
    Player player1;

    Animation anim_player_left(_T("img/paimon_left"), PLAYER_ANIM_COUNT, 100);
    Animation anim_player_right(_T("img/paimon_right"), PLAYER_ANIM_COUNT, 100);

    BeginBatchDraw();

    load_player_anim();

    ExMessage msg;
    IMAGE background_img;
    IMAGE shadow_img;

    loadimage(&background_img, _T("img/background.png"));
    loadimage(&shadow_img, _T("img/shadow_player.png"));

    bool is_left = false, is_right = false, is_up = false, is_down = false;

    while (running)
    {

        DWORD start_time = GetTickCount();

        while (peekmessage(&msg, EX_MOUSE | EX_KEY))
        {
            if (msg.message == WM_KEYDOWN)
            {
                switch (msg.vkcode)
                {
                case VK_LEFT:
                    is_left = true;
                    break;
                case VK_RIGHT:
                    is_right = true;
                    break;
                case VK_UP:
                    is_up = true;
                    break;
                case VK_DOWN:
                    is_down = true;
                    break;
                }
            }
            if (msg.message == WM_KEYUP)
            {
                switch (msg.vkcode)
                {
                case VK_LEFT:
                    is_left = false;
                    break;
                case VK_RIGHT:
                    is_right = false;
                    break;
                case VK_UP:
                    is_up = false;
                    break;
                case VK_DOWN:
                    is_down = false;
                    break;
                }
            }
        }

        if (is_left)
            player1.pos.x -= player1.speed;
        if (is_right)
            player1.pos.x += player1.speed;
        if (is_up)
            player1.pos.y -= player1.speed;
        if (is_down)
            player1.pos.y += player1.speed;

        static int count = 0;
        if (++count % 5 == 0)
            idx_current_anim++;

        // make animation loop
        idx_current_anim %= PLAYER_ANIM_COUNT;

        cleardevice();
        putimage(0, 0, &background_img);
        // putimage_alpha(player1.pos.x, player1.pos.y, &img_player_left[idx_current_anim]);
        draw_player(player1, 1000 / 144, is_left ? -1 : 1, anim_player_left, anim_player_right, shadow_img);

        FlushBatchDraw();

        DWORD delta_time = GetTickCount() - start_time;
        if (delta_time < 1000 / 144)
        {
            Sleep(1000 / 144 - delta_time);
        }
    }
    EndBatchDraw();
    return 0;
}