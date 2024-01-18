#include "pinscope.hpp"
#include <gtkmm/application.h>

int main(int argc, char **argv) {
  auto app = Gtk::Application::create("dev.reddocmd.pinscope");
  return app->make_window_and_run<pinscope::Pinscope>(argc, argv);
}
