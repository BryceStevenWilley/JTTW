#include "Character.hpp"
#include "Collisions.hpp"
#include "MoveablePlatform.hpp"
#include "Monk.hpp"
#include "Monkey.hpp"
#include "Piggy.hpp"
#include "Sandy.hpp"
#include <iostream>
#include "SimpleAudioEngine.h"

using namespace JTTW;

const double Character::JUMP_DECAY = ideal2Res(200);
const double Character::VEL_LIMIT = ideal2Res(800);
const double Character::JUMP_INIT_FRACTION = (7.0 / 12.0);
const double Character::CROWN_SCALE = screenScale * .3;
const std::array<cocos2d::Vec2, 9> Character::COL_POINTS = {
    cocos2d::Vec2(-228, 260 + 86.666),
    cocos2d::Vec2(228, 260 + 86.666),
    cocos2d::Vec2(228, - 260 + 86.666),
    cocos2d::Vec2(197.45379, -260 + 86.666 - 114),
    cocos2d::Vec2(114, -260 + 86.666 - 197.45379),
    cocos2d::Vec2(0.0, -260 + 86.666 - 228),
    cocos2d::Vec2(-114, -260 + 86.666 - 197.45379),
    cocos2d::Vec2(-197.45379, -260 + 86.666 - 114),
    cocos2d::Vec2(-228, -260 + 86.666)
};

Character * Character::createFromName(const std::string name, cocos2d::Vec2 startPosition, cocos2d::Size dimensions) {
    std::cout << "Jump Decay: " << JUMP_DECAY << std::endl;
    if (name == "Monkey") {
        return (Character *)new Monkey(startPosition, dimensions);
    } else if (name == "Monk") {
        return (Character *)new Monk(startPosition, dimensions);
    } else if (name == "Piggy") {
        return (Character *)new Piggy(startPosition, dimensions);
    } else { // (name == "Sandy"), ALSO DEFAULT
        return (Character *)new Sandy(startPosition, dimensions);
    }
}

Character::Character(const std::string artFilePrefix, cocos2d::PhysicsMaterial mat, cocos2d::Vec2 startPosition, cocos2d::Size dimensions, double impulseScale) :
        spine::SkeletonAnimation(),
        body(cocos2d::PhysicsBody::create()),
        characterName(artFilePrefix), _currentState(MID_AIR), _dimensions(dimensions), _impulseScale(impulseScale) {
    
    // PREVIOUS WORKING BUT BUGGY BODY COLLISION.
    /*
    double width = 480.0f;
    double height = 780.0f;
    auto bodyShape = cocos2d::PhysicsShapeBox::create(cocos2d::Size(width * .95, height * (2.0/3.0)), mat, cocos2d::Vec2(0.0, height/9.0));
    auto bottomSemiCircle = cocos2d::PhysicsShapeCircle::create(width * (.97/2.0), mat, cocos2d::Vec2(0.0, -height * (0.8/4.0)));
    body->addShape(bodyShape);
    body->addShape(bottomSemiCircle);
    */
    
    auto bodyShape = cocos2d::PhysicsShapePolygon::create(COL_POINTS.data(), COL_POINTS.size(), mat);
    body->addShape(bodyShape);
    
    body->setCategoryBitmask((int)CollisionCategory::Character);
    body->setCollisionBitmask((int)CollisionCategory::PlatformAndBoulder);
    body->setContactTestBitmask((int)CollisionCategory::ALL);
    body->setRotationEnable(false);
    body->setDynamic(true);

    body->setVelocityLimit(VEL_LIMIT);
    
    this->initWithJsonFile("characters/" + artFilePrefix + ".json", "characters/" + artFilePrefix + ".atlas", 1.0f);
    this->setContentSize(cocos2d::Size(/*480.0f*/0.0, 780.0f));
    this->setScaleX(dimensions.width / 780.0f);
    this->setScaleY(dimensions.height / 780.0f);
    this->setAnimation(0, "idle", true);
    this->setPhysicsBody(body);

    _mass = body->getMass();
    std::cout << "_mass: " << _mass << ", body mass: " << body->getMass() << std::endl;

    followcrown = cocos2d::Sprite::create("characters/Selection Crown.png");
    followcrown->setScale(CROWN_SCALE);
    followcrown->setPosition(0.0, 940);
    followcrown->setVisible(false);
    this->addChild(followcrown);
    
    alonecrown = cocos2d::Sprite::create("characters/AloneCrown.png");
    alonecrown->setScale(CROWN_SCALE);
    alonecrown->setPosition(0.0, 940);
    alonecrown->setVisible(false);
    this->addChild(alonecrown);

    currentCrown = alonecrown;

    this->setPosition(startPosition);
    
    _respawnPosition = startPosition;
    
    this->setTag(CHARACTER_TAG);
}

