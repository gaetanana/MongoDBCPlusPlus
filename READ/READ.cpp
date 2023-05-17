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
#include <chrono>

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
void readOneDocument(mongocxx::client &client) {
    mongocxx::database db = client["actiaDataBase"];
    cout << "Entrer le nom de la collection : ";
    string collectionName;

    getline(cin, collectionName);
    if (!collectionExist(db, collectionName)) {
        cout << "La collection n'existe pas.\n";
        return;
    }
    mongocxx::collection collection = db[collectionName];
    cout << "Entrer l'ID du document a lire : ";
    string id;
    getline(cin, id);
    bsoncxx::stdx::optional<bsoncxx::document::value> result = collection.find_one(
            document{} << "_id" << id << finalize);
    if (result) {
        cout << bsoncxx::to_json(*result) << "\n";
    } else {
        cout << "Erreur lors de la lecture du document.\n";
    }
}

/**
 * Cette fonction permet de lire tous les documents d'une collection
 */
void readAllDocument(mongocxx::client &client) {
    //Chrono
    auto startChrono = chrono::high_resolution_clock::now();
    int nbDocuments = 0;

    mongocxx::database db = client["actiaDataBase"];
    cout << "Entrer le nom de la collection : ";
    string collectionName;

    getline(cin, collectionName);
    if (!collectionExist(db, collectionName)) {
        cout << "La collection n'existe pas.\n";
        return;
    }
    mongocxx::collection collection = db[collectionName];
    mongocxx::cursor cursor = collection.find({});
    for (auto doc: cursor) {
        //cout << bsoncxx::to_json(doc) << "\n";
        //Affiche uniquement l'id du document
        cout << doc["_id"].get_oid().value.to_string() << "\n";
        nbDocuments++;
    }
    cout << "Nombre de documents : " << nbDocuments << "\n";
    //Fin chrono
    auto endChrono = chrono::high_resolution_clock::now();
    auto durationChrono = chrono::duration_cast<chrono::microseconds>(endChrono - startChrono);
    cout << "Temps d'execution : " << durationChrono.count() << " microsecondes\n";

}

/**
 * Cette fonction permet de lire tous les documents d'une collection avec un type Human
 */
void readAllDocumentWithHuman(mongocxx::client &client) {
    //Chrono
    auto startChrono = chrono::high_resolution_clock::now();
    int nbDocuments = 0;

    mongocxx::database db = client["actiaDataBase"];
    cout << "Entrer le nom de la collection : ";
    string collectionName;

    getline(cin, collectionName);
    if (!collectionExist(db, collectionName)) {
        cout << "La collection n'existe pas.\n";
        return;
    }
    mongocxx::collection collection = db[collectionName];
    auto cursor = collection.find(
            document{} << "tt:VideoAnalytics.0.tt:Frame.0.tt:Object.0.tt:Appearance.0.tt:Class.0.tt:Type.0.value"
                       << "Human" << finalize);
    for (auto doc: cursor) {
        //Affiche uniquement l'id du document
        cout << doc["_id"].get_oid().value.to_string() << "\n";
        nbDocuments++;
    }
    cout << "Nombre de documents : " << nbDocuments << "\n";
    //Fin chrono
    auto endChrono = chrono::high_resolution_clock::now();
    auto durationChrono = chrono::duration_cast<chrono::microseconds>(endChrono - startChrono);
    cout << "Temps d'execution : " << durationChrono.count() << " microsecondes\n";

}

/**
 * Cette fonction permet de lire tous les documents d'une collection avec un type Human et une probabilité
 * supérieure à 0.5
 */
void readAllDocumentWithHumanProbability(mongocxx::client &client) {
    auto start = chrono::high_resolution_clock::now();
    int nbDocs = 0;
    mongocxx::database db = client["actiaDataBase"];
    cout << "Entrer le nom de la collection : ";
    string collectionName;

    getline(cin, collectionName);
    if (!collectionExist(db, collectionName)) {
        cout << "La collection n'existe pas.\n";
        return;
    }
    mongocxx::collection collection = db[collectionName];
    auto cursor = collection.find({});

    for (auto doc : cursor) {
        auto analytics = doc["tt:VideoAnalytics"];
        if (analytics && analytics.type() == bsoncxx::type::k_array) {
            for (auto frame : analytics.get_array().value) {
                auto objects = frame["tt:Frame"][0]["tt:Object"];
                if (objects && objects.type() == bsoncxx::type::k_array) {
                    for (auto appearance : objects.get_array().value) {
                        auto classes = appearance["tt:Appearance"][0]["tt:Class"];
                        if (classes && classes.type() == bsoncxx::type::k_array) {
                            for (auto type : classes.get_array().value) {
                                auto typeVal = type["tt:Type"][0]["value"];
                                auto likelihood = type["tt:Type"][0]["attributes"]["Likelihood"];
                                if (typeVal && likelihood && likelihood.type() == bsoncxx::type::k_utf8 &&
                                    typeVal.get_utf8().value.to_string() == "Human" &&
                                    stod(likelihood.get_utf8().value.to_string()) > 0.5) {
                                    nbDocs++;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    auto finish = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = finish - start;
    cout << "Nombre de documents : " << nbDocs << endl;
    cout << "Temps d'execution : " << chrono::duration_cast<chrono::microseconds>(elapsed).count() << " microsecondes\n";
}


/**
 * Cette fonction permet de lire tous les documents d'une collection avec un type Human et une probabilité
 * supérieure à 0.5 et supérieure à une date donnée dans le programme
 */
void readAllDocumentWithHumanProbabilityAndDate(mongocxx::client &client) {
    mongocxx::database db = client["actiaDataBase"];
    cout << "Entrer le nom de la collection : ";
    string collectionName;

    getline(cin, collectionName);
    if (!collectionExist(db, collectionName)) {
        cout << "La collection n'existe pas.\n";
        return;
    }
    mongocxx::collection collection = db[collectionName];

    // Define the given date (modify as needed)
    std::string given_date = "2023-05-16";

    // Modify the query to filter documents with type Human, probability greater than 0.5, and date greater than the given date
    auto cursor = collection.find(
            document{} << "type" << "Human" << "probability" << open_document << "$gt" << 0.5 << close_document
                       << "date" << open_document << "$gt" << given_date << close_document << finalize);

    for (auto doc: cursor) {
        cout << bsoncxx::to_json(doc) << "\n";
    }
}

/**
 * Cette fonction permet de lire tous les documents d'une collection avec un type Human et une probabilité
 * supérieure à 0.5 et supérieure à une date donnée dans le programme ainsi que le genre de la personne doit "Male"
 */
void readAllDocumentWithHumanProbabilityAndDateGender(mongocxx::client &client) {
    mongocxx::database db = client["actiaDataBase"];
    cout << "Entrer le nom de la collection : ";
    string collectionName;
    getline(cin, collectionName);
    if (!collectionExist(db, collectionName)) {
        cout << "La collection n'existe pas.\n";
        return;
    }
    mongocxx::collection collection = db[collectionName];
    std::string given_date = "2023-05-16";
    auto cursor = collection.find(
            document{} << "type" << "Human" << "probability" << open_document << "$gt" << 0.5 << close_document
                       << "date" << open_document << "$gt" << given_date << close_document << "gender" << "Male"
                       << finalize);

    for (auto doc: cursor) {
        cout << bsoncxx::to_json(doc) << "\n";
    }
}