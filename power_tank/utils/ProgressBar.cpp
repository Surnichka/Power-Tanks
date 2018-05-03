#include "ProgressBar.h"

namespace
{
    static const sf::Color emptyCellColor = sf::Color(170, 170, 170);
    static const sf::Color fullCellColor = sf::Color::Green;
}

void ProgressBar::Init(sf::FloatRect rect, int maxElement,
                       ProgressBar::Orientation orientation, ProgressBar::FillDirection fillDir)
{
    m_maxElement = maxElement;
    m_orientation = orientation;
    m_fillDir = fillDir;

    float xPos = rect.left;
    float yPos = rect.top;
    float width = rect.width;
    float height = rect.height;

    if( Orientation::Horizontal == orientation )
    {
        float cellWidth = width / maxElement;
        for(int i = 0; i <= maxElement; i++)
        {
            sf::RectangleShape cell;
            cell.setPosition(xPos, yPos);
            cell.setSize({cellWidth, height});
            cell.setOutlineColor(sf::Color::Black);
            cell.setOutlineThickness(1);
            cell.setFillColor(emptyCellColor);

            xPos += cellWidth;
            m_bar.emplace_back(std::move(cell));
        }

    }
    else if( Orientation::Vertical == orientation )
    {
        float cellHeight = height / maxElement;
        for(int i = 0; i < maxElement; i++)
        {
            sf::RectangleShape cell;
            cell.setPosition(xPos, yPos);
            cell.setSize({width, cellHeight});
            cell.setOutlineColor(sf::Color::Black);
            cell.setOutlineThickness(1);
            cell.setFillColor(emptyCellColor);

            yPos += cellHeight;
            m_bar.emplace_back(std::move(cell));
        }
    }
}

void ProgressBar::Add()
{
    if( m_progressCounter == m_maxElement )
    {
        return;
    }
    m_progressCounter++;
    if( FillDirection::Front == m_fillDir)
    {
        m_bar[m_currentElement].setFillColor(fullCellColor);
    }
    else if(FillDirection::End == m_fillDir)
    {
        size_t elementId = (m_maxElement - 1) - m_currentElement;
        m_bar[elementId].setFillColor(fullCellColor);
    }
    m_currentElement = std::min(m_maxElement - 1, m_currentElement + 1);
}

void ProgressBar::Pop()
{
    if( m_progressCounter == 0 )
    {
        return;
    }
    m_progressCounter--;

    if( FillDirection::Front == m_fillDir)
    {
        m_bar[m_currentElement].setFillColor(emptyCellColor);
    }
    else if(FillDirection::End == m_fillDir)
    {
        size_t elementId = (m_maxElement - 1) - m_currentElement;
        m_bar[elementId].setFillColor(emptyCellColor);
    }
    m_currentElement = std::max(0, m_currentElement - 1);
}

void ProgressBar::Draw(sf::RenderWindow &window)
{
    for(const auto& bar : m_bar)
    {
        window.draw(bar);
    }
}

int ProgressBar::GetCurrent() const
{
    return m_currentElement;
}

bool ProgressBar::isFull() const
{
    return m_progressCounter == m_maxElement;
}

//void ProgressBar::FillTo(int element)
//{

//}

