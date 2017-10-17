#include <iostream>
#include <SFML/Graphics.hpp>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "Network.hpp"


using namespace std;

#define LD long double

vector< pair<vector<long double>, vector<long double>> > tests ;

void add_test(vector<LD> in  , vector<LD> out)
{
     tests.push_back(pair<vector<long double>, vector<long double>>(in, out));
}

int main()
{
    ios_base::sync_with_stdio(0);
    add_test({0 , 0} , {0});
    for(int i = 1 ; i < 10 ; i ++)
    {
        add_test({i , i} , {0});
        add_test({-i , -i} , {0});
    }
    add_test({100 , -100} , {1});
    add_test({-100 , 100} , {1});
    add_test({200 , 0} , {1});
    add_test({0 , -200} , {1});
    add_test({-200 , 0} , {1});
    add_test({0 , 200} , {1});




    srand(time(0));
    sf::RenderWindow window(sf::VideoMode(400, 400), "SFML works!");
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    network net(2, 4,1);

    sf::Image image;
    image.create(400, 400, sf::Color(128, 128, 128, 128));
    sf::Texture texture;
    sf::Sprite sprite;
    long double lasterror = 1;
    long double waga = 1;
    int testid = 0;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if(event.type == sf::Event::KeyPressed)
            {
                waga *=2;
            }

        }


        for(int i = 0 ; i < 400 ; i ++)
        {
            for(int j = 0 ; j < 400 ; j ++)
            {
                net.set_input(vector<long double> {(i-200), (j-200)});
                net.make();
                image.setPixel(i,j, sf::Color(net.output[0].value*255, net.output[0].value*255,net.output[0].value*255 ));
            }
        }
        //net.debug_write();

        long double act_err = 0;
        for(int i = 0 ; i < 100000 ; i ++)
        {
            act_err = net.learn(tests);
        }

        cout<<act_err<<" "<<"t="<<net.max_test<<endl;
        net.write_weight();
        net.debug_write();

        texture.loadFromImage(image);
        sprite.setTexture(texture);
        window.clear();
        window.draw(sprite);
        window.display();
    }


    return 0;
}



















