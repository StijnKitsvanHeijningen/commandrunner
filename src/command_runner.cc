#include "command_runner.h"
#include "gtkmm/enums.h"

#include <cstdio>
#include <fontconfig/fontconfig.h>
#include <gtkmm/messagedialog.h>
#include <json/json.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

Command_Runner::Command_Runner()
    : m_buttonRun("Run"),
      m_buttonEdit("Edit Command"),
      m_buttonAdd("Add Command"),
      m_buttonRemove("Remove Command"),
      m_labelName("Name"),
      m_labelDescription("Description!"),
      m_Description(),
      m_commandList(),
      m_labelCommandRunner("Command Runner"),
      m_buttonGetTerminalSize("Get terminal size"),
      m_updatingDialog(*this, "Updating...", false,
                            Gtk::MESSAGE_INFO, Gtk::BUTTONS_NONE){
  auto css = Gtk::CssProvider::create();
  Glib::RefPtr<const Gio::File> style;
  size_t len = sizeof(currentPath);
  int bytes = MIN(readlink("/proc/self/exe", currentPath, len), len - 1);
  if (bytes >= 0) currentPath[bytes - 14] = '\0';
  std::cout << "Current path is: " << currentPath << '\n';
  std::string stylepath = currentPath;
  stylepath.append("../share/styles/style.css");
    style = Gio::File::create_for_path(
        stylepath);
  if (not css->load_from_file(style)) {
    std::cerr << "Failed to load css\n";
    std::exit(1);
  }

  m_updateDone.connect(sigc::mem_fun(*this,&Command_Runner::on_UpdateDone));
  set_position(Gtk::WindowPosition::WIN_POS_CENTER_ALWAYS);
  m_fileActionGroup = Gio::SimpleActionGroup::create();

  m_fileActionGroup->add_action(
      "import", sigc::mem_fun(*this, &Command_Runner::on_file_import));
  m_fileActionGroup->add_action(
      "export", sigc::mem_fun(*this, &Command_Runner::on_file_export));
  m_fileActionGroup->add_action(
      "calibrate", sigc::mem_fun(*this, &Command_Runner::on_file_calibrate));
  m_refToggle = m_fileActionGroup->add_action_bool(
      "check", sigc::mem_fun(*this, &Command_Runner::on_checkUpdatesToggle),
      false);
  m_fileActionGroup->add_action(
      "desktop", sigc::mem_fun(*this, &Command_Runner::on_file_desktop));
  insert_action_group("file", m_fileActionGroup);

  m_helpActionGroup = Gio::SimpleActionGroup::create();

  m_helpActionGroup->add_action(
      "about", sigc::mem_fun(*this, &Command_Runner::on_help_about));

  insert_action_group("help", m_helpActionGroup);

  Glib::RefPtr<Gtk::Builder> m_refBuilder = Gtk::Builder::create();

  const char *ui_info =
      "<interface>"
      "  <menu id='menubar'>"
      "    <submenu>"
      "      <attribute name='label' translatable='yes'>_File</attribute>"
      "      <section>"
      "        <item>"
      "          <attribute name='label' translatable='yes'>_Import</attribute>"
      "          <attribute name='action'>file.import</attribute>"
      "        </item>"
      "        <item>"
      "          <attribute name='label' translatable='yes'>_Export</attribute>"
      "          <attribute name='action'>file.export</attribute>"
      "        </item>"
      "        <item>"
      "          <attribute name='label' translatable='yes'>_Calibrate "
      "Terminal Size</attribute>"
      "          <attribute name='action'>file.calibrate</attribute>"
      "        </item>"
      "        <item>"
      "          <attribute name='label' translatable='yes'>_Check for updates "
      "automatically</attribute>"
      "          <attribute name='action'>file.check</attribute>"
      "        </item>"
      "        <item>"
      "          <attribute name='label' translatable='yes'>_Install desktop file</attribute>"
      "          <attribute name='action'>file.desktop</attribute>"
      "        </item>"
      "      </section>"
      "    </submenu>"
      "    <submenu>"
      "      <attribute name='label' translatable='yes'>_Help</attribute>"
      "      <section>"
      "        <item>"
      "          <attribute name='label' translatable='yes'>_About</attribute>"
      "          <attribute name='action'>help.about</attribute>"
      "        </item>"
      "      </section>"
      "    </submenu>"
      "  </menu>"
      "</interface>";

  m_refBuilder->add_from_string(ui_info);

  Glib::RefPtr<Glib::Object> object = m_refBuilder->get_object("menubar");
  Glib::RefPtr<Gio::Menu> gmenu = Glib::RefPtr<Gio::Menu>::cast_dynamic(object);
  Gtk::MenuBar *pMenuBar = Gtk::make_managed<Gtk::MenuBar>(gmenu);
  m_box0.pack_start(*pMenuBar, Gtk::PACK_SHRINK);

  std::string iconpath = currentPath;
  try{
  iconpath.append("../share/icons/command_runner.png");
  set_icon_from_file(iconpath.c_str());
  win2.set_icon_from_file(iconpath.c_str());
  }catch(Gdk::PixbufError& e){
	  std::cout << "e::what(): " << e.what() << std::endl;
	  std::cout << "e::code(): " << e.code() << std::endl;
  }
  std::cout << __cplusplus << std::endl;
  selectedIndex = 0;
  m_labelName.set_markup("<span weight=\"bold\" size=\"large\">Name</span>");
  m_labelName.set_margin_top(15);
  m_labelName.set_margin_bottom(15);
  m_box5.set_margin_bottom(16);
  m_box5.set_margin_top(16);
  m_labelDescription.set_markup(
      "<span weight=\"bold\" size=\"large\">Description</span>");
  auto context = m_labelDescription.get_pango_context();
  auto font = context->get_font_description();
  // font.set_size(40 * PANGO_SCALE);
  font.set_family("Ubuntu");
  context->set_font_description(font);
  auto screen = Gdk::Screen::get_default();
  m_labelDescription.get_style_context()->add_provider_for_screen(
      screen, css, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  m_labelDescription.set_margin_bottom(15);
  m_labelDescription.set_margin_top(15);
  m_labelCommandRunner.set_markup(
      "<span weight=\"bold\" size=\"large\">Command Runner</span>");
  m_labelCommandRunner.set_margin_start(12);
  m_box3.add(m_box5);
  m_box2.add(m_labelName);
  m_box4.add(m_labelDescription);
  m_box5.add(m_labelCommandRunner);
  m_buttonRun.set_image(m_commandRunnerImage);
  m_buttonRun.set_always_show_image(true);
  m_buttonRun.set_alignment(0.0, 0.5);
  m_box5.set_margin_start(25);
  m_box1.set_homogeneous(true);
  set_resizable(false);

  Gtk::Allocation *alloc = new Gtk::Allocation(0, 300, 100, 100);
  Gtk::Allocation &alloc2 = *alloc;
  m_box1.size_allocate(alloc2);
  try{
  Glib::RefPtr<Gdk::Pixbuf> logo =
      Gdk::Pixbuf::create_from_file(iconpath.c_str(), 25, 25, true);

  m_commandRunnerImage.set(logo);
  m_commandRunnerImage.set_margin_end(5);
  m_commandRunnerImage.set_margin_start(30);
  }catch(Gdk::PixbufError& e){
	  std::cout << "e::what(): " << e.what() << std::endl;
	  std::cout << "e::code(): " << e.code() << std::endl;
  }
  std::string homedirectory = getenv("HOME");
  std::ifstream file(homedirectory + "/.config/command_runner.json");
  
  if (!file.is_open()) {
    std::ofstream outfile(homedirectory + "/.config/command_runner.json");
    outfile.write("{\"Commands\":[]}", 256);
  } else {
    file.close();
  }

  listTargets.push_back(
      Gtk::TargetEntry("GTK_LIST_BOX_ROW", Gtk::TARGET_SAME_APP, 0));

  // Signals
  m_commandList.signal_row_selected().connect(
      sigc::mem_fun(*this, &Command_Runner::on_rowSelected));
  m_buttonAdd.signal_clicked().connect(
      sigc::mem_fun(*this, &Command_Runner::on_addButton_clicked));
  m_buttonRun.signal_clicked().connect(
      sigc::mem_fun(*this, &Command_Runner::on_runButton_clicked));
  m_buttonRemove.signal_clicked().connect(
      sigc::mem_fun(*this, &Command_Runner::on_removeButton_clicked));
  m_buttonEdit.signal_clicked().connect(
      sigc::mem_fun(*this, &Command_Runner::on_editButton_clicked));
  m_buttonGetTerminalSize.signal_clicked().connect(
      sigc::mem_fun(*this, &Command_Runner::on_getTerminalSizeButton_clicked));
  win2.m_addButton.signal_clicked().connect(
      sigc::mem_fun(*this, &Command_Runner::on_addCommandButton_clicked));
  win2.m_editButton.signal_clicked().connect(
      sigc::mem_fun(*this, &Command_Runner::on_editCommandButton_clicked));
  win2.m_scaleSizeColumns.signal_change_value().connect(
      sigc::mem_fun(*this, &Command_Runner::on_ColumnValueChanged));
  win2.m_scaleSizeColumns.signal_button_press_event().connect(
      sigc::mem_fun(*this, &Command_Runner::on_ColumnScalePressed));
  win2.m_scaleSizeColumns.signal_button_release_event().connect(
      sigc::mem_fun(*this, &Command_Runner::on_ColumnScaleReleased));
  win2.m_scaleSizeRows.signal_button_release_event().connect(
      sigc::mem_fun(*this, &Command_Runner::on_ColumnScaleReleased));
  win2.m_scaleSizeRows.signal_change_value().connect(
      sigc::mem_fun(*this, &Command_Runner::on_RowValueChanged));

  win2.m_labelEventBox.signal_button_press_event().connect(
      sigc::mem_fun(*this, &Command_Runner::on_ReturnToDefault_clicked));
  win2.m_labelEventBox.signal_enter_notify_event().connect(
      sigc::mem_fun(*this, &Command_Runner::on_ReturnToDefault_entered));
  win2.m_labelEventBox.signal_leave_notify_event().connect(
      sigc::mem_fun(*this, &Command_Runner::on_ReturnToDefault_left));

  // layout
  m_scrolledWindow.set_size_request(175, 500);
  m_scrolledWindow.add(m_commandList);
  m_box1.set_orientation(Gtk::ORIENTATION_HORIZONTAL);
  set_default_size(600, 600);
  set_title("Command Runner");
  m_box2.set_orientation(Gtk::ORIENTATION_VERTICAL);
  m_box2.add(m_scrolledWindow);
  m_box3.set_orientation(Gtk::ORIENTATION_VERTICAL);
  m_box4.set_orientation(Gtk::ORIENTATION_VERTICAL);

  m_commandList.set_size_request(175, -1);
  m_Description.set_vexpand(true);
  m_Description.set_hexpand(false);
  m_Description.set_margin_bottom(22);
  m_Description.set_margin_end(25);
  m_Description.size_allocate(Gtk::Allocation(0, 0, 175, 400));
  m_Description.set_editable(false);
  m_Description.set_can_focus(false);
  m_Description.set_left_margin(3);
  m_Description.set_border_width(1);
  m_scrolledWindow.set_margin_start(25);
  m_scrolledWindow.set_border_width(1);
  m_scrolledWindow.set_policy(Gtk::POLICY_EXTERNAL, Gtk::POLICY_AUTOMATIC);
  m_scrolledWindow.get_style_context()->add_class("commandsScrollWindow");
  m_buttonGetTerminalSize.set_size_request(150, -1);
  m_buttonGetTerminalSize.set_margin_start(25);
  m_buttonGetTerminalSize.set_margin_bottom(10);
  m_buttonGetTerminalSize.set_margin_end(25);
  m_buttonRun.set_size_request(150, -1);
  m_buttonRun.set_margin_start(25);
  m_buttonRun.set_margin_bottom(10);
  m_buttonAdd.set_margin_start(25);
  m_buttonAdd.set_size_request(150, -1);
  m_buttonAdd.set_margin_bottom(10);
  m_buttonEdit.set_margin_start(25);
  m_buttonEdit.set_margin_bottom(10);
  m_buttonEdit.set_size_request(150, -1);
  m_buttonRemove.set_margin_start(25);
  m_buttonRemove.set_margin_bottom(10);
  m_labelError.set_margin_start(25);
  m_labelError.set_margin_end(25);
  m_buttonRemove.set_size_request(150, -1);
  m_buttonRun.set_margin_end(25);
  m_buttonAdd.set_margin_end(25);
  m_buttonEdit.set_margin_end(25);
  m_buttonRemove.set_margin_end(25);
  m_labelError.set_margin_top(15);
  m_labelError.set_xalign(0);

  m_box4.add(m_Description);
  m_box3.add(m_buttonRun);
  m_box3.add(m_buttonAdd);
  m_box3.add(m_buttonEdit);
  m_box3.add(m_buttonRemove);
  m_box3.add(m_labelError);
  m_box4.set_orientation(Gtk::ORIENTATION_VERTICAL);
  m_box1.add(m_box2);
  m_box1.add(m_box3);
  m_box1.add(m_box4);

  add(m_box0);
  m_box0.set_orientation(Gtk::ORIENTATION_VERTICAL);
  m_box0.add(m_box1);
  m_commandList.add(m_test);
  m_Description.set_wrap_mode(Gtk::WRAP_WORD_CHAR);

  m_Description.set_vexpand(true);
  m_previewWindow.resize(500, 500);
  m_previewWindow.set_title("Preview");
  m_previewWindow.set_can_focus(false);
  m_previewWindow.set_keep_above(true);
  m_previewWindow.set_accept_focus(false);
  m_previewWindow.set_position(Gtk::WindowPosition::WIN_POS_CENTER_ALWAYS);
  refresh();

  if (jsonFile["DesktopIntegration"].empty()){
    jsonFile["DesktopIntegration"] = true;
  }
  auto actiongroup = get_action_group("file");
  std::ifstream desktopfile(homedirectory + "/.local/share/applications/command_runner.desktop");
  
  if (desktopfile.is_open()) m_fileActionGroup->remove_action("desktop");
  if (!desktopfile.is_open() && jsonFile["DesktopIntegration"].asBool() == true) {
    Gtk::MessageDialog dialog = Gtk::MessageDialog("Would you like to add Command Runner to your start menu?",false,Gtk::MESSAGE_QUESTION,Gtk::BUTTONS_YES_NO,false);
    dialog.set_secondary_text("A desktop file will be created in " + homedirectory + "/.local/share/applications\nBe aware that if you move the AppImage to another location, you will have to delete the desktop file and recreate it.");
    dialog.set_title("Desktop Integration");
    int result = dialog.run();
  switch (result) {
    case (Gtk::RESPONSE_YES): {
      std::cout << "Generating desktop file and installing it" << std::endl;
      installDesktopFile();
      break;
    }
    case (Gtk::RESPONSE_NO): {
      std::cout << "No desktopfile installed" << std::endl;
      jsonFile["DesktopIntegration"] = false;
      break;
    }
    default: {
      std::cout << "Unexpected button clicked." << std::endl;
      break;
    }
  }
  }
  updateJsonFile();

  if (jsonFile["UpdateCheck"].empty()) {
    jsonFile["UpdateCheck"] = false;
} else
  {
    m_refToggle->change_state(jsonFile["UpdateCheck"].asBool());
  }
  if (jsonFile["UpdateCheck"].asBool() == true){
      updateCheck();
  }
  show_all();

  if (jsonFile["Terminal"]["RowSize"].empty() ||
      jsonFile["Terminal"]["RowSize"].empty()) {
    on_getTerminalSizeButton_clicked();
  }

  rowSize = jsonFile["Terminal"]["RowSize"].asDouble();
  columnSize = jsonFile["Terminal"]["ColumnSize"].asDouble();

  win2.m_scaleSizeColumns.set_range(
      25, this->get_screen()->get_root_window()->get_width() / columnSize - 5);
  win2.m_scaleSizeRows.set_range(
      3, this->get_screen()->get_root_window()->get_height() / rowSize - 5);
}

Command_Runner::~Command_Runner() {}

bool Command_Runner::on_ReturnToDefault_entered(GdkEventCrossing *event) {
  win2.m_labelReturnToDefault.set_markup(
      "<span foreground=\"#285ff7\" underline=\"single\" size=\"small\">Return "
      "to default</span>");
  return true;
}
bool Command_Runner::on_ReturnToDefault_left(GdkEventCrossing *event) {
  win2.m_labelReturnToDefault.set_markup(
      "<span underline=\"single\" size=\"small\">Return to default</span>");
  return true;
}

bool Command_Runner::on_ReturnToDefault_clicked(GdkEventButton *button) {
  win2.m_scaleSizeColumns.set_value(80);
  win2.m_scaleSizeRows.set_value(25);
  return true;
}
void Command_Runner::on_checkUpdatesToggle() {
  bool active = false;
  m_refToggle->get_state(active);
  active = !active;
  m_refToggle->change_state(active);
  jsonFile["UpdateCheck"] = active;
  updateJsonFile();
}

void Command_Runner::on_file_desktop(){
  installDesktopFile();
}

void Command_Runner::updateJsonFile(){
  std::string homedirectory = getenv("HOME");
  std::string directory = homedirectory + "/.config/command_runner.json";
  std::ofstream file_for_out(directory);
  file_for_out << jsonFile;
  file_for_out.close();
}

void Command_Runner::installDesktopFile(){
  std::string currentPathString = currentPath;
  std::string homedirectory = getenv("HOME");
  //install icon file
  std::filesystem::create_directory(homedirectory + "/.local/share/icons");
  std::ifstream  src(currentPathString + "../share/icons/command_runner.png", std::ios::binary);
    std::ofstream  dst(homedirectory + "/.local/share/icons/command_runner.png",   std::ios::binary);
    dst << src.rdbuf();
    dst.close();
  std::string desktopfileloc = homedirectory + "/.local/share/applications/command_runner.desktop"; 
  std::ofstream desktopfile(desktopfileloc);
  const char* appimageabspath = std::getenv("APPIMAGE") ? std::getenv("APPIMAGE") : "xd";
  desktopfile << "#!/usr/bin/env xdg-open\n";
  desktopfile <<  "[Desktop Entry]\n";
  desktopfile <<  "Icon=command_runner\n";
  desktopfile <<  "Exec=" << (std::getenv("APPIMAGE") ? std::getenv("APPIMAGE") : currentPathString.append("Command_Runner").c_str()) << "\n";
  desktopfile << "Type=Application\n";
  desktopfile <<  "Version=" << (std::getenv("VERSION") ? std::getenv("VERSION") : "1.0.0") << "\n";
  desktopfile << "Categories=Utility\n";
  desktopfile << "Name=Command Runner\n";
  desktopfile.close();
  std::cout << "Installing Desktop file." << std::endl;
  m_fileActionGroup->remove_action("desktop");
}
void Command_Runner::updateCheck(){

  int updateAvailable;

  std::string updatetoolcommand = currentPath;
  updatetoolcommand.append("appimageupdatetool");
  std::string appimageloc = std::getenv("APPIMAGE") ? std::getenv("APPIMAGE") : "";
  updateAvailable = WEXITSTATUS(system((updatetoolcommand + " -j " + appimageloc).c_str()));
  if (updateAvailable == 0) {
	  std::cout << "no update is available" << std::endl;
	  return;
  }else if (updateAvailable == 1){
     std::cout << "Update Available" << std::endl;
     Gtk::MessageDialog dialog(*this, "There is an update available.", false,
                            Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
    dialog.set_title("Update Available");
    dialog.set_secondary_text(
      "There is an update available, would you like to update?");

  int result = dialog.run();
  switch (result) {
    case (Gtk::RESPONSE_YES): {
      std::cout << "Performing update..." << std::endl;

      break;
    }
    case (Gtk::RESPONSE_NO): {
      std::cout << "No update performed" << std::endl;
      return;
    }
    default: {
      std::cout << "Unexpected button clicked." << std::endl;
      return;
    }
  }
  dialog.close();
  }else{
	  std::cout << "no update information found" << std::endl;
	  return;
  }
     
    m_updatingDialog.set_secondary_text(
      "Updating existing appimage...");
    m_updateThread = Glib::Thread::create(sigc::mem_fun(*this,&Command_Runner::performUpdate),true);

  int result = m_updatingDialog.run();
  pid_t c_pid = fork();

  if (c_pid == -1){
    perror("Error while forking");
    exit(EXIT_FAILURE);
  } 
  else if (c_pid > 0){
    exit(EXIT_SUCCESS);
  }else{
    std::string appimageloc = std::getenv("APPIMAGE") ? std::getenv("APPIMAGE") : "";
    std::cout << "running appimage: " << appimageloc.c_str() << std::endl;
    execl(appimageloc.c_str(),(char*)NULL);
  }

  

	 

}
void Command_Runner::performUpdate(){

  std::string updatetoolcommand = currentPath;
  updatetoolcommand.append("appimageupdatetool");
  int updateResult = WEXITSTATUS(system((updatetoolcommand + " -O " + " --self-update -r").c_str()));
  std::cout << "updateResult: " << updateResult << std::endl;
  m_updateDone.emit();
}
void Command_Runner::on_UpdateDone(){
   std::cout << "Update was succesful!" << std::endl;
   m_updatingDialog.close();
   m_updateThread->join();
   m_updateThread = NULL;
}
void Command_Runner::on_getTerminalSizeButton_clicked() {
  signal(SIGCHLD, SIG_IGN);
  pid_t c_pid = fork();

  if (c_pid == 0) {
    system(
        "gnome-terminal --geometry=100x25 -e 'bash -c \" echo getting "
        "terminal size... ; sleep 1\"'");
    exit(0);
  } else if (c_pid > 0) {
    std::cout << "printed from parent process" << std::endl;
  } else {
    perror("fork");
    exit(EXIT_FAILURE);
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  auto root = this->get_screen()->get_active_window();
  std::cout << root->get_type_hint() << std::endl;
  std::cout << root->get_base_type() << std::endl;
  rowSize = root->get_height() / 25.00;
  columnSize = root->get_width() / 100.00;

  jsonFile["Terminal"]["RowSize"] = rowSize;
  jsonFile["Terminal"]["ColumnSize"] = columnSize;

  std::string homedirectory = getenv("HOME");
  std::string directory = homedirectory + "/.config/command_runner.json";
  std::ofstream file_for_out(directory);
  file_for_out << jsonFile;
  file_for_out.close();
}

bool Command_Runner::on_ColumnScaleReleased(GdkEventButton *event) {
  m_previewWindow.hide();
  return true;
}

bool Command_Runner::on_ColumnScalePressed(GdkEventButton *event) {
  return true;
}

bool Command_Runner::on_ColumnValueChanged(Gtk::ScrollType scrollType,
                                           gdouble newValue) {
  if (!m_previewWindow.get_visible()) {
    m_previewWindow.show();
  }

  std::cout << win2.m_scaleSizeColumns.get_value() * columnSize << std::endl;

  m_previewWindow.resize(
      std::round(win2.m_scaleSizeColumns.get_value() * columnSize),
      std::round(win2.m_scaleSizeRows.get_value() * rowSize));
  m_previewWindow.set_opacity(0.5);
  return true;
}

bool Command_Runner::on_RowValueChanged(Gtk::ScrollType scrollType,
                                        gdouble newValue) {
  if (!m_previewWindow.get_visible()) {
    m_previewWindow.show();
  }

  m_previewWindow.resize(win2.m_scaleSizeColumns.get_value() * columnSize,
                         win2.m_scaleSizeRows.get_value() * rowSize);
  m_previewWindow.set_opacity(0.5);
  return true;
}

void Command_Runner::on_file_calibrate() {
  Gtk::MessageDialog dialog(*this, "Calibrating the terminal size", false,
                            Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_NONE);
  dialog.set_secondary_text(
      "Calibrating the terminal size will open up your terminal with a certain "
      "amount of rows and columns and extract the size from the window."
      "\n This is necessary for ensuring that the preview you get when u set "
      "the terminal size is accurate.\n"
      "\nYou should do this every time you change the font size of your "
      "terminal or anything else that affects the size of your terminal "
      "rows/columns.");
  dialog.add_button("_Calibrate", Gtk::RESPONSE_OK);
  dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
  int result = dialog.run();

  switch (result) {
    case (Gtk::RESPONSE_OK): {
      on_getTerminalSizeButton_clicked();
      break;
    }
    case (Gtk::RESPONSE_CANCEL): {
      std::cout << "Cancel clicked." << std::endl;
      break;
    }
    default: {
      std::cout << "Unexpected button clicked." << std::endl;
      break;
    }
  }
}

void Command_Runner::on_help_about() {
  std::string iconpath = currentPath;
  iconpath.append("../share/icons/command_runner.png");
  Glib::RefPtr<Gdk::Pixbuf> logo =
      Gdk::Pixbuf::create_from_file(iconpath.c_str(), 25, 25, true);
  m_AboutDialog.set_logo(logo);
  m_AboutDialog.set_icon_from_file(iconpath.c_str());
  m_AboutDialog.set_program_name("Command Runner");
  m_AboutDialog.set_copyright("Stijn Kits van Heijningen");
  m_AboutDialog.set_version("1.0.0");
  m_AboutDialog.set_authors({"Stijn Kits van Heijningen"});
  std::string licensepath = currentPath;
  std::ifstream licensefile(licensepath + "../../LICENSE.md");
  std::string line;
  std::string license;
  if (licensefile.is_open()){
    while (licensefile.good()){
  std::getline(licensefile,line);
    license.append(line + "\n");
    }
  }
  licensefile.close();
  m_AboutDialog.set_license(license);
  int result = m_AboutDialog.run();
  if (result == -4) {
    m_AboutDialog.hide();
  }
}

void Command_Runner::on_file_export() {
  Gtk::FileChooserDialog dialog("Please choose a folder to export to",
                                Gtk::FILE_CHOOSER_ACTION_SAVE);
  dialog.set_current_name("command_runner_backup.json");
  dialog.set_do_overwrite_confirmation(true);
  dialog.set_transient_for(*this);
  dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
  dialog.add_button("Export", Gtk::RESPONSE_OK);

  int result = dialog.run();

  switch (result) {
    case (Gtk::RESPONSE_OK): {
      std::cout << "Select clicked." << std::endl;
      std::cout << "Folder selected: " << dialog.get_filename();
      std::ofstream file_for_out(dialog.get_filename());
      file_for_out << jsonFile;
      file_for_out.close();
      break;
    }
    case (Gtk::RESPONSE_CANCEL): {
      std::cout << "Cancel clicked." << std::endl;
      return;
    }
    default: {
      std::cout << "Unexpected button clicked." << std::endl;
      return;
    }
  }
}

void Command_Runner::on_file_import() {
  bool wrongfilechosen = false;

  Gtk::FileChooserDialog dialog("Please choose a file to import",
                                Gtk::FILE_CHOOSER_ACTION_OPEN);
  dialog.set_transient_for(*this);
  dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
  dialog.add_button("Import", Gtk::RESPONSE_OK);
  int result = dialog.run();

  switch (result) {
    case (Gtk::RESPONSE_OK): {
      std::cout << "Select clicked." << std::endl;
      std::cout << "Folder selected: " << dialog.get_filename() << std::endl;
      std::filesystem::path p(dialog.get_filename());
      std::cout << "extension: " << p.extension() << std::endl;
      if (p.extension() != ".json") {
        Gtk::MessageDialog mdialog("Please choose a .json file", false,
                                   Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, false);
        mdialog.run();
      } else {
        std::string homedirectory = getenv("HOME");
        std::string directory = dialog.get_filename();
        std::ifstream infile(directory, std::ifstream::binary);
        Json::Value tempJsonFile;
        try {
          infile >> tempJsonFile;
          std::cout << "foutlocator" << std::endl;
          if (tempJsonFile["Commands"][0] == Json::nullValue) {
            Gtk::MessageDialog mdialog(
                "The chosen file is empty or has incorrect structure", false,
                Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, false);
            mdialog.run();
          } else {
            std::string directory =
                homedirectory + "/.config/command_runner.json";
            std::ofstream file_for_out(directory);
            file_for_out << tempJsonFile;
            file_for_out.close();
          }
        } catch (const std::exception &e) {
          Gtk::MessageDialog mdialog(
              "The chosen file is empty or has incorrect structure", false,
              Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, false);
          mdialog.run();
        }
      }
      break;
    }
    case (Gtk::RESPONSE_CANCEL): {
      std::cout << "Cancel clicked." << std::endl;
      return;
    }
    default: {
      std::cout << "Unexpected button clicked." << std::endl;
      return;
    }
  }

  std::cout << "import file: " << dialog.get_filename() << std::endl;
  refresh();
}

void Command_Runner::on_addButton_clicked() {
  if (win2.box3.get_children().size() > 4) {
    win2.box3.remove(
        *win2.box3.get_children().at(win2.box3.get_children().size() - 1));
  }
  win2.m_entryName.set_text("");
  win2.m_entryDescription.get_buffer().get()->set_text("");
  win2.m_entryCommand.get_buffer().get()->set_text("");
  win2.m_checkRunInTerminal.set_active(true);
  win2.m_scaleSizeColumns.set_value(80);
  win2.m_scaleSizeRows.set_value(25);
  win2.m_labelError.set_label("");
  win2.box3.add(win2.m_addButton);
  win2.set_title("Add Command");
  win2.set_default_size(500, 500);
  win2.set_size_request(500, 500);
  win2.set_resizable(false);

  win2.show_all();
  win2.selectedIndex = selectedIndex;
  std::cout << win2.m_labelName.get_label() << std::endl;
  std::cout << "Add Button" << std::endl;
}

void Command_Runner::on_editButton_clicked() {
  std::cout << "selected index: " << selectedIndex << std::endl;
  if (m_commandList.get_children().size() == 0 ||
      m_commandList.get_selected_row() == NULL) {
    m_labelError.set_label("Select command first");
    return;
  }
  if (win2.box3.get_children().size() > 4) {
    win2.box3.remove(
        *win2.box3.get_children().at(win2.box3.get_children().size() - 1));
  }
  win2.box3.add(win2.m_editButton);
  win2.set_title("Edit Command");
  win2.set_default_size(500, 500);
  win2.set_size_request(500, 500);
  win2.set_resizable(false);
  win2.show_all();
  win2.m_entryName.set_text(
      jsonFile["Commands"][selectedIndex]["name"].asString());
  win2.m_entryDescription.get_buffer().get()->set_text(
      jsonFile["Commands"][selectedIndex]["description"].asString());
  win2.m_entryDescription.set_border_width(1);
  win2.m_entryCommand.get_buffer().get()->set_text(
      jsonFile["Commands"][selectedIndex]["command"].asString());
  win2.m_entryCommand.set_border_width(1);
  win2.m_checkRunInTerminal.set_active(
      jsonFile["Commands"][selectedIndex]["runinterminal"].asBool());
  win2.m_scaleSizeColumns.set_value(
      jsonFile["Commands"][selectedIndex]["terminalColumns"].asInt64());
  win2.m_scaleSizeRows.set_value(
      jsonFile["Commands"][selectedIndex]["terminalRows"].asInt64());
  win2.m_labelError.set_label("");
  win2.box3.show_all();
  win2.set_size_request(500, 500);
  std::cout << "Edit Button" << std::endl;
}

void Command_Runner::on_runButton_clicked() {
  std::cout << "Terminal rowSize: " << rowSize << std::endl;
  std::cout << "Terminal columnSize: " << columnSize << std::endl;
  std::cout << "selected index1: " << selectedIndex << std::endl;
  if (m_commandList.get_children().size() == 0 ||
      m_commandList.get_selected_row() == NULL) {
    m_labelError.set_label("Select command first");
    return;
  }
  std::string fullcommand = "";
  std::cout << "Run Button" << std::endl;
  int terminalColumns =
      jsonFile["Commands"][selectedIndex]["terminalColumns"].asInt64();
  int terminalRows =
      jsonFile["Commands"][selectedIndex]["terminalRows"].asInt64();
  if (jsonFile["Commands"][selectedIndex]["runinterminal"].asBool() == false) {
    fullcommand = jsonFile["Commands"][selectedIndex]["command"].asString();
  } else {
    fullcommand.append("gnome-terminal --geometry=");
    fullcommand.append(std::to_string(terminalColumns));
    fullcommand.append("x");
    fullcommand.append(std::to_string(terminalRows));
    fullcommand.append("+");
    std::cout << "Primary monitor: "
              << this->get_screen()->get_primary_monitor() << std::endl;
    Gdk::Rectangle workarea;
    this->get_display()
        ->get_monitor_at_window(this->get_screen()->get_active_window())
        ->get_geometry(workarea);
    std::cout << "Chosen monitor resolution: " << workarea.get_width() << "x"
              << workarea.get_height() << std::endl;
    int xcord = workarea.get_width() / 2 - (terminalColumns / 2) * columnSize +
                workarea.get_x();
    int ycord = workarea.get_height() / 2 - (terminalRows / 2) * rowSize +
                workarea.get_y();
    fullcommand.append(std::to_string(xcord));
    fullcommand.append("+");
    fullcommand.append(std::to_string(ycord));
    fullcommand.append(
        " -e 'bash -c \"" +
        jsonFile["Commands"][selectedIndex]["command"].asString() +
        "; exec bash\"'");
  }
  const char *command = fullcommand.c_str();
  std::cout << command << std::endl;
  int pid = fork();
  if (pid == 0) {
    if (system(NULL))
      puts("Ok");
    else
      std::cout << "No command processor available" << std::endl;
    int i = system(command);
    std::cout << "result of command is: " << i << std::endl;
    exit(1);
  } else if (pid > 0) {
    std::cout << "parent process" << std::endl;
  } else {
    std::cout << "error" << std::endl;
  }
}

void Command_Runner::on_removeButton_clicked() {
  std::cout << "selected index1: " << selectedIndex << std::endl;
  if (m_commandList.get_children().size() == 0) {
    std::cout << "Test" << std::endl;
    m_labelError.set_label("Add command first");
    return;
  }
  std::cout << "Test" << std::endl;
  Gtk::MessageDialog dialog(*this, "Are you sure?", false,
                            Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
  dialog.set_secondary_text(
      "Are you sure you want to delete command " +
      jsonFile["Commands"][selectedIndex]["name"].asString() + "?");

  int result = dialog.run();
  switch (result) {
    case (Gtk::RESPONSE_YES): {
      std::cout << "Remove completed" << std::endl;
      break;
    }
    case (Gtk::RESPONSE_NO): {
      std::cout << "Remove canceled." << std::endl;
      return;
    }
    default: {
      std::cout << "Unexpected button clicked." << std::endl;
      return;
    }
  }
  std::string homedirectory = getenv("HOME");
  std::string directory = homedirectory + "/.config/command_runner.json";

  jsonFile["Commands"].removeIndex(selectedIndex,
                                   &jsonFile["Commands"][selectedIndex]);
  std::ofstream file_for_out(directory);
  file_for_out << jsonFile;
  file_for_out.close();
  refresh();
}

void Command_Runner::on_rowSelected(Gtk::ListBoxRow *row) {
  Gtk::ListBoxRow &currentRow = *row;
  if (&currentRow != nullptr) {
    selectedIndex = currentRow.get_index();
  }
  std::cout << "Row Selected: " << selectedIndex << std::endl;
  std::cout << selectedIndex << std::endl;
  if (m_commandList.get_selected_row() != NULL) {
    m_Description.get_buffer().get()->set_text(
        jsonFile["Commands"][selectedIndex]["description"].asString());
  } else {
    m_Description.get_buffer().get()->set_text("");
  }
  std::cout << "root children size in on_rowSelected: "
            << this->get_screen()->get_window_stack().size() << std::endl;
}

void Command_Runner::on_drag_begin(
    const Glib::RefPtr<Gdk::DragContext> &context, Gtk::EventBox *ptrBox) {
  Gtk::ListBoxRow *row = dynamic_cast<Gtk::ListBoxRow *>(
      ptrBox->get_ancestor(Gtk::ListBoxRow::get_type()));

  std::cout << "children size:"
            << this->get_screen()
                   .get()
                   ->get_active_window()
                   .get()
                   ->get_children()
                   .at(3)
                   .get()
                   ->get_children()
                   .size()
            << std::endl;
  auto root = this->get_screen()
                  .get()
                  ->get_active_window()
                  .get()
                  ->get_children()
                  .at(5)
                  .get()
                  ->get_children()
                  .at(0)
                  .get()
                  ->get_children()
                  .at(0);
  int height = row->get_allocated_height();
  int width = row->get_allocated_width();
  std::cout << "drag begin" << std::endl;
  auto pixels =
      Gdk::Pixbuf::create(root, row->get_allocation().get_x(),
                          row->get_allocation().get_y(), width, height);
  m_drag_icon = pixels;
  (*ptrBox).drag_source_set_icon(m_drag_icon);
}

void Command_Runner::on_label_drag_data_get(
    const Glib::RefPtr<Gdk::DragContext> &context,
    Gtk::SelectionData &selection_data, guint info, guint time,
    Gtk::EventBox *ptrBox) {
  // Gtk::Widget* widgetRow = box->get_parent();
  // Gtk::ListBoxRow* pRow = dynamic_cast<Gtk::ListBoxRow*>(widgetRow);
  // Gtk::ListBoxRow& row = *pRow;
  // std::cout << "source window: " << row.get_index() << std::endl;
  selection_data.set(selection_data.get_target(), 32, (const guchar *)&ptrBox,
                     12);
}

void Command_Runner::on_label_drop_drag_data_received(
    const Glib::RefPtr<Gdk::DragContext> &context, int x, int y,
    const Gtk::SelectionData &selection_data, guint info, guint time,
    Gtk::ListBoxRow *row) {
  int pos = row->get_index();
  Gtk::ListBoxRow *target;
  target = row;
  Gtk::EventBox *pBox;
  auto data = (gpointer) * (gpointer *)selection_data.get_data();
  pBox = (Gtk::EventBox *)data;
  Gtk::EventBox &box = *pBox;
  Gtk::ListBoxRow *source = dynamic_cast<Gtk::ListBoxRow *>(box.get_parent());
  std::cout << "source row: " << source->get_index() << std::endl;
  std::cout << "destination row:" << target->get_index() << std::endl;

  Json::Value tobeRemoved = jsonFile["Commands"][source->get_index()];
  jsonFile["Commands"].removeIndex(source->get_index(),
                                   &jsonFile["Commands"][source->get_index()]);
  jsonFile["Commands"].insert(pos, tobeRemoved);

  m_commandList.remove(*source);
  m_commandList.insert(*source, pos);
  context->drag_finish(true, false, false);

  std::string homedirectory = getenv("HOME");
  std::string directory = homedirectory + "/.config/command_runner.json";

  std::ofstream file_for_out(directory);
  file_for_out << jsonFile;
  file_for_out.close();

  refresh();
}

void Command_Runner::refresh() {
  if (m_commandList.get_children().size() != 0) {
    for (Gtk::Widget *child : m_commandList.get_children()) {
      child->~Widget();
    }
  }
  std::string homedirectory = getenv("HOME");
  std::string directory = homedirectory + "/.config/command_runner.json";
  std::ifstream infile(directory, std::ifstream::binary);
  infile >> jsonFile;

  for (Json::Value command : jsonFile["Commands"]) {
    Gtk::Label *pLabel =
        Gtk::manage(new Gtk::Label(command["name"].asString()));
    Gtk::Label &tempLabel = *pLabel;
    Gtk::EventBox *pBox = new Gtk::EventBox();
    Gtk::EventBox &box = *pBox;
    box.add(tempLabel);
    Gtk::ListBoxRow *pRow = new Gtk::ListBoxRow();
    Gtk::ListBoxRow &row = *pRow;
    row.add(box);
    m_commandList.insert(row, -1);
    box.drag_source_set(listTargets, Gdk::BUTTON3_MASK, Gdk::ACTION_MOVE);
    box.signal_drag_data_get().connect(sigc::bind(
        sigc::mem_fun(*this, &Command_Runner::on_label_drag_data_get), &box));
    box.signal_drag_begin().connect(
        sigc::bind(sigc::mem_fun(*this, &Command_Runner::on_drag_begin), &box));
    row.drag_dest_set(listTargets, Gtk::DEST_DEFAULT_ALL, Gdk::ACTION_MOVE);
    row.signal_drag_data_received().connect(sigc::bind(
        sigc::mem_fun(*this, &Command_Runner::on_label_drop_drag_data_received),
        &row));
    tempLabel.set_xalign(0);
  }
  m_commandList.show_all();
}

void Command_Runner::on_editCommandButton_clicked() {
  std::string homedirectory = getenv("HOME");
  std::string directory = homedirectory + "/.config/command_runner.json";
  std::string name = win2.m_entryName.get_text();
  std::string description =
      win2.m_entryDescription.get_buffer().get()->get_text();
  std::string command = win2.m_entryCommand.get_buffer().get()->get_text();
  bool runInTerminal = win2.m_checkRunInTerminal.get_active();

  double terminalRows = win2.m_scaleSizeRows.get_value();
  double terminalColumns = win2.m_scaleSizeColumns.get_value();

  if (name == "" || description == "" || command == "") {
    win2.m_labelError.set_label("Please fill out all fields");
    return;
  }
  jsonFile["Commands"][selectedIndex]["name"] = name;
  jsonFile["Commands"][selectedIndex]["description"] = description;
  jsonFile["Commands"][selectedIndex]["command"] = command;
  jsonFile["Commands"][selectedIndex]["runinterminal"] = runInTerminal;
  jsonFile["Commands"][selectedIndex]["terminalRows"] = terminalRows;
  jsonFile["Commands"][selectedIndex]["terminalColumns"] = terminalColumns;

  std::ofstream file_for_out(directory);
  file_for_out << jsonFile;
  file_for_out.close();
  win2.m_entryName.set_text("");
  win2.m_entryDescription.get_buffer().get()->set_text("");
  win2.m_entryCommand.get_buffer().get()->set_text("");
  win2.hide();
  refresh();
  m_commandList.select_row(*m_commandList.get_row_at_index(selectedIndex));
}
void Command_Runner::on_addCommandButton_clicked() {
  std::string homedirectory = getenv("HOME");
  std::string directory = homedirectory + "/.config/command_runner.json";

  std::string name = win2.m_entryName.get_text();
  std::string description =
      win2.m_entryDescription.get_buffer().get()->get_text();
  std::string command = win2.m_entryCommand.get_buffer().get()->get_text();
  bool runInTerminal = win2.m_checkRunInTerminal.get_active();
  int terminalRows = win2.m_scaleSizeRows.get_value();
  int terminalColumns = win2.m_scaleSizeColumns.get_value();

  if (name == "" || description == "" || command == "") {
    win2.m_labelError.set_label("Please fill out all fields");
    return;
  }
  Json::Value jsonCommand;
  jsonCommand["name"] = name;
  jsonCommand["description"] = description;
  jsonCommand["command"] = command;
  jsonCommand["runinterminal"] = runInTerminal;
  jsonCommand["terminalRows"] = terminalRows;
  jsonCommand["terminalColumns"] = terminalColumns;

  if (jsonFile["Commands"].size() == 0 ||
      m_commandList.get_selected_row() == NULL) {
    jsonFile["Commands"].append(jsonCommand);
  } else {
    jsonFile["Commands"].insert(selectedIndex + 1, jsonCommand);
  }

  std::ofstream file_for_out(directory);
  file_for_out << jsonFile;
  file_for_out.close();
  win2.m_entryName.set_text("");
  win2.m_entryDescription.get_buffer().get()->set_text("");
  win2.m_entryCommand.get_buffer().get()->set_text("");
  win2.hide();
  refresh();
  std::cout << "refreshed" << std::endl;
  if (m_commandList.get_children().size() > 1 &&
      m_commandList.get_children().size() < selectedIndex - 1) {
    m_commandList.select_row(
        *m_commandList.get_row_at_index(selectedIndex + 1));
  } else {
    m_commandList.select_row(*m_commandList.get_row_at_index(selectedIndex));
  }
}
