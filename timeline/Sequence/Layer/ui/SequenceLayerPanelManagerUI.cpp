#include "SequenceLayerPanelManagerUI.h"
/*
  ==============================================================================

    SequenceLayerPanelManagerUI.cpp
    Created: 28 Oct 2016 8:15:41pm
    Author:  bkupe

  ==============================================================================
*/

SequenceLayerPanelManagerUI::SequenceLayerPanelManagerUI(SequenceLayerManager * _manager) :
	BaseManagerUI<SequenceLayerManager, SequenceLayer, SequenceLayerPanel>("SequenceLayers", _manager)
{
	addExistingItems();
}

SequenceLayerPanelManagerUI::~SequenceLayerPanelManagerUI()
{
}



SequenceLayerPanel * SequenceLayerPanelManagerUI::createUIForItem(SequenceLayer * layer)
{
	return layer->getPanel();
}
