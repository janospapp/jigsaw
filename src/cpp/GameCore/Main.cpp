#include "GameCore/GameApp.hpp"
#include "TGUI/TGUI.hpp"

int main(){
    GameApp::initializeGame();
    GameApp::Gameloop();

    /*sf::RenderWindow screen(sf::VideoMode(800, 600, 32),
                                          "Test");
    tgui::Gui gui(screen);
    gui.setGlobalFont("Fonts/arial.ttf");

    tgui::ChildWindow::Ptr wind = tgui::ChildWindow::create("Gui/Black.conf");
    wind->setSize(sf::Vector2f(500.f,350.f));
    gui.add(wind);

    tgui::Button::Ptr btn = tgui::Button::create("Gui/Black.conf");
    btn->setText("Click");
    btn->setPosition(tgui::bindWidth(wind,0.1f),tgui::bindHeight(wind,0.3f));
    btn->setSize(tgui::bindSize(wind,sf::Vector2f(0.4f,0.15f)));
    wind->add(btn);
    //gui.draw();

    /*tgui::MessageBox::Ptr exitMsg = tgui::MessageBox::create("Gui/Black.conf");
    exitMsg->setText(sf::String("Are you sure you want to quit?"));
    exitMsg->addButton(sf::String("Yes"));
    exitMsg->addButton(sf::String("No"));
    auto exitHandle = [&](const sf::String &btn){
        if(btn.toAnsiString() == "Yes")
        {
            screen.close();
        }
    };
    exitMsg->connect("ButtonPressed",exitHandle);
    gui.add(exitMsg);*/

    /*while(screen.isOpen())
    {
        sf::Event currevt_;
        while(screen.pollEvent(currevt_))
        {
            gui.handleEvent(currevt_);

            if(currevt_.type == sf::Event::Closed)
            {
                screen.close();
            }

            screen.clear();
            gui.draw();
            screen.display();
        }
    }*/

	return 0;
}
