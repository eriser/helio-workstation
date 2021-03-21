/*
    This file is part of Helio Workstation.

    Helio is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Helio is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Helio. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "HelioTheme.h"
#include "ColourIDs.h"

class PanelBackgroundA final : public Component
{
public:

    PanelBackgroundA()
    {
        this->setOpaque(true);
        this->setPaintingIsUnclipped(true);
        this->setInterceptsMouseClicks(false, false);
    }

    ~PanelBackgroundA() override = default;

    void paint(Graphics &g) override
    {
        const auto &theme = HelioTheme::getCurrentTheme();
        if (theme.getBgCacheA().isValid())
        {
            g.setTiledImageFill(theme.getBgCacheA(), 0, 0, 1.f);
            g.fillRect(this->getLocalBounds());
        }
        else
        {
            g.setColour(findDefaultColour(ColourIDs::BackgroundA::fill));
            g.fillRect(this->getLocalBounds());
        }
    }

    static void redrawBgCache(HelioTheme &theme)
    {
        if (theme.getBgCacheA().isValid())
        {
            return;
        }

        constexpr int w = 128;
        constexpr int h = 128;
        Image render(Image::ARGB, w, h, true);
        Graphics g(render);
        g.setColour(theme.findColour(ColourIDs::BackgroundA::fill));
        g.fillAll();
        HelioTheme::drawNoise(theme, g, 0.5f);
        theme.getBgCacheA() = render;
    }

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PanelBackgroundA)
};
