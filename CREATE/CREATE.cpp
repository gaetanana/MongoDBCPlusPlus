//
// Created by g.gonfiantini on 16/05/2023.
//

#include "CREATE.h"

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include "mongocxx/instance.hpp"
#include "bsoncxx/stdx/optional.hpp"

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;

using namespace std;


/**
* Cette fonction permet de créer une collection dans la base de données MongoDB
 * Elle permet de créer une collection dans la base de données actiaDataBase
*/
void createCollection() {
    mongocxx::instance instance{}; // This should be done only once.
    mongocxx::uri uri("mongodb://root:examplepassword@localhost:27017");
    mongocxx::client client(uri);

    mongocxx::database db = client["actiaDataBase"];

    std::string collectionName;
    std::cout << "Veuillez entrer le nom de la collection : ";
    std::getline(std::cin, collectionName);

    bsoncxx::document::value create_collection_cmd =
            bsoncxx::builder::stream::document{}
                    << "create" << collectionName
                    << bsoncxx::builder::stream::finalize;

    db.run_command(create_collection_cmd.view());
    std::cout << "Collection " << collectionName << " créée avec succès.\n";
}
