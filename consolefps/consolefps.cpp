#include "consolegraphics.h"
#include <fstream>
#include <string>

#define WIDTH 360
#define HEIGHT 180

double testLine(double x, double y, double x1, double y1, double xtest)
{
    double a, testy;
    if (x != x1)
    {
        double b;
        a = (y1 - y) / (x1 - x);
        b = y - a * x;
        testy = a * xtest + b;
        if (testy > max(y, y1) || testy < min(y, y1))
            return -1;
        return testy;
    }
    else
    {
        if (x != xtest)
            return -1;
        else
            return y;
    }
}

int main()
{
    double testy = 0;
    double* points;
    wchar_t shade = 0x2588;
    short pcolor = 0x000F;
    double texx = 0.0;
    double texy = 0.0;
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
    short texturewidth = 20;
    double* p_move;
    std::string texture;
    texture += "bbbabbbabbbabbbabbba";
    texture += "bbbabbbabbbabbbabbba";
    texture += "aaaaaaaaaaaaaaaaaaaa";
    texture += "bbbabbbabbbabbbabbba";
    texture += "bbbabbbabbbabbbabbba";
    texture += "aaaaaaaaaaaaaaaaaaaa";
    texture += "bbbabbbabbbabbbabbba";
    texture += "bbbabbbabbbabbbabbba";
    texture += "aaaaaaaaaaaaaaaaaaaa";
    texture += "bbbabbbabbbabbbabbba";
    texture += "bbbabbbabbbabbbabbba";
    texture += "aaaaaaaaaaaaaaaaaaaa";
    texture += "bbbabbbabbbabbbabbba";
    texture += "bbbabbbabbbabbbabbba";
    texture += "aaaaaaaaaaaaaaaaaaaa";
    texture += "bbbabbbabbbabbbabbba";
    texture += "bbbabbbabbbabbbabbba";
    texture += "aaaaaaaaaaaaaaaaaaaa";
    texture += "bbbabbbabbbabbbabbba";
    texture += "bbbabbbabbbabbbabbba";
    texture += "aaaaaaaaaaaaaaaaaaaa";
    console::Console window(WIDTH, HEIGHT, "Game window");
    std::string tmp;
    std::string map;
    std::ifstream file;
    file.open("map.txt");
    //fetching map from file
    while (getline(file, tmp))
    {
        map += tmp;
    }
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
                    {
                        if((int)points[0] - playerx > 0) //If we've hit wall right of where the player is, consider x of the ray without decimal 
                            testy = testLine(playerx, playery, points[0], points[1], int(points[0]));
                        else //Otherwise, consider rounded x of the ray because we always want to stay within the bounds of the line player position - ray position
                            testy = testLine(playerx, playery, points[0], points[1], std::round(points[0]));
                        if ((int)testy == (int)points[1])
                        {
                            texx = (points[1] - (int)points[1]) * texturewidth; // If we've hit left or right of the cell, use y for sampling
                        }
                        else
                        {
                            texx = (points[0] - (int)points[0]) * texturewidth; // If we've hit top or bottom of the cell, use x for sampling
                        }
                        onwall = true;
                    } 
                    if (distancetowall >= visibility) //If distance to wall exceeds limit, exit
                    {
                        distancetowall = (double)HEIGHT;
                        break;
                    }
                    distancetowall += 0.01;
                }
                if (int(distancetowall)) //Define shading and color for a line if distance is nonzero
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
                        if (j < HEIGHT - ceiling && j >= ceiling)
                        {
                            //Do sampling for every pixel
                            texy = (j - ceiling) * (texturewidth/lineheight);
                            if (texture[static_cast<int64_t>(std::round(texy)) * texturewidth + static_cast<int64_t>(texx)] == 'b')
                                pcolor = 12;
                            else
                                pcolor = 14;
                            window.fillCell((short)rayx, j, shade, pcolor); //draw wall
                        } 
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
                deg += 0.9;
            if (GetAsyncKeyState(VK_RIGHT))
                deg -= 0.9;
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
    file.close();
    return 0;
}
