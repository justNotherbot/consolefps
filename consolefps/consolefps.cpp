#include "consolegraphics.h"

#define WIDTH 180
#define HEIGHT 80

int main()
{
    double* points;
    wchar_t shade = 0x2588;
    double distancetowall = 0;
    double ceiling = 0.0;
    double lineheight = 0.0;
    double playerx = 2;
    double playery = 2;
    double visibility = 21;
    double deg = 0.0;
    double rayx = 0.0;
    double fov = 90.0;
    short mapwidth = 20;
    short mapheight = 20;
    double* p_move;
    console::Console window(WIDTH, HEIGHT, "Game window");
    std::wstring map;
    map += L"####################";
    map += L"#                  #";
    map += L"#                  #";
    map += L"#                  #";
    map += L"#                  #";
    map += L"#                  #";
    map += L"#     #  ####      #";
    map += L"#     #     #      #";
    map += L"#     #     #      #";
    map += L"#     #######      #";
    map += L"#                  #";
    map += L"#                  #";
    map += L"##########         #";
    map += L"#       ##         #";
    map += L"#       ##         #";
    map += L"#       ##         #";
    map += L"#                  #";
    map += L"#                  #";
    map += L"#                  #";
    map += L"####################";
    if (window.createConsole()) //Only start drawing if console has been created
    {
        while (1) //Main loop
        {
            for (double i = deg - fov / 2; i < deg + fov / 2; i += fov / WIDTH)
            {
                bool onwall = false;
                distancetowall = 0.0;
                while (!onwall) //while a wall has not been hit, increment lookup distance
                {
                    points = window.moveByAngle(playerx, playery, distancetowall, i, fov);
                    if (map[static_cast<int64_t>(points[1]) * mapwidth + static_cast<int64_t>(points[0])] == '#')
                        onwall = true;
                    if (distancetowall >= visibility) //If distance to wall exceeds limit, exit
                    {
                        distancetowall = (double)HEIGHT;
                        break;
                    }
                    distancetowall += 0.1;
                }
                if (int(distancetowall)) //Define shading for a line if distance is nonzero
                {
                    lineheight = HEIGHT / (distancetowall);
                    ceiling = (HEIGHT - lineheight) / 2;
                    rayx = (i - deg + fov / 2) / (fov / WIDTH);
                    if (distancetowall <= 4.0)
                        shade = 0x2588;
                    else if (distancetowall > 4.0 && distancetowall <= 8.0)
                        shade = 0x2593;
                    else
                        shade = 0x2592;
                    for (int j = 0; j < HEIGHT - 1; j++)
                    {
                        if (j < HEIGHT - ceiling && j > ceiling)
                            window.fillCell((short)rayx, j, shade); //draw wall
                        else if (j <= (int)ceiling)
                            window.fillCell((short)rayx, j, ' '); //draw sky
                        else
                            window.fillCell((short)rayx, j, 0x2588, 2); //draw floor 
                    }
                }
                else
                {
                    for (int j = 0; j < HEIGHT - 1; j++)
                        window.fillCell((short)rayx, j, ' ');
                }
            }
            //Reacting to user input
            if (GetAsyncKeyState(VK_LEFT))
                deg += 0.4;
            if (GetAsyncKeyState(VK_RIGHT))
                deg -= 0.4;
            if (GetAsyncKeyState(0x57))
            {
                p_move = window.moveByAngle(playerx, playery, 0.01, deg, fov);
                if (p_move[0] > 0 && p_move[0] < mapwidth - 1 && p_move[1] > 0 && p_move[1] < mapheight - 1)
                {
                    playerx = p_move[0];
                    playery = p_move[1];
                }
            }
            if (GetAsyncKeyState(0x53))
            {
                p_move = window.moveByAngle(playerx, playery, 0.01, deg + 180.0, fov);
                if (p_move[0] > 0 && p_move[0] < mapwidth - 1 && p_move[1] > 0 && p_move[1] < mapheight - 1)
                {
                    playerx = p_move[0];
                    playery = p_move[1];
                }
            }
            if (GetAsyncKeyState(VK_ESCAPE))
                break;
            window.drawLine(0, 0, 10, 10);
            window.updateScreen(false);
        }
    }
    window.releaseMemory();
    return 0;
}
