#ifndef JANELA_PRINCIPAL
#define JANELA_PRINCIPAL

#include <memory>
#include <gtkmm-3.0/gtkmm.h>
#include <boost/thread.hpp>

# include "loader_thread.hpp"

class janela_principal : public Gtk::ApplicationWindow {
  public:
    janela_principal();
    ~janela_principal();
  protected:
    Glib::RefPtr<Gtk::Builder> janelator;
    //Widgets
    Gtk::Box * box_principal;
    Gtk::Label * lbl_n_23g,  * lbl_n_4g,  *lbl_n_central,  * lbl_n_bsc,  * lbl_n_rnc,
        * lbl_n_mme,  *lbl_n_central_up,  *lbl_n_bsc_up,  *lbl_n_rnc_up,  *lbl_n_mme_up;
    Gtk::Button * btn_load_23g,  * btn_load_4g, * btn_carregar,  * btn_subir_banco;
    Gtk::Button * btn_db_clean;
    Gtk::Statusbar * sts_bar;
    Gtk::Spinner * sts_spin;
    // Signals
    sigc::signal<void, const std::string&, const logparser::logtype&> signal_new_file;
    // Slots
    void slot_btn_load_23g();
    void slot_btn_load_4g();
    void slot_btn_carregar();
    void slot_btn_subir_banco();
    void slot_change_n_mobswitch(const std::string&);
    void slot_ready_for_new_work();
    void slot_readied_switches(const std::string&, const std::string&, const std::string&);
    void slot_db_working_node(std::string);
    void slot_db_cleaner();
    std::string slot_open_connect_string_file();
    // Variables
    logparser::parser runner;
    //std::shared_ptr<boost::thread> work_thread;
    boost::thread work_thread;
};




#endif
