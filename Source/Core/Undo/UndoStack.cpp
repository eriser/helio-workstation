/*
    This file is part of Helio Workstation.

    Helio is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Helio is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Helio. If not, see <http://www.gnu.org/licenses/>.
*/

#include "Common.h"
#include "UndoStack.h"
#include "UndoAction.h"
#include "SerializationKeys.h"

#include "ProjectTreeItem.h"

#include "PianoLayerTreeItemActions.h"
#include "AutoLayerTreeItemActions.h"
#include "LayerTreeItemActions.h"
#include "MidiLayerActions.h"
#include "NoteActions.h"
#include "AnnotationEventActions.h"
#include "AutomationEventActions.h"
#include "TimeSignatureEventActions.h"

#define MAX_TRANSACTIONS_TO_STORE 10


struct UndoStack::ActionSet
{
    ActionSet (ProjectTreeItem &parentProject, String  transactionName) :
    project(parentProject),
    name(std::move(transactionName))
    {}
    
    bool perform() const
    {
        for (int i = 0; i < actions.size(); ++i) {
            if (! actions.getUnchecked(i)->perform()) {
                return false;
            }
        }
        
        return true;
    }
    
    bool undo() const
    {
        //Logger::writeToLog(String(actions.size()) + " actions");
        
        for (int i = actions.size(); --i >= 0;) {
            if (! actions.getUnchecked(i)->undo()) {
                return false;
            }
        }
        
        return true;
    }
    
    int getTotalSize() const
    {
        int total = 0;
        
        for (int i = actions.size(); --i >= 0;) {
            total += actions.getUnchecked(i)->getSizeInUnits();
        }
        
        return total;
    }
    
    XmlElement *serialize() const
    {
        auto xml = new XmlElement(Serialization::Undo::transaction);
        
        xml->setAttribute(Serialization::Undo::name, this->name);
        
        for (int i = 0; i < this->actions.size(); ++i)
        {
            // prependChildElement здесь не даст особого выигрыша (в большинстве транзакций только одно событие), а только запутает
            xml->addChildElement(this->actions.getUnchecked(i)->serialize());
        }
        
        return xml;
    }
    
    void deserialize(const XmlElement &xml)
    {
        this->reset();
        
        this->name = xml.getStringAttribute(Serialization::Undo::name);
        
        forEachXmlChildElement(xml, childActionXml)
        {
            UndoAction *action = createUndoActionsByTagName(childActionXml->getTagName());
            action->deserialize(*childActionXml);
            this->actions.add(action);
        }
    }
    
    void reset()
    {
        this->actions.clear();
    }
    
