#include <iostream>
#include <SFML/Graphics.hpp>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "Network.hpp"
#include <cstdlib>

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
    srand(time(0));
   /* for(int i = 0 ; i < 40000 ; i ++)
    {
        int x = (rand()%400)-200;
        int y = (rand()%400)-200;
        cout<<x<<" "<<y<<endl;
        if( y > 100 )
        {
            add_test({x,y} , {1});
        }
        else
        {
            add_test({x,y} , {0});
        }
    }
    */
    for(int x = -200 ; x < 201 ; x++)
    {
        for(int y = -200 ; y < 201 ; y++)
        {
            if( y > 100 )
            {
                add_test({x,y} , {1});
            }
            else
            {
                add_test({x,y} , {0});
            }
        }
    }

    network net({2,1});




    sf::RenderWindow window(sf::VideoMode(400, 400), "SFML works!");




    sf::Image image;
    image.create(400, 400, sf::Color(128, 128, 128, 128));
    sf::Texture texture;
    sf::Sprite sprite;


    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }


        for(int i = 0 ; i < 400 ; i ++)
        {
            for(int j = 0 ; j < 400 ; j ++)
            {
                auto temp = net.calculate_for_input({i-200 , j-200});
                image.setPixel(i,j, sf::Color(temp[0]*255, temp[0]*255,temp[0]*255 ));
            }
        }


        long double act_err = 0;
        for(int i = 0 ; i < 1 ; i ++)
        {
            act_err = net.learn(tests , 0.5);
        }
        net.write_weight();
        cout<<"E = "<<act_err<<endl;
        /*cout<<"<----->"<<endl;
        net.forall_neurons([](neuron & x)->void { cout<<x.value<<endl; });
        cout<<"<-end->"<<endl;*/

        texture.loadFromImage(image);
        sprite.setTexture(texture);
        window.clear();
        window.draw(sprite);
        window.display();
    }


    return 0;
}



















