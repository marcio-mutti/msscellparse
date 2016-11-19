#include "banco.hpp"
#include <fstream>
#include <iostream>

using namespace std;

void pgsql::handle_command(PGresult* result, const std::string& context="") noexcept(false) {
    bool has_error(false);
    string error_message;
    if (PQresultStatus(result) != PGRES_COMMAND_OK) {
        error_message=PQresultErrorMessage(result);
        if (context.size() > 0) {
            error_message+='\n';
            error_message+=context;
        }
        has_error=true;
    }
    PQclear(result);
    if (has_error) throw (runtime_error(error_message));
}

pgsql::query_result::query_result() : success_(false), n_columns(0), n_rows(0)  {}
pgsql::query_result::query_result(PGresult * result) noexcept(false):
    success_(false), n_columns(0), n_rows(0)  {
    load_from_result(result);
}
pgsql::query_result::~query_result()                                            {}
void pgsql::query_result::clear() {
    success_ = false;
    n_columns = n_rows = 0;
    column_names.clear();
    fetched_values.clear();
}
bool pgsql::query_result::success() const         {
    return success_;
}
void pgsql::query_result::load_from_result(PGresult* result) noexcept(false) {
    bool query_error(false);
    string error_message;
    clear();
    if (PQresultStatus(result) !=  PGRES_TUPLES_OK) {
        query_error = true;
        error_message = PQresultErrorMessage(result);
        success_ = false;
    } else {
        n_rows = PQntuples(result);
        n_columns = PQnfields(result);
        for(size_t i = 0; i != n_columns; ++i) {
            column_names.push_back(PQfname(result, i));
        }
        for (size_t j = 0; j != n_rows; ++j) {
            vector<string> this_result;
            this_result.resize(n_columns);
            for (size_t i = 0; i != n_columns; ++i) {
                this_result.at(i) = PQgetvalue(result, j, i);
            }
            fetched_values.push_back(this_result);
        }
    }
    PQclear(result);
    if (query_error) throw(runtime_error(error_message));
    success_=true;
}

string pgsql::query_result::get_value(const size_t &row, const size_t &column) const {
    if (row+1>fetched_values.size()) return "";
    if (column+1>fetched_values.at(row).size()) return "";
    return fetched_values.at(row).at(column);
}

pgsql::db_connection::db_connection() : dbconn(nullptr) {}
pgsql::db_connection::~db_connection()                  {}

void pgsql::db_connection::load_database_config(const std::string& filename) {
    ifstream configfile(filename);
    bool titulo(true);
    if (configfile.fail()) {
        cerr << "Não foi possível abrir o arquivo " << filename << "." <<  endl;
        return;
    }
    while (configfile.good()) {
        string keyword, parameter;
        if (titulo) {
            titulo=false;
            continue;
        }
        configfile >> keyword >> parameter;
        connection_parameters.insert({keyword, parameter});
    }
    configfile.close();
}
void pgsql::db_connection::connect_to_database() noexcept(false) {
    const char ** keywords(nullptr);
    const char ** parameters(nullptr);
    int n(0);
    if (connection_parameters.size() ==  0)
        throw(runtime_error("Não foram fornecidos os dados de conexão."));
    if (dbconn !=  nullptr)
        PQfinish(dbconn);
    keywords = new const char*[connection_parameters.size()+1];
    parameters = new const char*[connection_parameters.size()+1];
    for (map<string, string>::iterator citer = connection_parameters.begin();
            citer != connection_parameters.end(); ++citer) {
        keywords[n] = citer->first.c_str();
        parameters[n++] = citer->second.c_str();
    }
    keywords[n] = 0;
    parameters[n] = 0;
    dbconn = PQconnectdbParams(keywords, parameters, 0);
    delete[] keywords;
    delete[] parameters;
    if (PQstatus(dbconn) != CONNECTION_OK) {
        PQfinish(dbconn);
        dbconn = nullptr;
        throw(runtime_error(PQerrorMessage(dbconn)));
    }
}
void pgsql::db_connection::execute_command(const std::string& query) noexcept(false) {
    handle_command(PQexec(dbconn, query.c_str()), query);
}
pgsql::query_result pgsql::db_connection::execute_returning_query(const std::string& query)
noexcept(false) {
    //pgsql::query_result result;
    //result.load_from_result(PQexec(dbconn, query.c_str()));
    //return result;
    return pgsql::query_result{PQexec(dbconn, query.c_str())};
}
void pgsql::db_connection::prepare_statement(const string& stmt_name, const string& stmt_query,
        const int& count) noexcept(false) {
    handle_command(PQprepare(dbconn, stmt_name.c_str(), stmt_query.c_str(), count, NULL));
    n_paremeter_for_stmt.insert({stmt_name, count});
}
void pgsql::db_connection::execute_prepared_statement(const string& stmt_name,
        const vector<string>& stmt_parameters) noexcept(false) {
    const char * paramvalues[n_paremeter_for_stmt.at(stmt_name)];
    int n(0);
    for (vector<string>::const_iterator piter = stmt_parameters.cbegin();
            piter != stmt_parameters.cend(); ++piter) {
        paramvalues[n++] = piter->c_str();
    }
    handle_command(PQexecPrepared(dbconn, stmt_name.c_str(), n_paremeter_for_stmt.at(stmt_name),
                                  paramvalues, NULL, NULL, 0));
}
pgsql::query_result pgsql::db_connection::execute_returning_prepared_statement(
    const string& stmt_name,
    const vector<string>& stmt_parameters) noexcept(false) {
    const char * paramvalues[n_paremeter_for_stmt.at(stmt_name)];
    int n(0);
    for (vector<string>::const_iterator piter = stmt_parameters.cbegin();
            piter != stmt_parameters.cend(); ++piter) {
        paramvalues[n++] = piter->c_str();
    }
    return pgsql::query_result{PQexecPrepared(dbconn, stmt_name.c_str(), n_paremeter_for_stmt.at(stmt_name), paramvalues, NULL, NULL, 0)};
}
bool pgsql::db_connection::connection_up() const {
    return PQstatus(dbconn)==CONNECTION_OK;
}
