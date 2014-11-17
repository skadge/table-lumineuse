#ifndef HANDLER_H
#define HANDLER_H

#include <memory>
#include <json/json.h>

#include "http_server/request_handler.hpp"

class Table;

class handler : public http::server::request_handler {

public:

    handler () {}

    void handle_request(const http::server::request& request,
                        http::server::reply& response);

    void set_table(std::shared_ptr<Table> table) {this->table = table;}

private:
    Json::Value root; // will contains the root value after parsing.
    Json::Reader reader;

    std::shared_ptr<Table> table;

};

#endif