cocos2d::Size Character::getSize() {
    return _dimensions;
}

void Character::impulseLeft(double deltaVel) {
    deltaVel *= _impulseScale;
    double impulse = getMass() * deltaVel;
    leftMomentum += impulse;
    if (getCurrentState() == STANDING ||
        getCurrentState() == MID_AIR) {
        rebalanceImpulse();
    } else if (getCurrentState() == QUICKSANDED) {
        double totalMomentum = rightMomentum - leftMomentum;
        if (totalMomentum == 0.0  && !aiControl) {
            body->setVelocity(cocos2d::Vec2(0.0, _q->_recoverVel));
        } else if (!aiControl) {
            double targetVelocity = totalMomentum / getMass() / 3.0;
            body->setVelocity(cocos2d::Vec2(targetVelocity, -_q->_sinkVel));
        }  else {
           body->setVelocity(cocos2d::Vec2(0, -_q->_sinkVel));
        }
    }
}

void Character::impulseRight(double deltaVel) {
    deltaVel *= _impulseScale;
    double impulse = getMass() * deltaVel;
    rightMomentum += impulse;
    if (getCurrentState() == State::STANDING ||
        getCurrentState() == State::MID_AIR) {
        rebalanceImpulse();
    }  else if (getCurrentState() == State::QUICKSANDED) {
        double totalMomentum = rightMomentum - leftMomentum;
        if (totalMomentum == 0.0  && !aiControl) {
            body->setVelocity(cocos2d::Vec2(0.0, _q->_recoverVel));
        } else if (!aiControl) {
            double targetVelocity = totalMomentum / getMass() / 3.0;

            body->setVelocity(cocos2d::Vec2(targetVelocity, -_q->_sinkVel));
        } else {
           body->setVelocity(cocos2d::Vec2(0, -_q->_sinkVel));
        }
    }
}

void Character::impulseLeftButNoRebalance(double deltaVel) {
    deltaVel *= _impulseScale;
    double impulse = getMass() * deltaVel;
    leftMomentum += impulse;
}

void Character::impulseRightButNoRebalance(double deltaVel) {
    deltaVel *= _impulseScale;
    double impulse = getMass() * deltaVel;
    rightMomentum += impulse;
}

void Character::applyForceRight(double fprime_x) {
    cocos2d::Vec2 F_x = cocos2d::Vec2(fprime_x * getMass(), 0);

    // Project the force onto the right direction vector to avoid 
    // bumping into objects as much as possible.
    if (getCurrentState() == State::STANDING ||
        getCurrentState() == State::MID_AIR) {
        body->applyForce(F_x.project(_rightVector));
    } else if (getCurrentState() == State::QUICKSANDED){
        if (fprime_x < 1.0 && !aiControl) {
           body->setVelocity(cocos2d::Vec2(0.0, _q->_recoverVel));
        } else if (!aiControl) {
           body->setVelocity(cocos2d::Vec2(fprime_x * getMass() / 3.0, -_q->_sinkVel));
        } else {
           body->setVelocity(cocos2d::Vec2(0, -_q->_sinkVel));
        }
    }
    // TODO: update the animation properly!
}

