#include"Editor.hpp"

void Editor::inputsFunction()
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

