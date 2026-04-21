#include "HelloWorldScene.h"

#include <algorithm>

USING_NS_CC;

namespace {
const float W = 1080.0f;
const float H = 2080.0f;
const float T = 1500.0f;
const float D = 0.18f;
const char* FONT = "fonts/Marker Felt.ttf";
}

Scene* HelloWorld::scene() {
    return HelloWorld::create();
}

bool HelloWorld::init() {
    if (!Scene::init()) {
        return false;
    }

    _tableRoot = Node::create();
    addChild(_tableRoot, 1);
    _stockRoot = Node::create();
    addChild(_stockRoot, 2);
    _handRoot = Node::create();
    addChild(_handRoot, 3);

    auto top = LayerColor::create(Color4B(180, 131, 76, 255), (int)W, (int)T);
    top->setIgnoreAnchorPointForPosition(false);
    top->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    addChild(top);

    auto bottom = LayerColor::create(Color4B(154, 42, 160, 255), (int)W, (int)(H - T));
    bottom->setIgnoreAnchorPointForPosition(false);
    bottom->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    bottom->setPosition(Vec2(0, T));
    addChild(bottom);

    auto frame = DrawNode::create();
    frame->drawRect(Vec2::ZERO, Vec2(W, H), Color4F(0.65f, 0.65f, 0.65f, 1));
    addChild(frame, 10);

    auto title = Label::createWithTTF("Card Demo", FONT, 42);
    title->setAnchorPoint(Vec2(0.5f, 1));
    title->setPosition(Vec2(W * 0.5f, H - 20));
    addChild(title, 5);

    _statusLabel = Label::createWithTTF("Tap reserve pile or a top card.", FONT, 34);
    _statusLabel->setAnchorPoint(Vec2(0, 0.5f));
    _statusLabel->setPosition(Vec2(50, T + 50));
    addChild(_statusLabel, 5);

    _handTitleLabel = Label::createWithTTF("Bottom", FONT, 34);
    _handTitleLabel->setAnchorPoint(Vec2(0, 0.5f));
    _handTitleLabel->setPosition(Vec2(650, T + 50));
    addChild(_handTitleLabel, 5);

    _undoButton = MenuItemLabel::create(
        Label::createWithTTF("Undo", FONT, 40),
        [this](Ref*) { undoLastMove(); });

    auto menu = Menu::create(_undoButton, nullptr);
    menu->setPosition(Vec2(W - 120, T + 60));
    addChild(menu, 5);

    auto reserveListener = EventListenerTouchOneByOne::create();
    reserveListener->setSwallowTouches(true);
    reserveListener->onTouchBegan = [this](Touch* touch, Event*) {
        Rect reserveRect(70.0f, 180.0f, 180.0f, 220.0f);
        return _gameManager.canDrawFromStock() && reserveRect.containsPoint(touch->getLocation());
    };
    reserveListener->onTouchEnded = [this](Touch*, Event*) { drawFromStock(); };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(reserveListener, this);

    _gameManager.setupDemo();
    rebuildAllCards();
    refreshStatusText("Try 3 -> 4 -> 5 -> 4 -> 3 -> 2.");
    return true;
}

Node* HelloWorld::createCardNode(int cardId) {
    const CardData* card = _gameManager.getCard(cardId);
    if (!card) {
        return nullptr;
    }

    auto root = Node::create();
    auto bg = Sprite::create("card_general.png");
    bg->setName("bg");
    root->setContentSize(bg->getContentSize());
    bg->setPosition(root->getContentSize() * 0.5f);
    root->addChild(bg);

    auto back = LayerColor::create(
        Color4B(62, 103, 188, 220),
        (int)root->getContentSize().width - 14,
        (int)root->getContentSize().height - 14);
    back->setName("backMask");
    back->setIgnoreAnchorPointForPosition(false);
    back->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    back->setPosition(Vec2(7, 7));
    root->addChild(back, 1);

    auto backLabel = Label::createWithTTF("CARD", FONT, 28);
    backLabel->setName("backLabel");
    backLabel->setPosition(root->getContentSize() * 0.5f);
    root->addChild(backLabel, 2);

    auto big = Sprite::create(getBigNumberFile(*card));
    big->setName("bigNumber");
    big->setPosition(Vec2(root->getContentSize().width * 0.57f, root->getContentSize().height * 0.5f));
    root->addChild(big, 3);

    auto smallNumberSprite = Sprite::create(getSmallNumberFile(*card));
    smallNumberSprite->setName("smallNumber");
    smallNumberSprite->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
    smallNumberSprite->setPosition(Vec2(12, root->getContentSize().height - 12));
    root->addChild(smallNumberSprite, 3);

    auto suit = Sprite::create(getSuitFile(card->suit));
    suit->setName("suit");
    suit->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
    suit->setPosition(Vec2(16, root->getContentSize().height - 60));
    root->addChild(suit, 3);

    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this, root, cardId](Touch* touch, Event*) {
        if (!root->isVisible()) {
            return false;
        }
        const CardData* cardData = _gameManager.getCard(cardId);
        if (!cardData || cardData->zone != CardZone::Table) {
            return false;
        }
        return getCardTouchRect(root).containsPoint(touch->getLocation());
    };
    listener->onTouchEnded = [this, cardId](Touch*, Event*) { tryMoveTableCard(cardId); };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, root);
    return root;
}

