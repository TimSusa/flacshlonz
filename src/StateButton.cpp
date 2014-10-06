#include "StateButton.hpp"

StateButton::StateButton(QPushButton *parent) :
    QPushButton(parent),
    //m_Button(),
    //m_StateMachine()
    m_SelfState(states::BUTTON_NONE)
{
    //this->setText("Update");
    connect(this, SIGNAL(clicked()), this, SLOT(slotInternalButtonClicked()));
    /*
    QState *stateUpdate = new QState();
    stateUpdate->assignProperty(this, "text", "Update");
    stateUpdate->setObjectName("update");

    QState *stateStopped = new QState();
    stateStopped->setObjectName("stop");
    stateStopped->assignProperty(this, "text", "Stop Scan");

    stateUpdate->addTransition(this, SIGNAL(clicked()), stateStopped);
    stateStopped->addTransition(this, SIGNAL(clicked()), stateUpdate);
    connect(this, SIGNAL(clicked(bool)), this, SIGNAL(sigButtonClicked(bool)));

    m_StateMachine.addState(stateUpdate);
    m_StateMachine.addState(stateStopped);
    m_StateMachine.setInitialState(stateUpdate);
    m_StateMachine.start();

   // m_Button.resize(100, 50);
    // m_Button.show();
    */
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
