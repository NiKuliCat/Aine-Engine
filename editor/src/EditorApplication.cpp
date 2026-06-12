#include "EditorApplication.h"

namespace AineEditor
{
    EditorApp::EditorApp()
    : Aine::Application({
          .AppName = "Aine Engine",
          .Width = 1600,
          .Height = 900
      })
    {
    }

}