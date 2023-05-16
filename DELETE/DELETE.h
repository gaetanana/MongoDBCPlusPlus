//
// Created by g.gonfiantini on 16/05/2023.
//

#ifndef MONGODBCPLUSPLUS_DELETE_H
#define MONGODBCPLUSPLUS_DELETE_H

#endif //MONGODBCPLUSPLUS_DELETE_H
#include <mongocxx/client.hpp>


void deleteAllDocuments(mongocxx::client &client);

void deleteOneDocument(mongocxx::client &client);