void HelloWorld::rebuildAllCards() {
    _cardNodes.clear();
    _tableRoot->removeAllChildren();
    _stockRoot->removeAllChildren();
    _handRoot->removeAllChildren();

    for (int id : _gameManager.getTableCardIds()) {
        auto node = createCardNode(id);
        if (node) {
            _cardNodes[id] = node;
        }
    }
    for (int id : _gameManager.getStockCardIds()) {
        auto node = createCardNode(id);
        if (node) {
            _cardNodes[id] = node;
        }
    }
    for (int id : _gameManager.getHandCardIds()) {
        if (_cardNodes.find(id) == _cardNodes.end()) {
            auto node = createCardNode(id);
            if (node) {
                _cardNodes[id] = node;
            }
        }
    }

    for (const auto& pair : _cardNodes) {
        refreshCardVisual(pair.first);
    }
    refreshStockPile();
    refreshHandPile();
}
void HelloWorld::refreshCardVisual(int cardId) {
    auto nodeIt = _cardNodes.find(cardId);
    if (nodeIt == _cardNodes.end()) {
        return;
    }

    auto node = nodeIt->second;
    const CardData* card = _gameManager.getCard(cardId);
    if (!card) {
        node->setVisible(false);
        return;
    }

    bool inHand = std::find(
                      _gameManager.getHandCardIds().begin(),
                      _gameManager.getHandCardIds().end(),
                      cardId) != _gameManager.getHandCardIds().end();
    bool onTable = card->zone == CardZone::Table && !card->removed;
    bool onStock = card->zone == CardZone::Stock && !card->removed;
    Node* parent = inHand ? _handRoot : (onTable ? _tableRoot : (onStock ? _stockRoot : nullptr));

    if (!parent) {
        node->setVisible(false);
        return;
    }

    node->setVisible(true);
    if (node->getParent() != parent) {
        node->retain();
        if (node->getParent()) {
            node->removeFromParent();
        }
        parent->addChild(node);
        node->release();
    }

    auto back = dynamic_cast<LayerColor*>(node->getChildByName("backMask"));
    auto backLabel = dynamic_cast<Label*>(node->getChildByName("backLabel"));
    auto bigNumberSprite = dynamic_cast<Sprite*>(node->getChildByName("bigNumber"));
    auto smallNumberSprite = dynamic_cast<Sprite*>(node->getChildByName("smallNumber"));
    auto suitSprite = dynamic_cast<Sprite*>(node->getChildByName("suit"));
    bool face = inHand || onTable || card->zone == CardZone::Hand;

    if (back) back->setVisible(!face);
    if (backLabel) backLabel->setVisible(!face);
    if (bigNumberSprite) bigNumberSprite->setVisible(face);
    if (smallNumberSprite) smallNumberSprite->setVisible(face);
    if (suitSprite) suitSprite->setVisible(face);

    if (onTable) {
        node->setPosition(getTableCardPosition(*card));
        node->setLocalZOrder(card->faceUp ? 100 + card->row : 10 + card->row);
        node->setScale(1.0f);
        node->setOpacity(card->faceUp ? 255 : 230);
    } else if (onStock) {
        node->setScale(0.95f);
        node->setOpacity(255);
    }
}

void HelloWorld::refreshHandPile() {
    const std::vector<int>& ids = _gameManager.getHandCardIds();
    for (size_t i = 0; i < ids.size(); ++i) {
        auto it = _cardNodes.find(ids[i]);
        if (it == _cardNodes.end()) {
            continue;
        }

        refreshCardVisual(ids[i]);

        auto node = it->second;
        if (node->getParent() != _handRoot) {
            node->retain();
            if (node->getParent()) {
                node->removeFromParent();
            }
            _handRoot->addChild(node);
            node->release();
        }

        bool isTop = i + 1 == ids.size();
        node->setVisible(isTop);
        node->stopAllActions();
        node->runAction(MoveTo::create(D, getHandCardPosition((int)i)));
        node->setLocalZOrder(100 + (int)i);
        node->setScale(1.0f);
        node->setOpacity(isTop ? 255 : 0);
    }

    if (_handTitleLabel) {
        _handTitleLabel->setString("Bottom");
    }
    if (_undoButton) {
        bool enabled = _gameManager.canUndo();
        _undoButton->setEnabled(enabled);
        _undoButton->setColor(enabled ? Color3B::WHITE : Color3B(150, 150, 150));
    }
}

