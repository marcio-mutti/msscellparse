#include "janela_principal.hpp"
#include <iostream>
#include <boost/bind.hpp>

using namespace std;

janela_principal::janela_principal() : janelator ( nullptr ), box_principal ( nullptr ),
    lbl_n_23g ( nullptr ), lbl_n_4g ( nullptr ), lbl_n_central ( nullptr ), lbl_n_bsc ( nullptr ),
    lbl_n_rnc ( nullptr ), lbl_n_mme ( nullptr ), lbl_n_central_up ( nullptr ),
    lbl_n_bsc_up ( nullptr ), lbl_n_rnc_up ( nullptr ), lbl_n_mme_up ( nullptr ),
    lbl_n_ss7arq(nullptr), lbl_n_ss7(nullptr),
    btn_load_23g ( nullptr ), btn_load_4g ( nullptr ), btn_carregar ( nullptr ),
    btn_subir_banco ( nullptr ), btn_db_clean(nullptr), btn_load_ss7(nullptr), sts_bar(nullptr),
    sts_spin(nullptr),
    work_thread() {
    try {
        janelator = Gtk::Builder::create_from_file ( "../msscellparse/Janela_Principal.glade" );
    } catch ( const Glib::FileError& error ) {
        cerr <<  "Encontrado um erro na tentativa de abrir o arquivo de definição da UI:" <<  endl;
        cerr <<  error.what() << endl <<   "Error code: " <<  error.code() <<  endl;
        cerr << "Diretorio de trabalho: " << Glib::get_current_dir() << endl;
        throw;
    }
    // Build the interface
    janelator->get_widget ( "box_principal", box_principal );
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
    janelator->get_widget ( "lbl_n_ss7arq", lbl_n_ss7arq );
    janelator->get_widget ( "lbl_n_ss7", lbl_n_ss7 );
    janelator->get_widget ( "btn_load_23g", btn_load_23g );
    janelator->get_widget ( "btn_load_4g", btn_load_4g );
    janelator->get_widget ( "btn_carregar", btn_carregar );
    janelator->get_widget ( "btn_subir_banco", btn_subir_banco );
    janelator->get_widget ( "btn_db_clean", btn_db_clean);
    janelator->get_widget ( "btn_load_ss7", btn_load_ss7 );
    janelator->get_widget ( "sts_bar", sts_bar );
    janelator->get_widget ( "sts_spin", sts_spin );
    this->add ( *box_principal );
    // Connect the dots
    btn_load_23g->signal_clicked().connect ( sigc::mem_fun ( *this,
            &janela_principal::slot_btn_load_23g ) );
    btn_load_4g->signal_clicked().connect ( sigc::mem_fun ( *this,
                                            &janela_principal::slot_btn_load_4g ) );
    btn_load_ss7->signal_clicked().connect(sigc::mem_fun(*this,&janela_principal::slot_btn_load_ss7));
    btn_carregar->signal_clicked().connect ( sigc::mem_fun ( *this,
            &janela_principal::slot_btn_carregar ) );
    btn_subir_banco->signal_clicked().connect ( sigc::mem_fun ( *this,
            &janela_principal::slot_btn_subir_banco ) );
    btn_db_clean->signal_clicked().connect(sigc::mem_fun(*this,&janela_principal::slot_db_cleaner));
    signal_new_file.connect ( sigc::mem_fun ( runner, &logparser::parser::slot_new_file ) );
    runner.signal_n_files().connect(sigc::mem_fun(*this,&janela_principal::slot_change_n_file));
    runner.signal_work_finish().connect(sigc::mem_fun(*this,
                                        &janela_principal::slot_ready_for_new_work));
    runner.signal_n_of_mobswitches_ready().connect ( sigc::mem_fun(*this,
            &janela_principal::slot_readied_switches) );
    runner.signal_n_of_mmes_ready().connect( sigc::mem_fun(*this,&janela_principal::slot_readied_mmes));
    runner.signal_ask_for_connect_string_file().connect (sigc::mem_fun(*this,
            &janela_principal::slot_open_connect_string_file));
    runner.signal_send_upload_node().connect(sigc::mem_fun(*this,
            &janela_principal::slot_db_working_node));
    show_all_children();
}
janela_principal::~janela_principal() {}
void janela_principal::slot_btn_load_23g() {
    log_loader("Selecione os arquivos das Switches:",logparser::logtype::mobswitch);
}
void janela_principal::slot_btn_load_4g() {
    log_loader("Selecione os arquivos das Mmes:",logparser::logtype::mme);
}

