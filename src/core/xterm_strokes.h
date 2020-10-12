const std::vector<KeyCombo> KeyCombo::Combos = {
  // special keys above arrow keys in most keyboards
  {Key_Insert|Meta_None, "[2~"},
  {Key_Delete|Meta_None, "[3~"},
  {Key_PageUp|Meta_None, "[5~"},
  {Key_PageDown|Meta_None, "[6~"},
  // mintty
  {Key_Home|Meta_None, "OH"},
  {Key_End|Meta_None, "OF"},
  // cmd prompt
  {Key_Home|Meta_None, "[1~"},
  {Key_End|Meta_None, "[4~"},

  // Meta with special keys
  {Key_Delete|Meta_Ctrl, "[3;5~"},
  {Key_PageUp|Meta_Ctrl, "[5;5~"},
  {Key_PageDown|Meta_Ctrl, "[6;5~"},
  {Key_PageUp|Meta_Ctrl|Meta_Shift, "[5;6~"},
  {Key_PageDown|Meta_Ctrl|Meta_Shift, "[6;6~"},
  // mintty
  {Key_Home|Meta_Ctrl, "[1;5H"},
  {Key_End|Meta_Ctrl, "[1;5F"},
  // cmd prompt
  {Key_Home|Meta_Ctrl, "[1;5~"},
  {Key_End|Meta_Ctrl, "[4;5~"},

  // Arrow Keys
  // mintty
  {Key_ArrowUp|Meta_None, "OA"},
  {Key_ArrowDown|Meta_None, "OB"},
  {Key_ArrowRight|Meta_None, "OC"},
  {Key_ArrowLeft|Meta_None, "OD"},
  // cmd prompt
  {Key_ArrowUp|Meta_None, "[A"},
  {Key_ArrowDown|Meta_None, "[B"},
  {Key_ArrowRight|Meta_None, "[C"},
  {Key_ArrowLeft|Meta_None, "[D"},

  // Shift+Arrow Keys
  {Key_ArrowUp|Meta_Shift, "[1;2A"},
  {Key_ArrowDown|Meta_Shift, "[1;2B"},
  {Key_ArrowRight|Meta_Shift, "[1;2C"},
  {Key_ArrowLeft|Meta_Shift, "[1;2D"},

  // Alt+Arrow Keys
  // mintty
  {Key_ArrowUp|Meta_Alt, "[1;3A"},
  {Key_ArrowDown|Meta_Alt, "[1;3B"},
  {Key_ArrowRight|Meta_Alt, "[1;3C"},
  {Key_ArrowLeft|Meta_Alt, "[1;3D"},
  // cmd prompt
  {Key_ArrowUp|Meta_Alt, "\e[A"},
  {Key_ArrowDown|Meta_Alt, "\e[B"},
  {Key_ArrowRight|Meta_Alt, "\e[C"},
  {Key_ArrowLeft|Meta_Alt, "\e[D"},

  // Ctrl+Arrow Keys
  {Key_ArrowUp|Meta_Ctrl, "[1;5A"},
  {Key_ArrowDown|Meta_Ctrl, "[1;5B"},
  {Key_ArrowRight|Meta_Ctrl, "[1;5C"},
  {Key_ArrowLeft|Meta_Ctrl, "[1;5D"},

  // Ctrl+Alt+Arrow Keys
  // mintty
  {Key_ArrowUp|Meta_Ctrl|Meta_Alt, "[1;7A"},
  {Key_ArrowDown|Meta_Ctrl|Meta_Alt, "[1;7B"},
  {Key_ArrowRight|Meta_Ctrl|Meta_Alt, "[1;7C"},
  {Key_ArrowLeft|Meta_Ctrl|Meta_Alt, "[1;7D"},
  // cmd prompt
  {Key_ArrowUp|Meta_Ctrl|Meta_Alt, "\e[1;5A"},
  {Key_ArrowDown|Meta_Ctrl|Meta_Alt, "\e[1;5B"},
  {Key_ArrowRight|Meta_Ctrl|Meta_Alt, "\e[1;5C"},
  {Key_ArrowLeft|Meta_Ctrl|Meta_Alt, "\e[1;5D"},

  // Ctrl+Shift+Arrow Keys
  // mintty
  {Key_ArrowUp|Meta_Ctrl|Meta_Shift, "[1;6A"},
  {Key_ArrowDown|Meta_Ctrl|Meta_Shift, "[1;6B"},
  {Key_ArrowRight|Meta_Ctrl|Meta_Shift, "[1;6C"},
  {Key_ArrowLeft|Meta_Ctrl|Meta_Shift, "[1;6D"},
  // cmd prompt
  {Key_ArrowUp|Meta_Ctrl|Meta_Shift, "\e[1;6A"},
  {Key_ArrowDown|Meta_Ctrl|Meta_Shift, "\e[1;6B"},
  {Key_ArrowRight|Meta_Ctrl|Meta_Shift, "\e[1;6C"},
  {Key_ArrowLeft|Meta_Ctrl|Meta_Shift, "\e[1;6D"},

  // Ctrl+Shift+Alt+Arrow Keys
  // mintty
  {Key_ArrowUp|Meta_Ctrl|Meta_Alt|Meta_Shift, "[1;8A"},
  {Key_ArrowDown|Meta_Ctrl|Meta_Alt|Meta_Shift, "[1;8B"},
  {Key_ArrowRight|Meta_Ctrl|Meta_Alt|Meta_Shift, "[1;8C"},
  {Key_ArrowLeft|Meta_Ctrl|Meta_Alt|Meta_Shift, "[1;8D"},

  // Alt+Shift+Arrow Keys
  // cmd prompt
  {Key_ArrowUp|Meta_Alt|Meta_Shift, "[1;4A"},
  {Key_ArrowDown|Meta_Alt|Meta_Shift, "[1;4B"},
  {Key_ArrowRight|Meta_Alt|Meta_Shift, "[1;4C"},
  {Key_ArrowLeft|Meta_Alt|Meta_Shift, "[1;4D"},
  // cmd prompt
  {Key_ArrowUp|Meta_Alt|Meta_Shift, "\e[1;2A"},
  {Key_ArrowDown|Meta_Alt|Meta_Shift, "\e[1;2B"},
  {Key_ArrowRight|Meta_Alt|Meta_Shift, "\e[1;2C"},
  {Key_ArrowLeft|Meta_Alt|Meta_Shift, "\e[1;2D"},

  // Meta keys + TAB
  {Key_Tab|Meta_Ctrl, "[1;5I"},
  {Key_Tab|Meta_Shift, "[Z"},
  {Key_Tab|Meta_Ctrl|Meta_Shift, "[1;6I"},

  // Function keys
  // mintty
  {Key_F1|Meta_None, "OP"},
  {Key_F2|Meta_None, "OQ"},
  {Key_F3|Meta_None, "OR"},
  {Key_F4|Meta_None, "OS"},
  {Key_F5|Meta_None, "[15~"},
  // cmd prompt
  {Key_F1|Meta_None, "[[A"},
  {Key_F2|Meta_None, "[[B"},
  {Key_F3|Meta_None, "[[C"},
  {Key_F4|Meta_None, "[[D"},
  {Key_F5|Meta_None, "[[E"},
  // common
  {Key_F6|Meta_None, "[17~"},
  {Key_F7|Meta_None, "[18~"},
  {Key_F8|Meta_None, "[19~"},
  {Key_F9|Meta_None, "[20~"},
  {Key_F10|Meta_None, "[21~"},
  {Key_F11|Meta_None, "[23~"},
  {Key_F12|Meta_None, "[24~"},

  // Ctrl + Function keys
  // mintty
  {Key_F1|Meta_Ctrl, "[1;5P"},
  {Key_F2|Meta_Ctrl, "[1;5Q"},
  {Key_F3|Meta_Ctrl, "[1;5R"},
  {Key_F4|Meta_Ctrl, "[1;5S"},
  {Key_F5|Meta_Ctrl, "[15;5~"},
  {Key_F6|Meta_Ctrl, "[17;5~"},
  {Key_F7|Meta_Ctrl, "[18;5~"},
  {Key_F8|Meta_Ctrl, "[19;5~"},
  {Key_F9|Meta_Ctrl, "[20;5~"},
  {Key_F10|Meta_Ctrl, "[21;5~"},
  {Key_F11|Meta_Ctrl, "[23;5~"},
  {Key_F12|Meta_Ctrl, "[24;5~"},
  // cmd prompt
  {Key_F1|Meta_Ctrl, "[11^"},
  {Key_F2|Meta_Ctrl, "[12^"},
  {Key_F3|Meta_Ctrl, "[13^"},
  {Key_F4|Meta_Ctrl, "[14^"},
  {Key_F5|Meta_Ctrl, "[15^"},
  {Key_F6|Meta_Ctrl, "[17^"},
  {Key_F7|Meta_Ctrl, "[18^"},
  {Key_F8|Meta_Ctrl, "[19^"},
  {Key_F9|Meta_Ctrl, "[20^"},
  {Key_F10|Meta_Ctrl, "[21^"},
  {Key_F11|Meta_Ctrl, "[23^"},
  {Key_F12|Meta_Ctrl, "[24^"},

  // Ctrl + Shift + Function keys
  // mintty
  {Key_F1|Meta_Ctrl|Meta_Shift, "[1;6P"},
  {Key_F2|Meta_Ctrl|Meta_Shift, "[1;6Q"},
  {Key_F3|Meta_Ctrl|Meta_Shift, "[1;6R"},
  {Key_F4|Meta_Ctrl|Meta_Shift, "[1;6S"},
  {Key_F5|Meta_Ctrl|Meta_Shift, "[15;6~"},
  {Key_F6|Meta_Ctrl|Meta_Shift, "[17;6~"},
  {Key_F7|Meta_Ctrl|Meta_Shift, "[18;6~"},
  {Key_F8|Meta_Ctrl|Meta_Shift, "[19;6~"},
  {Key_F9|Meta_Ctrl|Meta_Shift, "[20;6~"},
  {Key_F10|Meta_Ctrl|Meta_Shift, "[21;6~"},
  {Key_F11|Meta_Ctrl|Meta_Shift, "[23;6~"},
  {Key_F12|Meta_Ctrl|Meta_Shift, "[24;6~"},
  // cmd prompt (F1, F2 combinations are aliased!)
  {Key_F3|Meta_Ctrl|Meta_Shift, "[25^"},
  {Key_F4|Meta_Ctrl|Meta_Shift, "[26^"},
  {Key_F5|Meta_Ctrl|Meta_Shift, "[28^"},
  {Key_F6|Meta_Ctrl|Meta_Shift, "[29^"},
  {Key_F7|Meta_Ctrl|Meta_Shift, "[31^"},
  {Key_F8|Meta_Ctrl|Meta_Shift, "[32^"},
  {Key_F9|Meta_Ctrl|Meta_Shift, "[33^"},
  {Key_F10|Meta_Ctrl|Meta_Shift, "[34^"},
  {Key_F11|Meta_Ctrl|Meta_Shift, "[23@"},
  {Key_F12|Meta_Ctrl|Meta_Shift, "[24@"},
};
