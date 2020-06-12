#ifndef EDITEVENTWIDGET_HPP
#define EDITEVENTWIDGET_HPP

#include <QPushButton>
#include <memory>

#include "dummyrpg/dummy_types.hpp"
#include "editor/project.hpp"

namespace Editor {

class EventWidget;

class EditEventWidget : public QWidget
{
    Q_OBJECT
public:
    explicit EditEventWidget(QWidget* parent = nullptr);

    void setProject(std::shared_ptr<Project>);
    void setCurrentEvent(Dummy::char_id, Dummy::event_id);
    static EventWidget* createWidgetEvent(const Dummy::Event&, Dummy::char_id, Dummy::event_id,
                                          std::shared_ptr<Project>, QWidget* parent);

private:
    std::shared_ptr<Project> m_loadedProject;
    std::unique_ptr<QWidget> m_content;
    Dummy::char_id m_currChar = Dummy::undefChar;

private slots:
    void setNewRoot(Dummy::event_id);

signals:
    void rootEventChanged(Dummy::event_id);
};

///////////////////////////////////////////////////////////////////////////////

class AddEventButton : public QPushButton
{
    Q_OBJECT
public:
    explicit AddEventButton(Dummy::char_id, std::shared_ptr<Project>, QWidget* parent = nullptr);

private:
    std::unique_ptr<QDialog> m_dlg;
    std::shared_ptr<Project> m_loadedProject;
    Dummy::char_id m_currChar = Dummy::undefChar;

private slots:
    void btnClicked();
    void btnNewDialogClicked();
    void btnNewChoiceClicked();

signals:
    void AddedEvent(Dummy::event_id);
};

///////////////////////////////////////////////////////////////////////////////

class EventWidget : public QWidget
{
    Q_OBJECT
public:
    EventWidget(std::shared_ptr<Project>, Dummy::event_id, QWidget* parent = nullptr);

signals:
    void replaceBy(Dummy::event_id);

protected:
    std::shared_ptr<Project> m_loadedProject;
    Dummy::event_id m_id = Dummy::undefEvent;
};

///////////////////////////////////////////////////////////////////////////////

class DialogEventWidget : public EventWidget
{
    Q_OBJECT
public:
    explicit DialogEventWidget(std::shared_ptr<Project>, Dummy::event_id, QWidget* parent = nullptr);

private:
    std::unique_ptr<QWidget> m_nextContent;

private slots:
    void btnDeleteClicked();
    void setNextEvent(Dummy::event_id);
    void currTextChanged(const QString&);
};

///////////////////////////////////////////////////////////////////////////////

class ChoiceEventWidget : public EventWidget
{
    Q_OBJECT
public:
    explicit ChoiceEventWidget(std::shared_ptr<Project>, Dummy::char_id, Dummy::event_id, QWidget* parent = nullptr);

private:
    void updateOptionEvents();
    template <typename Func1, typename Func2, typename Func3>
    void createOptionWidget(std::unique_ptr<QWidget>& place, const Dummy::DialogChoice&, uint8_t optionIdx, Func1,
                            Func2, Func3);
    void activateOption(uint8_t choiceIdx, bool);
    void setNextEvent(uint8_t choiceIdx, Dummy::event_id next);
    void setText(uint8_t choiceIdx, const std::string&);

    std::unique_ptr<QWidget> m_optionContent1;
    std::unique_ptr<QWidget> m_optionContent2;
    std::unique_ptr<QWidget> m_optionContent3;
    std::unique_ptr<QWidget> m_optionContent4;
    Dummy::char_id m_charId = Dummy::undefChar;

private slots:
    void btnDeleteClicked();
    void questionChanged(const QString&);

    void option1Activated(bool);
    void option1Changed(const QString&);
    void setNextEvent1(Dummy::event_id);

    void option2Activated(bool);
    void option2Changed(const QString&);
    void setNextEvent2(Dummy::event_id);

    void option3Activated(bool);
    void option3Changed(const QString&);
    void setNextEvent3(Dummy::event_id);

    void option4Activated(bool);
    void setNextEvent4(Dummy::event_id);
    void option4Changed(const QString&);
};

} // namespace Editor
#endif // EDITEVENTWIDGET_HPP
