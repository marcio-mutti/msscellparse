#ifndef BANCO_H
#define BANCO_H

#include <stdexcept>
#include <string>

#include <libpq-fe.h>

namespace pgsql {
	class query_result {
	public:
		query_result();
		~query_result();
	protected:
		size_t n_columns, n_rows;
	};
	void handle_command(PGresult* result, const std::string& context) throw(const std::runtime_error&);
	
};

#endif
