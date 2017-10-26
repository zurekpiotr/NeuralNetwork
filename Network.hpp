#include <vector>
#include <math.h>
#include <iostream>

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
    long double last_grad= 0;
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
    long double u = 0;
    long double value=1;
    long double back_value;

    neuron(long double (*f)(long double) = [](long double in)->long double {return activation_functions::sigm(in,1);} )
    {
        id = amount++;
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
        u = ans;
        value = _f(ans);
    }

    void update_back_value()
    {
        back_value *= (value)*(1-(value));

        for(int i = 0 ; i < incoming.size() ; i ++)
        {
            incoming[i]->from->back_value += back_value * incoming[i]->W;
        }
    }
};
void edge::update_gradient()
{
    //cout<<"  "<<from->id<<" "<<from->value*to->back_value<<" "<<to->id<<endl;
    gradient+=from->value*to->back_value;
}
void edge::update_W(long double n = 0.1)
{
    W-=gradient*n;
}


class network
{


    vector<vector<neuron>> layers;

    vector<edge> E;

public:
    network(vector<int> neurons)
    {
        int i = 0;
        for(; i < neurons.size()-1 ; i++)
        {
            layers.push_back(vector<neuron>());
            for(int j = 0 ; j < neurons[i]+1 ; j++)
            {
                layers[i].push_back(neuron());
            }
        }

        layers.push_back(vector<neuron>());
        for(int j = 0 ; j < neurons[i] ; j++)
        {
            layers[i].push_back(neuron());
        }

        int edges = 0;
        for(i = 1 ; i < neurons.size() ; i++)
        {
            edges+=layers[i-1].size() * layers[i].size();
        }

        E.reserve(edges+4);

        for(i = 1 ; i < neurons.size()-1 ; i++)
        {
            for(int j = 1 ; j < layers[i].size() ; j++)
            {
                for(int k = 0 ; k < layers[i-1].size() ; k++)
                {
                    edge temp;
                    temp.from = &layers[i-1][k];
                    temp.to   = &layers[i  ][j];
                    temp.W = randd();
                    E.push_back(temp);
                    layers[i][j].incoming.push_back(&E[E.size() - 1]);
                }
            }
        }
        for(int j = 0 ; j < layers[i].size() ; j++)
        {
            for(int k = 0 ; k < layers[i-1].size() ; k++)
            {
                edge temp;
                temp.from = &layers[i-1][k];
                temp.to   = &layers[i  ][j];
                temp.W = randd();
                E.push_back(temp);
                layers[i][j].incoming.push_back(&E[E.size() - 1]);
            }
        }



    }
    ~network()
    {

    }

    long double last_error = 100000000;

    long double learn(vector<pair<vector<LD> , vector<LD>> > &tests , long double coefficient)
    {
        long double err= 0;
        clear();


        for(int i = 0 ; i < tests.size() ; i++)
        {
            forall_neurons( [](neuron & y)-> void {y.back_value = 0;});

            set_input(tests[i].first);
            make();
            err += set_back_input(tests[i].second);
            propagate();
            grad();
        }
        last_error = err;

        for(int i = 0 ; i < E.size() ; i++)
        {
            E[i].gradient += 0.95*E[i].last_grad;
        }

        update_weight(coefficient);

        err = 0;
        for(int i = 0 ; i < tests.size() ; i++)
        {
            set_input(tests[i].first);
            make();
            err += set_back_input(tests[i].second);
        }

        if(err > last_error* 1.1)
        {
            update_weight(-1.0*coefficient);
            for(int i = 0 ; i < E.size() ; i++)
            {
                E[i].gradient -= 0.95*E[i].last_grad;
            }
            update_weight(coefficient);
        }


        for(int i = 0 ; i < E.size() ; i++)
        {
            E[i].last_grad = E[i].gradient;
        }


        err = 0;
        for(int i = 0 ; i < tests.size() ; i++)
        {
            set_input(tests[i].first);
            make();
            err += set_back_input(tests[i].second);
        }

        return err;



    }

    vector<long double> calculate_for_input(vector<long double> &in)
    {
        vector<long double> out;
        set_input(in);
        make();
        for(int i = 0 ; i < layers.back().size() ; i++)
        {
            out.push_back(layers.back()[i].value);
        }
        return out;
    }
    vector<long double> calculate_for_input(vector<long double> in)
    {
        vector<long double> out;
        set_input(in);
        make();
        for(int i = 0 ; i < layers.back().size() ; i++)
        {
            out.push_back(layers.back()[i].value);
        }
        return out;
    }
    void write_weight()const
    {
        for(int i = 0 ; i < E.size() ; i++)
        {
            cout<<E[i].W<<" ";
            if(i%6 == 5)cout<<endl;
            //cout<<E[i].from->id<<" ("<<E[i].W<<" "<<E[i].gradient<<") "<<E[i].to->id<<endl;
        }
        cout<<endl;
    }

//private :

    void set_input(vector<long double> &in)
    {
        for(int i = 1 ; i <= in.size() ; i++)
        {
            layers[0][i].value = in[i-1];
        }
    }

    long double set_back_input(vector<long double>& out)
    {
        long double err = 0;
        for(int i = 0 ; i < layers.back().size() ; i++)
        {
            layers.back()[i].back_value = layers.back()[i].value - out[i];
            err+=(layers.back()[i].value - out[i])*(layers.back()[i].value - out[i]);
        }
        return err;
    }


    void make()
    {
        for(int i = 1 ; i < layers.size()-1 ; i++)
        {
            for(int j = 0 ; j < layers[i].size() ; j++)
            {
                layers[i][j].update_value();
            }
        }


        for(int i = 0 ; i < layers.back().size() ; i++)
        {
            layers.back()[i].update_value();
        }
    }

    void propagate()
    {
        for(int j = 0 ; j < layers.back().size() ; j++)
        {
            layers.back()[j].update_back_value();
        }

        for(int i = layers.size()-2 ; i >= 0 ; i--)
        {
            for(int j = 1 ; j < layers[i].size() ; j++)
            {
                layers[i][j].update_back_value();
            }

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

    void forall_neurons( void t(neuron &) )
    {
        for(int i = 0 ; i < layers.size() ; i++)
        {
            //cout<<"    >|"<<layers.size()<<endl;
            for(int j = 0 ; j < layers[i].size() ; j++)
            {
                t ( layers[i][j] );
            }

        }
    }



    void clear()
    {
        for(int i = 0 ; i < E.size() ; i++)
        {
            E[i].gradient = 0;
        }

    }
};
