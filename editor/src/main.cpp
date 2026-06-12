#include "EditorApplication.h"
#include <memory>

int main()
{
    return Aine::RunApplication(std::make_unique<AineEditor::EditorApp>());
}