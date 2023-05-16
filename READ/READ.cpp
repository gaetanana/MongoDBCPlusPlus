//
// Created by g.gonfiantini on 16/05/2023.
//

#include "READ.h"
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <fstream>
#include <filesystem>
#include "mongocxx/instance.hpp"
#include "bsoncxx/stdx/optional.hpp"
#include "json/value.h"
#include "json/writer.h"
#include "pugixml.hpp"
#include "../CREATE/CREATE.h"

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;
using namespace std;
/**
 * Cette fonction permet de lire un document grâce à son ID
 */
void readOneDocument(){
    mongocxx::instance instance{};
    mongocxx::uri uri("mongodb://root:examplepassword@localhost:27017");
    mongocxx::client client(uri);
    mongocxx::database db = client["actiaDataBase"];
    cout << "Entrer le nom de la collection : ";
    string collectionName;

    getline(cin, collectionName);
    if(!collectionExist(db, collectionName)){
        cout << "La collection n'existe pas.\n";
        return;
    }
    mongocxx::collection collection = db[collectionName];
    cout << "Entrer l'ID du document a lire : ";
    string id;
    getline(cin, id);
    bsoncxx::stdx::optional<bsoncxx::document::value> result = collection.find_one(document{} << "_id" << id << finalize);
    if (result) {
        cout << bsoncxx::to_json(*result) << "\n";
    } else {
        cout << "Erreur lors de la lecture du document.\n";
    }
}

/**
 * Cette fonction permet de lire tous les documents d'une collection
 */
void readAllDocument(){
    mongocxx::instance instance{};
    mongocxx::uri uri("mongodb://root:examplepassword@localhost:27017");
    mongocxx::client client(uri);
    mongocxx::database db = client["actiaDataBase"];
    cout << "Entrer le nom de la collection : ";
    string collectionName;

    getline(cin, collectionName);
    if(!collectionExist(db, collectionName)){
        cout << "La collection n'existe pas.\n";
        return;
    }
    mongocxx::collection collection = db[collectionName];
    mongocxx::cursor cursor = collection.find({});
    for (auto doc : cursor) {
        cout << bsoncxx::to_json(doc) << "\n";
    }

}

/**
 * Cette fonction permet de lire tous les documents d'une collection avec un type Human
 */

void readAllDocumentWithHuman(){

}

/**
 * Cette fonction permet de lire tous les documents d'une collection avec un type Human et une probabilité
 * supérieure à 0.5
 */
void readAllDocumentWithHumanProbability(){

}


void readAllDocumentWithHumanProbabilityAndDate(){

}
void readAllDocumentWithHumanProbabilityAndDateGender(){

}