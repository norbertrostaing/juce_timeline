/*
  ==============================================================================

    SequenceTimelineHeader.cpp
    Created: 23 Nov 2016 11:32:15pm
    Author:  Ben Kuper

  ==============================================================================
*/

SequenceTimelineHeader::SequenceTimelineHeader(Sequence * _sequence, TimeCueManagerUI * cueManagerUI, TimeNeedleUI * needleUI) :
	sequence(_sequence),
    needle(needleUI),
    cueManagerUI(cueManagerUI),
	selectionZoomMode(false)
{
	sequence->addAsyncContainerListener(this);
	
	if (this->cueManagerUI == nullptr) this->cueManagerUI.reset(new TimeCueManagerUI(this, sequence->cueManager.get()));
	addAndMakeVisible(this->cueManagerUI.get());
	
	if (needle == nullptr) needle.reset(new TimeNeedleUI());
	addAndMakeVisible(needle.get());
	needle->setInterceptsMouseClicks(false, false);

	setSize(100, 20);
    startTimerHz(20);
}

SequenceTimelineHeader::~SequenceTimelineHeader()
{
	sequence->removeAsyncContainerListener(this);
}

#pragma warning(push)
#pragma warning(disable:4244)
void SequenceTimelineHeader::paint(Graphics & g)
{
	g.setColour(BG_COLOR.darker(.1f));
	g.fillRoundedRectangle(getLocalBounds().toFloat(), 2);

	g.setColour(BG_COLOR.brighter(.1f));
	g.fillRoundedRectangle(getLocalBounds().removeFromTop(getHeight()/2).toFloat(), 2);

	//Draw ticks
	float start = floorf(sequence->viewStartTime->floatValue());
	float end = floorf(sequence->viewEndTime->floatValue());

	if (end <= start) return;

	int minGap = 10;
	int fadeGap = 25;

	float secondGap = getWidth() / (end - start);
	float minuteGap = (getWidth() / (end - start)) * 60;

	bool showSeconds = minuteGap > minGap;
	
	int secondSteps = 1;
	int minuteSteps = 1;

	if (showSeconds)
	{
		while (secondGap < minGap)
		{
			secondSteps *= 2;
			secondGap = (getWidth() / (end - start))*secondSteps;
		}
	}
	else
	{
		while (minuteGap < minGap)
		{
			minuteSteps *= 2;
			minuteGap = ((getWidth() / (end - start)) * 60)*minuteSteps;
		}
	}
	
	int minuteStartTime = floor((start / minuteSteps) / 60)*minuteSteps;
	int minuteEndTime = ceil((end / minuteSteps) / 60)*minuteSteps;

	g.setFont(12);
	float fadeAlpha = jlimit<float>(0, 1, jmap<float>(secondGap, minGap, fadeGap, 0, 1));


	for (float i = minuteStartTime; i <= minuteEndTime; i += minuteSteps)
	{
		int mtx = getXForTime(i*60);

		//Draw minute
		g.setColour(BG_COLOR.brighter(.6f));
		//g.drawLine(tx, 0, tx, getHeight(), 1);
		g.drawVerticalLine(mtx, 0, (float)getHeight());
		g.setColour(BG_COLOR.darker(.6f));
		g.drawRoundedRectangle(getLocalBounds().toFloat(), 2, 2);

		g.setColour(BG_COLOR.brighter(.7f));
		g.fillRoundedRectangle(mtx - 10, 0, 20, 14, 2);
		g.setColour(BG_COLOR.darker());
		g.drawText(String(i) + "'", mtx - 10, 2, 20, 14, Justification::centred);

		if (showSeconds)
		{
			int sIndex = 0;
			for (int s = secondSteps; s < 60 && i*60+s <= end; s += secondSteps)
			{
				int stx = getXForTime(i * 60 + s);

				float alpha = 1;
				if (sIndex % 2 == 0) alpha = fadeAlpha;
				g.setColour(BG_COLOR.brighter(.1f).withAlpha(alpha));
				//g.drawLine(tx, 0, tx, getHeight(), 1);
				g.drawVerticalLine(stx, 0, (float)getHeight());
				g.setColour(BG_COLOR.brighter(.5f).withAlpha(alpha));
				g.drawText(String(s), stx - 10, 2, 20, 14, Justification::centred);

				//show subsecond ?
				/*
				g.setColour(BG_COLOR);
				g.drawVerticalLine(tx, 0, (float)getHeight());
				*/

				sIndex++;
				
			}
		}
	}

	g.setColour(BG_COLOR.brighter(.7f));
	g.fillRoundedRectangle(0, 0, 20, 14, 2);
	g.setColour(BG_COLOR.darker());
	g.drawText(String(floor(start / 60)) + "'", 2, 2, 18, 14, Justification::centred);

	if (!selectionSpan.isOrigin())
	{
		Colour c = (selectionZoomMode ? BLUE_COLOR : GREEN_COLOR);
		float minPos = jmin(getXForTime(selectionSpan.x), getXForTime(selectionSpan.y));
		float maxPos = jmax(getXForTime(selectionSpan.x), getXForTime(selectionSpan.y));
		Rectangle<int> sr = Rectangle<int>(minPos,0, maxPos-minPos,getHeight()).reduced(1);
		g.setColour(c.withAlpha(.3f));
		g.fillRect(sr);
		g.setColour(c.withAlpha(.7f));
		g.drawRect(sr);
	}

	g.setColour(BG_COLOR.darker(.6f));
	g.drawRoundedRectangle(getLocalBounds().toFloat(), 2, 2);

}

