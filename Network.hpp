#include <vector>
#include <math.h>
#include <iostream>
#include "egde.hpp"
#include "neuron.hpp"

#define LD long double

using namespace std;

long double randd()
{
    return (rand()%10000)/(long double)5000;
}


class activation_functions
{
public:
    static long double sigm(long double x , long double b)
    {
        return 1/(1+exp(-x*b));
    }
};

class neuron;

class edge
{
public:
    neuron *from , *to;
    long double W = 0;
    long double gradient = 0;
    void update_gradient();

    void update_W(long double);

};
int amount = 0;
class neuron
{
public:
    long double (*_f)(long double);
    int id;
    vector<edge*> incoming;
    long double value=1;
    long double back_value;

    neuron(long double (*f)(long double) = [](long double in)->long double {return activation_functions::sigm(in,1);} )
    {
        id = ++amount;
        _f = f;
        value = 1;
    }

    void update_value()
    {
        long long ans = 0;
        for(int i = 0 ; i < incoming.size() ; i ++)
        {
            ans+=incoming[i]->from->value * incoming[i]->W;
        }
        value = _f(ans);
    }

    void update_back_value()
    {
        back_value *= (value)*(1-(value));

        for(int i = 0 ; i < incoming.size() ; i ++)
        {
            incoming[i]->from->back_value+=back_value * incoming[i]->W;
        }
    }
};
void edge::update_gradient()
{
    gradient+=from->value*to->back_value;
}
void edge::update_W(long double n = 0.1)
{
    W-=gradient*n;
}
class network
{
public:
    vector<neuron> input , hidden ,  output;
    vector<edge> E;


    network(int in , int k , int out)
    {

        E.reserve(in*k*2 + k*out*2);
        input.reserve(in+2);
        for(int i = 0 ; i < in + 1; i ++)
        {
            input.push_back( neuron() );
        }

        hidden.reserve(k+2);
        hidden.push_back( neuron() );

        for(int i = 1 ; i < k + 1 ; i ++)
        {
            hidden.push_back( neuron() );
            for(int j = 0 ; j < in + 1 ; j ++)
            {
                edge temp;
                temp.from = &input[j];
                temp.to = &hidden[i];
                temp.W = randd() -1;
                E.push_back(temp);
                hidden[i].incoming.push_back(&E[E.size()-1]);
                cout<<hidden[i].incoming.size()<<endl;
            }
        }
        output.reserve(out+2);
        for(int i = 0 ; i < out ; i ++)
        {
            output.push_back( neuron() );
            for(int j = 0 ; j < k + 1 ; j ++)
            {

                edge temp;
                temp.from = &hidden[j];
                temp.to = &output[i];
                temp.W = randd() -1;
                E.push_back(temp);
                output[i].incoming.push_back(&E[E.size()-1]);

            }
        }

        for(int i = 0 ; i < E.size() ; i++)
        {
            cout<<">"<<E[i].W<<endl;
        }

    }
    void set_input(vector<long double> in)
    {
        for(int i = 1 ; i <= in.size() ; i++)
        {
            input[i].value = in[i-1];
        }
    }
    long double set_back_input(vector<long double> out)
    {
        long double err = 0;
        for(int i = 0 ; i < output.size() ; i++)
        {
            output[i].back_value = output[i].value - out[i];
            err+=(output[i].value - out[i])*(output[i].value - out[i]);
        }
        return err;
    }
    void make()
    {
        for(int i = 1 ; i < hidden.size() ; i ++)
        {
            hidden[i].update_value();
        }
        for(int i = 0 ; i < output.size() ; i++)
        {
            output[i].update_value();
        }
    }

    void propagate()
    {
        for(int i = 0 ; i < output.size() ; i++)
        {
            output[i].update_back_value();
        }
        for(int i = 0 ; i < hidden.size() ; i ++)
        {
            hidden[i].update_back_value();
        }
        for(int i = 0 ; i < input.size() ; i++)
        {
            input[i].update_back_value();
        }

    }

    void grad()
    {
        for(int i = 0 ; i < E.size() ; i++)
        {
            E[i].update_gradient();
        }
    }

    void update_weight(long double e)
    {
        for(int i = 0 ; i < E.size() ; i++)
        {
            E[i].update_W(e);
        }
    }

    int max_test = 100;
    long double learn(vector<pair<vector<LD> , vector<LD>> > &tests)
    {
        long double err= 0;
        clear();
        for(int i = 0 ; i < tests.size() && i < max_test; i++)
        {
            for(int i = 0 ; i < output.size() ; i++)
            {
                output[i].back_value = 0;
            }
            for(int i = 0 ; i < hidden.size() ; i++)
            {
                hidden[i].back_value = 0;
            }
            for(int i = 0 ; i < input.size() ;  i++)
            {
                input[i].back_value = 0;
            }

            set_input(tests[i].first);
            make();
            err += set_back_input(tests[i].second);
            propagate();
            grad();
        }
        update_weight(1);
        if(err < 0.01)max_test++;
        return err;

    }
    void clear()
    {
        for(int i = 0 ; i < E.size() ; i++)
        {
            E[i].gradient = 0;
        }

    }
    void write_weight()
    {
        for(int i = 0 ; i < E.size() ; i++)
        {
            cout<<E[i].W<<" ";
            if(i%4 == 3)cout<<endl;
        }
        cout<<endl;
        /*cout<<"kutas"<<endl;
        int i = 0;
        cout<<E[i++].W<<" "<<hidden[1].incoming[0]->W<<endl;
        cout<<E[i++].W<<" "<<hidden[1].incoming[1]->W<<endl;
        cout<<E[i++].W<<" "<<hidden[1].incoming[2]->W<<endl;
        cout<<E[i++].W<<" "<<hidden[2].incoming[0]->W<<endl;
        cout<<E[i++].W<<" "<<hidden[2].incoming[1]->W<<endl;
        cout<<E[i++].W<<" "<<hidden[2].incoming[2]->W<<endl;
        cout<<E[i++].W<<" "<<hidden[3].incoming[0]->W<<endl;
        cout<<E[i++].W<<" "<<hidden[3].incoming[1]->W<<endl;
        cout<<E[i++].W<<" "<<hidden[3].incoming[2]->W<<endl;
        cout<<E[i++].W<<" "<<output[0].incoming[0]->W<<endl;

        cout<<"koniec kutas"<<endl;*/

    }

    void debug_write()
    {
        for(int i = 0 ; i < output.size() ; i++)
        {
            cout<<output[i].id<<endl;
            for(int j = 0 ; j < output[i].incoming.size() ; j++)
            {
                cout<<"   "<<output[i].incoming[j]->from->id<<"  "<<output[i].incoming[j]->from->value<<"\t W="<<output[i].incoming[j]->W<<endl;
            }
        }
        for(int i = 0 ; i < hidden.size() ; i++)
        {
            cout<<hidden[i].id<<endl;
            for(int j = 0 ; j < hidden[i].incoming.size() ; j++)
            {
                cout<<"   "<<hidden[i].incoming[j]->from->id<<"  "<<hidden[i].incoming[j]->from->value<<"\t W="<<hidden[i].incoming[j]->W<<endl;
            }
        }


    }
    //void set_back_input;



};
