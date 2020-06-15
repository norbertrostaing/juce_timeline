/*
  ==============================================================================

    TimeTrigger.cpp
    Created: 20 Nov 2016 3:18:20pm
    Author:  Ben Kuper

  ==============================================================================
*/

TimeTrigger::TimeTrigger(StringRef name) :
	BaseItem(name)
{
	itemDataType = "TimeTrigger"; 

	showWarningInUI = true;

	time = addFloatParameter("Time", "Time at which the action is triggered", 0, 0, 3600);
	
	time->defaultUI = FloatParameter::TIME;
	flagY = addFloatParameter("Flag Y", "Position of the trigger's flag", 0,0,1);
	isTriggered = addBoolParameter("Is Triggered", "Is this Time Trigger already triggered during this playing ?", false);
	
	color = addColorParameter("Color", "Color of the trigger", BG_COLOR.brighter(.1f));

	isTriggered->hideInEditor = true;
	isTriggered->isSavable = false;
}

TimeTrigger::~TimeTrigger()
{

}

void TimeTrigger::onContainerParameterChangedInternal(Parameter * p)
{
	if (p == isUILocked)
	{
		time->setControllableFeedbackOnly(isUILocked->boolValue());
		//flagY->setControllableFeedbackOnly(isLocked->boolValue());
	}
}

void TimeTrigger::setMovePositionReferenceInternal()
{
	movePositionReference = Point<float>(time->floatValue(), flagY->floatValue());
}

void TimeTrigger::setPosition(Point<float> targetPosition)
{
	time->setValue(targetPosition.x);
	flagY->setValue(targetPosition.y);
}

Point<float> TimeTrigger::getPosition()
{
	return Point<float>(time->floatValue(), flagY->floatValue());
}

void TimeTrigger::addUndoableMoveAction(Array<UndoableAction*>& actions)
{
	actions.add(time->setUndoableValue(movePositionReference.x, time->floatValue(), true));
	actions.add(flagY->setUndoableValue(movePositionReference.y, flagY->floatValue(), true));

}

void TimeTrigger::trigger()
{
	if (!enabled->boolValue()) return;
	isTriggered->setValue(true);
	triggerInternal();
}