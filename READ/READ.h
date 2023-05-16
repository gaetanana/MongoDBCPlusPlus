//
// Created by g.gonfiantini on 16/05/2023.
//

#ifndef MONGODBCPLUSPLUS_READ_H
#define MONGODBCPLUSPLUS_READ_H

#endif //MONGODBCPLUSPLUS_READ_H

#include <mongocxx/client.hpp>
void readOneDocument(mongocxx::client &client);
void readAllDocument(mongocxx::client &client);
void readAllDocumentWithHuman(mongocxx::client &client);
void readAllDocumentWithHumanProbability(mongocxx::client &client);
void readAllDocumentWithHumanProbabilityAndDate(mongocxx::client &client);
void readAllDocumentWithHumanProbabilityAndDateGender(mongocxx::client &client);