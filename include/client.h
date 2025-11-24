#ifndef CLIENT_H
#define CLIENT_H

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <map>
#include <cmath>
#include <thread>
#include <iostream>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

#include "reproduction.h"
#include "physics.h"
#include "creature.h"
#include "brain.h"
#include "threadsmg.h"
#include "filemanager.h"

typedef websocketpp::client<websocketpp::config::asio_client> client;


class SimpleClient {
public:
    SimpleClient(const std::string &uri, const std::string path);
    ~SimpleClient();
    void send(Packet pck);
    void run(SimTasker* stk,bool hl);
    bool headless = false;
    int state = 0;
    std::string sbfpath = "";
    std::string srvid = "";
    std::vector<std::string> selectioned;
    std::vector<float> scores;

private:
    client m_client;
    websocketpp::connection_hdl m_hdl;
    std::thread m_thread;
    SimTasker* mstk;
};

class DisplayService {
public:
    DisplayService(bool headless,bool* runningptr, bool* clean_exitptr, int agentPartitionsSize, bool mc);
    void render(std::vector<int>* groups_avail,std::vector<std::vector<Creature*>>*agentPartitions,float fps,int agent_size, int generation,int sous_sim_started,float acu,float simu_time,float evolution, sf::Vector2f start, std::vector<sf::Vector2f> goals);
private:
    bool* runningptr;
    bool* clean_exitptr;
    sf::RenderWindow window;
    int inputdelayBase;
    float inputdelay;
    int selected_agents;
    bool drawall;
    bool mc;
    int agent_partitions_size;
    sf::Font font;
    sf::Texture backgroundTexture;
    sf::Texture wifiTexture;
    sf::Texture startTexture;
    sf::Texture diamondTexture;
    sf::Texture emeraldTexture;
    sf::Texture nether_starTexture;
    sf::Texture netherite_ingotTexture;
    sf::Texture music_disc_othersideTexture;
    sf::Sprite backgroundSprite;
    sf::Sprite wifiSprite;
    sf::Sprite startSprite;
    sf::Sprite diamondSprite;
    sf::Sprite emeraldSprite;
    sf::Sprite nether_starSprite;
    sf::Sprite netherite_ingotSprite;
    sf::Sprite music_disc_othersideSprite;
};

int simulate(SimTasker stk, bool mc = false, bool headless = false, SimpleClient* cl = nullptr);


#endif // CLIENT_H