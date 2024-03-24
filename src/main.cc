#include "command_runner.h"
#include <gtkmm/application.h>
#include <fontconfig/fontconfig.h>
#include <iostream>

int main (int argc, char *argv[])
{
  

  unsigned char filepathstring2[] = "./fonts/BeaufortForLoL-OTF/BeaufortforLOL-Regular.otf";
  const FcChar8 *filepath2 = &filepathstring2[0];
  FcBool fontAddStatus2 = FcConfigAppFontAddFile(NULL,filepath2);
  if (fontAddStatus2 == FcFalse){
    std::cout << "font could not be added" << std::endl;
  }


  unsigned char filepathstring[] = "./fonts/Ubuntu-Regular.ttf";
  const FcChar8 *filepath = &filepathstring[0];
  FcBool fontAddStatus = FcConfigAppFontAddFile(NULL, filepath);
  if (fontAddStatus == FcFalse){
    std::cout << "font could not be added" << std::endl;
  }


  auto app = Gtk::Application::create(argc, argv, "org.gtkmm.commandrunner");

  app->set_accel_for_action("example.new","<Primary>n");

  Command_Runner commandrunner;

  //Shows the window and returns when it is closed.


  return app->run(commandrunner);
  
}
