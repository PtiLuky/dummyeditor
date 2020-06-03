#include "widgets/editEventWidget.hpp"

#include <QCheckBox>
#include <QDialog>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMessageBox>

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
        auto btn = std::make_unique<AddEventButton>(m_currChar, m_loadedProject, this);
        connect(btn.get(), &AddEventButton::AddedEvent, this, &EditEventWidget::setNewRoot);
        m_content = std::move(btn);
        layout()->addWidget(m_content.get());
    } else {
        m_content.reset(createWidgetEvent(*e, m_currChar, id, m_loadedProject, this));
    }
}

QWidget* EditEventWidget::createWidgetEvent(const Dummy::Event& e, Dummy::char_id charId, Dummy::event_id id,
                                            std::shared_ptr<Project> p, QWidget* parent)
{
    QWidget* ptr = nullptr;
    if (e.type == Dummy::EventType::Dialog) {
        ptr = new DialogEventWidget(p, id, parent);
    } else if (e.type == Dummy::EventType::Choice) {
        ptr = new ChoiceEventWidget(p, charId, id, parent);
    }

    if (ptr != nullptr)
        parent->layout()->addWidget(ptr);

    return ptr;
}

void EditEventWidget::setNewRoot(Dummy::event_id id)
{
    setCurrentEvent(m_currChar, id);
    emit rootEventChanged(id);
}

///////////////////////////////////////////////////////////////////////////////

AddEventButton::AddEventButton(Dummy::char_id charId, std::shared_ptr<Project> p, QWidget* parent)
    : QPushButton(parent)
    , m_dlg(std::make_unique<QDialog>(this))
    , m_loadedProject(p)
    , m_currChar(charId)
{
    setText(tr("Add Event"));
    connect(this, &QPushButton::clicked, this, &AddEventButton::btnClicked);

    QPushButton* addDialogButton = new QPushButton(tr("Add dialog"), m_dlg.get());
    QPushButton* addChoiceButton = new QPushButton(tr("Add choice"), m_dlg.get());

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

    if (m_loadedProject == nullptr)
        return;

    auto id = m_loadedProject->game().registerDialog(m_currChar, "");
    m_loadedProject->changed();

    emit AddedEvent(id);
}
void AddEventButton::btnNewChoiceClicked()
{
    m_dlg->accept();

    if (m_loadedProject == nullptr)
        return;

    auto id = m_loadedProject->game().registerChoice("");
    auto* c = m_loadedProject->game().choice(id);
    if (c == nullptr)
        return;
    c->addOption(Dummy::DialogOption());
    c->addOption(Dummy::DialogOption());
    m_loadedProject->changed();

    emit AddedEvent(id);
}

///////////////////////////////////////////////////////////////////////////////

DialogEventWidget::DialogEventWidget(std::shared_ptr<Project> p, Dummy::event_id id, QWidget* parent)
    : QWidget(parent)
    , m_loadedProject(p)
    , m_id(id)
{
    const auto* d = m_loadedProject ? m_loadedProject->game().dialog(m_id) : nullptr;
    const auto* c = d != nullptr ? m_loadedProject->game().character(d->speakerId()) : nullptr;
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
    auto* d = m_loadedProject ? m_loadedProject->game().dialog(m_id) : nullptr;
    if (d == nullptr)
        return;

    // Set data
    d->setNextEvent(id);

    // update view
    const auto* nextE = m_loadedProject->game().event(id);
    if (nextE == nullptr) {
        auto btn = std::make_unique<AddEventButton>(d->speakerId(), m_loadedProject, this);
        connect(btn.get(), &AddEventButton::AddedEvent, this, &DialogEventWidget::setNextEvent);
        m_nextContent = std::move(btn);
        layout()->addWidget(m_nextContent.get());

    } else {
        m_nextContent.reset(EditEventWidget::createWidgetEvent(*nextE, d->speakerId(), id, m_loadedProject, this));
    }
}

void DialogEventWidget::currTextChanged(const QString& txt)
{
    auto* d = m_loadedProject ? m_loadedProject->game().dialog(m_id) : nullptr;
    if (d == nullptr)
        return;
    d->setSentence(txt.toStdString());
}