    UndoAction *createUndoActionsByTagName(const String &tagName)
    {
        if      (tagName == Serialization::Undo::pianoLayerTreeItemInsertAction)        { return new PianoLayerTreeItemInsertAction(this->project); }
        else if (tagName == Serialization::Undo::pianoLayerTreeItemRemoveAction)        { return new PianoLayerTreeItemRemoveAction(this->project); }
        else if (tagName == Serialization::Undo::autoLayerTreeItemInsertAction)         { return new AutoLayerTreeItemInsertAction(this->project); }
        else if (tagName == Serialization::Undo::autoLayerTreeItemRemoveAction)         { return new AutoLayerTreeItemRemoveAction(this->project); }
        else if (tagName == Serialization::Undo::layerTreeItemRenameAction)             { return new LayerTreeItemRenameAction(this->project); }
        else if (tagName == Serialization::Undo::midiLayerChangeColourAction)           { return new MidiLayerChangeColourAction(this->project); }
        else if (tagName == Serialization::Undo::midiLayerChangeInstrumentAction)       { return new MidiLayerChangeInstrumentAction(this->project); }
        else if (tagName == Serialization::Undo::midiLayerMuteAction)                   { return new MidiLayerMuteAction(this->project); }
        else if (tagName == Serialization::Undo::noteInsertAction)                      { return new NoteInsertAction(this->project); }
        else if (tagName == Serialization::Undo::noteRemoveAction)                      { return new NoteRemoveAction(this->project); }
        else if (tagName == Serialization::Undo::noteChangeAction)                      { return new NoteChangeAction(this->project); }
        else if (tagName == Serialization::Undo::notesGroupInsertAction)                { return new NotesGroupInsertAction(this->project); }
        else if (tagName == Serialization::Undo::notesGroupRemoveAction)                { return new NotesGroupRemoveAction(this->project); }
        else if (tagName == Serialization::Undo::notesGroupChangeAction)                { return new NotesGroupChangeAction(this->project); }
        else if (tagName == Serialization::Undo::annotationEventInsertAction)           { return new AnnotationEventInsertAction(this->project); }
        else if (tagName == Serialization::Undo::annotationEventRemoveAction)           { return new AnnotationEventRemoveAction(this->project); }
        else if (tagName == Serialization::Undo::annotationEventChangeAction)           { return new AnnotationEventChangeAction(this->project); }
        else if (tagName == Serialization::Undo::annotationEventsGroupInsertAction)     { return new AnnotationEventsGroupInsertAction(this->project); }
        else if (tagName == Serialization::Undo::annotationEventsGroupRemoveAction)     { return new AnnotationEventsGroupRemoveAction(this->project); }
        else if (tagName == Serialization::Undo::annotationEventsGroupChangeAction)     { return new AnnotationEventsGroupChangeAction(this->project); }
        else if (tagName == Serialization::Undo::timeSignatureEventInsertAction)        { return new TimeSignatureEventInsertAction(this->project); }
        else if (tagName == Serialization::Undo::timeSignatureEventRemoveAction)        { return new TimeSignatureEventRemoveAction(this->project); }
        else if (tagName == Serialization::Undo::timeSignatureEventChangeAction)        { return new TimeSignatureEventChangeAction(this->project); }
        else if (tagName == Serialization::Undo::timeSignatureEventsGroupInsertAction)  { return new TimeSignatureEventsGroupInsertAction(this->project); }
        else if (tagName == Serialization::Undo::timeSignatureEventsGroupRemoveAction)  { return new TimeSignatureEventsGroupRemoveAction(this->project); }
        else if (tagName == Serialization::Undo::timeSignatureEventsGroupChangeAction)  { return new TimeSignatureEventsGroupChangeAction(this->project); }
        else if (tagName == Serialization::Undo::automationEventInsertAction)           { return new AutomationEventInsertAction(this->project); }
        else if (tagName == Serialization::Undo::automationEventRemoveAction)           { return new AutomationEventRemoveAction(this->project); }
        else if (tagName == Serialization::Undo::automationEventChangeAction)           { return new AutomationEventChangeAction(this->project); }
        else if (tagName == Serialization::Undo::automationEventsGroupInsertAction)     { return new AutomationEventsGroupInsertAction(this->project); }
        else if (tagName == Serialization::Undo::automationEventsGroupRemoveAction)     { return new AutomationEventsGroupRemoveAction(this->project); }
        else if (tagName == Serialization::Undo::automationEventsGroupChangeAction)     { return new AutomationEventsGroupChangeAction(this->project); }
        
        jassertfalse;
        return nullptr;
    }
    
    OwnedArray<UndoAction> actions;
    String name;
    
    ProjectTreeItem &project;
};

//==============================================================================
UndoStack::UndoStack (ProjectTreeItem &parentProject,
                      const int maxNumberOfUnitsToKeep,
                      const int minimumTransactions) :
project(parentProject),
totalUnitsStored(0),
nextIndex(0),
newTransaction(true),
reentrancyCheck(false)
{
    setMaxNumberOfStoredUnits (maxNumberOfUnitsToKeep,
                               minimumTransactions);
}