void Character::rebalanceImpulse() {
    double totalMomentum = rightMomentum - leftMomentum;
    double targetVelocity = totalMomentum / getMass();
    
    double actualDeltaVel = targetVelocity - body->getVelocity().x;
    
    double actualImpulse = getMass() * actualDeltaVel;
    
    auto finalVec = cocos2d::Vec2(actualImpulse, 0);
    body->applyImpulse(finalVec.project(_rightVector));
}

void Character::continueMotion() {
    if ((getCurrentState() == State::STANDING ||
            getCurrentState() == State::MID_AIR) &&
            //platformsStandingOn != 0 &&
            wallsHit == 0 && 
            std::abs(rightMomentum - leftMomentum)/getMass() > 0.01) {
        rebalanceImpulse();
    } else if (getCurrentState() == State::QUICKSANDED) {
        double totalMomentum = rightMomentum - leftMomentum;
        if (totalMomentum == 0.0 && !aiControl) {
            body->setVelocity(cocos2d::Vec2(0.0, _q->_recoverVel));
        } else if (!aiControl) {
            double targetVelocity = totalMomentum / getMass() / 3.0;
            body->setVelocity(cocos2d::Vec2(targetVelocity, -_q->_sinkVel));
        } else {
            body->setVelocity(cocos2d::Vec2(0, -_q->_sinkVel));
        }
    }
}

void Character::stop() {
    leftMomentum = 0.0;
    rightMomentum = 0.0;
    auto finalVec = getMass() * -(body->getVelocity().project(_rightVector));
    body->applyImpulse(finalVec);
}

void Character::die(CauseOfDeath cause) {
    if (getCurrentState() == DEAD) {
        // ignore.
        return;
    }

    // Handle death timer.
    if (cause == CUTSCENE) {
        _deathTimer = 1.0;
        _currentState = DEAD; // TODOTODOTODOOTODOTOTDO: very bad, only doing it to avoid the animation of falling down.
        return; // don't play sound.
    }

    // TODO: separate this sound effect to the multiple ways of dying.
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Sound/player_death.wav");
    if (cause == PROJECTILE) {
        _deathTimer = 1.5;
    } else if (cause == FALL) {
        _deathTimer = 0.0; // Respawn right away.
    } else if (cause == CRUSHED) {
        _deathTimer = 1.0;
    }
    setCurrentState(DEAD);
}

void Character::setCurrentState(State newState) {
    if (newState == getCurrentState()) {
        // Is there anything to do?
        return;
    }

    std::cout << characterName << " leaving state " << getCurrentState() << " and entering state " << newState << std::endl;

    // Clean up old states.
    switch (getCurrentState()) {
        case QUICKSANDED:
            if (newState == STUCK_SAND) {
                // ignore these changes.
                break;
            }
        case STUCK_SAND:
            sunkInQuicksand = 0.0;
            this->body->setGravityEnable(true);
            body->setCollisionBitmask((int)CollisionCategory::PlatformAndBoulder);
            body->setContactTestBitmask((int)CollisionCategory::ALL);
            _q = nullptr;
            body->setVelocity(cocos2d::Vec2::ZERO);
            break;
            
        case HANGING:
            if (currentAttached != nullptr) {
                ((Monkey *)currentAttached)->setHangingCharacter(nullptr);
            }
            currentAttached = nullptr;
            currentAttachedOffset = cocos2d::Vec2::ZERO;
            body->setRotationEnable(false);
            body->setAngularVelocity(0.0);
            this->setRotation(0.0);
            rebalanceImpulse();
            break;
            
        //case DEAD:
    }

    switch (newState) {
        case STANDING:
            this->setTimeScale(1.0);
            if (body->getVelocity().x > 10.0 || body->getVelocity().x < -10.0) {
                // TODO: Set walk or run depending on the speed (interpolate?)
                this->setAnimation(0, "walk", true);
            } else { // x == 0.0
                this->setAnimation(0, "idle", true);
            }
            break;
    
        case MID_AIR:
            _rightVector = cocos2d::Vec2(1, 0);
            if (getCurrentState() == STANDING || getCurrentState() == HANGING) {
                // Slow it down so it lasts the whole time.
                this->setAnimation(0, "jump", false);
                this->setTimeScale(0.9);
            } else if (getCurrentState() == State::FROZEN || getCurrentState() == State::DEAD) {
                this->setAnimation(0, "idle", true);
                this->setTimeScale(1.0);
            }
            
            break;
            
        case FROZEN:
            this->setAnimation(0, "Freeze", false);
            body->setVelocity(cocos2d::Vec2(0.0, 0.0));
            // TODO
            break;
            
        case HANGING:
            // TODO
            break;
            
        case DEAD:
            // TODO
            this->setAnimation(0, "fall", false);
            break;
        
        case QUICKSANDED:
            this->body->setGravityEnable(false);
            body->setVelocity(cocos2d::Vec2(0.0, 0.0));
            sunkInQuicksand = 0.0;
            break;
            
        case STUCK_SAND:
            body->setVelocity(cocos2d::Vec2(0.0, 0.0));
            this->setAnimation(0, "idle", false);
            break;
            
        default:
            std::cerr << characterName << " switched to state " << newState << ". ADD A CASE FOR IT." << std::endl;
            break;
            
    }
   _currentState = newState;
}

