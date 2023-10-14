/*
    This file is part of Helio music sequencer.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "ColourButton.h"

class ColourSwatches final : public Component, public ColourButton::Listener
{
public:

    ColourSwatches();

    void onColourButtonClicked(ColourButton *button) override;
    void setSelectedColour(Colour colour);
    Colour getColour() const noexcept;
    int getNumButtons() const noexcept;

    void resized() override;

private:

    Colour lastSelectedColour;
    OwnedArray<ColourButton> buttons;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ColourSwatches)
};