UndoStack::~UndoStack()
{
}

//==============================================================================
void UndoStack::clearUndoHistory()
{
    transactions.clear();
    totalUnitsStored = 0;
    nextIndex = 0;
    sendChangeMessage();
}

int UndoStack::getNumberOfUnitsTakenUpByStoredCommands() const
{
    return totalUnitsStored;
}

void UndoStack::setMaxNumberOfStoredUnits (const int maxNumberOfUnitsToKeep,
                                           const int minimumTransactions)
{
    maxNumUnitsToKeep          = jmax (1, maxNumberOfUnitsToKeep);
    minimumTransactionsToKeep  = jmax (1, minimumTransactions);
}

//==============================================================================
bool UndoStack::perform (UndoAction* const newAction, const String& actionName)
{
    if (perform (newAction))
    {
        if (actionName.isNotEmpty()) {
            setCurrentTransactionName (actionName);
        }
        
        return true;
    }
    
    return false;
}

bool UndoStack::perform (UndoAction* const newAction)
{
    if (newAction != nullptr)
    {
        ScopedPointer<UndoAction> action (newAction);
        
        if (reentrancyCheck)
        {
            jassertfalse;  // don't call perform() recursively from the UndoAction::perform()
            // or undo() methods, or else these actions will be discarded!
            return false;
        }
        
        if (action->perform())
        {
            ActionSet* actionSet = getCurrentSet();
            
            //Logger::writeToLog("size before " + String(actionSet->actions.size()));
            
            if (actionSet != nullptr && ! newTransaction)
            {
                // здесь имеет смысл пробежаться по всему стеку, вызывая createCoalescedAction,
                // так как если в транзакции повторяются несколько разнородных событий,
                // то стек будет распухать
                for (signed int i = (actionSet->actions.size() - 1); i >= 0; --i)
                {
                    if (UndoAction *const lastAction = actionSet->actions[i])
                    {
                        if (UndoAction *const coalescedAction = lastAction->createCoalescedAction(action))
                        {
                            //Logger::writeToLog("createCoalescedAction");
                            action = coalescedAction;
                            totalUnitsStored -= lastAction->getSizeInUnits();
                            actionSet->actions.remove(i);
                            break;
                        }
                    }
                }
                
                //if (UndoAction* const lastAction = actionSet->actions.getLast())
                //{
                //    if (UndoAction* const coalescedAction = lastAction->createCoalescedAction (action))
                //    {
                //        Logger::writeToLog("remove last");
                //        action = coalescedAction;
                //        totalUnitsStored -= lastAction->getSizeInUnits();
                //        actionSet->actions.removeLast();
                //    }
                //}
            }
            else
            {
                actionSet = new ActionSet (this->project, newTransactionName);
                transactions.insert (nextIndex, actionSet);
                ++nextIndex;
            }
            
            totalUnitsStored += action->getSizeInUnits();
            actionSet->actions.add (action.release());
            newTransaction = false;
            //Logger::writeToLog("size " + String(actionSet->actions.size()));
            
            clearFutureTransactions();
            sendChangeMessage();
            return true;
        }
    }
    
    return false;
}

void UndoStack::clearFutureTransactions()
{
    while (nextIndex < transactions.size())
    {
        totalUnitsStored -= transactions.getLast()->getTotalSize();
        transactions.removeLast();
    }
    
    while (nextIndex > 0
           && totalUnitsStored > maxNumUnitsToKeep
           && transactions.size() > minimumTransactionsToKeep)
    {
        totalUnitsStored -= transactions.getFirst()->getTotalSize();
        transactions.remove (0);
        --nextIndex;
        
        // if this fails, then some actions may not be returning
        // consistent results from their getSizeInUnits() method
        jassert (totalUnitsStored >= 0);
    }
}

void UndoStack::beginNewTransaction() noexcept
{
    beginNewTransaction (String());
}

