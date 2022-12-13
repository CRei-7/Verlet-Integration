#include <SFML/Graphics.hpp>
#include<cmath>

int main()
{
    float height = 720, width = 1080;
    sf::RenderWindow window(sf::VideoMode(width, height), "Verlet Integration");
    sf::CircleShape shape[7];
    for (int i = 0; i < 6; i++) {
        shape[i].setRadius(5.f);
        shape[i].setFillColor(sf::Color::Green);
    }
    
    sf::CircleShape circle(5.f);

    sf::Texture texture;
    texture.loadFromFile("minions.png");
    
    sf::ConvexShape polygon;
    polygon.setTexture(&texture);

    struct points {
        float x, y, oldx, oldy,angle=0,speed=0.1;
        bool pinned = false;
    };
    /*struct stick {
        points p1, p2;
        float length;
    };*/

    bool toggle = false;
    bool toggle_points = false;

    sf::VertexArray vertices;
    vertices.setPrimitiveType(sf::LineStrip);
    vertices.resize(61);
    float angle = 2 * 3.14 / 60;
    float iniPointY = 100;
    float iniPointX = 300;
    float rX = 50;
    float rY = 50;
    float num = 60;
    float theta = 0;
    for (int i = 0; i <= num; i++)
    {
        vertices[i].position = sf::Vector2f((iniPointX + cos(theta) * rX+5), (iniPointY + sin(theta) * rY+5));
        theta += angle;
    }

    //defining points
    points p[7];
    p[0].x = 100, p[0].y = 100, p[0].oldx = 85, p[0].oldy = 95;
    p[1].x = 200, p[1].y = 100, p[1].oldx = 200, p[1].oldy = 100;
    p[2].x = 200, p[2].y = 200, p[2].oldx = 200, p[2].oldy = 200;
    p[3].x = 100, p[3].y = 200, p[3].oldx = 100, p[3].oldy = 200;
    p[4].x = 250, p[4].y = 100, p[4].oldx = 250, p[4].oldy = 100;
    p[5].x = 300, p[5].y = 100, p[5].oldx = 300, p[5].oldy = 100;
    p[6].x = 450, p[6].y = 100, p[6].oldx = 450, p[6].oldy = 100, p[6].pinned=true;

    //length between points
    float length[8];
    for (int i = 0; i < 6; i++)
    {
        if (i < 6) {
            length[i] = sqrt(pow(p[i].x - p[i + 1].x, 2) + pow(p[i].y - p[i + 1].y, 2));
        }
    }
    length[6] = sqrt(pow(p[0].x - p[2].x, 2) + pow(p[0].y - p[2].y, 2));
    length[7] = sqrt(pow(p[1].x - p[3].x, 2) + pow(p[1].y - p[3].y, 2));
    

    float vx, vy, bounce = 0.9, gravity = 0.4, friction = 0.999;
    /*struct engine {
        float baseX = 450,
            baseY = 100,
            range = 100,
            angle = 0,
            speed = 0.05,
            x = 550,
            y = 100;
        bool pinned = true;
    };*/

    sf::Clock clock, clock1;

    //actual physics
    auto updatePoints = [&](points &p) {
        if (!p.pinned) {
            vx = (p.x - p.oldx) * friction;
            vy = (p.y - p.oldy) * friction;

            p.oldx = p.x;
            p.oldy = p.y;
            p.x += vx;
            p.y += vy;
        }
    };

    //so that points don't go outside the boundary
    auto constrainPoints = [&](points& p) {
        if (!p.pinned) {
            vx = (p.x - p.oldx) * friction;
            vy = (p.y - p.oldy) * friction;

            p.y += gravity;
            if (p.x >= width - 10) {
                p.x = width - 10;
                p.oldx = p.x + vx * bounce;
            }
            else if (p.x <= 0) {
                p.x = 0;
                p.oldx = p.x + vx * bounce;
            }
            if (p.y >= height - 10) {
                p.y = height - 10;
                p.oldy = p.y + vy * bounce;
            }
            else if (p.y <= 0) {
                p.y = 0;
                p.oldy = p.y + vy * bounce;
            }
        }
    };

    //this function is for the stick between two points
    auto updateSticks = [&](points& p, points& p1, float length) {
        //s.p1 = p; s.p2 = p1;
        

        if (!p.pinned || !p1.pinned) {
            float dx, dy, distance, diff, percent, offsetX, offsetY;
            dx = p1.x - p.x;
            dy = p1.y - p.y;
            distance = sqrt(dx * dx + dy * dy);
            diff = length - distance;
            percent = diff / distance / 2;
            offsetX = dx * percent;
            offsetY = dy * percent;

            if (!p.pinned) {
                p.x -= offsetX;
                p.y -= offsetY;
            }
            if (!p1.pinned) {
                p1.x += offsetX;
                p1.y += offsetY;
            }
        }
    };
    theta = 0;
    
    while (window.isOpen())
    {
        window.setFramerateLimit(60);
        
        if (clock.getElapsedTime().asSeconds())//this ensures, in each frame the points are updated
        {
            if (!toggle) {
                p[6].x = 500 + cos(p[6].angle) * 50;//for x movement within rectangle
                p[6].angle += p[6].speed;
                p[6].y = 100;
            }
            else {
                p[6].x = iniPointX + cos(theta) * rX;//for circular movement
                p[6].y = iniPointY + sin(theta) * rY;
                theta += angle;
            }
            for (int i = 0; i < 6; i++) {
                
                constrainPoints(p[i]);
                updatePoints(p[i]);
                if (i < 6) {
                    updateSticks(p[i], p[i + 1], length[i]);
                    updateSticks(p[2], p[0],length[6]);
                    updateSticks(p[1], p[3], length[7]);
                }
                else {
                    updateSticks(p[i], p[0],length[i]);
                    updateSticks(p[2], p[0],length[6]);
                    updateSticks(p[1], p[3], length[7]);
                }
            }

        }

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
            {
                toggle = !toggle;
                //p[6].pinned = !p[6].pinned;
            }
            if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
            {
                toggle_points = !toggle_points;
            }
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            
        }

        //sf::VertexArray line(sf::LineStrip, 2);
        //line[0].position = sf::Vector2f(s.p1.x, s.p1.y);
        //line[1].position = sf::Vector2f(s.p2.x, s.p2.y);

        sf::Vertex line[] = {
            sf::Vertex(sf::Vector2f(p[0].x+5, p[0].y+5),sf::Color::Red),
            sf::Vertex(sf::Vector2f(p[1].x+5, p[1].y+5),sf::Color::Red),
            sf::Vertex(sf::Vector2f(p[1].x+5, p[1].y+5)),
            sf::Vertex(sf::Vector2f(p[2].x+5, p[2].y+5)),
            sf::Vertex(sf::Vector2f(p[2].x+5, p[2].y+5)),
            sf::Vertex(sf::Vector2f(p[3].x+5, p[3].y+5)),
            sf::Vertex(sf::Vector2f(p[3].x+5, p[3].y+5)),
            sf::Vertex(sf::Vector2f(p[0].x+5, p[0].y+5)),
            sf::Vertex(sf::Vector2f(p[3].x + 5, p[3].y + 5)),
            sf::Vertex(sf::Vector2f(p[4].x + 5, p[4].y + 5)),
            sf::Vertex(sf::Vector2f(p[4].x + 5, p[4].y + 5)),
            sf::Vertex(sf::Vector2f(p[5].x + 5, p[5].y + 5)),
            sf::Vertex(sf::Vector2f(p[5].x + 5, p[5].y + 5)),
            sf::Vertex(sf::Vector2f(p[6].x + 5, p[6].y + 5)),
            sf::Vertex(sf::Vector2f(p[2].x+5, p[2].y+5)),//these two lines show the diagonal stick, hide it to form a square with no diagonal stick
            sf::Vertex(sf::Vector2f(p[0].x+5, p[0].y+5)),//better way would be to use a bool data type to say something is hidden or not
            sf::Vertex(sf::Vector2f(p[1].x + 5, p[1].y + 5)),//these two lines show the diagonal stick, hide it to form a square with no diagonal stick
            sf::Vertex(sf::Vector2f(p[3].x + 5, p[3].y + 5))
        };

        sf::Vertex Rect[] = {
            sf::Vertex(sf::Vector2f(450,100+10)),
            sf::Vertex(sf::Vector2f(450,100 )),
            sf::Vertex(sf::Vector2f(450,100 )),
            sf::Vertex(sf::Vector2f(560,100 )),
            sf::Vertex(sf::Vector2f(560,100 )),
            sf::Vertex(sf::Vector2f(560,100 + 10)),
            sf::Vertex(sf::Vector2f(560,100 + 10)),
            sf::Vertex(sf::Vector2f(450,100 + 10))
        };
        //line->color = sf::Color::Red;
        


        polygon.setPointCount(4);
        polygon.setPoint(0, sf::Vector2f(p[0].x + 5, p[0].y + 5));
        polygon.setPoint(1, sf::Vector2f(p[1].x + 5, p[1].y + 5));
        polygon.setPoint(2, sf::Vector2f(p[2].x + 5, p[2].y + 5));
        polygon.setPoint(3, sf::Vector2f(p[3].x + 5, p[3].y + 5));
        //polygon.setFillColor(sf::Color::Green);
        float dx = p[1].x - p[0].x;
        float dy = p[2].y - p[1].y;
        //polygon.setRotation(atan(dy / dx));//find solution for rotation, this doesn't work

        window.clear();

        circle.setFillColor(sf::Color::Green);

        circle.setPosition(p[6].x, p[6].y);

        window.draw(circle);

        window.draw(vertices);

        window.draw(Rect, 8,sf::Lines);

        if (toggle_points) {
            for (int i = 0; i < 6; i++) { //this block renders points!!
                shape[i].setPosition(p[i].x, p[i].y);
                window.draw(shape[i]);
            }
        }
        window.draw(polygon);
        window.draw(line, 18, sf::Lines);//name, vertex count, type
        window.display();
    }
    return 0;
}