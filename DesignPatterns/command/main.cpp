#include "Commands.h"
#include "Menu.h"
#include "MenuFP.h"

using namespace std;

class MenuHelpCommand : public ICommand {
public:
  MenuHelpCommand(const Menu& menu)
    : m_menu(menu) {}

  void Execute() override { m_menu.ShowInstructions(); }

private:
  const Menu& m_menu;
};

class ExitMenuCommand : public ICommand {
public:
  ExitMenuCommand(Menu& menu)
    : m_menu(menu) {}

  void Execute() override { m_menu.Exit(); }

private:
  Menu& m_menu;
};

template <typename Commands>
MenuFP::Command CreateMacroCommand(Commands&& commands) {
  return [=] {
    for (auto& command : commands) {
      command();
    }
  };
}

void TestMenuWithClassicCommandPattern() {
  Robot robot;
  Menu menu;
  menu.AddItem("on", "Turns the Robot on", make_unique<TurnOnCommand>(robot));
  menu.AddItem("off", "Turns the Robot off", make_unique<TurnOffCommand>(robot));

  menu.AddItem(
    "north", "Makes the Robot walk north", make_unique<WalkCommand>(robot, WalkDirection::North));
  menu.AddItem(
    "south", "Makes the Robot walk south", make_unique<WalkCommand>(robot, WalkDirection::South));
  menu.AddItem(
    "west", "Makes the Robot walk west", make_unique<WalkCommand>(robot, WalkDirection::West));
  menu.AddItem(
    "east", "Makes the Robot walk east", make_unique<WalkCommand>(robot, WalkDirection::East));
  auto cmd = make_unique<MacroCommand>();
  cmd->AddCommand(make_unique<TurnOnCommand>(robot));
  cmd->AddCommand(make_unique<WalkCommand>(robot, WalkDirection::North));
  cmd->AddCommand(make_unique<WalkCommand>(robot, WalkDirection::East));
  cmd->AddCommand(make_unique<WalkCommand>(robot, WalkDirection::South));
  cmd->AddCommand(make_unique<WalkCommand>(robot, WalkDirection::West));
  cmd->AddCommand(make_unique<TurnOffCommand>(robot));
  menu.AddItem("patrol", "Patrol the territory", move(cmd));

  menu.AddItem("stop", "Stops the Robot", make_unique<StopCommand>(robot));

  menu.AddItem("help", "Show instructions", make_unique<MenuHelpCommand>(menu));
  menu.AddItem("exit", "Exit from this menu", make_unique<ExitMenuCommand>(menu));

  menu.Run();
}

void TestMenuWithFunctionalCommandPattern() {
  Robot robot;
  MenuFP menu;

  menu.AddItem("on", "Turns the Robot on", [&] { robot.TurnOn(); });
  menu.AddItem("off", "Turns the Robot off", bind(&Robot::TurnOff, &robot));

  menu.AddItem(
    "north", "Makes the Robot walk north", bind(&Robot::Walk, &robot, WalkDirection::North));
  menu.AddItem(
    "south", "Makes the Robot walk south", bind(&Robot::Walk, &robot, WalkDirection::South));
  menu.AddItem(
    "west", "Makes the Robot walk west", bind(&Robot::Walk, &robot, WalkDirection::West));
  menu.AddItem(
    "east", "Makes the Robot walk east", bind(&Robot::Walk, &robot, WalkDirection::East));

  menu.AddItem("stop", "Stops the Robot", bind(&Robot::Stop, &robot));

  menu.AddItem(
    "patrol", "Patrol the territory",
    CreateMacroCommand<vector<MenuFP::Command>>(
      {bind(&Robot::TurnOn, &robot), bind(&Robot::Walk, &robot, WalkDirection::North),
       bind(&Robot::Walk, &robot, WalkDirection::South),
       bind(&Robot::Walk, &robot, WalkDirection::West),
       bind(&Robot::Walk, &robot, WalkDirection::East), bind(&Robot::TurnOff, &robot)}));

  menu.AddItem("help", "Show instructions", bind(&MenuFP::ShowInstructions, &menu));
  menu.AddItem("exit", "Exit from this menu", bind(&MenuFP::Exit, &menu));

  menu.Run();
}

int main() {
  MenuFP menu;
  menu.AddItem("c", "Classic command pattern implementation", [&] {
    TestMenuWithClassicCommandPattern();
    menu.ShowInstructions();
  });
  menu.AddItem("f", "Functional command pattern implementation", [&] {
    TestMenuWithFunctionalCommandPattern();
    menu.ShowInstructions();
  });

  menu.AddItem("q", "Exit Program", bind(&MenuFP::Exit, &menu));
  menu.Run();

  return 0;
}