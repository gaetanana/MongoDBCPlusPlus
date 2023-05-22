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
    for (auto doc: cursor) {
        auto analytics = doc["tt:VideoAnalytics"];
        if (analytics && analytics.type() == bsoncxx::type::k_array) {
            for (auto frame: analytics.get_array().value) {
                auto objects = frame["tt:Frame"][0]["tt:Object"];
                if (objects && objects.type() == bsoncxx::type::k_array) {
                    for (auto appearance: objects.get_array().value) {
                        auto classes = appearance["tt:Appearance"][0]["tt:Class"];
                        if (classes && classes.type() == bsoncxx::type::k_array) {
                            bool hasHumanWithHighLikelihood = std::any_of(classes.get_array().value.begin(),
                                                                          classes.get_array().value.end(),
                                                                          [](auto type) {
                                                                              auto typeVal = type["tt:Type"][0]["value"];
                                                                              auto likelihood = type["tt:Type"][0]["attributes"]["Likelihood"];
                                                                              return typeVal && likelihood &&
                                                                                     likelihood.type() ==
                                                                                     bsoncxx::type::k_utf8 &&
                                                                                     typeVal.get_utf8().value.to_string() ==
                                                                                     "Human" &&
                                                                                     stod(likelihood.get_utf8().value.to_string()) >=
                                                                                     0.50;
                                                                          });

                            if (hasHumanWithHighLikelihood) {
                                nbDocs++;
                                break;
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
    cout << "Temps d'execution : " << chrono::duration_cast<chrono::microseconds>(elapsed).count()
         << " microsecondes\n";
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

    auto cursor = collection.find({});

    int documentCount = 0;
    for (auto &&doc: cursor) {
        auto videoAnalytics = doc["tt:VideoAnalytics"].get_array().value;
        for (auto &&analytics: videoAnalytics) {
            auto frames = analytics["tt:Frame"].get_array().value;
            for (auto &&frame: frames) {
                auto objects = frame["tt:Object"].get_array().value;
                for (auto &&object: objects) {
                    auto appearances = object["tt:Appearance"].get_array().value;
                    for (auto &&appearance: appearances) {
                        auto classes = appearance["tt:Class"].get_array().value;
                        for (auto &&cls: classes) {
                            auto types = cls["tt:Type"].get_array().value;
                            for (auto &&type: types) {
                                std::string valueType = type["value"].get_utf8().value.to_string();
                                double valueLikelihood = std::stod(
                                        type["attributes"]["Likelihood"].get_utf8().value.to_string());
                                std::string utcTime = frame["UtcTime"].get_utf8().value.to_string();
                                if (valueType == "Human" && valueLikelihood > 0.5 && utcTime > "2023-04-21T14:45:23") {
                                    documentCount++;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    cout << "Nombre de documents: " << documentCount << "\n";
}

/**
 * Cette fonction permet de lire tout les documents d'une collection avec un type Human et une probabilité
 * Elle permet de filtrer les documents qui possèdent le type "Human"
 * Elle permet de filtrer les documents qui possèdent une probabilité supérieure à 0.5
 * Elle permet de filtrer les documents qui possèdent une date supérieure à une date donnée dans le programme
 * Elle permet de filtrer le genre Masculin
 */
void readAllDocumentWithHumanProbabilityAndDateGender(mongocxx::client &client) {
    //Chrono
    auto start = chrono::high_resolution_clock::now();
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
    int documentCount = 0;
    for (auto &&doc: cursor) {
        auto videoAnalytics = doc["tt:VideoAnalytics"].get_array().value;
        for (auto &&analytics: videoAnalytics) {
            auto frames = analytics["tt:Frame"].get_array().value;
            for (auto &&frame: frames) {
                auto objects = frame["tt:Object"].get_array().value;
                for (auto &&object: objects) {
                    auto appearances = object["tt:Appearance"].get_array().value;
                    for (auto &&appearance: appearances) {
                        auto classes = appearance["tt:Class"].get_array().value;
                        for (auto &&cls: classes) {
                            auto types = cls["tt:Type"].get_array().value;
                            for (auto &&type: types) {
                                std::string valueType = type["value"].get_utf8().value.to_string();
                                double valueLikelihood = std::stod(
                                        type["attributes"]["Likelihood"].get_utf8().value.to_string());
                                std::string utcTime = frame["UtcTime"].get_utf8().value.to_string();


                                if (appearance["tt:Extension"] &&
                                    appearance["tt:Extension"].get_array().value[0]["HumanFace"] &&
                                    appearance["tt:Extension"].get_array().value[0]["HumanFace"].get_array().value[0]["Gender"] &&
                                    appearance["tt:Extension"].get_array().value[0]["HumanFace"].get_array().value[0]["Gender"].get_array().value[0]["Male"] &&
                                    !appearance["tt:Extension"].get_array().value[0]["HumanFace"].get_array().value[0]["Gender"].get_array().value[0]["Male"].get_array().value[0]["value"].get_utf8().value.empty()) {

                                    double valueGender = std::stod(
                                            appearance["tt:Extension"].get_array().value[0]["HumanFace"].get_array().value[0]["Gender"].get_array().value[0]["Male"].get_array().value[0]["value"].get_utf8().value.to_string());
                                    if (valueType == "Human" && valueLikelihood > 0.5 && utcTime > "2023-05-16" &&
                                        valueGender > 0.5) {
                                        cout << doc["_id"].get_oid().value.to_string() << "\n";
                                        documentCount++;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    cout << "Nombre de documents: " << documentCount << "\n";
    auto finish = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = finish - start;
    cout << "Temps d'execution : " << chrono::duration_cast<chrono::microseconds>(elapsed).count()
         << " microsecondes\n";

}
