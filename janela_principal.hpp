#ifndef JANELA_PRINCIPAL
#define JANELA_PRINCIPAL

#include <gtkmm-3.0/gtkmm.h>

class janela_principal : public Gtk::ApplicationWindow
 {
public:
    janela_principal();
    ~janela_principal();
protected:
    Glib::RefPtr<Gtk::Builder> janelator;
    //Gtk::ApplicationWindow * mainwindow;
    Gtk::Box * box_principal;
    Gtk::Label * lbl_n_23g,  * lbl_n_4g,  *lbl_n_central,  * lbl_n_bsc,  * lbl_n_rnc, 
      * lbl_n_mme,  *lbl_n_central_up,  *lbl_n_bsc_up,  *lbl_n_rnc_up,  *lbl_n_mme_up;
    Gtk::Button * btn_load_23g,  * btn_load_4g, * btn_carregar,  * btn_subir_banco;
    
    void slot_btn_load_23g();
    void slot_btn_load_4g();
    void slot_btn_carregar();
    void slot_btn_subir_banco();
};




#endif