void UndoStack::beginNewTransaction (const String& actionName) noexcept
{
    newTransaction = true;
    newTransactionName = actionName;
}

void UndoStack::setCurrentTransactionName (const String& newName) noexcept
{
    if (newTransaction) {
        newTransactionName = newName;
    } else if (ActionSet* action = getCurrentSet()) {
        action->name = newName;
    }
}

//==============================================================================
UndoStack::ActionSet* UndoStack::getCurrentSet() const noexcept     { return transactions [nextIndex - 1]; }
UndoStack::ActionSet* UndoStack::getNextSet() const noexcept        { return transactions [nextIndex]; }

bool UndoStack::canUndo() const noexcept   { return getCurrentSet() != nullptr; }
bool UndoStack::canRedo() const noexcept   { return getNextSet()    != nullptr; }

bool UndoStack::undo()
{
    if (const ActionSet* const s = getCurrentSet())
    {
        const ScopedValueSetter<bool> setter (reentrancyCheck, true);
        
        if (s->undo()) {
            --nextIndex;
        } else {
            clearUndoHistory();
        }
        
        beginNewTransaction();
        sendChangeMessage();
        return true;
    }
    
    return false;
}

bool UndoStack::redo()
{
    if (const ActionSet* const s = getNextSet())
    {
        const ScopedValueSetter<bool> setter (reentrancyCheck, true);
        
        if (s->perform()) {
            ++nextIndex;
        } else {
            clearUndoHistory();
        }
        
        beginNewTransaction();
        sendChangeMessage();
        return true;
    }
    
    return false;
}

String UndoStack::getUndoDescription() const
{
    if (const ActionSet* const s = getCurrentSet()) {
        return s->name;
    }
    
    return String();
}

String UndoStack::getRedoDescription() const
{
    if (const ActionSet* const s = getNextSet()) {
        return s->name;
    }
    
    return String();
}

bool UndoStack::undoCurrentTransactionOnly()
{
    return newTransaction ? false : undo();
}

void UndoStack::getActionsInCurrentTransaction (Array<const UndoAction*>& actionsFound) const
{
    if (! newTransaction) {
        if (const ActionSet* const s = getCurrentSet()) {
            for (int i = 0; i < s->actions.size(); ++i) {
                actionsFound.add (s->actions.getUnchecked(i));
            }
        }
    }
}

int UndoStack::getNumActionsInCurrentTransaction() const
{
    if (! newTransaction) {
        if (const ActionSet* const s = getCurrentSet()) {
            return s->actions.size();
        }
    }
    
    return 0;
}


//===----------------------------------------------------------------------===//
// Serializable
//===----------------------------------------------------------------------===//

XmlElement *UndoStack::serialize() const
{
    auto xml = new XmlElement(Serialization::Undo::undoStack);
    
    int currentIndex = (this->nextIndex - 1);
    int numStoredTransactions = 0;
    
    while (currentIndex >= 0 &&
           numStoredTransactions < MAX_TRANSACTIONS_TO_STORE)
    {
        if (ActionSet *action = this->transactions[currentIndex])
        {
            xml->prependChildElement(action->serialize());
        }
        
        --currentIndex;
        ++numStoredTransactions;
    }
    
    return xml;
}

void UndoStack::deserialize(const XmlElement &xml)
{
    const XmlElement *root = (xml.getTagName() == Serialization::Undo::undoStack) ?
    &xml : xml.getChildByName(Serialization::Undo::undoStack);
    
    if (root == nullptr)
    { return; }
    
    this->reset();
    
    forEachXmlChildElement(*root, childTransactionXml)
    {
        auto actionSet = new ActionSet(this->project, String::empty);
        actionSet->deserialize(*childTransactionXml);
        this->transactions.insert(this->nextIndex, actionSet);
        ++this->nextIndex;
    }
}

void UndoStack::reset()
{
    this->clearUndoHistory();
}
