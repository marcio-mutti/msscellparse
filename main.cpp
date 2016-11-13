#include "janela_principal.hpp"

int main(int argc, char * argv[]) {
  Gtk::Main kit(argc, argv);
  janela_principal mainwindow;
  kit.run(mainwindow);
  return 0;
}
