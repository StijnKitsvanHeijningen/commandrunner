#include "second_window.h"
#include <iostream>
#include <fstream>


Second_Window::Second_Window()
    : m_labelName("Name"),
      m_labelDescription("Description"),
      m_labelCommand("Command"),
      m_addButton("Add"),
      m_editButton("Save"),
      m_checkRunInTerminal("Run in terminal"),
      m_labelTerminalRows("Terminal Rows"),
      m_labelTerminalColumns("Terminal Colums"),
      m_labelReturnToDefault("Return to default")
{

    //layout
    m_entryName.set_max_width_chars(25);
    m_entryDescription.set_size_request(400,175);
    m_entryCommand.set_size_request(400,175);
    m_entryDescription.set_wrap_mode(Gtk::WRAP_WORD_CHAR);
    m_entryCommand.set_wrap_mode(Gtk::WrapMode::WRAP_WORD_CHAR);

 
    //margin
    m_labelName.set_margin_end(15);
    m_labelName.set_margin_start(10);
    m_labelName.set_margin_top(15);

    m_entryName.set_margin_top(10);

    m_labelDescription.set_margin_end(15);
    m_labelDescription.set_margin_start(10);
    m_labelDescription.set_margin_top(40); 
    m_labelDescription.set_margin_bottom(52);

    m_entryDescription.set_margin_top(25);

    m_labelCommand.set_margin_end(15);
    m_labelCommand.set_margin_start(10);
    m_labelCommand.set_margin_top(110);
    m_labelCommand.set_margin_bottom(48);

    m_labelTerminalRows.set_margin_top(210);
    m_labelTerminalRows.set_margin_end(15);
    m_labelTerminalRows.set_margin_start(10);
    m_labelTerminalColumns.set_margin_start(10);
    m_labelTerminalColumns.set_margin_end(15);
    m_labelTerminalColumns.set_margin_top(38);
    m_labelTerminalColumns.set_margin_bottom(150);
    m_labelTerminalColumns.set_margin_start(10);
    m_labelTerminalRows.set_margin_start(10);

    m_entryCommand.set_margin_top(10);

    m_addButton.set_margin_top(25);
    m_editButton.set_margin_top(25);
    m_addButton.set_margin_bottom(25);
    m_editButton.set_margin_bottom(25);

    m_entryName.set_margin_end(15);
    m_entryDescription.set_margin_end(15);
    m_descriptionScrolledWindow.set_size_request(350,200);
    m_entryDescription.set_left_margin(3);
    m_entryCommand.set_left_margin(3);
    m_commandScrolledWindow.set_size_request(350,200);
    m_entryCommand.set_margin_end(15);

    m_labelReturnToDefault.set_markup("<span underline=\"single\" size=\"small\">Return to default</span>");
    m_labelReturnToDefault.set_margin_end(235);
    m_labelReturnToDefault.set_margin_top(5);

    m_labelEventBox.add(m_labelReturnToDefault);

    

    m_addButton.set_margin_end(15);
    m_editButton.set_margin_end(15);
    //adding
    box2.set_orientation(Gtk::ORIENTATION_VERTICAL);
    box3.set_orientation(Gtk::ORIENTATION_VERTICAL);
    box1.add(box2);
    box1.add(box3);
    box2.add(m_labelName);
    box2.add(m_labelDescription);
    box2.add(m_labelCommand);
    box2.add(m_labelTerminalRows);
    box2.add(m_labelTerminalColumns);
    box3.add(m_entryName);
    m_descriptionScrolledWindow.add(m_entryDescription);
    box3.add(m_descriptionScrolledWindow);
    m_commandScrolledWindow.add(m_entryCommand);
    box3.add(m_commandScrolledWindow);
    box3.add(m_labelError);
    box3.add(m_checkRunInTerminal);
    m_scaleSizeColumns.set_digits(0);
    m_scaleSizeRows.set_digits(0);
    m_scaleSizeColumns.set_margin_top(10);
    m_scaleSizeRows.set_margin_top(10);
    box3.add(m_scaleSizeRows);
    box3.add(m_scaleSizeColumns);
    box3.add(m_labelEventBox);
    box3.add(m_addButton);
    m_labelError.set_margin_top(15);
    m_labelError.set_xalign(0);
    add(box1);
    set_title("test");

}



Second_Window::~Second_Window()
{
}