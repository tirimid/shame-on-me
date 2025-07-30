// SPDX-License-Identifier: GPL-3.0-or-later

void i_handle(SDL_Event const *e);
void i_prepare(void);
bool i_kdown(SDL_Keycode k);
bool i_kpressed(SDL_Keycode k);
bool i_kreleased(SDL_Keycode k);
void i_mpos(OUT i32 *x, OUT i32 *y);
bool i_mdown(i32 btn);
bool i_mpressed(i32 btn);
bool i_mreleased(i32 btn);