void Character::freeze(double duration) {
    if (getCurrentState() == DEAD) {
        // Ignore.
        return;
    }
    setCurrentState(FROZEN);
    _frozenTimer = duration; // seconds
}

// While the key is pressed, apply a force (that is decaying) under a jump time
// amount.
void Character::initJump(double force) {
    if (getCurrentState() != State::STANDING && getCurrentState() != State::HANGING) {
        // Don't let them jump.
        return;
    }
    if (getCurrentState() == State::HANGING) {
        leavingHanging(cocos2d::Vec2::ZERO);
        // jump in direction of motion? copy monkey code.
    }
    
    jumpForce = force * getMass();
    
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("Sound/Jump.wav");
    body->applyImpulse(cocos2d::Vec2(0, jumpForce * JUMP_INIT_FRACTION));
}

void Character::stopJump() {
    jumpForce = 0.0;
}

/*void Character::jumpFromForce(double fprime_y) {
    if (getCurrentState() != State::STANDING &&
        getCurrentState() != State::HANGING) {
        // Don't let them jump.
        return;
    }

    body->applyForce(cocos2d::Vec2(0.0, fprime_y * getMass()));
} */

void Character::transferVelocity(Character *reciever) {
    std::cout << characterName << " is giving " << body->getVelocity().x << " x vel to " << reciever->characterName << std::endl;
    double totalMomentum = rightMomentum - leftMomentum;
    double targetVelocity = totalMomentum / getMass();
    
    reciever->impulseRight(targetVelocity);
    this->stop();
}
    
bool Character::justJumped() const {
    return body->getVelocity().y > 100;
}

void Character::landedCallback(cocos2d::PhysicsBody *plat, cocos2d::Vec2 newRightDir) {
    platformsStandingOn += 1;
    
    // TODO: check if we don't need to do this for plats > 2
    _rightVector = newRightDir;
    
    if (getCurrentState() == HANGING) {
        leavingHanging(cocos2d::Vec2::ZERO);
    }
    
    if (getCurrentState() == STANDING ||
        getCurrentState() == MID_AIR ||
        getCurrentState() == HANGING) {
        setCurrentState(STANDING);
    }
}

void Character::leftCallback(cocos2d::PhysicsBody *plat) {
    platformsStandingOn -= 1;

    if (platformsStandingOn == 0 &&
        getCurrentState() != HANGING &&
        getCurrentState() != DEAD &&
        getCurrentState() != QUICKSANDED) {
        std::cout << "Mid air from left callback" << std::endl;
        setCurrentState(MID_AIR);
    } else if (platformsStandingOn < 0) {
        std::cerr << "ERROR: how can " << characterName << " stand on negative platforms?!" << std::endl;
        platformsStandingOn = 0;
    }
}