///////////////////////////////////////////////////////////////////////////////

ChoiceEventWidget::ChoiceEventWidget(std::shared_ptr<Project> p, Dummy::char_id charId, Dummy::event_id id,
                                     QWidget* parent)
    : QWidget(parent)
    , m_loadedProject(p)
    , m_id(id)
    , m_charId(charId)
{
    const auto* c = m_loadedProject ? m_loadedProject->game().choice(m_id) : nullptr;
    if (c == nullptr)
        return;

    QWidget* textLine = new QWidget(this);
    textLine->setLayout(new QHBoxLayout());
    textLine->layout()->setMargin(0);

    QLabel* lbl = new QLabel(tr("Question: "), textLine);
    textLine->layout()->addWidget(lbl);

    QLineEdit* input = new QLineEdit(QString::fromStdString(c->question()), textLine);
    connect(input, &QLineEdit::textChanged, this, &ChoiceEventWidget::questionChanged);
    textLine->layout()->addWidget(input);

    setLayout(new QVBoxLayout());
    layout()->addWidget(textLine);
    layout()->setMargin(0);

    updateOptionEvents();
}

void ChoiceEventWidget::updateOptionEvents()
{
    const auto* c = m_loadedProject ? m_loadedProject->game().choice(m_id) : nullptr;
    if (c == nullptr)
        return;

    // Option1
    createOptionWidget(m_optionContent1, *c, 0,              //
                       &ChoiceEventWidget::option1Activated, //
                       &ChoiceEventWidget::option1Changed,   //
                       &ChoiceEventWidget::setNextEvent1);
    layout()->addWidget(m_optionContent1.get());

    // Option2
    createOptionWidget(m_optionContent2, *c, 1,              //
                       &ChoiceEventWidget::option2Activated, //
                       &ChoiceEventWidget::option2Changed,   //
                       &ChoiceEventWidget::setNextEvent2);
    layout()->addWidget(m_optionContent2.get());

    // Option3
    createOptionWidget(m_optionContent3, *c, 2,              //
                       &ChoiceEventWidget::option3Activated, //
                       &ChoiceEventWidget::option3Changed,   //
                       &ChoiceEventWidget::setNextEvent3);
    layout()->addWidget(m_optionContent3.get());

    // Option4
    createOptionWidget(m_optionContent4, *c, 3,              //
                       &ChoiceEventWidget::option4Activated, //
                       &ChoiceEventWidget::option4Changed,   //
                       &ChoiceEventWidget::setNextEvent4);
    layout()->addWidget(m_optionContent4.get());
}

template <typename Func1, typename Func2, typename Func3>
void ChoiceEventWidget::createOptionWidget(std::unique_ptr<QWidget>& place, const Dummy::DialogChoice& choice,
                                           uint8_t optionIdx, Func1 slotBox, Func2 slotTxt, Func3 slotNext)
{
    const int LEFT_MARG = 40;
    place.reset(new QWidget(this));
    place->setLayout(new QVBoxLayout());
    place->layout()->setContentsMargins(LEFT_MARG, 0, 0, 0);

    // fetch option object if existing
    Dummy::DialogOption option;
    auto nextId = Dummy::undefEvent;
    if (optionIdx < choice.nbOptions()) {
        option = choice.optionAt(optionIdx);
        nextId = option.nextEvent;
    }

    // Prepare first line : box + label + question input
    QWidget* optionTxt = new QWidget(place.get());
    optionTxt->setLayout(new QHBoxLayout());
    optionTxt->layout()->setMargin(0);
    // checkbox
    QCheckBox* box = new QCheckBox(place.get());
    box->setChecked(optionIdx < choice.nbOptions());
    bool canUncheck = (optionIdx < choice.nbOptions()) && (choice.nbOptions() > 1);
    bool canCheck   = (optionIdx == choice.nbOptions()) && (optionIdx != Dummy::DialogChoice::NB_OPTIONS_MAX);
    box->setEnabled(canCheck || canUncheck);
    connect(box, &QCheckBox::clicked, this, slotBox);
    optionTxt->layout()->addWidget(box);
    // label
    optionTxt->layout()->addWidget(new QLabel(tr("Option %1:").arg(optionIdx + 1), place.get()));
    place->layout()->addWidget(optionTxt);
    // option text
    QLineEdit* input = new QLineEdit(QString::fromStdString(option.option), place.get());
    input->setEnabled(optionIdx < choice.nbOptions());
    connect(input, &QLineEdit::textChanged, this, slotTxt);
    optionTxt->layout()->addWidget(input);

    // Second line : "Next Event" part
    const auto* nextEvent = m_loadedProject->game().event(nextId);
    if (nextEvent == nullptr) {
        auto* btn = new AddEventButton(m_charId, m_loadedProject, place.get());
        connect(btn, &AddEventButton::AddedEvent, this, slotNext);
        btn->setEnabled(optionIdx < choice.nbOptions());
        place->layout()->addWidget(btn);
    } else {
        EditEventWidget::createWidgetEvent(*nextEvent, m_charId, nextId, m_loadedProject, place.get());
    }
}

