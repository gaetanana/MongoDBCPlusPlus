//
// Created by g.gonfiantini on 16/05/2023.
//

#include <string>

#ifndef MONGODBCPLUSPLUS_UPDATE_H
#define MONGODBCPLUSPLUS_UPDATE_H

#endif //MONGODBCPLUSPLUS_UPDATE_H
#include <mongocxx/client.hpp>


void updateOneDocument(mongocxx::client &client);
void updateAllHumanDocument(mongocxx::client &client);
void updateAllKeyTypeContent(mongocxx::client &client);
