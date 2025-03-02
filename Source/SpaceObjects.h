/*
  ==============================================================================

	SpaceObjects.h
	Created: 2 Mar 2025 3:54:44pm
	Author:  97252

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class SpaceObject : public juce::Drawable
{
public:
    SpaceObject()
    {
        objectPath.clear();
        
        objectPath.startNewSubPath(20, 10);
        objectPath.lineTo(50, 5);
        objectPath.lineTo(80, 30);
        objectPath.lineTo(70, 60);
        objectPath.lineTo(40, 80);
        objectPath.lineTo(10, 50);
        objectPath.closeSubPath();
        setPosition(100, 100);
    }

    ~SpaceObject() override
    {
    }

    juce::Path getOutlineAsPath() const override
    {
        return objectPath;
    }

    void draw(juce::Graphics& g, float opacity) const
    {
        g.setColour(juce::Colours::darkgrey);
        g.fillPath(transformedPath);

        g.setColour(juce::Colours::black);
        g.strokePath(transformedPath, juce::PathStrokeType(2.0f));
    }

    std::unique_ptr<juce::Drawable> createCopy() const override
    {
        return std::make_unique<SpaceObject>();
    }

    void setBounds(const juce::Rectangle<float>& area) 
    {
        objectBoundingBox = area;
        setBoundsToEnclose(objectBoundingBox);
    }

    void setPosition(float x, float y)
    {
        auto translation = juce::AffineTransform::translation(x, y);
        transformedPath = objectPath; // Copy original path
        transformedPath.applyTransform(translation);
        position = { x, y };
    }

    juce::Rectangle<float> getDrawableBounds() const override
    {
        return objectBoundingBox;
    }

private:
	juce::Path objectPath;
    juce::Path transformedPath;
	juce::Rectangle<float> objectBoundingBox;
    juce::Point<float> position;
};