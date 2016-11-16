#include "janela_principal.hpp"
#include <iostream>
#include <boost/bind.hpp>

using namespace std;

janela_principal::janela_principal() : janelator ( nullptr ), box_principal ( nullptr ), lbl_n_23g ( nullptr ),   lbl_n_4g ( nullptr ),      lbl_n_central ( nullptr ), lbl_n_bsc ( nullptr ),   lbl_n_rnc ( nullptr ), lbl_n_mme ( nullptr ),  lbl_n_central_up ( nullptr ),  lbl_n_bsc_up ( nullptr ),
    lbl_n_rnc_up ( nullptr ),  lbl_n_mme_up ( nullptr ),  btn_load_23g ( nullptr ),  btn_load_4g ( nullptr ), btn_carregar ( nullptr ),
    btn_subir_banco ( nullptr ), work_thread() {
    try {
        janelator = Gtk::Builder::create_from_file ( "../Janela_Principal.glade" );
    } catch ( const Glib::FileError& error ) {
        cerr <<  "Encontrado um erro na tentativa de abrir o arquivo de definição da UI:" <<  endl;
        cerr <<  error.what() << endl <<   "Error code: " <<  error.code() <<  endl;
        throw;
    }
    // Build the interface
    janelator->get_widget ( "box_principal", box_principal );
    this->add ( *box_principal );
    janelator->get_widget ( "lbl_n_23g", lbl_n_23g );
    janelator->get_widget ( "lbl_n_4g",  lbl_n_4g );
    janelator->get_widget ( "lbl_n_bsc", lbl_n_bsc );
    janelator->get_widget ( "lbl_n_bsc_up", lbl_n_bsc_up );
    janelator->get_widget ( "lbl_n_central", lbl_n_central );
    janelator->get_widget ( "lbl_n_central_up",  lbl_n_central_up );
    janelator->get_widget ( "lbl_n_mme", lbl_n_mme );
    janelator->get_widget ( "lbl_n_mme_up", lbl_n_mme_up );
    janelator->get_widget ( "lbl_n_rnc", lbl_n_rnc );
    janelator->get_widget ( "lbl_n_rnc_up", lbl_n_rnc_up );
    janelator->get_widget ( "btn_load_23g", btn_load_23g );
    janelator->get_widget ( "btn_load_4g", btn_load_4g );
    janelator->get_widget ( "btn_carregar", btn_carregar );
    janelator->get_widget ( "btn_subir_banco", btn_subir_banco );
    // Connect the dots
    btn_load_23g->signal_clicked().connect ( sigc::mem_fun ( *this, &janela_principal::slot_btn_load_23g ) );
    btn_load_4g->signal_clicked().connect ( sigc::mem_fun ( *this, &janela_principal::slot_btn_load_4g ) );
    btn_carregar->signal_clicked().connect ( sigc::mem_fun ( *this, &janela_principal::slot_btn_carregar ) );
    btn_subir_banco->signal_clicked().connect ( sigc::mem_fun ( *this, &janela_principal::slot_btn_subir_banco ) );
    signal_new_file.connect ( sigc::mem_fun ( runner, &logparser::parser::slot_new_file ) );
    runner.signal_n_of_mobswitches().connect(sigc::mem_fun(*this, &janela_principal::slot_change_n_mobswitch));
    runner.signal_work_finish().connect(sigc::mem_fun(*this,&janela_principal::slot_ready_for_new_work));
    //box_principal->show_all_children();
    show_all_children();
}
janela_principal::~janela_principal() {}
void janela_principal::slot_btn_load_23g() {
    vector<string> filenames;
    Glib::RefPtr<Gtk::FileFilter> filter_logs ( nullptr );
    Gtk::FileChooserDialog filedialog ( *this, "Selecione os arquivos das Switches:", Gtk::FILE_CHOOSER_ACTION_OPEN );
    int dialog_result ( 0 );
    filedialog.set_select_multiple();
    filedialog.add_button ( "_Cancelar", Gtk::RESPONSE_CANCEL );
    filedialog.add_button ( "_Abrir", Gtk::RESPONSE_OK );
    filter_logs = Gtk::FileFilter::create();
    filter_logs->set_name ( "Log Files" );
    filter_logs->add_mime_type ( "text/plain" );
    filedialog.add_filter ( filter_logs );
    dialog_result = filedialog.run();
    switch ( dialog_result ) {
    case Gtk::RESPONSE_CANCEL:
        return;
    case Gtk::RESPONSE_OK:
        filenames = filedialog.get_filenames();
        for ( vector<string>::iterator riter = filenames.begin(); riter != filenames.end(); ++riter ) {
            signal_new_file.emit ( *riter, logparser::logtype::mobswitch );
        }
    }
}
void janela_principal::slot_btn_load_4g() {
}
void janela_principal::slot_btn_carregar() {
    btn_load_23g->set_sensitive(false);
    btn_carregar->set_label("Cancelar");
    work_thread=boost::thread(boost::bind(&logparser::parser::slot_run,&runner));
}
void janela_principal::slot_btn_subir_banco() {
}
void janela_principal::slot_change_n_mobswitch(const string& n_value) {
    lbl_n_23g->set_text(n_value);
}
void janela_principal::slot_ready_for_new_work(){
    btn_load_23g->set_sensitive(true);
    btn_carregar->set_label("Carregar Arquivos");
    //while (Gtk::Main::events_pending()) Gtk::Main::iteration();
}


