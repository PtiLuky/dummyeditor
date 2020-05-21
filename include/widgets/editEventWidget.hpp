#ifndef EDITEVENTWIDGET_HPP
#define EDITEVENTWIDGET_HPP

#include <QPushButton>
#include <memory>

#include "dummyrpg/dummy_types.hpp"
#include "editor/project.hpp"

namespace Editor {

class EditEventWidget : public QWidget
{
    Q_OBJECT
public:
    explicit EditEventWidget(QWidget* parent = nullptr);

    void setProject(std::shared_ptr<Project>);
    void setCurrentEvent(Dummy::char_id, Dummy::event_id);

private:
    std::shared_ptr<Project> m_loadedProject;
    std::unique_ptr<QWidget> m_content;
    Dummy::char_id m_currChar = Dummy::undefChar;

private slots:
    void setNewRoot(Dummy::EventType);

signals:
    void rootEventChanged(Dummy::event_id);
};

///////////////////////////////////////////////////////////////////////////////

class AddEventButton : public QPushButton
{
    Q_OBJECT
public:
    explicit AddEventButton(QWidget* parent = nullptr);

private:
    std::unique_ptr<QDialog> m_dlg;

private slots:
    void btnClicked();
    void btnNewDialogClicked();
    void btnNewChoiceClicked();

signals:
    void AddedEvent(Dummy::EventType);
};

///////////////////////////////////////////////////////////////////////////////

class DialogEventWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DialogEventWidget(std::shared_ptr<Project>, Dummy::event_id, QWidget* parent = nullptr);

    void setNextEvent(Dummy::event_id);

private:
    std::shared_ptr<Project> m_loadedProject;
    std::unique_ptr<QWidget> m_nextContent;
    Dummy::event_id m_id = Dummy::undefEvent;

private slots:
    void setNewNextEvent(Dummy::EventType);
    void currTextChanged(const QString&);
};

} // namespace Editor
#endif // EDITEVENTWIDGET_HPP
