#include "consolegraphics.h"

#define WIDTH 360
#define HEIGHT 180

double testLine(double x, double y, double x1, double y1, double xtest, bool bounds = true)
{
    double a, testy;
    if (x != x1)
    {
        double b;
        a = (y1 - y) / (x1 - x);
        b = y - a * x;
        testy = a * xtest + b;
        if (testy > max(y, y1) && bounds || testy < min(y, y1) && bounds)
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

struct Billboard {
    short x;
    short y;
    std::string* texture;
    short texture_width;
    short texture_height;
};

int main()
{
    bool tfetch = false;
    bool mfetch = false;
    double testy = 0;
    double* points;
    wchar_t shade = 0x2588;
    short pcolor = 0x000F;
    double texx = 0.0;
    double texy = 0.0;
    double distancetowall = 0;
    double ceiling = 0.0;
    double floor = 0.0;
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
    double depth_buffer[WIDTH] = { 0 };
    std::string texture;
    std::string s_texture;
    Billboard billboard_t;
    Billboard billboard_t1;
    billboard_t.x = 5;
    billboard_t.y = 8;
    billboard_t.texture = &s_texture;
    billboard_t.texture_width = 0;
    billboard_t.texture_height = 0;
    billboard_t1.x = 3;
    billboard_t1.y = 8;
    billboard_t1.texture = &s_texture;
    billboard_t1.texture_width = 0;
    billboard_t1.texture_height = 0;
    console::Console window(WIDTH, HEIGHT, "Game window");
    std::string tmp;
    std::string map;
    std::ifstream file;
    std::ifstream s_file;
    std::ofstream dump;   //enable dump file for debugging
    dump.open("dump.txt");
    file.open("data.txt");
    //fetching data from file
    while (getline(file, tmp))
    {
        if (mfetch)
            map += tmp;
        else if (tfetch)
            texture += tmp;
        if (tmp == "m")
        {
            mfetch = true;
            tfetch = false;
        }           
        else if (tmp == "t")
        {
            mfetch = false;
            tfetch = true;
        } 
    }
    file.close();
    s_file.open("sprite.txt");
    while (getline(s_file, tmp))
    {
        if (!billboard_t.texture_width)
        {
            while (tmp[billboard_t.texture_width])
            {
                billboard_t.texture_width++;
                billboard_t1.texture_width++;
            }
        }
        s_texture += tmp;
        billboard_t.texture_height++;
        billboard_t1.texture_height++;
    }
    Billboard objects[2] = { billboard_t, billboard_t1 };
    s_file.close();
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
                    points = window.moveByAngle(playerx, playery, distancetowall, i);
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
                        depth_buffer[int((i - deg + fov / 2) / (fov / WIDTH))] = distancetowall;
                    }
                    if (distancetowall >= visibility) //If distance to wall exceeds limit, exit
                    {
                        distancetowall = (double)HEIGHT;
                        depth_buffer[int((i - deg + fov / 2) / (fov / WIDTH))] = distancetowall;
                        onwall = true;
                    }
                    distancetowall += 0.01;
                }
                if (onwall)
                {
                    if (int(distancetowall)) //Define shading and color for a line if distance is nonzero
                    {
                        lineheight = HEIGHT / distancetowall;
                        rayx = (i - deg + fov / 2) / (fov / WIDTH);
                        ceiling = (HEIGHT - lineheight) / 2;
                        floor = HEIGHT - ceiling - lineheight;
                        if (distancetowall <= 4.0)
                            shade = 0x2588;
                        else if (distancetowall > 4.0 && distancetowall <= 8.0) 
                            shade = 0x2593;
                        else
                            shade = 0x2592;
                        for (int j = 0; j < HEIGHT - 1; j++)
                        {
                            if (j < HEIGHT - floor && j >= ceiling)
                            {
                                //Do sampling for every pixel
                                texy = (j - ceiling) * (texturewidth / lineheight);
                                if (texture[static_cast<int64_t>(std::round(texy)) * texturewidth + static_cast<int64_t>(texx)] == 'b')
                                    pcolor = 12;
                                else if (texture[static_cast<int64_t>(std::round(texy)) * texturewidth + static_cast<int64_t>(texx)] == 'a')
                                    pcolor = 14;
                                else if (texture[static_cast<int64_t>(std::round(texy)) * texturewidth + static_cast<int64_t>(texx)] == 'r')
                                    pcolor = 4;
                                else if (texture[static_cast<int64_t>(std::round(texy)) * texturewidth + static_cast<int64_t>(texx)] == 'g')
                                    pcolor = 2;
                                else if (texture[static_cast<int64_t>(std::round(texy)) * texturewidth + static_cast<int64_t>(texx)] == 'c')
                                    pcolor = 128;
                                else
                                    pcolor = 7;
                                window.fillCell((short)rayx, j, shade, pcolor); //draw wall
                            }
                            else if (j <= (int)ceiling)
                                window.fillCell((short)rayx, j, 0x2588, 9); //draw sky
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
            }
            for (int t = 0; t < int(sizeof(objects) / sizeof(objects[0])); t++) //draw billboards
            {
                double t_ang = atan2(objects[t].x - playerx, playery - objects[t].y) * 180 / 3.14159265 - deg + fov / 2 + 90;
                if (t_ang < fov && t_ang >= 0)
                {
                    double b_dist = sqrt(std::pow(playerx - objects[t].x, 2) + std::pow(playery - objects[t].y, 2));
                    if (b_dist <= visibility && b_dist > 1)
                    {
                        short bx = short(WIDTH * (t_ang) / fov);
                        double scale = HEIGHT / (b_dist * objects[t].texture_height);
                        window.drawSprite(bx, HEIGHT / 2, scale, *objects[t].texture, objects[t].texture_width, objects[t].texture_height, depth_buffer, b_dist);
                    }
                }
            }
            //Reacting to user input
            if (GetAsyncKeyState(VK_LEFT))
                deg += 0.9;
            if (GetAsyncKeyState(VK_RIGHT))
            {
                deg -= 0.9;
            }
            if (deg >= 360) deg = 0;
            else if (deg < 0) deg = 360;
            if (GetAsyncKeyState(0x57))
            {
                p_move = window.moveByAngle(playerx, playery, 1, deg);
                if (map[static_cast<int64_t>(p_move[1]) * mapwidth + static_cast<int64_t>(p_move[0])] != '#')
                {
                    p_move = window.moveByAngle(playerx, playery, 0.01, deg);
                    playerx = p_move[0];
                    playery = p_move[1];
                }
            }
            if (GetAsyncKeyState(0x53))
            {
                p_move = window.moveByAngle(playerx, playery, 1, deg + 180.0);
                if (map[static_cast<int64_t>(p_move[1]) * mapwidth + static_cast<int64_t>(p_move[0])] != '#')
                {
                    p_move = window.moveByAngle(playerx, playery, 0.01, deg + 180.0);
                    playerx = p_move[0];
                    playery = p_move[1];
                }
            }
            if (GetAsyncKeyState(VK_ESCAPE))
                break;
            window.updateScreen(false);
        }
    }
    window.releaseMemory();
    dump.close();
    return 0;
}
