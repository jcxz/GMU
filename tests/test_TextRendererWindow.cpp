/*
 * Copyright (C) 2014 Matus Fedorko <xfedor01@stud.fit.vutbr.cz>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:

 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include "test_TextRendererWindow.h"



void TextRendererWindow::onRedraw(void)
{
  glClear(GL_COLOR_BUFFER_BIT);

  m_text_renderer.renderSmall(10, 10, "Hello, World!");
  m_text_renderer.render(10, 110, "Hello, World!");
  m_text_renderer.renderLarge(10, 210, "Hello, World!");

  return;
}


void TextRendererWindow::onResize(int32_t width, int32_t height)
{
  glViewport(0, 0, width, height);
  return;
}