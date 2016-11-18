#ifndef BANCO_H
#define BANCO_H

# include <map>
#include <stdexcept>
#include <string>
# include <vector>

#include <libpq-fe.h>

namespace pgsql {
class query_result {
  public:
    query_result();
    query_result(PGresult *) noexcept(false);
    ~query_result();
    void clear();
    bool success() const;
    void load_from_result(PGresult *) noexcept(false);
    std::string get_value(const size_t&, const size_t&) const;
  protected:
    bool success_;
    size_t n_columns, n_rows;
    std::vector<std::string> column_names;
    std::vector<std::vector<std::string>> fetched_values;
};
void handle_command(PGresult* result, const std::string& context) noexcept(false);
class db_connection {
  public:
    db_connection();
    ~db_connection();
    void load_database_config(const std::string&);
    void connect_to_database() noexcept(false);
    void execute_command(const std::string&) noexcept(false);
    query_result execute_returning_query(const std::string&) noexcept(false);
    void prepare_statement(const std::string&,  const std::string&,  const int&) noexcept(false);
    void execute_prepared_statement(const std::string&,
                                    const std::vector<std::string>&) noexcept(false);
    query_result execute_returning_prepared_statement(const std::string&,
            const std::vector<std::string>&) noexcept(false);
    bool connection_up() const;
  protected:
    PGconn * dbconn;
    std::map<std::string, std::string> connection_parameters;
    std::map<std::string, int> n_paremeter_for_stmt;
};
};

#endif
