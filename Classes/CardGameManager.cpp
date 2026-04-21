#include "CardGameManager.h"

#include <algorithm>
#include <cstdlib>

CardGameManager::CardGameManager()
    : _nextCardId(1)
    , _handTopCardId(0)
{
}

void CardGameManager::setupDemo()
{
    _nextCardId = 1;
    _handTopCardId = 0;
    _cards.clear();
    _tableCardIds.clear();
    _stockCardIds.clear();
    _handCardIds.clear();
    _history.clear();

    CardData leftTop = createCard(3, CardSuit::Club, CardZone::Table, true, 0, 0);
    CardData leftMiddle = createCard(4, CardSuit::Diamond, CardZone::Table, false, 0, 1);
    CardData leftBottom = createCard(5, CardSuit::Heart, CardZone::Table, false, 0, 2);

    CardData centerTop = createCard(4, CardSuit::Spade, CardZone::Table, true, 1, 0);
    CardData centerMiddle = createCard(3, CardSuit::Heart, CardZone::Table, false, 1, 1);
    CardData centerBottom = createCard(2, CardSuit::Club, CardZone::Table, false, 1, 2);

    CardData rightTop = createCard(3, CardSuit::Diamond, CardZone::Table, true, 2, 0);
    CardData rightMiddle = createCard(4, CardSuit::Club, CardZone::Table, false, 2, 1);
    CardData rightBottom = createCard(5, CardSuit::Spade, CardZone::Table, false, 2, 2);

    leftMiddle.blockedBy.push_back(leftTop.id);
    leftBottom.blockedBy.push_back(leftMiddle.id);
    centerMiddle.blockedBy.push_back(centerTop.id);
    centerBottom.blockedBy.push_back(centerMiddle.id);
    rightMiddle.blockedBy.push_back(rightTop.id);
    rightBottom.blockedBy.push_back(rightMiddle.id);

    _cards[leftTop.id] = leftTop;
    _cards[leftMiddle.id] = leftMiddle;
    _cards[leftBottom.id] = leftBottom;
    _cards[centerTop.id] = centerTop;
    _cards[centerMiddle.id] = centerMiddle;
    _cards[centerBottom.id] = centerBottom;
    _cards[rightTop.id] = rightTop;
    _cards[rightMiddle.id] = rightMiddle;
    _cards[rightBottom.id] = rightBottom;

    _tableCardIds.push_back(leftTop.id);
    _tableCardIds.push_back(leftMiddle.id);
    _tableCardIds.push_back(leftBottom.id);
    _tableCardIds.push_back(centerTop.id);
    _tableCardIds.push_back(centerMiddle.id);
    _tableCardIds.push_back(centerBottom.id);
    _tableCardIds.push_back(rightTop.id);
    _tableCardIds.push_back(rightMiddle.id);
    _tableCardIds.push_back(rightBottom.id);

    CardData stock6 = createCard(6, CardSuit::Heart, CardZone::Stock, false, 0, 0);
    CardData stock7 = createCard(7, CardSuit::Spade, CardZone::Stock, false, 0, 1);
    CardData stock8 = createCard(8, CardSuit::Diamond, CardZone::Stock, false, 0, 2);
    _cards[stock6.id] = stock6;
    _cards[stock7.id] = stock7;
    _cards[stock8.id] = stock8;
    _stockCardIds.push_back(stock6.id);
    _stockCardIds.push_back(stock7.id);
    _stockCardIds.push_back(stock8.id);

    CardData handTop = createCard(4, CardSuit::Club, CardZone::Hand, true, 0, 0);
    _cards[handTop.id] = handTop;
    _handCardIds.push_back(handTop.id);
    _handTopCardId = handTop.id;
}

const std::vector<int>& CardGameManager::getTableCardIds() const
{
    return _tableCardIds;
}

const std::vector<int>& CardGameManager::getStockCardIds() const
{
    return _stockCardIds;
}

const std::vector<int>& CardGameManager::getHandCardIds() const
{
    return _handCardIds;
}

const CardData* CardGameManager::getCard(int cardId) const
{
    auto it = _cards.find(cardId);
    if (it == _cards.end())
    {
        return nullptr;
    }

    return &it->second;
}

CardData* CardGameManager::getCard(int cardId)
{
    auto it = _cards.find(cardId);
    if (it == _cards.end())
    {
        return nullptr;
    }

    return &it->second;
}

int CardGameManager::getHandTopCardId() const
{
    return _handTopCardId;
}

int CardGameManager::getRemainingStockCount() const
{
    int count = 0;
    for (int cardId : _stockCardIds)
    {
        const CardData* card = getCard(cardId);
        if (card && !card->removed)
        {
            ++count;
        }
    }

    return count;
}

int CardGameManager::getNextStockCardId() const
{
    for (int cardId : _stockCardIds)
    {
        const CardData* card = getCard(cardId);
        if (card && !card->removed)
        {
            return cardId;
        }
    }

    return 0;
}

bool CardGameManager::canMoveTableCardToHand(int cardId) const
{
    const CardData* card = getCard(cardId);
    const CardData* handTopCard = getCard(_handTopCardId);
    if (!card || !handTopCard)
    {
        return false;
    }

    if (!isTableCardSelectable(*card))
    {
        return false;
    }

    return isAdjacentValue(card->value, handTopCard->value);
}