void Character::wallHitCallback(cocos2d::PhysicsBody *wall) {
    wallsHit += 1;
}

void Character::wallLeftCallback(cocos2d::PhysicsBody *wall) {
    wallsHit -= 1;
    if (wallsHit < 0) {
        std::cerr << "ERROR: can't be in contact with negative walls, " << characterName << std::endl;
        wallsHit = 0;
    }
}

bool Character::isMovingLeft() const {
    return body->getVelocity().x < 0.0;
}

bool Character::isMovingRight() const {
    return body->getVelocity().x > 0.0;
}

float Character::getMass() {
    if (_mass > 50000) {
        updateMass();
    }
    return _mass;
}

const Character::State Character::getCurrentState() const {
    return _currentState;
}

void Character::updateLoop(float delta) {
    // Kinda unrelated stuff regarding frozen.
    if (getCurrentState() == State::FROZEN) {
        _frozenTimer -= delta;
        if (_frozenTimer <= 0.0) {
            std::cout << "Un-freezing" << std::endl;
            if (platformsStandingOn > 0) {
                setCurrentState(STANDING);
            } else {
                setCurrentState(MID_AIR);
            }
        }
    }
 
    if (getCurrentState() == DEAD) {
        _deathTimer -= delta;
        if (_deathTimer <= 0.0) {
            std::cout << "Respawning " << characterName << std::endl;
            restartFromRespawn();
        }
    }
 
    // Apply jump force.
    if (jumpForce > 0.0) {
        body->applyForce(cocos2d::Vec2(0.0, jumpForce));
        jumpForce = jumpForce - (JUMP_DECAY * getMass()) * delta;
    } else {
        jumpForce = 0.0;
    }
    
    cocos2d::Vec2 currentRelVel;
    
    if (getCurrentState() == State::FROZEN ||
        getCurrentState() == State::DEAD ||
        getCurrentState() == State::STUCK_SAND) {
        return; // don't change facing direction.
    }
    if (aiControl) {
        auto bodyX = body->getVelocity().x;
        if (bodyX < ideal2Res(20) && bodyX > -ideal2Res(20)) {
            bodyX = 0.0;
        } else {
            bodyX = bodyX / std::abs(bodyX) * 100;
        }
        currentRelVel = cocos2d::Vec2(bodyX, body->getVelocity().y);
        // TODO: WILL SCREW UP WHEN
        // STANDING ON MOVING PLATFORMS, PUT REAL REL VEL STUFF BACK IN!!!!!!
    } else { // player control
        currentRelVel = cocos2d::Vec2((rightMomentum - leftMomentum)/ getMass(), body->getVelocity().y);
    }
    if (getCurrentState() == QUICKSANDED && currentRelVel.y == _oldVel.y) {
        if (_oldVel.y == -_q->_sinkVel) {
            // For at least delta time, we've been sinking at the sinking rate.
            sunkInQuicksand += _q->_sinkVel * delta;
        } else if (_oldVel.y == _q->_recoverVel) {
            sunkInQuicksand -= _q->_recoverVel * delta;
        }
    }
    
    if (sunkInQuicksand > 50) {
        setCurrentState(STUCK_SAND);
    }
    
    if (_oldVel == currentRelVel) {
        // nothing changed.
        return;
    }
    if (getCurrentState() == STANDING) {
        if (std::abs(_oldVel.x) >= 5 && std::abs(currentRelVel.x) < 5) {
            // Slowing down.
            this->setAnimation(0, "idle", true);
        }
        if (std::abs(_oldVel.x) < 5 && std::abs(currentRelVel.x) >= 5) {
            // Speeding up.
            this->setAnimation(0, "walk", true);
        }
    }

    //  Left vs. Right
    if (_oldVel.x <= 0.0 && currentRelVel.x > 0.0) {
      // Set to go right.
      this->setScaleX(std::abs(this->getScaleX()));
    }
    if (_oldVel.x >= 0.0 && currentRelVel.x < 0.0) {
      this->setScaleX(-1 * std::abs(this->getScaleX()));
    }
    
    _oldVel = currentRelVel;
}


