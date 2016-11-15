#ifndef JANELA_PRINCIPAL
#define JANELA_PRINCIPAL

#include <gtkmm-3.0/gtkmm.h>

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
    // Signals
    sigc::signal<void, const std::string&, const logparser::logtype&> signal_new_file;
    // Slots
    void slot_btn_load_23g();
    void slot_btn_load_4g();
    void slot_btn_carregar();
    void slot_btn_subir_banco();
    // Variables
    logparser::parser runner;
};




#endif
