//
// Created by g.gonfiantini on 16/05/2023.
//

#include "UPDATE.h"
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
* Cette fonction permet de modifier une valeur d'une clé en lui donnant l'id du document et une nouvelle valeur
*/
void updateOneDocument() {
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

    string documentId;
    cout << "Entrez l'ID du document à mettre à jour : ";
    getline(cin, documentId);

    string fieldName;
    cout << "Entrez le nom du champ à mettre à jour : ";
    getline(cin, fieldName);

    string newValue;
    cout << "Entrez la nouvelle valeur pour le champ : ";
    getline(cin, newValue);

    // Construit un nouveau document avec le champ à mettre à jour et sa nouvelle valeur
    auto updatedDocument = bsoncxx::builder::stream::document{} << "$set" << bsoncxx::builder::stream::open_document << fieldName << newValue << bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize;

    // Mise à jour du document dans la collection
    bsoncxx::stdx::optional<mongocxx::result::update> result = collection.update_one(bsoncxx::builder::stream::document{} << "_id" << bsoncxx::oid{documentId} << bsoncxx::builder::stream::finalize, updatedDocument.view());

    if(result && result->modified_count() > 0) {
        cout << "Le document a été mis à jour avec succès.\n";
    } else {
        cout << "Aucun document n'a été mis à jour. Vérifiez que l'ID du document est correct.\n";
    }
}


/**
* Cette fonction permet de modifier toutes les valeurs qui ont le type "Human" pour le remplacer par une nouvelle valeur
*/
void updateAllHumanDocument(){

}