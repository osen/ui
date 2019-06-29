#include <ui/ui.h>

#include <stdio.h>
#include <stdlib.h>

ref(Panel) WindowAddContentPanel(ref(Window) w)
{
  ref(Panel) panel = NULL;
  ref(Button) btn = NULL;

  panel = WindowAddPanel(w);
  PanelSetSize(panel, 200, 200);
  PanelLayout(panel, MOVE_LEFT);

  btn = PanelAddButton(panel);
  ButtonLayout(btn, MOVE_DOWN);
  ButtonLayout(btn, MOVE_RIGHT);

  return panel;
}

void WizardWindowCreate(ref(Platform) p)
{
  ref(Window) w = NULL;
  ref(Button) nextButton = NULL;
  ref(Button) prevButton = NULL;
  ref(Button) cancelButton = NULL;
  ref(Line) line = NULL;
  ref(Panel) content = NULL;

  w = PlatformAddWindow(p);
  WindowSetSize(w, 640, 480);

  nextButton = WindowAddButton(w);
  ButtonLayout(nextButton, MOVE_LEFT);
  ButtonLayout(nextButton, MOVE_DOWN);
  ButtonLayout(nextButton, MOVE_RIGHT);
  ButtonFocus(nextButton);

  prevButton = WindowAddButton(w);
  ButtonLayout(prevButton, MOVE_LEFT);
  ButtonLayout(prevButton, MOVE_DOWN);
  ButtonLayout(prevButton, MOVE_RIGHT);

  cancelButton = WindowAddButton(w);
  ButtonLayout(cancelButton, MOVE_LEFT);
  ButtonLayout(cancelButton, MOVE_DOWN);
  ButtonLayout(cancelButton, MOVE_RIGHT);

  line = WindowAddLine(w);
  LineLayout(line, MOVE_LEFT);
  LineLayout(line, EXPAND_RIGHT);
  LineLayout(line, MOVE_DOWN);

  WindowAddContentPanel(w);
  content = WindowAddContentPanel(w);
  PanelSetSize(content, 100, 200);
  PanelLayout(content, MOVE_RIGHT);
  PanelLayout(content, EXPAND_LEFT);
  PanelLayout(content, EXPAND_UP);
  PanelLayout(content, EXPAND_DOWN);
}

int main()
{
  ref(Platform) p = NULL;

  p = PlatformInit();

  WizardWindowCreate(p);
/*
  WizardWindowCreate(p);
*/

  PlatformRun(p);
  PlatformCleanup(p);

  return 0;
}

