#include <graphics.h>
#include <string>
#include <vector>

int idx_current_anim = 0;
const int PLAYER_ANIM_COUNT = 6;

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

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

class Bullet{
    public:
        POINT pos = {0, 0};

    public:
        Bullet() = default;
        ~Bullet() = default;

        void Draw()const
        {
            setlinecolor(RGB(255, 155, 50));
            setfillcolor(RGB(200, 75, 10));
            fillcircle(pos.x, pos.y, RADIUS);
        }

    private:
        const int RADIUS = 10;
};

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
    Player()
    {
        pos.x = 500;
        pos.y = 500;
        speed = 10;
        loadimage(&shadow_img, _T("img/shadow_player.png"));
        anim_left = new Animation(_T("img/paimon_left"), PLAYER_ANIM_COUNT, 100);
        anim_right = new Animation(_T("img/paimon_right"), PLAYER_ANIM_COUNT, 100);
    }
    ~Player()
    {
        delete anim_left;
        delete anim_right;
    }

    void processEvent(ExMessage& msg)
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

    void Move()
    {
        int dir_x = is_left ? -1 : is_right ? 1
                                            : 0;
        int dir_y = is_up ? -1 : is_down ? 1
                                         : 0;
        double len_dir = sqrt(pow(dir_x, 2) + pow(dir_y, 2));

        if (len_dir > 0)
        {
            double normalized_x = dir_x / len_dir;
            double normalized_y = dir_y / len_dir;
            pos.x += int(normalized_x * speed);
            pos.y += int(normalized_y * speed);
        }

        if (pos.x < 0)
            pos.x = 0;
        if (pos.y < 0)
            pos.y = 0;
        if (pos.x > WINDOW_WIDTH - FRAME_WIDTH)
            pos.x = WINDOW_WIDTH - FRAME_WIDTH;
        if (pos.y > WINDOW_HEIGHT - FRAME_HEIGHT)
            pos.y = WINDOW_HEIGHT - FRAME_HEIGHT;
    }

    void Draw(int delta)
    {
        int pos_shadow_x = pos.x + (FRAME_WIDTH - SHADOW_WIDTH) / 2;
        int pos_shadow_y = pos.y + FRAME_HEIGHT - 8;
        putimage_alpha(pos_shadow_x, pos_shadow_y, &shadow_img);

        static bool facing_left = false;

        int dir_x = is_left - is_right;

        if (dir_x > 0)
        {
            facing_left = false;
        }
        else if (dir_x < 0)
        {
            facing_left = true;
        }

        if (facing_left)
            anim_left->play(pos.x, pos.y, delta);
        else
            anim_right->play(pos.x, pos.y, delta);
    }

    const POINT& GetPos() const
    {
        return pos;
    }


private:
    const int FRAME_WIDTH = 80;
    const int FRAME_HEIGHT = 80;
    const int SHADOW_WIDTH = 32;
private:
    POINT pos;
    int speed = 5;  // todo:in next version,add some increase effect tools
    IMAGE shadow_img;
    Animation* anim_left;
    Animation* anim_right;
    bool is_left = false, is_right = false, is_up = false, is_down = false;
};

class Enemy
{
public:
    Enemy()
    {
        pos.x = 0;
        pos.y = 0;
        loadimage(&shadow_img, _T("img/shadow_player.png"));
        anim_left = new Animation(_T("img/boar_left"), PLAYER_ANIM_COUNT, 100);
        anim_right = new Animation(_T("img/boar_right"), PLAYER_ANIM_COUNT, 100);

        // enemy spawn edge
        enum class SpawnEdge
        {
            TOP = 0,
            BOTTOM,
            LEFT,
            RIGHT
        };

        SpawnEdge edge = SpawnEdge(rand() % 4);

        switch(edge)
        {
            case SpawnEdge::TOP:
                pos.x = rand() % WINDOW_WIDTH;
                pos.y = -FRAME_HEIGHT;
                break;
            case SpawnEdge::BOTTOM:
                pos.x = rand() % WINDOW_WIDTH;
                pos.y = WINDOW_HEIGHT;
                break;
            case SpawnEdge::LEFT:
                pos.x = -FRAME_WIDTH;
                pos.y = rand() % WINDOW_HEIGHT;
                break;
            case SpawnEdge::RIGHT:
                pos.x = WINDOW_WIDTH;
                pos.y = rand() % WINDOW_HEIGHT;
                break;
            default:
                break;
        }
    }

