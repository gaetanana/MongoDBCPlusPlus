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
#include <json/json.h>
#include <json/reader.h>
#include <iostream>
#include <sstream>

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;
using namespace std;

/**
* Cette fonction permet de modifier une valeur d'une clé en lui donnant l'id du document et une nouvelle valeur
 * Cette fonction modifie "Human" par une nouvelle valeur
*/
void updateOneDocument(mongocxx::client &client) {
    //Demande à l'utilisateur le nom de la collection ou se trouve le document à modifier
    cout << "Entrez le nom de la collection ou se trouve le document : ";
    string collectionName;
    getline(cin, collectionName);
    //Vérifie si la collection existe
    if (!collectionExist(client["actiaDataBase"], collectionName)) {
        cout << "La collection n'existe pas.\n";
        return;
    }
    //Demande à l'utilisateur l'id du document à modifier
    cout << "Entrez l'id du document : ";
    string id;
    getline(cin, id);
    auto collection = client["actiaDataBase"]["testCollection"];
    //Requête pour récupérer le document
    auto builder = bsoncxx::builder::stream::document{};
    bsoncxx::document::value query = builder
            << "_id" << bsoncxx::oid(id)
            << bsoncxx::builder::stream::finalize;
    //Récupère le document
    auto doc = collection.find_one(query.view());
    //Demande à l'utilisateur la nouvelle valeur
    cout << "Entrez la nouvelle valeur : ";
    string newType;
    getline(cin, newType);
    //Condition pour vérifier si le document existe
    if (doc) {
        auto view = doc->view();
        //Condition pour vérifier si la valeur de la clé est "Human"
        if (view["metadata"]["tt:MetadataStream"]["tt:VideoAnalytics"]["tt:Frame"]["tt:Object"]
        ["tt:Appearance"]["tt:Class"]["tt:Type"]["content"].get_utf8().value.to_string() == "Human") {
            //Requête pour modifier la valeur de la clé
            bsoncxx::builder::stream::document update_doc{};
            update_doc << "$set" << bsoncxx::builder::stream::open_document
                       << "metadata.tt:MetadataStream.tt:VideoAnalytics.tt:Frame.tt:Object.tt:Appearance.tt:Class.tt:Type.content" << newType
                       << bsoncxx::builder::stream::close_document;
            collection.update_one(query.view(), update_doc.view());
        }
    }
}

/**
* Cette fonction permet de modifier toutes les valeurs qui ont le type "Human" pour le remplacer par une nouvelle valeur
*/
void updateAllHumanDocument(mongocxx::client &client) {

    // Spécifier la base de données et la collection dans laquelle vous souhaitez mettre à jour les documents
    cout << "Entrez le nom de la collection ou se trouve les documents : ";
    string collectionName;
    getline(cin, collectionName);

    // Vérifie si la collection existe
    if(!collectionExist(client["actiaDataBase"], collectionName)){
        cout << "La collection n'existe pas.\n";
        return;
    }

    auto collection = client["actiaDataBase"][collectionName];

    // Début du chronométrage
    auto start = std::chrono::high_resolution_clock::now();


    // Créer le document de filtre
    bsoncxx::builder::stream::document filter_doc;
    filter_doc << "tt:VideoAnalytics.0.tt:Frame.0.tt:Object.0.tt:Appearance.0.tt:Class.0.tt:Type.0.value"
               << "Human";

    // Demander la nouvelle valeur à l'utilisateur
    cout << "Entrez la nouvelle valeur : ";
    string newValue;
    getline(cin, newValue);

    // Créer le document de mise à jour
    bsoncxx::builder::stream::document update_doc;
    update_doc << "$set"
               << bsoncxx::builder::stream::open_document << "tt:VideoAnalytics.0.tt:Frame.0.tt:Object.0."
                                                             "tt:Appearance.0.tt:Class.0.tt:Type.0.value"
               << newValue << bsoncxx::builder::stream::close_document;

    // Mettre à jour tous les documents qui correspondent au filtre
    collection.update_many(filter_doc.view(), update_doc.view());

    // Fin du chronométrage
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    cout << "Temps d'execution : " << elapsed.count() << " s\n";
}


/**
 * Cette fonction permet de modifier toutes les valeurs de Type pour les remplacer par une nouvelle valeur (par exemple : "Human" par "Car")
 */
void updateAllKeyTypeContent(mongocxx::client &client) {


    // Spécifiez la base de données et la collection dans laquelle vous voulez mettre à jour les documents
    cout << "Entrez le nom de la collection ou se trouve le document : ";
    string collectionName;
    getline(cin, collectionName);

    // Vérifie si la collection existe
    if(!collectionExist(client["actiaDataBase"], collectionName)){
        cout << "La collection n'existe pas.\n";
        return;
    }

    auto collection = client["actiaDataBase"][collectionName];

    // Demandez la nouvelle valeur à l'utilisateur
    cout << "Entrez la nouvelle valeur : ";
    string newValue;
    getline(cin, newValue);

    // Début du chronométrage
    auto start = std::chrono::high_resolution_clock::now();
    // Créer le document de mise à jour
    bsoncxx::builder::stream::document update_doc;
    update_doc << "$set"
               << bsoncxx::builder::stream::open_document << "tt:VideoAnalytics.0.tt:Frame."
                                                             "0.tt:Object.0.tt:Appearance.0.tt:Class.0.tt:Type.0.value"
               << newValue << bsoncxx::builder::stream::close_document;

    // Mettre à jour tous les documents
    collection.update_many({}, update_doc.view());

    // Fin du chronométrage
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    cout << "Temps d'execution : " << elapsed.count() << " s\n";
}