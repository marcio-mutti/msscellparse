#include "janela_principal.hpp"
#include <iostream>

using namespace std;

janela_principal::janela_principal() : janelator(nullptr), box_principal(nullptr), lbl_n_23g(nullptr),   lbl_n_4g(nullptr),      lbl_n_central(nullptr), lbl_n_bsc(nullptr),   lbl_n_rnc(nullptr), lbl_n_mme(nullptr),  lbl_n_central_up(nullptr),  lbl_n_bsc_up(nullptr),
lbl_n_rnc_up(nullptr),  lbl_n_mme_up(nullptr),  btn_load_23g(nullptr),  btn_load_4g(nullptr), btn_carregar(nullptr), 
btn_subir_banco(nullptr) {
  try {
    janelator = Gtk::Builder::create_from_file("/home/marcio/programacao/novo_cell_parse/Janela_Principal.glade");
  } catch (const Glib::FileError& error) {
    cerr <<  "Encontrado um erro na tentativa de abrir o arquivo de definição da UI:" <<  endl;
    cerr <<  error.what() << endl <<   "Error code: " <<  error.code() <<  endl;
    throw;
  }
  //janelator->get_widget("janela_principal", mainwindow);
  janelator->get_widget("box_principal", box_principal);
  this->add(*box_principal);
  janelator->get_widget("lbl_n_23g", lbl_n_23g);
  janelator->get_widget("lbl_n_4g",  lbl_n_4g);
  janelator->get_widget("lbl_n_bsc", lbl_n_bsc);
  janelator->get_widget("lbl_n_bsc_up", lbl_n_bsc_up);
  janelator->get_widget("lbl_n_central", lbl_n_central);
  janelator->get_widget("lbl_n_central_up",  lbl_n_central_up);
  janelator->get_widget("lbl_n_mme", lbl_n_mme);
  janelator->get_widget("lbl_n_mme_up", lbl_n_mme_up);
  janelator->get_widget("lbl_n_rnc", lbl_n_rnc);
  janelator->get_widget("lbl_n_rnc_up", lbl_n_rnc_up);
  janelator->get_widget("btn_load_23g", btn_load_23g);
  janelator->get_widget("btn_load_4g", btn_load_4g);
  janelator->get_widget("btn_carregar", btn_carregar);
  janelator->get_widget("btn_subir_banco", btn_subir_banco);
  btn_load_23g->signal_clicked().connect(sigc::mem_fun(*this, &janela_principal::slot_btn_load_23g));
  btn_load_4g->signal_clicked().connect(sigc::mem_fun(*this, &janela_principal::slot_btn_load_4g));
  btn_carregar->signal_clicked().connect(sigc::mem_fun(*this, &janela_principal::slot_btn_carregar));
  btn_subir_banco->signal_clicked().connect(sigc::mem_fun(*this, &janela_principal::slot_btn_subir_banco));
  //box_principal->show_all_children();
  show_all_children();
}
janela_principal::~janela_principal() {}
void janela_principal::slot_btn_load_23g()
{
}
void janela_principal::slot_btn_load_4g()
{
}
void janela_principal::slot_btn_carregar()
{
}
void janela_principal::slot_btn_subir_banco()
{
}



