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
 *
*/
void updateOneDocument(mongocxx::client &client) {
    cout << "Entrez le nom de la collection ou se trouve le document : ";
    string collectionName;
    getline(cin, collectionName);

    if (!collectionExist(client["actiaDataBase"], collectionName)) {
        cout << "La collection n'existe pas.\n";
        return;
    }
    cout << "Entrez l'id du document : ";
    string id;
    getline(cin, id);
    auto collection = client["actiaDataBase"]["testCollection"];

    auto builder = bsoncxx::builder::stream::document{};
    bsoncxx::document::value query = builder
            << "_id" << bsoncxx::oid(id)
            << bsoncxx::builder::stream::finalize;

    auto doc = collection.find_one(query.view());

    cout << "Entrez la nouvelle valeur : ";
    string newType;
    getline(cin, newType);
    if (doc) {
        auto view = doc->view();

        if (view["metadata"]["tt:MetadataStream"]["tt:VideoAnalytics"]["tt:Frame"]["tt:Object"]["tt:Appearance"]["tt:Class"]["tt:Type"]["content"].get_utf8().value.to_string() == "Human") {
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
    // Chrono
    auto start = std::chrono::system_clock::now();
    // Demander à l'utilisateur le nom de la collection
    std::string collectionName;
    std::cout << "Entrez le nom de la collection: ";
    std::getline(std::cin, collectionName);
    // Demander à l'utilisateur la nouvelle valeur
    std::string newType;
    std::cout << "Entrez la nouvelle valeur pour remplacer 'Human': ";
    std::getline(std::cin, newType);
    auto collection = client["actiaDataBase"][collectionName];
    auto cursor = collection.find({});
    for(auto doc_view : cursor) {
        bool isUpdated = false;
        auto videoAnalytics = doc_view["tt:VideoAnalytics"].get_array().value;
        for(auto& frameDoc : videoAnalytics) {
            auto frame = frameDoc.get_document().value["tt:Frame"].get_array().value;
            for(auto& objectDoc : frame) {
                auto object = objectDoc.get_document().value["tt:Object"].get_array().value;
                for(auto& appearanceDoc : object) {
                    auto appearance = appearanceDoc.get_document().value["tt:Appearance"].get_array().value;
                    for(auto& classDoc : appearance) {
                        auto classData = classDoc.get_document().value["tt:Class"].get_array().value;
                        for(auto& typeDoc : classData) {
                            auto type = typeDoc.get_document().value["tt:Type"].get_array().value;
                            for(auto& typeFieldDoc : type) {
                                auto typeField = typeFieldDoc.get_document().value;
                                if(typeField["value"].get_utf8().value.to_string() == "Human") {
                                    typeFieldDoc.get_document().value["value"] = bsoncxx::types::b_utf8{newType};
                                    isUpdated = true;
                                }
                            }
                        }
                    }
                }
            }
        }
        if(isUpdated) {
            bsoncxx::builder::stream::document newDoc{};
            newDoc << "tt:VideoAnalytics" << videoAnalytics;
            bsoncxx::builder::stream::document replace_doc{};
            replace_doc << "$set" << newDoc;
            bsoncxx::builder::stream::document filter_doc{};
            filter_doc << "_id" << doc_view["_id"].get_oid().value;
            collection.find_one_and_replace(filter_doc.view(), replace_doc.view());
        }
    }
    // Fin chrono
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cout << "Temps d'execution : " << elapsed_seconds.count() << "s\n";
}