void Character::restartFromRespawn() {
    std::cout << characterName << " is respawning " << _respawnPosition.x << std::endl;
    body->setVelocity(cocos2d::Vec2(0, 0));
    body->resetForces();
    this->setPosition(_respawnPosition);
    setCurrentState(MID_AIR);
}

void Character::setNewRespawn(cocos2d::Vec2 newRespawn) {
    _respawnPosition = newRespawn;
}

double Character::getRespawnProgress() const {
    return _respawnPosition.x;
}

void Character::toggleToAI() {
    aiControl = true;
}

void Character::toggleToPlayer() {
    aiControl = false;
}

/**
 * 'alreadyOn' is only to change the animation.
 */
void Character::enteringHanging(cocos2d::PhysicsWorld *world, Character *m, cocos2d::Vec2 offsetVec, bool alreadyOn) {
    if (getCurrentState() == DEAD) {
        // Ignore
        return;
    }
    removeFromHanging();
    offsetVec.set(0.0, offsetVec.y * .8);
    // create a joint between you and Monkey.
    pinJoint = cocos2d::PhysicsJointPin::construct(this->body, m->body, cocos2d::Vec2::ZERO, offsetVec);
    world->addJoint(pinJoint);
    setCurrentState(HANGING);
    
    if (!alreadyOn) {
        this->setTimeScale(2.0);
        this->setAnimation(0, "jumptoswing", false);
    } else {
        //this->setTimeScale(2.0);
        //this->setTimeScale(1.0);
        //this->setAnimation(0, "Climb", false);
        //this->addAnimation(0, "Swing", true);
    }
    
    // set the character to rotate with the vine.
    body->setRotationEnable(true);
    body->setRotationOffset(0.0);
    double gearPhase = 0.0;
    double gearRatio = 1.0;
    gearJoint = cocos2d::PhysicsJointGear::construct(body, m->body, gearPhase, gearRatio);
    world->addJoint(gearJoint);

    // set these params so you can climb later.
    currentAttached = m;
    currentAttachedOffset = offsetVec;
    currentWorld = world;
}

void Character::leavingHanging(cocos2d::Vec2 vel) {
    removeFromHanging();
    body->setVelocity(vel);
    setCurrentState(MID_AIR);
}

void Character::removeFromHanging() {
    if (pinJoint != nullptr) {
        pinJoint->removeFormWorld();
        pinJoint = nullptr;
    }

    if (gearJoint != nullptr) {
        gearJoint->removeFormWorld();
        gearJoint = nullptr;
    }
}

void Character::callHey() {
    this->setAnimation(0, "wave", false);
    this->addAnimation(0, "idle", true);
}

void Character::landedInQuicksand(Quicksand *q) {
    if (getCurrentState() != HANGING && getCurrentState() != DEAD) {
        std::cout << characterName << " entering quicksand." << std::endl;
        _q = q;
        setCurrentState(QUICKSANDED);
    }
}

void Character::leftQuicksand() {
    if (getCurrentState() == QUICKSANDED || getCurrentState() == STUCK_SAND) {
        setCurrentState(STANDING);
        return;
    }
    std::cout << characterName << " ignoring leaving quicksand with state " << _currentState << std::endl;
    // Happening because you're hanging and swung through some quicksand. Ignore.
}

bool Character::shouldBeControlled() {
    return getCurrentState() != HANGING ||
           getCurrentState() != DEAD ||
           getCurrentState() != FROZEN ||
           getCurrentState() != STUCK_SAND;
}

void Character::updateMass() {
    _mass = body->getMass();
}