void ChoiceEventWidget::activateOption(uint8_t choiceIdx, bool activate)
{
    auto* c = m_loadedProject ? m_loadedProject->game().choice(m_id) : nullptr;
    if (c == nullptr)
        return;

    if (activate && choiceIdx >= c->nbOptions())
        c->addOption(Dummy::DialogOption());
    else if (! activate && choiceIdx < c->nbOptions()) {
        auto answer = QMessageBox::question(
            this, tr("Confirmation"), tr("You are about to delete an option and all following events. Are you sure?"),
            QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
        if (answer == QMessageBox::Ok)
            c->removeOption(choiceIdx);
    }

    m_loadedProject->changed();
    updateOptionEvents();
}

void ChoiceEventWidget::setNextEvent(uint8_t choiceIdx, Dummy::event_id next)
{
    auto* c = m_loadedProject ? m_loadedProject->game().choice(m_id) : nullptr;
    if (c == nullptr)
        return;
    auto opt      = c->optionAt(choiceIdx);
    opt.nextEvent = next;
    c->setOption(opt, choiceIdx);
    m_loadedProject->changed();
    updateOptionEvents();
}

void ChoiceEventWidget::setText(uint8_t choiceIdx, const std::string& txt)
{
    auto* c = m_loadedProject ? m_loadedProject->game().choice(m_id) : nullptr;
    if (c == nullptr)
        return;
    auto opt   = c->optionAt(choiceIdx);
    opt.option = txt;
    c->setOption(opt, choiceIdx);
    m_loadedProject->changed();
}

void ChoiceEventWidget::questionChanged(const QString& txt)
{
    auto* c = m_loadedProject ? m_loadedProject->game().choice(m_id) : nullptr;
    if (c == nullptr)
        return;
    c->setQuestion(txt.toStdString());
    m_loadedProject->changed();
}

void ChoiceEventWidget::option1Activated(bool activ)
{
    activateOption(0, activ);
}
void ChoiceEventWidget::option1Changed(const QString& txt)
{
    setText(0, txt.toStdString());
}
void ChoiceEventWidget::setNextEvent1(Dummy::event_id id)
{
    setNextEvent(0, id);
}

void ChoiceEventWidget::option2Activated(bool activ)
{
    activateOption(1, activ);
}
void ChoiceEventWidget::option2Changed(const QString& txt)
{
    setText(1, txt.toStdString());
}
void ChoiceEventWidget::setNextEvent2(Dummy::event_id id)
{
    setNextEvent(1, id);
}

void ChoiceEventWidget::option3Activated(bool activ)
{
    activateOption(2, activ);
}
void ChoiceEventWidget::option3Changed(const QString& txt)
{
    setText(2, txt.toStdString());
}
void ChoiceEventWidget::setNextEvent3(Dummy::event_id id)
{
    setNextEvent(2, id);
}

void ChoiceEventWidget::option4Activated(bool activ)
{
    activateOption(3, activ);
}
void ChoiceEventWidget::option4Changed(const QString& txt)
{
    setText(3, txt.toStdString());
}
void ChoiceEventWidget::setNextEvent4(Dummy::event_id id)
{
    setNextEvent(3, id);
}
} // namespace Editor
