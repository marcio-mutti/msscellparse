#include "banco.hpp"

using namespace std;

void pgsql::handle_command(PGresult* result, const std::string& context="") throw(const runtime_error&) {
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

pgsql::query_result::query_result() : n_columns(0), n_rows(0)
{
}