    bool CheckBulletCollision(const Bullet& bullet)
    {
        return false;
    }

    bool heckPlayerCollision(const Player& player)
    {
        return false;
    }

    void Move(const Player& player)
    {
        const POINT& player_pos = player.GetPos();
        int dir_x = player_pos.x - pos.x;
        int dir_y = player_pos.y - pos.y;

        double len_dir = sqrt(pow(dir_x, 2) + pow(dir_y, 2));

        if(len_dir > 0)
        {
            double normalized_x = dir_x / len_dir;
            double normalized_y = dir_y / len_dir;
            pos.x += int(normalized_x * speed);
            pos.y += int(normalized_y * speed);
        }

        if (pos.x < 0)
            pos.x = 0;
        if (pos.y < 0)
            pos.y = 0;
        if (pos.x > WINDOW_WIDTH - FRAME_WIDTH)
            pos.x = WINDOW_WIDTH - FRAME_WIDTH;
        if (pos.y > WINDOW_HEIGHT - FRAME_HEIGHT)
            pos.y = WINDOW_HEIGHT - FRAME_HEIGHT;
    }

    void Draw(int delta)
    {
        int pos_shadow_x = pos.x + (FRAME_WIDTH - SHADOW_WIDTH) / 2;
        int pos_shadow_y = pos.y + FRAME_HEIGHT - 8;
        putimage_alpha(pos_shadow_x, pos_shadow_y, &shadow_img);

        static bool facing_left = false;

        int dir_x = is_left - is_right;

        if (dir_x > 0)
        {
            facing_left = false;
        }
        else if (dir_x < 0)
        {
            facing_left = true;
        }

        if (facing_left)
            anim_left->play(pos.x, pos.y, delta);
        else
            anim_right->play(pos.x, pos.y, delta);
    }

    const POINT& GetPos() const
    {
        return pos;
    }

    ~Enemy()
    {
        delete anim_left;
        delete anim_right;
    }

private:
    const int speed = 6;
    const int FRAME_WIDTH = 80;
    const int FRAME_HEIGHT = 80;
    const int SHADOW_WIDTH = 48;

private:
    POINT pos;
    IMAGE shadow_img;
    Animation *anim_left;
    Animation *anim_right;
    bool facing_left = false;
    bool is_left = false, is_right = false, is_up = false, is_down = false;
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

void TryGenerateEnemy(std::vector<Enemy*>& enemy_list)
{
    const int INTERVAL = 100;
    static int counter = 0;
    if((++counter) % INTERVAL == 0)
        enemy_list.push_back(new Enemy());
}


int main()
{
    initgraph(WINDOW_WIDTH, WINDOW_HEIGHT);
    bool running = true;
    Player player1;


    BeginBatchDraw();

    load_player_anim();

    ExMessage msg;
    IMAGE background_img;
    std::vector<Enemy*> enemy_list;

    loadimage(&background_img, _T("img/background.png"));

    while (running)
    {

        DWORD start_time = GetTickCount();

        while (peekmessage(&msg, EX_MOUSE | EX_KEY))
        {
            player1.processEvent(msg);
        }

        player1.Move();
        TryGenerateEnemy(enemy_list);
        for(auto enemy : enemy_list)
        {
            enemy->Move(player1);
        }


        cleardevice();
        putimage(0, 0, &background_img);
        player1.Draw(1000 / 144);\
        for(auto enemy : enemy_list)
        {
            enemy->Draw(1000 / 144);
        }

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