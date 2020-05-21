#include "widgets/editEventWidget.hpp"

#include <QDialog>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>

namespace Editor {


EditEventWidget::EditEventWidget(QWidget* parent)
    : QWidget(parent)
{
    setLayout(new QVBoxLayout(this));
}

void EditEventWidget::setProject(std::shared_ptr<Project> p)
{
    m_loadedProject = p;
}

void EditEventWidget::setCurrentEvent(Dummy::char_id charId, Dummy::event_id id)
{
    if (m_loadedProject == nullptr)
        return;

    m_currChar = charId;

    auto* e = m_loadedProject->game().event(id);
    if (e == nullptr) {
        m_content = std::make_unique<AddEventButton>(this);
        connect(reinterpret_cast<AddEventButton*>(m_content.get()), &AddEventButton::AddedEvent, //
                this, &EditEventWidget::setNewRoot);
        layout()->addWidget(m_content.get());

    } else if (e->type == Dummy::EventType::Dialog) {
        m_content = std::make_unique<DialogEventWidget>(m_loadedProject, id, this);
        layout()->addWidget(m_content.get());

    } else if (e->type == Dummy::EventType::Choice) {
    }
}

void EditEventWidget::setNewRoot(Dummy::EventType type)
{
    if (m_loadedProject == nullptr)
        return;

    auto id = Dummy::undefEvent;
    if (type == Dummy::EventType::Dialog) {
        id = m_loadedProject->game().registerDialog(m_currChar, "");
        m_loadedProject->changed();

    } else if (type == Dummy::EventType::Choice) {
        id = m_loadedProject->game().registerChoice("");
        m_loadedProject->changed();
    }

    setCurrentEvent(m_currChar, id);
    emit rootEventChanged(id);
}

///////////////////////////////////////////////////////////////////////////////

AddEventButton::AddEventButton(QWidget* parent)
    : QPushButton(parent)
    , m_dlg(std::make_unique<QDialog>(this))
{
    setText(tr("Add Event"));
    connect(this, &QPushButton::clicked, this, &AddEventButton::btnClicked);

    QPushButton* addDialogButton = new QPushButton(tr("Add dialog"), m_dlg.get());
    QPushButton* addChoiceButton = new QPushButton(tr("Add choice"), m_dlg.get());
    addChoiceButton->setEnabled(false);

    connect(addDialogButton, &QPushButton::clicked, this, &AddEventButton::btnNewDialogClicked);
    connect(addChoiceButton, &QPushButton::clicked, this, &AddEventButton::btnNewChoiceClicked);

    m_dlg->setLayout(new QVBoxLayout);
    m_dlg->layout()->addWidget(addDialogButton);
    m_dlg->layout()->addWidget(addChoiceButton);
    m_dlg->setMinimumSize(250, 100);
}

void AddEventButton::btnClicked()
{
    m_dlg->exec();
}

void AddEventButton::btnNewDialogClicked()
{
    m_dlg->accept();
    emit AddedEvent(Dummy::EventType::Dialog);
}
void AddEventButton::btnNewChoiceClicked()
{
    m_dlg->accept();
    emit AddedEvent(Dummy::EventType::Choice);
}

///////////////////////////////////////////////////////////////////////////////

DialogEventWidget::DialogEventWidget(std::shared_ptr<Project> p, Dummy::event_id id, QWidget* parent)
    : QWidget(parent)
    , m_loadedProject(p)
    , m_id(id)
{
    if (m_loadedProject == nullptr)
        return;
    const auto* e = m_loadedProject->game().event(id);
    if (e == nullptr || e->type != Dummy::EventType::Dialog)
        return;
    const auto* d = m_loadedProject->game().dialog(e->idxPerType);
    if (d == nullptr)
        return;
    const auto* c = m_loadedProject->game().character(d->speakerId());
    if (c == nullptr)
        return;

    QWidget* textLine = new QWidget(this);
    textLine->setLayout(new QHBoxLayout());
    textLine->layout()->setMargin(0);

    QLabel* lbl = new QLabel(tr("%1 says: ").arg(QString::fromStdString(c->name())), textLine);
    textLine->layout()->addWidget(lbl);

    QLineEdit* input = new QLineEdit(QString::fromStdString(d->sentence()), textLine);
    connect(input, &QLineEdit::textChanged, this, &DialogEventWidget::currTextChanged);
    textLine->layout()->addWidget(input);

    setLayout(new QVBoxLayout());
    layout()->addWidget(textLine);
    layout()->setMargin(0);

    setNextEvent(d->nextEvent());
}

void DialogEventWidget::setNextEvent(Dummy::event_id id)
{
    auto* nextE = m_loadedProject->game().event(id);

    // Set data
    const auto* e = m_loadedProject->game().event(m_id);
    if (e == nullptr || e->type != Dummy::EventType::Dialog)
        return;
    auto* d = m_loadedProject->game().dialog(e->idxPerType);
    if (d == nullptr)
        return;
    d->setNextEvent(id);

    // update view
    if (nextE == nullptr) {
        m_nextContent = std::make_unique<AddEventButton>(this);
        connect(reinterpret_cast<AddEventButton*>(m_nextContent.get()), &AddEventButton::AddedEvent, //
                this, &DialogEventWidget::setNewNextEvent);
        layout()->addWidget(m_nextContent.get());

    } else if (nextE->type == Dummy::EventType::Dialog) {
        m_nextContent = std::make_unique<DialogEventWidget>(m_loadedProject, id, this);
        layout()->addWidget(m_nextContent.get());

    } else if (nextE->type == Dummy::EventType::Choice) {
    }
}

void DialogEventWidget::setNewNextEvent(Dummy::EventType type)
{
    if (m_loadedProject == nullptr)
        return;
    const auto* e = m_loadedProject->game().event(m_id);
    if (e == nullptr || e->type != Dummy::EventType::Dialog)
        return;
    const auto* d = m_loadedProject->game().dialog(e->idxPerType);
    if (d == nullptr)
        return;


    auto id = Dummy::undefEvent;
    if (type == Dummy::EventType::Dialog) {
        id = m_loadedProject->game().registerDialog(d->speakerId(), "");
        m_loadedProject->changed();

    } else if (type == Dummy::EventType::Choice) {
        id = m_loadedProject->game().registerChoice("");
        m_loadedProject->changed();
    }

    setNextEvent(id);
}

void DialogEventWidget::currTextChanged(const QString& txt)
{
    const auto* e = m_loadedProject->game().event(m_id);
    if (e == nullptr || e->type != Dummy::EventType::Dialog)
        return;
    auto* d = m_loadedProject->game().dialog(e->idxPerType);
    if (d == nullptr)
        return;
    d->setSentence(txt.toStdString());
}

} // namespace Editor