void HelloWorld::refreshStockPile() {
    int remainingCount = _gameManager.getRemainingStockCount();
    int index = 0;

    for (int cardId : _gameManager.getStockCardIds()) {
        auto it = _cardNodes.find(cardId);
        if (it == _cardNodes.end()) {
            continue;
        }

        auto node = it->second;
        const CardData* card = _gameManager.getCard(cardId);
        if (!card || card->removed || card->zone != CardZone::Stock) {
            node->setVisible(false);
            continue;
        }

        if (node->getParent() != _stockRoot) {
            node->retain();
            if (node->getParent()) {
                node->removeFromParent();
            }
            _stockRoot->addChild(node);
            node->release();
        }

        refreshCardVisual(cardId);
        node->setVisible(true);
        node->stopAllActions();
        node->runAction(MoveTo::create(D, getStockCardPosition(index)));
        node->setLocalZOrder(40 + index);
        ++index;
    }

    if (_statusLabel && remainingCount == 0) {
        _statusLabel->setString("Reserve pile is empty.");
    }
}

void HelloWorld::refreshStatusText(const std::string& text) {
    if (_statusLabel) {
        _statusLabel->setString(text);
    }
}

void HelloWorld::tryMoveTableCard(int cardId) {
    const CardData* card = _gameManager.getCard(cardId);
    if (!card || card->zone != CardZone::Table || card->removed) {
        return;
    }

    if (!_gameManager.canMoveTableCardToHand(cardId)) {
        const CardData* top = _gameManager.getCard(_gameManager.getHandTopCardId());
        if (!_gameManager.canSelectTableCard(cardId)) {
            refreshStatusText("This card is blocked by another card.");
        } else if (top) {
            refreshStatusText(StringUtils::format(
                "%s can only match %s +/- 1.",
                valueToString(card->value).c_str(),
                valueToString(top->value).c_str()));
        }
        return;
    }

    MoveResult result = _gameManager.moveTableCardToHand(cardId);
    if (!result.success) {
        return;
    }

    for (int flippedCardId : result.flippedCardIds) {
        refreshCardVisual(flippedCardId);
    }
    refreshHandPile();
    refreshStockPile();

    const CardData* moved = _gameManager.getCard(_gameManager.getHandTopCardId());
    if (moved) {
        refreshStatusText(StringUtils::format("Moved %s to bottom.", valueToString(moved->value).c_str()));
    }
}

void HelloWorld::drawFromStock() {
    MoveResult result = _gameManager.drawFromStock();
    if (!result.success) {
        refreshStatusText("Reserve pile is empty.");
        return;
    }

    refreshStockPile();
    refreshHandPile();

    const CardData* moved = _gameManager.getCard(_gameManager.getHandTopCardId());
    if (moved) {
        refreshStatusText(StringUtils::format(
            "Flipped %s from reserve pile to bottom.",
            valueToString(moved->value).c_str()));
    }
}

void HelloWorld::undoLastMove() {
    UndoResult result = _gameManager.undo();
    if (!result.success) {
        refreshStatusText("Nothing to undo.");
        return;
    }

    refreshCardVisual(result.movedCardId);
    for (int cardId : result.revertedFaceDownCardIds) {
        refreshCardVisual(cardId);
    }
    refreshHandPile();
    refreshStockPile();
    refreshStatusText("Undo completed.");
}

Vec2 HelloWorld::getTableCardPosition(const CardData& card) const {
    const float x[3] = {170.0f, 470.0f, 770.0f};
    return Vec2(x[card.column], 1240.0f - card.row * 190.0f);
}

Vec2 HelloWorld::getStockCardPosition(int index) const {
    return Vec2(120.0f + index * 10.0f, 290.0f);
}

Vec2 HelloWorld::getHandCardPosition(int index) const {
    CC_UNUSED_PARAM(index);
    return Vec2(690.0f, 290.0f);
}

Rect HelloWorld::getCardTouchRect(Node* node) const {
    Vec2 world = node->getParent()->convertToWorldSpace(node->getPosition());
    Size size = node->getContentSize();
    return Rect(world.x - size.width * 0.5f, world.y - size.height * 0.5f, size.width, size.height);
}

std::string HelloWorld::getSuitFile(CardSuit suit) const {
    switch (suit) {
        case CardSuit::Club:
            return "suits/club.png";
        case CardSuit::Diamond:
            return "suits/diamond.png";
        case CardSuit::Heart:
            return "suits/heart.png";
        default:
            return "suits/spade.png";
    }
}

std::string HelloWorld::getBigNumberFile(const CardData& card) const {
    return StringUtils::format(
        "number/big_%s_%s.png",
        isRedSuit(card.suit) ? "red" : "black",
        valueToString(card.value).c_str());
}

std::string HelloWorld::getSmallNumberFile(const CardData& card) const {
    return StringUtils::format(
        "number/small_%s_%s.png",
        isRedSuit(card.suit) ? "red" : "black",
        valueToString(card.value).c_str());
}

bool HelloWorld::isRedSuit(CardSuit suit) const {
    return suit == CardSuit::Diamond || suit == CardSuit::Heart;
}

std::string HelloWorld::valueToString(int value) const {
    switch (value) {
        case 1:
            return "A";
        case 11:
            return "J";
        case 12:
            return "Q";
        case 13:
            return "K";
        default:
            return StringUtils::format("%d", value);
    }
}