void SequenceTimelineHeader::resized()
{
	cueManagerUI->setBounds(getLocalBounds().removeFromBottom(getHeight()/2));
	updateNeedlePosition();
	
}

void SequenceTimelineHeader::updateNeedlePosition()
{
	Rectangle<int> nr = getLocalBounds().withSize(7, getHeight());
	nr.setPosition(getXForTime(sequence->currentTime->floatValue()) - needle->getWidth() / 2, 0);
	needle->setBounds(nr);
}

#pragma warning(pop)

void SequenceTimelineHeader::mouseDown(const MouseEvent & e)
{
	if (e.mods.isRightButtonDown() || (e.mods.isLeftButtonDown() && e.mods.isShiftDown()))
	{
		float pos = getTimeForX(e.getPosition().x);
		selectionSpan.setXY(pos, pos);
		selectionZoomMode = e.mods.isRightButtonDown();

	}else if (e.mods.isLeftButtonDown())
	{
		sequence->setCurrentTime(getTimeForX(e.getPosition().x), true, true);	
	}
}

void SequenceTimelineHeader::mouseDrag(const MouseEvent & e)
{
	if(e.mods.isRightButtonDown() || (e.mods.isLeftButtonDown() && e.mods.isShiftDown()))
	{
		float pos = getTimeForX(e.getPosition().x);
		selectionSpan.setY(pos);
		repaint();
	}
	else if(e.mods.isLeftButtonDown())
	{
		sequence->setCurrentTime(getTimeForX(e.getPosition().x), true, true);
	}
}

void SequenceTimelineHeader::mouseDoubleClick(const MouseEvent & e)
{
	if (e.mods.isLeftButtonDown())
	{
		cueManagerUI->addCueAtPos(e.getMouseDownX());
	}
}

void SequenceTimelineHeader::mouseUp(const MouseEvent& e)
{
	if (e.mods.isRightButtonDown() || (e.mods.isLeftButtonDown() && e.mods.isShiftDown()))
	{
		if (e.mouseWasDraggedSinceMouseDown())
		{
			float minPos = jmin(selectionSpan.x, selectionSpan.y);
			float maxPos = jmax(selectionSpan.x, selectionSpan.y);

			if (selectionZoomMode)
			{
				sequence->viewStartTime->setValue(minPos);
				sequence->viewEndTime->setValue(maxPos);
			}
			else
			{
				PopupMenu m;
				m.addItem(1, "Select items in this range");
				m.addItem(2, "Remove items in this range");
				m.addSeparator();
				m.addItem(3, "Remove timespan");
				m.addItem(4, "Insert timespan");

				int result = m.show();
				switch (result)
				{
				case 1:
					sequence->selectAllItemsBetween(minPos, maxPos);
					break;

				case 2:
					sequence->removeAllItemsBetween(minPos, maxPos);
					break;

				case 3:
					sequence->removeTimespan(minPos, maxPos);
					break;

				case 4:
					sequence->insertTimespan(minPos, maxPos - minPos);
					break;
				}
			}

			selectionSpan.setXY(0, 0);
			repaint();
		}
	}
}

int SequenceTimelineHeader::getXForTime(float time)
{
	float viewStart = sequence->viewStartTime->floatValue();
	float viewEnd = sequence->viewEndTime->floatValue();
	if (viewStart == viewEnd) return 0;
	return (int)jmap<float>(time, viewStart, viewEnd, 0, (float)getWidth());
}

float SequenceTimelineHeader::getTimeForX(int tx)
{
	float viewStart = sequence->viewStartTime->floatValue();
	float viewEnd = sequence->viewEndTime->floatValue();
	return jmap<float>((float)tx, 0, (float)getWidth(), viewStart, viewEnd);
}

void SequenceTimelineHeader::newMessage(const ContainerAsyncEvent & e)
{
	switch (e.type)
	{
	case ContainerAsyncEvent::ControllableFeedbackUpdate:

		if (e.targetControllable == sequence->viewStartTime || e.targetControllable == sequence->viewEndTime)
		{
			repaint();
			resized();
			cueManagerUI->updateContent();
		} else if (e.targetControllable == sequence->currentTime)
		{
            shouldUpdateNeedle = true;
		} else if (e.targetControllable == sequence->totalTime)
		{
			resized();
		}
		break;
		
	default:
	//other events not handled 
	break;
	}
}

void SequenceTimelineHeader::timerCallback()
{
    if(shouldUpdateNeedle)
    {
        shouldUpdateNeedle = false;
        updateNeedlePosition();
    }
}


#pragma warning(push)
#pragma warning(disable:4244)
void TimeNeedleUI::paint(Graphics & g)
{
	g.setColour(HIGHLIGHT_COLOR);
	Path p;
	p.addTriangle(0, 0, getWidth(), 0, getWidth()/2.f, 4);
	p.addTriangle(0, getHeight(), getWidth() / 2, getHeight() - 4, getWidth(), getHeight());
	g.fillPath(p);
	g.drawVerticalLine(getWidth() / 2, 0, getHeight());
}
#pragma warning(pop)
