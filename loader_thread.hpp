#ifndef LOADER_H
#define LOADER_H

#include <fstream>
#include <map>
#include <memory>
#include <regex>
#include <set>
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
    sigc::signal<void, const std::string&, const std::string&, const std::string&>
    signal_n_of_mobswitches_ready();
    sigc::signal<std::string> signal_ask_for_connect_string_file();
    sigc::signal<void,std::string> signal_send_upload_node();
    // Slots
    void slot_new_file ( const std::string&, const logtype& ) noexcept(false);
    void slot_run() noexcept(false);
    void slot_upload_data() noexcept;

  protected:
    //Variables
    pgsql::db_connection connector;
    std::vector<std::string> lista_de_mobswitches;
    std::vector<std::string> lista_de_mmes;
    std::map<std::string, std::shared_ptr<boost::thread>> threads_de_execucao;
    std::vector<std::shared_ptr<::mobswitch>> working_switches;
    std::vector<std::shared_ptr<::mme>> working_mmes;
    std::map<std::string, std::regex> triggers;
    std::map<std::string, std::regex> regexers;
    pgsql::db_connection db_interface;
    std::set<std::string> statements;
    //Signals
    sigc::signal<void, const std::string&> signal_new_logger;
    sigc::signal<void, const std::string&> signal_n_of_mobswitches_;
    sigc::signal<void> signal_work_finish_;
    sigc::signal<void, const std::string&, const std::string&, const std::string&>
    signal_n_of_mobswitches_ready_;
    sigc::signal<std::string> signal_ask_for_connect_string_file_;
    sigc::signal<void,std::string> signal_send_upload_node_;
    //Methods
    void parse_switch(std::shared_ptr<::mobswitch>,const std::string&);
    void parse_mme(std::shared_ptr<::mme>, const std::string&);

};

}

#endif
