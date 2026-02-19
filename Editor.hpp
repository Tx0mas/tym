#include<vector>
#include<string>
#include<ncurses.h>
#include<iostream>
#include<fstream>

enum class Mode
{
    NormalMode,
    InsertMode,
    VisualMode,
    CommandMode,
};

class Editor
{
private:
    Mode mode{Mode::NormalMode};
    std::vector<std::string> buffer{};
    std::string bufferCommandLine{};
    std::string linea{};
    std::fstream file;
    std::string fileName;
    std::ofstream outFile;
    int letra{};
    int y_actual{};
    int x_actual{};
public:
    Editor(std::string nombreFile)
    :fileName{nombreFile}
    {
        file.open(nombreFile,std::ios::in | std::ios::out | std::ios::binary);;
        if (!file.is_open())
        {
            std::cerr<<"Error de file";
            return;
        }
        while (std::getline(file,linea))
        {
            buffer.push_back(linea);
        }
        if (buffer.empty())
        {
            buffer.push_back("");
        }
    }

    void renderLine()
    {
        clrtoeol(); 
        mvprintw(y_actual,0,"%s",buffer[y_actual].c_str());
        move(y_actual,x_actual);
        refresh();
    }

    void renderScreen()
    {
        for (int fila{};fila<buffer.size();fila++)
        {
            clrtoeol(); 
            mvprintw(fila,0,"%s",buffer[fila].c_str());
        }
        move(y_actual,x_actual);
        refresh();
    }

    void renderFirstScreen()
    {
        initscr();            
        noecho();             
        keypad(stdscr, TRUE);
        cbreak();             
        scrollok(stdscr, FALSE); 
        idlok(stdscr, FALSE);   
        renderScreen();
    }

    void handleInsertMode()
    {
        getyx(stdscr, y_actual, x_actual);
        buffer[y_actual].insert(x_actual,1,letra);
        x_actual+=1;
        move(y_actual,x_actual);
        renderLine();
    }

    void handleNormalMode()
    {
        if (letra == 'j')
        {
            y_actual+=1;
        }
        else if (letra == 'k' && y_actual>0)
        {
            y_actual-=1;
        }
        else if (letra == 'l')
        {
            x_actual+=1;
        }
        else if (letra == 'h' && x_actual>0)
        {
            x_actual-=1;
        }
        move(y_actual,x_actual);
        refresh();
    }

    void handleCommandsTypes()
    {
        if( bufferCommandLine == ":q")
        {
            file.close();
            endwin();             
            return;
        }
        if( bufferCommandLine == ":wq")
        {
            std::fstream outFile (fileName, std::ios::trunc);
            for (auto &lineas : buffer)
            {
                outFile<<lineas<<'\n';
            }
            outFile.close();
            file.close();
            endwin();
            return;
        }

    }

    void handleCommandMode()
    {
        getyx(stdscr, y_actual, x_actual);
        bufferCommandLine.clear();
        bufferCommandLine.push_back(letra);
        int command_x{0};
        int command_y{28};

        move(command_y,command_x);
        command_x+=1;
        mvprintw(command_y,command_x,"%s",bufferCommandLine.c_str());
        move(command_y,command_x);
        refresh();

        while((letra = getch()) != 27)
        {
            char cLetra = (char)letra;
            if(letra == KEY_ENTER)
            {
                handleCommandsTypes();
                break;
            }
            else
            {
                bufferCommandLine.push_back(cLetra);
                command_x+=1;
                mvprintw(command_y,command_x,"%s",bufferCommandLine.c_str());
                move(command_y,command_x+1);
                refresh();
            }
        }
        clrtoeol(); 
        move(y_actual,x_actual);
        mode = Mode::NormalMode;
        refresh();
    }

    void inputsFunction()
    {
        if (mode == Mode::NormalMode)
        {
            handleNormalMode();
        }
        else if (mode == Mode::InsertMode)
        {
            handleInsertMode();
        }
        else if (mode == Mode::CommandMode)
        {
            handleCommandMode();
        }
    }

    void general()
    {
        renderFirstScreen();

        while (true)
        {
            letra = getch();
            if (letra == 'i' && mode == Mode::NormalMode)
            {
                mode=Mode::InsertMode;
            }
            else if (letra == 27 && mode == Mode::InsertMode)
            {
                mode=Mode::NormalMode;
            }
            else if (letra == ':' && mode == Mode::NormalMode)
            {
                mode=Mode::CommandMode;
                inputsFunction();
            }
            else
            {
                inputsFunction();
            }
        }

        endwin();
    }




};
