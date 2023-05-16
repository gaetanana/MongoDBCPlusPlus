//
// Created by g.gonfiantini on 16/05/2023.
//

#include "database.hpp"
#include <string>
#ifndef MONGODBCPLUSPLUS_CREATE_H
#define MONGODBCPLUSPLUS_CREATE_H

#endif //MONGODBCPLUSPLUS_CREATE_H
using namespace std;
void createCollection(mongocxx::client &client);
void createOneDocumentJSON();
void createManyDocumentsJSON();
bool collectionExist(mongocxx::database database, string collectionName);