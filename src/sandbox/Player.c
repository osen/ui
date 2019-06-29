#include "Player.h"

component(Player)
{
  int health;
  int ammo;
};

event Init(struct Player *ctx, struct InitEvent *e)
{

}

event Enter(struct Player *ctx, struct EnterEvent *e)
{

}

event Click(struct Player *ctx, struct ClickEvent *e)
{

}

events(Player)
{
  oninit;
  onclick;
  onenter;
}

