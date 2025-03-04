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
        
        objectPath.startNewSubPath(50, 13);
        objectPath.lineTo(44, 15);
        objectPath.lineTo(37, 21);
        objectPath.lineTo(34, 22);
        objectPath.lineTo(33, 25);
        objectPath.lineTo(31, 32);
        objectPath.lineTo(33, 37);
        objectPath.lineTo(35, 41);
        objectPath.lineTo(41, 44);
        objectPath.lineTo(52, 46);
        objectPath.lineTo(63, 40);
        objectPath.lineTo(68, 32);
        objectPath.lineTo(69, 22);
        objectPath.lineTo(65, 17);
        objectPath.lineTo(59, 14);
        objectPath.closeSubPath();
        setPosition(100, 100);
        objectAlpha = 1.0f;
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
        g.setColour(juce::Colours::darkgrey.withAlpha(objectAlpha));
        g.fillPath(transformedPath);

        g.setColour(juce::Colours::black.withAlpha(objectAlpha));
        g.strokePath(transformedPath, juce::PathStrokeType(1.0f));
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

    void setAlpha(float newAlpha)
    {
        objectAlpha = juce::jlimit(0.0f, 1.0f, newAlpha);
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
	float objectAlpha;
};