void janela_principal::slot_btn_load_ss7() {
    log_loader("Selecione os arquivos de log com informações de rotas:",logparser::logtype::ss7);
}
void janela_principal::slot_btn_carregar() {
    if (btn_carregar->get_label() != "Cancelar") {
        btn_load_23g->set_sensitive(false);
        btn_load_4g->set_sensitive(false);
        btn_carregar->set_label("Cancelar");
        //work_thread=make_shared<boost::thread>(boost::thread(boost::bind(&logparser::parser::slot_run,&runner)));
        //work_thread=make_shared<boost::thread>(boost::bind(&logparser::parser::slot_run,&runner));
        //work_thread=boost::thread([&](){ runner.slot_run(); });
        runner.slot_run();
    } else {
    }
}
void janela_principal::slot_btn_subir_banco() {
    btn_carregar->set_sensitive(false);
    btn_load_23g->set_sensitive(false);
    btn_load_4g->set_sensitive(false);
    btn_subir_banco->set_sensitive(false);
    runner.slot_upload_data();
    btn_carregar->set_sensitive(true);
    btn_load_23g->set_sensitive(true);
    btn_load_4g->set_sensitive(true);
    btn_subir_banco->set_sensitive(true);
}

void janela_principal::slot_ready_for_new_work() {
    btn_load_23g->set_sensitive(true);
    btn_load_4g->set_sensitive(true);
    lbl_n_23g->set_text("0");
    lbl_n_4g->set_text("0");
    btn_carregar->set_label("Carregar Arquivos");
    //while (Gtk::Main::events_pending()) Gtk::Main::iteration();
    //DEBUG

}
void janela_principal::slot_readied_switches(const std::string& n_sw_value,
        const string &n_bsc_value,
        const string& n_rnc_value) {
    lbl_n_central->set_text(n_sw_value);
    lbl_n_bsc->set_text(n_bsc_value);
    lbl_n_rnc->set_text(n_rnc_value);
    while (Gtk::Main::events_pending()) Gtk::Main::iteration();
}

void janela_principal::slot_readied_mmes(const string &n_mme_value) {
    lbl_n_mme->set_text(n_mme_value);
    while (Gtk::Main::events_pending()) Gtk::Main::iteration();
}
string janela_principal::slot_open_connect_string_file() {
    string filename;
    Glib::RefPtr<Gtk::FileFilter> filter_connect ( nullptr );
    Gtk::FileChooserDialog filedialog ( *this, "Selecione o arquivo com dados de conexão:",
                                        Gtk::FILE_CHOOSER_ACTION_OPEN );
    filedialog.add_button ( "_Cancelar", Gtk::RESPONSE_CANCEL );
    filedialog.add_button ( "_Abrir", Gtk::RESPONSE_OK );
    filter_connect = Gtk::FileFilter::create();
    filter_connect->set_name ( "Connection File" );
    filter_connect->add_mime_type ( "text/plain" );
    filedialog.add_filter ( filter_connect );
    if(filedialog.run() == Gtk::RESPONSE_OK) filename = filedialog.get_filename();
    while (Gtk::Main::events_pending()) Gtk::Main::iteration();
    return filename;
}

void janela_principal::log_loader(const string &title, const logparser::logtype &infotype) {
    vector<string> filenames;
    Glib::RefPtr<Gtk::FileFilter> filter_logs ( nullptr );
    Gtk::FileChooserDialog filedialog ( *this, title,
                                        Gtk::FILE_CHOOSER_ACTION_OPEN );
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
            signal_new_file.emit ( *riter, infotype );
        }
    }
}

void janela_principal::slot_db_working_node(std::string node) {
    sts_bar->push(node);
    while (Gtk::Main::events_pending()) Gtk::Main::iteration();
}

void janela_principal::slot_db_cleaner() {
    Gtk::MessageDialog dialogo(*this, "Esta ação removerá todos os dados de topologia re"
                               "ferentes a esta aplicação do banco. Confirma a operação?",
                               false,Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
    int resposta=dialogo.run();
    while (Gtk::Main::events_pending()) Gtk::Main::iteration();
    if (resposta == Gtk::RESPONSE_NO) return;
    runner.clean_database();
}

void janela_principal::slot_change_n_file(const string & n_file,
        const logparser::logtype & nodetype) {
    switch (nodetype) {
    case logparser::logtype::mobswitch:
        lbl_n_23g->set_text(n_file);
        break;
    case logparser::logtype::mme:
        lbl_n_4g->set_text(n_file);
        break;
    case logparser::logtype::ss7:
        lbl_n_ss7arq->set_text(n_file);
        break;
    }
}
