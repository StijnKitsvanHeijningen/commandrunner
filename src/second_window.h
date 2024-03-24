#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/entry.h>
#include <gtkmm/eventbox.h>
#include <gtkmm/label.h>
#include <gtkmm/overlay.h>
#include <gtkmm/scale.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/textview.h>
#include <gtkmm/window.h>

class Second_Window : public Gtk::Window {
 public:
  Second_Window();
  virtual ~Second_Window();

  int selectedIndex;
  Gtk::ScrolledWindow m_descriptionScrolledWindow, m_commandScrolledWindow;
  Gtk::EventBox m_labelEventBox;
  Gtk::Label m_labelName, m_labelDescription, m_labelCommand, m_labelError,
      m_labelTerminalRows, m_labelTerminalColumns, m_labelReturnToDefault;
  Gtk::Entry m_entryName;
  Gtk::TextView m_entryDescription, m_entryCommand;
  Gtk::Button m_addButton, m_editButton, m_testButton;
  Gtk::Box box1, box2, box3;
  Gtk::CheckButton m_checkRunInTerminal;
  Gtk::Scale m_scaleSizeRows;
  Gtk::Scale m_scaleSizeColumns;

 protected:
  // Signal handlers:

  // Member widgets:
};
