#ifndef LOADER_H
#define LOADER_H

#include <fstream>
#include <map>
#include <memory>
#include <regex>
#include <stdexcept>
#include <string>
#include <vector>

#include <sigc++/sigc++.h>
#include <boost/thread.hpp>
#include "banco.hpp"
#include "estruturas.hpp"

namespace logparser {
enum logtype { mobswitch, mme };
std::string readline ( std::ifstream& );
class parser : public sigc::trackable {
public:
    parser();
    ~parser();
    //Methods
    sigc::signal<void, const std::string&> signal_n_of_mobswitches();
    sigc::signal<void> signal_work_finish();
    // Slots
    void slot_new_file ( const std::string&, const logtype& ) throw ( const std::domain_error& );
    void slot_run() throw ( const std::runtime_error& );
    
protected:
    //Variables
    pgsql::db_connection connector;
    std::vector<std::string> lista_de_mobswitches;
    std::vector<std::string> lista_de_mmes;
    std::map<std::string, std::shared_ptr<boost::thread>> threads_de_execucao;
    std::map<boost::thread::id, std::shared_ptr<::mobswitch>> working_switches;
    std::map<boost::thread::id, std::shared_ptr<::mme>> working_mmes;
    std::map<std::string, std::regex> triggers;
    std::map<std::string, std::regex> regexers;
    //Signals
    sigc::signal<void, const std::string&> signal_new_logger;
    sigc::signal<void, const std::string&> signal_n_of_mobswitches_;
    sigc::signal<void> signal_work_finish_;
    //Methods
    void parse_switch(std::shared_ptr<::mobswitch>,const std::string&);
    void parse_mme(std::shared_ptr<::mme>, const std::string&);
    
};

}

#endif
