//////////////////////////////////////////////////////////////////////
//!\file StateButton.hpp
//!\brief Multi state GUI Button.
//!\author timmsusa@googlemail.com
//////////////////////////////////////////////////////////////////////

#include "StateButton.hpp"

StateButton::StateButton(QPushButton *parent) :
    QPushButton(parent),
    m_SelfState(states::BUTTON_NONE)
{
    connect(this, SIGNAL(clicked()), this, SLOT(slotInternalButtonClicked()));
}

void StateButton::slotSetButtonState(states::eButtonState state)
{
    setSelfState(state);
}

void StateButton::slotInternalButtonClicked()
{
    emit sigButtonClicked(m_SelfState);
}

void StateButton::setSelfState(states::eButtonState state)
{
    m_SelfState = state;
    updateButtonTxt();
}

void StateButton::updateButtonTxt()
{
    QString txt="";
    switch(m_SelfState)
    {
    case states::BUTTON_SET_PATH:
        txt = tr("Set Path");
        break;

    case states::BUTTON_STOP:
        txt = tr("Stop");
        break;

    case states::BUTTON_UPDATE:
        txt = tr("Update");
        break;
    default:
        txt = tr("INVALID STATE");
        break;
    }

    this->setText(txt);
}
