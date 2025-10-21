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

int simulate(SimTasker stk);
void logm(const std::string& message, const std::string& level = "INFO");

#endif // CLIENT_H