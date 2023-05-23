//
// Created by g.gonfiantini on 16/05/2023.
//

#include "DELETE.h"
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
 * Cette fonction supprime un document de la collection avec son ID
 */
void deleteOneDocument(mongocxx::client &client) {
    mongocxx::database db = client["actiaDataBase"];
    //Demande à l'utilisateur le nom de la collection
    cout << "Entrer le nom de la collection : ";
    string collectionName;
    getline(cin, collectionName);
    //Vérifie si la collection existe
    if(!collectionExist(db, collectionName)){
        cout << "La collection n'existe pas.\n";
        return;
    }
    mongocxx::collection collection = db[collectionName]; //Récupère la collection
    //Demande à l'utilisateur l'ID du document à supprimer
    cout << "Entrer l'ID du document a supprimer : ";
    string id;
    getline(cin, id);
    //Requête pour supprimer le document
    bsoncxx::stdx::optional<mongocxx::result::delete_result> result = collection.delete_one(document{} << "_id" << id << finalize);
    //Vérifie si la suppression s'est bien passée
    if (result) {
        cout << "Document supprime avec succes.\n";
    } else {
        cout << "Erreur lors de la suppression du document.\n";
    }
}

/**
 * Cette fonction permet de supprimer tous les documents d'une collection
 */

void deleteAllDocuments(mongocxx::client &client) {
    //Chronomètre pour calculer le temps d'exécution
    auto startChrono = chrono::high_resolution_clock::now();
    mongocxx::database db = client["actiaDataBase"];
    //Demande à l'utilisateur le nom de la collection à vider
    cout << "Entrer le nom de la collection : ";
    string collectionName;
    getline(cin, collectionName);
    //Vérifie si la collection existe
    if(!collectionExist(db, collectionName)){
        cout << "La collection n'existe pas.\n";
        return;
    }
    mongocxx::collection collection = db[collectionName];
    //Requête pour supprimer tous les documents de la collection
    collection.delete_many({});
    cout << "Tous les documents ont ete supprimes avec succes.\n";
    //Chronomètre fin
    auto endChrono = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = endChrono - startChrono;
    cout << "Temps d'execution : " << elapsed.count() << " secondes\n";
}




