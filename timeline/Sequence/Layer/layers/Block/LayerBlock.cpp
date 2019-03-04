#include "LayerBlock.h"
/*
  ==============================================================================

	LayerBlock.cpp
	Created: 14 Feb 2019 11:14:35am
	Author:  bkupe

  ==============================================================================
*/

LayerBlock::LayerBlock(StringRef name, float _time) :
	BaseItem(name, true, false)
{
	time = addFloatParameter("Start Time", "Time of the start of the clip", 0, 0, 3600);
	time->setValue(_time);
	time->defaultUI = FloatParameter::TIME;

	coreLength = addFloatParameter("Length", "Length of the clip's core, without looping (in seconds)", 10, .1f, 3600);
	coreLength->defaultUI = FloatParameter::TIME;
	loopLength = addFloatParameter("Loop Length", "Length of the clip's oop, after the core", 0, 0, 3600);
	loopLength->defaultUI = FloatParameter::TIME;

	isActive = addBoolParameter("Is Active", "This is a feedback to know if block is currently active in the timeline", false);
	isActive->setControllableFeedbackOnly(true);
}

LayerBlock::~LayerBlock()
{
}

float LayerBlock::getTotalLength()
{
	return coreLength->floatValue() + loopLength->floatValue();
}

float LayerBlock::getCoreEndTime()
{
	return time->floatValue() + coreLength->floatValue();
}

float LayerBlock::getEndTime()
{
	return time->floatValue() + getTotalLength();
}

bool LayerBlock::isInRange(float _time)
{
	return _time >= time->floatValue() && _time < getEndTime();
}


void LayerBlock::setCoreLength(float newLength, bool /*stretch*/, bool /*stickToCoreEnd*/)
{
	coreLength->setValue(newLength);
}

void LayerBlock::setLoopLength(float newLength)
{
	loopLength->setValue(newLength);
}

void LayerBlock::setStartTime(float newStart, bool keepCoreEnd, bool stickToCoreEnd)
{
	float timeDiff = newStart - time->floatValue();
	time->setValue(newStart);
	if (keepCoreEnd) setCoreLength(coreLength->floatValue() - timeDiff, false, stickToCoreEnd);
}

double LayerBlock::getRelativeTime(double t, bool timeIsAbsolute)
{
	if (timeIsAbsolute) t -= time->floatValue();
	return fmod(t, coreLength->floatValue());
}