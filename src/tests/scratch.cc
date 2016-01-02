/*
 * OpenZone - simple cross-platform FPS/RTS game engine.
 *
 * Copyright © 2002-2016 Davorin Učakar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ozCore/ozCore.hh>
#include <ozEngine/ozEngine.hh>

using namespace oz;

class MainStage : public Application::Stage
{
public:

  void update() override;
  void present(bool isEnoughTime) override;

};

void MainStage::update()
{}

void MainStage::present(bool isEnoughTime)
{
  if (isEnoughTime) {
    glClearColor(0.2f, 0.2f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    Window::swapBuffers();
  }
}

static MainStage mainStage;

int main()
{
  System::init();

  Application::defaults.name         = "scratch";
  Application::defaults.window.title = "Scratch";
  Application::defaults.loadConfig   = true;
  Application::defaults.saveConfig   = true;
  Application::run(&mainStage);
  return 0;
}
