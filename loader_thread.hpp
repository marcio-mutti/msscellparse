#ifndef LOADER_H
#define LOADER_H

#include <fstream>
#include <map>
#include <regex>
#include <stdexcept>
#include <string>
#include <vector>
#include <sigc++/sigc++.h>
#include <boost/thread.hpp>

# include "banco.hpp"

namespace logparser {
enum logtype { mobswitch, mme };
std::string readline ( std::ifstream& );
class parser : public sigc::trackable {
public:
    parser();
    ~parser();
    void slot_new_file ( const std::string&, const logtype& ) throw ( const std::domain_error& );
    void slot_run() throw ( const std::runtime_error& );
    sigc::signal<void, const std::string&> signal_n_of_mobswitches();
    // Slots
    
protected:
    pgsql::db_connection connector;
    sigc::signal<void, const std::string&> signal_new_logger;
    sigc::signal<void, const std::string&> signal_n_of_mobswitches_;
    sigc::signal<void> signal_work_finish;
    std::vector<std::string> lista_de_mobswitches;
    std::map<std::string, boost::thread> threads_de_execucao;
    std::map<std::string, std::regex> triggers;
    std::map<std::string, std::regex> regexers;
};

}

#endif
