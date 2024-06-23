/* Created by Matthew Brown on 6/23/2024 */
#include "gui/Panel.h"
#include "GeometryDash.h"

void Panel::render() {
    m_shape.setFillColor(m_style.backgroundColor);
    m_shape.setOutlineColor(m_style.borderColor);
    m_shape.setOutlineThickness(m_style.borderThickness);

    // Cpp file is necessary to not be recursive here
    GeometryDash::getInstance().getWindow().getWindow().draw(m_shape);
}