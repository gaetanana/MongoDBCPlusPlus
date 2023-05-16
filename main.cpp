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


int main() {
    mongocxx::instance inst{};
    mongocxx::client conn{mongocxx::uri{"mongodb://root:examplepassword@localhost:27017"}};

    auto collection = conn["actiaDataBase"]["testCoc+llection"];

    // Insert a document
    bsoncxx::document::value doc_value = document{} << "name" << "John Doe" << "age" << 30 << finalize;
    bsoncxx::document::view doc_view = doc_value.view();
    bsoncxx::stdx::optional<mongocxx::result::insert_one> result = collection.insert_one(doc_view);
    std::cout << "Inserted " << result->inserted_id().get_oid().value.to_string() << std::endl;

    // Update the document
    /*document filter_builder{};
    filter_builder << "name" << "John Doe";
    collection.update_one(filter_builder.view(),document{} << "$set" << open_document << "age" << 31 << close_document << finalize);
    std::cout << "Updated" << std::endl;*/

    // Delete the document
    /*document filter_builder_delete{};
    filter_builder_delete << "name" << "John Doe";
    collection.delete_one(filter_builder_delete.view());
    std::cout << "Deleted" << std::endl;*/

    // Read all documents
    mongocxx::cursor cursor = collection.find({});
    for(auto doc : cursor) {
        std::cout << bsoncxx::to_json(doc) << std::endl;
    }
    return 0;
}
