#include <gtkmm.h>

#include <vector>

#include "json/json.h"
#include "second_window.h"
class Command_Runner : public Gtk::Window {
 public:
  Command_Runner();
  virtual ~Command_Runner();

 protected:
  // Signal handlers:
  void on_addButton_clicked();
  void on_runButton_clicked();
  void on_editButton_clicked();
  void on_removeButton_clicked();
  void on_getTerminalSizeButton_clicked();
  void on_rowSelected(Gtk::ListBoxRow* row);
  void on_addCommandButton_clicked();
  void on_editCommandButton_clicked();
  void on_checkUpdatesToggle();
  Glib::RefPtr<Gio::SimpleAction> m_refToggle;
  void on_label_drag_data_get(const Glib::RefPtr<Gdk::DragContext>& context,
                              Gtk::SelectionData& selection_data, guint info,
                              guint time, Gtk::EventBox* box);
  void on_label_drop_drag_data_received(
      const Glib::RefPtr<Gdk::DragContext>& context, int x, int y,
      const Gtk::SelectionData& selection_data, guint info, guint time,
      Gtk::ListBoxRow* row);
  void on_drag_begin(const Glib::RefPtr<Gdk::DragContext>& context,
                     Gtk::EventBox* box);
  void refresh();
  void on_file_import();
  void on_file_export();
  void on_file_quit();
  void on_help_about();
  void on_file_calibrate();
  void on_file_desktop();
  bool on_ColumnValueChanged(Gtk::ScrollType scrollType, gdouble newValue);
  bool on_ColumnScalePressed(GdkEventButton* event);
  bool on_ColumnScaleReleased(GdkEventButton* event);
  bool on_RowValueChanged(Gtk::ScrollType scrollType, gdouble newValue);
  void runProgram(std::string command);
  void updateCheck();
  void performUpdate();
  void installDesktopFile();
  void on_UpdateDone();
  void updateJsonFile();
  bool on_ReturnToDefault_clicked(GdkEventButton* button);
  bool on_ReturnToDefault_entered(GdkEventCrossing* event);
  bool on_ReturnToDefault_left(GdkEventCrossing* event);
  // Member widgets:
  Glib::Dispatcher m_updateDone;
  Glib::Thread* m_updateThread;
  char currentPath[256];
  Gtk::EventBox m_test;
  Glib::RefPtr<Gdk::Pixbuf> m_drag_icon;
  Json::Value jsonFile;
  std::vector<std::string> currentCommand;
  int selectedIndex;
  Gtk::Box m_box0, m_box1, m_box2, m_box3, m_box4, m_box5;
  Gtk::Button m_buttonRun, m_buttonEdit, m_buttonAdd, m_buttonRemove,
      m_buttonGetTerminalSize;
  Gtk::Label m_labelName, m_labelDescription, m_labelError,
      m_labelCommandRunner;
  Gtk::ListBox m_commandList;
  Gtk::MessageDialog m_updatingDialog;
  Gtk::Image m_commandRunnerImage;
  Gtk::ScrolledWindow m_scrolledWindow;
  std::vector<Gtk::TargetEntry> listTargets;
  Gtk::TextView m_Description;
  Second_Window win2;
  Gtk::MenuBar menubar;
  Gtk::AboutDialog m_AboutDialog;
  Gtk::Window m_previewWindow;
  double rowSize;
  double columnSize;

  Glib::RefPtr<Gtk::Builder> m_refBuilder;
  Glib::RefPtr<Gio::SimpleActionGroup> m_fileActionGroup;
  Glib::RefPtr<Gio::SimpleActionGroup> m_helpActionGroup;

};
