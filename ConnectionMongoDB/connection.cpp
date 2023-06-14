//
// Created by g.gonfiantini on 16/05/2023.
//

#include "connection.h"
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


/**
 * Cette fonction permet de se connecter à la base de données MongoDB
 * @return
 */

mongocxx::collection connectionMongoDB(){
    mongocxx::instance inst{};
    mongocxx::client conn{mongocxx::uri{"mongodb://root:examplepassword@localhost:27017"}};
    auto collection = conn["actiaDataBase"]["testCollection"];
    return collection;
}