bool CardGameManager::canSelectTableCard(int cardId) const
{
    const CardData* card = getCard(cardId);
    if (!card)
    {
        return false;
    }

    return isTableCardSelectable(*card);
}

MoveResult CardGameManager::moveTableCardToHand(int cardId)
{
    MoveResult result;
    if (!canMoveTableCardToHand(cardId))
    {
        return result;
    }

    CardData* card = getCard(cardId);
    result.success = true;
    result.type = MoveType::TableToHand;
    result.movedCardId = cardId;
    result.previousHandTopCardId = _handTopCardId;

    moveCardToHand(*card);
    result.flippedCardIds = collectCardsToFlip();

    for (int flippedCardId : result.flippedCardIds)
    {
        CardData* flippedCard = getCard(flippedCardId);
        if (flippedCard)
        {
            flippedCard->faceUp = true;
        }
    }

    MoveRecord record;
    record.type = result.type;
    record.movedCardId = result.movedCardId;
    record.previousHandTopCardId = result.previousHandTopCardId;
    record.flippedCardIds = result.flippedCardIds;
    _history.push_back(record);

    return result;
}

bool CardGameManager::canDrawFromStock() const
{
    return getNextStockCardId() != 0;
}

MoveResult CardGameManager::drawFromStock()
{
    MoveResult result;
    int drawCardId = getNextStockCardId();
    if (drawCardId == 0)
    {
        return result;
    }

    CardData* card = getCard(drawCardId);
    if (!card)
    {
        return result;
    }

    result.success = true;
    result.type = MoveType::StockToHand;
    result.movedCardId = drawCardId;
    result.previousHandTopCardId = _handTopCardId;

    moveCardToHand(*card);

    MoveRecord record;
    record.type = result.type;
    record.movedCardId = result.movedCardId;
    record.previousHandTopCardId = result.previousHandTopCardId;
    _history.push_back(record);

    return result;
}

bool CardGameManager::canUndo() const
{
    return !_history.empty();
}

UndoResult CardGameManager::undo()
{
    UndoResult result;
    if (_history.empty())
    {
        return result;
    }

    MoveRecord record = _history.back();
    _history.pop_back();

    CardData* card = getCard(record.movedCardId);
    if (!card)
    {
        return result;
    }

    if (!_handCardIds.empty() && _handCardIds.back() == card->id)
    {
        _handCardIds.pop_back();
    }
    else
    {
        std::vector<int>::iterator it = std::find(_handCardIds.begin(), _handCardIds.end(), card->id);
        if (it != _handCardIds.end())
        {
            _handCardIds.erase(it);
        }
    }

    card->removed = false;
    card->zone = (record.type == MoveType::TableToHand) ? CardZone::Table : CardZone::Stock;
    card->faceUp = (card->zone != CardZone::Stock);
    _handTopCardId = record.previousHandTopCardId;

    for (int cardId : record.flippedCardIds)
    {
        CardData* flippedCard = getCard(cardId);
        if (flippedCard)
        {
            flippedCard->faceUp = false;
            result.revertedFaceDownCardIds.push_back(cardId);
        }
    }

    result.success = true;
    result.undoneType = record.type;
    result.movedCardId = record.movedCardId;
    result.restoredHandTopCardId = record.previousHandTopCardId;
    return result;
}

CardData CardGameManager::createCard(int value, CardSuit suit, CardZone zone, bool faceUp, int column, int row) const
{
    CardData card;
    card.id = _nextCardId;
    card.value = value;
    card.suit = suit;
    card.zone = zone;
    card.faceUp = faceUp;
    card.removed = false;
    card.column = column;
    card.row = row;
    ++const_cast<CardGameManager*>(this)->_nextCardId;
    return card;
}

bool CardGameManager::isAdjacentValue(int lhs, int rhs) const
{
    return std::abs(lhs - rhs) == 1;
}

bool CardGameManager::isTableCardSelectable(const CardData& card) const
{
    if (card.zone != CardZone::Table || card.removed || !card.faceUp)
    {
        return false;
    }

    for (int blockerId : card.blockedBy)
    {
        const CardData* blocker = getCard(blockerId);
        if (blocker && !blocker->removed)
        {
            return false;
        }
    }

    return true;
}

std::vector<int> CardGameManager::collectCardsToFlip() const
{
    std::vector<int> flippedCardIds;

    for (int cardId : _tableCardIds)
    {
        const CardData* card = getCard(cardId);
        if (!card || card->removed || card->faceUp)
        {
            continue;
        }

        bool allBlockersRemoved = true;
        for (int blockerId : card->blockedBy)
        {
            const CardData* blocker = getCard(blockerId);
            if (blocker && !blocker->removed)
            {
                allBlockersRemoved = false;
                break;
            }
        }

        if (allBlockersRemoved)
        {
            flippedCardIds.push_back(cardId);
        }
    }

    return flippedCardIds;
}

void CardGameManager::moveCardToHand(CardData& card)
{
    card.removed = true;
    card.zone = CardZone::Hand;
    _handCardIds.push_back(card.id);
    _handTopCardId = card.id;
}
