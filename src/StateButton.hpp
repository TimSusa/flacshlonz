//////////////////////////////////////////////////////////////////////
//!\file StateButton.hpp
//!\brief Multi state GUI Button.
//!\author timmsusa@googlemail.com
//////////////////////////////////////////////////////////////////////

#ifndef STATEBUTTON_HPP
#define STATEBUTTON_HPP

#include <QWidget>
#include <QPushButton>
#include <QStateMachine>
#include "States.hpp"

class StateButton : public QPushButton
{
    Q_OBJECT
public:
    explicit StateButton(QPushButton *parent = 0);

signals:
    void sigButtonClicked(states::eButtonState state);

public slots:
    void slotSetButtonState(states::eButtonState state);

private slots:
    void slotInternalButtonClicked(); // triggers sigButtonClicked with self state

private:
    states::eButtonState m_SelfState;
    void setSelfState(states::eButtonState state);
    void updateButtonTxt();
};

#endif // STATEBUTTON_HPP
